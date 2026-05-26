#include "multiboot.h"
#include "system.h"
#include "vga.h"
#include "idt.h"
#include "timer.h"
#include "keyboard.h"
#include "shell.h"
#include "string.h"
#include "io.h"
#include "kprintf.h"

void kernel_main(uint32_t magic, uint32_t mbi_addr) {
    terminal_initialize();

    g_multiboot_magic = magic;
    if (magic == MULTIBOOT_BOOTLOADER_MAGIC && mbi_addr != 0) {
        multiboot_info_t* mbi = (multiboot_info_t*)mbi_addr;
        if (mbi->flags & 0x1) {
            g_mem_lower_kb = mbi->mem_lower;
            g_mem_upper_kb = mbi->mem_upper;
        }
        if ((mbi->flags & (1 << 9)) && mbi->boot_loader_name) {
            strncpy(g_bootloader_name, (const char*)mbi->boot_loader_name, sizeof(g_bootloader_name) - 1);
            g_bootloader_name[sizeof(g_bootloader_name) - 1] = '\0';
        }
    }

    idt_install();
    timer_install();
    keyboard_install();
    enable_interrupts();

    shell_boot_animation();
    shell_start();

    system_halt();
}
