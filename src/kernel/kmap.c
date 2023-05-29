#include <kmap.h>
#include <kclock.h>
#include <types.h>
#include <mmu.h>
#include <assert.h>
#include <string.h>
#include <memlayout.h>

typedef char byte_t;


size_t pages_count;				// Total number of physical p ages
size_t pages_basemem_count;		// base memory (in pages)

pde_t *kern_page_directory;		// Kernel page directory
struct page_status *pages;
static struct page_status *page_free_list;


#define PAGE_ALLIGNED_ROUND(addr)										\
	(typeof(addr)) ((uint32_t)addr + (PAGE_SIZE - (uint32_t)addr % PAGE_SIZE))

#define IS_PPRESENT(page)												\
	(page & PTE_P)

static int
vram_read(int reg)
{
	unsigned int r1, r2;
	r1 =  mc_read(reg);
	r2 = (mc_read(reg+1) << 8);
	return r1 | r2;
}

static void
get_memory_size(void)
{
	size_t basemem, extmem, ext16mem, totalmem;

	// Use read call to get available memory.
	basemem 	= vram_read(NVRAM_BASELO);
	extmem  	= vram_read(NVRAM_EXTLO);
	ext16mem 	= vram_read(NVRAM_EXT16LO);

	if (ext16mem != 0) {
		totalmem = 16 * 1024 + ext16mem;
	} else if (extmem != 0) {
		totalmem = 1  * 1024 + extmem;
	} else {
		totalmem = basemem;
	}

	pages_basemem_count = basemem / (PAGE_SIZE / 1024);
	pages_count = totalmem / (PAGE_SIZE / 1024);

	cprintf("[Info] Total memory: %u Kb\nAvailable: %u Kb\nExtended: %u Kb\n",
		totalmem, basemem, totalmem - basemem);
	cprintf("[Info] Total pages: %u\nTotal pages Available: %u\n",
		pages_count, pages_basemem_count);
}

static void *
boot_alloc(unsigned int nbytes)
{
	static byte_t *next_free = NULL;
	extern byte_t *end_bss;
	extern byte_t *etext;
	byte_t *alloc_address;

	if (next_free == NULL) {
		// Provide next free address. For that i provided end_bss variable
		// generated by the linker. This variable should point to end of bss in kern
		// Use next_free to not modify end_bss value. The address should be page alligned
		next_free = PAGE_ALLIGNED_ROUND(end_bss);
	}

	alloc_address = next_free;
	next_free = PAGE_ALLIGNED_ROUND(next_free + nbytes);

	if ((virtaddr_t) next_free > (KERNEL_BASE_ADDRESS + PAGE_TABLE_SIZE)) {
		return NULL;
	}

	cprintf("[Info] %u bytes starting from address %p is allocated\n",
		nbytes, alloc_address);
	cprintf("[Debug]: ==- %p %p -==\n", etext, end_bss);

	return (void *)alloc_address;
}

static pde_t *
create_kern_pgdir(void)
{
	// This function will provide address of the first page in kernel page directory
	// This will not alloc the whole page directory insted will alloc first page
	pde_t *pgdir;

	pgdir = (pde_t *) boot_alloc(PAGE_SIZE);
	if (pgdir == NULL) {
		cprintf("[Error] [create_kern_pgdir] Couldn't allocate memory\n");
		return NULL;
	}
	cprintf("DEBUG %p\n", pgdir +0x100000);
	memset(pgdir + 0x100000, 0, PAGE_SIZE);
	cprintf("DEBUG2\n");
	return pgdir;
}

void
init_pages(void)
{
	int i;
	// Mark the physical page 0 indisponible to preserve IDT and BIOS structure
	pages[0].refs = 1;
	pages[0].next_free = NULL;

	// Mark base memory as free
	for (i = 1; i < pages_basemem_count; ++i) {
		pages[i].next_free = page_free_list;
		page_free_list = &pages[i];
		pages[i].refs = 0;
	}

	// mark IO hole (IOPHYSMEM -> EXTPHYSMEM) as in used (it should not be allocated)
	for (; i < ((uint32_t) boot_alloc(0)) / PAGE_SIZE; ++i) {
		pages[i].next_free = NULL;
		pages[i].refs = 1;
	}

	// mark extended memory as free
	for (; i < pages_count; ++i) {
		pages[i].next_free = page_free_list;
		page_free_list = &pages[i];
		pages[i].refs = 0;
	}
}

