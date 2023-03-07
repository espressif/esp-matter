/*
 *  Copyright (c) 2019, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file implements the OpenThread platform abstraction for power (sleep)
 * management.
 *
 */

#define  CURRENT_MODULE_NAME "OPENTHREAD"
#include OPENTHREAD_PROJECT_CORE_CONFIG_FILE

#include <assert.h>
#include <openthread/tasklet.h>
#include "sleep.h"
#include "em_core.h"
#include "sl_component_catalog.h"
#include "platform-efr32.h"

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
 
//------------------------------------------------------------------------------
// Forward declarations

#if (defined(SL_CATALOG_POWER_MANAGER_PRESENT) && !defined(SL_CATALOG_KERNEL_PRESENT))

static void energy_mode_transition_callback(sl_power_manager_em_t from,
                                            sl_power_manager_em_t to);

//------------------------------------------------------------------------------
// Static and extern variables

static bool em1_requirement_set = false;

static sl_power_manager_em_transition_event_handle_t pm_handle;
static sl_power_manager_em_transition_event_info_t pm_event_info =
{ POWER_MANAGER_EVENTS_OF_INTEREST, energy_mode_transition_callback };

#endif // SL_CATALOG_POWER_MANAGER_PRESENT && !SL_CATALOG_KERNEL_PRESENT

extern otInstance *sInstance;

//------------------------------------------------------------------------------
// Internal APIs and callbacks

void sl_ot_sleep_init(void)
{
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)

#ifndef SL_CATALOG_KERNEL_PRESENT
  sl_power_manager_subscribe_em_transition_event(&pm_handle, &pm_event_info);
#endif

  // Ensure EM2 is the lowest low power mode
  sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM2);

  // Set initial power requirement to EM1
  sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM1);

#ifndef SL_CATALOG_KERNEL_PRESENT
  em1_requirement_set = true;
#endif

#endif // SL_CATALOG_POWER_MANAGER_PRESENT
}

__WEAK bool efr32AllowSleepCallback(void)
{
    return true;
}

#if (defined(SL_CATALOG_POWER_MANAGER_PRESENT) && !defined(SL_CATALOG_KERNEL_PRESENT))

// This is invoked only the bare metal case.
bool sl_ot_is_ok_to_sleep(void)
{
  // If the application does not permit sleep, we don't sleep.
  if (!efr32AllowSleepCallback()) {
    return false;
  }

  uint32_t duration_ms = 0;

  CORE_ATOMIC_IRQ_DISABLE();

  // If the stack says we can nap, it means that no tasks are pending. We may
  // either sleep (EM2) or idle (EM1) for some amount of time. Otherwise,
  // we can't sleep/idle at all.
  if (!otTaskletsArePending(sInstance)) {
    // Compute sleep/idle duration. we will never sleep/idle longer than the
    // duration to our next event.
    duration_ms = efr32AlarmPendingTime();

    // If the sleep duration is below our minimum threshold, we dont bother sleeping.
    // If so, we can try to idle instead.
    if (efr32AlarmIsRunning(sInstance) && (duration_ms < OPENTHREAD_CONFIG_MIN_SLEEP_DURATION_MS)) {
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

  CORE_ATOMIC_IRQ_ENABLE();

  // We dont need to start a separate timer to wake us up, as the alarm code already sets one.
  return (efr32AlarmIsRunning(sInstance) ? (duration_ms != 0) : true);
}

//------------------------------------------------------------------------------
// Static functions

static void energy_mode_transition_callback(sl_power_manager_em_t from,
                                            sl_power_manager_em_t to)
{
  if (from == SL_POWER_MANAGER_EM2) { // Leaving EM2
    // emberStackPowerUp(); // TO DO: Do we need to take care of any state?
  } else if (to == SL_POWER_MANAGER_EM2) { // Going to EM2
    // emberStackPowerDown(); // TO DO: Do we need to take care of any state?
  }
}

#endif // SL_CATALOG_POWER_MANAGER_PRESENT && !SL_CATALOG_KERNEL_PRESENT

