/*
 * Copyright 2016-2021, Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
 *
 * This software, including source code, documentation and related
 * materials ("Software") is owned by Cypress Semiconductor Corporation
 * or one of its affiliates ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products.  Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */
/**
 * A few utilities with a reasonable implementation for SPAR.
 */
#pragma once

#ifdef __GNUC__
#define sprintf __2sprintf
#define snprintf __2snprintf
#endif

unsigned int _tx_v7m_get_and_disable_int(void);
void _tx_v7m_set_int(unsigned int posture);

#if ENABLE_DEBUG
#include "wiced_hal_wdog.h"
#include <string.h>

/// When debugging is enabled, sets up the HW for debugging.
#define SETUP_APP_FOR_DEBUG_IF_DEBUG_ENABLED()   do{        \
      wiced_hal_gpio_select_function(CY_PLATFORM_SWDCK, WICED_SWDCK); \
      wiced_hal_gpio_select_function(CY_PLATFORM_SWDIO, WICED_SWDIO); \
      wiced_hal_wdog_disable(); \
   }while(0)

/// Optionally waits in a pseudo while(1) until the user allows the CPU to continue
#define BUSY_WAIT_TILL_MANUAL_CONTINUE_IF_DEBUG_ENABLED()     do{   \
        volatile unsigned char spar_debug_continue = 0;             \
        unsigned int interrupt_save = _tx_v7m_get_and_disable_int();\
        while(!spar_debug_continue);                                \
        _tx_v7m_set_int(interrupt_save);                            \
        }while(0)
#else
#define SETUP_APP_FOR_DEBUG_IF_DEBUG_ENABLED()
#define BUSY_WAIT_TILL_MANUAL_CONTINUE_IF_DEBUG_ENABLED()
#endif

/// Allow the app to place code in retention RAM.
/// Note that there is very limited retention RAM, so choose
/// what goes into this area very carefully.
#define PLACE_CODE_IN_RETENTION_RAM    __attribute__ ((section(".code_in_retention_ram")))

/// Allow app to place this data in retention RAM.
#define PLACE_DATA_IN_RETENTION_RAM    __attribute__ ((section(".data_in_retention_ram")))

// If we panic from SPAR, we might not even have access to anything in
// the ROM or the Flash -- we suspect that we've been linked against
// the wrong image. So this.

#define SPAR_ASSERT_PANIC(expr) \
    do { if (!(expr)) while (1) ; } while(0)
