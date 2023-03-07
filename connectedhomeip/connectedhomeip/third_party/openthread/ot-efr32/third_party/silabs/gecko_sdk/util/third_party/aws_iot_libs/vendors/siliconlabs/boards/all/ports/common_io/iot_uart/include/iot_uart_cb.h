/***************************************************************************//**
 * @file    iot_uart_cb.h
 * @brief   UART callback definitions.
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

#ifndef _IOT_UART_CB_H_
#define _IOT_UART_CB_H_

/*******************************************************************************
 *                               INCLUDES
 ******************************************************************************/

#include "sl_status.h"

/*******************************************************************************
 *                               MACROS
 ******************************************************************************/

/* use callbacks that are defined in HAL layer */
#define IOT_UART_CB_TX    iot_uart_hal_cb_tx
#define IOT_UART_CB_RX    iot_uart_hal_cb_rx

/*******************************************************************************
 *                             PROTOTYPES
 ******************************************************************************/

void IOT_UART_CB_TX(int32_t lUartInstance);
void IOT_UART_CB_RX(int32_t lUartInstance);

/*******************************************************************************
 *                              SAFE GUARD
 ******************************************************************************/

#endif /* _IOT_UART_CB_H_ */
