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
#include "hal.h"
#include "ff.h"

extern DSTATUS Drive1_Stat; /* Physical drive status */

#if defined(MTK_SSUSB_HOST_ENABLE)
#include <FreeRTOS.h>
#include <string.h>
#include "ssusb_host_msc.h"
/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS usb_disk_initialize(
    BYTE pdrv                 /* Physical drive nmuber to identify the drive */
)
{
    struct usbdev *dev;

    if (DRV_USB != pdrv)
    {
        return RES_PARERR;
    }

    if (Drive1_Stat & STA_NODISK)
    {
        return Drive1_Stat;
    }

    if ( !(Drive1_Stat & STA_NOINIT)  ) // Avoid Re-Init
    {
        return Drive1_Stat;
    }

    dev = get_msc_device();
    if (!dev)
    {
        printf("Error, No Udisk Found! \r\n");
        return Drive1_Stat;
    }
    else
    {
        Drive1_Stat &= ~STA_NOINIT; /* Clear STA_NOINIT flag */
        printf("Udisk Found! \r\n");
        return Drive1_Stat;
    }

    return Drive1_Stat;

}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT usb_disk_read(
    BYTE pdrv,      /* Physical drive nmuber to identify the drive */
    BYTE *buff,     /* Data buffer to store read data */
    _DWORD sector,   /* Sector address in LBA */
    UINT count      /* Number of sectors to read */
)
{
    int result;
    struct usbdev *dev;

    if ((DRV_USB != pdrv) || !count)
    {
        return RES_PARERR;
    }

    if (Drive1_Stat & STA_NOINIT) // The device has not been initialized
    {
        return RES_NOTRDY;
    }

    dev = get_msc_device();
    if (!dev)
    {
        printf("Error, No Udisk Found! \r\n");
        return Drive1_Stat;
    }

    result = readwrite_blocks_512(dev, sector, count, cbw_direction_data_in, (u8 *)buff);
    if (result)
    {
        printf("Read data from udisk fail\n");
        return RES_ERROR;
    }
    else
    {
        return RES_OK;
    }
    return RES_ERROR;

}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/
#if _USE_WRITE
DRESULT usb_disk_write(
    BYTE pdrv,          /* Physical drive nmuber to identify the drive */
    const BYTE *buff,   /* Data to be written */
    _DWORD sector,       /* Sector address in LBA */
    UINT count          /* Number of sectors to write */
)
{
    int result;
    struct usbdev *dev;
    if ((DRV_USB != pdrv) || !count) {
        return RES_PARERR;
    }

    if (Drive1_Stat & STA_NOINIT) // The device has not been initialized
    {
        return RES_NOTRDY;
    }

    dev = get_msc_device();
    if (!dev)
    {
        printf("Error, No Udisk Found! \r\n");
        return Drive1_Stat;
    }

    result = readwrite_blocks_512(dev, sector, count, cbw_direction_data_out, (u8 *)buff);
    if (result)
    {
        printf("Write data to udisk fail\n");
        return RES_ERROR;
    }
    else
    {
        return RES_OK;
    }
    return RES_ERROR;

}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT usb_disk_ioctl(
    BYTE pdrv,      /* Physical drive nmuber (0..) */
    BYTE cmd,       /* Control code */
    void *buff      /* Buffer to send/receive control data */
)
{
    DRESULT result;
    _DWORD *temp_erase_pointer, erase_start, erase_end;
    uint64_t sector_count;
    struct usbdev *dev;
    int status;

    if (DRV_USB != pdrv)
    {
        return RES_PARERR;    // Check parameter
    }

    if (Drive1_Stat & STA_NOINIT)
    {
        return RES_NOTRDY;    // Check if drive is ready
    }

    dev = get_msc_device();
    if (!dev)
    {
        printf("Error, No Udisk Found! \r\n");
        return Drive1_Stat;
    }

    result = RES_ERROR;

    switch (cmd) {
        case CTRL_SYNC :           // Write process has been completed
            result = RES_OK;
            break;

        case GET_SECTOR_COUNT :    // Get drive capacity in unit of sector (_DWORD)
            sector_count = get_capacity(dev);
            *(uint32_t *)buff = (uint32_t)sector_count;   // unit is byte
            result = RES_OK;
            break;

        case GET_BLOCK_SIZE :      // Get erase block size in unit of sector (_DWORD)
            *(uint32_t *)buff = (uint32_t)(get_block_size(dev)*512);
            result = RES_OK;
            break;

        case CTRL_TRIM :           // Erase a block of sectors (used when _USE_TRIM == 1)
            temp_erase_pointer = buff;
            erase_start = temp_erase_pointer[0];
            erase_end = temp_erase_pointer[1];
            status = erase_sector(dev, erase_start, (erase_end - erase_start));
            if (!status)
            {
                result = RES_OK;
            }
            break;

        default:
            result = RES_PARERR;
    }
    return result;
}
#endif
#endif

