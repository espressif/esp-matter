/***************************************************************************//**
 * @file
 * @brief Driver for the Bosch Sensortec BMP280 barometric pressure sensor
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

#ifndef BAP_H
#define BAP_H

/***************************************************************************//**
 * @addtogroup BAP
 * @{
 ******************************************************************************/

#include "bap_config.h"

/**************************************************************************//**
* @name Error Codes
* @{
******************************************************************************/
#define BAP_OK                             0x0000  /**< No errors                                        */
#define BAP_ERROR_DRIVER_NOT_INITIALIZED   0x0001  /**< The driver is not initialized                    */
#define BAP_ERROR_I2C_TRANSACTION_FAILED   0x0002  /**< I2C transaction failed                           */
#define BAP_ERROR_DEVICE_ID_MISMATCH       0x0003  /**< The device ID does not match the expected value  */
/**@}*/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
#define BAP_DEVICE_ID_BMP280               0x58   /**< Device ID of the BMP280 chip          */
/** @endcond */

/***************************************************************************//**
 * @brief
 *    Structure to configure the BMP280 device
 ******************************************************************************/
typedef struct __BAP_Config {
  uint8_t oversampling;       /**< Oversampling value                         */
  uint8_t powerMode;          /**< SLEEP, FORCED or NORMAL power mode setting */
  uint8_t standbyTime;        /**< Standby time setting                       */
} BAP_Config;

uint32_t BAP_init             (uint8_t *deviceId);
void     BAP_deInit           (void);
uint32_t BAP_config           (BAP_Config *cfg);
uint32_t BAP_getTemperature   (float *temperature);
uint32_t BAP_getPressure      (float *pressure);

/** @} */
/** @} */

#endif // BMP_H
