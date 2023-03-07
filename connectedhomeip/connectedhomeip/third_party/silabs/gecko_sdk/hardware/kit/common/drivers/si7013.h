/***************************************************************************//**
 * @file
 * @brief Driver for the Si7013 Temperature / Humidity sensor
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

#ifndef __SI7013_H
#define __SI7013_H

#include "em_device.h"
#include <stdbool.h>

/***************************************************************************//**
 * @addtogroup kitdrv
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup Si7013
 * @{
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

/** I2C device address for Si7013 */
#define SI7013_ADDR      0x82
/** I2C device address for Si7021 */
#define SI7021_ADDR      0x80

/** Device ID value for Si7013 */
#define SI7013_DEVICE_ID 0x0D
/** Device ID value for Si7020 */
#define SI7020_DEVICE_ID 0x14
/** Device ID value for Si7021 */
#define SI7021_DEVICE_ID 0x21

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

int32_t Si7013_MeasureRHAndTemp(I2C_TypeDef *i2c, uint8_t addr,
                                uint32_t *rhData, int32_t *tData);

int32_t Si7013_GetFirmwareRevision(I2C_TypeDef *i2c, uint8_t addr, uint8_t *fwRev);

bool Si7013_Detect(I2C_TypeDef *i2c, uint8_t addr, uint8_t *deviceId);
int32_t Si7013_ReadNoHoldRHAndTemp(I2C_TypeDef *i2c, uint8_t addr, uint32_t *rhData,
                                   int32_t *tData);
int32_t Si7013_StartNoHoldMeasureRHAndTemp(I2C_TypeDef *i2c, uint8_t addr);
int32_t Si7013_MeasureV(I2C_TypeDef *i2c, uint8_t addr, int32_t *vData);
#ifdef __cplusplus
}
#endif

/** @} (end group Si7013) */
/** @} (end group kitdrv) */

#endif /* __SI7013_H */
