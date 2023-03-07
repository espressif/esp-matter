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

#include "pw_kvs/alignment.h"

#include "pw_status/try.h"

namespace pw {

StatusWithSize AlignedWriter::Write(span<const std::byte> data) {
  while (!data.empty()) {
    size_t to_copy = std::min(write_size_ - bytes_in_buffer_, data.size());

    std::memcpy(&buffer_[bytes_in_buffer_], data.data(), to_copy);
    PW_TRY_WITH_SIZE(AddBytesToBuffer(to_copy));
    data = data.subspan(to_copy);
  }

  return StatusWithSize(bytes_written_);
}

StatusWithSize AlignedWriter::Flush() {
  Status status;

  // If data remains in the buffer, pad it to the alignment size and flush the
  // remaining data.
  if (bytes_in_buffer_ != 0u) {
    const size_t remaining_bytes = AlignUp(bytes_in_buffer_, alignment_bytes_);
    std::memset(&buffer_[bytes_in_buffer_],
                int(kPadByte),
                remaining_bytes - bytes_in_buffer_);
    status = output_.Write(buffer_, remaining_bytes).status();

    bytes_written_ += remaining_bytes;  // Include padding in the total.
    bytes_in_buffer_ = 0;
  }

  const StatusWithSize result(status, bytes_written_);
  bytes_written_ = 0;
  return result;
}

StatusWithSize AlignedWriter::Write(Input& input, size_t size) {
  while (size > 0u) {
    const size_t to_read = std::min(write_size_ - bytes_in_buffer_, size);
    StatusWithSize result = input.Read(buffer_ + bytes_in_buffer_, to_read);
    if (!result.ok()) {
      return StatusWithSize(result.status(), bytes_written_);
    }
    PW_TRY_WITH_SIZE(AddBytesToBuffer(to_read));
    size -= result.size();
  }

  return StatusWithSize(bytes_written_);
}

StatusWithSize AlignedWriter::AddBytesToBuffer(size_t bytes_added) {
  bytes_in_buffer_ += bytes_added;

  // If the buffer is full, write it out.
  if (bytes_in_buffer_ == write_size_) {
    StatusWithSize result = output_.Write(buffer_, write_size_);

    // Always use write_size_ for the bytes written. If there was an error
    // assume the space was written or at least disturbed.
    bytes_written_ += write_size_;
    bytes_in_buffer_ = 0;

    if (!result.ok()) {
      return StatusWithSize(result.status(), bytes_written_);
    }
  }

  return StatusWithSize(bytes_written_);
}

}  // namespace pw
