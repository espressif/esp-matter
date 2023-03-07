/*
 * Copyright (c) 2018-2020 Arm Limited. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Driver_Flash.h"
#include "gfc100_eflash_drv.h"
#include "RTE_Device.h"
#include "device_cfg.h"
#include "device_definition.h"
#include "system_core_init.h"
#include "cmsis_driver_config.h"

/**
 * \file Driver_GFC100_EFlash.c
 *
 * \brief CMSIS Flash driver for GFC100 flash controller
 */

#ifndef ARG_UNUSED
#define ARG_UNUSED(arg)  (void)arg
#endif

#ifndef ARG_NOT_USED
#define ARG_NOT_USED    0U
#endif

/* Driver version */
#define ARM_FLASH_DRV_VERSION   ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0)

const ARM_DRIVER_VERSION GFC100_EFlash_Driver_Version = {
    ARM_FLASH_API_VERSION,  /* Defined in the CMSIS Flash Driver header file */
    ARM_FLASH_DRV_VERSION
};

/**
 * Event ready values for ARM_FLASH_CAPABILITIES::event_ready
 * \ref ARM_FLASH_CAPABILITIES
 */
enum {
    EVENT_READY_NOT_AVAILABLE = 0u,
    EVENT_READY_AVAILABLE
};

/**
 * Data width values for ARM_FLASH_CAPABILITIES::data_width
 * \ref ARM_FLASH_CAPABILITIES
 */
 enum {
    DATA_WIDTH_8BIT   = 0u,
    DATA_WIDTH_16BIT,
    DATA_WIDTH_32BIT
};

/**
 * Erase chip values for ARM_FLASH_CAPABILITIES::erase_chip
 * \ref ARM_FLASH_CAPABILITIES
 */
enum {
    CHIP_ERASE_NOT_SUPPORTED = 0u,
    CHIP_ERASE_SUPPORTED
};

/** Driver Capabilities */
static const ARM_FLASH_CAPABILITIES DriverCapabilities = {
    .event_ready = EVENT_READY_NOT_AVAILABLE,
    .data_width  = DATA_WIDTH_32BIT,
    .erase_chip  = CHIP_ERASE_SUPPORTED
};

/**
 * \brief Flash busy values flash status  \ref ARM_FLASH_STATUS
 */
enum {
    DRIVER_STATUS_IDLE = 0u,
    DRIVER_STATUS_BUSY
};

/**
 * \brief Flash error values flash status  \ref ARM_FLASH_STATUS
 */
enum {
    DRIVER_STATUS_NO_ERROR = 0u,
    DRIVER_STATUS_ERROR
};

/**
 * \brief Arm Flash device structure.
 */
typedef struct _FLASHx_Resources {
    struct gfc100_eflash_dev_t* dev;  /*!< FLASH memory device structure */
    ARM_FLASH_INFO *data;             /*!< FLASH memory device data */
    ARM_FLASH_STATUS status;
} ARM_FLASHx_Resources;

static ARM_DRIVER_VERSION ARM_Flash_GetVersion(void)
{
    return GFC100_EFlash_Driver_Version;
}

static ARM_FLASH_CAPABILITIES ARM_Flash_GetCapabilities(void)
{
    return DriverCapabilities;
}

static int32_t ARM_Flashx_Initialize(ARM_FLASHx_Resources *ARM_FLASHx_DEV,
                                     ARM_Flash_SignalEvent_t cb_event)
{
    ARG_UNUSED(cb_event);

    uint32_t page_size = gfc100_get_eflash_page_size(ARM_FLASHx_DEV->dev);
    uint32_t flash_size = gfc100_get_eflash_size(ARM_FLASHx_DEV->dev);

    /* Validate hardcoded parameters of the flash */
    if ((ARM_FLASHx_DEV->data->page_size != page_size) ||
        (ARM_FLASHx_DEV->data->sector_size != page_size) ||
        (ARM_FLASHx_DEV->data->sector_count != (flash_size / page_size))) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    gfc100_eflash_init(ARM_FLASHx_DEV->dev, SystemCoreClock);

    return ARM_DRIVER_OK;
}

static int32_t ARM_Flashx_Uninitialize(ARM_FLASHx_Resources *ARM_FLASHx_DEV)
{
    ARG_UNUSED(ARM_FLASHx_DEV);

    /* Nothing to do */
    return ARM_DRIVER_OK;
}

