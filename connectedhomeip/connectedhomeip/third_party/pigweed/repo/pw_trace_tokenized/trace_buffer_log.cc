// Copyright 2020 The Pigweed Authors
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
//==============================================================================
//
#include "pw_trace_tokenized/trace_buffer_log.h"

#include "pw_base64/base64.h"
#include "pw_log/log.h"
#include "pw_span/span.h"
#include "pw_string/string_builder.h"
#include "pw_trace_tokenized/trace_buffer.h"

namespace pw {
namespace trace {
namespace {

constexpr int kMaxEntrySize = PW_TRACE_BUFFER_MAX_BLOCK_SIZE_BYTES;
constexpr int kMaxEntrySizeBase64 = pw::base64::EncodedSize(kMaxEntrySize);
constexpr int kLineLength = 80;

class ScopedTracePause {
 public:
  ScopedTracePause() : was_enabled_(pw_trace_IsEnabled()) {
    PW_TRACE_SET_ENABLED(false);
  }
  ~ScopedTracePause() { PW_TRACE_SET_ENABLED(was_enabled_); }

 private:
  bool was_enabled_;
};

}  // namespace

pw::Status DumpTraceBufferToLog() {
  std::byte line_buffer[kLineLength] = {};
  std::byte entry_buffer[kMaxEntrySize + 1] = {};
  char entry_base64_buffer[kMaxEntrySizeBase64] = {};
  pw::StringBuilder line_builder(line_buffer);
  ScopedTracePause pause_trace;
  pw::ring_buffer::PrefixedEntryRingBuffer* trace_buffer =
      pw::trace::GetBuffer();
  size_t bytes_read = 0;
  PW_LOG_INFO("[TRACE] begin");
  while (trace_buffer->PeekFront(span(entry_buffer).subspan(1), &bytes_read) !=
         pw::Status::OutOfRange()) {
    trace_buffer->PopFront()
        .IgnoreError();  // TODO(b/242598609): Handle Status properly
    entry_buffer[0] = static_cast<std::byte>(bytes_read);
    // The entry buffer is formatted as (size, entry) with an extra byte as
    // a header to the entry. The calcuation of bytes_read + 1 represents
    // the extra size header.
    size_t to_write = pw::base64::Encode(span(entry_buffer, bytes_read + 1),
                                         span(entry_base64_buffer));
    size_t space_left = line_builder.max_size() - line_builder.size();
    size_t written = 0;
    while (to_write - written >= space_left) {
      line_builder.append(entry_base64_buffer + written, space_left);
      PW_LOG_INFO("[TRACE] data: %s", line_builder.c_str());
      line_builder.clear();
      written += space_left;
      space_left = line_builder.max_size();
    }
    line_builder.append(entry_base64_buffer + written, to_write - written);
  }
  if (!line_builder.empty()) {
    PW_LOG_INFO("[TRACE] data: %s", line_builder.c_str());
  }
  PW_LOG_INFO("[TRACE] end");
  return pw::OkStatus();
}

}  // namespace trace
}  // namespace pw
