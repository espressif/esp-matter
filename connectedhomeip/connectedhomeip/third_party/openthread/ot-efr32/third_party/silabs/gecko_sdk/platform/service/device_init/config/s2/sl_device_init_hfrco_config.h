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
// <cmuHFRCODPLLFreq_1M0Hz=> 1 MHz
// <cmuHFRCODPLLFreq_2M0Hz=> 2 MHz
// <cmuHFRCODPLLFreq_4M0Hz=> 4 MHz
// <cmuHFRCODPLLFreq_7M0Hz=> 7 MHz
// <cmuHFRCODPLLFreq_13M0Hz=> 13 MHz
// <cmuHFRCODPLLFreq_16M0Hz=> 16 MHz
// <cmuHFRCODPLLFreq_19M0Hz=> 19 MHz
// <cmuHFRCODPLLFreq_26M0Hz=> 26 MHz
// <cmuHFRCODPLLFreq_32M0Hz=> 32 MHz
// <cmuHFRCODPLLFreq_38M0Hz=> 38 MHz
// <cmuHFRCODPLLFreq_48M0Hz=> 48 MHz
// <cmuHFRCODPLLFreq_56M0Hz=> 56 MHz
// <cmuHFRCODPLLFreq_64M0Hz=> 64 MHz
// <cmuHFRCODPLLFreq_80M0Hz=> 80 MHz
// <i> Default: cmuHFRCODPLLFreq_80M0Hz
#define SL_DEVICE_INIT_HFRCO_BAND           cmuHFRCODPLLFreq_80M0Hz

// <<< end of configuration section >>>

#endif // SL_DEVICE_INIT_HFRCO_CONFIG_H
