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

#include "pw_varint/varint.h"

#include <algorithm>
#include <cstddef>

namespace pw {
namespace varint {
namespace {

inline bool ZeroTerminated(pw_varint_Format format) {
  return (static_cast<unsigned>(format) & 0b10) == 0;
}

inline bool LeastSignificant(pw_varint_Format format) {
  return (static_cast<unsigned>(format) & 0b01) == 0;
}

}  // namespace

extern "C" size_t pw_varint_EncodeCustom(uint64_t integer,
                                         void* output,
                                         size_t output_size,
                                         pw_varint_Format format) {
  size_t written = 0;
  std::byte* buffer = static_cast<std::byte*>(output);

  int value_shift = LeastSignificant(format) ? 1 : 0;
  int term_shift = value_shift == 1 ? 0 : 7;

  std::byte cont, term;
  if (ZeroTerminated(format)) {
    cont = std::byte(0x01) << term_shift;
    term = std::byte(0x00) << term_shift;
  } else {
    cont = std::byte(0x00) << term_shift;
    term = std::byte(0x01) << term_shift;
  }

  do {
    if (written >= output_size) {
      return 0;
    }

    bool last_byte = (integer >> 7) == 0u;

    // Grab 7 bits and set the eighth according to the continuation bit.
    std::byte value = (static_cast<std::byte>(integer) & std::byte(0x7f))
                      << value_shift;

    if (last_byte) {
      value |= term;
    } else {
      value |= cont;
    }

    buffer[written++] = value;
    integer >>= 7;
  } while (integer != 0u);

  return written;
}

extern "C" size_t pw_varint_DecodeCustom(const void* input,
                                         size_t input_size,
                                         uint64_t* output,
                                         pw_varint_Format format) {
  uint64_t decoded_value = 0;
  uint_fast8_t count = 0;
  const std::byte* buffer = static_cast<const std::byte*>(input);

  // The largest 64-bit ints require 10 B.
  const size_t max_count = std::min(kMaxVarint64SizeBytes, input_size);

  std::byte mask;
  uint32_t shift;
  if (LeastSignificant(format)) {
    mask = std::byte(0xfe);
    shift = 1;
  } else {
    mask = std::byte(0x7f);
    shift = 0;
  }

  // Determines whether a byte is the last byte of a varint.
  auto is_last_byte = [&](std::byte byte) {
    if (ZeroTerminated(format)) {
      return (byte & ~mask) == std::byte(0);
    }
    return (byte & ~mask) != std::byte(0);
  };

  while (true) {
    if (count >= max_count) {
      return 0;
    }

    // Add the bottom seven bits of the next byte to the result.
    decoded_value |= static_cast<uint64_t>((buffer[count] & mask) >> shift)
                     << (7 * count);

    // Stop decoding if the end is reached.
    if (is_last_byte(buffer[count++])) {
      break;
    }
  }

  *output = decoded_value;
  return count;
}

// TODO(frolv): Remove this deprecated alias.
extern "C" size_t pw_VarintEncode(uint64_t integer,
                                  void* output,
                                  size_t output_size) {
  return pw_varint_Encode(integer, output, output_size);
}

extern "C" size_t pw_varint_ZigZagEncode(int64_t integer,
                                         void* output,
                                         size_t output_size) {
  return pw_varint_Encode(ZigZagEncode(integer), output, output_size);
}

// TODO(frolv): Remove this deprecated alias.
extern "C" size_t pw_VarintDecode(const void* input,
                                  size_t input_size,
                                  uint64_t* output) {
  return pw_varint_Decode(input, input_size, output);
}

extern "C" size_t pw_varint_ZigZagDecode(const void* input,
                                         size_t input_size,
                                         int64_t* output) {
  uint64_t value = 0;
  size_t bytes = pw_varint_Decode(input, input_size, &value);
  *output = ZigZagDecode(value);
  return bytes;
}

extern "C" size_t pw_varint_EncodedSize(uint64_t integer) {
  return EncodedSize(integer);
}

extern "C" size_t pw_varint_ZigZagEncodedSize(int64_t integer) {
  return ZigZagEncodedSize(integer);
}

}  // namespace varint
}  // namespace pw
