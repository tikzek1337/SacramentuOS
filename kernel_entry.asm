[bits 32]
[extern kmain]
global _start
_start:
    ; ----- 5 в видеопамять (следующий символ после 4) -----
    mov byte [0xB8004], '5'
    mov byte [0xB8005], 0x0F

    call kmain
    jmp $