#include <command.h>
#include <stdio.h>
#include <memlayout.h>
#include <types.h>
#include <string.h>

struct command_t commands[] = {
	{"help", "Display all commands avalable", 0, NULL, display_help},
	{"kerninfo", "Display information about cezOS", 0, NULL, display_kernel_info},
	{"shutdown", "Shutdown the machine", 0, NULL, poweroff_machine},
};

int run_command(char *command_name)
{
	int i;
	int commands_count = (sizeof(commands) / sizeof(commands[0]));

	for (int i = 0; i < commands_count; ++i) {
		if (strcmp(command_name, commands[i].name) == 0) {
			return commands[i].function(commands[i].acount, commands[i].args);
		}
	}

	return INVALID;
}

void
wait_input(void)
{
	char *output_buffer;
	int res;
	int i;

	char *clr = "\n\n\n\n\n\n\n\n\n\n";

	for (i = 0; i < 3; i++) {
		cprintf("%s", clr);
	}

	while(1) {
		output_buffer = readline("cezOS > ");


		if (output_buffer != NULL) {
			res = run_command(output_buffer);
			if (res == POWEROFF)
				return;
		}
	}
}

int
display_help(int argc, char **argv)
{
	int i;
	int commands_count = sizeof(commands) / sizeof(commands[0]);

	for (i = 0; i < commands_count; i++) {
		cprintf("[Info] Command: %s\n[Info]\tDescription: %s\n[Info]\n",
			commands[i].name, commands[i].description);
	}

	return OK;
}

int
display_kernel_info(int argc, char **argv)
{
	extern char _start[], entry[], etext[], edata[], end_bss[];

	cprintf("[Info] Special kernel symbols:\n");
	cprintf("[Info] _start                  %08x (phys)\n", _start);
	cprintf("[Info] entry  %08x (virt)  %08x (phys)\n", entry, entry - KERNEL_BASE_ADDRESS);
	cprintf("[Info] etext  %08x (virt)  %08x (phys)\n", etext, etext - KERNEL_BASE_ADDRESS);
	cprintf("[Info] edata  %08x (virt)  %08x (phys)\n", edata, edata - KERNEL_BASE_ADDRESS);
	cprintf("[Info] end    %08x (virt)  %08x (phys)\n", end_bss, end_bss - KERNEL_BASE_ADDRESS);
	return OK;
}

int poweroff_machine(int argc, char **argv)
{
	return POWEROFF;
}