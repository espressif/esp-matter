/***************************************************************************//**
 * @file
 * @brief Kit specific board defines for the CPT007B demo
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

#ifndef CPT007B_CONFIG_H
#define CPT007B_CONFIG_H

/////////////////////////////////////////////////////////////////////////////
// Project Configurations
/////////////////////////////////////////////////////////////////////////////
// The Demo mode determines the total number of total capsense buttons,
// the pins on the STK that correspond to the pins on the CPT device
// and so on.

// 0 - STK works with CPT007B board with GPIO mode capsense buttons
// 1 - STK works with CPT112S board with I2C mode capsense buttons
#ifndef CPT_DEMO_MODE
#define CPT_DEMO_MODE             0

#if CPT_DEMO_MODE
// Not supported
#else
#define TOTAL_CAPSENSE_PIN        7
#define CS0_0_PORT                gpioPortD
#define CS0_0_PIN                 1
#define CS0_1_PORT                gpioPortD
#define CS0_1_PIN                 3
#define CS0_2_PORT                gpioPortD
#define CS0_2_PIN                 6
#define CS0_3_PORT                gpioPortC
#define CS0_3_PIN                 6
#define CS0_4_PORT                gpioPortC
#define CS0_4_PIN                 3
#define CS0_5_PORT                gpioPortC
#define CS0_5_PIN                 4
#define CS0_6_PORT                gpioPortC
#define CS0_6_PIN                 5

#define CS0_0_PRESENT             0x01
#define CS0_1_PRESENT             0x02
#define CS0_2_PRESENT             0x04
#define CS0_3_PRESENT             0x08
#define CS0_4_PRESENT             0x10
#define CS0_5_PRESENT             0x20
#define CS0_6_PRESENT             0x40
#endif

#endif /* CPT_DEMO_MODE */
#endif /* CPT007B_CONFIG_H */
