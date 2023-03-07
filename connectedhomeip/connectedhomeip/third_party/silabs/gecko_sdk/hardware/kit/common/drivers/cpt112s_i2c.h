/***************************************************************************//**
 * @file
 * @brief Driver for retrieving capsense button and slider inputs from CPT112S
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

#ifndef CPT112S_I2C_H
#define CPT112S_I2C_H

#include <stdint.h>

/***************************************************************************//**
 * @addtogroup kitdrv
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup CPT112S_I2C
 * @brief Silicon Labs 12-pad Capacitive Touch Evaluation Board I2C driver.
 *
 * @details
 * <B>I2C data structure</B>
 *
 * For CPT112S there are 12 capsense outputs, each pin
 * corresponds to a capsenseCurrent bit showing whether a
 * cap-sense button pressed or not, and capsensePrevious
 * bit showing the status of previous cycle.
 *
 * If capsenseCurrent==1 && capsensePrevious==0, detects a "press"
 * If capsenseCurrent==0 && capsensePrevious==1, detects a "release"
 *
 * capsenseCurrent bit-map byte:
 * | N/A | N/A | N/A | N/A| cC11 | cC10| cC9 | cC8 |
 * | cC7 | cC6 | cC5 | cC4 | cC3 | cC2 | cC1 | cC0 |
 *
 * capsensePrevious bit-map byte:
 * | N/A | N/A | N/A | N/A| cP11 | cP10| cP9 | cP8 |
 * | cP7 | cP6 | cP5 | cP4 | cP3 | cP2 | cP1 | cP0 |
 *
 * *cC,cP are abbreviation for capsenseCurrent and capsensePrevious
 *
 * @{
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

#define CPT112S_I2C_ADDRESS         0xE0
#define CPT112S_I2C_RXBUFFER_SIZE   3

#define CPT112S_I2C_TOUCH_EVENT     0
#define CPT112S_I2C_RELEASE_EVENT   1
#define CPT112S_I2C_SLIDER_ACTIVITY 2

/*******************************************************************************
 *******************************  FUNCTIONS  ***********************************
 ******************************************************************************/

uint16_t CPT112S_getCapsenseCurrent(void);
uint16_t CPT112S_getCapsensePrevious(void);
uint16_t CPT112S_getSliderCurrent(void);
uint16_t CPT112S_getSliderPrevious(void);
void CPT112S_init(void);
void CPT112S_update(void);

#ifdef __cplusplus
}
#endif

/** @} (end group CPT112S_I2C) */
/** @} (end group kitdrv) */

#endif /* CPT112S_I2C_H */
