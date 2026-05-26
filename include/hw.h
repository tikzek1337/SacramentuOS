#ifndef SACRAMENTUOS_HW_H
#define SACRAMENTUOS_HW_H

#include <stdint.h>

typedef struct hw_info {
    int cpuid_ok;
    char cpu_vendor[13];
    char cpu_brand[49];
    uint32_t cpu_signature;
    uint32_t cpu_features_edx;
    uint32_t cpu_features_ecx;
    uint32_t pci_devices;
    uint16_t vga_vendor, vga_device;
    uint16_t net_vendor, net_device;
    uint16_t storage_vendor, storage_device;
    uint16_t usb_vendor[8], usb_device[8];
    uint8_t usb_prog_if[8];
    uint8_t usb_count;
    char usb_summary[96];
} hw_info_t;

void hw_detect(void);
const hw_info_t* hw_get_info(void);
const char* hw_usb_type(uint8_t prog_if);
void hw_format_pci_id(char* out, uint16_t vendor, uint16_t device);
uint32_t hw_pci_read(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);

#endif
