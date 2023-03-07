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

#include "pw_transfer/internal/chunk.h"

#include "gtest/gtest.h"
#include "pw_bytes/array.h"

namespace pw::transfer::internal {
namespace {

TEST(Chunk, EncodedSizeMatchesEncode) {
  // Use a START chunk to encode a bunch of legacy fields as well.
  Chunk chunk(ProtocolVersion::kVersionTwo, Chunk::Type::kStart);
  chunk.set_session_id(42).set_resource_id(7).set_window_end_offset(128);

  std::array<std::byte, 64> buffer;
  EXPECT_LT(chunk.EncodedSize(), buffer.size());

  auto result = chunk.Encode(buffer);
  ASSERT_EQ(result.status(), OkStatus());
  EXPECT_EQ(chunk.EncodedSize(), result->size_bytes());
}

TEST(Chunk, EncodedSizeGreaterThanBuffer) {
  Chunk chunk(ProtocolVersion::kVersionTwo, Chunk::Type::kParametersRetransmit);
  chunk.set_session_id(42).set_resource_id(7).set_window_end_offset(128);

  std::array<std::byte, 8> buffer;
  EXPECT_GT(chunk.EncodedSize(), buffer.size());

  auto result = chunk.Encode(buffer);
  ASSERT_EQ(result.status(), Status::ResourceExhausted());
}

TEST(Chunk, EncodedSizeMatchesBuffer) {
  // 16 bytes for metadata.
  Chunk chunk(ProtocolVersion::kVersionTwo, Chunk::Type::kStart);
  chunk.set_session_id(42).set_resource_id(7).set_window_end_offset(128);
  EXPECT_EQ(chunk.EncodedSize(), 16u);

  // 2 bytes for payload key & size, leaving 46 for data.
  constexpr auto kData = bytes::Initialized<46>(0x11);
  chunk.set_payload(kData);

  std::array<std::byte, 64> buffer;
  EXPECT_EQ(chunk.EncodedSize(), buffer.size());

  auto result = chunk.Encode(buffer);
  ASSERT_EQ(result.status(), OkStatus());
  EXPECT_EQ(chunk.EncodedSize(), result->size_bytes());
}

}  // namespace
}  // namespace pw::transfer::internal
