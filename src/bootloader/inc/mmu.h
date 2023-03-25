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

#endif



