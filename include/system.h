#ifndef SACRAMENTUOS_SYSTEM_H
#define SACRAMENTUOS_SYSTEM_H

#include <stdint.h>

#define OS_NAME "SacramentuOS"
#define OS_VERSION "0.2.3"
#define OS_CODENAME "Compact Help IDE Keys"
#define OS_ARCH "i386"
#define OS_BUILD_TYPE "freestanding C + GNU Assembly"

extern uint32_t g_multiboot_magic;
extern uint32_t g_mem_lower_kb;
extern uint32_t g_mem_upper_kb;
extern char g_bootloader_name[64];

void system_reboot(void);
void system_halt(void);

#endif
