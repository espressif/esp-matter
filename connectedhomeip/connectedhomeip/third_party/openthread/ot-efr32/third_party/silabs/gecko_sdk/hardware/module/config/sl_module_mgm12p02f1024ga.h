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
 
#ifndef SL_MODULE_MGM12P02F1024GA_H
#define SL_MODULE_MGM12P02F1024GA_H

#include "em_cmu.h"
#include "em_emu.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BSP_CLK_HFXO_CTUNE 		282
#define BSP_CLK_HFXO_FREQ 		38400000UL
#define BSP_CLK_HFXO_INIT 		CMU_HFXOINIT_DEFAULT
#define BSP_CLK_HFXO_PRESENT 	1U

#define BSP_CLK_LFXO_CTUNE 		32U 
#define BSP_CLK_LFXO_FREQ 		32768UL
#define BSP_CLK_LFXO_INIT 		CMU_LFXOINIT_DEFAULT
#define BSP_CLK_LFXO_PRESENT 	1U

#define BSP_DCDC_INIT 			EMU_DCDCINIT_DEFAULT
#define BSP_DCDC_PRESENT 		1U

#ifndef HAL_DCDC_BYPASS
#define HAL_DCDC_BYPASS 		0U
#endif //HAL_DCDC_BYPASS

#ifndef BSP_PA_VOLTAGE
#define BSP_PA_VOLTAGE 			1800U
#endif //BSP_PA_VOLTAGE

#ifdef __cplusplus
}
#endif

#endif // SL_MODULE_MGM12P02F1024GA_H
