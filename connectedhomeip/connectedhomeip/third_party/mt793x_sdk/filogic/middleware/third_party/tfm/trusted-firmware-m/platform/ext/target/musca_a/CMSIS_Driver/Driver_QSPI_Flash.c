/*
 * Copyright (c) 2013-2020 Arm Limited. All rights reserved.
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

#include <stdbool.h>
#include "cmsis_driver_config.h"
#include "RTE_Device.h"
#include "flash_layout.h"


#ifndef ARG_UNUSED
#define ARG_UNUSED(arg)  ((void)arg)
#endif

/* Driver version */
#define ARM_FLASH_DRV_VERSION   ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0)

static const ARM_DRIVER_VERSION DriverVersion = {
    ARM_FLASH_API_VERSION,  /* Defined in the CMSIS Flash Driver header file */
    ARM_FLASH_DRV_VERSION
};

/**
 * \brief Flash driver capability macro definitions \ref ARM_FLASH_CAPABILITIES
 */
/* Flash Ready event generation capability values */
#define EVENT_READY_NOT_AVAILABLE   (0u)
#define EVENT_READY_AVAILABLE       (1u)
/* Data access size values */
#define DATA_WIDTH_8BIT             (0u)
#define DATA_WIDTH_16BIT            (1u)
#define DATA_WIDTH_32BIT            (2u)
/* Chip erase capability values */
#define CHIP_ERASE_NOT_SUPPORTED    (0u)
#define CHIP_ERASE_SUPPORTED        (1u)

/* Driver Capabilities */
static const ARM_FLASH_CAPABILITIES DriverCapabilities = {
    EVENT_READY_NOT_AVAILABLE,
    DATA_WIDTH_32BIT,
    CHIP_ERASE_SUPPORTED
};

/**
 * \brief Flash status macro definitions \ref ARM_FLASH_STATUS
 */
/* Busy status values of the Flash driver */
#define DRIVER_STATUS_IDLE      (0u)
#define DRIVER_STATUS_BUSY      (1u)
/* Error status values of the Flash driver */
#define DRIVER_STATUS_NO_ERROR  (0u)
#define DRIVER_STATUS_ERROR     (1u)

/**
 * \brief Arm Flash device structure.
 */
struct arm_flash_dev_t {
    struct mt25ql_dev_t* dev;   /*!< FLASH memory device structure */
    ARM_FLASH_INFO *data;       /*!< FLASH memory device data */
};

/**
 * \brief      Check if the Flash memory boundaries are not violated.
 * \param[in]  flash_dev  Flash device structure \ref arm_flash_dev_t
 * \param[in]  offset     Highest Flash memory address which would be accessed.
 * \return     Returns true if Flash memory boundaries are not violated, false
 *             otherwise.
 */
static bool is_range_valid(struct arm_flash_dev_t *flash_dev,
                           uint32_t offset)
{
    uint32_t flash_limit = 0;

    /* Calculating the highest address of the Flash memory address range */
    flash_limit = FLASH_TOTAL_SIZE - 1;

    return (offset > flash_limit) ? (false) : (true) ;
}

/**
 * \brief        Check if the parameter is aligned to program_unit.
 * \param[in]    flash_dev  Flash device structure \ref arm_flash_dev_t
 * \param[in]    param      Any number that can be checked against the
 *                          program_unit, e.g. Flash memory address or
 *                          data length in bytes.
 * \return       Returns true if param is aligned to program_unit, false
 *               otherwise.
 */
static bool is_write_aligned(struct arm_flash_dev_t *flash_dev,
                             uint32_t param)
{
    return ((param % flash_dev->data->program_unit) != 0) ? (false) : (true);
}

#if (RTE_QSPI_FLASH0)
static ARM_FLASH_INFO ARM_FLASH0_DEV_DATA = {
    .sector_info    = NULL,     /* Uniform sector layout */
    .sector_count   = FLASH_TOTAL_SIZE / SUBSECTOR_4KB,
    .sector_size    = SUBSECTOR_4KB,
    .page_size      = FLASH_PAGE_SIZE,
    .program_unit   = 1u,       /* Minimum write size in bytes */
    .erased_value   = 0xFF
};

static struct arm_flash_dev_t ARM_FLASH0_DEV = {
    .dev    = &FLASH0_DEV,
    .data   = &(ARM_FLASH0_DEV_DATA)
};

/* Flash Status */
static ARM_FLASH_STATUS ARM_FLASH0_STATUS = {0, 0, 0};

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
    enum mt25ql_error_t err = MT25QL_ERR_NONE;

    ARG_UNUSED(cb_event);

    qspi_ip6514e_enable(ARM_FLASH0_DEV.dev->controller);

    /* Configure QSPI Flash controller to operate in single SPI mode and
     * to use fast Flash commands */
    err = mt25ql_config_mode(ARM_FLASH0_DEV.dev, MT25QL_FUNC_STATE_FAST);
    if(err != MT25QL_ERR_NONE) {
        return ARM_DRIVER_ERROR;
    }

    return ARM_DRIVER_OK;
}

static int32_t ARM_Flash_Uninitialize(void)
{
    enum mt25ql_error_t err = MT25QL_ERR_NONE;

    /* Restores the QSPI Flash controller and MT25QL to reset state */
    err = mt25ql_restore_reset_state(ARM_FLASH0_DEV.dev);
    if(err != MT25QL_ERR_NONE) {
        return ARM_DRIVER_ERROR;
    }

    return ARM_DRIVER_OK;
}

