#ifndef __INC_MMU_H__
#define __INC_MMU_H__

/**
 * In this file i wrote definitions that should work like those in x86 for 
 * memory management unit [MMU].
 */

// A linear address (Virtual address) 'la' has a three-part structure as follows:
//
// +--------10------+-------10-------+---------12----------+
// | Page Directory |   Page Table   | Offset within Page  |
// |      Index     |      Index     |                     |
// +----------------+----------------+---------------------+
//  \--- PDX(la) --/ \--- PTX(la) --/ \---- PGOFF(la) ----/
//  \---------- PGNUM(la) ----------/
//


#define PTXSHIFT    0xC     // 12
#define PDXSHIFT    0x16    // 22
#define PGSHIFT     0xC     // 12
#define PTSHIFT     0x16    // 22


/* Page Directory Index*/
#define PDX(la)     ((((virtaddr_t) (la)) >> PDXSHIFT) & 0x3FF) /* 10 of 1 in bits = 0x3FF */
/* Page Table Index */ 
#define PTX(la)     ((((virtaddr_t) (la)) >> PTXSHIFT) & 0x3FF) 
/* Page Number filed off address */
#define PGNUM(la)   ((((virtaddr_t) (la)) >> PTXSHIFT) & 0xFFFFF) /* 20 if 1 in bits = 0xFFFFF */
/* Page offset */
#define PGOFF(la)   (((virtaddr_t) (la)) & 0xFFF) /* 12 of 1 in bits = 0xFFF */


/* Construct liner address from pdx ptx and off */
#define PGADDR(_pdx, _ptx, _off) \
    ((void *) ((_pdx) << PDXSHIFT | (_ptx) << PTXSHIFT | (off)))

#define NUMBER_PDENTRIES    1024    // 1024 Page Tables
#define NUMBER_PTENTRIES    1024    // 1024 Pages

#define PAGE_SIZE           4096
#define PAGE_TABLE_SIZE     (PAGE_SIZE * NUMBER_PTENTRIES)

/* 
 * https://wiki.osdev.org/Paging     
 *
 * Page table/directory entry flags.
 */

#define PTE_P		0x001	// Present         b0000 0000 0001  
#define PTE_W		0x002	// Writeable       b0000 0000 0010
#define PTE_U		0x004	// User            b0000 0000 0100
#define PTE_PWT		0x008	// Write-Through   b0000 0000 1000
#define PTE_PCD		0x010	// Cache-Disable   b0000 0001 0000
#define PTE_A		0x020	// Accessed        b0000 0010 0000
#define PTE_D		0x040	// Dirty           b0000 0100 0000
#define PTE_PS		0x080	// Page Size       b0000 1000 1000
#define PTE_G		0x100	// Global          b0001 0000 0000

// The PTE_AVAIL bits aren't used by the kernel or interpreted by the
// hardware, so user processes are allowed to set them arbitrarily.
#define PTE_AVAIL	0xE00	// Available for software use

/* PTE Flags for SYSCALL */
#define PTE_SYSCALL (PTE_AVAIL | PTE_P | PTE_W | PTE_U) //  b1110 0000 0111

/* Address in page table / page directory */
#define PTE_ADDR(_pte)      ((physaddr_t) (_pte) & 0xFFFFF000) // last 20 bits / 32

/* 
 * Defines for Control Registers CRx 
 *
 * https://en.wikipedia.org/wiki/Control_register 
 */

/* 
 * Defines for CR0 
 * -------------------------------------------------------------------------------------------------
 * The CR0 register is 32 bits long on the 386 and higher processors. 
 * On x64 processors in long mode, it (and the other control registers) is 64 bits long. 
 * CR0 has various control flags that modify the basic operation of the processor. 
 * Register CR0 is the 32 Bit version of the old Machine Status Word (MSW) register. 
 * The MSW register was expanded to the Control Register with the appearance of the i386 processor.  
 * -------------------------------------------------------------------------------------------------
 */
#define CR0_PE		0x00000001	// Protected mode Enable
#define CR0_MP		0x00000002	// Monitor coProcessor
#define CR0_EM		0x00000004	// Emulation
#define CR0_TS		0x00000008	// Task Switched
#define CR0_ET		0x00000010	// Extension Type
#define CR0_NE		0x00000020	// Numeric Errror
#define CR0_WP		0x00010000	// Write Protect
#define CR0_AM		0x00040000	// Alignment Mask
#define CR0_NW		0x20000000	// Not Writethrough
#define CR0_CD		0x40000000	// Cache Disable
#define CR0_PG		0x80000000	// Paging

