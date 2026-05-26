#ifndef SACRAMENTUOS_IDT_H
#define SACRAMENTUOS_IDT_H

#include <stdint.h>

typedef struct registers {
    uint32_t gs, fs, es, ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
} registers_t;

typedef void (*isr_t)(registers_t*);

void idt_install(void);
void register_interrupt_handler(uint8_t n, isr_t handler);

#endif
