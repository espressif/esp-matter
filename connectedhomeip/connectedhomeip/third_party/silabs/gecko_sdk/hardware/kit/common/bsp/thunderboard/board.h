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

#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include <stdbool.h>
#include "gpiointerrupt.h"

#include "bspconfig.h"

typedef void (*BOARD_IrqCallback)(void);/**< Interrupt callback function type definition */

/***************************************************************************//**
 * @defgroup TBSense_BSP Thunderboard Sense BSP
 * @{
 * @brief BSP for Thunderboard Sense and Thunderboard Sense 2
 ******************************************************************************/

/**************************************************************************//**
* @name BOARD Error Codes
* @{
******************************************************************************/
#define BOARD_OK                              0     /**< OK                                        */
#define BOARD_ERROR_I2C_TRANSFER_TIMEOUT      0x01  /**< I2C timeout occurred                      */
#define BOARD_ERROR_I2C_TRANSFER_NACK         0x02  /**< No acknowledgement received               */
#define BOARD_ERROR_I2C_TRANSFER_FAILED       0x03  /**< I2C transaction failed                    */
#define BOARD_ERROR_PIC_ID_MISMATCH           0x04  /**< The ID of the PIC is invalid              */
#define BOARD_ERROR_PIC_FW_INVALID            0x05  /**< Invalid PIC firmware                      */
#define BOARD_ERROR_PIC_FW_UPDATE_FAILED      0x06  /**< PIC firmware update failed                */

#define BOARD_ERROR_NO_POWER_INT_CTRL         0x10  /**< Power and Interrupt Controller not found  */
#define BOARD_ERROR_I2C_BUS_SELECT_INVALID    0x11  /**< Invalid I2C bus selection                 */
#define BOARD_ERROR_I2C_BUS_SELECT_FAILED     0x12  /**< I2C bus selection failed                  */
/**@}*/
/**@}*/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
#define BOARD_I2C_BUS_SELECT_NONE         0           /**< No I2C bus selected                               */
#define BOARD_I2C_BUS_SELECT_ENV_SENSOR   (1 << 0)    /**< The I2C bus of the environmental sensors selected */
#define BOARD_I2C_BUS_SELECT_GAS          (1 << 1)    /**< The I2C bus of the gas sensors selected           */
#define BOARD_I2C_BUS_SELECT_HALL         (1 << 2)    /**< The I2C bus of the Hall sensor selected           */
/** @endcond */

uint32_t BOARD_init                (void);

uint32_t BOARD_imuEnable           (bool enable);
uint32_t BOARD_imuEnableIRQ        (bool enable);
void     BOARD_imuClearIRQ         (void);
void     BOARD_imuSetIRQCallback   (BOARD_IrqCallback cb);

uint32_t BOARD_envSensEnable       (bool enable);
uint32_t BOARD_envSensEnableIRQ    (bool enable);

void     BOARD_flashDeepPowerDown  (void);

uint32_t BOARD_rhtempEnable        (bool enable);

uint32_t BOARD_alsEnable           (bool enable);
uint32_t BOARD_alsEnableIRQ        (bool enable);
void     BOARD_alsClearIRQ         (void);
void     BOARD_alsSetIRQCallback   (BOARD_IrqCallback cb);

void     BOARD_ledSet              (uint8_t leds);

uint32_t BOARD_micEnable           (bool enable);

uint32_t BOARD_i2cBusSelect        (uint8_t select);

uint8_t  BOARD_pushButtonGetState  (void);
void     BOARD_pushButtonEnableIRQ (bool enable);

#ifdef BSP_TBSENSE2
  #include "board_4166.h"
#endif

#ifdef BSP_BRD4184A
  #include "board_4184.h"
#endif

#endif // BOARD_H
