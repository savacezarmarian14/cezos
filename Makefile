GDBPORT=1234	
# Define directories
SOURCE_DIR := src
BUILD_DIR := build

# Define sources and objects directories
BOOT_SRC_DIR := $(SOURCE_DIR)/bootloader
BOOT_BUILD_DIR := $(BUILD_DIR)/bootloader

KERN_SRC_DIR := $(SOURCE_DIR)/kernel
KERN_BUILD_DIR := $(BUILD_DIR)/kernel

LIB_SRC_DIR := $(SOURCE_DIR)/lib
INC_SRC_DIR := $(SOURCE_DIR)/inc

# Define compiler and linker flags
CC := gcc
ASM := nasm
CFLAGS := -m32 -fno-pie -c -g -fno-stack-protector
CFLAGS += -std=gnu99
CFLAGS += -static
CFLAGS += -Wno-builtin-declaration-mismatch -Wno-unused-parameter
CFLAGS += -static-libgcc -lgcc -ffreestanding

ASMFLAGS := -f elf32
LD := ld
LDFLAGS := -m elf_i386
INCLUDE := -I $(INC_SRC_DIR) -I $(SOURCE_DIR)/kernel -I $(SOURCE_DIR)/lib

# Define QEMU 
QEMU=qemu-system-i386
QEMUFLAGS+=-drive file=kernel.img,index=0,media=disk,format=raw -serial mon:stdio -gdb tcp::$(GDBPORT)


# Define the list of source files
# ------------------------------- BOOT SOURCES -----------------------------------#
BOOT_SRCS := $(wildcard $(BOOT_SRC_DIR)/*.S) $(wildcard $(BOOT_SRC_DIR)/*.c)
# ------------------------------ KERNEL SOURCES ----------------------------------#
KERN_SRCS := $(wildcard $(KERN_SRC_DIR)/*.S) $(wildcard $(LIB_SRC_DIR)/*.c) $(wildcard $(KERN_SRC_DIR)/*.c) 


# Convert source file names to object file names
# ------------------------------- BOOT OBJECTS -----------------------------------#
BOOT_OBJS := $(patsubst $(BOOT_SRC_DIR)/%.S,$(BOOT_BUILD_DIR)/%.o,$(BOOT_SRCS))
BOOT_OBJS := $(patsubst $(BOOT_SRC_DIR)/%.c,$(BOOT_BUILD_DIR)/%.o,$(BOOT_OBJS))
# ------------------------------ KERNEL OBJECTS ----------------------------------#
KERN_OBJS := $(patsubst $(KERN_SRC_DIR)/%.S,$(KERN_BUILD_DIR)/%.o,$(KERN_SRCS))
KERN_OBJS := $(patsubst $(KERN_SRC_DIR)/%.c,$(KERN_BUILD_DIR)/%.o,$(KERN_OBJS))
KERN_OBJS := $(patsubst $(LIB_SRC_DIR)/%.c,$(KERN_BUILD_DIR)/%.o,$(KERN_OBJS))

# Define the target executable
BOOT_TARGET := $(BOOT_BUILD_DIR)/bootloader 
KERN_TARGET := $(KERN_BUILD_DIR)/kernel
OS_TARGET 	:= kernel.img


# Define the default target
all: $(OS_TARGET)

$(OS_TARGET): build
	dd if=/dev/zero of=$(OS_TARGET)~ count=10000 2>/dev/null
	dd if=$(BOOT_BUILD_DIR)/bootloader of=$(OS_TARGET)~ conv=notrunc 2>/dev/null
	dd if=$(KERN_BUILD_DIR)/kernel of=$(OS_TARGET)~ seek=1 conv=notrunc 2>/dev/null
	mv $(OS_TARGET)~ $(OS_TARGET)

build: $(BOOT_TARGET) $(KERN_TARGET)

# Define the rule to build the target executable
$(BOOT_TARGET): $(BOOT_OBJS)
	$(LD) $(LDFLAGS) -N -e boot -Ttext 0x7C00 -o $@.out $^
	objcopy -S -O binary -j .text $@.out $@
	perl $(BOOT_SRC_DIR)/sign.pl $(BOOT_BUILD_DIR)/bootloader 
$(KERN_TARGET): $(KERN_OBJS)
	$(LD) $(LDFLAGS) -T $(KERN_SRC_DIR)/kernel.ld -o $@ $^

# Define the rule to build object files from C source files
$(BOOT_BUILD_DIR)/%.o: $(BOOT_SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I $(INC_SRC_DIR) -Os -o $@ $<

$(BOOT_BUILD_DIR)/%.o: $(BOOT_SRC_DIR)/%.S
	$(CC) $(CFLAGS) -I $(INC_SRC_DIR) -o $@ $<

$(KERN_BUILD_DIR)/%.o: $(KERN_SRC_DIR)/%.S
	$(CC) $(CFLAGS) -T $(KERN_SRC_DIR)/kernel.ld $(INCLUDE) -o $@ $<

$(KERN_BUILD_DIR)/%.o: $(KERN_SRC_DIR)/%.c
	$(CC) $(CFLAGS) -T $(KERN_SRC_DIR)/kernel.ld $(INCLUDE) -o $@ $<

$(KERN_BUILD_DIR)/%.o: $(LIB_SRC_DIR)/%.c
	$(CC) $(CFLAGS) -T $(KERN_SRC_DIR)/kernel.ld $(INCLUDE) -o $@ $<

qemu: $(OS_TARGET)
	$(QEMU) $(QEMUFLAGS) -nographic
qemu-gdb: $(OS_TARGET)
	$(QEMU) $(QEMUFLAGS) -S -nographic
gdb:
	objcopy --only-keep-debug $(BOOT_BUILD_DIR)/bootloader.out $(BOOT_BUILD_DIR)/boot.sym
	objcopy --only-keep-debug $(KERN_BUILD_DIR)/kernel $(KERN_BUILD_DIR)/kern.sym
	gdb -ex 'target remote localhost:$(GDBPORT)' -ex 'symbol-file $(BOOT_BUILD_DIR)/boot.sym' \
		-ex 'add-symbol-file $(KERN_BUILD_DIR)/kern.sym'
		
# Define the clean target
clean:
	rm -rf $(BUILD_DIR)
	mkdir -p $(BOOT_BUILD_DIR)
	mkdir -p $(KERN_BUILD_DIR)
	rm -f $(OS_TARGET)
