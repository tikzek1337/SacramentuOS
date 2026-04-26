[bits 32]
[extern kmain]
[extern irq0_handler]   ; обработчик таймера

global _start
global idt_load
global irq0_stub

_start:
    call kmain
    jmp $

; Заглушка для IRQ0 – вызывает C-функцию, затем отправляет EOI
irq0_stub:
    pusha
    call irq0_handler
    mov al, 0x20
    out 0x20, al
    popa
    iret

; Процедура загрузки IDT (параметр: адрес дескриптора idt_ptr)
idt_load:
    mov eax, [esp + 4]   ; адрес idt_ptr
    lidt [eax]
    ret