// Copyright 2021 The Pigweed Authors
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
#pragma once

#include <string_view>

#include "pw_bytes/span.h"
#include "pw_log/levels.h"
#include "pw_log/proto/log.pwpb.h"
#include "pw_log_tokenized/metadata.h"
#include "pw_result/result.h"
#include "pw_status/try.h"

namespace pw::log {

// Packs line number and log level into a single uint32_t as dictated by the
// line_level field in the Log proto message.
//
// Note:
//   line_number is restricted to 29 bits. Values beyond 536870911 will be lost.
//   level is restricted to 3 bits. Values beyond 7 will be lost.
constexpr inline uint32_t PackLineLevel(uint32_t line_number, uint8_t level) {
  return (level & PW_LOG_LEVEL_BITMASK) |
         ((line_number << PW_LOG_LEVEL_BITS) & ~PW_LOG_LEVEL_BITMASK);
}

// Unpacks the line_level field as dictated by the Log proto message into line
// number (uint32_t) and level (uint8_t).
constexpr inline std::tuple<uint32_t, uint8_t> UnpackLineLevel(
    uint32_t line_and_level) {
  return std::make_tuple(
      (line_and_level & ~PW_LOG_LEVEL_BITMASK) >> PW_LOG_LEVEL_BITS,
      line_and_level & PW_LOG_LEVEL_BITMASK);
}

// Convenience functions to encode multiple log attributes as a log proto
// message.
//
// Returns:
// OK - A byte span containing the encoded log proto.
// INVALID_ARGUMENT - `message` argument is zero-length.
// RESOURCE_EXHAUSTED - The provided buffer was not large enough to encode the
//   proto.
Result<ConstByteSpan> EncodeLog(int level,
                                unsigned int flags,
                                std::string_view module_name,
                                std::string_view thread_name,
                                std::string_view file_name,
                                int line_number,
                                int64_t ticks_since_epoch,
                                std::string_view message,
                                ByteSpan encode_buffer);

// Encodes tokenized message and metadata, with a timestamp as a log proto.
// Extra fields can be encoded into the returned encoder. The caller must check
// the encoder status.
pwpb::LogEntry::MemoryEncoder CreateEncoderAndEncodeTokenizedLog(
    log_tokenized::Metadata metadata,
    ConstByteSpan tokenized_data,
    int64_t ticks_since_epoch,
    ByteSpan encode_buffer);

// Convenience functions to convert from tokenized metadata to the log proto
// format.
//
// Returns:
// OK - A byte span containing the encoded log proto.
// RESOURCE_EXHAUSTED - The provided buffer was not large enough to store the
//   proto.
inline Result<ConstByteSpan> EncodeTokenizedLog(
    log_tokenized::Metadata metadata,
    ConstByteSpan tokenized_data,
    int64_t ticks_since_epoch,
    ByteSpan encode_buffer) {
  pwpb::LogEntry::MemoryEncoder encoder = CreateEncoderAndEncodeTokenizedLog(
      metadata, tokenized_data, ticks_since_epoch, encode_buffer);
  PW_TRY(encoder.status());
  return ConstByteSpan(encoder);
}

inline Result<ConstByteSpan> EncodeTokenizedLog(
    log_tokenized::Metadata metadata,
    const uint8_t* tokenized_data,
    size_t tokenized_data_size,
    int64_t ticks_since_epoch,
    ByteSpan encode_buffer) {
  return EncodeTokenizedLog(metadata,
                            as_bytes(span(tokenized_data, tokenized_data_size)),
                            ticks_since_epoch,
                            encode_buffer);
}

// Encodes tokenized message (passed as pointer and size), tokenized metadata,
// timestamp, and thread name as a log proto.
//
// Returns:
// OK - A byte span containing the encoded log proto.
// RESOURCE_EXHAUSTED - The provided buffer was not large enough to store the
//   proto.
inline Result<ConstByteSpan> EncodeTokenizedLog(
    log_tokenized::Metadata metadata,
    const uint8_t* tokenized_data,
    size_t tokenized_data_size,
    int64_t ticks_since_epoch,
    ConstByteSpan thread_name,
    ByteSpan encode_buffer) {
  pwpb::LogEntry::MemoryEncoder encoder = CreateEncoderAndEncodeTokenizedLog(
      metadata,
      as_bytes(span(tokenized_data, tokenized_data_size)),
      ticks_since_epoch,
      encode_buffer);
  if (!thread_name.empty()) {
    encoder.WriteThread(thread_name).IgnoreError();
  }
  PW_TRY(encoder.status());
  return ConstByteSpan(encoder);
}

// Encodes tokenized message (passed as a byte span), tokenized metadata,
// timestamp, and thread name as a log proto.
//
// Returns:
// OK - A byte span containing the encoded log proto.
// RESOURCE_EXHAUSTED - The provided buffer was not large enough to store the
//   proto.
inline Result<ConstByteSpan> EncodeTokenizedLog(
    log_tokenized::Metadata metadata,
    ConstByteSpan tokenized_data,
    int64_t ticks_since_epoch,
    ConstByteSpan thread_name,
    ByteSpan encode_buffer) {
  pwpb::LogEntry::MemoryEncoder encoder = CreateEncoderAndEncodeTokenizedLog(
      metadata, tokenized_data, ticks_since_epoch, encode_buffer);
  if (!thread_name.empty()) {
    encoder.WriteThread(thread_name).IgnoreError();
  }
  PW_TRY(encoder.status());
  return ConstByteSpan(encoder);
}
}  // namespace pw::log
