#ifndef __INC_MEMLEYOUT_H__
#define __INC_MEMLAYOUT_H__

#ifndef __ASSEMBLER__

#include <types.h>
#include <mmu.h>

#endif  /* For the case we include this header in some .asm file*/

//                          
//                     Global Descriptor Table Entry
// 0x00 |---------------------------------------------------------|
//      |  base   |  flags  |  limit   |  access byte |   base    |
//      | [24-31] |  [0-3]  | [16-19]  |    [0-7]     |  [16-23]  |   
//      |---------------------------------------------------------|
//      |            base              |            limit         |
//      |           [0-15]             |            [0-15]        |
//      |---------------------------------------------------------|
// 0x08   
//
//
//

/* Gobal Descriptor table index */
#define GD_NULL                     0x00        // First Segment 
#define GD_KERNEL_TEXT              0x08        // Second Segment
#define GD_KERNEL_DATA              0x10        // Third Segment
#define GD_USER_TEXT                0x18        // Fourth Segment
#define GD_USER_DATA                0x20        // Fifth Segment
#define GD_TASK_SEGMENT_SELECTOR    0x28        // Sixth Segment


/*
 * Virtual memory map:                                Permissions
 *                                                    kernel/user
 *
 *    4 Gig -------->  +------------------------------+
 *                     |                              | RW/--
 *                     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *                     :              .               :
 *                     :              .               :
 *                     :              .               :
 *                     |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~| RW/--
 *                     |                              | RW/--
 *                     |   Remapped Physical Memory   | RW/--
 *                     |                              | RW/--
 *    KERNBASE, ---->  +------------------------------+ 0xf0000000      --+
 *    KSTACKTOP        |     CPU0's Kernel Stack      | RW/--  KSTKSIZE   |
 *                     | - - - - - - - - - - - - - - -|                   |
 *                     |      Invalid Memory (*)      | --/--  KSTKGAP    |
 *                     +------------------------------+                   |
 *                     |     CPU1's Kernel Stack      | RW/--  KSTKSIZE   |
 *                     | - - - - - - - - - - - - - - -|                 PTSIZE
 *                     |      Invalid Memory (*)      | --/--  KSTKGAP    |
 *                     +------------------------------+                   |
 *                     :              .               :                   |
 *                     :              .               :                   |
 *    MMIOLIM ------>  +------------------------------+ 0xefc00000      --+
 *                     |       Memory-mapped I/O      | RW/--  PTSIZE
 * ULIM, MMIOBASE -->  +------------------------------+ 0xef800000
 *                     |  Cur. Page Table (User R-)   | R-/R-  PTSIZE
 *    UVPT      ---->  +------------------------------+ 0xef400000
 *                     |          RO PAGES            | R-/R-  PTSIZE
 *    UPAGES    ---->  +------------------------------+ 0xef000000
 *                     |           RO ENVS            | R-/R-  PTSIZE
 * UTOP,UENVS ------>  +------------------------------+ 0xeec00000
 * UXSTACKTOP -/       |     User Exception Stack     | RW/RW  PGSIZE
 *                     +------------------------------+ 0xeebff000
 *                     |       Empty Memory (*)       | --/--  PGSIZE
 *    USTACKTOP  --->  +------------------------------+ 0xeebfe000
 *                     |      Normal User Stack       | RW/RW  PGSIZE
 *                     +------------------------------+ 0xeebfd000
 *                     |                              |
 *                     |                              |
 *                     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *                     .                              .
 *                     .                              .
 *                     .                              .
 *                     |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
 *                     |     Program Data & Heap      |
 *    UTEXT -------->  +------------------------------+ 0x00800000
 *    PFTEMP ------->  |       Empty Memory (*)       |        PTSIZE
 *                     |                              |
 *    UTEMP -------->  +------------------------------+ 0x00400000      --+
 *                     |       Empty Memory (*)       |                   |
 *                     | - - - - - - - - - - - - - - -|                   |
 *                     |  User STAB Data (optional)   |                 PTSIZE
 *    USTABDATA ---->  +------------------------------+ 0x00200000        |
 *                     |       Empty Memory (*)       |                   |
 *    0 ------------>  +------------------------------+                 --+
 */

/* Kernel Base Address */
#define KERNEL_BASE_ADDRESS         0xF0000000

/* Kernel Stack */
#define KERNEL_STACK_TOP            KERNEL_BASE_ADDRESS
#define KERNEL_STACK_SIZE           (8 * PAGE_SIZE)
#define KERNEL_STACK_GUARD          (8 * PAGE_SIZE)

/* Memory mapped I/O */
#define MEMORY_MAPPED_IO_LIMIT      (KERNEL_STACK_TOP        - PAGE_TABLE_SIZE)
#define MEMORY_MAPPED_IO_BASE       (MEMORY_MAPPED_IO_LIMIT  - PAGE_TABLE_SIZE)

/* User */
#define U_LIMIT                  (MEMORY_MAPPED_IO_BASE)
#define U_VIRTUAL_PAGE_TABLE     (USER_LIMIT              - PAGE_TABLE_SIZE)
#define U_PAGES                  (USER_VIRTUAL_PAGE_TABLE - PAGE_TABLE_SIZE)
#define U_ENVIRONMENTS           (USER_PAGES              - PAGE_TABLE_SIZE)
#define U_TOP                    (USER_ENVIRONMENTS)
#define U_EXCEPTION_STACK_TOP    (USER_TOP)
#define U_TEXT                   (2 * PAGE_TABLE_SIZE)

/* Temporary */
// Used for temporary page mappings.  Typed 'void*' for convenience
#define UTEMP		((void*) PTSIZE)
// Used for temporary page mappings for the user page-fault handler
// (should not conflict with other temporary page mappings)
#define PFTEMP		(UTEMP + PTSIZE - PGSIZE)
// The location of the user-level STABS data structure
#define USTABDATA	(PTSIZE / 2)

#define IOPHYSMEM	0x0A0000
#define EXTPHYSMEM	0x100000

#ifndef __ASSEMBLER__

/* Typedef for PageTable Entry and PageDirectory Entry */
typedef uint32_t pte_t;
typedef uint32_t pde_t;

#endif /* __ASSEMBLER__ */

#endif /* __INC_MEMLAYOUT_H__*/