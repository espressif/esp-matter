/***************************************************************************//**
 * @file
 * @brief Power Manager API definition.
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

#ifndef SL_POWER_MANAGER_H
#define SL_POWER_MANAGER_H

#ifndef SL_POWER_MANAGER_DEBUG
#include "sl_power_manager_config.h"
#endif
#include "em_device.h"
#include "em_core.h"
#include "sl_slist.h"
#include "sl_status.h"
#include "sl_sleeptimer.h"
#include "sl_enum.h"
#include "em_emu.h"

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup power_manager Power Manager
 *
 * @details Power manager is a platform level software module that manages
 * the system's energy modes. Its main purpose is to transition the system to a
 * low energy mode when the processor has nothing to execute. The energy mode the
 * system will transition to is determined each time the system goes to sleep
 * using requirements. These requirements are set by the different software modules
 * (drivers, stacks, application code, etc...). Power manager also ensures a
 * strict control of some power hungry resources such as the high frequency
 * external oscillator (normally called HFXO). Power manager also
 * offers a notification mechanism through which any piece of software module can be
 * notified of energy mode transitions through callbacks.
 *
 * @note Sleep Driver is deprecated. Use Power Manager for all sleep-related
 *       operations. See <a href="https://www.silabs.com/documents/
 *       public/application-notes/
 *       an1358-migrating-from-sleep-driver-to-power-manager.pdf">AN1358:
 *       Migrating from Sleep Driver to Power Manager</a> for information on how
 *       to migrate from Sleep Driver to Power Manager.
 *
 * @details
 * ## Initialization
 *
 *   Power manager must be initialized prior to any call to power manager API.
 *   If sl_system is used, only sl_system_init() must be called, otherwise
 *   sl_power_manager_init() must be called manually. Note that power manager
 *   must be initialized after the clock(s), when initialized manually, as the
 *   power manager check which oscillators are used during the initialization phase.
 *
 * ## Add and remove requirements
 *
 *   The drivers should add and remove energy mode requirements, at runtime, on the
 *   lowest energy mode for them depending on their state. When calling
 *   sl_power_manager_sleep(), the lowest possible Energy mode will be automatically
 *   selected.
 *
 *   It is possible to add and remove requirements from ISR. If a specific energy mode
 *   is required in the ISR, but not required to generate the interrupt, a requirement
 *   on the energy mode can be added from the ISR. It is guaranteed that the associated
 *   clock will be active once sl_power_manager_add_requirement() returns. The EM
 *   requirement can be also be removed from an ISR.
 *
 * ## Subscribe to events
 *
 *   It possible to get notified when the system transition from a power level to
 *   another power level. This can allow to do some operations depending on which level
 *   the system goes, such as saving/restoring context.
 *
 * ## Sleep
 *
 *   When the software has no more operation and only need to wait for an event, the
 *   software must call sl_power_manager_sleep(). This is automatically done when the
 *   kernel is present, but it needs to be called from the super loop in a baremetal
 *   project.
 *
 * ## Query callback functions
 *
 * ### Is OK to sleep
 *
 *   Between the time `sl_power_manager_sleep` is called and the MCU is really put
 *   in a lower Energy mode, it is possible that an ISR occur and require the system
 *   to resume at that time instead of sleeping. So a callback is called in a critical
 *   section to validate that the MCU can go to sleep.
 *
 *   In case of an application that runs on an RTOS, the RTOS will take care of determining
 *   if it is ok to sleep. In case of a baremetal application, the function `sl_power_manager_is_ok_to_sleep()`
 *   will be generated automatically by Simplicity Studio's wizard.
 *   The function will look at multiple software modules from the SDK to take a decision.
 *   The application can contribute to the decision by defining the function `app_is_ok_to_sleep()`.
 *   If any of the software modules (including the application via `app_is_ok_to_sleep()`) return false,
 *   the process of entering in sleep will be aborted.
 *
 * ### Sleep on ISR exit
 *
 *   When the system enters sleep, the only way to wake it up is via an interrupt or
 *   exception. By default, power manager will assume that when an interrupt
 *   occurs and the corresponding ISR has been executed, the system must not go back
 *   to sleep. However, in the case where all the processing related to this interrupt
 *   is performed in the ISR, it is possible to go back to sleep by using this hook.
 *
 *   In case of an application that runs on an RTOS, the RTOS will take care of determining
 *   if the system can go back to sleep on ISR exit. Power manager will ensure the system resumes
 *   its operations as soon as a task is resumed, posted or that its delay expires.
 *   In case of a baremetal application, the function `sl_power_manager_sleep_on_isr_exit()` will be generated
 *   automatically by Simplicity Studio's wizard. The function will look at multiple software modules from the SDK
 *   to take a decision. The application can contribute to the decision by defining the
 *   function `app_sleep_on_isr_exit()`.
 *   The generated function will take a decision based on the value returned by the different software modules
 *   (including the application via `app_sleep_on_isr_exit()`):
 *
 *   `SL_POWER_MANAGER_IGNORE`: if the software module did not cause the system wakeup and/or doesn't want to contribute to the decision.
 *   `SL_POWER_MANAGER_SLEEP`: if the software module did cause the system wakeup, but the system should go back to sleep.
 *   `SL_POWER_MANAGER_WAKEUP`: if the software module did cause the system wakeup, and the system should not go back to sleep.
 *
 *   If any software module returned `SL_POWER_MANAGER_SLEEP` and none returned `SL_POWER_MANAGER_WAKEUP`,
 *   the system will go back to sleep. Any other combination will cause the system not to go back to sleep.
 *
 * ### Debugging feature
 *
 *   By setting the configuration define SL_POWER_MANAGER_DEBUG to 1, it is possible
 *   to record the requirements currently set and their owner. It is possible to print
 *   at any time a table that lists all the added requirements and their owner. This
 *   table can be printed by caling the function
 *   sl_power_manager_debug_print_em_requirements().
 *   Make sure to add the following define
 *   ```
 *   #define CURRENT_MODULE_NAME "<Module printable name here>"
 *   ```
 *   to any application code source file that adds and removes requirements.
 *
 * ## Usage Example
 *
 * ```
 * #define EM_EVENT_MASK_ALL  (SL_POWER_MANAGER_EVENT_TRANSITION_ENTERING_EM0   \
 *                             | SL_POWER_MANAGER_EVENT_TRANSITION_LEAVING_EM0  \
 *                             | SL_POWER_MANAGER_EVENT_TRANSITION_ENTERING_EM1 \
 *                             | SL_POWER_MANAGER_EVENT_TRANSITION_LEAVING_EM1  \
 *                             | SL_POWER_MANAGER_EVENT_TRANSITION_ENTERING_EM2 \
 *                             | SL_POWER_MANAGER_EVENT_TRANSITION_LEAVING_EM2  \
 *                             | SL_POWER_MANAGER_EVENT_TRANSITION_ENTERING_EM3 \
 *                             | SL_POWER_MANAGER_EVENT_TRANSITION_LEAVING_EM3)
 *
 * sl_power_manager_em_transition_event_handle_t event_handle;
 * sl_power_manager_em_transition_event_info_t event_info = {
 *   .event_mask = EM_EVENT_MASK_ALL,
 *   .on_event = my_events_callback,
 * }
 *
 * void main(void)
 * {
 *   // Initialize power manager; not needed if sl_system_init() is used.
 *   sl_power_manager_init();
 *
 *   // Limit sleep level to EM1
 *   sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM1);
 *
 *   // Subscribe to all event types; get notified for every power transition.
 *   sl_power_manager_subscribe_em_transition_event(&event_handle, &event_info);
 *   while (1) {
 *     // Actions
 *     [...]
 *     if (completed) {
 *        // Remove energy mode requirement, can go to EM2 or EM3 now, depending on the configuration
 *        sl_power_manager_remove_em_requirement(SL_POWER_MANAGER_EM1);
 *     }
 *
 *     // Sleep to lowest possible energy mode; This call is not needed when using the kernel.
 *     sl_power_manager_sleep();
 *     // Will resume after an interrupt or exception
 *   }
 * }
 *
 * void my_events_callback(sl_power_manager_em_t from,
 *                         sl_power_manager_em_t to)
 * {
 *   printf("Event:%s-%s\r\n", string_lookup_table[from], string_lookup_table[to]);
 * }
 * ```
 *
 * @{
 ******************************************************************************/

