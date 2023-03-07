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

#include <array>
#include <cstddef>

#include "pw_bytes/span.h"
#include "pw_result/result.h"
#include "pw_span/span.h"
#include "pw_stream/seek.h"
#include "pw_stream/stream.h"

namespace pw::stream {

class MemoryWriter : public SeekableWriter {
 public:
  using difference_type = ptrdiff_t;
  using reference = const std::byte&;
  using const_reference = const std::byte&;
  using pointer = const std::byte*;
  using const_pointer = const std::byte*;
  using iterator = const std::byte*;
  using const_iterator = const std::byte*;

  constexpr MemoryWriter(ByteSpan dest) : dest_(dest) {}

  // Construct writer with prepopulated data in the buffer. The number of
  // pre-written bytes is provided as `bytes_written`.
  //
  // Precondition: The number of pre-written bytes must not be greater than the
  // size of the provided buffer.
  constexpr MemoryWriter(ByteSpan dest, size_t bytes_written)
      : dest_(dest), position_(bytes_written) {
    PW_ASSERT(position_ <= dest.size_bytes());
  }

  ConstByteSpan WrittenData() const { return dest_.first(position_); }

  void clear() { position_ = 0; }

  std::byte* data() { return dest_.data(); }
  const std::byte* data() const { return dest_.data(); }

  const std::byte& operator[](size_t index) const { return dest_[index]; }

  [[nodiscard]] bool empty() const { return size() == 0u; }

  size_t size() const { return position_; }
  size_t bytes_written() const { return size(); }

  size_t capacity() const { return dest_.size(); }

  const std::byte* begin() const { return dest_.data(); }
  const std::byte* end() const { return dest_.data() + position_; }

 private:
  size_t ConservativeLimit(LimitType type) const override {
    return type == LimitType::kWrite ? dest_.size_bytes() - position_ : 0;
  }

  // Implementation for writing data to this stream.
  //
  // If the in-memory buffer is exhausted in the middle of a write, this will
  // perform a partial write and Status::ResourceExhausted() will be returned.
  Status DoWrite(ConstByteSpan data) final;

  Status DoSeek(ptrdiff_t offset, Whence origin) final {
    return CalculateSeek(offset, origin, dest_.size(), position_);
  }

  size_t DoTell() final { return position_; }

  ByteSpan dest_;
  size_t position_ = 0;
};

template <size_t kSizeBytes>
class MemoryWriterBuffer final : public MemoryWriter {
 public:
  constexpr MemoryWriterBuffer() : MemoryWriter(buffer_) {}

 private:
  std::array<std::byte, kSizeBytes> buffer_;
};

class MemoryReader final : public SeekableReader {
 public:
  constexpr MemoryReader(ConstByteSpan source)
      : source_(source), position_(0) {}

  size_t bytes_read() const { return position_; }

  const std::byte* data() const { return source_.data(); }

 private:
  size_t ConservativeLimit(LimitType type) const override {
    return type == LimitType::kRead ? source_.size_bytes() - position_ : 0;
  }

  Status DoSeek(ptrdiff_t offset, Whence origin) override {
    return CalculateSeek(offset, origin, source_.size(), position_);
  }

  size_t DoTell() override { return position_; }

  // Implementation for reading data from this stream.
  //
  // If the in-memory buffer does not have enough remaining bytes for what was
  // requested, this will perform a partial read and OK will still be returned.
  StatusWithSize DoRead(ByteSpan dest) override;

  ConstByteSpan source_;
  size_t position_;
};

}  // namespace pw::stream
