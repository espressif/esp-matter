/***************************************************************************//**
 * @brief Connect Application Framework sleep code.
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
// Define module name for Power Manager debuging feature.
#define CURRENT_MODULE_NAME    "FLEX"

#include "callback_dispatcher.h"
#include "app_framework_callback.h"
#include "hal.h"

#include "sl_component_catalog.h"
#include "sl_sleeptimer.h"

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
  #include "sl_power_manager.h"
#endif // SL_CATALOG_POWER_MANAGER_PRESENT

// Power manager transition events of interest.
#define POWER_MANAGER_EVENTS_OF_INTEREST            \
  (SL_POWER_MANAGER_EVENT_TRANSITION_ENTERING_EM0   \
   | SL_POWER_MANAGER_EVENT_TRANSITION_LEAVING_EM0  \
   | SL_POWER_MANAGER_EVENT_TRANSITION_ENTERING_EM1 \
   | SL_POWER_MANAGER_EVENT_TRANSITION_LEAVING_EM1  \
   | SL_POWER_MANAGER_EVENT_TRANSITION_ENTERING_EM2 \
   | SL_POWER_MANAGER_EVENT_TRANSITION_LEAVING_EM2)

#define MINIMUM_SLEEP_DURATION_MS 5

//------------------------------------------------------------------------------
// Forward declarations

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)

static void energy_mode_transition_callback(sl_power_manager_em_t from,
                                            sl_power_manager_em_t to);

static void wakeup_timer_callback(sl_sleeptimer_timer_handle_t* timer_id,
                                  void *user);

//------------------------------------------------------------------------------
// Static and extern variables

static bool em1_requirement_set = false;

static sl_power_manager_em_transition_event_handle_t pm_handle;
static sl_power_manager_em_transition_event_info_t pm_event_info =
{ POWER_MANAGER_EVENTS_OF_INTEREST, energy_mode_transition_callback };

static sl_sleeptimer_timer_handle_t wakeup_timer_id;

extern const EmberEventData emAppEvents[];
extern EmberTaskId emAppTask;

#endif // SL_CATALOG_POWER_MANAGER_PRESENT

//------------------------------------------------------------------------------
// Internal APIs and callbacks

// This is invoked only in case we are running bare-metal (no OS).
void connect_sleep_init(void)
{
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)

  sl_power_manager_subscribe_em_transition_event(&pm_handle, &pm_event_info);

  // Ensure EM2 is the lowest low power mode
  sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM2);

  // Set initial power requirement to EM1
  sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM1);
  em1_requirement_set = true;

#endif // SL_CATALOG_POWER_MANAGER_PRESENT
}

#if (defined(SL_CATALOG_POWER_MANAGER_PRESENT) && !defined(CONNECT_CMSIS_STACK_IPC_PRESENT))

// This is invoked only the bare metal case.
bool connect_is_ok_to_sleep(void)
{
  uint32_t duration_ms = 0;

  INTERRUPTS_OFF();

  // If the stack says we can nap, it means that we may sleep (EM2) for some
  // amount of time.  Otherwise, we can't sleep at all, although we can try to
  // idle (EM1).
  if (emberOkToNap()) {
    // If the stack says we can hiberate, it means we can sleep as long as we
    // want.  Otherwise, we cannot sleep longer than the duration to its next
    // event.  In either case, we will never sleep longer than the duration
    // to our next event.
    duration_ms = (emberOkToHibernate()
                   ? MAX_INT32U_VALUE
                   : emberMsToNextStackEvent());
    duration_ms = emberMsToNextEvent(emAppEvents, duration_ms);

    // If the sleep duration is below our minimum threshold, we don't bother
    // sleeping.  It takes time to shut everything down and bring everything
    // back up and, at some point, it becomes useless to sleep. If the
    // duration is long enough we will sleep. Otherwise, we will try to idle.
    if (duration_ms < MINIMUM_SLEEP_DURATION_MS) {
      duration_ms = 0;
      if (!em1_requirement_set) {
        // Prevent the system from entering em2
        sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM1);
        em1_requirement_set = true;
      }
    } else {
      if (em1_requirement_set) {
        // Allow the system to enter em2
        sl_power_manager_remove_em_requirement(SL_POWER_MANAGER_EM1);
        em1_requirement_set = false;
      }
    }
  } else {
    if (!em1_requirement_set) {
      // Prevent the system from entering em2
      sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM1);
      em1_requirement_set = true;
    }
  }

  // If durations_ms is 0 it means we could not enter em2, so we see how long we
  // can enter em1 instead.
  if (duration_ms == 0) {
    assert(em1_requirement_set);

    duration_ms = emberStackIdleTimeMs(NULL);
    duration_ms = emberMsToNextEvent(emAppEvents, duration_ms);
  }

  INTERRUPTS_ON();

  // We need to stay awake.
  if (duration_ms == 0) {
    return false;
  } else {
    bool app_allowed =
      emberAfCommonOkToEnterLowPowerCallback(!em1_requirement_set, duration_ms);

    if (app_allowed && duration_ms > 0 && duration_ms < MAX_INT32U_VALUE) {
      // We can enter em1 or em2 for a limited amount of time: either way we set
      // a timer to wake us up.
      assert(sl_sleeptimer_restart_timer_ms(&wakeup_timer_id,
                                            duration_ms,
                                            wakeup_timer_callback,
                                            NULL,
                                            0u,
                                            0u) == SL_STATUS_OK);
    }

    return app_allowed;
  }
}

#endif // SL_CATALOG_POWER_MANAGER_PRESENT && !CONNECT_CMSIS_STACK_IPC_PRESENT

//------------------------------------------------------------------------------
// Static functions

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)

static void energy_mode_transition_callback(sl_power_manager_em_t from,
                                            sl_power_manager_em_t to)
{
  if (from == SL_POWER_MANAGER_EM2) { // Leaving EM2
    emberStackPowerUp();
  } else if (to == SL_POWER_MANAGER_EM2) { // Going to EM2
    emberStackPowerDown();
  }
}

static void wakeup_timer_callback(sl_sleeptimer_timer_handle_t* timer_id,
                                  void *user)
{
  (void)timer_id;
  (void)user;
}

#endif // SL_CATALOG_POWER_MANAGER_PRESENT
