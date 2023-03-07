/***************************************************************************//**
 * @file
 * @brief Driver for the Si1147 Proximity sensor
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

#ifndef __SI1147_H
#define __SI1147_H

#include "em_device.h"

/***************************************************************************//**
 * @addtogroup kitdrv
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup Si114x
 * @{
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *******************************   STRUCTS   ***********************************
 ******************************************************************************/
/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

typedef struct {
  I2C_TypeDef *i2c;
  uint8_t addr;
} si114x_i2c_t;

/** @endcond */

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/
uint32_t Si1147_Write_Register (I2C_TypeDef *i2c, uint8_t addr, uint8_t reg, uint8_t data);
uint32_t Si1147_Write_Block_Register (I2C_TypeDef *i2c, uint8_t addr, uint8_t reg, uint8_t length, uint8_t const *data);
uint32_t Si1147_Read_Block_Register (I2C_TypeDef *i2c, uint8_t addr, uint8_t reg, uint8_t length, uint8_t  *data);
uint32_t Si1147_Read_Register (I2C_TypeDef *i2c, uint8_t addr, uint8_t reg, uint8_t *data);

#ifdef __cplusplus
}
#endif

/** @} (end group Si114x) */
/** @} (end group kitdrv) */

#endif /* __TEMPSENS_H */
