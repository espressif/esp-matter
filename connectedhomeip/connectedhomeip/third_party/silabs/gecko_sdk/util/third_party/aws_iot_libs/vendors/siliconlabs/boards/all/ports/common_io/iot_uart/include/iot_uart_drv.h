/***************************************************************************//**
 * @file    iot_uart_drv.h
 * @brief   UART driver header file.
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

#ifndef _IOT_UART_DRV_H_
#define _IOT_UART_DRV_H_

/*******************************************************************************
 *                               INCLUDES
 ******************************************************************************/

#include "sl_status.h"

/*******************************************************************************
 *                              PROTOTYPES
 ******************************************************************************/

/* UART h/w control */
sl_status_t iot_uart_drv_driver_init(void *pvHndl);
sl_status_t iot_uart_drv_driver_deinit(void *pvHndl);

/* UART h/w control */
sl_status_t iot_uart_drv_hw_enable(void *pvHndl);
sl_status_t iot_uart_drv_hw_disable(void *pvHndl);

/* UART set/get config */
sl_status_t iot_uart_drv_config_set(void *pvHndl,
                                       uint32_t ulBaudrate,
                                       uint8_t ucParity,
                                       uint8_t ucStopbits,
                                       uint8_t ucWordlength,
                                       uint8_t ucFlowControl);
sl_status_t iot_uart_drv_config_get(void *pvHndl,
                                       uint32_t *pulBaudrate,
                                       uint8_t *pucParity,
                                       uint8_t *pucStopbits,
                                       uint8_t *pucWordlength,
                                       uint8_t *pucFlowControl);

/* UART transfer (sync) routines */
sl_status_t iot_uart_drv_transfer_tx(void *pvHndl, uint8_t *pucData);
sl_status_t iot_uart_drv_transfer_rx(void *pvHndl, uint8_t *pucData);

/* UART start (async) routines */
sl_status_t iot_uart_drv_start_tx(void *pvHndl);
sl_status_t iot_uart_drv_start_rx(void *pvHndl);

/* UART stop (async) routines */
sl_status_t iot_uart_drv_stop_tx(void *pvHndl);
sl_status_t iot_uart_drv_stop_rx(void *pvHndl);

/*******************************************************************************
 *                              SAFE GUARD
 ******************************************************************************/

#endif /* _IOT_UART_DRV_H_ */
