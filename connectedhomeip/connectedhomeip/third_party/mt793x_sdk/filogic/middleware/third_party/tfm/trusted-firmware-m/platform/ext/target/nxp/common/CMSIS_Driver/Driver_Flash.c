/*
 * Copyright (c) 2013-2018 ARM Limited. All rights reserved.
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
 *
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "Driver_Flash.h"
#include "platform_base_address.h"
#include "flash_layout.h"
#include "fsl_iap.h"
#include "log/tfm_log.h"

#ifndef ARG_UNUSED
#define ARG_UNUSED(arg)  ((void)arg)
#endif

/* Below CMDs apply to both C040HDATFC and C040HDFC flash */
#define FLASH_CMD_INIT 0
#define FLASH_CMD_READ_SINGLE_WORD 3

/* INT_STATUS - Interrupt status bits - taken from LPC5588_cm33_core0.h */

#define FLASH_INT_STATUS_OVL_MASK                (0x10U)

/* Alignment macros - taken from bootloader_common.h */
#ifndef ALIGN_DOWN
#define ALIGN_DOWN(x, a) ((x) & -(a))
#endif

/* Flash property defines */

/* Mask the number of bits required to select the 32-bit data word (DATAW) from the flash line */
#define FLASH_DATAW_IDX_MAX 3    /* Max DATAW index, 3 for a 128-bit flash line, 7 for 256-bit. */

#define FLASH_READMODE_REG          (FLASH->DATAW[0])
#define FLASH_READMODE_ECC_MASK     (0x4U)
#define FLASH_READMODE_ECC_SHIFT    (2U)
#define FLASH_READMODE_ECC(x)       (((uint32_t)(((uint32_t)(x)) << FLASH_READMODE_ECC_SHIFT)) & FLASH_READMODE_ECC_MASK)
#define FLASH_READMODE_MARGIN_MASK  (0xC00U)
#define FLASH_READMODE_MARGIN_SHIFT (10U)
#define FLASH_READMODE_MARGIN(x)    (((uint32_t)(((uint32_t)(x)) << FLASH_READMODE_MARGIN_SHIFT)) & FLASH_READMODE_MARGIN_MASK)
#define FLASH_READMODE_DMACC_MASK   (0x8000U)
#define FLASH_READMODE_DMACC_SHIFT  (15U)
#define FLASH_READMODE_DMACC(x)     (((uint32_t)(((uint32_t)(x)) << FLASH_READMODE_DMACC_SHIFT)) & FLASH_READMODE_DMACC_MASK)

/* Driver version */
#define ARM_FLASH_DRV_VERSION    ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0)

/* ARM FLASH device structure */
struct arm_flash_dev_t {
    const uint32_t memory_base;   /*!< FLASH memory base address */
    ARM_FLASH_INFO *data;         /*!< FLASH data */
    flash_config_t flashInstance; /*!< FLASH config*/
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
    2, /* data_width = 0:8-bit, 1:16-bit, 2:32-bit */
    1  /* erase_chip */
};

static ARM_FLASH_INFO ARM_FLASH0_DEV_DATA = {
    .sector_info  = NULL,                  /* Uniform sector layout */
    .sector_count = FLASH0_SIZE / FLASH0_PAGE_SIZE,
    .sector_size  = FLASH0_PAGE_SIZE,
    .page_size    = FLASH0_PAGE_SIZE,
    .program_unit = FLASH0_PAGE_SIZE,
    .erased_value = 0x00};

static struct arm_flash_dev_t ARM_FLASH0_DEV = {
#if (__DOMAIN_NS == 1)
    .memory_base = FLASH0_BASE_NS,
#else
    .memory_base = FLASH0_BASE_S,
#endif /* __DOMAIN_NS == 1 */
    .data        = &(ARM_FLASH0_DEV_DATA)};

struct arm_flash_dev_t *FLASH0_DEV = &ARM_FLASH0_DEV;

/* Prototypes */

/* Internal function Flash command sequence. Called by driver APIs only*/
static status_t flash_command_sequence(flash_config_t *config);

//static status_t FLASH_ReadInit(flash_config_t *config);
static status_t FLASH_ReadData(flash_config_t *config, uint32_t start, uint8_t *dest, uint32_t lengthInBytes);
static status_t FLASH_ReadSingleWord(flash_config_t *config, uint32_t start, uint32_t *readbackData);

static bool is_range_valid(struct arm_flash_dev_t *flash_dev,
                           uint32_t offset);
static bool is_write_aligned(struct arm_flash_dev_t *flash_dev,
                             uint32_t param);

