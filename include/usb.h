#ifndef SACRAMENTUOS_USB_H
#define SACRAMENTUOS_USB_H

#include <stdint.h>

typedef struct usb_stack_info {
    uint8_t controllers;
    uint8_t xhci;
    uint8_t ehci;
    uint8_t ohci;
    uint8_t uhci;
    char status[128];
} usb_stack_info_t;

void usb_stack_init(void);
const usb_stack_info_t* usb_stack_get_info(void);

#endif
