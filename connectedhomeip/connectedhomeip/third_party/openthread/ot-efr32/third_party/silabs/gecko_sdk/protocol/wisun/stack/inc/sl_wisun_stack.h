/***************************************************************************//**
 * @file sl_wisun_stack.h
 * @brief Wi-SUN stack API
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_WISUN_STACK_H
#define SL_WISUN_STACK_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************//**
 * Initialize Wi-SUN stack
 *****************************************************************************/
void sl_wisun_stack_init();

/**************************************************************************//**
 * Poll Wi-SUN stack for pending events.
 *
 * If there are none, the call will return immediately. Otherwise, a single
 * event is delivered via the callback handler sl_wisun_on_event().
 *****************************************************************************/
void sl_wisun_stack_step();

#ifdef __cplusplus
}
#endif

#endif  // SL_WISUN_STACK_H
