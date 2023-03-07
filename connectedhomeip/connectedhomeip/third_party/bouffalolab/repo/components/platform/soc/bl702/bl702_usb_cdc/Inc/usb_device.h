#ifndef __USB_DEVICE__H__
#define __USB_DEVICE__H__

#include "bl702_common.h"
#include "usb_def.h"
#include "usbd_def.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"

void USB_Device_Init(void);
void USB_Error_Handler(void);

#endif