/*
 * Defines for CR4
 * ------------------------------------------------------------------------------------------------
 * Used in protected mode to control operations such as virtual-8086 support, 
 * enabling I/O breakpoints, page size extension and machine-check exceptions. 
 * ------------------------------------------------------------------------------------------------
 */
#define CR4_PCE		0x00000100	// Performance counter enable
#define CR4_MCE		0x00000040	// Machine Check Enable
#define CR4_PSE		0x00000010	// Page Size Extensions
#define CR4_DE		0x00000008	// Debugging Extensions
#define CR4_TSD		0x00000004	// Time Stamp Disable
#define CR4_PVI		0x00000002	// Protected-Mode Virtual Interrupts
#define CR4_VME		0x00000001	// V86 Mode Extensions

/*
 * Defines for GDT/LDT and Segment descriptor / selector
 * -------------------------------------------------------------------------------------------------
 * https://wiki.osdev.org/Global_Descriptor_Table
 * https://wiki.osdev.org/Segment_Selector
 * https://wiki.osdev.org/Descriptor
 * -------------------------------------------------------------------------------------------------
 */


#ifndef __ASSEMBLER__	// not __ASSEMBLER__

#include <types.h>

struct Pseudodesc {
	uint16_t l_limit;
	uint32_t l_base;
} __attribute__((packed));

// Segment Descriptors
struct Segdesc {
	unsigned int lim_15_0 : 16;  // Low bits of segment limit
	unsigned int base_15_0 : 16; // Low bits of segment base address
	unsigned int base_23_16 : 8; // Middle bits of segment base address
	unsigned int type : 4;       // Segment type (see STS_ constants)
	unsigned int s : 1;          // 0 = system, 1 = application
	unsigned int dpl : 2;        // Descriptor Privilege Level
	unsigned int p : 1;          // Present
	unsigned int lim_19_16 : 4;  // High bits of segment limit
	unsigned int avl : 1;        // Unused (available for software use)
	unsigned int rsv1 : 1;       // Reserved
	unsigned int db : 1;         // 0 = 16-bit segment, 1 = 32-bit segment
	unsigned int g : 1;          // Granularity: limit scaled by 4K when set
	unsigned int base_31_24 : 8; // High bits of segment base address
};
// Null segment
#define SEG_NULL	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
// Segment that is loadable but faults when used
#define SEG_FAULT	{ 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0 }
// Normal segment
#define SEG(type, base, lim, dpl) 					\
{ ((lim) >> 12) & 0xffff, (base) & 0xffff, ((base) >> 16) & 0xff,	\
    type, 1, dpl, 1, (unsigned) (lim) >> 28, 0, 0, 1, 1,		\
    (unsigned) (base) >> 24 }
#define SEG16(type, base, lim, dpl) (struct Segdesc)			\
{ (lim) & 0xffff, (base) & 0xffff, ((base) >> 16) & 0xff,		\
    type, 1, dpl, 1, (unsigned) (lim) >> 16, 0, 0, 1, 0,		\
    (unsigned) (base) >> 24 }

#endif /* !__ASSEMBLER__ */

// Application segment type bits
#define STA_X		0x8	    // Executable segment
#define STA_E		0x4	    // Expand down (non-executable segments)
#define STA_C		0x4	    // Conforming code segment (executable only)
#define STA_W		0x2	    // Writeable (non-executable segments)
#define STA_R		0x2	    // Readable (executable segments)
#define STA_A		0x1	    // Accessed

// System segment type bits
#define STS_T16A	0x1	    // Available 16-bit TSS
#define STS_LDT		0x2	    // Local Descriptor Table
#define STS_T16B	0x3	    // Busy 16-bit TSS
#define STS_CG16	0x4	    // 16-bit Call Gate
#define STS_TG		0x5	    // Task Gate / Coum Transmitions
#define STS_IG16	0x6	    // 16-bit Interrupt Gate
#define STS_TG16	0x7	    // 16-bit Trap Gate
#define STS_T32A	0x9	    // Available 32-bit TSS
#define STS_T32B	0xB	    // Busy 32-bit TSS
#define STS_CG32	0xC	    // 32-bit Call Gate
#define STS_IG32	0xE	    // 32-bit Interrupt Gate
#define STS_TG32	0xF	    // 32-bit Trap Gate

#define KERNEL_PRIVILAGE_LEVEL 0
#define USER_PRIVILAGE_LEVEL 3

#endif



