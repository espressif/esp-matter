/***************************************************************************//**
 * @file
 * @brief Kit specific board defines for BRD8001A sensor expansion board.
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

#ifndef BRD8001A_CONFIG_H
#define BRD8001A_CONFIG_H

#include "em_device.h"
#include "em_gpio.h"

// Definition for the proximity detector part of SI1147
#define BRD8001A_INT_INPUT_PORT   gpioPortA
#define BRD8001A_INT_INPUT_PIN    1

// Definition for powering up sensor board STK
#define BRD8001A_POWER_PORT       gpioPortA
#define BRD8001A_POWER_PIN        0

#endif /* BRD8001A_CONFIG_H */
