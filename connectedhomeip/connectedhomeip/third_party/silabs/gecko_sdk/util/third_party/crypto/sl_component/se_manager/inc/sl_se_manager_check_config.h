/**************************************************************************/ /**
 * @file
 * @brief Consistency checks for SE Manager configuration options
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

#ifndef SL_SE_MANAGER_CHECK_CONFIG_H
#define SL_SE_MANAGER_CHECK_CONFIG_H

#if defined (SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif

#if defined(SL_SE_MANAGER_THREADING)                                    \
  && !defined(SL_SE_MANAGER_YIELD_WHILE_WAITING_FOR_COMMAND_COMPLETION) \
  && !defined(CRYPTOACC_PRESENT)
#error "Yield when waiting for SE commands to finish is currently required in RTOS mode."
#endif
#if defined(SL_SE_MANAGER_YIELD_WHILE_WAITING_FOR_COMMAND_COMPLETION) \
  && !defined(SL_SE_MANAGER_THREADING)
#error "Yield when waiting for SE commands to finish currently requires RTOS mode. I.e. yield support is not available in bare metal mode."
#endif

#if (defined(SL_CATALOG_MICRIUMOS_KERNEL_PRESENT) || defined(SL_CATALOG_FREERTOS_KERNEL_PRESENT)) \
  && !defined(SL_SE_MANAGER_THREADING)
#error "RTOS requires threading mode."
#endif

#if (defined(CRYPTOACC_PRESENT) && defined(SL_SE_MANAGER_YIELD_WHILE_WAITING_FOR_COMMAND_COMPLETION))
#error "Yield support is not available on EFR32xG22 devices"
#endif

#endif // SL_SE_MANAGER_CHECK_CONFIG_H
