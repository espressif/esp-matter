/***************************************************************************//**
 * @file    iot_spi_cb.h
 * @brief   SPI callback definitions.
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

#ifndef _IOT_SPI_CB_H_
#define _IOT_SPI_CB_H_

/*******************************************************************************
 *                               INCLUDES
 ******************************************************************************/

#include "sl_status.h"

/*******************************************************************************
 *                               MACROS
 ******************************************************************************/

/* use callbacks that are defined in HAL layer */
#define IOT_SPI_CB_RX    iot_spi_hal_cb_rx
#define IOT_SPI_CB_TX    iot_spi_hal_cb_tx
#define IOT_SPI_CB_XX    iot_spi_hal_cb_xx

/*******************************************************************************
 *                             PROTOTYPES
 ******************************************************************************/

void iot_spi_hal_cb_rx(int32_t lInstNum, sl_status_t xSlStatus);
void iot_spi_hal_cb_tx(int32_t lInstNum, sl_status_t xSlStatus);
void iot_spi_hal_cb_xx(int32_t lInstNum, sl_status_t xSlStatus);

/*******************************************************************************
 *                              SAFE GUARD
 ******************************************************************************/

#endif /* _IOT_SPI_CB_H_ */
