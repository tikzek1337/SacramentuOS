/* Interrupt and IRQ stubs for SacramentuOS. */
.section .text

.global idt_load
idt_load:
    mov 4(%esp), %eax
    lidt (%eax)
    ret

.extern isr_handler
.extern irq_handler

.macro ISR_NOERR num
.global isr\num
isr\num:
    cli
    pushl $0
    pushl $\num
    jmp isr_common_stub
.endm

.macro ISR_ERR num
.global isr\num
isr\num:
    cli
    pushl $\num
    jmp isr_common_stub
.endm

isr_common_stub:
    pusha
    push %ds
    push %es
    push %fs
    push %gs
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    push %esp
    call isr_handler
    add $4, %esp
    pop %gs
    pop %fs
    pop %es
    pop %ds
    popa
    add $8, %esp
    sti
    iret

.macro IRQ num irqnum
.global irq\irqnum
irq\irqnum:
    cli
    pushl $0
    pushl $\num
    jmp irq_common_stub
.endm

irq_common_stub:
    pusha
    push %ds
    push %es
    push %fs
    push %gs
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    push %esp
    call irq_handler
    add $4, %esp
    pop %gs
    pop %fs
    pop %es
    pop %ds
    popa
    add $8, %esp
    sti
    iret

ISR_NOERR 0
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 4
ISR_NOERR 5
ISR_NOERR 6
ISR_NOERR 7
ISR_ERR   8
ISR_NOERR 9
ISR_ERR   10
ISR_ERR   11
ISR_ERR   12
ISR_ERR   13
ISR_ERR   14
ISR_NOERR 15
ISR_NOERR 16
ISR_ERR   17
ISR_NOERR 18
ISR_NOERR 19
ISR_NOERR 20
ISR_NOERR 21
ISR_NOERR 22
ISR_NOERR 23
ISR_NOERR 24
ISR_NOERR 25
ISR_NOERR 26
ISR_NOERR 27
ISR_NOERR 28
ISR_NOERR 29
ISR_ERR   30
ISR_NOERR 31

IRQ 32 0
IRQ 33 1
IRQ 34 2
IRQ 35 3
IRQ 36 4
IRQ 37 5
IRQ 38 6
IRQ 39 7
IRQ 40 8
IRQ 41 9
IRQ 42 10
IRQ 43 11
IRQ 44 12
IRQ 45 13
IRQ 46 14
IRQ 47 15
