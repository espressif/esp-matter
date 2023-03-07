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

#include "pw_snapshot/uuid.h"

#include <array>

#include "gtest/gtest.h"
#include "pw_bytes/span.h"
#include "pw_protobuf/encoder.h"
#include "pw_result/result.h"
#include "pw_snapshot_metadata_proto/snapshot_metadata.pwpb.h"
#include "pw_span/span.h"
#include "pw_status/status.h"

namespace pw::snapshot {
namespace {

ConstByteSpan EncodeSnapshotWithUuid(ConstByteSpan uuid, ByteSpan dest) {
  pwpb::SnapshotBasicInfo::MemoryEncoder snapshot_encoder(dest);
  {
    pwpb::Metadata::StreamEncoder metadata_encoder =
        snapshot_encoder.GetMetadataEncoder();
    EXPECT_EQ(OkStatus(), metadata_encoder.WriteSnapshotUuid(uuid));
  }
  EXPECT_EQ(OkStatus(), snapshot_encoder.status());

  return snapshot_encoder;
}

TEST(ReadUuid, ReadUuid) {
  const std::array<uint8_t, 8> kExpectedUuid = {
      0x1F, 0x8F, 0xBF, 0xC4, 0x86, 0x0E, 0xED, 0xD4};
  std::array<std::byte, 16> snapshot_buffer;
  ConstByteSpan snapshot =
      EncodeSnapshotWithUuid(as_bytes(span(kExpectedUuid)), snapshot_buffer);

  std::array<std::byte, kUuidSizeBytes> uuid_dest;
  Result<ConstByteSpan> result = ReadUuidFromSnapshot(snapshot, uuid_dest);
  EXPECT_EQ(OkStatus(), result.status());
  EXPECT_EQ(kExpectedUuid.size(), result->size());
  EXPECT_EQ(0, memcmp(result->data(), kExpectedUuid.data(), result->size()));
}

TEST(ReadUuid, NoUuid) {
  std::array<std::byte, 16> snapshot_buffer;

  // Write some snapshot metadata, but no UUID.
  pwpb::SnapshotBasicInfo::MemoryEncoder snapshot_encoder(snapshot_buffer);
  {
    pwpb::Metadata::StreamEncoder metadata_encoder =
        snapshot_encoder.GetMetadataEncoder();
    EXPECT_EQ(OkStatus(), metadata_encoder.WriteFatal(true));
  }
  EXPECT_EQ(OkStatus(), snapshot_encoder.status());

  ConstByteSpan snapshot(snapshot_encoder);
  std::array<std::byte, kUuidSizeBytes> uuid_dest;
  Result<ConstByteSpan> result = ReadUuidFromSnapshot(snapshot, uuid_dest);
  EXPECT_EQ(Status::NotFound(), result.status());
}

TEST(ReadUuid, UndersizedBuffer) {
  const std::array<uint8_t, 17> kExpectedUuid = {0xF4,
                                                 0x1B,
                                                 0xE1,
                                                 0x2D,
                                                 0x10,
                                                 0x9B,
                                                 0xB2,
                                                 0x1A,
                                                 0x88,
                                                 0xE0,
                                                 0xC4,
                                                 0x77,
                                                 0xCA,
                                                 0x18,
                                                 0x83,
                                                 0xB5,
                                                 0xBB};
  std::array<std::byte, 32> snapshot_buffer;
  ConstByteSpan snapshot =
      EncodeSnapshotWithUuid(as_bytes(span(kExpectedUuid)), snapshot_buffer);

  std::array<std::byte, kUuidSizeBytes> uuid_dest;
  Result<ConstByteSpan> result = ReadUuidFromSnapshot(snapshot, uuid_dest);
  EXPECT_EQ(Status::ResourceExhausted(), result.status());
}

}  // namespace
}  // namespace pw::snapshot
