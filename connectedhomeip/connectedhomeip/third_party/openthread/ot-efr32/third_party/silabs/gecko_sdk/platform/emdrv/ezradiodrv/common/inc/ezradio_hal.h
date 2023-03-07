/***************************************************************************//**
 * @file
 * @brief This file contains EZRadio HAL.
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

#ifndef _EZRADIO_HAL_H_
#define _EZRADIO_HAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ezradiodrv_config.h"

/***************************************************************************//**
 * @addtogroup ezradiodrv
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup ezradiodrv_hal EZRADIODRV HAL
 * @brief EzRadio Hardware Abstraction Layer
 * @{
 ******************************************************************************/

void    ezradio_hal_GpioInit(GPIOINT_IrqCallbackPtr_t ezradioIrqCallback, bool enablePTI);
void    ezradio_hal_SpiInit(void);

void    ezradio_hal_AssertShutdown  (void);
void    ezradio_hal_DeassertShutdown(void);
void    ezradio_hal_ClearNsel       (void);
void    ezradio_hal_SetNsel         (void);
uint8_t ezradio_hal_NirqLevel       (void);

#if defined(EZRADIODRV_DISABLE_PTI) && defined(EZRADIODRV_COMM_USE_GPIO1_FOR_CTS)
uint8_t ezradio_hal_Gpio1Level(void);
#endif

void    ezradio_hal_SpiWriteByte    (uint8_t byteToWrite);
void    ezradio_hal_SpiReadByte     (uint8_t* readByte);

void    ezradio_hal_SpiWriteData    (uint8_t byteCount, uint8_t* pData);
void    ezradio_hal_SpiReadData     (uint8_t byteCount, uint8_t* pData);

void ezradio_hal_SpiWriteReadData(uint8_t byteCount, uint8_t* txData, uint8_t* rxData);

/** @} (end addtogroup ezradiodrv_hal) */
/** @} (end addtogroup ezradiodrv) */

#ifdef __cplusplus
}
#endif

#endif //_EZRADIO_HAL_H_
