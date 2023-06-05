#ifndef __KMAP_H__
#define __KMAP_H__

#include <types.h>
#include <memlayout.h>
#include <stdio.h>

#define MEM_ERROR_LOOP(msg)									\
{															\
	cprintf("[Error] %s %s %s\n", __FILE__, __LINE__, msg);	\
	while(1);												\
}

#define PAGE_ZERO_FILLED		(1 << 0)


extern char *bootstacktop, *bootstack;

struct page_status {
	struct page_status *next_free;
	size_t refs;
};



extern struct page_status *pages;
extern size_t pages_count;
extern pde_t *kern_page_directory;

// Return phys address from virt address in kernel space
//
static physaddr_t
_paddr(const char *file, int line, void *kva)
{
	physaddr_t pa = (physaddr_t)kva - KERNEL_BASE_ADDRESS;
	// cprintf("[Info]: Conversion from V:%x P:%x\n", (physaddr_t)kva, pa);

	if (pa < 0) {
		cprintf("[Error] [_paddr] Conversion failed. Invalid address %x\n",
			(physaddr_t)kva);
		return -1;
	}

	return pa;
}

static void *
_kaddr(const char *file, int line, physaddr_t pa)
{
	if (PGNUM(pa) >= pages_count) {
		cprintf("[Error] [_kaddr] Invalid phys address %x\n", pa);
		return NULL;
	}

	void *kva = (void *)(pa + KERNEL_BASE_ADDRESS);
	// cprintf("[Info]: Conversion from P:%x V: %p\n", kva);

	return kva;
}

#define PADDR(virtaddr) _paddr(__FILE__, __LINE__, virtaddr)
#define KADDR(physaddr) _kaddr(__FILE__, __LINE__, physaddr)


static physaddr_t
page2pa(struct page_status *pp)
{
	return (pp - pages) << PGSHIFT;
}

static struct page_status*
pa2page(physaddr_t pa)
{
	if (PGNUM(pa) >= pages_count) {
		cprintf("[Error] [pa2page] Invalid phys address\n");
		return NULL;
	}

	return &pages[PGNUM(pa)];
}

static void *
page2kva(struct page_status *ps)
{
	return KADDR(page2pa(ps));
}

// ================== MEMORY FUNCTIONS ======================== //

/* Get the physical layout and initialize memory management */
void				initialize_memory(void);

void				init_pages(struct page_status *);
struct page_status *page_alloc(int flags);
void				free_page(struct page_status *);
int					insert_page(pde_t *page_directory, struct page_status *ps,
						void *virtaddr, int permissions);
void 				remove_page(pde_t *page_directory, void *virtaddr);
struct page_status *search_page(pde_t *page_directory, void *virtaddr,
						pte_t **pte_buffer);

void 				invalidate_tlb(void *va);

pte_t 				*get_page_table(pde_t *pgdir, const void *va, int create);




#endif // __KMAP_H__