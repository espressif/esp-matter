/***************************************************************************//**
 * @file
 * @brief Audio classifier application
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "os.h"
#include "sl_power_manager.h"
#include "sl_status.h"
#include "sl_led.h"
#include "sl_simple_led_instances.h"
#include "audio_classifier.h"
#include "recognize_commands.h"
#include "config/audio_classifier_config.h"
#include "sl_tflite_micro_model.h"
#include "sl_tflite_micro_init.h"
#include "sl_ml_audio_feature_generation.h"
#include "sl_sleeptimer.h"
#include <cmath>

#if SL_SIMPLE_LED_COUNT < 2
  #error "Sample application requires two leds"
#endif

// Pointer to RecognizeCommands object for handling recognitions.
static RecognizeCommands *command_recognizer = nullptr;

// Micrium OS Task variables
static OS_TCB tcb;
static CPU_STK stack[TASK_STACK_SIZE];

// Variables for detection/activity
static int32_t detected_timeout = 0;
static int32_t activity_timestamp = 0;
static int32_t activity_toggle_timestamp = 0;
static uint8_t previous_score = 0;
static int32_t previous_score_timestamp = 0;
static int previous_result = 0;

// Category label variables
int category_count = 0;
const char* category_labels[] = CATEGORY_LABELS;
static int category_label_count = sizeof(category_labels) / sizeof(category_labels[0]);

static void audio_classifier_task(void *arg);
static void handle_result(int32_t current_time, int result, uint8_t score, bool is_new_command);

/***************************************************************************//**
 * Run model inference 
 * 
 * Copies the currently available data from the feature_buffer into the input 
 * tensor and runs inference, updating the global output tensor.
 *
 * @return
 *   SL_STATUS_OK on success, other value on failure.
 ******************************************************************************/
