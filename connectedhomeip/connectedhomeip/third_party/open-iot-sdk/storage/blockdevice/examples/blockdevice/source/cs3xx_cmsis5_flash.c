/*
 * Copyright (c) 2022 Arm Limited. All rights reserved.
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

#include <stddef.h>
#include <stdint.h>

#include "iotsdk/Driver_Flash.h"

#include "flash_cs300.h"

#include "hal/flash_api.h"

#include "partition/flash_layout.h"

#define FLASH_SECTOR_COUNT ((uint32_t)FLASH_TOTAL_SIZE / FLASH_AREA_IMAGE_SECTOR_SIZE)
#define FLASH_SECTOR_SIZE  ((uint32_t)FLASH_AREA_IMAGE_SECTOR_SIZE)
#define FLASH_PAGE_SIZE    ((uint32_t)256)
#define FLASH_PROGRAM_UNIT ((uint32_t)TFM_HAL_FLASH_PROGRAM_UNIT)
#define FLASH_ERASED_VALUE ((uint8_t)0xFF)

#define ARM_FLASH_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0)

// CMSIS Driver_Flash functions
static ARM_DRIVER_VERSION get_version(void);
static ARM_FLASH_CAPABILITIES get_capabilities(void);
static int32_t init(ARM_Flash_SignalEvent_t cb_event);
static int32_t deinit(void);
static int32_t control_power(ARM_POWER_STATE state);
static int32_t read_data(uint32_t addr, void *data, uint32_t cnt);
static int32_t program_data(uint32_t addr, const void *data, uint32_t cnt);
static int32_t erase_sector(uint32_t addr);
static ARM_FLASH_STATUS get_status(void);
static ARM_FLASH_INFO *get_info(void);

static ARM_FLASH_INFO gs_flash_info = {.sector_info = NULL,
                                       .sector_count = FLASH_SECTOR_COUNT,
                                       .sector_size = FLASH_SECTOR_SIZE,
                                       .page_size = FLASH_PAGE_SIZE,
                                       .program_unit = FLASH_PROGRAM_UNIT,
                                       .erased_value = FLASH_ERASED_VALUE,
                                       .reserved = {[0] = 0U, [1] = 0U, [2] = 0U}};
static ARM_FLASH_STATUS gs_flash_status = {.busy = 0, .error = 0, .reserved = 0};
static const ARM_DRIVER_VERSION gs_driver_version = {.api = ARM_FLASH_API_VERSION, .drv = ARM_FLASH_DRV_VERSION};
static const ARM_FLASH_CAPABILITIES gs_driver_capabilities = {
    .event_ready = 0, .data_width = 0, .erase_chip = 0, .reserved = 0};
static mdh_flash_t *gs_device = NULL;

ARM_DRIVER_FLASH Driver_Flash0 = {.GetVersion = get_version,
                                  .GetCapabilities = get_capabilities,
                                  .Initialize = init,
                                  .Uninitialize = deinit,
                                  .PowerControl = control_power,
                                  .ReadData = read_data,
                                  .ProgramData = program_data,
                                  .EraseSector = erase_sector,
                                  .EraseChip = NULL,
                                  .GetStatus = get_status,
                                  .GetInfo = get_info};

ARM_DRIVER_FLASH *get_example_block_device(void)
{
    return &Driver_Flash0;
}

size_t get_flash_size(void)
{
    return gs_flash_info.sector_count * gs_flash_info.sector_size;
}

int32_t get_erase_size(uint32_t addr, size_t *erase_size)
{
    uint32_t block_device_base_addr = mdh_flash_get_start_address(gs_device);
    uint32_t physical_addr = block_device_base_addr + addr;

    if ((block_device_base_addr <= physical_addr) && (physical_addr < block_device_base_addr + get_flash_size())) {
        *erase_size = gs_flash_info.sector_size;
        return ARM_DRIVER_OK;
    }

    return ARM_DRIVER_ERROR_PARAMETER;
}

static ARM_DRIVER_VERSION get_version(void)
{
    return gs_driver_version;
}

static ARM_FLASH_CAPABILITIES get_capabilities(void)
{
    return gs_driver_capabilities;
}

static int32_t init(ARM_Flash_SignalEvent_t cb_event)
{
    (void)cb_event;

    gs_flash_status.busy = 0U;
    gs_flash_status.error = 0U;

    gs_device = get_ram_drive_instance();

    return (NULL == gs_device) ? ARM_DRIVER_ERROR : ARM_DRIVER_OK;
}

static int32_t deinit(void)
{
    gs_device = NULL;

    return ARM_DRIVER_OK;
}

static int32_t control_power(ARM_POWER_STATE state)
{
    (void)state;

    return ARM_DRIVER_ERROR;
}

static int32_t read_data(uint32_t addr, void *data, uint32_t cnt)
{
    uint32_t block_device_base_addr = mdh_flash_get_start_address(gs_device);

    gs_flash_status.busy = 1U;
    int status = mdh_flash_read(gs_device, block_device_base_addr + addr, (uint8_t *)data, cnt);

    gs_flash_status.error = (0 == status) ? 0U : 1U;
    gs_flash_status.busy = 0U;

    return (gs_flash_status.error) ? ARM_DRIVER_ERROR : ARM_DRIVER_OK;
}

static int32_t program_data(uint32_t addr, const void *data, uint32_t cnt)
{
    uint32_t block_device_base_addr = mdh_flash_get_start_address(gs_device);

    uint32_t curr_addr = addr;
    uint32_t remaining_size = cnt;
    const uint8_t *buffer = (const uint8_t *)data;

    gs_flash_status.busy = 1U;
    gs_flash_status.error = 0U;

    while (remaining_size > 0) {
        // Multiple pages can be programmed at once but cannot cross sector boundaries
        size_t sector_size = SIZE_MAX;
        int32_t status = get_erase_size(curr_addr, &sector_size);
        if (ARM_DRIVER_OK != status) {
            gs_flash_status.busy = 0U;
            gs_flash_status.error = 1U;
            return status;
        }

        const uint32_t sector_end = (curr_addr / sector_size + 1) * sector_size;
        const uint32_t chunk = ((curr_addr + remaining_size) > sector_end) ? (sector_end - curr_addr) : remaining_size;

        status = mdh_flash_program_page(gs_device, block_device_base_addr + curr_addr, (uint8_t *)buffer, chunk);
        if (ARM_DRIVER_OK != status) {
            gs_flash_status.busy = 0U;
            gs_flash_status.error = 1U;
            return status;
        }

        curr_addr += chunk;
        remaining_size -= chunk;
        buffer += chunk;
    }

    gs_flash_status.busy = 0U;

    return ARM_DRIVER_OK;
}

static int32_t erase_sector(uint32_t addr)
{
    gs_flash_status.busy = 1U;
    int status = mdh_flash_erase_sector(gs_device, mdh_flash_get_start_address(gs_device) + addr);

    gs_flash_status.error = (0 == status) ? 0U : 1U;
    gs_flash_status.busy = 0U;

    return (gs_flash_status.error) ? ARM_DRIVER_ERROR : ARM_DRIVER_OK;
}

static ARM_FLASH_STATUS get_status(void)
{
    return gs_flash_status;
}

static ARM_FLASH_INFO *get_info(void)
{
    return &gs_flash_info;
}
