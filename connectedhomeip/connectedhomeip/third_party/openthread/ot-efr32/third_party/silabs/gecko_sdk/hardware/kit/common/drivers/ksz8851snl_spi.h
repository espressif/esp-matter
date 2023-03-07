/***************************************************************************//**
 * @file
 * @brief SPI interface API for KSZ8851SNL Ethernet controller
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

#ifndef KSZ8851SNL_SPI_H__
#define KSZ8851SNL_SPI_H__

#ifndef SL_SUPRESS_DEPRECATION_WARNINGS_SDK_3_1
#warning "The KSZ8851SNL SPI driver is deprecated and marked for removal in a later release."
#endif

#include <stdint.h>

/**************************************************************************//**
* @addtogroup kitdrv
* @{
******************************************************************************/

/**************************************************************************//**
* @addtogroup ksz8851snl
* @{
******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

void KSZ8851SNL_SPI_Init(void);
uint16_t KSZ8851SNL_SPI_ReadRegister(uint8_t reg);
void KSZ8851SNL_SPI_WriteRegister(uint8_t reg, uint16_t value);
void KSZ8851SNL_SPI_ReadFifo(int numBytes, uint8_t *data);
void KSZ8851SNL_SPI_WriteFifoBegin(void);
void KSZ8851SNL_SPI_WriteFifo(int numBytes, const uint8_t *data);
void KSZ8851SNL_SPI_WriteFifoEnd(void);

#ifdef __cplusplus
}
#endif

/** @} (end group EthSpi) */
/** @} (end group kitdrv) */

#endif
