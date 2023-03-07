/***************************************************************************//**
 * @file
 * @brief Module Configuration Header
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
 
#ifndef SL_MODULE_MGM210PA22JIA2_H
#define SL_MODULE_MGM210PA22JIA2_H

#include "em_cmu.h"
#include "em_emu.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BSP_CLK_HFXO_CTUNE_TOKEN
#define BSP_CLK_HFXO_CTUNE_TOKEN	1U 
#endif //BSP_CLK_HFXO_CTUNE_TOKEN
#define BSP_CLK_HFXO_FREQ 			38400000UL
#define BSP_CLK_HFXO_INIT 			CMU_HFXOINIT_DEFAULT
#define BSP_CLK_HFXO_PRESENT 		1U

#ifndef BSP_PA_VOLTAGE
#define BSP_PA_VOLTAGE 			3300U
#endif //BSP_PA_VOLTAGE

#ifdef __cplusplus
}
#endif

#endif // SL_MODULE_MGM210PA22JIA2_H
