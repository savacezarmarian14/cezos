#ifndef __PROC_H__
#define __PROC_H__

#include <trapframe.h>
#include <uproc.h>

void proc_create(uint8_t *bin_entry);
void proc_start(void);
void proc_free(struct task_struct *t);

struct task_struct *pid_to_proc(pid_t pid, int check_perms);


#endif