struct page_status *
page_alloc(int alloc_flags)
{
	struct page_status *ps;
	void *vaddr;

	ps = page_free_list;

	if (ps == NULL) {
		cprintf("[Error]: Not enought memory\n");
		return NULL;
	}

	page_free_list = page_free_list->next_free;
	ps->next_free = NULL;

	if ((alloc_flags & 1) == 1) {
		// Create a 0 filled page
		vaddr = page2kva(ps);
		memset(vaddr, 0 , PAGE_SIZE);
		cprintf("[Info] Page [%d] [%p : %p] Filled with 0\n", (PDX(vaddr) | PTX(vaddr)),
			vaddr, (char *)(vaddr + PAGE_SIZE));
	}

	return ps;
}

// Get specific page table for address virtaddr. If create flag not 0
// than the page that virtaddr should refer is created.
pde_t *
pgdir_walk(pde_t *pgdir, const void *virtaddr, int create)
{
	virtaddr_t vaddr = (virtaddr_t) virtaddr;
	pde_t pde = pgdir[PDX(vaddr)];
	physaddr_t paddr;
	uint32_t perm = PTE_W | PTE_P | PTE_U;
	struct page_status *ps = NULL;

	if (IS_PPRESENT(pde) == 0 && create == 0) {
		cprintf("[Error]: Invalid virtual address %p\n", virtaddr);
		return NULL;
	}

	if (IS_PPRESENT(pde) == 0 && create != 0) {
		ps = page_alloc(1);
		if (ps == NULL) {
			cprintf("[Error]: Couldn't allocate page\n");
			return NULL;
		}
		ps->refs += 1;
		pde = page2pa(ps) | perm;
		pgdir[PDX(vaddr)] = pde;
	}

	paddr = PTE_ADDR(pde);
	pde_t *pagetable_va = KADDR(paddr);
	return &pagetable_va[PTX(vaddr)];
}

/* Map virtual addresses to phys addreses */
static void
map_region(pde_t *pgdir, virtaddr_t virtaddr, size_t size,
	physaddr_t physaddr, unsigned int permission)
{
	int i;
	int pcount = size / PAGE_SIZE;
	pte_t *pte = NULL;
	void *page_virtual_address;


	for (i = 0; i < pcount; ++i) {
		page_virtual_address =  (void *) (virtaddr + i * PAGE_SIZE);
		pte = pgdir_walk(pgdir, page_virtual_address, 1);
		*pte = (physaddr + i * PAGE_SIZE) | permission | PTE_P ;
	}
}

void
initialize_memory(void)
{
	uint32_t cr0; // Use to store control register 0 to set up memory
	uint32_t pages_region_count;
	uint32_t remaining_mem = 0xFFFFFFFF - KERNEL_BASE_ADDRESS;
	// +1 to point exactly to the end of pa adrress

	// Detect installed memory on machine
	get_memory_size();

	kern_page_directory = create_kern_pgdir();
	*(kern_page_directory + PDX(U_VIRTUAL_PAGE_TABLE)) =
		PADDR(kern_page_directory) | PTE_U | PTE_P;

	cprintf("[Info] Kernel page directory allocated\n");

	pages = (struct page_status *)boot_alloc(pages_count * sizeof(struct page_status));
	pages_region_count = (uint32_t) boot_alloc(0) - (uint32_t) pages;
	memset(pages, 0, pages_count);

	cprintf("[Info] Virtual pages created\n");
	init_pages();

	// In order to create a real and functional page directory,
	// I have to create a boot region. First map virtual address
	// starting from U_PAGES to phys addrs starting from PADDR(pages)
	map_region (kern_page_directory, /*start virt*/ U_PAGES, /*size*/PAGE_TABLE_SIZE,
		/*start phys*/PADDR(pages), PTE_U);
	cprintf("[Info] Mapped starting from virt %p and phys %p\n", U_PAGES, PADDR(pages));

	// Now i will use phys addres that bootstack refers to map a kernel stack
	map_region(kern_page_directory, /*start virt*/ KERNEL_STACK_TOP - KERNEL_STACK_SIZE,
		/*size*/ PAGE_TABLE_SIZE, /*start phys*/ PADDR(bootstack), PTE_U);
	cprintf("[Info] Mapped starting from virt %p and phys %p\n",
		KERNEL_STACK_TOP - KERNEL_STACK_SIZE, PADDR(bootstack));

	// Map the rest of phys memory starting from virt KERNEL_BASE_ADDRESS and
	// phys address 0 . [KERNEL_BASE_ADDRESS, 2^32] -> [0, 2^32 - KERNEL_BASE_ADDRESS]
	map_region(kern_page_directory, /*virt*/ KERNEL_BASE_ADDRESS,
		/*size*/ remaining_mem, 0, PTE_W);
	cprintf("[Info] Mapped starting from virt %p and phys %p\n",
		KERNEL_BASE_ADDRESS, 0);


}




