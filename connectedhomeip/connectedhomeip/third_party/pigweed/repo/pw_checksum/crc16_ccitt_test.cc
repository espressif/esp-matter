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

#include "pw_checksum/crc16_ccitt.h"

#include <string_view>

#include "gtest/gtest.h"

namespace pw::checksum {
namespace {

// The expected CRC16 values were calculated using
//
//   http://www.sunshine2k.de/coding/javascript/crc/crc_js.html
//
// with polynomial 0x1021, initial value 0xFFFF.
constexpr uint8_t kBytes[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
constexpr uint16_t kBufferCrc = 0x3B0A;

constexpr std::string_view kString =
    "In the beginning the Universe was created. This has made a lot of "
    "people very angry and been widely regarded as a bad move.";
constexpr uint16_t kStringCrc = 0xC184;

TEST(Crc16, Empty) {
  EXPECT_EQ(Crc16Ccitt::Calculate(span<std::byte>()),
            Crc16Ccitt::kInitialValue);
}

TEST(Crc16, ByteByByte) {
  uint16_t crc = Crc16Ccitt::kInitialValue;
  for (size_t i = 0; i < sizeof(kBytes); i++) {
    crc = Crc16Ccitt::Calculate(std::byte{kBytes[i]}, crc);
  }
  EXPECT_EQ(crc, kBufferCrc);
}

TEST(Crc16, Buffer) {
  EXPECT_EQ(Crc16Ccitt::Calculate(as_bytes(span(kBytes))), kBufferCrc);
}

TEST(Crc16, String) {
  EXPECT_EQ(Crc16Ccitt::Calculate(as_bytes(span(kString))), kStringCrc);
}

TEST(Crc16Class, Buffer) {
  Crc16Ccitt crc16;
  crc16.Update(as_bytes(span(kBytes)));
  EXPECT_EQ(crc16.value(), kBufferCrc);
}

TEST(Crc16Class, String) {
  Crc16Ccitt crc16;
  crc16.Update(as_bytes(span(kString)));
  EXPECT_EQ(crc16.value(), kStringCrc);
}

extern "C" uint16_t CallChecksumCrc16Ccitt(const void* data, size_t size_bytes);

TEST(Crc16FromC, Buffer) {
  EXPECT_EQ(CallChecksumCrc16Ccitt(kBytes, sizeof(kBytes)), kBufferCrc);
}

TEST(Crc16FromC, String) {
  EXPECT_EQ(CallChecksumCrc16Ccitt(kString.data(), kString.size()), kStringCrc);
}

}  // namespace
}  // namespace pw::checksum
