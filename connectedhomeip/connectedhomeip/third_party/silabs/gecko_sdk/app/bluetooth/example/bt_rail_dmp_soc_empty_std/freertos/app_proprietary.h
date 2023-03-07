/***************************************************************************//**
 * @file
 * @brief Propriatery application interface.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef APP_PROPRIETARY_H
#define APP_PROPRIETARY_H

#include "FreeRTOS.h"
#include "event_groups.h"

// Dummy OS event flag
#define APP_PROPRIETARY_EVENT_FLAG        (1 << 0)

/// OS event to communicate with the proprietary task
extern EventGroupHandle_t app_proprietary_event_group_handle;

/**************************************************************************//**
 * Proprietary application init.
 *****************************************************************************/
void app_proprietary_init(void);

#endif // APP_PROPRIETARY_H
