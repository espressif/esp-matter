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
#ifdef SD_DISK_ENABLE

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "fsl_sd_disk.h"

/*******************************************************************************
 * Definitons
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*! @brief Card descriptor */
sd_card_t g_sd;

/*******************************************************************************
 * Code
 ******************************************************************************/
DRESULT sd_disk_write(uint8_t physicalDrive, const uint8_t *buffer, uint32_t sector, uint8_t count)
{
    if (physicalDrive != SDDISK)
    {
        return RES_PARERR;
    }

    if (kStatus_Success != SD_WriteBlocks(&g_sd, buffer, sector, count))
    {
        return RES_ERROR;
    }

    return RES_OK;
}

DRESULT sd_disk_read(uint8_t physicalDrive, uint8_t *buffer, uint32_t sector, uint8_t count)
{
    if (physicalDrive != SDDISK)
    {
        return RES_PARERR;
    }

    if (kStatus_Success != SD_ReadBlocks(&g_sd, buffer, sector, count))
    {
        return RES_ERROR;
    }

    return RES_OK;
}

DRESULT sd_disk_ioctl(uint8_t physicalDrive, uint8_t command, void *buffer)
{
    DRESULT result = RES_OK;

    if (physicalDrive != SDDISK)
    {
        return RES_PARERR;
    }

    switch (command)
    {
        case GET_SECTOR_COUNT:
            if (buffer)
            {
                *(uint32_t *)buffer = g_sd.blockCount;
            }
            else
            {
                result = RES_PARERR;
            }
            break;
        case GET_SECTOR_SIZE:
            if (buffer)
            {
                *(uint32_t *)buffer = g_sd.blockSize;
            }
            else
            {
                result = RES_PARERR;
            }
            break;
        case GET_BLOCK_SIZE:
            if (buffer)
            {
                *(uint32_t *)buffer = g_sd.csd.eraseSectorSize;
            }
            else
            {
                result = RES_PARERR;
            }
            break;
        case CTRL_SYNC:
            result = RES_OK;
            break;
        default:
            result = RES_PARERR;
            break;
    }

    return result;
}

DSTATUS sd_disk_status(uint8_t physicalDrive)
{
    if (physicalDrive != SDDISK)
    {
        return STA_NOINIT;
    }

    return 0;
}

DSTATUS sd_disk_initialize(uint8_t physicalDrive)
{
    if (physicalDrive != SDDISK)
    {
        return STA_NOINIT;
    }

    if(g_sd.isHostReady)
    {
        /* reset host */
        SD_HostReset(&(g_sd.host));
    }
    else
    {
        return STA_NOINIT;
    }

    if (kStatus_Success != SD_CardInit(&g_sd))
    {
        SD_CardDeinit(&g_sd);
        memset(&g_sd, 0U, sizeof(g_sd));
        return STA_NOINIT;
    }

    return 0;
}
#endif /* SD_DISK_ENABLE */
