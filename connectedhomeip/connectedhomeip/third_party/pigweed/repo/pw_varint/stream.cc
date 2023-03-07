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

#include "pw_varint/stream.h"

#include <cstddef>
#include <cstdint>

#include "pw_span/span.h"
#include "pw_status/status_with_size.h"
#include "pw_stream/stream.h"
#include "pw_varint/varint.h"

namespace pw {
namespace varint {

StatusWithSize Read(stream::Reader& reader, int64_t* output, size_t max_size) {
  uint64_t value = 0;
  StatusWithSize count = Read(reader, &value, max_size);
  if (!count.ok()) {
    return count;
  }

  *output = ZigZagDecode(value);
  return count;
}

StatusWithSize Read(stream::Reader& reader, uint64_t* output, size_t max_size) {
  uint64_t value = 0;
  size_t count = 0;

  while (true) {
    if (count >= varint::kMaxVarint64SizeBytes) {
      // Varint can't fit a uint64_t, this likely means we're reading binary
      // data that is not actually a varint.
      return StatusWithSize::DataLoss();
    }

    if (count >= max_size) {
      // Varint didn't fit within the range given; return OutOfRange() if
      // max_size was 0, but DataLoss if we were reading something we thought
      // was going to be a varint.
      return count > 0 ? StatusWithSize::DataLoss()
                       : StatusWithSize::OutOfRange();
    }

    std::byte b;
    if (auto result = reader.Read(span(&b, 1)); !result.ok()) {
      if (count > 0 && result.status().IsOutOfRange()) {
        // Status::OutOfRange on the first byte means we tried to read a varint
        // when we reached the end of file. But after the first byte it means we
        // failed to decode a varint we were in the middle of, and that's not
        // a normal error condition.
        return StatusWithSize(Status::DataLoss(), 0);
      }
      return StatusWithSize(result.status(), 0);
    }

    value |= static_cast<uint64_t>(b & std::byte(0b01111111)) << (7 * count);
    ++count;

    // MSB == 0 indicates last byte of the varint.
    if ((b & std::byte(0b10000000)) == std::byte(0)) {
      break;
    }
  }

  *output = value;
  return StatusWithSize(count);
}

}  // namespace varint
}  // namespace pw
