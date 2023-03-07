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

#include "pw_hdlc/encoder.h"

#include <algorithm>
#include <array>
#include <cstddef>

#include "gtest/gtest.h"
#include "pw_bytes/array.h"
#include "pw_hdlc/encoded_size.h"
#include "pw_hdlc/internal/encoder.h"
#include "pw_hdlc/internal/protocol.h"
#include "pw_stream/memory_stream.h"

using std::byte;

namespace pw::hdlc {
namespace {

constexpr uint8_t kAddress = 0x7B;  // 123
constexpr uint8_t kEncodedAddress = (kAddress << 1) | 1;

#define EXPECT_ENCODER_WROTE(...)                                           \
  do {                                                                      \
    constexpr auto expected_data = (__VA_ARGS__);                           \
    EXPECT_EQ(writer_.bytes_written(), expected_data.size());               \
    EXPECT_EQ(                                                              \
        std::memcmp(                                                        \
            writer_.data(), expected_data.data(), writer_.bytes_written()), \
        0);                                                                 \
  } while (0)

class WriteUnnumberedFrame : public ::testing::Test {
 protected:
  WriteUnnumberedFrame() : writer_(buffer_) {}

  stream::MemoryWriter writer_;
  // Allocate a buffer that will fit any 7-byte payload.
  std::array<byte, MaxEncodedFrameSize(7)> buffer_;
};

constexpr byte kUnnumberedControl = byte{0x3};

TEST_F(WriteUnnumberedFrame, EmptyPayload) {
  ASSERT_EQ(OkStatus(), WriteUIFrame(kAddress, span<byte>(), writer_));
  EXPECT_ENCODER_WROTE(bytes::Concat(
      kFlag, kEncodedAddress, kUnnumberedControl, uint32_t{0x832d343f}, kFlag));
}

TEST_F(WriteUnnumberedFrame, OneBytePayload) {
  ASSERT_EQ(OkStatus(), WriteUIFrame(kAddress, bytes::String("A"), writer_));
  EXPECT_ENCODER_WROTE(bytes::Concat(kFlag,
                                     kEncodedAddress,
                                     kUnnumberedControl,
                                     'A',
                                     uint32_t{0x653c9e82},
                                     kFlag));
}

TEST_F(WriteUnnumberedFrame, OneBytePayload_Escape0x7d) {
  ASSERT_EQ(OkStatus(), WriteUIFrame(kAddress, bytes::Array<0x7d>(), writer_));
  EXPECT_ENCODER_WROTE(bytes::Concat(kFlag,
                                     kEncodedAddress,
                                     kUnnumberedControl,
                                     kEscape,
                                     byte{0x7d} ^ byte{0x20},
                                     uint32_t{0x4a53e205},
                                     kFlag));
}

TEST_F(WriteUnnumberedFrame, OneBytePayload_Escape0x7E) {
  ASSERT_EQ(OkStatus(), WriteUIFrame(kAddress, bytes::Array<0x7e>(), writer_));
  EXPECT_ENCODER_WROTE(bytes::Concat(kFlag,
                                     kEncodedAddress,
                                     kUnnumberedControl,
                                     kEscape,
                                     byte{0x7e} ^ byte{0x20},
                                     uint32_t{0xd35ab3bf},
                                     kFlag));
}

TEST_F(WriteUnnumberedFrame, AddressNeedsEscaping) {
  // Becomes 0x7d when encoded.
  constexpr uint8_t kEscapeRequiredAddress = 0x7d >> 1;
  ASSERT_EQ(OkStatus(),
            WriteUIFrame(kEscapeRequiredAddress, bytes::String("A"), writer_));
  EXPECT_ENCODER_WROTE(bytes::Concat(kFlag,
                                     kEscape,
                                     byte{0x5d},
                                     kUnnumberedControl,
                                     'A',
                                     uint32_t{0x899E00D4},
                                     kFlag));
}

TEST_F(WriteUnnumberedFrame, Crc32NeedsEscaping) {
  ASSERT_EQ(OkStatus(), WriteUIFrame(kAddress, bytes::String("aa"), writer_));

  // The CRC-32 of {kEncodedAddress, kUnnumberedControl, "aa"} is 0x7ee04473, so
  // the 0x7e must be escaped.
  constexpr auto expected_crc32 = bytes::Array<0x73, 0x44, 0xe0, 0x7d, 0x5e>();
  EXPECT_ENCODER_WROTE(bytes::Concat(kFlag,
                                     kEncodedAddress,
                                     kUnnumberedControl,
                                     bytes::String("aa"),
                                     expected_crc32,
                                     kFlag));
}

TEST_F(WriteUnnumberedFrame, MultiplePayloads) {
  ASSERT_EQ(OkStatus(), WriteUIFrame(kAddress, bytes::String("ABC"), writer_));
  ASSERT_EQ(OkStatus(), WriteUIFrame(kAddress, bytes::String("DEF"), writer_));
  EXPECT_ENCODER_WROTE(bytes::Concat(kFlag,
                                     kEncodedAddress,
                                     kUnnumberedControl,
                                     bytes::String("ABC"),
                                     uint32_t{0x72410ee4},
                                     kFlag,
                                     kFlag,
                                     kEncodedAddress,
                                     kUnnumberedControl,
                                     bytes::String("DEF"),
                                     uint32_t{0x4ba1ae47},
                                     kFlag));
}

TEST_F(WriteUnnumberedFrame, PayloadWithNoEscapes) {
  ASSERT_EQ(
      OkStatus(),
      WriteUIFrame(kAddress, bytes::String("1995 toyota corolla"), writer_));

  EXPECT_ENCODER_WROTE(bytes::Concat(kFlag,
                                     kEncodedAddress,
                                     kUnnumberedControl,
                                     bytes::String("1995 toyota corolla"),
                                     uint32_t{0x53ee911c},
                                     kFlag));
}

TEST_F(WriteUnnumberedFrame, MultibyteAddress) {
  ASSERT_EQ(OkStatus(), WriteUIFrame(0x3fff, bytes::String("abc"), writer_));

  EXPECT_ENCODER_WROTE(bytes::Concat(kFlag,
                                     bytes::String("\xfe\xff"),
                                     kUnnumberedControl,
                                     bytes::String("abc"),
                                     uint32_t{0x8cee2978},
                                     kFlag));
}

TEST_F(WriteUnnumberedFrame, PayloadWithMultipleEscapes) {
  ASSERT_EQ(
      OkStatus(),
      WriteUIFrame(kAddress,
                   bytes::Array<0x7E, 0x7B, 0x61, 0x62, 0x63, 0x7D, 0x7E>(),
                   writer_));
  EXPECT_ENCODER_WROTE(bytes::Concat(
      kFlag,
      kEncodedAddress,
      kUnnumberedControl,
      bytes::
          Array<0x7D, 0x5E, 0x7B, 0x61, 0x62, 0x63, 0x7D, 0x5D, 0x7D, 0x5E>(),
      uint32_t{0x1563a4e6},
      kFlag));
}

TEST_F(WriteUnnumberedFrame, PayloadTooLarge_WritesNothing) {
  constexpr auto data = bytes::Initialized<sizeof(buffer_)>(0x7e);
  EXPECT_EQ(Status::ResourceExhausted(), WriteUIFrame(kAddress, data, writer_));
  EXPECT_EQ(0u, writer_.bytes_written());
}

class ErrorWriter : public stream::NonSeekableWriter {
 private:
  Status DoWrite(ConstByteSpan) override { return Status::Unimplemented(); }
};

TEST(WriteUIFrame, WriterError) {
  ErrorWriter writer;
  EXPECT_EQ(Status::Unimplemented(),
            WriteUIFrame(kAddress, bytes::Array<0x01>(), writer));
}

}  // namespace
}  // namespace pw::hdlc
