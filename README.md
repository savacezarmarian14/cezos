# cezos
Minimal monolithic x86 (32-bit) operating system

cezos is a minimal monolithic operating system developed as a personal learning project.
The project focuses on low-level x86 concepts such as bootstrapping, segmentation, paging,
and basic process scheduling.

---

## Features

- Custom x86 bootloader
- Real mode to protected mode transition
- Global Descriptor Table (GDT) configuration
- 32-bit paging-based memory management
- Kernel and user space separation
- Simple scheduler (single process execution)
- Designed to run on QEMU (i386)

---

## Bootloader

The bootloader is custom written and performs the early initialization of the system.

Responsibilities:
- Executes in 16-bit real mode
- Loads the kernel image from disk into RAM
- Uses direct port I/O operations (inb / outb) for hardware access
- Switches the CPU to protected mode
- Sets up the Global Descriptor Table (GDT)
- Transfers control to the kernel entry point

The bootloader is linked at address 0x7C00 and written to the first sector of the disk image.

---

## Global Descriptor Table (GDT)

The Global Descriptor Table is used to configure segmentation and memory protection
in protected mode.

In cezos, the GDT:
- Defines kernel code and data segments
- Defines user code and data segments
- Enforces privilege levels
- Provides hardware-level memory protection

Segmentation is primarily used for protection, while paging is used for address translation.

---

## Memory Management

Memory management is implemented using 32-bit paging.

Characteristics:
- Page directory and page tables are used
- Virtual addresses are translated to physical addresses
- Kernel memory is mapped with higher privileges
- User space memory is isolated from kernel space

This setup provides basic isolation and protection between execution contexts.

---

## Process Scheduling

The scheduler is intentionally simple and educational.

Characteristics:
- Single-core execution
- Only one process can run on the CPU at a time
- No preemption
- Minimal context switching

The goal is to demonstrate basic task execution inside a kernel.

---

## Project Structure

src/
  bootloader/        Bootloader source code
  kernel/            Kernel source code and linker script
  lib/               Kernel support libraries
  inc/               Shared header files

build/
  bootloader/        Bootloader object files and binary
  kernel/            Kernel object files and binary

kernel.img           Bootable disk image

---

## Build & Run

The project is built using a custom Makefile and targets a 32-bit x86 environment.

### Build

make

This generates:
- build/bootloader/bootloader
- build/kernel/kernel
- kernel.img

### Run in QEMU

make qemu

### Debugging with GDB

Start QEMU in debug mode:
make qemu-gdb

In another terminal, start GDB:
make gdb

GDB connects to QEMU on port 1234 and loads symbols for the bootloader and kernel.

### Clean

make clean

Removes all build artifacts and recreates the build directories.

---

## Notes / Limitations

- Educational project
- Freestanding environment (no libc)
- Statically linked kernel
- No multitasking or preemption
- Not intended for production use

---

## License

MIT License

