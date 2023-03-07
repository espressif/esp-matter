/***************************************************************************//**
 * @file keywork_detection.cc
 * @brief Top level application functions
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "sl_status.h"
#include "sl_sleeptimer.h"
#include "sl_component_catalog.h"
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
#include "sl_power_manager.h"
#endif

#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"

#include "keyword_detection.h"
#include "recognize_commands.h"
#include "recognize_commands_config.h"
#include "sl_tflite_micro_model.h"
#include "sl_tflite_micro_init.h"
#include "sl_ml_audio_feature_generation.h"

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/
#ifndef INFERENCE_INTERVAL_MS
#define INFERENCE_INTERVAL_MS 200
#endif
/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/

//  Instance Pointers
static RecognizeCommands *command_recognizer = nullptr;

static volatile bool inference_timeout;
static sl_sleeptimer_timer_handle_t inference_timer;

/***************************************************************************//**
 * Run model inference
 *
 * Copies the currently available data from the feature_buffer into the input
 * tensor and runs inference, updating the global output_tensor.
 ******************************************************************************/
static sl_status_t run_inference()
{
  // Update model input tensor
  sl_status_t status = sl_ml_audio_feature_generation_fill_tensor(sl_tflite_micro_get_input_tensor());
  if (status != SL_STATUS_OK) {
    return SL_STATUS_FAIL;
  }
  // Run the model on the spectrogram input and make sure it succeeds.
  TfLiteStatus invoke_status = sl_tflite_micro_get_interpreter()->Invoke();
  if (invoke_status != kTfLiteOk) {
    return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Processes the output from output_tensor
 ******************************************************************************/
sl_status_t process_output()
{
  // Determine whether a command was recognized based on the output of inference
  uint8_t found_command_index = 0;
  uint8_t score = 0;
  bool is_new_command = false;
  uint32_t current_time_stamp;

  // Get current time stamp needed by CommandRecognizer
  current_time_stamp = sl_sleeptimer_tick_to_ms(sl_sleeptimer_get_tick_count());

  TfLiteStatus process_status = command_recognizer->ProcessLatestResults(
    sl_tflite_micro_get_output_tensor(), current_time_stamp, &found_command_index, &score, &is_new_command);

  if (process_status != kTfLiteOk) {
    return SL_STATUS_FAIL;
  }

  if (is_new_command) {
    if (found_command_index == 0 || found_command_index == 1) {
      printf("Heard %s (%d) @%ldms\r\n", kCategoryLabels[found_command_index],
             score, current_time_stamp);
      keyword_detected(found_command_index);
    }
  }

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Inference timer callback
 ******************************************************************************/
static void inference_timer_callback(sl_sleeptimer_timer_handle_t *handle, void* data)
{
  (void)handle;
  (void)data;
  inference_timeout = true;
}

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * Initialize application.
 ******************************************************************************/
void keyword_detection_init(void)
{
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
  // Add EM1 requirement to allow for continous microphone sampling
  sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM1);
#endif

  // Initialize audio feature generation
  sl_ml_audio_feature_generation_init();

  // Instantiate CommandRecognizer
  static RecognizeCommands static_recognizer(sl_tflite_micro_get_error_reporter(), SMOOTHING_WINDOW_DURATION_MS,
                                             DETECTION_THRESHOLD, SUPPRESION_TIME_MS, MINIMUM_DETECTION_COUNT);
  command_recognizer = &static_recognizer;

  // Start periodic timer for inference interval
  sl_sleeptimer_start_periodic_timer_ms(&inference_timer, INFERENCE_INTERVAL_MS, inference_timer_callback, NULL, 0, 0);
}

/***************************************************************************//**
 * Keyword detection process action
 ******************************************************************************/
void keyword_detection_process_action(void)
{
  // Perform keyword detection every INFERENCE_INTERVAL_MS ms
  if (inference_timeout == true) {
    sl_ml_audio_feature_generation_update_features();
    run_inference();
    process_output();
    inference_timeout = false;
  }
}
