[bits 32]
[extern kmain]
[extern irq0_handler]

global _start
global idt_load
global irq0_stub

_start:
    call kmain
    jmp $

irq0_stub:
    pusha
    call irq0_handler
    mov al, 0x20
    out 0x20, al
    popa
    iret

idt_load:
    mov eax, [esp + 4]
    lidt [eax]
    ret