/*
 * Copyright (c) 2013-2019 ARM Limited. All rights reserved.
 * Copyright (c) 2019, Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* This is a modified copy of the ref_twincpu version at
 * platform/ext/target/ref_twincpu/CMSIS_Driver/Driver_Flash.c
 */

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "Driver_Flash.h"
#include "RTE_Device.h"
#include "flash_layout.h"
#include "region_defs.h"

#include "cycfg.h"
#include "cy_device.h"
#include "cy_flash.h"

#ifndef ARG_UNUSED
#define ARG_UNUSED(arg)  ((void)arg)
#endif

/* Driver version */
#define ARM_FLASH_DRV_VERSION      ARM_DRIVER_VERSION_MAJOR_MINOR(1, 1)
#define ARM_FLASH_DRV_ERASE_VALUE  0xFF

#define FLASH0_BASE                    CY_FLASH_BASE        // User Flash start address for both CM0+ & CM4
#define FLASH0_SIZE                    CY_FLASH_SIZE        // 1 MB
#define FLASH0_SECTOR_SIZE             CY_FLASH_SIZEOF_ROW  // 512 B
#define FLASH0_PAGE_SIZE               CY_FLASH_SIZEOF_ROW  // 512 B

struct arm_flash_dev_t {
    const uint32_t memory_base;   /*!< FLASH memory base address */
    ARM_FLASH_INFO *data;         /*!< FLASH data */
};

/* Flash Status */
static ARM_FLASH_STATUS FlashStatus = {0, 0, 0};

/* Driver Version */
static const ARM_DRIVER_VERSION DriverVersion = {
    ARM_FLASH_API_VERSION,
    ARM_FLASH_DRV_VERSION
};

/* Driver Capabilities */
static const ARM_FLASH_CAPABILITIES DriverCapabilities = {
    0, /* event_ready */
    0, /* data_width = 0:8-bit, 1:16-bit, 2:32-bit */
    1  /* erase_chip */
};


#if (RTE_FLASH0)

static ARM_FLASH_INFO ARM_FLASH0_DEV_DATA = {
    .sector_info  = NULL,                  /* Uniform sector layout */
    .sector_count = FLASH0_SIZE / FLASH0_SECTOR_SIZE,
    .sector_size  = FLASH0_SECTOR_SIZE,
    .page_size    = FLASH0_PAGE_SIZE,
    .program_unit = SST_FLASH_PROGRAM_UNIT,
    .erased_value = ARM_FLASH_DRV_ERASE_VALUE
};

static struct arm_flash_dev_t ARM_FLASH0_DEV = {
    .memory_base = FLASH0_BASE,
    .data        = &(ARM_FLASH0_DEV_DATA)
};

struct arm_flash_dev_t *FLASH0_DEV = &ARM_FLASH0_DEV;

/*
 * Functions
 */

static ARM_DRIVER_VERSION ARM_Flash_GetVersion(void)
{
    return DriverVersion;
}

static ARM_FLASH_CAPABILITIES ARM_Flash_GetCapabilities(void)
{
    return DriverCapabilities;
}

static int32_t ARM_Flash_Initialize(ARM_Flash_SignalEvent_t cb_event)
{
    ARG_UNUSED(cb_event);

    return ARM_DRIVER_OK;
}

static int32_t ARM_Flash_Uninitialize(void)
{
    /* Nothing to be done */
    return ARM_DRIVER_OK;
}

static int32_t ARM_Flash_PowerControl(ARM_POWER_STATE state)
{
    switch (state) {
        case ARM_POWER_FULL:
            /* Nothing to be done */
            return ARM_DRIVER_OK;
            break;

        case ARM_POWER_OFF:
        case ARM_POWER_LOW:
        default:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
    }
}

static int32_t ARM_Flash_ReadData(uint32_t addr, void *data, uint32_t cnt)
{
    if ( (data == NULL) || (cnt == 0) ) {
        return ARM_DRIVER_OK;
    }

    // Wraparound check (before adding FLASH0_DEV->memory_base + addr)
    if (FLASH0_DEV->memory_base >= UINT32_MAX - addr) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    uint32_t start_addr = FLASH0_DEV->memory_base + addr;

    if (start_addr >= FLASH0_DEV->memory_base + FLASH0_SIZE) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    // Wraparound check (before adding start_addr + cnt)
    if (start_addr >= UINT32_MAX - cnt) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    if ( (start_addr + cnt < FLASH0_DEV->memory_base) ||
         (start_addr + cnt > FLASH0_DEV->memory_base + FLASH0_SIZE)
       ) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    // Using memcpy for reading
    memcpy(data, (void *)start_addr, cnt);

    return ARM_DRIVER_OK;
}

