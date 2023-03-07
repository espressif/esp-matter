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

#include "pw_hdlc/decoder.h"

#include <array>
#include <cstddef>

#include "gtest/gtest.h"
#include "pw_bytes/array.h"
#include "pw_hdlc/internal/protocol.h"

namespace pw::hdlc {
namespace {

using std::byte;

TEST(Frame, Fields) {
  static constexpr auto kFrameData =
      bytes::String("\x05\xab\x42\x24\xf9\x54\xfb\x3d");
  auto result = Frame::Parse(kFrameData);
  ASSERT_TRUE(result.ok());
  Frame& frame = result.value();

  EXPECT_EQ(frame.address(), 2u);
  EXPECT_EQ(frame.control(), byte{0xab});

  EXPECT_EQ(frame.data().size(), 2u);
  EXPECT_EQ(frame.data()[0], byte{0x42});
  EXPECT_EQ(frame.data()[1], byte{0x24});
}

TEST(Frame, MultibyteAddress) {
  static constexpr auto kFrameData =
      bytes::String("\x2c\xd9\x33\x01\x02\xaf\xc8\x77\x48");
  auto result = Frame::Parse(kFrameData);
  ASSERT_TRUE(result.ok());
  Frame& frame = result.value();

  EXPECT_EQ(frame.address(), 0b11011000010110u);
  EXPECT_EQ(frame.control(), byte{0x33});

  EXPECT_EQ(frame.data().size(), 2u);
  EXPECT_EQ(frame.data()[0], byte{0x01});
  EXPECT_EQ(frame.data()[1], byte{0x02});
}

TEST(Frame, MultibyteAddressTooLong) {
  // 11-byte encoded address.
  constexpr auto kLongAddress =
      bytes::String("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01");
  static constexpr auto kFrameData = bytes::Concat(
      kLongAddress, bytes::String("\x33\x01\x02\xaf\xc8\x77\x48"));
  auto result = Frame::Parse(kFrameData);
  EXPECT_EQ(result.status(), Status::DataLoss());
}

TEST(Decoder, Clear) {
  DecoderBuffer<8> decoder;

  // Process a partial packet
  decoder.Process(bytes::String("~1234abcd"),
                  [](const Result<Frame>&) { FAIL(); });

  decoder.Clear();
  Status status = Status::Unknown();

  decoder.Process(
      bytes::String("~1234\xa3\xe0\xe3\x9b~"),
      [&status](const Result<Frame>& result) { status = result.status(); });

  EXPECT_EQ(OkStatus(), status);
}

TEST(Decoder, ExactFit) {
  DecoderBuffer<8> decoder;

  for (byte b : bytes::String("~1234\xa3\xe0\xe3\x9b")) {
    EXPECT_EQ(Status::Unavailable(), decoder.Process(b).status());
  }
  auto result = decoder.Process(kFlag);
  ASSERT_EQ(OkStatus(), result.status());
  ASSERT_EQ(result.value().data().size(), 2u);
  ASSERT_EQ(result.value().data()[0], byte{'3'});
  ASSERT_EQ(result.value().data()[1], byte{'4'});
}

TEST(Decoder, MinimumSizedBuffer) {
  DecoderBuffer<6> decoder;

  for (byte b : bytes::String("~12\xcd\x44\x53\x4f")) {
    EXPECT_EQ(Status::Unavailable(), decoder.Process(b).status());
  }

  auto result = decoder.Process(kFlag);
  ASSERT_EQ(OkStatus(), result.status());
  EXPECT_EQ(result.value().data().size(), 0u);
}

TEST(Decoder, TooLargeForBuffer_ReportsResourceExhausted) {
  DecoderBuffer<8> decoder;

  for (byte b : bytes::String("~12345\x1c\x3a\xf5\xcb")) {
    EXPECT_EQ(Status::Unavailable(), decoder.Process(b).status());
  }
  EXPECT_EQ(Status::ResourceExhausted(), decoder.Process(kFlag).status());

  for (byte b : bytes::String("~12345678901234567890\xf2\x19\x63\x90")) {
    EXPECT_EQ(Status::Unavailable(), decoder.Process(b).status());
  }
  EXPECT_EQ(Status::ResourceExhausted(), decoder.Process(kFlag).status());
}

TEST(Decoder, TooLargeForBuffer_StaysWithinBufferBoundaries) {
  std::array<byte, 16> buffer = bytes::Initialized<16>('?');

  Decoder decoder(span(buffer.data(), 8));

  for (byte b : bytes::String("~12345678901234567890\xf2\x19\x63\x90")) {
    EXPECT_EQ(Status::Unavailable(), decoder.Process(b).status());
  }

  for (size_t i = 8; i < buffer.size(); ++i) {
    ASSERT_EQ(byte{'?'}, buffer[i]);
  }

  EXPECT_EQ(Status::ResourceExhausted(), decoder.Process(kFlag).status());
}

TEST(Decoder, TooLargeForBuffer_DecodesNextFrame) {
  DecoderBuffer<8> decoder;

  for (byte b : bytes::String("~12345678901234567890\xf2\x19\x63\x90")) {
    EXPECT_EQ(Status::Unavailable(), decoder.Process(b).status());
  }
  EXPECT_EQ(Status::ResourceExhausted(), decoder.Process(kFlag).status());

  for (byte b : bytes::String("1234\xa3\xe0\xe3\x9b")) {
    EXPECT_EQ(Status::Unavailable(), decoder.Process(b).status());
  }
  EXPECT_EQ(OkStatus(), decoder.Process(kFlag).status());
}

}  // namespace
}  // namespace pw::hdlc
