#include "fb.h"
#include "multiboot.h"
#include "system.h"
#include "string.h"

static uint8_t* g_fb = 0;
static uint32_t g_w = 0;
static uint32_t g_h = 0;
static uint32_t g_pitch = 0;
static uint32_t g_bpp = 0;
static uint8_t g_red_mask = 8, g_red_pos = 16;
static uint8_t g_green_mask = 8, g_green_pos = 8;
static uint8_t g_blue_mask = 8, g_blue_pos = 0;
static uint8_t g_reserved_mask = 8, g_reserved_pos = 24;

#define FB_BACKBUFFER_MAX_W 1280u
#define FB_BACKBUFFER_MAX_H 1024u
#define FB_BACKBUFFER_MAX_BYTES (FB_BACKBUFFER_MAX_W * FB_BACKBUFFER_MAX_H * 4u)

static uint8_t g_backbuffer[FB_BACKBUFFER_MAX_BYTES];
static uint8_t* g_draw = 0;
static uint32_t g_draw_pitch = 0;
static uint8_t g_use_backbuffer = 0;

static uint32_t bytes_per_pixel(void) { return g_bpp / 8u; }

static void fb_select_backbuffer(void) {
    uint32_t bpp_bytes = bytes_per_pixel();
    uint32_t need = g_w * g_h * bpp_bytes;
    if (g_fb && g_w && g_h && bpp_bytes && need <= FB_BACKBUFFER_MAX_BYTES) {
        g_draw = g_backbuffer;
        g_draw_pitch = g_w * bpp_bytes;
        g_use_backbuffer = 1;
    } else {
        g_draw = g_fb;
        g_draw_pitch = g_pitch;
        g_use_backbuffer = 0;
    }
}

static uint32_t scale_component(uint8_t v, uint8_t bits) {
    if (bits >= 8) return v;
    if (bits == 0) return 0;
    return (uint32_t)(v >> (8 - bits));
}

uint32_t fb_rgb(uint8_t r, uint8_t g, uint8_t b) {
    uint32_t c = 0;
    c |= scale_component(r, g_red_mask) << g_red_pos;
    c |= scale_component(g, g_green_mask) << g_green_pos;
    c |= scale_component(b, g_blue_mask) << g_blue_pos;
    if (g_reserved_mask) c |= ((1u << g_reserved_mask) - 1u) << g_reserved_pos;
    return c;
}

int fb_available(void) { return g_fb != 0 && g_w > 0 && g_h > 0 && (g_bpp == 32 || g_bpp == 24 || g_bpp == 16); }
uint32_t fb_width(void) { return g_w; }
uint32_t fb_height(void) { return g_h; }
uint32_t fb_bpp(void) { return g_bpp; }
uint32_t fb_pitch(void) { return g_pitch; }

int fb_init_from_multiboot(uint32_t magic, uint32_t mbi_addr) {
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC || mbi_addr == 0) return 0;
    multiboot_info_t* mbi = (multiboot_info_t*)mbi_addr;
    if (!(mbi->flags & (1u << 11)) || mbi->vbe_mode_info == 0) return 0;

    vbe_mode_info_t* mi = (vbe_mode_info_t*)mbi->vbe_mode_info;
    if (!(mi->attributes & 0x80)) return 0; /* linear framebuffer required */
    if (mi->physbase == 0 || mi->x_resolution == 0 || mi->y_resolution == 0) return 0;
    if (!(mi->bits_per_pixel == 32 || mi->bits_per_pixel == 24 || mi->bits_per_pixel == 16)) return 0;

    g_fb = (uint8_t*)(uintptr_t)mi->physbase;
    g_w = mi->x_resolution;
    g_h = mi->y_resolution;
    g_pitch = mi->pitch;
    g_bpp = mi->bits_per_pixel;
    if (mi->red_mask_size && mi->green_mask_size && mi->blue_mask_size) {
        g_red_mask = mi->red_mask_size;
        g_red_pos = mi->red_field_position;
        g_green_mask = mi->green_mask_size;
        g_green_pos = mi->green_field_position;
        g_blue_mask = mi->blue_mask_size;
        g_blue_pos = mi->blue_field_position;
        g_reserved_mask = mi->reserved_mask_size;
        g_reserved_pos = mi->reserved_field_position;
    }
    fb_select_backbuffer();
    return fb_available();
}

