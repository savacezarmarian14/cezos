#include <proc.h>
#include <types.h>
#include <trapframe.h>
#include <mmu.h>
#include <x86.h>
#include <stdio.h>
#include <kmap.h>


struct task_struct *tasks;
struct task_struct *current;
struct task_struct *free_tasks;

struct Segdesc global_descriptor_table[] = {
	/* First ssegment is NULL */
	[GD_NULL_INDEX] = SEG_NULL,

	/* Define descriptor for kernel text segment */
	[GD_KERNEL_TEXT] = SEG(/*Execute*/ STA_X | /*read*/ STA_R,
							/*base*/ 0x0, /*lim*/ 0xffffffff,
							/*Descriptor privilage level*/ KERNEL_PRIVILAGE_LEVEL),

	/* Define descriptor for kenrel data segment */
	[GD_KD_INDEX] = SEG(STA_W, 0x0, 0xffffffff, KERNEL_PRIVILAGE_LEVEL),

	/* Define descriptor for user text segment */
	[GD_UT_INDEX] = SEG(STA_X | STA_R, 0x0, 0xffffffff, USER_PRIVILAGE_LEVEL),

	/* Define descriptor for user data segment */
	[GD_UD_INDEX] = SEG(STA_W, 0x0, 0xffffffff, USER_PRIVILAGE_LEVEL),

	/* Task segment selector is NULL for the moment */
	[GD_TSS_INDEX] = SEG_NULL,
};

struct Pseudodesc gdt_pd = {
	.l_limit  = sizeof(global_descriptor_table) - 1,
	.l_base = (uint32_t) global_descriptor_table,
};

struct task_struct *
pid_to_proc(pid_t pid, int check_perms)
{
	struct task_struct *task;

	if (pid == 0) {
		return current;
	}

	task = &tasks[pid];
	if (task == NULL) {
		cprintf("[Error] Invalid proccess for pid %d\n", pid);
		return NULL;
	}

	/* Check for permissions */
	if (check_perms != 0) {
		if (task != current) {
			cprintf("[Error] Invalid proccess for pid %d\n", pid);
			return NULL;
		}

		if (task->ppid != current->pid) {
			cprintf("[Error] Invalid proccess for pid %d\n", pid);
			return NULL;
		}
	}

	return task;
}

void
init_procs(void)
{
	int i;

	for (i = 0; i < PROC_COUNT; ++i) {
		if (i == 0) {
			free_tasks = &tasks[i];
			free_tasks->pid = 0;
		} else {
			tasks[i].pid = 0;
			tasks[i-1].next_free = &tasks[i]; // just for free list
		}
	}

	tasks[i-1].next_free = NULL; // end of free list
}


void
proc_init_percpu(void)
{
	lgdt(&gdt_pd);
	// The kernel does use ES, DS, and SS.  We'll change between
	// the kernel and user data segments as needed.
	asm volatile("movw %%ax,%%ds" : : "a" (GD_KERNEL_DATA));
	asm volatile("movw %%ax,%%es" : : "a" (GD_KERNEL_DATA));
	asm volatile("movw %%ax,%%ss" : : "a" (GD_KERNEL_DATA));
	// Load the kernel text segment into CS.
	asm volatile("ljmp %0,$1f\n 1:\n" : : "i" (GD_KERNEL_TEXT));
}

void
proc_init_vm(struct task_struct *task)
{
	int i;
	struct page_status *ps;
	pde_t *task_pgdir;

	ps = page_alloc(1);
	if (ps == NULL) {
		MEM_ERROR_LOOP("Not enought memory to init a task");
	}

	task_pgdir = page2kva(ps);
	if (task_pgdir == NULL) {
		MEM_ERROR_LOOP("Not a valid address for user page directory\n");
	}

	/* Make new pgdir created look like kernpgdir */
	task_pgdir = memcpy(task_pgdir, kern_page_directory, PAGE_SIZE);
	if (task_pgdir == NULL) {
		MEM_ERROR_LOOP("Couldn't copy memory form kern_pgdir to user_pgdir");
	}

	task->task_pgdir = task_pgdir;
	task->task_pgdir[PDX(U_VIRTUAL_PAGE_TABLE)] =
		(uint32_t) (PADDR(task->task_pgdir) | PTE_P | PTE_W);

	cprintf("[Info]: New user page directory crated: %x\n", task->task_pgdir);
	ps->refs += 1;
}

struct task_struct *
proc_alloc(pid_t ppid)
{
	// TODO
	return NULL;
}
