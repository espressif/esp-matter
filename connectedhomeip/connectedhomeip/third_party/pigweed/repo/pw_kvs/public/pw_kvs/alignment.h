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
#pragma once

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <initializer_list>
#include <utility>

#include "pw_bytes/span.h"
#include "pw_kvs/io.h"
#include "pw_span/span.h"
#include "pw_status/status_with_size.h"

namespace pw {

// Returns the value rounded down to the nearest multiple of alignment.
constexpr size_t AlignDown(size_t value, size_t alignment) {
  return (value / alignment) * alignment;
}

// Returns the value rounded up to the nearest multiple of alignment.
constexpr size_t AlignUp(size_t value, size_t alignment) {
  return (value + alignment - 1) / alignment * alignment;
}

// Returns the number of padding bytes required to align the provided length.
constexpr size_t Padding(size_t length, size_t alignment) {
  return AlignUp(length, alignment) - length;
}

// Class for managing aligned writes. Stores data in an intermediate buffer and
// calls an output function with aligned data as the buffer becomes full. Any
// bytes remaining in the buffer are written to the output when Flush() is
// called or the AlignedWriter goes out of scope.
class AlignedWriter {
 public:
  AlignedWriter(span<std::byte> buffer, size_t alignment_bytes, Output& writer)
      : buffer_(buffer.data()),
        write_size_(AlignDown(buffer.size(), alignment_bytes)),
        alignment_bytes_(alignment_bytes),
        output_(writer),
        bytes_written_(0),
        bytes_in_buffer_(0) {
    // TODO(hepler): Add DCHECK to ensure that buffer.size() >= alignment_bytes.
  }

  AlignedWriter(const AlignedWriter&) = delete;
  AlignedWriter& operator=(const AlignedWriter&) = delete;

  ~AlignedWriter() {
    Flush().IgnoreError();  // TODO(b/242598609): Handle Status properly
  }

  // Writes bytes to the AlignedWriter. The output may be called if the internal
  // buffer is filled.
  //
  // The size in the return value is the total number of bytes for which a write
  // has been attempted since Flush or Reset. The size is set for both
  // successful and failed Write calls. On a failed write call, knowing the
  // bytes attempted may be important when working with flash memory, since it
  // can only be written once between erases.
  StatusWithSize Write(span<const std::byte> data);

  StatusWithSize Write(const void* data, size_t size) {
    return Write(
        span<const std::byte>(static_cast<const std::byte*>(data), size));
  }

  // Reads size bytes from the input and writes them to the output.
  StatusWithSize Write(Input& input, size_t size);

  // Flush and reset the AlignedWriter. Any remaining bytes in the buffer are
  // zero-padded to an alignment boundary and written to the output. Flush is
  // automatically called when the AlignedWriter goes out of scope.
  StatusWithSize Flush();

 private:
  static constexpr std::byte kPadByte = static_cast<std::byte>(0);

  StatusWithSize AddBytesToBuffer(size_t bytes_added);

  std::byte* const buffer_;
  const size_t write_size_;
  const size_t alignment_bytes_;

  Output& output_;
  size_t bytes_written_;
  size_t bytes_in_buffer_;
};

// Declares an AlignedWriter with a built-in buffer.
template <size_t kBufferSize>
class AlignedWriterBuffer : public AlignedWriter {
 public:
  template <typename... Args>
  AlignedWriterBuffer(Args&&... aligned_writer_args)
      : AlignedWriter(buffer_, std::forward<Args>(aligned_writer_args)...) {}

 private:
  std::byte buffer_[kBufferSize];
};

// Writes data from multiple buffers using an AlignedWriter.
template <size_t kBufferSize>
StatusWithSize AlignedWrite(Output& output,
                            size_t alignment_bytes,
                            span<const span<const std::byte>> data) {
  // TODO(davidrogers): This should convert to PW_CHECK once that is available
  // for use in host tests.
  if (alignment_bytes > kBufferSize) {
    return StatusWithSize::Internal();
  }

  AlignedWriterBuffer<kBufferSize> buffer(alignment_bytes, output);

  for (const span<const std::byte>& chunk : data) {
    StatusWithSize result = buffer.Write(chunk);
    if (!result.ok()) {
      return result;
    }
  }

  return buffer.Flush();
}

// Calls AlignedWrite with an initializer list.
template <size_t kBufferSize>
StatusWithSize AlignedWrite(Output& output,
                            size_t alignment_bytes,
                            std::initializer_list<span<const std::byte>> data) {
  return AlignedWrite<kBufferSize>(
      output,
      alignment_bytes,
      span<const ConstByteSpan>(data.begin(), data.size()));
}

}  // namespace pw
