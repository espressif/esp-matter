/***************************************************************************//**
 * @file
 * @brief Retarget stdout to a serial port.
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

#ifndef __RETARGETSERIAL_H
#define __RETARGETSERIAL_H

#if defined(HAL_CONFIG)
#include "retargetserialhalconfig.h"
#else
#include "retargetserialconfig.h"
#endif
#include <stdbool.h>

/***************************************************************************//**
 * @addtogroup kitdrv
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup RetargetIo
 * @brief Standard I/O retargeting driver
 * @details
 *    This module provide low-level retargeting of stdio for all
 *    supported toolchains.
 * @{
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__CROSSWORKS_ARM)
int __putchar(int ch);
int __getchar(void);
#endif

int  RETARGET_ReadChar(void);
int  RETARGET_WriteChar(char c);

void RETARGET_SerialCrLf(int on);
void RETARGET_SerialInit(void);
bool RETARGET_SerialEnableFlowControl(void);
void RETARGET_SerialFlush(void);

void RETARGET_RequireEm1(bool requireEm1);

#ifdef __cplusplus
}
#endif

/** @} (end group RetargetIo) */
/** @} (end group kitdrv) */

#endif
