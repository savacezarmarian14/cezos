# Define compiler and linker flags
CC := gcc
ASM := nasm
CFLAGS := -m32 -fno-pie -c -g -Wall -nostdlib -fno-builtin

ASMFLAGS := -f elf32
LD := ld
LDFLAGS := -m elf_i386
INCLUDE := inc

# Define QEMU 
QEMU=qemu-system-i386
QEMUFLAGS= -nographic

SRC_FILES := $(wildcard *.c)
S_FILES := $(wildcard *.S)
OBJS := $(patsubst %.c,%.o,$(SRC_FILES))
OBJS += $(patsubst %.S,%.o,$(S_FILES))


kernel: $(OBJS)
	$(LD) $(LDFLAGS) -T kernel.ld -o kernel $^

%.o: %.c
	$(CC) $(CFLAGS) -T kernel.ld  -c -I inc -I . $< -o $@
%.o: %.S
	$(CC) $(CFLAGS) -T kernel.ld  -c -I inc -I . $< -o $@

clean:
	rm -f *.o
