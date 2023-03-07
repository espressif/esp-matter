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
 
#ifndef SL_MODULE_MGM12P32F1024GE_H
#define SL_MODULE_MGM12P32F1024GE_H

#include "em_cmu.h"
#include "em_emu.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BSP_CLK_HFXO_CTUNE 		282
#define BSP_CLK_HFXO_FREQ 		38400000UL
#define BSP_CLK_HFXO_INIT 		CMU_HFXOINIT_DEFAULT
#define BSP_CLK_HFXO_PRESENT 	1

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
#define BSP_PA_VOLTAGE 			3300U
#endif //BSP_PA_VOLTAGE
 
// Configure integrated FEM
#define HAL_FEM_ENABLE 	        1U
#define HAL_FEM_RX_ACTIVE 		1U
#define BSP_FEM_RX_PIN 			10U
#define BSP_FEM_RX_PORT 		gpioPortD
#define BSP_FEM_RX_LOC  		0U
#define BSP_FEM_RX_CHANNEL 		5U
#define BSP_FEM_SLEEP_PIN 		11U
#define BSP_FEM_SLEEP_PORT 		gpioPortD
#define BSP_FEM_SLEEP_LOC 		13U
#define BSP_FEM_SLEEP_CHANNEL 	6U
#define HAL_FEM_TX_ACTIVE 		0U
#define BSP_FEM_TX_PIN 			10U
#define BSP_FEM_TX_PORT 		gpioPortD
#define BSP_FEM_TX_LOC  		0U
#define BSP_FEM_TX_CHANNEL 		5U
// Disable other FEM settings
#define HAL_FEM_TX_HIGH_POWER 	0U
#define BSP_FEM_TXPOWER         0U
#define HAL_FEM_BYPASS_ENABLE 	0U
#define BSP_FEM_BYPASS          0U


#ifdef __cplusplus
}
#endif

#endif // SL_MODULE_MGM12P32F1024GE_H
