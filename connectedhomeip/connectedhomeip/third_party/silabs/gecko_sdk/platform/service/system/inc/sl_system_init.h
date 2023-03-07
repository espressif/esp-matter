/***************************************************************************//**
 * @file
 * @brief System Initialization.
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
#ifndef SL_SYSTEM_INIT_H
#define SL_SYSTEM_INIT_H

/***************************************************************************//**
 * @addtogroup system System Initialization and Action Processing
 * @brief System Initialization and Action Processing
 * @details
 * ### System Init
 * System Init provides a function for initializing the system and the products:
 *
 *   - sl_system_init().
 *
 * This function calls a set of functions that are automatically generated
 * and located in `autogen/sl_event_handler.c`. Handlers can be registered
 * for the following events using the Event Handler API provided by the
 * Event Handler component:
 *
 *   - platform_init      -> sl_platform_init()
 *   - driver_init        -> sl_driver_init()
 *   - service_init       -> sl_service_init()
 *   - stack_init         -> sl_stack_init()
 *   - internal_app_init  -> sl_internal_app_init()
 *
 * These events are fired in the order listed above when `sl_system_init()`
 * is called.
 *
 * ### System Kernel
 *
 * System Kernel component provides a function for starting the kernel:
 *
 *    - sl_system_kernel_start().
 *
 *  This function calls a functions that is automatically generated
 *  and located in `$autogen/sl_event_handler.c`. Handlers can be registered
 *  for the following events using the Event Handler API provided by the
 *  Event Handler component:
 *
 *    - kernel_start -> sl_kernel_start()
 *
 *  The event is fired when `sl_system_kernel_start()` is called.
 *
 * ### System Process Action
 *
 * System Process Action component provides a function for running
 * the products from a super loop:
 *
 *   - sl_system_process_action().
 *
 * This function calls a set of functions that are automatically generated
 * and located in `$autogen/sl_event_handler.c`. Handlers can be registered
 * for the following events using the Event Handler API provided by the
 * Event Handler component:
 *
 *   - platform_process_action     -> sl_platform_process_action()
 *   - service_process_action      -> sl_service_process_action()
 *   - stack_process_action        -> sl_stack_process_action()
 *   - internal_app_process_action -> sl_internal_process_action()
 *
 * These events are fired in the order listed above when `sl_system_process_action()`
 * is called.
 *
 * Usage example:
 *
 * @code{.c}
 * #if defined(SL_COMPONENT_CATALOG_PRESENT)
 * #include "sl_component_catalog.h"
 * #endif
 * #include "sl_system_init.h"
 * #include "sl_power_manager.h"
 * #include "app.h"
 * #if defined(SL_CATALOG_KERNEL_PRESENT)
 * #include "sl_system_kernel.h"
 * #else
 * #include "sl_system_process_action.h"
 * #endif
 *
 * int main(void)
 * {
 *   // Initialize Silicon Labs device, system, service(s) and protocol stack(s).
 *   // Note that if the kernel is present, processing task(s) will be created by
 *   // this call.
 *   sl_system_init();
 *
 *   // Initialize the application.
 *   app_init();
 *
 * #if defined(SL_CATALOG_KERNEL_PRESENT)
 *   // Start the kernel. Task(s) created in app_init() will start running.
 *   sl_system_kernel_start();
 * #else
 *   do {
 *     // Do not remove this call: Silicon Labs components process action routine
 *     // must be called from the super loop.
 *     sl_system_process_action();
 *
 *     // Application process.
 *     app_process_action();
 *
 *     // Let the CPU go to sleep if the system allow it.
 *     sl_power_manager_sleep();
 *   } while (1);
 * #endif // SL_CATALOG_KERNEL_PRESENT
 * }
 * @endcode
 * @{
 *
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize Silicon Labs products
 */
void sl_system_init(void);

#ifdef __cplusplus
}
#endif

/** @} (end addtogroup system) */

#endif // SL_SYSTEM_INIT_H
