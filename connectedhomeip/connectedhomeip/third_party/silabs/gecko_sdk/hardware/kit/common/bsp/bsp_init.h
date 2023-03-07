/***************************************************************************//**
 * @file
 * @brief Board support package device initialization
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
#ifndef BSP_INIT_H
#define BSP_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup BSP
 * @{
 * @addtogroup BSP_INIT Device Initialization
 * @brief Device Initialization using HAL configuration
 * @details
 *  The BSP Device Initialization APIs provide functionality to initialize the
 *  device for a specific application and board based on a HAL configuration
 *  header file. The BSP Device Initialization module provides APIs that
 *  perform
 *
 *  - EFM32/EZR32/EFR32 errata workaround
 *  - DC-to-DC converter initialization on devices with DCDC
 *  - Clock initialization for crystal oscillators and clock selection for HF
 *    and LF clock trees
 *
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @brief Initialize the device using HAL config settings
 *
 * @details Perform errata workarounds, DC-to-DC converter initialization and
 *          clock initialization and selection
 ******************************************************************************/
void BSP_initDevice(void);

/***************************************************************************//**
 * @brief Configure and initialize the DC-to-DC converter
 *
 * @details Initialize the DC-to-DC converter if the board is wired for DCDC
 *          operation. Power down the DC-to-DC converter if the board is not
 *          wired for DCDC operation.
 ******************************************************************************/
void BSP_initDcdc(void);

/***************************************************************************//**
 * @brief Configure EMU energy modes
 *
 * @details Configure EM2/3.
 ******************************************************************************/
void BSP_initEmu(void);

/***************************************************************************//**
 * @brief Initialize crystal oscillators and configure clock trees
 *
 * @details Initialize HFXO and LFXO crystal oscillators if present.
 *          Select clock sources for HF and LF clock trees.
 ******************************************************************************/
void BSP_initClocks(void);

/***************************************************************************//**
 * @brief Initialize board based on HAL configuration
 *
 * @details Initialize peripherals to communicate with board components
 ******************************************************************************/
void BSP_initBoard(void);

/** @} endgroup BSP_INIT */
/** @} endgroup BSP */

#ifdef __cplusplus
}
#endif

#endif // BSP_INIT_H