/* Functions */

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
    status_t status = kStatus_Success;

    /* Call initialization from Flash API */
    status = FLASH_Init(&FLASH0_DEV->flashInstance);

    if(status != kStatus_Success){
    	return ARM_DRIVER_ERROR;
    }


    /* Disable Error Detection functionality */
    FLASH0_DEV->flashInstance.modeConfig.readSingleWord.readWithEccOff = 0x1;

#if 0
    /* Initialization of Flash by means of its registers to be able read data*/
    if(FLASH_ReadInit(&FLASH0_DEV->flashInstance) != kStatus_Success){
			return ARM_DRIVER_ERROR;
    }
#endif

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

    case ARM_POWER_OFF:
    case ARM_POWER_LOW:
    default:
        return ARM_DRIVER_ERROR_UNSUPPORTED;
    }
}

static int32_t ARM_Flash_ReadData(uint32_t addr, void *data, uint32_t cnt)
{
    static uint32_t status;

    /* Check Flash memory boundaries */
    status = is_range_valid(FLASH0_DEV, addr + cnt);
    if(status != kStatus_Success) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    /* Read Data */
#if 1
    if(cnt) {
        status  = FLASH_ReadData(&FLASH0_DEV->flashInstance, addr, (uint8_t *)data, cnt);
        if(status != kStatus_Success) {
            return ARM_DRIVER_ERROR;
        }
    }
#else /* Bus fault when reading erased memory */
    (void)memcpy(data, (uint8_t *)addr, cnt);
#endif
    return ARM_DRIVER_OK;
}

static int32_t ARM_Flash_ProgramData(uint32_t addr, const void *data, uint32_t cnt)
{
    static uint32_t status;
    uint32_t failedAddress, failedData;

    /* Check Flash memory boundaries */
    status = is_range_valid(FLASH0_DEV, addr);
    status |= is_write_aligned(FLASH0_DEV, addr);
    status |= is_write_aligned(FLASH0_DEV, cnt);
    if(status != kStatus_Success) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    status = FLASH_Program(&FLASH0_DEV->flashInstance, addr, (uint8_t *)data, cnt);
    if (status != kStatus_Success) {
        return ARM_DRIVER_ERROR;
    }

    status = FLASH_VerifyProgram(&FLASH0_DEV->flashInstance, addr, cnt, (const uint8_t *)data,
				 &failedAddress, &failedData);
    if (status != kStatus_Success) {
        return ARM_DRIVER_ERROR;
    }

    return ARM_DRIVER_OK;
}

static int32_t ARM_Flash_EraseSector(uint32_t addr)
{
    static uint32_t status;

    status = is_range_valid(FLASH0_DEV, addr);
    status |= is_write_aligned(FLASH0_DEV, addr);
    if(status != kStatus_Success) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    status = FLASH_Erase(&FLASH0_DEV->flashInstance, addr, FLASH0_DEV->data->sector_size, kFLASH_ApiEraseKey);
    if (status != kStatus_Success) {
        return ARM_DRIVER_ERROR;
    }

    return ARM_DRIVER_OK;
}

