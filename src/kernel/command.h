#ifndef __COMMAND_H__
#define __COMMAND_H__

#define OK			0
#define POWEROFF	-1
#define INVALID 	-10

void wait_input(void);
int display_help(int argc, char **argv);
int display_kernel_info(int argc, char **argv);
int poweroff_machine(int argc, char **argv);

struct command_t {
	const char name[10];
	char description[100];
	int acount;
	char **args;
	int (*function)(int argc, char **argv);
};

#endif /* __COMMAND_H__ */