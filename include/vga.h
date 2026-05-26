#ifndef SACRAMENTUOS_VGA_H
#define SACRAMENTUOS_VGA_H

#include <stddef.h>
#include <stdint.h>

#define VGA_WIDTH  80
#define VGA_HEIGHT 25

typedef enum {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN = 14,
    VGA_COLOR_WHITE = 15,
} vga_color_t;

uint8_t vga_entry_color(vga_color_t fg, vga_color_t bg);
uint16_t vga_entry(unsigned char uc, uint8_t color);

void terminal_initialize(void);
void terminal_setcolor(uint8_t color);
uint8_t terminal_getcolor(void);
void terminal_setfg(vga_color_t fg);
void terminal_clear(void);
void terminal_putchar(char c);
void terminal_write(const char* data, size_t size);
void terminal_writestring(const char* data);
void terminal_backspace(void);
void terminal_update_cursor(void);
void terminal_setcursor(size_t row, size_t column);
size_t terminal_get_row(void);
size_t terminal_get_column(void);
void terminal_center(const char* text);
void terminal_draw_rule(char ch);

#endif
