/***************************************************************************//**
 * @file
 * @brief Driver for Hall effect sensor
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

#ifndef HALL_H
#define HALL_H

/**************************************************************************//**
* @addtogroup TBSense_BSP
* @{
******************************************************************************/

/**************************************************************************//**
* @addtogroup HALL
* @{
******************************************************************************/

/**************************************************************************//**
* @name Error Codes
* @{
******************************************************************************/
#define HALL_OK                             0x0000  /**< No errors                                        */
#define HALL_ERROR_DRIVER_NOT_INITIALIZED   0x0001  /**< The driver is not initialized                    */
#define HALL_ERROR_I2C_TRANSACTION_FAILED   0x0002  /**< I2C transaction failed                           */
#define HALL_ERROR_DEVICE_ID_MISMATCH       0x0003  /**< The device ID does not match the expected value  */
/**@}*/

/**************************************************************************//**
* @name Configuration values
* @{
******************************************************************************/
#define HALL_CONFIG_POLARITY_OMNIPOLAR    0x00     /**< Omnipolar field polarity                */
#define HALL_CONFIG_POLARITY_NEGATIVE     0x01     /**< Negative field polarity                 */
#define HALL_CONFIG_POLARITY_POSITIVE     0x02     /**< Positive field polarity                 */
/**@}*/

/***************************************************************************//**
 * @brief
 *    Structure to configure the Hall effect sensor device
 ******************************************************************************/
typedef struct __HALL_Config {
  float              threshold;     /**< Decision point for magnetic field high or low in mT  */
  float              hysteresis;    /**< Hysteresis in mT                                     */
  uint8_t            polarity;      /**< Magnetic field polarity setting                      */
  bool               outputInvert;  /**< The polarity of the output pin                       */
} HALL_Config;

typedef void (*HALL_IntCallback)(bool pinHigh);    /**<  Interrupt callback function         */

uint32_t HALL_init             (void);
void     HALL_deInit           (void);
uint32_t HALL_configure        (HALL_Config *config);
uint32_t HALL_measure          (uint32_t scale, float *result);
void     HALL_registerCallback (HALL_IntCallback callback);
float    HALL_getTamperLevel   (void);

/** @} */
/** @} */

#endif /* HALL_H */
