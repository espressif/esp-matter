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

#include "pw_base64/base64.h"

#include <cstdint>

#include "pw_assert/check.h"

namespace pw::base64 {
namespace {

// Encoding functions
constexpr size_t kEncodedGroupSize = 4;
constexpr char kChar62 = '+';  // URL safe encoding uses - instead
constexpr char kChar63 = '/';  // URL safe encoding uses _ instead
constexpr char kPadding = '=';

// Table that encodes a 6-bit pattern as a Base64 character
constexpr char kEncodeTable[64] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K',     'L',    'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',     'Y',    'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k',     'l',    'm',
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',     'y',    'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', kChar62, kChar63};

constexpr char BitGroup0Char(uint8_t byte0) {
  return kEncodeTable[(byte0 & 0b11111100) >> 2];
}
constexpr char BitGroup1Char(uint8_t byte0, uint8_t byte1 = 0) {
  return kEncodeTable[((byte0 & 0b00000011) << 4) |
                      ((byte1 & 0b11110000) >> 4)];
}
constexpr char BitGroup2Char(uint8_t byte1, uint8_t byte2 = 0) {
  return kEncodeTable[((byte1 & 0b00001111) << 2) |
                      ((byte2 & 0b11000000) >> 6)];
}
constexpr char BitGroup3Char(uint8_t byte2) {
  return kEncodeTable[byte2 & 0b00111111];
}

// Decoding functions
constexpr char kMinValidChar = '+';
constexpr char kMaxValidChar = 'z';
constexpr uint8_t kX = 0xff;  // Value used for invalid characters

// Table that decodes a Base64 character to its 6-bit value. Supports the
// standard (+/) and URL-safe (-_) alphabets. Starts from the lowest-value valid
// character, which is +.
constexpr uint8_t kDecodeTable[] = {
    62, kX, 62, kX, 63, 52, 53, 54, 55, 56,  //  0 - 09
    57, 58, 59, 60, 61, kX, kX, kX, 0,  kX,  // 10 - 19
    kX, kX, 0,  1,  2,  3,  4,  5,  6,  7,   // 20 - 29
    8,  9,  10, 11, 12, 13, 14, 15, 16, 17,  // 30 - 39
    18, 19, 20, 21, 22, 23, 24, 25, kX, kX,  // 40 - 49
    kX, kX, 63, kX, 26, 27, 28, 29, 30, 31,  // 50 - 59
    32, 33, 34, 35, 36, 37, 38, 39, 40, 41,  // 60 - 69
    42, 43, 44, 45, 46, 47, 48, 49, 50, 51,  // 70 - 79
};

constexpr uint8_t CharToBits(char ch) {
  return kDecodeTable[ch - kMinValidChar];
}

constexpr uint8_t Byte0(uint8_t bits0, uint8_t bits1) {
  return (bits0 << 2) | ((bits1 & 0b110000) >> 4);
}
constexpr uint8_t Byte1(uint8_t bits1, uint8_t bits2) {
  return ((bits1 & 0b001111) << 4) | ((bits2 & 0b111100) >> 2);
}
constexpr uint8_t Byte2(uint8_t bits2, uint8_t bits3) {
  return ((bits2 & 0b000011) << 6) | bits3;
}

}  // namespace

extern "C" void pw_Base64Encode(const void* binary_data,
                                const size_t binary_size_bytes,
                                char* output) {
  const uint8_t* bytes = static_cast<const uint8_t*>(binary_data);

  // Encode groups of 3 source bytes into 4 output characters.
  size_t remaining = binary_size_bytes;
  for (; remaining >= 3u; remaining -= 3u, bytes += 3) {
    *output++ = BitGroup0Char(bytes[0]);
    *output++ = BitGroup1Char(bytes[0], bytes[1]);
    *output++ = BitGroup2Char(bytes[1], bytes[2]);
    *output++ = BitGroup3Char(bytes[2]);
  }

  // If the source data length isn't a multiple of 3, pad the end with either 1
  // or 2 '=' characters, to stay Python-compatible.
  if (remaining > 0u) {
    *output++ = BitGroup0Char(bytes[0]);
    if (remaining == 1u) {
      *output++ = BitGroup1Char(bytes[0]);
      *output++ = kPadding;
    } else {
      *output++ = BitGroup1Char(bytes[0], bytes[1]);
      *output++ = BitGroup2Char(bytes[1]);
    }
    *output++ = kPadding;
  }
}

extern "C" size_t pw_Base64Decode(const char* base64,
                                  size_t base64_size_bytes,
                                  void* output) {
  // If too small, can't be valid input, due to likely missing padding
  if (base64_size_bytes < 4) {
    return 0;
  }

  uint8_t* binary = static_cast<uint8_t*>(output);
  for (size_t ch = 0; ch < base64_size_bytes; ch += kEncodedGroupSize) {
    const uint8_t char0 = CharToBits(base64[ch + 0]);
    const uint8_t char1 = CharToBits(base64[ch + 1]);
    const uint8_t char2 = CharToBits(base64[ch + 2]);
    const uint8_t char3 = CharToBits(base64[ch + 3]);

    *binary++ = Byte0(char0, char1);
    *binary++ = Byte1(char1, char2);
    *binary++ = Byte2(char2, char3);
  }

  size_t pad = 0;
  if (base64[base64_size_bytes - 2] == kPadding) {
    pad = 2;
  } else if (base64[base64_size_bytes - 1] == kPadding) {
    pad = 1;
  }

  return binary - static_cast<uint8_t*>(output) - pad;
}

extern "C" bool pw_Base64IsValid(const char* base64_data, size_t base64_size) {
  if (base64_size % kEncodedGroupSize != 0) {
    return false;
  }

  for (size_t i = 0; i < base64_size; ++i) {
    if (base64_data[i] < kMinValidChar || base64_data[i] > kMaxValidChar ||
        CharToBits(base64_data[i]) == kX /* invalid char */) {
      return false;
    }
  }
  return true;
}

size_t Encode(span<const std::byte> binary, span<char> output_buffer) {
  const size_t required_size = EncodedSize(binary.size_bytes());
  if (output_buffer.size_bytes() < required_size) {
    return 0;
  }
  pw_Base64Encode(binary.data(), binary.size_bytes(), output_buffer.data());
  return required_size;
}

size_t Decode(std::string_view base64, span<std::byte> output_buffer) {
  if (output_buffer.size_bytes() < MaxDecodedSize(base64.size()) ||
      !IsValid(base64)) {
    return 0;
  }
  return Decode(base64, output_buffer.data());
}

void Encode(span<const std::byte> binary, InlineString<>& output) {
  const size_t initial_size = output.size();
  const size_t final_size = initial_size + EncodedSize(binary.size());

  PW_CHECK(final_size <= output.capacity());

  output.resize_and_overwrite([&](char* data, size_t) {
    Encode(binary, data + initial_size);
    return final_size;
  });
}

}  // namespace pw::base64