// -----------------------------------------------------------------------------
// Defines

// Current module name for debugging features
#ifndef CURRENT_MODULE_NAME
#define CURRENT_MODULE_NAME "Anonymous"
#endif

// Power transition events
#define SL_POWER_MANAGER_EVENT_TRANSITION_ENTERING_EM0     (1 << 0)
#define SL_POWER_MANAGER_EVENT_TRANSITION_LEAVING_EM0      (1 << 1)
#define SL_POWER_MANAGER_EVENT_TRANSITION_ENTERING_EM1     (1 << 2)
#define SL_POWER_MANAGER_EVENT_TRANSITION_LEAVING_EM1      (1 << 3)
#define SL_POWER_MANAGER_EVENT_TRANSITION_ENTERING_EM2     (1 << 4)
#define SL_POWER_MANAGER_EVENT_TRANSITION_LEAVING_EM2      (1 << 5)
#define SL_POWER_MANAGER_EVENT_TRANSITION_ENTERING_EM3     (1 << 6)
#define SL_POWER_MANAGER_EVENT_TRANSITION_LEAVING_EM3      (1 << 7)
/// @endcond

// -----------------------------------------------------------------------------
// Data Types

/// @brief Energy modes
typedef  enum  {
  SL_POWER_MANAGER_EM0 = 0,   ///< Run Mode (Energy Mode 0)
  SL_POWER_MANAGER_EM1,       ///< Sleep Mode (Energy Mode 1)
  SL_POWER_MANAGER_EM2,       ///< Deep Sleep Mode (Energy Mode 2)
  SL_POWER_MANAGER_EM3,       ///< Stop Mode (Energy Mode 3)
  SL_POWER_MANAGER_EM4,       ///< Shutoff Mode (Energy Mode 4)
} sl_power_manager_em_t;

