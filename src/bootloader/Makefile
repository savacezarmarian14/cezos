QEMU=qemu-system-i386
QEMU_FLAGS= -nographic

ASM=nasm

BOOTLOADER=bootloader

all: build

run: build
	$(QEMU) $(QEMU_FLAGS) $(BOOTLOADER).bin

build: $(BOOTLOADER).bin

$(BOOTLOADER).bin:
	nasm -f bin $(BOOTLOADER).asm -o $(BOOTLOADER).bin

clean:
	rm -f $(BOOTLOADER).bin
