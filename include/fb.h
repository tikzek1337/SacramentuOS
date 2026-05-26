#ifndef SACRAMENTUOS_FB_H
#define SACRAMENTUOS_FB_H

#include <stdint.h>

int fb_init_from_multiboot(uint32_t magic, uint32_t mbi_addr);
int fb_available(void);
uint32_t fb_width(void);
uint32_t fb_height(void);
uint32_t fb_bpp(void);
uint32_t fb_pitch(void);
uint32_t fb_rgb(uint8_t r, uint8_t g, uint8_t b);
void fb_put_pixel(int x, int y, uint32_t color);
void fb_clear(uint32_t color);
void fb_present(void);
int fb_uses_backbuffer(void);
void fb_fill_rect(int x, int y, int w, int h, uint32_t color);
void fb_rect(int x, int y, int w, int h, uint32_t color);
void fb_line(int x0, int y0, int x1, int y1, uint32_t color);
void fb_draw_char(int x, int y, char ch, uint32_t fg, uint32_t bg, int scale);
void fb_draw_text(int x, int y, const char* text, uint32_t fg, uint32_t bg, int scale);
void fb_draw_text_transparent(int x, int y, const char* text, uint32_t fg, int scale);

#endif
