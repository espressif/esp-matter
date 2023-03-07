/***************************************************************************//**
 * @brief Connect Debug Print component configuration header.
 *
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

// <<< Use Configuration Wizard in Context Menu >>>

// <h>Connect Debug Print configuration

// <q CONNECT_DEBUG_PRINTS_ENABLED> Prints enabled
// <i> Default: 1
// <i> If this option is enabled, all enabled debug groups shall be enabled, otherwise all debug prints shall be disabled.
#define CONNECT_DEBUG_PRINTS_ENABLED                                         (1)

// <q CONNECT_DEBUG_STACK_GROUP_ENABLED> Stack group enabled
// <i> Default: 1
// <i> If this option is enabled, prints belonging to the "stack" group shall be enabled.
#define CONNECT_DEBUG_STACK_GROUP_ENABLED                                    (1)

// <q CONNECT_DEBUG_CORE_GROUP_ENABLED> Core group enabled
// <i> Default: 1
// <i> If this option is enabled, prints belonging to the "core" group shall be enabled.
#define CONNECT_DEBUG_CORE_GROUP_ENABLED                                     (1)

// <q CONNECT_DEBUG_APP_GROUP_ENABLED> App group enabled
// <i> Default: 1
// <i> If this option is enabled, prints belonging to the "app" group shall be enabled.
#define CONNECT_DEBUG_APP_GROUP_ENABLED                                      (1)

// </h>

// <<< end of configuration section >>>
