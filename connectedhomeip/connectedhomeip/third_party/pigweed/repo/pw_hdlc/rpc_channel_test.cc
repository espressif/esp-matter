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

#include "pw_hdlc/rpc_channel.h"

#include <algorithm>
#include <array>
#include <cstddef>

#include "gtest/gtest.h"
#include "pw_bytes/array.h"
#include "pw_hdlc/encoded_size.h"
#include "pw_stream/memory_stream.h"

using std::byte;

namespace pw::hdlc {
namespace {

constexpr byte kFlag = byte{0x7E};
constexpr uint8_t kAddress = 0x7b;  // 123
constexpr uint8_t kEncodedAddress = (kAddress << 1) | 1;
constexpr byte kControl = byte{0x3};  // UI-frame control sequence.

// Size of the in-memory buffer to use for this test. All tests send a one-byte
// payload.
constexpr size_t kSinkBufferSize = MaxEncodedFrameSize(1);

TEST(RpcChannelOutput, 1BytePayload) {
  stream::MemoryWriterBuffer<kSinkBufferSize> memory_writer;

  RpcChannelOutput output(memory_writer, kAddress, "RpcChannelOutput");

  constexpr byte test_data = byte{'A'};
  std::array<std::byte, 128> buffer;
  std::memcpy(buffer.data(), &test_data, sizeof(test_data));

  constexpr auto expected = bytes::Concat(
      kFlag, kEncodedAddress, kControl, 'A', uint32_t{0x653c9e82}, kFlag);

  EXPECT_EQ(OkStatus(), output.Send(span(buffer).first(sizeof(test_data))));

  ASSERT_EQ(memory_writer.bytes_written(), expected.size());
  EXPECT_EQ(
      std::memcmp(
          memory_writer.data(), expected.data(), memory_writer.bytes_written()),
      0);
}

TEST(RpcChannelOutput, EscapingPayloadTest) {
  stream::MemoryWriterBuffer<kSinkBufferSize> memory_writer;

  RpcChannelOutput output(memory_writer, kAddress, "RpcChannelOutput");

  constexpr auto test_data = bytes::Array<0x7D>();
  std::array<std::byte, 128> buffer;
  std::memcpy(buffer.data(), test_data.data(), test_data.size());

  constexpr auto expected = bytes::Concat(kFlag,
                                          kEncodedAddress,
                                          kControl,
                                          byte{0x7d},
                                          byte{0x7d} ^ byte{0x20},
                                          uint32_t{0x4a53e205},
                                          kFlag);
  EXPECT_EQ(OkStatus(), output.Send(span(buffer).first(test_data.size())));

  ASSERT_EQ(memory_writer.bytes_written(), 10u);
  EXPECT_EQ(
      std::memcmp(
          memory_writer.data(), expected.data(), memory_writer.bytes_written()),
      0);
}

TEST(FixedMtuChannelOutput, CompileTest) {
  constexpr size_t kRequiredMtu =
      MaxEncodedFrameSize(rpc::cfg::kEncodingBufferSizeBytes);
  stream::MemoryWriterBuffer<kRequiredMtu> memory_writer;
  [[maybe_unused]] FixedMtuChannelOutput<kRequiredMtu> channel_output(
      memory_writer, kAddress, "RpcChannelOutput");
  EXPECT_EQ(channel_output.MaxSafePayloadSize(), rpc::MaxSafePayloadSize());
}

TEST(RpcChannelOutputBuffer, 1BytePayload) {
  stream::MemoryWriterBuffer<kSinkBufferSize> memory_writer;

  RpcChannelOutput output(memory_writer, kAddress, "RpcChannelOutput");

  constexpr byte test_data = byte{'A'};
  std::array<std::byte, 128> buffer;
  std::memcpy(buffer.data(), &test_data, sizeof(test_data));

  constexpr auto expected = bytes::Concat(
      kFlag, kEncodedAddress, kControl, 'A', uint32_t{0x653c9e82}, kFlag);

  EXPECT_EQ(OkStatus(), output.Send(span(buffer).first(sizeof(test_data))));

  ASSERT_EQ(memory_writer.bytes_written(), expected.size());
  EXPECT_EQ(
      std::memcmp(
          memory_writer.data(), expected.data(), memory_writer.bytes_written()),
      0);
}

TEST(RpcChannelOutputBuffer, MultibyteAddress) {
  stream::MemoryWriterBuffer<kSinkBufferSize> memory_writer;

  RpcChannelOutput output(memory_writer, 0x3fff, "RpcChannelOutput");

  constexpr byte test_data = byte{'A'};
  std::array<std::byte, 128> buffer;
  std::memcpy(buffer.data(), &test_data, sizeof(test_data));

  constexpr auto expected = bytes::Concat(kFlag,
                                          bytes::String("\xfe\xff"),
                                          kControl,
                                          'A',
                                          uint32_t{0xd393a8a0},
                                          kFlag);

  EXPECT_EQ(OkStatus(), output.Send(span(buffer).first(sizeof(test_data))));

  ASSERT_EQ(memory_writer.bytes_written(), expected.size());
  EXPECT_EQ(
      std::memcmp(
          memory_writer.data(), expected.data(), memory_writer.bytes_written()),
      0);
}

}  // namespace
}  // namespace pw::hdlc
