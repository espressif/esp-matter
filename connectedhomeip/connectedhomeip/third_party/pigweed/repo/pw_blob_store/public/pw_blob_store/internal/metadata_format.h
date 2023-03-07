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
#pragma once

#include <cstddef>
#include <cstdint>

#include "pw_preprocessor/compiler.h"

namespace pw::blob_store::internal {

enum MetadataVersion : uint32_t {
  // Original metadata format does not include a version.
  kVersion1 = 0,
  kVersion2 = 0x1197851D,
  kLatest = kVersion2
};

// Technically the original BlobMetadataV1 was not packed.
PW_PACKED(struct) BlobMetadataV1 {
  typedef uint32_t ChecksumValue;

  // The checksum of the blob data stored in flash.
  ChecksumValue checksum;

  // Number of blob data bytes stored in flash.
  // Technically this was originally size_t, but backwards compatibility for
  // platform-specific sized types has been dropped.
  uint32_t data_size_bytes;
};

// Changes to the metadata format should also get a different key signature to
// avoid new code improperly reading old format metadata.
PW_PACKED(struct) BlobMetadataHeaderV2 {
  BlobMetadataV1 v1_metadata;

  // Metadata encoding version stored in flash.
  MetadataVersion version;

  // Length of the file name stored in the metadata entry.
  uint8_t file_name_length;

  // Following this struct is file_name_length chars of file name. Note that
  // the string of characters is NOT null terminated.

  constexpr void reset() {
    *this = {
        .v1_metadata =
            {
                .checksum = 0,
                .data_size_bytes = 0,
            },
        .version = MetadataVersion::kLatest,
        .file_name_length = 0,
    };
  }
};

using BlobMetadataHeader = BlobMetadataHeaderV2;
using ChecksumValue = BlobMetadataV1::ChecksumValue;

}  // namespace pw::blob_store::internal
