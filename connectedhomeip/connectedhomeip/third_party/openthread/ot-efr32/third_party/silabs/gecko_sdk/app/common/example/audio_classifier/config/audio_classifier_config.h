/***************************************************************************//**
 * @file
 * @brief Audio classifier application config
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef AUDIO_CLASSIFIER_CONFIG_H
#define AUDIO_CLASSIFIER_CONFIG_H

#if __has_include("sl_tflite_micro_model_parameters.h")
// Include MLTK model parameters that are extracted from tflite file
  #include "sl_tflite_micro_model_parameters.h"
#endif

// <<< Use Configuration Wizard in Context Menu >>>
// <h> Audio Classification configuration
// <i> Settings for further recognition of the instantaneous model outputs

// <o SMOOTHING_WINDOW_DURATION_MS> Smoothing window duration [ms] <300-1000>
// <i> Sets the duration of a time window from which the network results will be
// <i> averaged together to determine a keyword detection. Longer durations
// <i> reduce the chance of misdetections, but lowers the confidence score which
// <i> may result in missed commands.
// <i> Default: 600
#if defined(SL_TFLITE_MODEL_AVERAGE_WINDOW_DURATION_MS)
  #define SMOOTHING_WINDOW_DURATION_MS SL_TFLITE_MODEL_AVERAGE_WINDOW_DURATION_MS
#else
  #define SMOOTHING_WINDOW_DURATION_MS 600
#endif

// <o MINIMUM_DETECTION_COUNT> Minimum detection count <0-50>
// <i> Sets the minimum number of results required in the smoothing window for
// <i> the keyword detection to be considered a reliable result.
// <i> Default: 3
#if defined(SL_TFLITE_MODEL_MINIMUM_COUNT)
  #define MINIMUM_DETECTION_COUNT SL_TFLITE_MODEL_MINIMUM_COUNT
#else
  #define MINIMUM_DETECTION_COUNT 3
#endif

// <o DETECTION_THRESHOLD> Detection Threshold <0-255>
// <i> Sets a threshold for determining the confidence score required in order
// <i> to classify a keyword as detected. This can be increased to avoid
// <i> misclassifications.
// <i> The confidence scores are in the range <0-255>
// <i> Default: 100
#if defined(SL_TFLITE_MODEL_DETECTION_THRESHOLD)
  #define DETECTION_THRESHOLD SL_TFLITE_MODEL_DETECTION_THRESHOLD
#else
  #define DETECTION_THRESHOLD 100
#endif

// <o SUPPRESION_TIME_MS> Suppresion time after detection [ms] <0-2000>
// <i> Sets a time window to wait after a detected keyword before triggering
// <i> a new detection.
// <i> Default: 1000
#if defined(SL_TFLITE_MODEL_SUPPRESSION_MS)
  #define SUPPRESION_TIME_MS SL_TFLITE_MODEL_SUPPRESSION_MS
#else
  #define SUPPRESION_TIME_MS 1000
#endif

// <o SENSITIVITY> Sensitivity of the activity indicator
// <i> Sets the sensitivity of the activation indicator. If the change in result
// <i> between two classification divided by the duration between the two results
// <i> is larger than this value then the activity indicator is enabled.
// <i> Default: 0.5
#define SENSITIVITY .5f

// <q IGNORE_UNDERSCORE_LABELS> Ignore labels with leading underscore
// <i> When this configuration is set all the labels with leading underscore
// <i> are ignored when checking for new classifications. This is typically
// <i> used to ignore background noise and silence which can be labled
// <i> with "_unknown_" or "_silence_".
// <i> Default: 1
#define IGNORE_UNDERSCORE_LABELS 1

// <o DETECTION_LED> LED to use for detection
// <i> Default: sl_led_led1
#define DETECTION_LED sl_led_led1

// <o ACTIVITY_LED> LED to use for activity
// <i> Default: sl_led_led0
#define ACTIVITY_LED sl_led_led0

// <q VERBOSE_MODEL_OUTPUT_LOGS> Enable verbose model output logging
// <i> Default: 1
#define VERBOSE_MODEL_OUTPUT_LOGS 1

// <o INFERENCE_INTERVAL_MS> Delay between each inference
// <i> Sets the number of milliseconds between each inference.
// <i> Default: 200
#define INFERENCE_INTERVAL_MS 200

// <o MAX_CATEGORY_COUNT> Max number of categories supported.
// <i> Default: 16
#define MAX_CATEGORY_COUNT    16

// <o MAX_RESULT_COUNT> Max number of results supported.
// <i> Default: 50
#define MAX_RESULT_COUNT      50

// <o TASK_STACK_SIZE> Application task stack size.
// <i> Default: 512
#define TASK_STACK_SIZE      512

// <o TASK_PRIORITY> Application task priority.
// <i> Default: 20
#define TASK_PRIORITY         20

// <o CATEGORY_LABELS> Label for each category.
// <i> This is a list of all labels for the given tflite model.
#if defined(SL_TFLITE_MODEL_CLASSES)
  #define CATEGORY_LABELS SL_TFLITE_MODEL_CLASSES
#else
// Insert model specific labels here. The provided labels is an
// example of how this can be done and will have to be edited by the user when
// replacing the model with a model that has not been processed by
// Silicon Labs MLTK.
  #define CATEGORY_LABELS { "on", "off", "_unknown_", "_silence_" }
#endif

// <<< end of configuration section >>>

#endif // AUDIO_CLASSIFIER_CONFIG_H
