#ifndef __BL702_USB_CDC_H__
#define __BL702_USB_CDC_H__


#include "bl702_glb.h"
#include "bl702_usb.h"


void usb_cdc_start(int fd_console);
void usb_cdc_monitor(void);
int usb_cdc_is_port_open(void);
int usb_cdc_read(uint8_t *data, uint32_t len);
int usb_cdc_write(const uint8_t *data, uint32_t len);
int usb_cdc_data_send(const uint8_t *data, uint32_t len);
void usb_cdc_stop(void);
void usb_cdc_restore(void);
void usb_cdc_data_out_irq(uint8_t epnum);


#endif
