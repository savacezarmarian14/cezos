#ifndef __UPROC_H__
#define __UPROC_H__

#include <types.h>
#include <memlayout.h>
#include <trapframe.h>

#define PROC_BIT_LEN		10
#define PROC_COUNT			(1 << PROC_BIT_LEN)
#define PROC_INDEX(proc_id)	((proc_id) & (PROC_COUNT))


/* Define states */
#define STATE_FREE		0
#define STATE_DONE		1
#define STATE_READY		2
#define STATE_RUNNING	3
#define STATE_WAITING	4

struct task_struct {
	struct trapframe task_tf;

	/* Proccess identifier and parent identifier */
	pid_t pid;
	pid_t ppid;

	/* Link variable for free list */
	struct task_struct *next_free;

	/* State in CPU READY / RUNNING / WAITING / DONE / FREE */
	uint32_t cpu_state;
	uint32_t proc_runs;

	pde_t *task_pgdir;
};

#endif /* __UPROC_H__ */