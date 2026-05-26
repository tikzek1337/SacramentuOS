#include "hw.h"
#include "io.h"
#include "string.h"

static hw_info_t g_hw;

static void cpuid(uint32_t leaf, uint32_t subleaf, uint32_t* a, uint32_t* b, uint32_t* c, uint32_t* d) {
    __asm__ volatile ("cpuid" : "=a"(*a), "=b"(*b), "=c"(*c), "=d"(*d) : "a"(leaf), "c"(subleaf));
}

static void copy_reg(char* dst, uint32_t v) {
    dst[0] = (char)(v & 0xFF);
    dst[1] = (char)((v >> 8) & 0xFF);
    dst[2] = (char)((v >> 16) & 0xFF);
    dst[3] = (char)((v >> 24) & 0xFF);
}

uint32_t hw_pci_read(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address = ((uint32_t)1 << 31) | ((uint32_t)bus << 16) | ((uint32_t)slot << 11) | ((uint32_t)func << 8) | (offset & 0xFC);
    outl(0xCF8, address);
    return inl(0xCFC);
}

const char* hw_usb_type(uint8_t prog_if) {
    if (prog_if == 0x00) return "UHCI";
    if (prog_if == 0x10) return "OHCI";
    if (prog_if == 0x20) return "EHCI";
    if (prog_if == 0x30) return "xHCI";
    if (prog_if == 0x80) return "USB vendor-specific";
    if (prog_if == 0xFE) return "USB device controller";
    return "USB unknown";
}

void hw_format_pci_id(char* out, uint16_t vendor, uint16_t device) {
    char v[16], d[16];
    strcpy(out, "ven=0x");
    utoa(vendor, v, 16);
    strcat(out, v);
    strcat(out, " dev=0x");
    utoa(device, d, 16);
    strcat(out, d);
}

static void add_usb_summary(uint8_t pi) {
    const char* t = hw_usb_type(pi);
    if (g_hw.usb_summary[0]) strcat(g_hw.usb_summary, ", ");
    if (strlen(g_hw.usb_summary) + strlen(t) < sizeof(g_hw.usb_summary) - 1) strcat(g_hw.usb_summary, t);
}

void hw_detect(void) {
    memset(&g_hw, 0, sizeof(g_hw));
    strcpy(g_hw.cpu_vendor, "unknown");
    strcpy(g_hw.cpu_brand, "unknown CPU");
    strcpy(g_hw.usb_summary, "none");

    uint32_t a,b,c,d;
    cpuid(0, 0, &a, &b, &c, &d);
    if (a != 0) {
        g_hw.cpuid_ok = 1;
        copy_reg(g_hw.cpu_vendor + 0, b);
        copy_reg(g_hw.cpu_vendor + 4, d);
        copy_reg(g_hw.cpu_vendor + 8, c);
        g_hw.cpu_vendor[12] = '\0';
        cpuid(1, 0, &a, &b, &c, &d);
        g_hw.cpu_signature = a;
        g_hw.cpu_features_ecx = c;
        g_hw.cpu_features_edx = d;
        uint32_t max_ext;
        cpuid(0x80000000u, 0, &max_ext, &b, &c, &d);
        if (max_ext >= 0x80000004u) {
            uint32_t regs[12];
            cpuid(0x80000002u,0,&regs[0],&regs[1],&regs[2],&regs[3]);
            cpuid(0x80000003u,0,&regs[4],&regs[5],&regs[6],&regs[7]);
            cpuid(0x80000004u,0,&regs[8],&regs[9],&regs[10],&regs[11]);
            for (int i = 0; i < 12; i++) copy_reg(g_hw.cpu_brand + i * 4, regs[i]);
            g_hw.cpu_brand[48] = '\0';
        }
    }

    g_hw.usb_summary[0] = '\0';
    for (uint16_t bus = 0; bus < 256; bus++) {
        for (uint8_t slot = 0; slot < 32; slot++) {
            for (uint8_t func = 0; func < 8; func++) {
                uint32_t id = hw_pci_read((uint8_t)bus, slot, func, 0x00);
                if (id == 0xFFFFFFFFu) continue;
                uint16_t vendor = (uint16_t)(id & 0xFFFF);
                uint16_t device = (uint16_t)((id >> 16) & 0xFFFF);
                uint32_t classreg = hw_pci_read((uint8_t)bus, slot, func, 0x08);
                uint8_t prog_if = (uint8_t)((classreg >> 8) & 0xFF);
                uint8_t subclass = (uint8_t)((classreg >> 16) & 0xFF);
                uint8_t class_code = (uint8_t)((classreg >> 24) & 0xFF);
                g_hw.pci_devices++;
                if (class_code == 0x03 && g_hw.vga_vendor == 0) { g_hw.vga_vendor = vendor; g_hw.vga_device = device; }
                if (class_code == 0x02 && g_hw.net_vendor == 0) { g_hw.net_vendor = vendor; g_hw.net_device = device; }
                if (class_code == 0x01 && g_hw.storage_vendor == 0) { g_hw.storage_vendor = vendor; g_hw.storage_device = device; }
                if (class_code == 0x0C && subclass == 0x03 && g_hw.usb_count < 8) {
                    int idx = g_hw.usb_count++;
                    g_hw.usb_vendor[idx] = vendor;
                    g_hw.usb_device[idx] = device;
                    g_hw.usb_prog_if[idx] = prog_if;
                    add_usb_summary(prog_if);
                }
                if (func == 0) {
                    uint32_t header = hw_pci_read((uint8_t)bus, slot, func, 0x0C);
                    if (!((header >> 23) & 1)) break;
                }
            }
        }
    }
    if (!g_hw.usb_summary[0]) strcpy(g_hw.usb_summary, "none");
}

const hw_info_t* hw_get_info(void) { return &g_hw; }
