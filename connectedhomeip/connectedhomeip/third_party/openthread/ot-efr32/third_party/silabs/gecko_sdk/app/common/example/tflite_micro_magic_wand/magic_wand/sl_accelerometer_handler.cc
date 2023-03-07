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

#include "magic_wand/accelerometer_handler.h"
#include "magic_wand/constants.h"
#include "sl_imu.h"
#include "gpiointerrupt.h"
#include "em_gpio.h"
#include "em_core.h"

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif

#if defined (SL_CATALOG_ICM20689_DRIVER_PRESENT)
#include "sl_icm20689_config.h"
#define  SL_IMU_INT_PORT SL_ICM20689_INT_PORT
#define  SL_IMU_INT_PIN  SL_ICM20689_INT_PIN
#elif defined (SL_CATALOG_ICM20648_DRIVER_PRESENT)
#include "sl_icm20648_config.h"
#define  SL_IMU_INT_PORT SL_ICM20648_INT_PORT
#define  SL_IMU_INT_PIN  SL_ICM20648_INT_PIN
#else
#error "No IMU driver defined"
#endif

// Set up a ring array. 600 spaces is default in this TensorFlow example.
static int saved_imu_data[600];

// Index of the ring array
int begin_index = 0;

// Wait for buffer to be filled first time
static bool init_done = false;

/***************************************************************************//**
 * Accelerometer interrupt callback
 ******************************************************************************/
void DataAvailable(uint8_t intNo, void *ctx)
{
  // This function is called every time the IMU has new data,
  // depending on the configured sample rate. 

  if (!sl_imu_is_data_ready()) {
    return;
  }
  sl_imu_update();

  int16_t measurement_xyz[3];
  sl_imu_get_acceleration(measurement_xyz);
  // Saving accelerometer data to ringbuffer
  // Coordinate frame in model is left-hand, while the IMU frame
  // is right-hand and rotated. Transformation required.
  saved_imu_data[begin_index++] = - measurement_xyz[1];
  saved_imu_data[begin_index++] =   measurement_xyz[0];
  saved_imu_data[begin_index++] =   measurement_xyz[2];
  if (begin_index >= 600) begin_index = 0;
}

/***************************************************************************//**
 * Accelerometer Setup
 ******************************************************************************/
TfLiteStatus SetupAccelerometer(tflite::ErrorReporter* error_reporter) {
  int intNo;
  // Initialize accelerometer sensor
  sl_imu_init();
  sl_imu_configure(kTargetHz);
  // Setup interrupt from accelerometer on falling edge
  GPIO_PinModeSet(SL_IMU_INT_PORT,SL_IMU_INT_PIN, gpioModeInput, 0);
  intNo = GPIOINT_CallbackRegisterExt(SL_IMU_INT_PIN, DataAvailable, NULL);
  EFM_ASSERT(intNo != INTERRUPT_UNAVAILABLE);
  GPIO_ExtIntConfig(SL_IMU_INT_PORT, SL_IMU_INT_PIN, intNo, false, true, true);
  return kTfLiteOk;
}

/***************************************************************************//**
 * Import accelerometer data to TensorFlow model
 ******************************************************************************/
bool ReadAccelerometer(tflite::ErrorReporter* error_reporter, float* input,
                       int length) {

  if (begin_index >= length) {
    init_done = true;
  }
  if (init_done == false) {
    return false;
  }

  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  for (int i = 0; i < length; i++)
  {
    int ring_buffer_index = begin_index - length + i;
    if (ring_buffer_index < 0) {
      ring_buffer_index += 600;
    }
    input[i] = saved_imu_data[ring_buffer_index];
  }
  CORE_EXIT_CRITICAL();
  return true;
}
