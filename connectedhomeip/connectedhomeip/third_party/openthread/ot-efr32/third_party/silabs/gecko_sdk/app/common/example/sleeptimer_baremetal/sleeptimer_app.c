/***************************************************************************//**
 * @file
 * @brief Sleeptimer examples functions
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

#include <stdio.h>
#include <string.h>
#include "sleeptimer_app.h"
#include "sl_sleeptimer.h"
#include "sl_simple_led_instances.h"
#include "sl_simple_button_instances.h"
#include "sl_iostream_init_instances.h"

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

#ifndef BUTTON_INSTANCE_0
#define BUTTON_INSTANCE_0   sl_button_btn0
#endif

#ifndef BUTTON_INSTANCE_1
#define BUTTON_INSTANCE_1   sl_button_btn1
#endif

#ifndef LED_INSTANCE_0
#define LED_INSTANCE_0      sl_led_led0
#endif

#ifndef LED_INSTANCE_1
#define LED_INSTANCE_1      sl_led_led1
#endif

#define TIMEOUT_MS 10000
/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/

static sl_sleeptimer_timer_handle_t periodic_timer;
static sl_sleeptimer_timer_handle_t one_shot_timer;
static sl_sleeptimer_timer_handle_t status_timer;
static bool print_status = false;

/*******************************************************************************
 ************************   LOCAL FUNCTIONS ************************************
 ******************************************************************************/

// Periodic timer callback
static void on_periodic_timeout(sl_sleeptimer_timer_handle_t *handle,
                                void *data)
{
  (void)&handle;
  (void)&data;
  sl_led_toggle(&LED_INSTANCE_0);
}

// One-shot timer callback
static void on_one_shot_timeout(sl_sleeptimer_timer_handle_t *handle,
                                void *data)
{
  (void)&handle;
  (void)&data;
  sl_led_toggle(&LED_INSTANCE_1);
}

// Status timer callback
static void on_status_timeout(sl_sleeptimer_timer_handle_t *handle,
                              void *data)
{
  (void)&handle;
  (void)&data;
  print_status = true;
}

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * Initialize example.
 ******************************************************************************/
void sleeptimer_app_init(void)
{
  /* Output on vcom usart instance */
  printf("Sleeptimer example\r\n\r\n");
  printf("LED0 is controlled by a periodic timer\r\n");
  printf("LED1 is controlled by a one-shot timer\r\n");
  printf("Use buttons to start and restart timers\r\n");

  // Create timer for waking up the system periodically.
  sl_sleeptimer_start_periodic_timer_ms(&periodic_timer,
                                        TIMEOUT_MS,
                                        on_periodic_timeout, NULL,
                                        0,
                                        SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG);

  // Create one-shot timer for waking up the system.
  sl_sleeptimer_start_timer_ms(&one_shot_timer,
                               TIMEOUT_MS,
                               on_one_shot_timeout, NULL,
                               0,
                               SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG);

  // Create periodic timer to report status of other timers
  sl_sleeptimer_start_periodic_timer_ms(&status_timer,
                                        1000,
                                        on_status_timeout, NULL,
                                        0,
                                        SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG);
}

/***************************************************************************//**
 * Ticking function.
 ******************************************************************************/
void sleeptimer_app_process_action(void)
{
  uint32_t remaining;

  // Periodically report the status of the other timers
  if (print_status == true) {
    print_status = false;
    if (0 == sl_sleeptimer_get_timer_time_remaining(&one_shot_timer, &remaining) ) {
      printf("One shot timer has %lu ms remaining\r\n", sl_sleeptimer_tick_to_ms(remaining));
    }
    if (0 == sl_sleeptimer_get_timer_time_remaining(&periodic_timer, &remaining) ) {
      printf("Periodic timer has %lu ms remaining\r\n", sl_sleeptimer_tick_to_ms(remaining));
    }
  }
}

/***************************************************************************//**
 * Function called on button change
 ******************************************************************************/
void sl_button_on_change(const sl_button_t *handle)
{
  bool is_running = false;
  if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED) {
    // Button 1 controls the one-shot timer
    if (&BUTTON_INSTANCE_1 == handle) {
      if (sl_sleeptimer_is_timer_running(&one_shot_timer, &is_running) == 0) {
        if (is_running) {
          // If timer is running, stop it
          sl_sleeptimer_stop_timer(&one_shot_timer);
        } else {
          // If timer is stopped, restart it
          sl_sleeptimer_restart_timer_ms(&one_shot_timer,
                                         TIMEOUT_MS,
                                         on_one_shot_timeout, NULL,
                                         0,
                                         SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG);
        }
      }
    }
    if (&BUTTON_INSTANCE_0 == handle) {
      // Button 0 controls the periodic timer
      if (sl_sleeptimer_is_timer_running(&periodic_timer, &is_running) == 0) {
        if (is_running) {
          // If timer is running, stop it
          sl_sleeptimer_stop_timer(&periodic_timer);
        } else {
          // If timer is stopped, restart it
          sl_sleeptimer_restart_periodic_timer_ms(&periodic_timer,
                                                  TIMEOUT_MS,
                                                  on_periodic_timeout, NULL,
                                                  0,
                                                  SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG);
        }
      }
    }
  }
}
