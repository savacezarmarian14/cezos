
#include <x86.h>
#include <elf.h>

#define SECTSIZE	512

/* Initialize a pointer to elf header in memory */
const struct Elf *kelf_header = ((struct Elf *) 0x10000);

void readsect(void*, uint32_t);
void readseg(uint32_t, uint32_t, uint32_t);

void 
load_error()
{
	outw(0x8A00, 0x8A00);
	outw(0x8A00, 0x8E00);

	while (1);	/* SPIN -> halt state (SHOULD NOT GET HERE )*/
}

void
kload(void)
{
	struct Proghdr *ph, *eph;

	// read 1st page off disk
	readseg((uint32_t) kelf_header, SECTSIZE*8, 0);

	// is this a valid ELF?
	if (kelf_header->e_magic != ELF_MAGIC)
		load_error();

	// load each program segment (ignores ph flags)
	/* Program header start */
    ph = (struct Proghdr *) ((uint8_t *) kelf_header + kelf_header->e_phoff);
	/* Program header end */
    eph = ph + kelf_header->e_phnum; 
	for (; ph < eph; ph++)
		// p_pa is the load address of this segment (as well
		// as the physical address) 
		readseg(ph->p_pa, ph->p_memsz, ph->p_offset);

	// call the entry point from the ELF header
	// note: does not return!
	((void (*)(void)) (kelf_header->e_entry))();
	 
	load_error(); /* entry should not return */
}

// Read 'count' bytes at 'offset' from kernel into physical address 'pa'.
// Might copy more than asked
void
readseg(uint32_t pa, uint32_t count, uint32_t offset)
{
	uint32_t end_pa;

	end_pa = pa + count;

	// round down to sector boundary
	pa &= ~(SECTSIZE - 1); /* last 20 bits */

	// translate from bytes to sectors, and kernel starts at sector 1
	offset = (offset / SECTSIZE) + 1; /* offset cuz pa should ne pagealligned */
	
	while (pa < end_pa) {
		// Since we haven't enabled paging yet and we're using
		// an identity segment mapping (see boot.S), we can
		// use physical addresses directly.
		readsect((uint8_t*) pa, offset);
		pa += SECTSIZE;
		offset++;
	}
}

void
waitdisk(void)
{
	// wait for disk reaady
	while ((inb(0x1F7) & 0xC0) != 0x40)
		/* do nothing */;
}

void
readsect(void *dst, uint32_t offset)
{
	// wait for disk to be ready
	waitdisk();

	outb(0x1F2, 1);		// count = 1
	outb(0x1F3, offset);
	outb(0x1F4, offset >> 8);
	outb(0x1F5, offset >> 16);
	outb(0x1F6, (offset >> 24) | 0xE0);
	outb(0x1F7, 0x20);	// cmd 0x20 - read sectors

	// wait for disk to be ready
	waitdisk();

	// read a sector
	insl(0x1F0, dst, SECTSIZE/4);
}
