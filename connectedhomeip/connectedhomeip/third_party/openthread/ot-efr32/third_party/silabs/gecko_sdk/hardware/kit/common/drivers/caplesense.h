/***************************************************************************//**
 * @file
 * @brief Capacitive sense driver
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

#ifndef __CAPLESENSE_H_
#define __CAPLESENSE_H_

#include <stdint.h>
#include <stdbool.h>

/***************************************************************************//**
 * @addtogroup kitdrv
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup CapSense
 * @{
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

uint8_t  CAPLESENSE_getSegmentChannel(uint8_t capSegment);
uint32_t CAPLESENSE_getVal(uint8_t channel);
uint32_t CAPLESENSE_getNormalizedVal(uint8_t channel);
int32_t CAPLESENSE_getSliderPosition(void);
void CAPLESENSE_Init(bool sleep);
void CAPLESENSE_setupLESENSE(bool sleep);
void CAPLESENSE_setupCallbacks(void (*scanCb)(void), void (*chCb)(void));
void CAPLESENSE_Sleep(void);

#ifdef __cplusplus
}
#endif

/** @} (end group CapSense) */
/** @} (end group kitdrv) */

#endif /* __CAPSENSE_H_ */
