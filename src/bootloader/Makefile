ASM=nasm
ASM_FLAGS=-f bin

QEMU=qemu-system-i386
QEMUFLAGS=

BOOTLOADER=bootloader

build: clean
	$(ASM) $(ASM_FLAGS) $(BOOTLOADER).asm -o $(BOOTLOADER).bin

run: build
	$(QEMU) $(QEMUFLAGS) $(BOOTLOADER).bin

gdb: build
	$(QEMU) $(QEMUFLAGS) -s -hda $(BOOTLOADER).bin

clean:
	rm -f $(BOOTLOADER).bin
