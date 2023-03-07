/***************************************************************************//**
 * @file
 * @brief Device initialization for NVIC.
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef SL_DEVICE_INIT_NVIC_H
#define SL_DEVICE_INIT_NVIC_H

#include "sl_status.h"

/**
 * @addtogroup device_init
 * @{
 * @addtogroup device_init_nvic NVIC Initialization
 * @brief Initialize the NVIC Interrupt Handlers priority.
 * @details
 * Set all interrupt priorities, except the Fault exceptions, to
 * `CORE_INTERRUPT_DEFAULT_PRIORITY` value.
 *
 * When the Device Init NVIC module is present and no configuration overwrites
 * the em_core `CORE_ATOMIC_METHOD` define, the atomic method is automatically
 * set to BASEPRI (`CORE_ATOMIC_METHOD_BASEPRI`).
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * Sets default priority for all configurable interrupts in NVIC.
 *
 * @return Status code
 ******************************************************************************/
sl_status_t sl_device_init_nvic(void);

#ifdef __cplusplus
}
#endif

/**
 * @} device_init_nvic
 * @} device_init
 */

#endif // SL_DEVICE_INIT_NVIC_H
