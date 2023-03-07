/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2014        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ffconf.h"
#include "diskio.h"     /* FatFs lower layer API */
#include <stdbool.h>
#include <stdio.h>
#include "diskio_sd.h"

#if defined(MTK_SSUSB_HOST_ENABLE)
	#include "diskio_usb.h"
#endif

#include "ff.h"

#define DEV_SD  0
#define DEV_USB 1

DSTATUS Drive0_Stat = STA_NOINIT;   /* SD physical drive status */
DSTATUS Drive1_Stat = STA_NOINIT;   /* USB physical drive status */

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status(
    BYTE pdrv       /* Physical drive nmuber to identify the drive */
)
{
    switch (pdrv)
    {
        case DEV_SD:
            return Drive0_Stat;
#if defined(MTK_SSUSB_HOST_ENABLE)
        case DEV_USB:
            return Drive1_Stat;
#endif
        default:
            return Drive0_Stat;
    }
}


/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(
    BYTE pdrv               /* Physical drive nmuber to identify the drive */
)
{
    switch (pdrv)
    {
        case DEV_SD:
            return sd_disk_initialize(pdrv);
#if defined(MTK_SSUSB_HOST_ENABLE)
        case DEV_USB:
            return usb_disk_initialize(pdrv);
#endif
        default:
            return RES_PARERR;
    }


}


/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(
    BYTE pdrv,      /* Physical drive nmuber to identify the drive */
    BYTE *buff,     /* Data buffer to store read data */
    _DWORD sector,   /* Sector address in LBA */
    UINT count      /* Number of sectors to read */
)
{
    switch (pdrv)
    {
        case DEV_SD:
            return sd_disk_read(pdrv, buff, sector, count);
#if defined(MTK_SSUSB_HOST_ENABLE)
        case DEV_USB:
            return usb_disk_read(pdrv, buff, sector, count);
#endif
        default:
            return RES_PARERR;
    }
}


/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write(
    BYTE pdrv,          /* Physical drive nmuber to identify the drive */
    const BYTE *buff,   /* Data to be written */
    _DWORD sector,       /* Sector address in LBA */
    UINT count          /* Number of sectors to write */
)
{
    switch (pdrv)
    {
        case DEV_SD:
            return sd_disk_write(pdrv, buff, sector, count);
#if defined(MTK_SSUSB_HOST_ENABLE)
        case DEV_USB:
            return usb_disk_write(pdrv, buff, sector, count);
#endif
        default:
            return RES_PARERR;
    }
}
#endif



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl(
    BYTE pdrv,      /* Physical drive nmuber (0..) */
    BYTE cmd,       /* Control code */
    void *buff      /* Buffer to send/receive control data */
)
{
    switch (pdrv)
    {
        case DEV_SD:
            return sd_disk_ioctl(pdrv, cmd, buff);
#if defined(MTK_SSUSB_HOST_ENABLE)
        case DEV_USB:
            return usb_disk_ioctl(pdrv, cmd, buff);
#endif
        default:
            return RES_PARERR;
    }
}
#endif




#if !_FS_READONLY && !_FS_NORTC
_DWORD get_fattime(void)
{
    // get the current time

    return 0; //return the current time instead of 0 if current time is got
}
#endif


