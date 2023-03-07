/***************************************************************************//**
 * @file
 * @brief DEVICE_INIT_DPLL Config
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

#ifndef SL_DEVICE_INIT_DPLL_CONFIG_H
#define SL_DEVICE_INIT_DPLL_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <o SL_DEVICE_INIT_DPLL_FREQ> Target Frequency <1000000-80000000>
// <i> DPLL target frequency
// <i> Default: 80000000
#define SL_DEVICE_INIT_DPLL_FREQ                80000000

// <o SL_DEVICE_INIT_DPLL_N> Numerator (N) <300-4095>
// <i> Value of N for output frequency calculation fout = fref * (N+1) / (M+1)
// <i> Default: 3999
#define SL_DEVICE_INIT_DPLL_N                   3999

// <o SL_DEVICE_INIT_DPLL_M> Denominator (M) <0-4095>
// <i> Value of M for output frequency calculation fout = fref * (N+1) / (M+1)
// <i> Default: 1919
#define SL_DEVICE_INIT_DPLL_M                   1919

// <o SL_DEVICE_INIT_DPLL_REFCLK> Reference Clock
// <i> Reference clock source for DPLL
// <cmuSelect_HFXO=> HFXO
// <cmuSelect_LFXO=> LFXO
// <cmuSelect_CLKIN0=> CLKIN0
// <i> Default: cmuSelect_HFXO
#define SL_DEVICE_INIT_DPLL_REFCLK              cmuSelect_HFXO

// <o SL_DEVICE_INIT_DPLL_EDGE> Reference Clock Edge Detect
// <i> Edge detection for reference clock
// <cmuDPLLEdgeSel_Fall=> Falling Edge
// <cmuDPLLEdgeSel_Rise=> Rising Edge
// <i> Default: cmuDPLLEdgeSel_Fall
#define SL_DEVICE_INIT_DPLL_EDGE                cmuDPLLEdgeSel_Fall

// <o SL_DEVICE_INIT_DPLL_LOCKMODE> DPLL Lock Mode
// <i> Lock mode
// <cmuDPLLLockMode_Freq=> Frequency-Lock Loop
// <cmuDPLLLockMode_Phase=> Phase-Lock Loop
// <i> Default: cmuDPLLLockMode_Freq
#define SL_DEVICE_INIT_DPLL_LOCKMODE            cmuDPLLLockMode_Freq

// <q SL_DEVICE_INIT_DPLL_AUTORECOVER> Automatic Lock Recovery
// <i> Default: 1
#define SL_DEVICE_INIT_DPLL_AUTORECOVER         1

// <q SL_DEVICE_INIT_DPLL_DITHER> Enable Dither
// <i> Default: 0
#define SL_DEVICE_INIT_DPLL_DITHER              0

// <<< end of configuration section >>>

#endif // SL_DEVICE_INIT_DPLL_CONFIG_H