static int32_t ARM_Flash_EraseChip(void)
{
    static uint32_t status;
    uint32_t addr = FLASH0_DEV->memory_base;

    status = FLASH_Erase(&FLASH0_DEV->flashInstance, addr, FLASH_TOTAL_SIZE, kFLASH_ApiEraseKey);
    if (status != kStatus_Success)
        return ARM_DRIVER_ERROR;

    return ARM_DRIVER_OK;
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

#if 0
static status_t FLASH_ReadInit(flash_config_t *config)
{
    status_t status = kStatus_Fail;

	    if (config == NULL)
    {
        return kStatus_FLASH_InvalidArgument;
    }

    /* config->PFlashTotalSize was initialized to 0xA0000 (640kB) by FLASH API.
		   This value needs to be changed to 0x98000 (608kB)*/
    config->PFlashTotalSize = FLASH_TOTAL_SIZE;

    /* Immediately after leaving reset mode, an initialization phase takes place,
       where some memory locations are read, and corresponding volatile locations
       are initialized depending on the value just read. */
    FLASH->INT_CLR_STATUS = FLASH_INT_CLR_STATUS_FAIL_MASK | FLASH_INT_CLR_STATUS_ERR_MASK | FLASH_INT_CLR_STATUS_ECC_ERR_MASK;
    FLASH->CMD = FLASH_CMD_INIT;
    status = flash_command_sequence(config);

    if (kStatus_FLASH_Success != status)
    {
        return status;
    }

    return kStatus_FLASH_Success;
}
#endif

static status_t FLASH_ReadData(flash_config_t *config, uint32_t start, uint8_t *dest, uint32_t lengthInBytes)
{
    status_t status = kStatus_Fail;

    uint32_t readbackData[FLASH_DATAW_IDX_MAX + 1];
    while (lengthInBytes)
    {
        uint32_t alignedStart = ALIGN_DOWN(start, kFLASH_AlignementUnitSingleWordRead);
        status = FLASH_ReadSingleWord(config, alignedStart, readbackData);
        if (status != kStatus_FLASH_Success)
        {
            break;
        }
        for (uint32_t i = 0; i < sizeof(readbackData); i++)
        {
            if ((alignedStart == start) && lengthInBytes)
            {
                *dest = *((uint8_t *)readbackData + i);
                dest++;
                start++;
                lengthInBytes--;
            }
            alignedStart++;
        }
    }

    return status;
}

static status_t FLASH_ReadSingleWord(flash_config_t *config, uint32_t start, uint32_t *readbackData)
{
    status_t status = kStatus_Fail;
    /* uint32_t byteSizes = sizeof(uint32_t) * (FLASH_DATAW_IDX_MAX + 1); */

    if (readbackData == NULL)
    {
        return kStatus_FLASH_InvalidArgument;
    }

    FLASH->INT_CLR_STATUS = FLASH_INT_CLR_STATUS_FAIL_MASK | FLASH_INT_CLR_STATUS_ERR_MASK |
                            FLASH_INT_CLR_STATUS_DONE_MASK | FLASH_INT_CLR_STATUS_ECC_ERR_MASK;

    /* Set start address */
    FLASH->STARTA = start >> 4;

    /* ReadSingleWord notes:
    Flash contains one DMACC word per page. Such words are not readable through
    the read interface. DMACC words are managed internally by the controller in
    order to store a flag (all1), which can be used to verify whether a programming
    operation was prematurely terminated.
    DMACC words are all_0 for an erased page, all_1 for a programmed page */

    /* Set read modes */
    FLASH_READMODE_REG = FLASH_READMODE_ECC(config->modeConfig.readSingleWord.readWithEccOff) |
                         FLASH_READMODE_MARGIN(config->modeConfig.readSingleWord.readMarginLevel) |
                         FLASH_READMODE_DMACC(config->modeConfig.readSingleWord.readDmaccWord);

    /* Calling flash command sequence function to execute the command */
    FLASH->CMD = FLASH_CMD_READ_SINGLE_WORD;
    status = flash_command_sequence(config);

    if (kStatus_FLASH_Success == status)
    {
        for (uint32_t datawIndex = 0; datawIndex <= FLASH_DATAW_IDX_MAX; datawIndex++)
        {
            *readbackData++ = FLASH->DATAW[datawIndex];
        }
    }

    return status;
}

/* This function is used to perform the command write sequence to the flash. */
static status_t flash_command_sequence(flash_config_t *config)
{
    status_t status = kStatus_Fail;
    uint32_t registerValue;

    while (!(FLASH->INT_STATUS & FLASH_INT_STATUS_DONE_MASK))
        ;

    /* Check error bits */
    /* Get flash status register value */
    registerValue = FLASH->INT_STATUS;

    /* Checking access error */
    if (registerValue & FLASH_INT_STATUS_FAIL_MASK)
    {
        status = kStatus_FLASH_CommandFailure;
    }
    else if (registerValue & FLASH_INT_STATUS_ERR_MASK)
    {
        status = kStatus_FLASH_CommandNotSupported;
    }
    else if (registerValue & FLASH_INT_STATUS_ECC_ERR_MASK)
    {
        status = kStatus_FLASH_EccError;
    }
    else if (registerValue & FLASH_INT_STATUS_OVL_MASK)
    {
        status = kStatus_FLASH_RegulationLoss;
    }
    else
    {
        status = kStatus_FLASH_Success;
    }

    return status;
}

/* Check if the Flash memory boundaries are not violated. */
static bool is_range_valid(struct arm_flash_dev_t *flash_dev,
                           uint32_t offset)
{

    uint32_t flash_limit = FLASH_TOTAL_SIZE - 1;

    return (offset > flash_limit) ? (kStatus_Fail) : (kStatus_Success) ;
}

/* Check if the parameter is aligned to program_unit. */
static bool is_write_aligned(struct arm_flash_dev_t *flash_dev,
                             uint32_t param)
{
    return ((param % flash_dev->data->program_unit) != 0) ? (kStatus_Fail) : (kStatus_Success);
}
