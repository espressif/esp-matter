/*-----------------------------------------------------------------------/
/  Low level disk interface modlue include file   (C)ChaN, 2014          /
/-----------------------------------------------------------------------*/

#ifndef __DISKIO_USB_H__
#define __DISKIO_USB_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "hal.h"
#include "diskio.h"

#if defined(MTK_SSUSB_HOST_ENABLE)
/*---------------------------------------*/
/* Prototypes for disk control functions */

DSTATUS usb_disk_initialize (BYTE pdrv);
DRESULT usb_disk_read (BYTE pdrv, BYTE* buff, _DWORD sector, UINT count);
DRESULT usb_disk_write (BYTE pdrv, const BYTE* buff, _DWORD sector, UINT count);
DRESULT usb_disk_ioctl (BYTE pdrv, BYTE cmd, void* buff);

#endif
#ifdef __cplusplus
}
#endif

#endif
/* __DISKIO_USB_H__ */
