/*
 * Copyright (c) 2018-2020 Arm Limited
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

/**
 * \file gfc100_eflash_drv.h
 *
 * \brief Generic driver for GFC100 flash controller
 */

#ifndef __GFC100_FLASH_DRV_H__
#define __GFC100_FLASH_DRV_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

enum gfc100_error_t {
    GFC100_ERROR_NONE = 0U,          /*!< No error */
    GFC100_ERROR_NOT_INITED,         /*!< Device not inited error */
    GFC100_ERROR_CMD_PENDING,        /*!< Previous cmd is still pending error */
    GFC100_ERROR_INVALID_WORD_WIDTH, /*!< Invalid word width error */
    GFC100_ERROR_INVALID_PARAM,      /*!< Invalid parameter error */
    GFC100_ERROR_OUT_OF_RANGE,       /*!< Flash address out of range error */
    GFC100_ERROR_CMD_FAIL,           /*!< Command failed error */
    GFC100_ERROR_UNALIGNED_PARAM     /*!< Unaligned parameter error */
};

/** GFC100 Flash controller device configuration structure */
struct gfc100_eflash_dev_cfg_t {
    const uint32_t base;  /*!< GFC100 base address */
};

/** GFC100 Flash controller device data structure */
struct gfc100_eflash_dev_data_t {
    bool is_initialized;     /*!< Indicates if the device is initialized */
    uint32_t flash_size;     /*!< Size of the flash area */
};

/** GFC100 Flash controller device structure */
struct gfc100_eflash_dev_t {
    const struct gfc100_eflash_dev_cfg_t* const cfg;
                                                    /*!< GFC100 configuration */
    struct gfc100_eflash_dev_data_t* const data;    /*!< GFC100 data */
};

enum gfc100_erase_type_t {
    GFC100_ERASE_PAGE = 0U,        /*!< Erase 1 page */
    GFC100_MASS_ERASE_MAIN_AREA,   /*!< Erase main area */
    GFC100_MASS_ERASE_ALL          /*!< Erase main + extended area */
};

/**
 * \brief Initializes GFC100 flash controller
 *
 * \param[in] dev      GFC100 device struct \ref gfc100_eflash_dev_t
 * \param[in] sys_clk  System clock in Hz
 *
 * \note This API needs to be called prior to any other APIs.
 * \note For better performance, this function doesn't check if dev is NULL
 */
void gfc100_eflash_init(struct gfc100_eflash_dev_t *dev, uint32_t sys_clk);

/**
 * \brief Reads data from the flash in a blocking call
 *
 * \param[in]     dev   GFC100 device struct \ref gfc100_eflash_dev_t
 * \param[in]     addr  Address to read data from the flash
 * \param[out]    data  Pointer to store data read from the flash
 * \param[in,out] len   Number of bytes to read, number of bytes read
 *
 * \return Returns error code as specified in \ref gfc100_error_t
 *
 * \note This API reads the flash memory by sending read commands with the
 *       controller, but reading through the AHB directly mapped address gives
 *       better performance (if flash is mapped for direct read).
 * \note Addr is expected to be within the [0x0 - Flash size] range
 * \note For better performance, this function doesn't check if dev is NULL
 * \note Addr, data and len can have any aligment
 */
enum gfc100_error_t gfc100_eflash_read(struct gfc100_eflash_dev_t *dev,
                                      uint32_t addr, void *data, uint32_t *len);

/**
 * \brief Writes data to the flash in a blocking call
 *
 * \param[in] dev      GFC100 device struct \ref gfc100_eflash_dev_t
 * \param[in] addr     Address to write data to the flash
 * \param[in] data     Pointer to the data to be written
 * \param[in] len      Number of bytes to write
 *
 * \return Returns error code as specified in \ref gfc100_error_t
 *
 * \note Flash area needs to be pre-erased before writing to it
 * \note Addr is expected to be within the [0x0 - Flash size] range
 * \note For better performance, this function doesn't check if dev is NULL
 * \note Addr and len must be 4 bytes aligned
 */
