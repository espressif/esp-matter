// Copyright 2022 The Pigweed Authors
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy of
// the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations under
// the License.

#include <array>
#include <cstddef>
#include <mutex>

#include "pw_bytes/span.h"
#include "pw_chrono/system_clock.h"
#include "pw_log/proto_utils.h"
#include "pw_log_string/handler.h"
#include "pw_log_tokenized/metadata.h"
#include "pw_metric/global.h"
#include "pw_multisink/multisink.h"
#include "pw_result/result.h"
#include "pw_string/string_builder.h"
#include "pw_sync/interrupt_spin_lock.h"
#include "pw_sync/lock_annotations.h"
#include "pw_system/config.h"
#include "pw_system_private/log.h"
#include "pw_tokenizer/tokenize_to_global_handler_with_payload.h"

namespace pw::system {
namespace {

// Sample metric usage.
PW_METRIC_GROUP_GLOBAL(log_metric_group, "log");
PW_METRIC(log_metric_group, total_created, "total_created", 0u);
PW_METRIC(log_metric_group, total_dropped, "total_dropped", 0u);

// Buffer used to encode each log entry before saving into log buffer.
sync::InterruptSpinLock log_encode_lock;
std::array<std::byte, PW_SYSTEM_MAX_LOG_ENTRY_SIZE> log_encode_buffer
    PW_GUARDED_BY(log_encode_lock);

// String-only logs may need to be formatted first. This buffer is required
// so the format string may be passed to the proto log encode.
std::array<std::byte, PW_SYSTEM_MAX_LOG_ENTRY_SIZE> log_format_buffer
    PW_GUARDED_BY(log_encode_lock);

const int64_t boot_time_count =
    pw::chrono::SystemClock::now().time_since_epoch().count();

}  // namespace

// Provides time since boot in units defined by the target's pw_chrono backend.
int64_t GetTimestamp() {
  return pw::chrono::SystemClock::now().time_since_epoch().count() -
         boot_time_count;
}

// Implementation for tokenized log handling. This will be optimized out for
// devices that only use string logging.
extern "C" void pw_tokenizer_HandleEncodedMessageWithPayload(
    pw_tokenizer_Payload payload, const uint8_t message[], size_t size_bytes) {
  log_tokenized::Metadata metadata = payload;
  const int64_t timestamp = GetTimestamp();

  std::lock_guard lock(log_encode_lock);
  Result<ConstByteSpan> encoded_log_result = log::EncodeTokenizedLog(
      metadata, message, size_bytes, timestamp, log_encode_buffer);
  if (!encoded_log_result.ok()) {
    GetMultiSink().HandleDropped();
    total_dropped.Increment();
    return;
  }
  GetMultiSink().HandleEntry(encoded_log_result.value());
  total_created.Increment();
}

// Implementation for string log handling. This will be optimized out for
// devices that only use tokenized logging.
extern "C" void pw_log_string_HandleMessageVaList(int level,
                                                  unsigned int flags,
                                                  const char* module_name,
                                                  const char* file_name,
                                                  int line_number,
                                                  const char* message,
                                                  va_list args) {
  const int64_t timestamp = GetTimestamp();

  std::lock_guard lock(log_encode_lock);
  StringBuilder message_builder(log_format_buffer);
  message_builder.FormatVaList(message, args);

  Result<ConstByteSpan> encoded_log_result =
      log::EncodeLog(level,
                     flags,
                     module_name,
                     /*thread_name=*/{},
                     file_name,
                     line_number,
                     timestamp,
                     message_builder.view(),
                     log_encode_buffer);
  if (!encoded_log_result.ok()) {
    GetMultiSink().HandleDropped();
    total_dropped.Increment();
    return;
  }
  GetMultiSink().HandleEntry(encoded_log_result.value());
  total_created.Increment();
}

}  // namespace pw::system
