// Copyright 2022 The Pigweed Authors
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

#include "pw_hdlc/encoded_size.h"

#include <array>
#include <cstddef>
#include <cstdint>

#include "gtest/gtest.h"
#include "pw_bytes/array.h"
#include "pw_hdlc/decoder.h"
#include "pw_hdlc/encoder.h"
#include "pw_hdlc/internal/encoder.h"
#include "pw_result/result.h"
#include "pw_stream/memory_stream.h"
#include "pw_varint/varint.h"

namespace pw::hdlc {
namespace {

// The varint-encoded address that represents the value that will result in the
// largest on-the-wire address after HDLC escaping.
constexpr auto kWidestVarintAddress =
    bytes::String("\x7e\x7e\x7e\x7e\x7e\x7e\x7e\x7e\x7e\x03");

// This is the decoded varint value of kWidestVarintAddress. This is
// pre-calculated as a constant to simplify tests.
constexpr uint64_t kWidestAddress = 0xbf7efdfbf7efdfbf;

// UI frames created by WriteUIFrame() will never be have an escaped control
// field, but it's technically possible for other HDLC frame types to produce
// control bytes that would need to be escaped.
constexpr size_t kEscapedControlCost = kControlSize;

// UI frames created by WriteUIFrame() will never have an escaped control
// field, but it's technically possible for other HDLC frame types to produce
// control bytes that would need to be escaped.
constexpr size_t kEscapedFcsCost = kMaxEscapedFcsSize - kFcsSize;

// Due to API limitations, the worst case buffer calculations used by the HDLC
// encoder/decoder can't be fully saturated. This constexpr value accounts for
// this by expressing the delta between the constant largest testable HDLC frame
// and the calculated worst-case-scenario.
constexpr size_t kTestLimitationsOverhead =
    kEscapedControlCost + kEscapedFcsCost;

// A payload only containing bytes that need to be escaped.
constexpr auto kFullyEscapedPayload =
    bytes::String("\x7e\x7e\x7e\x7e\x7e\x7e\x7e\x7e");

constexpr uint8_t kEscapeAddress = static_cast<uint8_t>(kFlag);
constexpr uint8_t kNoEscapeAddress = 6;

TEST(EncodedSize, Constants_WidestAddress) {
  uint64_t address = 0;
  size_t address_size =
      varint::Decode(kWidestVarintAddress, &address, kAddressFormat);
  EXPECT_EQ(address_size, 10u);
  EXPECT_EQ(address_size, kMaxAddressSize);
  EXPECT_EQ(kMaxEscapedVarintAddressSize, 19u);
  EXPECT_EQ(EscapedSize(kWidestVarintAddress), kMaxEscapedVarintAddressSize);
  EXPECT_EQ(address, kWidestAddress);
  EXPECT_EQ(varint::EncodedSize(kWidestAddress), 10u);
}

TEST(EncodedSize, EscapedSize_AllEscapeBytes) {
  EXPECT_EQ(EscapedSize(kFullyEscapedPayload), kFullyEscapedPayload.size() * 2);
}

TEST(EncodedSize, EscapedSize_NoEscapeBytes) {
  constexpr auto kData = bytes::String("\x01\x23\x45\x67\x89\xab\xcd\xef");
  EXPECT_EQ(EscapedSize(kData), kData.size());
}

TEST(EncodedSize, EscapedSize_SomeEscapeBytes) {
  constexpr auto kData = bytes::String("\x7epabu\x7d");
  EXPECT_EQ(EscapedSize(kData), kData.size() + 2);
}

TEST(EncodedSize, EscapedSize_Address) {
  EXPECT_EQ(EscapedSize(kWidestVarintAddress),
            varint::EncodedSize(kWidestAddress) * 2 - 1);
}

TEST(EncodedSize, MaxEncodedSize_Overload) {
  EXPECT_EQ(MaxEncodedFrameSize(kFullyEscapedPayload.size()),
            MaxEncodedFrameSize(kWidestAddress, kFullyEscapedPayload));
}

TEST(EncodedSize, MaxEncodedSize_EmptyPayload) {
  EXPECT_EQ(14u, MaxEncodedFrameSize(kNoEscapeAddress, {}));
  EXPECT_EQ(14u, MaxEncodedFrameSize(kEscapeAddress, {}));
}

TEST(EncodedSize, MaxEncodedSize_PayloadWithoutEscapes) {
  constexpr auto data = bytes::Array<0x00, 0x01, 0x02, 0x03>();
  EXPECT_EQ(18u, MaxEncodedFrameSize(kNoEscapeAddress, data));
  EXPECT_EQ(18u, MaxEncodedFrameSize(kEscapeAddress, data));
}

TEST(EncodedSize, MaxEncodedSize_PayloadWithOneEscape) {
  constexpr auto data = bytes::Array<0x00, 0x01, 0x7e, 0x03>();
  EXPECT_EQ(19u, MaxEncodedFrameSize(kNoEscapeAddress, data));
  EXPECT_EQ(19u, MaxEncodedFrameSize(kEscapeAddress, data));
}

TEST(EncodedSize, MaxEncodedSize_PayloadWithAllEscapes) {
  constexpr auto data = bytes::Initialized<8>(0x7e);
  EXPECT_EQ(30u, MaxEncodedFrameSize(kNoEscapeAddress, data));
  EXPECT_EQ(30u, MaxEncodedFrameSize(kEscapeAddress, data));
}

TEST(EncodedSize, MaxPayload_UndersizedFrame) {
  EXPECT_EQ(MaxSafePayloadSize(4), 0u);
}

TEST(EncodedSize, MaxPayload_SmallFrame) {
  EXPECT_EQ(MaxSafePayloadSize(128), 48u);
}

TEST(EncodedSize, MaxPayload_MediumFrame) {
  EXPECT_EQ(MaxSafePayloadSize(512), 240u);
}

TEST(EncodedSize, FrameToPayloadInversion_Odd) {
  static constexpr size_t kIntendedPayloadSize = 1234567891;
  EXPECT_EQ(MaxSafePayloadSize(MaxEncodedFrameSize(kIntendedPayloadSize)),
            kIntendedPayloadSize);
}

TEST(EncodedSize, PayloadToFrameInversion_Odd) {
  static constexpr size_t kIntendedFrameSize = 1234567891;
  EXPECT_EQ(MaxEncodedFrameSize(MaxSafePayloadSize(kIntendedFrameSize)),
            kIntendedFrameSize);
}

TEST(EncodedSize, FrameToPayloadInversion_Even) {
  static constexpr size_t kIntendedPayloadSize = 42;
  EXPECT_EQ(MaxSafePayloadSize(MaxEncodedFrameSize(kIntendedPayloadSize)),
            kIntendedPayloadSize);
}

TEST(EncodedSize, PayloadToFrameInversion_Even) {
  static constexpr size_t kIntendedFrameSize = 42;
  // Because of HDLC encoding overhead requirements, the last byte of the
  // intended frame size is wasted because it doesn't allow sufficient space for
  // another byte since said additional byte could require escaping, therefore
  // requiring a second byte to increase the safe payload size by one.
  const size_t max_frame_usage =
      MaxEncodedFrameSize(MaxSafePayloadSize(kIntendedFrameSize));
  EXPECT_EQ(max_frame_usage, kIntendedFrameSize - 1);

  // There's no further change if the inversion is done again since the frame
  // size is aligned to the reduced bounds.
  EXPECT_EQ(MaxEncodedFrameSize(MaxSafePayloadSize(max_frame_usage)),
            kIntendedFrameSize - 1);
}

TEST(EncodedSize, MostlyEscaped) {
  constexpr auto kMostlyEscapedPayload =
      bytes::String(":)\x7e\x7e\x7e\x7e\x7e\x7e\x7e\x7e");
  constexpr size_t kUnescapedBytes =
      2 * kMostlyEscapedPayload.size() - EscapedSize(kMostlyEscapedPayload);
  // Subtracting 2 should still leave enough space since two bytes won't need
  // to be escaped.
  constexpr size_t kExpectedMaxFrameSize =
      MaxEncodedFrameSize(kMostlyEscapedPayload.size()) - kUnescapedBytes;
  std::array<std::byte, kExpectedMaxFrameSize> dest_buffer;
  stream::MemoryWriter writer(dest_buffer);
  EXPECT_EQ(kUnescapedBytes, 2u);
  EXPECT_EQ(OkStatus(),
            WriteUIFrame(kWidestAddress, kFullyEscapedPayload, writer));
  EXPECT_EQ(writer.size(),
            kExpectedMaxFrameSize - kTestLimitationsOverhead - kUnescapedBytes);
}

TEST(EncodedSize, BigAddress_SaturatedPayload) {
  constexpr size_t kExpectedMaxFrameSize =
      MaxEncodedFrameSize(kFullyEscapedPayload.size());
  std::array<std::byte, kExpectedMaxFrameSize> dest_buffer;
  stream::MemoryWriter writer(dest_buffer);
  EXPECT_EQ(OkStatus(),
            WriteUIFrame(kWidestAddress, kFullyEscapedPayload, writer));
  EXPECT_EQ(writer.size(), kExpectedMaxFrameSize - kTestLimitationsOverhead);
}

TEST(EncodedSize, BigAddress_OffByOne) {
  constexpr size_t kExpectedMaxFrameSize =
      MaxEncodedFrameSize(kFullyEscapedPayload.size()) - 1;
  std::array<std::byte, kExpectedMaxFrameSize> dest_buffer;
  stream::MemoryWriter writer(dest_buffer);
  EXPECT_EQ(Status::ResourceExhausted(),
            WriteUIFrame(kWidestAddress, kFullyEscapedPayload, writer));
}

TEST(EncodedSize, SmallAddress_SaturatedPayload) {
  constexpr auto kSmallerEscapedAddress = bytes::String("\x7e\x7d");
  // varint::Decode() is not constexpr, so this is a hard-coded and then runtime
  // validated.
  constexpr size_t kVarintDecodedAddress = 7999;
  constexpr size_t kExpectedMaxFrameSize =
      MaxEncodedFrameSize(kVarintDecodedAddress, kFullyEscapedPayload);
  std::array<std::byte, kExpectedMaxFrameSize> dest_buffer;
  stream::MemoryWriter writer(dest_buffer);

  uint64_t address = 0;
  size_t address_size =
      varint::Decode(kSmallerEscapedAddress, &address, kAddressFormat);
  EXPECT_EQ(address, kVarintDecodedAddress);
  EXPECT_EQ(address_size, 2u);

  EXPECT_EQ(OkStatus(), WriteUIFrame(address, kFullyEscapedPayload, writer));
  EXPECT_EQ(writer.size(), kExpectedMaxFrameSize - kTestLimitationsOverhead);
}

TEST(EncodedSize, SmallAddress_OffByOne) {
  constexpr auto kSmallerEscapedAddress = bytes::String("\x7e\x7d");
  // varint::Decode() is not constexpr, so this is a hard-coded and then runtime
  // validated.
  constexpr size_t kVarintDecodedAddress = 7999;
  constexpr size_t kExpectedMaxFrameSize =
      MaxEncodedFrameSize(kVarintDecodedAddress, kFullyEscapedPayload);
  std::array<std::byte, kExpectedMaxFrameSize - 1> dest_buffer;
  stream::MemoryWriter writer(dest_buffer);

  uint64_t address = 0;
  size_t address_size =
      varint::Decode(kSmallerEscapedAddress, &address, kAddressFormat);
  EXPECT_EQ(address, kVarintDecodedAddress);
  EXPECT_EQ(address_size, 2u);

  EXPECT_EQ(Status::ResourceExhausted(),
            WriteUIFrame(address, kFullyEscapedPayload, writer));
}

TEST(DecodedSize, BigAddress_SaturatedPayload) {
  constexpr auto kNoEscapePayload =
      bytes::String("The decoder needs the most space when there's no escapes");
  constexpr size_t kExpectedMaxFrameSize =
      MaxEncodedFrameSize(kNoEscapePayload.size());
  std::array<std::byte, kExpectedMaxFrameSize> dest_buffer;
  stream::MemoryWriter writer(dest_buffer);
  EXPECT_EQ(OkStatus(),
            WriteUIFrame(kNoEscapeAddress, kNoEscapePayload, writer));

  // Allocate at least enough real buffer space.
  constexpr size_t kDecoderBufferSize =
      Decoder::RequiredBufferSizeForFrameSize(kExpectedMaxFrameSize);
  std::array<std::byte, kDecoderBufferSize> buffer;

  // Pretend the supported frame size is whatever the final size of the encoded
  // frame was.
  const size_t max_frame_size =
      Decoder::RequiredBufferSizeForFrameSize(writer.size());

  Decoder decoder(ByteSpan(buffer).first(max_frame_size));
  for (const std::byte b : writer.WrittenData()) {
    Result<Frame> frame = decoder.Process(b);
    if (frame.ok()) {
      EXPECT_EQ(frame->address(), kNoEscapeAddress);
      EXPECT_EQ(frame->data().size(), kNoEscapePayload.size());
      EXPECT_TRUE(std::memcmp(frame->data().data(),
                              kNoEscapePayload.begin(),
                              kNoEscapePayload.size()) == 0);
    }
  }
}

TEST(DecodedSize, BigAddress_OffByOne) {
  constexpr auto kNoEscapePayload =
      bytes::String("The decoder needs the most space when there's no escapes");
  constexpr size_t kExpectedMaxFrameSize =
      MaxEncodedFrameSize(kNoEscapePayload.size());
  std::array<std::byte, kExpectedMaxFrameSize> dest_buffer;
  stream::MemoryWriter writer(dest_buffer);
  EXPECT_EQ(OkStatus(),
            WriteUIFrame(kNoEscapeAddress, kNoEscapePayload, writer));

  // Allocate at least enough real buffer space.
  constexpr size_t kDecoderBufferSize =
      Decoder::RequiredBufferSizeForFrameSize(kExpectedMaxFrameSize);
  std::array<std::byte, kDecoderBufferSize> buffer;

  // Pretend the supported frame size is whatever the final size of the encoded
  // frame was.
  const size_t max_frame_size =
      Decoder::RequiredBufferSizeForFrameSize(writer.size());

  Decoder decoder(ByteSpan(buffer).first(max_frame_size - 1));
  for (size_t i = 0; i < writer.size(); i++) {
    Result<Frame> frame = decoder.Process(writer[i]);
    if (i < writer.size() - 1) {
      EXPECT_EQ(frame.status(), Status::Unavailable());
    } else {
      EXPECT_EQ(frame.status(), Status::ResourceExhausted());
    }
  }
}

}  // namespace
}  // namespace pw::hdlc
