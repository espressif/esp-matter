/*-----------------------------------------------------------------------/
/  Low level disk interface modlue include file   (C)ChaN, 2014          /
/-----------------------------------------------------------------------*/

#ifndef __DISKIO_SD_H__
#define __DISKIO_SD_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "hal.h"
#include "diskio.h"

#if defined(HAL_SD_MODULE_ENABLED) || defined(MTK_FATFS_ON_SPI_SD)
/*---------------------------------------*/
/* Prototypes for disk control functions */

DSTATUS sd_disk_initialize (BYTE pdrv);
DRESULT sd_disk_read (BYTE pdrv, BYTE* buff, _DWORD sector, UINT count);
DRESULT sd_disk_write (BYTE pdrv, const BYTE* buff, _DWORD sector, UINT count);
DRESULT sd_disk_ioctl (BYTE pdrv, BYTE cmd, void* buff);

#endif
#ifdef __cplusplus
}
#endif

#endif
/* __DISKIO_SD_H__ */
