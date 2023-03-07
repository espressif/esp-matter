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

#include "gtest/gtest.h"
#include "pw_protobuf/encoder.h"
#include "pw_snapshot_protos/snapshot.pwpb.h"
#include "pw_span/span.h"

namespace pw::snapshot {
namespace {

TEST(Status, CompileTest) {
  constexpr size_t kMaxProtoSize = 256;
  std::byte encode_buffer[kMaxProtoSize];
  std::byte submessage_buffer[kMaxProtoSize];

  stream::MemoryWriter writer(encode_buffer);
  pwpb::Snapshot::StreamEncoder snapshot_encoder(writer, submessage_buffer);
  {
    pwpb::Metadata::StreamEncoder metadata_encoder =
        snapshot_encoder.GetMetadataEncoder();
    ASSERT_EQ(OkStatus(),
              metadata_encoder.WriteReason(
                  as_bytes(span("It just died, I didn't do anything"))));
    ASSERT_EQ(OkStatus(), metadata_encoder.WriteFatal(true));
    ASSERT_EQ(OkStatus(),
              metadata_encoder.WriteProjectName(as_bytes(span("smart-shoe"))));
    ASSERT_EQ(
        OkStatus(),
        metadata_encoder.WriteDeviceName(as_bytes(span("smart-shoe-p1"))));
  }
  ASSERT_TRUE(snapshot_encoder.status().ok());
}

}  // namespace
}  // namespace pw::snapshot
