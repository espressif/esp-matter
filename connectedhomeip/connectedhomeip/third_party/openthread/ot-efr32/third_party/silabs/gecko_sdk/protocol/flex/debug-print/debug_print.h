/***************************************************************************//**
 * @file
 * @brief debug print API definition.
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

#ifndef DEBUG_PRINT_H
#define DEBUG_PRINT_H

#ifdef EMBER_TEST
#include "printf-sim.h"
#define local_printf printf_sim
#else
#include "printf.h"
#define local_printf printf
#endif

#include "debug_print_config.h"

// TODO: add doxygen

#if ((CONNECT_DEBUG_PRINTS_ENABLED == 1) && (CONNECT_DEBUG_STACK_GROUP_ENABLED == 1))
#define connect_stack_debug_print(...) local_printf(__VA_ARGS__)
#else
#define connect_stack_debug_print(...)
#endif

#if ((CONNECT_DEBUG_PRINTS_ENABLED == 1) && (CONNECT_DEBUG_CORE_GROUP_ENABLED == 1))
#define connect_core_debug_print(...) local_printf(__VA_ARGS__)
#else
#define connect_core_debug_print(...)
#endif

#if ((CONNECT_DEBUG_PRINTS_ENABLED == 1) && (CONNECT_DEBUG_APP_GROUP_ENABLED == 1))
#define connect_app_debug_print(...) local_printf(__VA_ARGS__)
#else
#define connect_app_debug_print(...)
#endif

#endif // DEBUG_PRINT_H
