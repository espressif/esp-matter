/***************************************************************************//**
 * @file
 * @brief SSD2119 LCD controller driver
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
#ifndef __SSD2119_H
#define __SSD2119_H

#include <stdbool.h>
#include "em_device.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

#define notSSD2119_REGISTER_ACCESS_HOOKS
/** if you define this value, driver include functions allowing registering
 * your own TFT controller functions. This will allow changing way of accessing
 * controller to SPI for example but will also slow graphics.
 */

#define notLCD_DEVFUNC_CONTRADDR     0x35

/** if you define below value, direct access to TFT controller will be replaced
 * with access to addresses defined by separate function. This will allow
 * dynamic TFT controller address change but will also slow graphics.
 */

/** Performance verification (Release version compiled with IAR):
 *  NO FUNCTIONS HOOKS & NO VARIABLE CONTROLLER ADDRESS: 800 kpix/s
 *  NO FUNCTIONS HOOKS & VARIABLE CONTROLLER ADDRESS:    623 kpix/s
 *  FUNCTIONS HOOKS & NO VARIABLE CONTROLLER ADDRESS:    618 kpix/s
 *  FUNCTIONS HOOKS & VARIABLE CONTROLLER ADDRESS:       537 kpix/s
 */

#ifdef SSD2119_REGISTER_ACCESS_HOOKS
  #define LCD_DEVFUNC_WRITEREGISTER 0x30
  #define LCD_DEVFUNC_SETREGISTER   0x31
  #define LCD_DEVFUNC_WRITEDATA     0x32
  #define LCD_DEVFUNC_READDATA      0x33
#endif

#define LCD_DEVFUNC_INITIALIZE    0x34

#ifndef LCD_DEVFUNC_CONTRADDR
#define SSD2119_REGISTER_ADDRESS 0x88000000
#define SSD2119_DATA_ADDRESS     0x88000002
#endif

/** @endcond */

#ifdef __cplusplus
}
#endif

#endif /* __SSD2119_H */
