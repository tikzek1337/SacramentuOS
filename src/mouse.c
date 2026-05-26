#include "mouse.h"
#include "idt.h"
#include "io.h"
#include "fb.h"

static volatile int g_x = 320;
static volatile int g_y = 240;
static volatile int g_dx = 0;
static volatile int g_dy = 0;
static volatile int g_w = 640;
static volatile int g_h = 480;
static volatile uint8_t g_buttons = 0;
static volatile uint8_t g_left_pressed = 0;
static volatile uint8_t g_left_released = 0;
static volatile uint8_t g_right_pressed = 0;
static volatile uint8_t g_right_released = 0;
static volatile uint8_t g_moved = 0;
static volatile uint8_t g_present = 0;
static volatile uint8_t g_packet[3];
static volatile int g_cycle = 0;

static void ps2_wait_input(void) {
    for (int i = 0; i < 100000; i++) {
        if (!(inb(0x64) & 0x02)) return;
    }
}

static int ps2_wait_output(void) {
    for (int i = 0; i < 100000; i++) {
        if (inb(0x64) & 0x01) return 1;
    }
    return 0;
}

static void mouse_write(uint8_t value) {
    ps2_wait_input();
    outb(0x64, 0xD4);
    ps2_wait_input();
    outb(0x60, value);
}

static uint8_t mouse_read(void) {
    if (!ps2_wait_output()) return 0;
    return inb(0x60);
}

static void clamp_mouse(void) {
    if (g_x < 0) g_x = 0;
    if (g_y < 0) g_y = 0;
    if (g_x >= g_w) g_x = g_w - 1;
    if (g_y >= g_h) g_y = g_h - 1;
}

static void mouse_process_byte(uint8_t data) {
    if (g_cycle == 0 && !(data & 0x08)) return; /* packet sync bit */
    g_packet[g_cycle++] = data;
    if (g_cycle < 3) return;
    g_cycle = 0;

    int dx = (int8_t)g_packet[1];
    int dy = (int8_t)g_packet[2];
    if (g_packet[0] & 0x40) dx = 0; /* X overflow */
    if (g_packet[0] & 0x80) dy = 0; /* Y overflow */

    uint8_t old = g_buttons;
    uint8_t now = g_packet[0] & 0x07;
    g_buttons = now;
    if ((now & MOUSE_LEFT) && !(old & MOUSE_LEFT)) g_left_pressed = 1;
    if (!(now & MOUSE_LEFT) && (old & MOUSE_LEFT)) g_left_released = 1;
    if ((now & MOUSE_RIGHT) && !(old & MOUSE_RIGHT)) g_right_pressed = 1;
    if (!(now & MOUSE_RIGHT) && (old & MOUSE_RIGHT)) g_right_released = 1;

    if (dx || dy) {
        g_dx += dx;
        g_dy += -dy;
        g_x += dx;
        g_y -= dy;
        clamp_mouse();
        g_moved = 1;
    }
}

static void mouse_callback(registers_t* regs) {
    (void)regs;
    uint8_t status = inb(0x64);
    if (!(status & 0x01)) return;
    uint8_t data = inb(0x60);
    mouse_process_byte(data);
}

void mouse_poll(void) {
    if (!g_present) return;
    for (int i = 0; i < 32; i++) {
        uint8_t status = inb(0x64);
        if (!(status & 0x01)) break;
        if (!(status & 0x20)) break; /* do not steal keyboard bytes */
        mouse_process_byte(inb(0x60));
    }
}

void mouse_set_bounds(int width, int height) {
    if (width > 0) g_w = width;
    if (height > 0) g_h = height;
    if (g_x >= g_w) g_x = g_w / 2;
    if (g_y >= g_h) g_y = g_h / 2;
}

void mouse_install(void) {
    if (fb_available()) {
        g_x = (int)fb_width() / 2;
        g_y = (int)fb_height() / 2;
        mouse_set_bounds((int)fb_width(), (int)fb_height());
    }

    register_interrupt_handler(44, mouse_callback);

    ps2_wait_input(); outb(0x64, 0xA8); /* enable auxiliary PS/2 device */
    ps2_wait_input(); outb(0x64, 0x20); /* read controller command byte */
    uint8_t status = mouse_read();
    status |= 0x02;  /* enable IRQ12 */
    status &= ~0x20; /* enable mouse clock */
    ps2_wait_input(); outb(0x64, 0x60);
    ps2_wait_input(); outb(0x60, status);

    mouse_write(0xF6); mouse_read(); /* defaults */
    mouse_write(0xF4); mouse_read(); /* enable streaming */

    outb(0xA1, inb(0xA1) & ~(1 << 4)); /* IRQ12 */
    outb(0x21, inb(0x21) & ~(1 << 2)); /* cascade */
    g_present = 1;
}

int mouse_get_state(mouse_state_t* out) {
    mouse_poll();
    if (!out) return 0;
    out->x = g_x; out->y = g_y; out->dx = g_dx; out->dy = g_dy;
    out->buttons = g_buttons;
    out->left_pressed = g_left_pressed;
    out->left_released = g_left_released;
    out->right_pressed = g_right_pressed;
    out->right_released = g_right_released;
    out->moved = g_moved;
    out->present = g_present;
    g_dx = 0; g_dy = 0; g_left_pressed = 0; g_left_released = 0;
    g_right_pressed = 0; g_right_released = 0; g_moved = 0;
    return 1;
}
