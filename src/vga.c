#include "vga.h"
#include "io.h"
#include "string.h"

static const size_t VGA_MEMORY_ADDRESS = 0xB8000;
static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t* terminal_buffer;

uint8_t vga_entry_color(vga_color_t fg, vga_color_t bg) {
    return (uint8_t)(fg | bg << 4);
}

uint16_t vga_entry(unsigned char uc, uint8_t color) {
    return (uint16_t)uc | (uint16_t)color << 8;
}

static void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
    terminal_buffer[y * VGA_WIDTH + x] = vga_entry((unsigned char)c, color);
}

static void terminal_scroll(void) {
    for (size_t y = 1; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            terminal_buffer[(y - 1) * VGA_WIDTH + x] = terminal_buffer[y * VGA_WIDTH + x];
        }
    }
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        terminal_putentryat(' ', terminal_color, x, VGA_HEIGHT - 1);
    }
    if (terminal_row > 0) terminal_row--;
}

void terminal_update_cursor(void) {
    uint16_t pos = (uint16_t)(terminal_row * VGA_WIDTH + terminal_column);
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void terminal_setcursor(size_t row, size_t column) {
    if (row >= VGA_HEIGHT) row = VGA_HEIGHT - 1;
    if (column >= VGA_WIDTH) column = VGA_WIDTH - 1;
    terminal_row = row;
    terminal_column = column;
    terminal_update_cursor();
}

size_t terminal_get_row(void) {
    return terminal_row;
}

size_t terminal_get_column(void) {
    return terminal_column;
}

void terminal_initialize(void) {
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    terminal_buffer = (uint16_t*)VGA_MEMORY_ADDRESS;
    terminal_clear();
}

void terminal_setcolor(uint8_t color) {
    terminal_color = color;
}

uint8_t terminal_getcolor(void) {
    return terminal_color;
}

void terminal_setfg(vga_color_t fg) {
    terminal_color = vga_entry_color(fg, VGA_COLOR_BLACK);
}

void terminal_clear(void) {
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            terminal_putentryat(' ', terminal_color, x, y);
        }
    }
    terminal_row = 0;
    terminal_column = 0;
    terminal_update_cursor();
}

void terminal_putchar(char c) {
    if (c == '\n') {
        terminal_column = 0;
        terminal_row++;
        if (terminal_row >= VGA_HEIGHT) terminal_scroll();
        terminal_update_cursor();
        return;
    }

    if (c == '\r') {
        terminal_column = 0;
        terminal_update_cursor();
        return;
    }

    if (c == '\t') {
        for (int i = 0; i < 4; i++) terminal_putchar(' ');
        return;
    }

    terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
    terminal_column++;
    if (terminal_column >= VGA_WIDTH) {
        terminal_column = 0;
        terminal_row++;
        if (terminal_row >= VGA_HEIGHT) terminal_scroll();
    }
    terminal_update_cursor();
}

void terminal_backspace(void) {
    if (terminal_column > 0) {
        terminal_column--;
    } else if (terminal_row > 0) {
        terminal_row--;
        terminal_column = VGA_WIDTH - 1;
    } else {
        return;
    }
    terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);
    terminal_update_cursor();
}

void terminal_write(const char* data, size_t size) {
    for (size_t i = 0; i < size; i++) terminal_putchar(data[i]);
}

void terminal_writestring(const char* data) {
    terminal_write(data, strlen(data));
}

void terminal_center(const char* text) {
    size_t len = strlen(text);
    size_t spaces = len < VGA_WIDTH ? (VGA_WIDTH - len) / 2 : 0;
    for (size_t i = 0; i < spaces; i++) terminal_putchar(' ');
    terminal_writestring(text);
    terminal_putchar('\n');
}

void terminal_draw_rule(char ch) {
    for (int i = 0; i < VGA_WIDTH - 1; i++) terminal_putchar(ch);
    terminal_putchar('\n');
}

void terminal_put_at(size_t row, size_t column, char c, uint8_t color) {
    if (row >= VGA_HEIGHT || column >= VGA_WIDTH) return;
    terminal_putentryat(c, color, column, row);
}

void terminal_write_at(size_t row, size_t column, const char* text, uint8_t color) {
    if (!text || row >= VGA_HEIGHT || column >= VGA_WIDTH) return;
    size_t x = column;
    for (size_t i = 0; text[i] && x < VGA_WIDTH; i++, x++) {
        terminal_putentryat(text[i], color, x, row);
    }
}

void terminal_fill_rect(size_t row, size_t column, size_t width, size_t height, char ch, uint8_t color) {
    for (size_t y = 0; y < height && row + y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < width && column + x < VGA_WIDTH; x++) {
            terminal_putentryat(ch, color, column + x, row + y);
        }
    }
}

void terminal_hide_cursor(void) {
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);
}
