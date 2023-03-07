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

#include "pw_bytes/byte_builder.h"

namespace pw {

ByteBuilder& ByteBuilder::append(size_t count, std::byte b) {
  std::byte* const append_destination = buffer_.data() + size_;
  std::fill_n(append_destination, ResizeForAppend(count), b);
  return *this;
}

ByteBuilder& ByteBuilder::append(const void* bytes, size_t count) {
  std::byte* const append_destination = buffer_.data() + size_;
  std::copy_n(static_cast<const std::byte*>(bytes),
              ResizeForAppend(count),
              append_destination);
  return *this;
}

size_t ByteBuilder::ResizeForAppend(size_t bytes_to_append) {
  if (!status_.ok()) {
    return 0;
  }

  if (bytes_to_append > max_size() - size()) {
    status_ = Status::ResourceExhausted();
    return 0;
  }

  size_ += bytes_to_append;
  status_ = OkStatus();
  return bytes_to_append;
}

void ByteBuilder::resize(size_t new_size) {
  if (new_size <= size_) {
    size_ = new_size;
    status_ = OkStatus();
  } else {
    status_ = Status::OutOfRange();
  }
}

}  // namespace pw
