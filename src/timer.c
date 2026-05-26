#include "timer.h"
#include "idt.h"
#include "io.h"

static volatile uint32_t tick_count = 0;

static void timer_callback(registers_t* regs) {
    (void)regs;
    tick_count++;
}

static void timer_phase(uint32_t hz) {
    uint32_t divisor = 1193180 / hz;
    outb(0x43, 0x36);
    outb(0x40, (uint8_t)(divisor & 0xFF));
    outb(0x40, (uint8_t)((divisor >> 8) & 0xFF));
}

void timer_install(void) {
    timer_phase(TIMER_HZ);
    register_interrupt_handler(32, timer_callback);
}

uint32_t timer_ticks(void) {
    return tick_count;
}

uint32_t timer_seconds(void) {
    return tick_count / TIMER_HZ;
}

void timer_sleep(uint32_t ms) {
    uint32_t ticks = (ms * TIMER_HZ) / 1000;
    if (ticks == 0) ticks = 1;
    uint32_t end = tick_count + ticks;
    while (tick_count < end) {
        cpu_halt();
    }
}
