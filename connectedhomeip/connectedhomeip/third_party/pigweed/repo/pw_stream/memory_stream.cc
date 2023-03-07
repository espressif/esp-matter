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

#include "pw_stream/memory_stream.h"

#include <cstddef>
#include <cstring>

#include "pw_status/status_with_size.h"

namespace pw::stream {

Status MemoryWriter::DoWrite(ConstByteSpan data) {
  if (ConservativeWriteLimit() == 0) {
    return Status::OutOfRange();
  }
  if (ConservativeWriteLimit() < data.size_bytes()) {
    return Status::ResourceExhausted();
  }

  size_t bytes_to_write = data.size_bytes();
  if (bytes_to_write == 0) {
    // Calling memmove with a null pointer is undefined behavior, even when zero
    // bytes are moved. We must return early here to avoid performing such a
    // call when data is an empty span.
    return OkStatus();
  }
  std::memmove(dest_.data() + position_, data.data(), bytes_to_write);
  position_ += bytes_to_write;

  return OkStatus();
}

StatusWithSize MemoryReader::DoRead(ByteSpan dest) {
  if (source_.size_bytes() == position_) {
    return StatusWithSize::OutOfRange();
  }

  size_t bytes_to_read =
      std::min(dest.size_bytes(), source_.size_bytes() - position_);
  if (bytes_to_read == 0) {
    // Calling memcpy with a null pointer is undefined behavior, even when zero
    // bytes are copied. We must return early here to avoid performing such a
    // call when the dest span is empty.
    return StatusWithSize(0);
  }

  std::memcpy(dest.data(), source_.data() + position_, bytes_to_read);
  position_ += bytes_to_read;

  return StatusWithSize(bytes_to_read);
}

}  // namespace pw::stream
