#include "multiboot.h"
#include "system.h"
#include "vga.h"
#include "fb.h"
#include "idt.h"
#include "timer.h"
#include "keyboard.h"
#include "mouse.h"
#include "gui.h"
#include "hw.h"
#include "usb.h"
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

    fb_init_from_multiboot(magic, mbi_addr);
    hw_detect();
    usb_stack_init();

    idt_install();
    timer_install();
    keyboard_install();
    if (fb_available()) mouse_install();
    enable_interrupts();

    gui_boot_animation();
    gui_start();

    system_halt();
}
