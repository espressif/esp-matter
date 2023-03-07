/***************************************************************************//**
 * @file
 * @brief Power Manager examples functions
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
// Define module name for Power Manager debuging feature.
#define CURRENT_MODULE_NAME    "APP_COMMON_EXAMPLE_POWER_MANAGER"

#include <stdio.h>
#include "power_manager_app.h"
#include "sl_power_manager.h"
#include "sl_power_manager_debug.h"
#include "sl_sleeptimer.h"
#include "sl_simple_button_instances.h"
#include "em_emu.h"

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

#define EM_EVENT_MASK_ALL  (SL_POWER_MANAGER_EVENT_TRANSITION_ENTERING_EM0   \
                            | SL_POWER_MANAGER_EVENT_TRANSITION_LEAVING_EM0  \
                            | SL_POWER_MANAGER_EVENT_TRANSITION_ENTERING_EM1 \
                            | SL_POWER_MANAGER_EVENT_TRANSITION_LEAVING_EM1  \
                            | SL_POWER_MANAGER_EVENT_TRANSITION_ENTERING_EM2 \
                            | SL_POWER_MANAGER_EVENT_TRANSITION_LEAVING_EM2  \
                            | SL_POWER_MANAGER_EVENT_TRANSITION_ENTERING_EM3 \
                            | SL_POWER_MANAGER_EVENT_TRANSITION_LEAVING_EM3)

#ifndef BUTTON_INSTANCE_0
#define BUTTON_INSTANCE_0   sl_button_btn0
#endif

#ifndef BUTTON_INSTANCE_1
#define BUTTON_INSTANCE_1   sl_button_btn1
#endif
#define NO_OF_EMODE_TESTS 5

/*******************************************************************************
 *******************  LOCAL FUNCTION DECLARATIONS   ****************************
 ******************************************************************************/

// Emode transition callback
static void transition_callback(sl_power_manager_em_t from,
                                sl_power_manager_em_t to);

// sleeptimer callback
static void timer_callback(sl_sleeptimer_timer_handle_t *handle,
                           void *data);

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/
// Energy mode enumerations
typedef enum {
  EMODE_0,
  EMODE_1,
  EMODE_2,
  EMODE_3,
  EMODE_4,
  NUM_EMODES
} energy_mode_enum_t;

static sl_power_manager_em_transition_event_handle_t event_handle;
// energy mode transition callback settings
static sl_power_manager_em_transition_event_info_t event_info = {
  .event_mask = EM_EVENT_MASK_ALL,     // subscribe to all em transitions
  .on_event = transition_callback      // callback triggered on transition event
};

static sl_sleeptimer_timer_handle_t my_timer;

// Flag to indicate to power manager if application can sleep
// The application will start in EM0
static bool ok_to_sleep = false;
// Flag to indicate to power manager if the application should return to sleep
// after an interrupt
static sl_power_manager_on_isr_exit_t isr_ok_to_sleep = SL_POWER_MANAGER_IGNORE;
// Target energy mode
static sl_power_manager_em_t em_mode = SL_POWER_MANAGER_EM0;
// User selected energy mode
static volatile energy_mode_enum_t selected_emode;
// Start selected energy mode test.
static volatile int start_test;

/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/

/***************************************************************************//**
 * Function called when energy mode changes.
 ******************************************************************************/
static void transition_callback(sl_power_manager_em_t from,
                                sl_power_manager_em_t to)
{
  // Set breakpoint to monitor transitions
  (void)from;
  (void)to;
}

/***************************************************************************//**
 * Function called when sleep timer expires.
 ******************************************************************************/
static void timer_callback(sl_sleeptimer_timer_handle_t *handle,
                           void *data)
{
  (void)handle;
  (void)data;
  // when the timer expires, prevent return to sleep
  ok_to_sleep = false;
  isr_ok_to_sleep = SL_POWER_MANAGER_WAKEUP;
  // Clear requirements
  if (em_mode == SL_POWER_MANAGER_EM1
      || em_mode == SL_POWER_MANAGER_EM2) {
    sl_power_manager_remove_em_requirement(em_mode);
  }
}

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * Hook for power manager
 ******************************************************************************/
