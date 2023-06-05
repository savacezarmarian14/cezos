#ifndef __LIBUSER_H__
#define __LIBUSER_H__

#include <types.h>
#include <uproc.h>

extern const char *bin;
extern const struct task_struct *current;
extern const struct task_struct tasks[PROC_COUNT];
extern const struct page_status pages[];

void user_main(int argc, char *argv[]);

void exit(void);

void sys_cputs(const char *, size_t );
int  sys_cgetc(void);

pid_t sys_getpid(void);
int sys_kill(pid_t);



#endif /*__LIBUSER_H__ */