static int32_t ARM_Flashx_PowerControl(ARM_FLASHx_Resources *ARM_FLASHx_DEV,
                                       ARM_POWER_STATE state)
{
    ARG_UNUSED(ARM_FLASHx_DEV);

    switch(state) {
    case ARM_POWER_FULL:
        /* Nothing to do */
        return ARM_DRIVER_OK;
    case ARM_POWER_OFF:
    case ARM_POWER_LOW:
        return ARM_DRIVER_ERROR_UNSUPPORTED;
    default:
        return ARM_DRIVER_ERROR_PARAMETER;
    }
}

static int32_t ARM_Flashx_ReadData(ARM_FLASHx_Resources *ARM_FLASHx_DEV,
                                   uint32_t addr, void *data, uint32_t cnt)
{
    enum gfc100_error_t err = GFC100_ERROR_NONE;
    uint32_t flash_size = gfc100_get_eflash_size(ARM_FLASHx_DEV->dev);

    ARM_FLASHx_DEV->status.error = DRIVER_STATUS_NO_ERROR;

    /* Check if range is valid */
    if ((addr + cnt) > flash_size) {
        ARM_FLASHx_DEV->status.error = DRIVER_STATUS_ERROR;
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    ARM_FLASHx_DEV->status.busy = DRIVER_STATUS_BUSY;

    err = gfc100_eflash_read(ARM_FLASHx_DEV->dev, addr, data, &cnt);

    ARM_FLASHx_DEV->status.busy = DRIVER_STATUS_IDLE;

    if(err != GFC100_ERROR_NONE) {
        ARM_FLASHx_DEV->status.error = DRIVER_STATUS_ERROR;
        return ARM_DRIVER_ERROR;
    }

    return ARM_DRIVER_OK;
}

static int32_t ARM_Flashx_ProgramData(ARM_FLASHx_Resources *ARM_FLASHx_DEV,
                                      uint32_t addr, const void *data,
                                      uint32_t cnt)
{
    enum gfc100_error_t err = GFC100_ERROR_NONE;

    ARM_FLASHx_DEV->status.error = DRIVER_STATUS_NO_ERROR;
    ARM_FLASHx_DEV->status.busy = DRIVER_STATUS_BUSY;

    /* Note: There is a significantly faster way to write to the flash using
     * gfc100_eflash_row_write. It has the disadvantage that all IRQs have
     * to be disabled, because the implementation is sensitive to timing.
     * For applications where timing can be guaranteed this can be replaced
     * with gfc100_eflash_row_write. For generic use, simple write is used here.
     */
    err = gfc100_eflash_write(ARM_FLASHx_DEV->dev, addr, data, &cnt);

    ARM_FLASHx_DEV->status.busy = DRIVER_STATUS_IDLE;

    if(err != GFC100_ERROR_NONE) {
        ARM_FLASHx_DEV->status.error = DRIVER_STATUS_ERROR;
        if ((err == GFC100_ERROR_OUT_OF_RANGE) ||
            (err == GFC100_ERROR_UNALIGNED_PARAM)) {
            /* The native driver checks aligment and range */
            return ARM_DRIVER_ERROR_PARAMETER;
        }
        return ARM_DRIVER_ERROR;
    }

    return ARM_DRIVER_OK;
}

static int32_t ARM_Flashx_EraseSector(ARM_FLASHx_Resources *ARM_FLASHx_DEV,
                                      uint32_t addr)
{
    enum gfc100_error_t err = GFC100_ERROR_NONE;

    ARM_FLASHx_DEV->status.error = DRIVER_STATUS_NO_ERROR;
    ARM_FLASHx_DEV->status.busy = DRIVER_STATUS_BUSY;

    /* The erase function checks whether the address is within the valid flash
     * address range, and the HW will align the address to page boundary if
     * it is not aligned.
     */
    err = gfc100_eflash_erase(ARM_FLASHx_DEV->dev, addr, GFC100_ERASE_PAGE);

    ARM_FLASHx_DEV->status.busy = DRIVER_STATUS_IDLE;

    if (err != GFC100_ERROR_NONE) {
        ARM_FLASHx_DEV->status.error = DRIVER_STATUS_ERROR;
        if (err == GFC100_ERROR_OUT_OF_RANGE) {
            return ARM_DRIVER_ERROR_PARAMETER;
        }
        return ARM_DRIVER_ERROR;
    }

    return ARM_DRIVER_OK;
}

static int32_t ARM_Flashx_EraseChip(ARM_FLASHx_Resources *ARM_FLASHx_DEV)
{
    enum gfc100_error_t err = GFC100_ERROR_NONE;

    ARM_FLASHx_DEV->status.error = DRIVER_STATUS_NO_ERROR;
    ARM_FLASHx_DEV->status.busy = DRIVER_STATUS_BUSY;

    /* The erase function checks whether the address is aligned with
     * the sector or subsector and checks the Flash memory boundaries.
     */
    err = gfc100_eflash_erase(ARM_FLASHx_DEV->dev, ARG_NOT_USED,
                              GFC100_MASS_ERASE_MAIN_AREA);

    ARM_FLASHx_DEV->status.busy = DRIVER_STATUS_IDLE;

    if(err != GFC100_ERROR_NONE) {
        ARM_FLASHx_DEV->status.error = DRIVER_STATUS_ERROR;
        return ARM_DRIVER_ERROR;
    }

    return ARM_DRIVER_OK;
}

static ARM_FLASH_STATUS
ARM_Flashx_GetStatus(ARM_FLASHx_Resources *ARM_FLASHx_DEV)
{
    return ARM_FLASHx_DEV->status;
}

static ARM_FLASH_INFO * ARM_Flashx_GetInfo(ARM_FLASHx_Resources *ARM_FLASHx_DEV)
{
    return ARM_FLASHx_DEV->data;
}

#if (RTE_EFLASH0)
static ARM_FLASH_INFO ARM_FLASH0_DEV_DATA = {
    .sector_info    = NULL,     /* Uniform sector layout */
    .sector_count   = (0x200000u / 0x4000u), /* 2MB (flash size) / 16kB */
    .sector_size    = 0x4000u,  /* 16kB - as there are no sectors the page size
                                 *        size is used here
                                 */
    .page_size      = 0x4000u,  /* 16kB */
    .program_unit   = 4u,       /* Minimum write size in bytes */
    .erased_value   = 0xFF
};

static ARM_FLASHx_Resources ARM_FLASH0_DEV = {
    .dev    = &EFLASH0_DEV,
    .data   = &(ARM_FLASH0_DEV_DATA),
    .status = {
       .busy     = DRIVER_STATUS_IDLE,
       .error    = DRIVER_STATUS_NO_ERROR,
       .reserved = 0,
     },
};

static ARM_DRIVER_VERSION ARM_Flash0_GetVersion(void)
{
    return ARM_Flash_GetVersion();
}

static ARM_FLASH_CAPABILITIES ARM_Flash0_GetCapabilities(void)
{
    return ARM_Flash_GetCapabilities();
}

static int32_t ARM_Flash0_Initialize(ARM_Flash_SignalEvent_t cb_event)
{
    return ARM_Flashx_Initialize(&ARM_FLASH0_DEV, cb_event);
}

static int32_t ARM_Flash0_Uninitialize(void)
{
    return ARM_Flashx_Uninitialize(&ARM_FLASH0_DEV);
}

static int32_t ARM_Flash0_PowerControl(ARM_POWER_STATE state)
{
    return ARM_Flashx_PowerControl(&ARM_FLASH0_DEV, state);
}

static int32_t ARM_Flash0_ReadData(uint32_t addr, void *data, uint32_t cnt)
{
    return ARM_Flashx_ReadData(&ARM_FLASH0_DEV, addr, data, cnt);
}

static int32_t ARM_Flash0_ProgramData(uint32_t addr, const void *data,
                                      uint32_t cnt)
{
    return ARM_Flashx_ProgramData(&ARM_FLASH0_DEV, addr, data, cnt);
}

static int32_t ARM_Flash0_EraseSector(uint32_t addr)
{
    return ARM_Flashx_EraseSector(&ARM_FLASH0_DEV, addr);
}

static int32_t ARM_Flash0_EraseChip(void)
{
    return ARM_Flashx_EraseChip(&ARM_FLASH0_DEV);
}

static ARM_FLASH_STATUS ARM_Flash0_GetStatus(void)
{
    return ARM_Flashx_GetStatus(&ARM_FLASH0_DEV);
}

static ARM_FLASH_INFO * ARM_Flash0_GetInfo(void)
{
    return ARM_Flashx_GetInfo(&ARM_FLASH0_DEV);
}

ARM_DRIVER_FLASH Driver_EFLASH0 = {
    ARM_Flash0_GetVersion,
    ARM_Flash0_GetCapabilities,
    ARM_Flash0_Initialize,
    ARM_Flash0_Uninitialize,
    ARM_Flash0_PowerControl,
    ARM_Flash0_ReadData,
    ARM_Flash0_ProgramData,
    ARM_Flash0_EraseSector,
    ARM_Flash0_EraseChip,
    ARM_Flash0_GetStatus,
    ARM_Flash0_GetInfo
};
#endif /* RTE_EFLASH0 */

#if (RTE_EFLASH1)
static ARM_FLASH_INFO ARM_FLASH1_DEV_DATA = {
    .sector_info    = NULL,     /* Uniform sector layout */
    .sector_count   = (0x200000u / 0x4000u), /* 2MB (flash size) / 16kB */
    .sector_size    = 0x4000u,  /* 16kB - as there are no sectors the page size
                                 *        size is used here
                                 */
    .page_size      = 0x4000u,  /* 16kB */
    .program_unit   = 4u,       /* Minimum write size in bytes */
    .erased_value   = 0xFF
};

static ARM_FLASHx_Resources ARM_FLASH1_DEV = {
    .dev    = &EFLASH1_DEV,
    .data   = &ARM_FLASH1_DEV_DATA,
    .status = {
       .busy     = DRIVER_STATUS_IDLE,
       .error    = DRIVER_STATUS_NO_ERROR,
       .reserved = 0,
     },
};

static ARM_DRIVER_VERSION ARM_Flash1_GetVersion(void)
{
    return ARM_Flash_GetVersion();
}

static ARM_FLASH_CAPABILITIES ARM_Flash1_GetCapabilities(void)
{
    return ARM_Flash_GetCapabilities();
}

static int32_t ARM_Flash1_Initialize(ARM_Flash_SignalEvent_t cb_event)
{
    return ARM_Flashx_Initialize(&ARM_FLASH1_DEV, cb_event);
}

static int32_t ARM_Flash1_Uninitialize(void)
{
    return ARM_Flashx_Uninitialize(&ARM_FLASH1_DEV);
}

static int32_t ARM_Flash1_PowerControl(ARM_POWER_STATE state)
{
    return ARM_Flashx_PowerControl(&ARM_FLASH1_DEV, state);
}

static int32_t ARM_Flash1_ReadData(uint32_t addr, void *data, uint32_t cnt)
{
    return ARM_Flashx_ReadData(&ARM_FLASH1_DEV, addr, data, cnt);
}

static int32_t ARM_Flash1_ProgramData(uint32_t addr, const void *data,
                                      uint32_t cnt)
{
    return ARM_Flashx_ProgramData(&ARM_FLASH1_DEV, addr, data, cnt);
}

static int32_t ARM_Flash1_EraseSector(uint32_t addr)
{
    return ARM_Flashx_EraseSector(&ARM_FLASH1_DEV, addr);
}

static int32_t ARM_Flash1_EraseChip(void)
{
    return ARM_Flashx_EraseChip(&ARM_FLASH1_DEV);
}

static ARM_FLASH_STATUS ARM_Flash1_GetStatus(void)
{
    return (ARM_Flashx_GetStatus(&ARM_FLASH1_DEV));
}

static ARM_FLASH_INFO * ARM_Flash1_GetInfo(void)
{
    return ARM_Flashx_GetInfo(&ARM_FLASH1_DEV);
}

ARM_DRIVER_FLASH Driver_EFLASH1 = {
    ARM_Flash1_GetVersion,
    ARM_Flash1_GetCapabilities,
    ARM_Flash1_Initialize,
    ARM_Flash1_Uninitialize,
    ARM_Flash1_PowerControl,
    ARM_Flash1_ReadData,
    ARM_Flash1_ProgramData,
    ARM_Flash1_EraseSector,
    ARM_Flash1_EraseChip,
    ARM_Flash1_GetStatus,
    ARM_Flash1_GetInfo
};
#endif /* RTE_EFLASH1 */
