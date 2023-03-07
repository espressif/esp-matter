/**************************************************************************/ /**
 * @file
 * @brief SE Manager configuration options
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

#ifndef SL_SE_MANAGER_CONFIG_H
#define SL_SE_MANAGER_CONFIG_H

/// This file include the configuration options of the SE Manager.
/// For the time being the user should not change the default settings
/// of the configuration options in this file.

#if defined (SL_COMPONENT_CATALOG_PRESENT)
  #include "sl_component_catalog.h"
#endif

#if defined(SL_CATALOG_MICRIUMOS_KERNEL_PRESENT) || defined(SL_CATALOG_FREERTOS_KERNEL_PRESENT)
// Threading support (as opposed to API calls only from a single thread)
// is currently required in RTOS mode.
  #define SL_SE_MANAGER_THREADING

  #if !defined(SL_SE_MANAGER_YIELD_WHILE_WAITING_FOR_COMMAND_COMPLETION) && !defined(CRYPTOACC_PRESENT)
// Enable yield support. Configure sl_se_command_context_t to yield CPU while waiting for SE commands.
// This is not supported on EFR32xG22.
    #define SL_SE_MANAGER_YIELD_WHILE_WAITING_FOR_COMMAND_COMPLETION
  #endif
#endif

// Check consistency of configuration options.
// Always include se_manager_check_config.h in order to assert that the
// configuration options dependencies and restrictions are aok.
#include "sl_se_manager_check_config.h"

#endif // SL_SE_MANAGER_CONFIG_H
