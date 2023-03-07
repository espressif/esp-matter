/* Copyright 2017 The TensorFlow Authors. All Rights Reserved.

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

#include "recognize_commands.h"

#include <limits>

// Labels used by the Voice Control tflite model
// 0 - on
// 1 - off
// 2 - unknown
// 3 - background
const char* kCategoryLabels[kMaxCategoryCount] = {
  "on",
  "off",
  "unknown",
  "background",
};

RecognizeCommands::RecognizeCommands(tflite::ErrorReporter* error_reporter,
                                     int32_t average_window_duration_ms,
                                     uint8_t detection_threshold,
                                     int32_t suppression_ms,
                                     int32_t minimum_count)
  : error_reporter_(error_reporter),
  average_window_duration_ms_(average_window_duration_ms),
  detection_threshold_(detection_threshold),
  suppression_ms_(suppression_ms),
  minimum_count_(minimum_count),
  previous_results_(error_reporter)
{
  previous_top_label_index_ = 3; // "background"
  previous_top_label_time_ = std::numeric_limits < int32_t > ::min();
}

TfLiteStatus RecognizeCommands::ProcessLatestResults(
  const TfLiteTensor* latest_results, const int32_t current_time_ms,
  uint8_t* found_command_index, uint8_t* score, bool* is_new_command)
{

uint8_t category_count = latest_results->dims->data[1];
int8_t current_top_index = 0;
int32_t current_top_score = 0;
uint8_t converted_scores[kMaxCategoryCount];

// Convert the model output to uint8
if (latest_results->type == kTfLiteInt8) {
    for(int i = 0; i < category_count; ++i) {
      converted_scores[i] = (uint8_t)(latest_results->data.int8[i] + 128);
    }
  } else {
      TF_LITE_REPORT_ERROR(error_reporter_, "Unsupported output tensor data type, must be int8 or float32");
      return kTfLiteError;
  }

  for (int i = 0; i < category_count; i++) {
    if (converted_scores[i] > current_top_score) {
      current_top_score = converted_scores[i];
      current_top_index = i;
    }
  }

  // If we've recently had another label trigger, assume one that occurs too
  // soon afterwards is a bad result.
  int64_t time_since_last_top;
  if ((previous_top_label_index_ == 3 || previous_top_label_index_ == 2)
      || (previous_top_label_time_ == std::numeric_limits < int32_t > ::min())) {
    time_since_last_top = std::numeric_limits < int32_t > ::max();
    // Reset timer if detecting background / unknown
  } else {
    time_since_last_top = current_time_ms - previous_top_label_time_;
  }

  if ((current_top_score > detection_threshold_)
      && ((current_top_index != previous_top_label_index_)
          || (time_since_last_top > suppression_ms_))) {
    previous_top_label_index_ = current_top_index;
    previous_top_label_time_ = current_time_ms;
    *is_new_command = true;
  } else {
    *is_new_command = false;
  }

  *found_command_index = current_top_index;
  *score = current_top_score;
  return kTfLiteOk;
}
