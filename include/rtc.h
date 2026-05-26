#ifndef SACRAMENTUOS_RTC_H
#define SACRAMENTUOS_RTC_H

#include <stdint.h>

typedef struct rtc_time {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint16_t year;
} rtc_time_t;

rtc_time_t rtc_read_time(void);

#endif
