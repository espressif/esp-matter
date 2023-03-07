/***************************************************************************//**
 * @file
 * @brief Microsecond delay function API definition.
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
#ifndef __SILICON_LABS_USTIMER_H
#define __SILICON_LABS_USTIMER_H

#include <stdint.h>
#include "ecode.h"
#include "ustimer_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup ustimer
 * @{
 ******************************************************************************/

#define ECODE_EMDRV_USTIMER_OK ( ECODE_OK ) ///< Success return value.

Ecode_t USTIMER_Init(void);
Ecode_t USTIMER_DeInit(void);
Ecode_t USTIMER_Delay(uint32_t usec);
Ecode_t USTIMER_DelayIntSafe(uint32_t usec);

#ifdef __cplusplus
}
#endif

/** @} (end group ustimer) */

#endif
