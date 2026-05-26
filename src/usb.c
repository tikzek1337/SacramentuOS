#include "usb.h"
#include "hw.h"
#include "string.h"

static usb_stack_info_t g_usb;

void usb_stack_init(void) {
    memset(&g_usb, 0, sizeof(g_usb));
    const hw_info_t* h = hw_get_info();
    g_usb.controllers = h->usb_count;
    for (uint8_t i = 0; i < h->usb_count; i++) {
        if (h->usb_prog_if[i] == 0x00) g_usb.uhci++;
        else if (h->usb_prog_if[i] == 0x10) g_usb.ohci++;
        else if (h->usb_prog_if[i] == 0x20) g_usb.ehci++;
        else if (h->usb_prog_if[i] == 0x30) g_usb.xhci++;
    }
    strcpy(g_usb.status, "PCI USB host-controller enumeration active; HID transfer engine is not enabled yet");
}

const usb_stack_info_t* usb_stack_get_info(void) { return &g_usb; }