static int32_t ARM_Flash_ProgramData(uint32_t addr, const void *data,
                                     uint32_t cnt)
{
    static uint8_t prog_buf[CY_FLASH_SIZEOF_ROW];
    uint8_t *data_ptr = (uint8_t *) data;
    uint32_t address = FLASH0_DEV->memory_base + addr;
    cy_en_flashdrv_status_t cy_status = CY_FLASH_DRV_ERR_UNC;

    // Address checks can be omitted here as they are done by Cypress API

    if ( (data == NULL) || (cnt == 0) ) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    // Make sure cnt argument is aligned to program_unit size
    if (cnt % FLASH0_DEV->data->program_unit) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    // Wraparound check (before adding FLASH0_DEV->memory_base + addr)
    if (FLASH0_DEV->memory_base >= UINT32_MAX - addr) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    while (cnt)
    {
        uint32_t offset = address % CY_FLASH_SIZEOF_ROW;
        uint32_t chunk_size;
        if (offset + cnt > CY_FLASH_SIZEOF_ROW) {
            chunk_size = CY_FLASH_SIZEOF_ROW - offset;
        } else {
            chunk_size = cnt;
        }
        uint32_t row_address = address / CY_FLASH_SIZEOF_ROW * CY_FLASH_SIZEOF_ROW;
        memcpy(prog_buf, (const void *)row_address, CY_FLASH_SIZEOF_ROW);
        memcpy(prog_buf + offset, data_ptr, chunk_size);

        cy_status = Cy_Flash_ProgramRow(row_address, (const uint32_t *)prog_buf);
        if (cy_status != CY_FLASH_DRV_SUCCESS) {
            break;
        }
        data_ptr += chunk_size;
        address += chunk_size;
        cnt -= chunk_size;
    }

    switch (cy_status)
    {
        case CY_FLASH_DRV_SUCCESS:
            return ARM_DRIVER_OK;

        case CY_FLASH_DRV_IPC_BUSY:
            return ARM_DRIVER_ERROR_BUSY;

        case CY_FLASH_DRV_INVALID_INPUT_PARAMETERS:
            return ARM_DRIVER_ERROR_PARAMETER;

        default:
            return ARM_DRIVER_ERROR;
    }
}

static int32_t ARM_Flash_EraseSector(uint32_t addr)
{
    // Wraparound check (before adding FLASH0_DEV->memory_base + addr)
    if (FLASH0_DEV->memory_base >= UINT32_MAX - addr) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    uint32_t start_addr = FLASH0_DEV->memory_base + addr;

    // Address checks can be omitted here as they are done by Cypress API

    cy_en_flashdrv_status_t cy_status = Cy_Flash_EraseRow(start_addr);

    switch (cy_status)
    {
        case CY_FLASH_DRV_SUCCESS:
            return ARM_DRIVER_OK;

        case CY_FLASH_DRV_IPC_BUSY:
            return ARM_DRIVER_ERROR_BUSY;

        case CY_FLASH_DRV_INVALID_INPUT_PARAMETERS:
            return ARM_DRIVER_ERROR_PARAMETER;

        default:
            return ARM_DRIVER_ERROR;
    }
}

static int32_t ARM_Flash_EraseChip(void)
{
    uint32_t addr = FLASH0_DEV->memory_base;

    cy_en_flashdrv_status_t cy_status = CY_FLASH_DRV_ERR_UNC;


    if (DriverCapabilities.erase_chip != 1) {
        return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    for (uint32_t i = 0; i < FLASH0_DEV->data->sector_count; i++) {

        cy_status = Cy_Flash_EraseRow(addr);

        if (cy_status != CY_FLASH_DRV_SUCCESS) {
            break;
        }

        addr += FLASH0_DEV->data->sector_size;
    }

    switch (cy_status)
    {
        case CY_FLASH_DRV_SUCCESS:
            return ARM_DRIVER_OK;

        case CY_FLASH_DRV_IPC_BUSY:
            return ARM_DRIVER_ERROR_BUSY;

        case CY_FLASH_DRV_INVALID_INPUT_PARAMETERS:
            return ARM_DRIVER_ERROR_PARAMETER;

        default:
            return ARM_DRIVER_ERROR;
    }
}

static ARM_FLASH_STATUS ARM_Flash_GetStatus(void)
{
    return FlashStatus;
}

static ARM_FLASH_INFO * ARM_Flash_GetInfo(void)
{
    return FLASH0_DEV->data;
}

ARM_DRIVER_FLASH Driver_FLASH0 = {
    ARM_Flash_GetVersion,
    ARM_Flash_GetCapabilities,
    ARM_Flash_Initialize,
    ARM_Flash_Uninitialize,
    ARM_Flash_PowerControl,
    ARM_Flash_ReadData,
    ARM_Flash_ProgramData,
    ARM_Flash_EraseSector,
    ARM_Flash_EraseChip,
    ARM_Flash_GetStatus,
    ARM_Flash_GetInfo
};

#endif /* RTE_FLASH0 */
