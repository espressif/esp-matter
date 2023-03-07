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

const char* kCategoryLabels[kCategoryCount] = {
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
  if ((latest_results->dims->size != 2)
      || (latest_results->dims->data[0] != 1)
      || (latest_results->dims->data[1] != kCategoryCount)) {
    TF_LITE_REPORT_ERROR(
      error_reporter_,
      "The results for recognition should contain %d elements, but there are "
      "%d in an %d-dimensional shape",
      kCategoryCount, latest_results->dims->data[1],
      latest_results->dims->size);
    return kTfLiteError;
  }

  if (latest_results->type != kTfLiteInt8) {
    TF_LITE_REPORT_ERROR(
      error_reporter_,
      "The results for recognition should be int8_t elements, but are %d",
      latest_results->type);
    return kTfLiteError;
  }

  if ((!previous_results_.empty())
      && (current_time_ms < previous_results_.front().time_)) {
    TF_LITE_REPORT_ERROR(
      error_reporter_,
      "Results must be fed in increasing time order, but received a "
      "timestamp of %d that was earlier than the previous one of %d",
      current_time_ms, previous_results_.front().time_);
    return kTfLiteError;
  }

  // Add the latest results to the head of the queue.
  previous_results_.push_back({ current_time_ms, latest_results->data.int8 });

  // Prune any earlier results that are too old for the averaging window.
  const int64_t time_limit = current_time_ms - average_window_duration_ms_;
  while ((!previous_results_.empty())
         && previous_results_.front().time_ < time_limit) {
    previous_results_.pop_front();
  }

  // If there are too few results, assume the result will be unreliable and
  // bail.
  const int32_t how_many_results = previous_results_.size();
  if ((how_many_results < minimum_count_)) {
    *found_command_index = previous_top_label_index_;
    *score = 0;
    *is_new_command = false;
    /* Don't report non-error
       TF_LITE_REPORT_ERROR(
       error_reporter_,
       "The smoothing window contains less than %d inference result(s), a "
       "reliable keyword detection can not be made.",
       minimum_count_);
     */
    return kTfLiteOk;
  }

  // Calculate the average score across all the results in the window.
  int32_t average_scores[kCategoryCount];
  for (int offset = 0; offset < previous_results_.size(); ++offset) {
    // Iterates the amount of times to achieve average_window_duration
    PreviousResultsQueue::Result previous_result =
      previous_results_.from_front(offset);
    const int8_t* scores = previous_result.scores;
    for (int i = 0; i < kCategoryCount; ++i) {
      if (offset == 0) {
        average_scores[i] = scores[i] + 128;
      } else {
        average_scores[i] += scores[i] + 128;
      }
    }
  }

  for (int i = 0; i < kCategoryCount; ++i) {
    average_scores[i] /= how_many_results;
  }

  // Find the current highest scoring category.
  int8_t current_top_index = 0;
  int32_t current_top_score = 0;
  for (int i = 0; i < kCategoryCount; ++i) {
    if (average_scores[i] > current_top_score) {
      current_top_score = average_scores[i];
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
