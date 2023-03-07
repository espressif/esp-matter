/* 
Copyright 2019 The TensorFlow Authors. All Rights Reserved.
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

#include "audio_provider.h"
#include "micro_features/micro_model_settings.h"

#include "sl_mic.h"
#include "sl_status.h"

#define AUDIO_SAMPLE_RATE  16000
#define MIC_SAMPLE_SIZE    512 

namespace {
constexpr int kAudioCaptureBufferSize = 6 * MIC_SAMPLE_SIZE;
int16_t g_audio_output_data[kMaxAudioSampleSize];
int16_t g_audio_capture_buffer[kAudioCaptureBufferSize];
volatile int32_t g_latest_audio_timestamp = 0;
int16_t g_mic_sample_data[MIC_SAMPLE_SIZE * 2];
bool g_is_audio_initialized = false;
}  // namespace

static void CaptureSamples(const int16_t *buffer, uint32_t sample_count) {
  const int32_t start_sample_offset =
      g_latest_audio_timestamp * (kAudioSampleFrequency / 1000);
  for (uint32_t i = 0; i < sample_count; ++i) {
    const uint32_t capture_index = (start_sample_offset + i) % kAudioCaptureBufferSize;
    g_audio_capture_buffer[capture_index] = (buffer[i]);
  }

  // Update the current timestamp to let the outside world know that new audio data has arrived.
  g_latest_audio_timestamp += (sample_count / (kAudioSampleFrequency / 1000));
}

static TfLiteStatus InitAudioRecording(tflite::ErrorReporter* error_reporter) {

  sl_status_t mic_status;

  mic_status = sl_mic_init(AUDIO_SAMPLE_RATE, 1);
  mic_status += sl_mic_start_streaming(g_mic_sample_data, MIC_SAMPLE_SIZE, (sl_mic_buffer_ready_callback_t)CaptureSamples);
  
  if(mic_status != SL_STATUS_OK){
    return kTfLiteError;
  }
  // Block until we have our first audio sample
  while (!g_latest_audio_timestamp) {
  }

  return kTfLiteOk;
}

TfLiteStatus GetAudioSamples(tflite::ErrorReporter* error_reporter,
                             int start_ms, int duration_ms,
                             int* audio_samples_size, int16_t** audio_samples) {
  // This function copies duration_ms samples from the g_audio_capture buffer to the 
  // audio_output buffer.

  if (!g_is_audio_initialized) {
    TfLiteStatus init_status = InitAudioRecording(error_reporter);
    if (init_status != kTfLiteOk) {
      return init_status;
    }
    g_is_audio_initialized = true;
  } 

  // This should only be called when the main thread notices that the latest
  // audio sample data timestamp has changed, so that there's new data in the
  // capture ring buffer. The ring buffer will eventually wrap around and
  // overwrite the data, but the assumption is that the main thread is checking
  // often enough and the buffer is large enough that this call will be made
  // before that happens.

  int start_offset = start_ms * (AUDIO_SAMPLE_RATE / 1000); 
  const int duration_sample_count = duration_ms * (AUDIO_SAMPLE_RATE / 1000); 

  int capture_index = 0;
  for (int i = 0; i < duration_sample_count; ++i) {
    capture_index = (start_offset + i) % kAudioCaptureBufferSize;
    g_audio_output_data[i] = g_audio_capture_buffer[capture_index];
  }

  *audio_samples_size = kMaxAudioSampleSize;
  *audio_samples = g_audio_output_data;
  return kTfLiteOk;
}

int32_t LatestAudioTimestamp() {
  return g_latest_audio_timestamp;
}
