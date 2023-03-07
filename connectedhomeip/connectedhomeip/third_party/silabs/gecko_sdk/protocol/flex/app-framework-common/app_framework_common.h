/***************************************************************************//**
 * @brief Connect Application Framework common code.
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

#ifndef APP_FRAMEWORK_COMMON_H
#define APP_FRAMEWORK_COMMON_H

#include "stack/include/ember.h"

void connect_stack_init(void);
void connect_app_framework_init(void);
void connect_sleep_init(void);

void connect_stack_tick(void);
void connect_app_framework_tick(void);
void connect_sleep_tick(void);

bool connect_is_ok_to_sleep(void);

void connect_standard_phy_2_4g(void);

/**
 * @addtogroup app_framework_common
 * @{
 */

/**
 *
 * @brief Allocate a new event to the app event table.
 *
 *  @param[out] control   The EmberEventControl to allocate
 *
 *  @param[in] handler   Pointer to the handler function associated to the event
 *
 *  @return   An ::EmberStatus value of:
 *  - ::EMBER_SUCCESS if the event was successfully allocated.
 *  - ::EMBER_TABLE_FULL if no more event could be allocated.
 *  @sa emberAfAllocateEvent()
 */
EmberStatus emberAfAllocateEvent(EmberEventControl **control, void (*handler)(void));
/**
 * @}
 */

#endif // APP_FRAMEWORK_COMMON_H