/// @brief Mask of all the event(s) to listen to.
typedef uint32_t sl_power_manager_em_transition_event_t;

/***************************************************************************//**
 * Typedef for the user supplied callback function which is called when
 * an energy mode transition occurs.
 *
 * @param from Energy mode we are leaving.
 * @param to   Energy mode we are entering.
 ******************************************************************************/
typedef void (*sl_power_manager_em_transition_on_event_t)(sl_power_manager_em_t from,
                                                          sl_power_manager_em_t to);

/// @brief Struct representing energy mode transition event information
typedef struct {
  const sl_power_manager_em_transition_event_t event_mask;  ///< Mask of the transitions on which the callback should be called.
  const sl_power_manager_em_transition_on_event_t on_event; ///< Function that must be called when the event occurs.
} sl_power_manager_em_transition_event_info_t;

/// @brief Struct representing energy mode transition event handle
typedef struct {
  sl_slist_node_t node;                               ///< List node.
  sl_power_manager_em_transition_event_info_t *info;  ///< Handle event info.
} sl_power_manager_em_transition_event_handle_t;

/// On ISR Exit Hook answer
SL_ENUM(sl_power_manager_on_isr_exit_t) {
  SL_POWER_MANAGER_IGNORE = (1UL << 0UL),     ///< The module did not trigger an ISR and it doesn't want to contribute to the decision
  SL_POWER_MANAGER_SLEEP  = (1UL << 1UL),     ///< The module was the one that caused the system wakeup and the system SHOULD go back to sleep
  SL_POWER_MANAGER_WAKEUP = (1UL << 2UL),     ///< The module was the one that caused the system wakeup and the system MUST NOT go back to sleep
};

// -----------------------------------------------------------------------------
// Internal Prototypes only to be used by Power Manager module
void sli_power_manager_update_em_requirement(sl_power_manager_em_t em,
                                             bool  add);

// To make sure that we are able to optimize out the string argument when the
// debug feature is disable, we use a pre-processor macro resulting in a no-op.
// We also make sure to always have a definition for the function regardless if
// the debug feature is enable or not for binary compatibility.
#if (SL_POWER_MANAGER_DEBUG == 1)
void sli_power_manager_debug_log_em_requirement(sl_power_manager_em_t em,
                                                bool                  add,
                                                const char            *name);
#else
#define sli_power_manager_debug_log_em_requirement(em, add, name) /* no-op */
#endif

// -----------------------------------------------------------------------------
// Prototypes

/***************************************************************************//**
 * Initialize Power Manager module.
 * @return Status code
 ******************************************************************************/
sl_status_t sl_power_manager_init(void);