void fb_put_pixel(int x, int y, uint32_t color) {
    if (!fb_available()) return;
    if (x < 0 || y < 0 || (uint32_t)x >= g_w || (uint32_t)y >= g_h) return;
    uint8_t* p = g_draw + (uint32_t)y * g_draw_pitch + (uint32_t)x * bytes_per_pixel();
    if (g_bpp == 32) {
        *(volatile uint32_t*)p = color;
    } else if (g_bpp == 24) {
        p[0] = (uint8_t)(color & 0xFF);
        p[1] = (uint8_t)((color >> 8) & 0xFF);
        p[2] = (uint8_t)((color >> 16) & 0xFF);
    } else if (g_bpp == 16) {
        *(volatile uint16_t*)p = (uint16_t)color;
    }
}

void fb_fill_rect(int x, int y, int w, int h, uint32_t color) {
    if (!fb_available() || w <= 0 || h <= 0) return;
    if (x < 0) { w += x; x = 0; }
    if (y < 0) { h += y; y = 0; }
    if (x + w > (int)g_w) w = (int)g_w - x;
    if (y + h > (int)g_h) h = (int)g_h - y;
    if (w <= 0 || h <= 0) return;
    for (int yy = 0; yy < h; yy++) {
        uint8_t* row = g_draw + (uint32_t)(y + yy) * g_draw_pitch + (uint32_t)x * bytes_per_pixel();
        if (g_bpp == 32) {
            volatile uint32_t* px = (volatile uint32_t*)row;
            for (int xx = 0; xx < w; xx++) px[xx] = color;
        } else {
            for (int xx = 0; xx < w; xx++) fb_put_pixel(x + xx, y + yy, color);
        }
    }
}

void fb_clear(uint32_t color) { fb_fill_rect(0, 0, (int)g_w, (int)g_h, color); }

void fb_present(void) {
    if (!fb_available() || !g_use_backbuffer) return;
    uint32_t bpp_bytes = bytes_per_pixel();
    uint32_t row_bytes = g_w * bpp_bytes;
    for (uint32_t y = 0; y < g_h; y++) {
        uint8_t* dst = g_fb + y * g_pitch;
        uint8_t* src = g_backbuffer + y * g_draw_pitch;
        memcpy(dst, src, row_bytes);
    }
}

int fb_uses_backbuffer(void) { return g_use_backbuffer ? 1 : 0; }

void fb_rect(int x, int y, int w, int h, uint32_t color) {
    fb_fill_rect(x, y, w, 1, color);
    fb_fill_rect(x, y + h - 1, w, 1, color);
    fb_fill_rect(x, y, 1, h, color);
    fb_fill_rect(x + w - 1, y, 1, h, color);
}