static sl_status_t run_inference()
{
  // Update model input tensor
  sl_status_t status = sl_ml_audio_feature_generation_fill_tensor(sl_tflite_micro_get_input_tensor());
  if (status != SL_STATUS_OK){
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
 * Processes the output from the output tensor
 *
 * @return
 *   SL_STATUS_OK on success, other value on failure.
 ******************************************************************************/
static sl_status_t process_output(){
  // Determine whether a command was recognized based on the output of inference
  uint8_t result = 0;
  uint8_t score = 0;
  bool is_new_command = false;
  uint32_t current_time_stamp;
  sl_status_t status = SL_STATUS_OK;

  // Get current time stamp needed by CommandRecognizer
  current_time_stamp = sl_sleeptimer_tick_to_ms(sl_sleeptimer_get_tick_count());

  // Process the latest result from the model output
  TfLiteStatus process_status = command_recognizer->ProcessLatestResults(
      sl_tflite_micro_get_output_tensor(), current_time_stamp, &result, &score, &is_new_command);

  if (process_status == kTfLiteOk) {
    // Take an action based on the new result/score
    handle_result(current_time_stamp, result, score, is_new_command);
  } else {
    status = SL_STATUS_FAIL;
  }

  return status;
}

/***************************************************************************//**
 * Initialize audio classifier application.
 ******************************************************************************/
void audio_classifier_init(void)
{
  RTOS_ERR err;

  // Create Application Task
  char task_name[] = "audio classifier task";
  OSTaskCreate(&tcb,
               task_name,
               audio_classifier_task,
               DEF_NULL,
               TASK_PRIORITY,
               &stack[0],
               (TASK_STACK_SIZE / 10u),
               TASK_STACK_SIZE,
               0u,
               0u,
               DEF_NULL,
               (OS_OPT_TASK_STK_CLR),
               &err);

  EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
}

/***************************************************************************//**
 * Audio classifier task function
 *
 * This function is executed by a Micrium OS task and does not return.
 *
 * @param arg ignored
 ******************************************************************************/
void audio_classifier_task(void *arg)
{
  RTOS_ERR err;
  (void)&arg;

  printf("Audio Classifier\r\n");
  sl_ml_audio_feature_generation_init();
  
  // Instantiate CommandRecognizer  
  static RecognizeCommands static_recognizer(sl_tflite_micro_get_error_reporter(), SMOOTHING_WINDOW_DURATION_MS,
      DETECTION_THRESHOLD, SUPPRESION_TIME_MS, MINIMUM_DETECTION_COUNT, IGNORE_UNDERSCORE_LABELS);
  command_recognizer = &static_recognizer;

  const TfLiteTensor* input = sl_tflite_micro_get_input_tensor();
  const TfLiteTensor* output = sl_tflite_micro_get_output_tensor();

  // Validate model tensors
  if ((output->dims->size == 2) && (output->dims->data[0] == 1)) {
    category_count = output->dims->data[1];
  } else {
    printf("ERROR: Invalid output tensor shape\n"
           "expecting an output tensor of shape [1,x]\n"
           "where x is the number of classification results\n");
    while (1)
      ;
  }

  if (category_count != category_label_count) {
    printf("WARNING: Number of categories(%d) is not equal to the number of labels(%d).\n"
           "Make sure that CATEGORY_LABELS is configured correctly for the model in use.\n",
           category_count, category_label_count);
  }

  // Validate input/output type of the tflite model
  if ((input->type != kTfLiteInt8) || (output->type != kTfLiteInt8)) {
    printf("ERROR: Invalid input/output tensor type.\n"
           "Application requires input and output tensors to be of type int8.\n");
    while (1)
      ;
  }

  // Add EM1 requirement to allow microphone sampling 
  sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM1);

  while (1) {
    // Delay task in order to do periodic inference
    OSTimeDlyHMSM(0, 0, 0, INFERENCE_INTERVAL_MS, OS_OPT_TIME_PERIODIC, &err);
    EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

    // Perform a word detection
    sl_ml_audio_feature_generation_update_features();
    run_inference();
    process_output();
  }
}

/***************************************************************************//**
 * Handle inference result
 *
 * This function is called whenever we have a succesfull inference result.
 *
 * @param current_time timestamp of the inference result.
 * @param result classification result, this is number >= 0.
 * @param score the score of the result. This is number represents the confidence
 *   of the result classification.
 * @param is_new_command true if the result is a new command, false otherwise.
 ******************************************************************************/
static void handle_result(int32_t current_time, int result, uint8_t score, bool is_new_command) {
  const char *label = get_category_label(result);

  if (is_new_command) {
    printf("Detected class=%d label=%s score=%d @%ldms\n", result, label, score, current_time);
    sl_led_turn_on(&DETECTION_LED);
    sl_led_turn_off(&ACTIVITY_LED);
    detected_timeout = current_time + SUPPRESION_TIME_MS;
  } else if (detected_timeout != 0 && current_time >= detected_timeout) {
    detected_timeout = 0;
    previous_score = score;
    previous_result = result;
    previous_score_timestamp = current_time;
    sl_led_turn_off(&DETECTION_LED);
  }

  // When detection timeout has passed we start to check for activity, which is
  // signaled by a change in the score value.
  if (detected_timeout == 0) {
    if (previous_score == 0) {
      previous_result = result;
      previous_score = score;
      previous_score_timestamp = current_time;
      return;
    }

    // Calculate the rate of difference in score between the two last results
    const int32_t time_delta = current_time - previous_score_timestamp;
    const int8_t score_delta = (int8_t)(score - previous_score);
    const float diff = (time_delta > 0) ? std::fabs(score_delta) / time_delta : 0.0f;

    previous_score = score;
    previous_score_timestamp = current_time;

    if (diff >= SENSITIVITY || (previous_result != result)) {
      previous_result = result;
      activity_timestamp = current_time + 500;
    } else if(current_time >= activity_timestamp) {
      activity_timestamp = 0;
      sl_led_turn_off(&ACTIVITY_LED);
    }

    if (activity_timestamp != 0) {
      if (current_time - activity_toggle_timestamp >= 100) {
        activity_toggle_timestamp = current_time;
        sl_led_toggle(&ACTIVITY_LED);
      }
    }
  }
}

/***************************************************************************//**
 * Get the label for a certain category/class
 *
 * @param index
 *   index of the category/class
 *
 * @return
 *   pointer to the label string. The label is "?" if no corresponding label
 *   was found.
 ******************************************************************************/
const char * get_category_label(int index)
{
  if ((index >= 0) && (index < category_label_count)) {
    return category_labels[index];
  } else {
    return "?";
  }
}
