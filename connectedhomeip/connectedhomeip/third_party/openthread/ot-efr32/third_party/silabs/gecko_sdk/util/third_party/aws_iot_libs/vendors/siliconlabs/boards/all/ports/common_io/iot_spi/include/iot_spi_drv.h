/***************************************************************************//**
 * @file    iot_spi_drv.h
 * @brief   SPI driver header file
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

#ifndef _IOT_SPI_DRV_H_
#define _IOT_SPI_DRV_H_

/*******************************************************************************
 *                               INCLUDES
 ******************************************************************************/

#include "sl_status.h"

/*******************************************************************************
 *                              PROTOTYPES
 ******************************************************************************/

/* SPI h/w control */
sl_status_t iot_spi_drv_driver_init(void *pvHndl);
sl_status_t iot_spi_drv_driver_deinit(void *pvHndl);

/* SPI h/w control */
sl_status_t iot_spi_drv_hw_enable(void *pvHndl);
sl_status_t iot_spi_drv_hw_disable(void *pvHndl);

/* SPI set/get config */
sl_status_t iot_spi_drv_config_set(void *pvHndl,
                                      uint32_t ulFreq,
                                      uint8_t ucMode,
                                      uint8_t ucBitOrder,
                                      uint8_t ucDummyValue);
sl_status_t iot_spi_drv_config_get(void *pvHndl,
                                      uint32_t *pulFreq,
                                      uint8_t *pucMode,
                                      uint8_t *pucBitOrder,
                                      uint8_t *pucDummyValue);

/* SPI operations: read */
sl_status_t iot_spi_drv_read_sync(void *pvHndl,
                                     uint8_t *pvBuf,
                                     uint32_t ulCount);
sl_status_t iot_spi_drv_read_async(void *pvHndl,
                                      uint8_t *pvBuf,
                                      uint32_t ulCount);
sl_status_t iot_spi_drv_read_abort(void *pvHndl);

/* SPI operations: write */
sl_status_t iot_spi_drv_write_sync(void *pvHndl,
                                      uint8_t *pvBuf,
                                      uint32_t ulCount);
sl_status_t iot_spi_drv_write_async(void *pvHndl,
                                       uint8_t *pvBuf,
                                       uint32_t ulCount);
sl_status_t iot_spi_drv_write_abort(void *pvHndl);

/* SPI operations: transfer */
sl_status_t iot_spi_drv_transfer_sync(void *pvHndl,
                                         uint8_t *pvTxBuf,
                                         uint8_t *pvRxBuf,
                                         uint32_t ulCount);
sl_status_t iot_spi_drv_transfer_async(void *pvHndl,
                                          uint8_t *pvTxBuf,
                                          uint8_t *pvRxBuf,
                                          uint32_t ulCount);
sl_status_t iot_spi_drv_transfer_abort(void *pvHndl);

/* SPI status */
sl_status_t iot_spi_drv_status_tx(void *pvHndl, uint32_t *pulCount);
sl_status_t iot_spi_drv_status_rx(void *pvHndl, uint32_t *pulCount);

/*******************************************************************************
 *                              SAFE GUARD
 ******************************************************************************/

#endif /* _IOT_SPI_DRV_H_ */