static int iabs(int v) { return v < 0 ? -v : v; }
void fb_line(int x0, int y0, int x1, int y1, uint32_t color) {
    int dx = iabs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -iabs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    while (1) {
        fb_put_pixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

static const uint8_t* glyph(char ch) {
    static const uint8_t sp[7]={0,0,0,0,0,0,0};
    static const uint8_t unk[7]={14,17,1,6,4,0,4};
    static const uint8_t bang[7]={4,4,4,4,4,0,4};
    static const uint8_t dot[7]={0,0,0,0,0,0,4};
    static const uint8_t comma[7]={0,0,0,0,0,4,8};
    static const uint8_t colon[7]={0,4,4,0,4,4,0};
    static const uint8_t semi[7]={0,4,4,0,4,4,8};
    static const uint8_t dash[7]={0,0,0,31,0,0,0};
    static const uint8_t plus[7]={0,4,4,31,4,4,0};
    static const uint8_t slash[7]={1,2,4,8,16,0,0};
    static const uint8_t bslash[7]={16,8,4,2,1,0,0};
    static const uint8_t lpar[7]={2,4,8,8,8,4,2};
    static const uint8_t rpar[7]={8,4,2,2,2,4,8};
    static const uint8_t lb[7]={14,8,8,8,8,8,14};
    static const uint8_t rb[7]={14,2,2,2,2,2,14};
    static const uint8_t gt[7]={0,16,8,4,8,16,0};
    static const uint8_t lt[7]={0,1,2,4,2,1,0};
    static const uint8_t eq[7]={0,0,31,0,31,0,0};
    static const uint8_t us[7]={0,0,0,0,0,0,31};
    static const uint8_t zero[7]={14,17,19,21,25,17,14};
    static const uint8_t one[7]={4,12,4,4,4,4,14};
    static const uint8_t two[7]={14,17,1,2,4,8,31};
    static const uint8_t three[7]={30,1,1,14,1,1,30};
    static const uint8_t four[7]={2,6,10,18,31,2,2};
    static const uint8_t five[7]={31,16,16,30,1,1,30};
    static const uint8_t six[7]={6,8,16,30,17,17,14};
    static const uint8_t seven[7]={31,1,2,4,8,8,8};
    static const uint8_t eight[7]={14,17,17,14,17,17,14};
    static const uint8_t nine[7]={14,17,17,15,1,2,12};
    static const uint8_t A[7]={14,17,17,31,17,17,17};
    static const uint8_t B[7]={30,17,17,30,17,17,30};
    static const uint8_t C[7]={14,17,16,16,16,17,14};
    static const uint8_t D[7]={30,17,17,17,17,17,30};
    static const uint8_t E[7]={31,16,16,30,16,16,31};
    static const uint8_t F[7]={31,16,16,30,16,16,16};
    static const uint8_t G[7]={14,17,16,23,17,17,15};
    static const uint8_t H[7]={17,17,17,31,17,17,17};
    static const uint8_t I[7]={14,4,4,4,4,4,14};
    static const uint8_t J[7]={7,2,2,2,18,18,12};
    static const uint8_t K[7]={17,18,20,24,20,18,17};
    static const uint8_t L[7]={16,16,16,16,16,16,31};
    static const uint8_t M[7]={17,27,21,21,17,17,17};
    static const uint8_t N[7]={17,25,21,19,17,17,17};
    static const uint8_t O[7]={14,17,17,17,17,17,14};
    static const uint8_t P[7]={30,17,17,30,16,16,16};
    static const uint8_t Q[7]={14,17,17,17,21,18,13};
    static const uint8_t R[7]={30,17,17,30,20,18,17};
    static const uint8_t S[7]={15,16,16,14,1,1,30};
    static const uint8_t T[7]={31,4,4,4,4,4,4};
    static const uint8_t U[7]={17,17,17,17,17,17,14};
    static const uint8_t V[7]={17,17,17,17,17,10,4};
    static const uint8_t W[7]={17,17,17,21,21,21,10};
    static const uint8_t X[7]={17,17,10,4,10,17,17};
    static const uint8_t Y[7]={17,17,10,4,4,4,4};
    static const uint8_t Z[7]={31,1,2,4,8,16,31};
    if (ch >= 'a' && ch <= 'z') ch = (char)(ch - 32);
    switch (ch) {
        case ' ': return sp; case '!': return bang; case '.': return dot; case ',': return comma;
        case ':': return colon; case ';': return semi; case '-': return dash; case '+': return plus;
        case '/': return slash; case '\\': return bslash; case '(': return lpar; case ')': return rpar;
        case '[': return lb; case ']': return rb; case '<': return lt; case '>': return gt;
        case '=': return eq; case '_': return us;
        case '0': return zero; case '1': return one; case '2': return two; case '3': return three; case '4': return four;
        case '5': return five; case '6': return six; case '7': return seven; case '8': return eight; case '9': return nine;
        case 'A': return A; case 'B': return B; case 'C': return C; case 'D': return D; case 'E': return E; case 'F': return F;
        case 'G': return G; case 'H': return H; case 'I': return I; case 'J': return J; case 'K': return K; case 'L': return L;
        case 'M': return M; case 'N': return N; case 'O': return O; case 'P': return P; case 'Q': return Q; case 'R': return R;
        case 'S': return S; case 'T': return T; case 'U': return U; case 'V': return V; case 'W': return W; case 'X': return X;
        case 'Y': return Y; case 'Z': return Z;
        default: return unk;
    }
}

void fb_draw_char(int x, int y, char ch, uint32_t fg, uint32_t bg, int scale) {
    if (scale <= 0) scale = 1;
    const uint8_t* g = glyph(ch);
    for (int row = 0; row < 7; row++) {
        for (int col = 0; col < 5; col++) {
            uint32_t c = (g[row] & (1 << (4 - col))) ? fg : bg;
            fb_fill_rect(x + col * scale, y + row * scale, scale, scale, c);
        }
    }
}

void fb_draw_text(int x, int y, const char* text, uint32_t fg, uint32_t bg, int scale) {
    if (!text) return;
    int cx = x;
    int advance = 6 * (scale <= 0 ? 1 : scale);
    while (*text) {
        fb_draw_char(cx, y, *text, fg, bg, scale);
        cx += advance;
        text++;
    }
}

void fb_draw_text_transparent(int x, int y, const char* text, uint32_t fg, int scale) {
    if (!text) return;
    if (scale <= 0) scale = 1;
    int cx = x;
    const uint32_t transparent = 0;
    while (*text) {
        const uint8_t* g = glyph(*text++);
        for (int row = 0; row < 7; row++) {
            for (int col = 0; col < 5; col++) {
                if (g[row] & (1 << (4 - col))) {
                    fb_fill_rect(cx + col * scale, y + row * scale, scale, scale, fg);
                }
            }
        }
        cx += 6 * scale;
        (void)transparent;
    }
}
