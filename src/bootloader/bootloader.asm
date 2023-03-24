;;; Sava Cezar Marian
;;; Lucrare Licenta 2023
;;; Version 1.0


;;; Aceasta este prima varianta de implementare a unui bootloader minimal
;;; in care afiseaza folosind o intrerupere in bootmode

bits 16                     ; CPU starts in 16-bit mode
org 0x7c00                  ; Start address should be 0x7c00

boot:
    mov si, hello           ; Incarc in si ce se afla in 'hello'
    mov ah, 0x0e            ; 0x0e - writing caracter in TTY mode

.loop:
    lodsb                   ; load byte at adress ds:si into a1
    or al, al               ; Set 'zero' flag if al in 0
    jz halt                 ; --- ERROR if al is 0 ---
    int 0x10                ; BIOS interrupt for VIDEO SERVICE
    jmp .loop


halt:
    cli                     ; disable intrerrupts
    hlt                     ; halt execution



hello:  db "Hello, world!", 0
times 510 - ($-$$) db 0     ; The remaining bytes in the bootloader are set to 0
                            ; Bootloader should have 512 bytes
dw 0xaa55                   ; BIOS expect the last double word to be this value