enum gfc100_error_t gfc100_eflash_write(struct gfc100_eflash_dev_t *dev,
                                        uint32_t addr, const void *data,
                                        uint32_t *len);

/**
 * \brief Writes data to the flash in a blocking call
 *
 * \param[in] dev      GFC100 device struct \ref gfc100_eflash_dev_t
 * \param[in] addr     Address to write data to the flash
 * \param[in] data     Pointer to the data to be written
 * \param[in] len      Number of bytes to write
 *
 * \return Returns error code as specified in \ref gfc100_error_t
 *
 * \note This API uses the Row Write command by sending the commands
 *       continuously so the controller won't close the transfer and start a new
 *       for the next 4 bytes.This gives much better performance
 *       than simple Write command, so this API is preferred to use if many
 *       bytes needs to be written to the flash.
 * \note Addr is expected to be within the [0x0 - Flash size] range
 * \note For better performance, this function doesn't check if dev is NULL
 * \note Addr and len must be 4 bytes aligned
 */
enum gfc100_error_t gfc100_eflash_row_write(struct gfc100_eflash_dev_t *dev,
                                           uint32_t addr, const void *data,
                                           uint32_t *len);

/**
 * \brief Erases the flash
 *
 * \param[in] dev      GFC100 device struct \ref gfc100_eflash_dev_t
 * \param[in] addr     Address of the page to erase
 * \param[in] erase    Erase type \ref gfc100_erase_type_t
 *
 * \return Returns error code as specified in \ref gfc100_error_t
 *
 * \note For better performance, this function doesn't check if dev is NULL
 * \note Addr is expected to be within the [0x0 - Flash size] range
 * \note Addr is only used for page erase, and is automatically aligned
 *       to page size.
 */
enum gfc100_error_t gfc100_eflash_erase(struct gfc100_eflash_dev_t *dev,
                                       uint32_t addr,
                                       enum gfc100_erase_type_t erase);

/**
 * \brief Checks if controller is locked
 *
 * \param[in] dev      GFC100 device struct \ref gfc100_eflash_dev_t
 *
 * \return Returns true is controller is locked, false otherwise
 *
 * \note For better performance, this function doesn't check if dev is NULL
 * \note This can be used by the software to detect why the APB request
 *       is being held up.
 */
bool gfc100_is_controller_locked(struct gfc100_eflash_dev_t *dev);

/**
 * \brief Gets flash memory size in bytes
 *
 * \param[in] dev      GFC100 device struct \ref gfc100_eflash_dev_t
 *
 * \return Returns the size of the flash memory in bytes
 *
 * \note For better performance, this function doesn't check if dev is NULL
 */
uint32_t gfc100_get_eflash_size(struct gfc100_eflash_dev_t *dev);

/**
 * \brief Gets flash page size in bytes
 *
 * \param[in] dev      GFC100 device struct \ref gfc100_eflash_dev_t
 *
 * \return Returns the page size of the flash memory in bytes
 *
 * \note For better performance, this function doesn't check if dev is NULL
 */
uint32_t gfc100_get_eflash_page_size(struct gfc100_eflash_dev_t *dev);

/**
 * \brief Gets number of info pages
 *
 * \param[in] dev      GFC100 device struct \ref gfc100_eflash_dev_t
 *
 * \return Returns the number of info pages from the extended area
 *
 * \note For better performance, this function doesn't check if dev is NULL.
 */
uint32_t gfc100_get_num_of_info_pages(struct gfc100_eflash_dev_t *dev);

/**
 * \brief Gets process specific error bits
 *
 * \param[in] dev      GFC100 device struct \ref gfc100_eflash_dev_t
 *
 * \return Returns the error bits specified by the process specific part
 *         of the controller.
 *
 * \note For better performance, this function doesn't check if dev is NULL.
 * \note Can be used for debug purposes.
 */
uint32_t gfc100_get_proc_spec_error(struct gfc100_eflash_dev_t *dev);

#ifdef __cplusplus
}
#endif

#endif  /* __GFC100_FLASH_DRV_H__ */

