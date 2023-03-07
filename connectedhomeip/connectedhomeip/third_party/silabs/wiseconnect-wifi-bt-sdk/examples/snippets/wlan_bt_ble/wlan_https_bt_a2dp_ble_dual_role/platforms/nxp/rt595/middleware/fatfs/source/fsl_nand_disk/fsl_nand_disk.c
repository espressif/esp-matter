/*
 * Copyright 2018-2019 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ffconf.h"
/* This fatfs subcomponent is disabled by default
 * To enable it, define following macro in ffconf.h */
#ifdef NAND_DISK_ENABLE

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "fsl_nand_disk.h"
#include "fsl_nand_flash.h"
#include "map.h"

/*******************************************************************************
 * Definitons
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
typedef struct _dhara_handle
{
    struct dhara_nand d;
    nand_handle_t nandHandle;
    uint8_t page_buf[1U << EXAMPLE_DHARA_NAND_LOG2_PAGE_SIZE];
}dhara_handle_t;
/*******************************************************************************
 * Variables
 ******************************************************************************/

/*! @brief Card descriptor */
struct dhara_map map;
dhara_handle_t dhara_handle;
extern nand_config_t nandConfig;

/*******************************************************************************
 * Code
 ******************************************************************************/
DRESULT nand_disk_write(uint8_t physicalDrive, const uint8_t *buffer, uint32_t sector, uint8_t count)
{
    assert(count == 1U);

    dhara_error_t err;

    if (physicalDrive != NANDDISK)
    {
        return RES_PARERR;
    }

    if (dhara_map_write(&map, sector, buffer, &err) < 0)
    {
        return RES_ERROR;
    }

    return RES_OK;
}

DRESULT nand_disk_read(uint8_t physicalDrive, uint8_t *buffer, uint32_t sector, uint8_t count)
{
    assert(count == 1U);

    dhara_error_t err;

    if (physicalDrive != NANDDISK)
    {
        return RES_PARERR;
    }

    if (dhara_map_read(&map, sector, buffer, &err) < 0)
    {
        return RES_ERROR;
    }

    return RES_OK;
}

DRESULT nand_disk_ioctl(uint8_t physicalDrive, uint8_t command, void *buffer)
{
    DRESULT result = RES_OK;
    DWORD sector;
    dhara_error_t err;

    if (physicalDrive != NANDDISK)
    {
        return RES_PARERR;
    }

    switch (command)
    {
        case GET_SECTOR_COUNT:
            if (buffer)
            {
                *(uint32_t *)buffer = dhara_map_capacity(&map);
            }
            else
            {
                result = RES_PARERR;
            }
            break;
        case GET_SECTOR_SIZE:
            if (buffer)
            {
                *(uint32_t *)buffer = 1 << EXAMPLE_DHARA_NAND_LOG2_PAGE_SIZE;
            }
            else
            {
                result = RES_PARERR;
            }
            break;
        case GET_BLOCK_SIZE:
            if (buffer)
            {
                *(uint32_t *)buffer = 1 << EXAMPLE_DHARA_NAND_LOG2_PAGE_PER_BLOCK;
            }
            else
            {
                result = RES_PARERR;
            }
            break;
        case CTRL_SYNC:
            if(dhara_map_sync(&map, NULL) != 0U)
            {
                result = RES_ERROR;
            }
            else
            {
                result = RES_OK;
            }
            break;
        case CTRL_TRIM:
            for(sector = ((DWORD *)buffer)[0]; sector <= ((DWORD *)buffer)[1]; sector++)
            {
                if(dhara_map_trim(&map, sector, &err) < 0)
                {
                    result = RES_ERROR;
                    break;
                }
            }
            break;

        default:
            result = RES_PARERR;
            break;
    }

    return result;
}

DSTATUS nand_disk_status(uint8_t physicalDrive)
{
    if (physicalDrive != NANDDISK)
    {
        return STA_NOINIT;
    }

    return 0;
}

DSTATUS nand_disk_initialize(uint8_t physicalDrive)
{
    status_t status;

    if (physicalDrive != NANDDISK)
    {
        return STA_NOINIT;
    }

    status = Nand_Flash_Init(&nandConfig, &(dhara_handle.nandHandle));
    if (status != kStatus_Success)
    {
        return STA_NOINIT;
    }

    dhara_handle.d.log2_page_size = EXAMPLE_DHARA_NAND_LOG2_PAGE_SIZE;
    dhara_handle.d.log2_ppb = EXAMPLE_DHARA_NAND_LOG2_PAGE_PER_BLOCK;
    dhara_handle.d.num_blocks = EXAMPLE_DHARA_NAND_BLOCKS_NUMBER;

    dhara_map_init(&map, &(dhara_handle.d), dhara_handle.page_buf, EXAMPLE_DHARA_GARBAGE_COLLECTION_RATIO);
    dhara_map_resume(&map, NULL);

    return 0;
}
#endif /* NAND_DISK_ENABLE */
