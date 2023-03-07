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
#pragma once

#include <cstdint>

#include "pw_bytes/span.h"
#include "pw_containers/vector.h"
#include "pw_log_tokenized/metadata.h"
#include "pw_protobuf/decoder.h"

namespace pw::log_rpc {

struct TestLogEntry {
  log_tokenized::Metadata metadata;
  int64_t timestamp = 0;
  uint32_t dropped = 0;
  ConstByteSpan tokenized_data = {};
  ConstByteSpan file = {};
  ConstByteSpan thread = {};
};

// Unpacks a `LogEntry` proto buffer to compare it with the expected data and
// updates the total drop count found.
void VerifyLogEntry(protobuf::Decoder& entry_decoder,
                    const TestLogEntry& expected_entry,
                    uint32_t& drop_count_out);

// Verifies a stream of log entries and updates the total entry and drop counts.
void VerifyLogEntries(protobuf::Decoder& entries_decoder,
                      const Vector<TestLogEntry>& expected_entries,
                      uint32_t expected_first_entry_sequence_id,
                      size_t& entries_count_out,
                      uint32_t& drop_count_out);

size_t CountLogEntries(protobuf::Decoder& entries_decoder);

}  // namespace pw::log_rpc
