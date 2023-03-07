/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ffconf.h"
/* This fatfs subcomponent is disabled by default
 * To enable it, define following macro in ffconf.h */
#ifdef RAM_DISK_ENABLE

#include "fsl_common.h"
#include "fsl_ram_disk.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* clang-format off */
#define SECTOR_SIZE FF_MIN_SS /* usualy 512 B */
#define DISK_SIZE 65536     /* minmal disk size calculated as 128 * FF_MIN_SS (ff.c ln 4112) , 128*512=65536 */
/* clang-format on */

/*******************************************************************************
 * Globals
 ******************************************************************************/
static uint8_t disk_space[DISK_SIZE];

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Get RAM disk status.
 */
DSTATUS ram_disk_status(BYTE pdrv)
{
    if (pdrv != RAMDISK)
    {
        return STA_NOINIT;
    }
    return 0;
}

/*!
 * @brief Inidialize a RAM disk.
 */
DSTATUS ram_disk_initialize(BYTE pdrv)
{
    if (pdrv != RAMDISK)
    {
        return STA_NOINIT;
    }
    return 0;
}

/*!
 * @brief Read Sector(s) from RAM disk.
 */
DRESULT ram_disk_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count)
{
    if (pdrv != RAMDISK)
    {
        return RES_PARERR;
    }
    memcpy(buff, disk_space + sector * SECTOR_SIZE, SECTOR_SIZE * count);
    return RES_OK;
}

/*!
 * @brief Write Sector(s) to RAM disk.
 */
DRESULT ram_disk_write(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count)
{
    if (pdrv != RAMDISK)
    {
        return RES_PARERR;
    }
    memcpy(disk_space + sector * SECTOR_SIZE, buff, SECTOR_SIZE * count);
    return RES_OK;
}

/*!
 * @brief Miscellaneous RAM disk Functions.
 */
DRESULT ram_disk_ioctl(BYTE pdrv, BYTE cmd, void *buff)
{
    if (pdrv != RAMDISK)
    {
        return RES_PARERR;
    }
    switch (cmd)
    {
        case GET_SECTOR_COUNT:
            *(uint32_t *)buff = DISK_SIZE / SECTOR_SIZE;
            return RES_OK;
            break;
        case GET_SECTOR_SIZE:
            *(uint32_t *)buff = SECTOR_SIZE;
            return RES_OK;
            break;
        case CTRL_SYNC:
            return RES_OK;
            break;
        default:
            break;
    }
    return RES_PARERR;
}
#endif /* RAM_DISK_ENABLE */
