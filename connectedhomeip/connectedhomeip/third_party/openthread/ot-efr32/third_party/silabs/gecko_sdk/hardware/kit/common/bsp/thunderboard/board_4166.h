/***************************************************************************//**
 * @file
 * @brief BOARD module header file
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef BOARD_4166_H
#define BOARD_4166_H

#include <stdint.h>

/**************************************************************************//**
* @addtogroup TBSense_BSP
* @{
******************************************************************************/

/***************************************************************************//**
 * @addtogroup BOARD_4166
 * @{
 ******************************************************************************/

uint32_t BOARD_hallSensorEnable    (bool enable);
uint32_t BOARD_hallSensorEnableIRQ (bool enable);
void     BOARD_hallSensorSetIRQCallback(BOARD_IrqCallback cb);
void     BOARD_hallSensorClearIRQ(void);

uint32_t BOARD_gasSensorEnable     (bool enable);
uint32_t BOARD_gasSensorEnableIRQ  (bool enable);
void     BOARD_gasSensorClearIRQ   (void);
uint32_t BOARD_gasSensorWake       (bool wake);
void     BOARD_gasSensorSetIRQCallback(BOARD_IrqCallback cb);

uint32_t BOARD_bapEnable           (bool enable);

void     BOARD_pushButton0SetIRQCallback (BOARD_IrqCallback cb);
void     BOARD_pushButton0ClearIRQ       (void);
void     BOARD_pushButton1SetIRQCallback (BOARD_IrqCallback cb);
void     BOARD_pushButton1ClearIRQ       (void);

void     BOARD_rgbledEnable        (bool enable, uint8_t mask);
void     BOARD_rgbledSetColor      (uint8_t red, uint8_t green, uint8_t blue);
void     BOARD_rgbledSetRawColor   (uint16_t red, uint16_t green, uint16_t blue);
void     BOARD_rgbledPowerEnable   (bool enable);

/** @} */
/** @} */

#endif // BOARD_4166_H
