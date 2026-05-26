#include "kprintf.h"
#include "vga.h"
#include "string.h"

static void print_uint(uint32_t value, int base) {
    char buf[33];
    utoa(value, buf, base);
    terminal_writestring(buf);
}

static void print_int(int value) {
    char buf[33];
    itoa(value, buf, 10);
    terminal_writestring(buf);
}

void kvprintf(const char* fmt, va_list args) {
    for (size_t i = 0; fmt[i]; i++) {
        if (fmt[i] != '%') {
            terminal_putchar(fmt[i]);
            continue;
        }

        i++;
        switch (fmt[i]) {
            case '%': terminal_putchar('%'); break;
            case 'c': terminal_putchar((char)va_arg(args, int)); break;
            case 's': {
                const char* s = va_arg(args, const char*);
                terminal_writestring(s ? s : "(null)");
                break;
            }
            case 'd':
            case 'i': print_int(va_arg(args, int)); break;
            case 'u': print_uint(va_arg(args, uint32_t), 10); break;
            case 'x':
            case 'X': print_uint(va_arg(args, uint32_t), 16); break;
            default:
                terminal_putchar('%');
                terminal_putchar(fmt[i]);
                break;
        }
    }
}

void kprintf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    kvprintf(fmt, args);
    va_end(args);
}
