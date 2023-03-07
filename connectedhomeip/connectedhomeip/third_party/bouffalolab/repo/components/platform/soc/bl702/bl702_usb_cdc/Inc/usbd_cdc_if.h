#ifndef __USBD_CDC_IF_H__
#define __USBD_CDC_IF_H__

#include "bl702_common.h"
#include "usb_def.h"
#include "usbd_cdc.h"
#include "bl702_usb.h"
#include "bl702_hal_def.h"
#include "bl702_hal_pcd.h"

extern USBD_CDC_LineCodingTypeDef cdc_line_coding;

/** CDC Interface callback. */
extern USBD_CDC_ItfTypeDef USBD_Interface_fops_FS;

#endif /* __USBD_CDC_IF_H__ */

