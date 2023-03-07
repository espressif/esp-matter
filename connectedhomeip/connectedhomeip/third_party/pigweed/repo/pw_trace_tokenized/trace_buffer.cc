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
#include "pw_trace_tokenized/trace_buffer.h"

#include "pw_ring_buffer/prefixed_entry_ring_buffer.h"
#include "pw_span/span.h"
#include "pw_trace_tokenized/trace_callback.h"

namespace pw {
namespace trace {
namespace {

class TraceBuffer {
 public:
  TraceBuffer() {
    ring_buffer_.SetBuffer(raw_buffer_)
        .IgnoreError();  // TODO(b/242598609): Handle Status properly
    Callbacks::Instance()
        .RegisterSink(
            TraceSinkStartBlock, TraceSinkAddBytes, TraceSinkEndBlock, this)
        .IgnoreError();  // TODO(b/242598609): Handle Status properly
  }

  static void TraceSinkStartBlock(void* user_data, size_t size) {
    TraceBuffer* buffer = reinterpret_cast<TraceBuffer*>(user_data);
    if (size > PW_TRACE_BUFFER_MAX_BLOCK_SIZE_BYTES) {
      buffer->block_size_ = 0;  // Skip this block
      return;
    }
    buffer->block_size_ = static_cast<uint16_t>(size);
    buffer->block_idx_ = 0;
  }

  static void TraceSinkAddBytes(void* user_data,
                                const void* bytes,
                                size_t size) {
    TraceBuffer* buffer = reinterpret_cast<TraceBuffer*>(user_data);
    if (buffer->block_size_ == 0 ||
        buffer->block_idx_ + size > buffer->block_size_) {
      return;  // Block is too large, skipping.
    }
    memcpy(&buffer->current_block_[buffer->block_idx_], bytes, size);
    buffer->block_idx_ += size;
  }

  static void TraceSinkEndBlock(void* user_data) {
    TraceBuffer* buffer = reinterpret_cast<TraceBuffer*>(user_data);
    if (buffer->block_idx_ != buffer->block_size_) {
      return;  // Block is too large, skipping.
    }
    buffer->ring_buffer_
        .PushBack(span<const std::byte>(&buffer->current_block_[0],
                                        buffer->block_size_))
        .IgnoreError();  // TODO(b/242598609): Handle Status properly
  }

  pw::ring_buffer::PrefixedEntryRingBuffer& RingBuffer() {
    return ring_buffer_;
  }

  ConstByteSpan DeringAndViewRawBuffer() {
    ring_buffer_.Dering()
        .IgnoreError();  // TODO(b/242598609): Handle Status properly
    return ByteSpan(raw_buffer_, ring_buffer_.TotalUsedBytes());
  }

 private:
  uint16_t block_size_ = 0;
  uint16_t block_idx_ = 0;
  std::byte current_block_[PW_TRACE_BUFFER_MAX_BLOCK_SIZE_BYTES];
  std::byte raw_buffer_[PW_TRACE_BUFFER_SIZE_BYTES];
  pw::ring_buffer::PrefixedEntryRingBuffer ring_buffer_{false};
};

#if PW_TRACE_BUFFER_SIZE_BYTES > 0
TraceBuffer trace_buffer_instance;
#endif  // PW_TRACE_BUFFER_SIZE_BYTES > 0

}  // namespace

void ClearBuffer() { trace_buffer_instance.RingBuffer().Clear(); }

pw::ring_buffer::PrefixedEntryRingBuffer* GetBuffer() {
  return &trace_buffer_instance.RingBuffer();
}

ConstByteSpan DeringAndViewRawBuffer() {
  return trace_buffer_instance.DeringAndViewRawBuffer();
}

}  // namespace trace
}  // namespace pw
