#include "system.h"
#include "io.h"
#include "kprintf.h"

uint32_t g_multiboot_magic = 0;
uint32_t g_mem_lower_kb = 0;
uint32_t g_mem_upper_kb = 0;
char g_bootloader_name[64] = "unknown";

void system_reboot(void) {
    kprintf("Rebooting...\n");
    uint8_t good = 0x02;
    while (good & 0x02) good = inb(0x64);
    outb(0x64, 0xFE);
    while (1) cpu_halt();
}

void system_halt(void) {
    kprintf("CPU halted. Power off the machine or close QEMU.\n");
    disable_interrupts();
    while (1) cpu_halt();
}
