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
#include "pw_persistent_ram/persistent_buffer.h"

#include "pw_bytes/span.h"
#include "pw_checksum/crc16_ccitt.h"
#include "pw_status/status.h"

namespace pw::persistent_ram {

Status PersistentBufferWriter::DoWrite(ConstByteSpan data) {
  if (ConservativeWriteLimit() == 0) {
    return Status::OutOfRange();
  }
  if (ConservativeWriteLimit() < data.size_bytes()) {
    return Status::ResourceExhausted();
  }
  if (data.empty()) {
    return OkStatus();
  }

  std::memcpy(buffer_.data() + size_, data.data(), data.size_bytes());

  // Only checksum newly written data.
  checksum_ = checksum::Crc16Ccitt::Calculate(
      ByteSpan(buffer_.data() + size_, data.size_bytes()), checksum_);
  size_ = size_ + data.size_bytes();  // += on a volatile is deprecated in C++20

  return OkStatus();
}

}  // namespace pw::persistent_ram