/***************************************************************************//**
 * Sleep at the lowest allowed energy mode.
 *
 * @note Must not be called from ISR
 * @par
 * @note This function will expect and call a callback with the following
 *       signature: `bool sl_power_manager_is_ok_to_sleep(void)`
 *
 * @note This function can be used to cancel a sleep action and handle the
 *       possible race condition where an ISR that would cause a wakeup is
 *       triggered right after the decision to call sl_power_manager_sleep()
 *       has been made.
 *
 * Usage example:
 *
 * ```c
 * void main(void)
 * {
 *   sl_power_manager_init();
 *   while (1) {
 *     tick();
 *     sl_power_manager_sleep();
 *   }
 * }
 * ```
 ******************************************************************************/
void sl_power_manager_sleep(void);

/***************************************************************************//**
 * Adds requirement on given energy mode.
 *
 * @param em  Energy mode to add the requirement to:
 *            - ::SL_POWER_MANAGER_EM1
 *            - ::SL_POWER_MANAGER_EM2
 ******************************************************************************/
__STATIC_INLINE void sl_power_manager_add_em_requirement(sl_power_manager_em_t em)
{
#if (SL_POWER_MANAGER_LOWEST_EM_ALLOWED != 1)
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_CRITICAL();
  sli_power_manager_update_em_requirement(em, true);

  sli_power_manager_debug_log_em_requirement(em, true, (const char *)CURRENT_MODULE_NAME);
  CORE_EXIT_CRITICAL();
#else
  (void)em;
#endif
}

/***************************************************************************//**
 * Removes requirement on given energy mode.
 *
 * @param em  Energy mode to remove the requirement to:
 *            - ::SL_POWER_MANAGER_EM1
 *            - ::SL_POWER_MANAGER_EM2
 ******************************************************************************/
__STATIC_INLINE void sl_power_manager_remove_em_requirement(sl_power_manager_em_t em)
{
#if (SL_POWER_MANAGER_LOWEST_EM_ALLOWED != 1)
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_CRITICAL();
  sli_power_manager_update_em_requirement(em, false);

  sli_power_manager_debug_log_em_requirement(em, false, (const char *)CURRENT_MODULE_NAME);
  CORE_EXIT_CRITICAL();
#else
  (void)em;
#endif
}

/***************************************************************************//**
 * Registers a callback to be called on given Energy Mode transition(s).
 *
 * @param event_handle  Event handle (no initialization needed).
 *
 * @param event_info    Event info structure that contains the event mask and the
 *                      callback that must be called.
 *
 * @note Adding and removing requirement(s) from a callback on a transition event
 *       is not supported.
 *
 * @note The parameters passed must be persistent, meaning that they need to survive
 *       until the callback fires.
 *
 * Usage example:
 *
 * ```c
 * #define EM_EVENT_MASK_ALL      (  SL_POWER_MANAGER_EVENT_TRANSITION_ENTERING_EM0 \
 *                                 | SL_POWER_MANAGER_EVENT_TRANSITION_LEAVING_EM0  \
 *                                 | SL_POWER_MANAGER_EVENT_TRANSITION_ENTERING_EM1 \
 *                                 | SL_POWER_MANAGER_EVENT_TRANSITION_LEAVING_EM1  \
 *                                 | SL_POWER_MANAGER_EVENT_TRANSITION_ENTERING_EM2 \
 *                                 | SL_POWER_MANAGER_EVENT_TRANSITION_LEAVING_EM2  \
 *                                 | SL_POWER_MANAGER_EVENT_TRANSITION_ENTERING_EM3 \
 *                                 | SL_POWER_MANAGER_EVENT_TRANSITION_LEAVING_EM3)
 *
 * sl_power_manager_em_transition_event_handle_t event_handle;
 * sl_power_manager_em_transition_event_info_t event_info = {
 *   .event_mask = EM_EVENT_MASK_ALL,
 *   .on_event = my_callback,
 * };
 *
 * void my_callback(sl_power_manager_em_t from,
 *                  sl_power_manager_em_t to)
 * {
 *   [...]
 * }
 *
 * void main(void)
 * {
 *   sl_power_manager_init();
 *   sl_power_manager_subscribe_em_transition_event(&event_handle, &event_info);
 * }
 * ```
 ******************************************************************************/
void sl_power_manager_subscribe_em_transition_event(sl_power_manager_em_transition_event_handle_t     *event_handle,
                                                    const sl_power_manager_em_transition_event_info_t *event_info);

/***************************************************************************//**
 * Unregisters an event callback handle on Energy mode transition.
 *
 * @param event_handle  Event handle which must be unregistered (must have been
 *                      registered previously).
 *
 * @note  An EFM_ASSERT is thrown if the handle is not found.
 ******************************************************************************/
