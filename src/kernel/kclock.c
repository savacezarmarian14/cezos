#include <kclock.h>
#include <types.h>
#include <stdio.h>
#include <x86.h>

unsigned int
mc_read(unsigned int reg)
{
	unsigned int ret;

	outb(IO_RTC, reg);
	ret = inb(IO_RTC+1);

	return ret;
}

void mc_write(unsigned int reg, unsigned int datum)
{
	outb(IO_RTC, reg);
	outb(IO_RTC+1, datum);
}