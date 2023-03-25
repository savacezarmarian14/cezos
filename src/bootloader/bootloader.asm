;;; Sava Cezar Marian
;;; Lucrare Licenta 2023
;;; Version 2.0

;;; This file contains the code of a bootloader

bits 16
org 0x7c00

boot:
    mov ax, 0x2401          ; this number is needed to enable A20 bit
    int 0x15                ; call that enable a20 bit

mov ax, 0x3                 ; 0x3 is needed to set the vga mode 3
int 0x10                    ; call that set vha mode 3

lgdt [gdt_pointer]

mov eax, cr0                ; set eax value to cr0 value
or eax, 0x1                 ; set bit 0 to 1 (jump to protected mode)
mov cr0, eax               ; move to cr0 the modifcation

jmp CODE_SEG:boot2          ; jump to CODE SEGMENT - boot2 label
    
gdt_start:
    dq 0x0                  ; NULL (8 bytes)
gdt_code:
    dw 0xFFFF               ; Limit [1]
    dw 0x0                  ; Base [1]
    db 0x0                  ; Base [2]
    db 10011010b            ; Access byte [P DPL1 DPL2 S E DC RW A] 
    db 11001111b            ; Limit [2] [1111 - 0xF] Flags [1100 0xC]
    db 0x0                  ; Base [3]    
gdt_data:
    dw 0xFFFF               ; Limit [1]
    dw 0x0                  ; Base [1]
    db 0x0                  ; Base [2]
    db 10010010b            ; Access byte [P DPL1 DPL2 S E DC RW A] 
    db 11001111b            ; Limit [2] [1111 - 0xF] Flags [1100 0xC]
    db 0x0                  ; Base [3]  
gdt_end:

gdt_pointer:
    dw gdt_end - gdt_start  ; Size
    dd gdt_start            ; Start
CODE_SEG equ gdt_code - gdt_start   ; CODE SEGMENT
DATA_SEG equ gdt_data - gdt_start   ; DATA SEGMENT

bits 32
boot2:
    mov ax, DATA_SEG
    mov ds, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esi, hello
    mov ebx, 0xb8000
.loop:
    lodsb
    or al, al
    jz halt
    or eax, 0x0100
    mov word [ebx], ax
    add ebx, 2
    jmp .loop

halt:
    cli
    hlt

hello: db "Hello world!", 0

times 510 - ($-$$) db 0

dw 0xaa55


