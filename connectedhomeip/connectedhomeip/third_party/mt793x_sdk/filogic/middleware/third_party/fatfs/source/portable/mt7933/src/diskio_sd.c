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

extern DSTATUS Drive0_Stat; /* Physical drive status */

#if defined(HAL_SD_MODULE_ENABLED)
#include "hal_msdc.h"
#include "hal_sd.h"
#include "hal_sd_define.h"
#include <FreeRTOS.h>

#define FATFS_SD_PORT (HAL_SD_PORT_0)

#define BLOCK_PER_FFSECTOR      (FF_MAX_SS / 512)
#define SD_CLOCK                (13000)

#if BLOCK_PER_FFSECTOR > 2
    #define FF_SD_DMA            1
#else
    #define FF_SD_DMA            0
#endif

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS sd_disk_initialize(
    BYTE pdrv                 /* Physical drive nmuber to identify the drive */
)
{
    hal_sd_config_t fatfs_sd_config = {HAL_SD_BUS_WIDTH_4, SD_CLOCK};
    uint32_t retry = 0;
    hal_sd_status_t result;

    if (DRV_CARD != pdrv) {
        return RES_PARERR;    /* Supports only drive 0 */
    }
    printf("FF_MAX_SS: %d\n", FF_MAX_SS);
    printf("SD init frequency: %d\n", SD_CLOCK);
#if FF_SD_DMA == 1
    printf("DMA mode\n");
#else
    printf("PIO mode\n");
#endif

    /* End user's SD card may have some problems of stability and            */
    /* performance, leading to some corner cases which cause failure.        */
    /* In order to resync the state between the SD card and the              */
    /* device when operations fail, retry machanism is needed.               */
    while (retry < 3) {
        result = hal_sd_init(FATFS_SD_PORT, &fatfs_sd_config);

        if (HAL_SD_STATUS_OK == result) {
            Drive0_Stat &= ~STA_NOINIT; /* Clear STA_NOINIT flag */
            return Drive0_Stat;
        } else {
            retry++;
        }
    }

    return Drive0_Stat;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT sd_disk_read(
    BYTE pdrv,      /* Physical drive nmuber to identify the drive */
    BYTE *buff,     /* Data buffer to store read data */
    _DWORD sector,   /* Sector address in LBA */
    UINT count      /* Number of sectors to read */
)
{
    hal_sd_config_t fatfs_sd_config = {HAL_SD_BUS_WIDTH_4, SD_CLOCK};
    uint32_t retry = 0;
    hal_sd_status_t status;
    if (DRV_CARD != pdrv || !count) {
        return RES_PARERR;    /* Check parameter */
    }
    if (Drive0_Stat & STA_NOINIT) {
        return RES_NOTRDY;    /* Check if drive is ready */
    }

    /* End user's SD card may have some problems of stability and            */
    /* performance, leading to some corner cases which cause failure.        */
    /* In order to resync the state between the SD card and the              */
    /* device when operations fail, retry machanism is needed.               */
#if FF_SD_DMA == 1
    unsigned char *buffin = (unsigned char *)PLAT_MALLOC_NC(count*FF_MAX_SS);
#endif
    while (retry < 3) {
#if FF_SD_DMA == 1
        status = hal_sd_read_blocks_dma_blocking(FATFS_SD_PORT, (uint32_t *)buffin, sector*BLOCK_PER_FFSECTOR, count*BLOCK_PER_FFSECTOR);        // DMA mode
#else
        status = hal_sd_read_blocks(FATFS_SD_PORT, (uint32_t *)buff, sector*BLOCK_PER_FFSECTOR, count*BLOCK_PER_FFSECTOR);                       // PIO mode
#endif
        if (HAL_SD_STATUS_OK == status) {
#if FF_SD_DMA == 1
            if(buffin == NULL){
                return RES_ERROR;
            }
            memcpy(buff, buffin, count*FF_MAX_SS);
            PLAT_FREE_NC((void *)buffin);
#endif
            return RES_OK;
        } else {
            status = hal_sd_init(FATFS_SD_PORT, &fatfs_sd_config);
            if (HAL_SD_STATUS_OK == status) {
                retry++;
            } else {
#if FF_SD_DMA == 1
                PLAT_FREE_NC((void *)buffin);
#endif
                return RES_ERROR;
            }
        }
    }
    return RES_ERROR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT sd_disk_write(
    BYTE pdrv,          /* Physical drive nmuber to identify the drive */
    const BYTE *buff,   /* Data to be written */
    _DWORD sector,       /* Sector address in LBA */
    UINT count          /* Number of sectors to write */
)
{
    hal_sd_config_t fatfs_sd_config = {HAL_SD_BUS_WIDTH_4, SD_CLOCK};
    uint32_t retry = 0;
    hal_sd_status_t status;

    if ((DRV_CARD != pdrv) || !count) {
        return RES_PARERR;    /* Check parameter */
    }
    if (Drive0_Stat & STA_NOINIT) {
        return RES_NOTRDY;    /* Check if drive is ready */
    }

    /* End user's SD card may have some problems of stability and            */
    /* performance, leading to some corner cases which cause failure.        */
    /* In order to resync the state between the SD card and the              */
    /* device when operations fail, retry machanism is needed.               */
#if FF_SD_DMA == 1
    //unsigned char *buffout = (unsigned char *)pvPortMallocNC(count*FF_MAX_SS);
    unsigned char *buffout = (unsigned char *)PLAT_MALLOC_NC(count*FF_MAX_SS);
    if(buffout == NULL) {
        return RES_ERROR;
    }
    memcpy(buffout, buff, count*FF_MAX_SS);
#endif
    while (retry < 3) {
#if FF_SD_DMA == 1
        status = hal_sd_write_blocks_dma_blocking(FATFS_SD_PORT, (uint32_t *)buffout, sector*BLOCK_PER_FFSECTOR, count*BLOCK_PER_FFSECTOR);     // DMA mode
#else
        status = hal_sd_write_blocks(FATFS_SD_PORT, (uint32_t *)buff, sector*BLOCK_PER_FFSECTOR, count*BLOCK_PER_FFSECTOR);                     // PIO mode
#endif
        if (HAL_SD_STATUS_OK == status) {
#if FF_SD_DMA == 1
            PLAT_FREE_NC(buffout);
#endif
            return RES_OK;
        } else {
            status = hal_sd_init(FATFS_SD_PORT, &fatfs_sd_config);
            if (HAL_SD_STATUS_OK == status) {
                retry++;
            } else {
#if FF_SD_DMA == 1
                PLAT_FREE_NC(buffout);
#endif
                return RES_ERROR;
            }
        }
    }
    return RES_ERROR;
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT sd_disk_ioctl(
    BYTE pdrv,      /* Physical drive nmuber (0..) */
    BYTE cmd,       /* Control code */
    void *buff      /* Buffer to send/receive control data */
)
{
    DRESULT result;
    _DWORD *temp_erase_pointer, erase_start, erase_end;
    hal_sd_status_t status;
    uint64_t sector_count;

    if (DRV_CARD != pdrv) {
        return RES_PARERR;    /* Check parameter */
    }
    if (Drive0_Stat & STA_NOINIT) {
        return RES_NOTRDY;    /* Check if drive is ready */
    }

    result = RES_ERROR;


    switch (cmd) {
        case CTRL_SYNC :        /* write process has been completed */
            result = RES_OK;
            break;

        case GET_SECTOR_COUNT :          /* Get drive capacity in unit of sector (DWORD) */
            status = hal_sd_get_capacity(FATFS_SD_PORT, &sector_count);
            if (status == HAL_SD_STATUS_OK) {
                *(uint32_t *)buff = (uint32_t)(sector_count / FF_MAX_SS);   /* unit is byte */
                result = RES_OK;
            }

            break;

        case GET_BLOCK_SIZE :   /* Get erase block size in unit of sector (DWORD) */
            status = hal_sd_get_erase_sector_size(FATFS_SD_PORT, (uint32_t *)buff);
            *(uint32_t *)buff *= BLOCK_PER_FFSECTOR;
            if (status == HAL_SD_STATUS_OK) {
                result = RES_OK;
            }

            break;

        case CTRL_TRIM :    /* Erase a block of sectors (used when _USE_TRIM == 1) */
            temp_erase_pointer = buff;
            erase_start = temp_erase_pointer[0];
            erase_end = temp_erase_pointer[1];
            status = hal_sd_erase_sectors(FATFS_SD_PORT, erase_start, (erase_end - erase_start));
            if (status == HAL_SD_STATUS_OK) {
                result = RES_OK;
            }

            break;


        /* Following commands are not used by FatFs module */

        case MMC_GET_TYPE :     /* Get MMC/SDC type (BYTE) */
            status = hal_sd_get_card_type(FATFS_SD_PORT, buff);
            if (status == HAL_SD_STATUS_OK) {
                result = RES_OK;
            }

            break;

        case MMC_GET_CSD :      /* Read CSD (16 bytes) */
            status = hal_sd_get_csd(FATFS_SD_PORT, buff);
            if (status == HAL_SD_STATUS_OK) {
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

