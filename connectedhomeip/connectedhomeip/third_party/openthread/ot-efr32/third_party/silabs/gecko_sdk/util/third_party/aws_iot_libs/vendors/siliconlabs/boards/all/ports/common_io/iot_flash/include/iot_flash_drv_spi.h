/***************************************************************************//**
 * @file    iot_flash_drv_spi.h
 * @brief   SPI Flash driver header file
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

/*******************************************************************************
 *                              SAFE GUARD
 ******************************************************************************/

#ifndef _IOT_FLASH_DRV_SPI_H_
#define _IOT_FLASH_DRV_SPI_H_

/*******************************************************************************
 *                               INCLUDES
 ******************************************************************************/

#include "sl_status.h"

/*******************************************************************************
 *                              PROTOTYPES
 ******************************************************************************/

/* flash h/w control */
sl_status_t iot_flash_drv_spi_driver_init(void *pvHndl);
sl_status_t iot_flash_drv_spi_driver_deinit(void *pvHndl);

/* flash memory attributes */
sl_status_t iot_flash_drv_spi_get_flash_size(void *pvHndl,
                                             uint32_t *ulFlashSize);
sl_status_t iot_flash_drv_spi_get_block_size(void *pvHndl,
                                             uint32_t *ulBlockSize);
sl_status_t iot_flash_drv_spi_get_sector_size(void *pvHndl,
                                              uint32_t *ulSectorSize);
sl_status_t iot_flash_drv_spi_get_page_size(void *pvHndl,
                                            uint32_t *ulPageSize);
sl_status_t iot_flash_drv_spi_get_lock_size(void *pvHndl,
                                            uint32_t *ulLockSize);
sl_status_t iot_flash_drv_spi_get_async_flag(void *pvHndl,
                                             uint8_t *ubAsyncSupport);
sl_status_t iot_flash_drv_spi_get_device_id(void *pvHndl,
                                            uint32_t *ulDeviceId);

/* flash erase functions */
sl_status_t iot_flash_drv_spi_erase_sector(void *pvHndl, uint32_t ulAddress);
sl_status_t iot_flash_drv_spi_erase_chip(void *pvHndl);

/* flash i/o functions */
sl_status_t iot_flash_drv_spi_data_write(void *pvHndl,
                                         uint32_t ulAddress,
                                         uint8_t *pubBuffer,
                                         uint32_t ulSize);
sl_status_t iot_flash_drv_spi_data_read(void *pvHndl,
                                        uint32_t ulAddress,
                                        uint8_t *pubBuffer,
                                        uint32_t ulSize);

/*******************************************************************************
 *                              SAFE GUARD
 ******************************************************************************/

#endif /* _IOT_FLASH_DRV_SPI_H_ */
