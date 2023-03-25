.set NSEG,           0x0         # In GDT, first selector is NULL
.set PROT_MODE_CSEG, 0x8         # kernel code segment selector (64-128) 
.set PROT_MODE_DSEG, 0x10        # kernel data segment selector (128-192)
.set CR0_PE_ON,      0x1         # protected mode enable flag (PE)

.globl boot
boot:
  .code16                     # Assemble for 16-bit mode
  cli                         # Disable interrupts
  cld                         # String operations increment

  # Set up the important data segment registers (DS, ES, SS).
  xorw    %ax,%ax             # Segment number zero
  movw    %ax,%ds             # -> Data Segment
  movw    %ax,%es             # -> Extra Segment
  movw    %ax,%ss             # -> Stack Segment
  movw    %ax,%gs             #
  movw    %ax,%fs             #

  # Enable A20:
  #   For backwards compatibility with the earliest PCs, physical
  #   address line 20 is tied low, so that addresses higher than
  #   1MB wrap around to zero by default.  This code undoes this.
enableA20.1:
  inb     $0x64,%al               # Wait for not busy
  testb   $0x2,%al
  jnz     enableA20.1

  movb    $0xd1,%al               # 0xd1 -> port 0x64
  outb    %al,$0x64

enableA20.2:
  inb     $0x64,%al               # Wait for not busy
  testb   $0x2,%al
  jnz     enableA20.2             # If is busy, retry until is not anymore

  movb    $0xdf,%al               # 0xdf -> port 0x60
  outb    %al,$0x60

  # Switch from real to protected mode, using a bootstrap GDT (gdt)
  # and segment translation that makes virtual addresses 
  # identical to their physical addresses, so that the 
  # effective memory map does not change during the switch.
  lgdt    gdtdesc
  movl    %cr0, %eax              # Set eax to value stored in cr0 register
  orl     $CR0_PE_ON, %eax        # Set Prot mode enable bit 
  movl    %eax, %cr0              # Save modification
  
  # Jump to next instruction, but in 32-bit code segment.
  # Switches processor into 32-bit mode.
  ljmp    $PROT_MODE_CSEG, $protcseg

  .code32                     # Assemble for 32-bit mode
protcseg:
  # Set up the protected-mode data segment registers
  movw    $PROT_MODE_DSEG, %ax    # Our data segment selector
  movw    %ax, %ds                # -> DS: Data Segment
  movw    %ax, %es                # -> ES: Extra Segment
  movw    %ax, %fs                # -> FS
  movw    %ax, %gs                # -> GS
  movw    %ax, %ss                # -> SS: Stack Segment
  
  # Set up the stack pointer and call into C.
  movl    $boot, %esp
  call kload

  # If bootmain returns (it shouldn't), loop.
spin:
  jmp spin

# Bootstrap GDT
.p2align 2                                # force 4 byte alignment
gdt:
gdt_null:
  .word 0x0000                            #
  .word 0x0000                            #
  .byte 0x00                              #
  .byte 0x00                              # GDT[0] = NULL
  .byte 0x00                              #
  .byte 0x00                              #
gdt_cseg:
  .word 0xFFFF                            #
  .word 0x0000                            #  
  .byte 0x00                              #
  .byte 0x9A                              # GDT[1] --- > CODE SEGMENT 
  .byte 0xCF                              #
  .byte 0x00                              #
gdt_dseg:
  .word 0xFFFF                            #
  .word 0x0000                            #
  .byte 0x00                              #
  .byte 0x92                              #
  .byte 0xCF                              #
  .byte 0x00                              #
gdt_end:
gdtdesc:
  .word   gdt_end-gdt                     # sizeof(gdt) - 1
  .long   gdt                             # address gdt

