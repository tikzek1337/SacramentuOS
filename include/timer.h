#ifndef SACRAMENTUOS_TIMER_H
#define SACRAMENTUOS_TIMER_H

#include <stdint.h>

#define TIMER_HZ 100

void timer_install(void);
uint32_t timer_ticks(void);
uint32_t timer_seconds(void);
void timer_sleep(uint32_t ms);

#endif
