/***************************************************************************//**
 * @file
 * @brief Wake and sleep functionality
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

#ifndef SL_WAKE_LOCK_H
#define SL_WAKE_LOCK_H

#include "sl_wake_lock_config.h"

/**************************************************************************//**
 * Wake and sleep (lock) initialization function
 *****************************************************************************/
void sl_wake_lock_init(void);

/**************************************************************************//**
 * Signal wake-up to remote controller.
 *****************************************************************************/
void sl_wake_lock_set_remote_req(void);

/**************************************************************************//**
 * Signal go to sleep (lock) to remote controller.
 *****************************************************************************/
void sl_wake_lock_clear_remote_req(void);

/**************************************************************************//**
 * Wake-up signal arrived from remote controller.
 *****************************************************************************/
void sl_wake_lock_set_req_rx_cb(void);

/**************************************************************************//**
 * Go to sleep (lock) signal arrived from remote controller.
 *****************************************************************************/
void sl_wake_lock_clear_req_rx_cb(void);

/**************************************************************************//**
 * Enable sleep mode locally.
 *****************************************************************************/
void sl_wake_lock_set_local(void);

/**************************************************************************//**
 * Disable sleep mode locally.
 *****************************************************************************/
void sl_wake_lock_clear_local(void);

#endif // SL_WAKE_LOCK_H
