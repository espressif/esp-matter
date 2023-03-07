/***************************************************************************//**
 * @file
 * @brief DEVICE_INIT_HFRCO Config
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_DEVICE_INIT_HFRCO_CONFIG_H
#define SL_DEVICE_INIT_HFRCO_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <o SL_DEVICE_INIT_HFRCO_BAND> Frequency Band
// <i> RC Oscillator Frequency Band
// <cmuHFRCOFreq_1M0Hz=> 1 MHz
// <cmuHFRCOFreq_2M0Hz=> 2 MHz
// <cmuHFRCOFreq_4M0Hz=> 4 MHz
// <cmuHFRCOFreq_7M0Hz=> 7 MHz
// <cmuHFRCOFreq_13M0Hz=> 13 MHz
// <cmuHFRCOFreq_16M0Hz=> 16 MHz
// <cmuHFRCOFreq_19M0Hz=> 19 MHz
// <cmuHFRCOFreq_26M0Hz=> 26 MHz
// <cmuHFRCOFreq_32M0Hz=> 32 MHz
// <cmuHFRCOFreq_38M0Hz=> 38 MHz
// <cmuHFRCOFreq_48M0Hz=> 48 MHz
// <cmuHFRCOFreq_56M0Hz=> 56 MHz
// <cmuHFRCOFreq_64M0Hz=> 64 MHz
// <cmuHFRCOFreq_72M0Hz=> 72 MHz
// <i> Default: cmuHFRCOFreq_72M0Hz
#define SL_DEVICE_INIT_HFRCO_BAND           cmuHFRCOFreq_72M0Hz

// <<< end of configuration section >>>

#endif // SL_DEVICE_INIT_HFRCO_CONFIG_H