bool app_is_ok_to_sleep(void)
{
  // return false to prevent sleep mode and force EM0
  // return true to indicate to power manager that application can sleep
  return ok_to_sleep;
}

/***************************************************************************//**
 * Hook for power manager
 ******************************************************************************/
sl_power_manager_on_isr_exit_t app_sleep_on_isr_exit(void)
{
  // flag used by power manager to determine if device can return to sleep
  // following interrupt
  return isr_ok_to_sleep;
}

/***************************************************************************//**
 * Callback on button change.
 *
 * This function overrides a weak implementation defined in the simple_button
 * module. It is triggered when the user activates one of the buttons.
 *
 ******************************************************************************/
void sl_button_on_change(const sl_button_t *handle)
{
  if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED) {
    if (&BUTTON_INSTANCE_0 == handle) {
      start_test = true;
    } else if (&BUTTON_INSTANCE_1 == handle) {
      selected_emode = (energy_mode_enum_t)((selected_emode + 1) % NO_OF_EMODE_TESTS);
    }
    isr_ok_to_sleep = SL_POWER_MANAGER_WAKEUP;
  }
}

/***************************************************************************//**
 * Initialize example.
 ******************************************************************************/
void power_manager_app_init(void)
{
  // Subscribe to energy mode transitions
  // The callback set in event_info will be called when the energy mode changes,
  // based on the transition mask
  sl_power_manager_subscribe_em_transition_event(&event_handle, &event_info);
}

/***************************************************************************//**
 * Ticking function.
 ******************************************************************************/
void power_manager_app_process_action(void)
{
  energy_mode_enum_t emode;

  if (start_test) {
    start_test = false;
    emode = selected_emode;
    selected_emode = EMODE_0;

    switch (emode) {
      case EMODE_0:
        // flag returned by app_is_ok_to_sleep()
        // returning false will prevent sleep
        ok_to_sleep = false;
        em_mode = SL_POWER_MANAGER_EM0;
        break;
      case EMODE_1:
        // Set flag returned by app_is_ok_to_sleep() to indicate
        // to power manager that application can sleep
        ok_to_sleep = true;
        // Set flag returned by app_sleep_on_isr_exit() to indicate
        // to power manager that application can return to sleep after
        // interrupt
        isr_ok_to_sleep = SL_POWER_MANAGER_SLEEP;
        em_mode = SL_POWER_MANAGER_EM1;
        // Ensure app stays in selected energy mode
        sl_power_manager_add_em_requirement(em_mode);
        // Sleep timer functions in EM1
        sl_sleeptimer_start_timer_ms(&my_timer,
                                     5000,
                                     timer_callback,
                                     (void *)NULL,
                                     0,
                                     0);
        break;
      case EMODE_2:
        // Set flag returned by app_is_ok_to_sleep() to indicate
        // to power manager that application can sleep
        ok_to_sleep = true;
        // Set flag returned by app_sleep_on_isr_exit() to indicate
        // to power manager that application can return to sleep after
        // interrupt
        isr_ok_to_sleep = SL_POWER_MANAGER_SLEEP;
        em_mode = SL_POWER_MANAGER_EM2;
        // Ensure app stays in selected energy mode
        sl_power_manager_add_em_requirement(em_mode);
        // Sleep timer functions in EM2
        sl_sleeptimer_start_timer_ms(&my_timer,
                                     5000,
                                     timer_callback,
                                     (void *)NULL,
                                     0,
                                     SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG);
        break;
      case EMODE_3:
        // Set flag returned by app_is_ok_to_sleep() to indicate
        // to power manager that application can sleep
        ok_to_sleep = true;
        // Set flag returned by app_sleep_on_isr_exit() to indicate
        // to power manager that application can return to sleep after
        // interrupt
        isr_ok_to_sleep = SL_POWER_MANAGER_SLEEP;
        em_mode = SL_POWER_MANAGER_EM3;
        // Sleep timer does not function in EM3
        // Application will be woken from sleep in button interrupt
        break;
      case EMODE_4:
        // EM4 is not supported by Power Manager
        em_mode = SL_POWER_MANAGER_EM4;
        EMU_EnterEM4();
        break;
      default:
        EFM_ASSERT(false);
        break;
    }
  }
}
