#include "rtc.h"
#include "io.h"

static uint8_t cmos_read(uint8_t reg) {
    outb(0x70, reg);
    io_wait();
    return inb(0x71);
}

static int cmos_update_in_progress(void) {
    outb(0x70, 0x0A);
    io_wait();
    return inb(0x71) & 0x80;
}

static uint8_t bcd_to_bin(uint8_t value) {
    return (uint8_t)((value & 0x0F) + ((value / 16) * 10));
}

rtc_time_t rtc_read_time(void) {
    rtc_time_t t;
    while (cmos_update_in_progress());

    t.second = cmos_read(0x00);
    t.minute = cmos_read(0x02);
    t.hour   = cmos_read(0x04);
    t.day    = cmos_read(0x07);
    t.month  = cmos_read(0x08);
    t.year   = cmos_read(0x09);

    uint8_t status_b = cmos_read(0x0B);
    if (!(status_b & 0x04)) {
        t.second = bcd_to_bin(t.second);
        t.minute = bcd_to_bin(t.minute);
        t.hour = (uint8_t)((t.hour & 0x80) | bcd_to_bin((uint8_t)(t.hour & 0x7F)));
        t.day = bcd_to_bin(t.day);
        t.month = bcd_to_bin(t.month);
        t.year = bcd_to_bin((uint8_t)t.year);
    }

    if (!(status_b & 0x02) && (t.hour & 0x80)) {
        t.hour = (uint8_t)(((t.hour & 0x7F) + 12) % 24);
    }

    t.year += 2000;
    return t;
}