static int32_t ARM_Flash_PowerControl(ARM_POWER_STATE state)
{
    switch(state) {
    case ARM_POWER_FULL:
        /* Nothing to be done */
        return ARM_DRIVER_OK;
    case ARM_POWER_OFF:
    case ARM_POWER_LOW:
        return ARM_DRIVER_ERROR_UNSUPPORTED;
    default:
        return ARM_DRIVER_ERROR_PARAMETER;
    }
}

static int32_t ARM_Flash_ReadData(uint32_t addr, void *data, uint32_t cnt)
{
    enum mt25ql_error_t err = MT25QL_ERR_NONE;
    bool is_valid = true;

    ARM_FLASH0_STATUS.error = DRIVER_STATUS_NO_ERROR;

    /* Check Flash memory boundaries */
    is_valid = is_range_valid(&ARM_FLASH0_DEV, addr + cnt);
    if(is_valid != true) {
        ARM_FLASH0_STATUS.error = DRIVER_STATUS_ERROR;
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    ARM_FLASH0_STATUS.busy = DRIVER_STATUS_BUSY;

    err = mt25ql_command_read(ARM_FLASH0_DEV.dev, addr, data, cnt);

    ARM_FLASH0_STATUS.busy = DRIVER_STATUS_IDLE;

    if(err != MT25QL_ERR_NONE) {
        ARM_FLASH0_STATUS.error = DRIVER_STATUS_ERROR;
        return ARM_DRIVER_ERROR;
    }

    return ARM_DRIVER_OK;
}

static int32_t ARM_Flash_ProgramData(uint32_t addr,
                                     const void *data, uint32_t cnt)
{
    enum mt25ql_error_t err = MT25QL_ERR_NONE;

    ARM_FLASH0_STATUS.error = DRIVER_STATUS_NO_ERROR;

    /* Check Flash memory boundaries and alignment with minimum write size
     * (program_unit), data size also needs to be a multiple of program_unit.
     */
    if(!(is_range_valid(&ARM_FLASH0_DEV, addr + cnt) &&
         is_write_aligned(&ARM_FLASH0_DEV, addr)     &&
         is_write_aligned(&ARM_FLASH0_DEV, cnt)      )) {

        ARM_FLASH0_STATUS.error = DRIVER_STATUS_ERROR;
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    ARM_FLASH0_STATUS.busy = DRIVER_STATUS_BUSY;

    err = mt25ql_command_write(ARM_FLASH0_DEV.dev, addr, data, cnt);

    ARM_FLASH0_STATUS.busy = DRIVER_STATUS_IDLE;

    if(err != MT25QL_ERR_NONE) {
        ARM_FLASH0_STATUS.error = DRIVER_STATUS_ERROR;
        return ARM_DRIVER_ERROR;
    }

    return ARM_DRIVER_OK;
}

static int32_t ARM_Flash_EraseSector(uint32_t addr)
{
    enum mt25ql_error_t err = MT25QL_ERR_NONE;

    ARM_FLASH0_STATUS.error = DRIVER_STATUS_NO_ERROR;
    ARM_FLASH0_STATUS.busy = DRIVER_STATUS_BUSY;

    /* The erase function checks whether the address is aligned with
     * the sector or subsector and checks the Flash memory boundaries.
     */
    err = mt25ql_erase(ARM_FLASH0_DEV.dev, addr,
                       (enum mt25ql_erase_t) ARM_FLASH0_DEV.data->sector_size);

    ARM_FLASH0_STATUS.busy = DRIVER_STATUS_IDLE;

    if(err != MT25QL_ERR_NONE) {
        ARM_FLASH0_STATUS.error = DRIVER_STATUS_ERROR;

        if((err == MT25QL_ERR_ADDR_NOT_ALIGNED) ||
           (err == MT25QL_ERR_ADDR_TOO_BIG)     ||
           (err == MT25QL_ERR_WRONG_ARGUMENT)    ) {
            return ARM_DRIVER_ERROR_PARAMETER;
        }
        return ARM_DRIVER_ERROR;
    }

    return ARM_DRIVER_OK;
}

static int32_t ARM_Flash_EraseChip(void)
{
    enum mt25ql_error_t err = MT25QL_ERR_NONE;

    if(DriverCapabilities.erase_chip == 1) {

        ARM_FLASH0_STATUS.error = DRIVER_STATUS_NO_ERROR;
        ARM_FLASH0_STATUS.busy = DRIVER_STATUS_BUSY;

        /* The erase function checks whether the address is aligned with
         * the sector or subsector and checks the Flash memory boundaries.
         */
        err = mt25ql_erase(ARM_FLASH0_DEV.dev, 0, MT25QL_ERASE_ALL_FLASH);

        ARM_FLASH0_STATUS.busy = DRIVER_STATUS_IDLE;

        if(err != MT25QL_ERR_NONE) {
            ARM_FLASH0_STATUS.error = DRIVER_STATUS_ERROR;

            if((err == MT25QL_ERR_ADDR_NOT_ALIGNED) ||
               (err == MT25QL_ERR_ADDR_TOO_BIG)     ||
               (err == MT25QL_ERR_WRONG_ARGUMENT)    ) {
                return ARM_DRIVER_ERROR_PARAMETER;
            }
            return ARM_DRIVER_ERROR;
        }

        return ARM_DRIVER_OK;

    } else {
        return ARM_DRIVER_ERROR_UNSUPPORTED;
    }
}

static ARM_FLASH_STATUS ARM_Flash_GetStatus(void)
{
    return ARM_FLASH0_STATUS;
}

static ARM_FLASH_INFO * ARM_Flash_GetInfo(void)
{
    return ARM_FLASH0_DEV.data;
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
#endif /* RTE_QSPI_FLASH0 */
