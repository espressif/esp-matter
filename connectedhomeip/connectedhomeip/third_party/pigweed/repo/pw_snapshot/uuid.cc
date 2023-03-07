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

#include <cstddef>

#include "pw_bytes/span.h"
#include "pw_protobuf/decoder.h"
#include "pw_result/result.h"
#include "pw_snapshot_metadata_proto/snapshot_metadata.pwpb.h"
#include "pw_span/span.h"
#include "pw_status/try.h"

namespace pw::snapshot {

using protobuf::Decoder;

Result<ConstByteSpan> ReadUuidFromSnapshot(ConstByteSpan snapshot,
                                           UuidSpan output) {
  Decoder decoder(snapshot);
  ConstByteSpan metadata;
  while (decoder.Next().ok()) {
    if (decoder.FieldNumber() ==
        static_cast<uint32_t>(
            pw::snapshot::pwpb::SnapshotBasicInfo::Fields::METADATA)) {
      PW_TRY(decoder.ReadBytes(&metadata));
      break;
    }
  }
  if (metadata.empty()) {
    return Status::NotFound();
  }

  // Start to read from the metadata.
  decoder.Reset(metadata);
  ConstByteSpan snapshot_uuid;
  while (decoder.Next().ok()) {
    if (decoder.FieldNumber() ==
        static_cast<uint32_t>(
            pw::snapshot::pwpb::Metadata::Fields::SNAPSHOT_UUID)) {
      PW_TRY(decoder.ReadBytes(&snapshot_uuid));
      break;
    }
  }
  if (snapshot_uuid.empty()) {
    return Status::NotFound();
  }
  if (snapshot_uuid.size_bytes() > output.size_bytes()) {
    return Status::ResourceExhausted();
  }

  memcpy(output.data(), snapshot_uuid.data(), snapshot_uuid.size_bytes());
  return ConstByteSpan(output.first(snapshot_uuid.size_bytes()));
}

}  // namespace pw::snapshot
