/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

   This file has been modified by Silicon Labs.
   ==============================================================================*/

#include "magic_wand/output_handler.h"
#include <cstdio>
#include "sl_led.h"
#include "sl_simple_led_instances.h"
#include "sl_sleeptimer.h"

#ifndef TOGGLE_DELAY_MS
#define TOGGLE_DELAY_MS         2000
#endif

static sl_sleeptimer_timer_handle_t led_timer;

/***************************************************************************//**
 * LED timeout callback
 ******************************************************************************/
void OnTimeoutLed(sl_sleeptimer_timer_handle_t *led_timer, void *data)
{
  sl_led_turn_off(&sl_led_led0);
  sl_led_turn_off(&sl_led_led1);
}

/***************************************************************************//**
 * Prints result and toggle LEDs. (red: wing, yellow: ring, green: slope)
 ******************************************************************************/
void HandleOutput(tflite::ErrorReporter* error_reporter, int kind)
{
  if (kind == 0) {
    printf(R"EOF(
 __    __ _               __            _ _ 
/ / /\ \ (_)_ __   __ _  / _\_ __   ___| | |
\ \/  \/ / | '_ \ / _` | \ \| '_ \ / _ \ | |
 \  /\  /| | | | | (_| | _\ \ |_) |  __/ | |
  \/  \/ |_|_| |_|\__, | \__/ .__/ \___|_|_|
                  |___/     |_|             


            *         *         *
             *       * *       *
              *     *   *     *
               *   *     *   *
                * *       * *
                 *         *    

)EOF");
    sl_led_turn_on(&sl_led_led0);
    sl_sleeptimer_start_timer_ms(&led_timer,
                                 TOGGLE_DELAY_MS,
                                 OnTimeoutLed, NULL,
                                 0,
                                 SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG);
  } else if (kind == 1) {
    printf(R"EOF(
 _____  
|  __ \ _                __            _ _ 
| |__) (_) _ __   __ _  / _\_ __   ___| | |
|  _  /| || '_ \ / _` | \ \| '_ \ / _ \ | |
| | \ \| || | | | (_| | _\ \ |_) |  __/ | |
|_|  \_|_||_| |_|\__, | \__/ .__/ \___|_|_|
                  |___/     |_|            


                     *
                  *     *
                *         *
               *           *
                *         *
                  *     *
                     *

)EOF");
    sl_led_turn_on(&sl_led_led0);
    sl_led_turn_on(&sl_led_led1);
    sl_sleeptimer_start_timer_ms(&led_timer,
                                 TOGGLE_DELAY_MS,
                                 OnTimeoutLed, NULL,
                                 0,
                                 SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG);
  } else if (kind == 2) {
    printf(R"EOF(
 __ _                    __            _ _ 
/ _\ | ___  _ __   ___  / _\_ __   ___| | |
\ \| |/ _ \| '_ \ / _ \ \ \| '_ \ / _ \ | |
_\ \ | (_) | |_) |  __/ _\ \ |_) |  __/ | |
\__/_|\___/| .__/ \___| \__/ .__/ \___|_|_|
           |_|             |_|         

                      *
                     *
                    *
                   *
                  *
                 *
                *
               * * * * * * * * 

)EOF");
    sl_led_turn_on(&sl_led_led1);
    sl_sleeptimer_start_timer_ms(&led_timer,
                                 TOGGLE_DELAY_MS,
                                 OnTimeoutLed, NULL,
                                 0,
                                 SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG);
  } else if (kind == 3) {
    // No spell detected
  }
}