void sl_power_manager_unsubscribe_em_transition_event(sl_power_manager_em_transition_event_handle_t *event_handle);

/***************************************************************************//**
 * Get configurable overhead value for early restore time in Sleeptimer ticks
 * when a schedule wake-up is set.
 *
 * @return  Current overhead value for early restore time.
 *
 * @note This function will return 0 in case SL_POWER_MANAGER_LOWEST_EM_ALLOWED
 *       config is set to EM1.
 ******************************************************************************/
int32_t sl_power_manager_schedule_wakeup_get_restore_overhead_tick(void);

/***************************************************************************//**
 * Set configurable overhead value for early restore time in Sleeptimer ticks
 * used for schedule wake-up.
 * Must be called after initialization else the value will be overwritten.
 *
 * @param overhead_tick Overhead value to set for early restore time.
 *
 * @note The overhead value can also be negative to remove time from the restore
 *       process.
 *
 * @note This function will do nothing when SL_POWER_MANAGER_LOWEST_EM_ALLOWED
 *       config is set to EM1.
 ******************************************************************************/
void sl_power_manager_schedule_wakeup_set_restore_overhead_tick(int32_t overhead_tick);

/***************************************************************************//**
 * Get configurable minimum off-time value for schedule wake-up in Sleeptimer
 * ticks.
 *
 * @return  Current minimum off-time value for schedule wake-up.
 *
 * @note  Turning on external high frequency clock, such as HFXO, requires more
 *        energy since we must supply higher current for the wake-up.
 *        Therefore, when an 'external high frequency clock enable' is scheduled
 *        in 'x' time, there is a threshold 'x' value where turning off the clock
 *        is not worthwhile since the energy consumed by taking into account the
 *        wake-up will be greater than if we just keep the clock on until the next
 *        scheduled clock enabled. This threshold value is what we refer as the
 *        minimum off-time.
 *
 * @note This function will return 0 in case SL_POWER_MANAGER_LOWEST_EM_ALLOWED
 *       config is set to EM1.
 ******************************************************************************/
uint32_t sl_power_manager_schedule_wakeup_get_minimum_offtime_tick(void);

/***************************************************************************//**
 * Set configurable minimum off-time value for schedule wake-up in Sleeptimer
 * ticks.
 *
 * @param minimum_offtime_tick  minimum off-time value to set for schedule
 *                              wake-up.
 *
 * @note  Turning on external high frequency clock, such as HFXO, requires more
 *        energy since we must supply higher current for the wake-up.
 *        Therefore, when an 'external high frequency clock enable' is scheduled
 *        in 'x' time, there is a threshold 'x' value where turning off the clock
 *        is not worthwhile since the energy consumed by taking into account the
 *        wake-up will be greater than if we just keep the clock on until the next
 *        scheduled clock enabled. This threshold value is what we refer as the
 *        minimum off-time.
 *
 * @note This function will do nothing when SL_POWER_MANAGER_LOWEST_EM_ALLOWED
 *       config is set to EM1.
 ******************************************************************************/
void sl_power_manager_schedule_wakeup_set_minimum_offtime_tick(uint32_t minimum_offtime_tick);

#if defined(EMU_VSCALE_PRESENT)
/***************************************************************************//**
 * Enable or disable fast wake-up in EM2 and EM3
 *
 * @note Will also update the wake up time from EM2 to EM0.
 *
 * @note This function will do nothing when SL_POWER_MANAGER_LOWEST_EM_ALLOWED
 *       config is set to EM1.
 ******************************************************************************/
void sl_power_manager_em23_voltage_scaling_enable_fast_wakeup(bool enable);
#endif

/**************************************************************************//**
 * Determines if the HFXO interrupt was part of the last wake-up and/or if
 * the HFXO early wakeup expired during the last ISR
 * and if it was the only timer to expire in that period.
 *
 * @return true if power manager sleep can return to sleep,
 *         false otherwise.
 *
 * @note This function will always return false in case
 *       SL_POWER_MANAGER_LOWEST_EM_ALLOWED config is set to EM1, since we will
 *       never sleep at a lower level than EM1.
 *****************************************************************************/
bool sl_power_manager_is_latest_wakeup_internal(void);

/** @} (end addtogroup power_manager) */

#ifdef __cplusplus
}
#endif

#endif // SL_POWER_MANAGER_H
