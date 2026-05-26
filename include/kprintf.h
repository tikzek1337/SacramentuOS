#ifndef SACRAMENTUOS_KPRINTF_H
#define SACRAMENTUOS_KPRINTF_H

#include <stdarg.h>
#include <stdint.h>

void kprintf(const char* fmt, ...);
void kvprintf(const char* fmt, va_list args);

#endif
