/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Copyright 2020 Silicon Laboratories Inc. www.silabs.com

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

#include "output_handler.h"
#include "constants.h"
#include "sl_pwm_instances.h"
#include "sl_pwm.h"
#include <stdio.h>

void HandleOutput(tflite::ErrorReporter* error_reporter, float x_value,
                  float y_value) {
  printf("x=%f y=%f\n", x_value, y_value);

  // Set PWM duty cycle equivalent to y_value 
  uint8_t pwm_percent = (uint8_t)(((y_value + 1)) * 50); 
  sl_pwm_set_duty_cycle(&sl_pwm_led0, pwm_percent);
}
