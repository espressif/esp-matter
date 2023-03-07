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

#include "pw_bytes/span.h"
#include "pw_result/result.h"
#include "pw_span/span.h"

namespace pw::snapshot {

// Snapshot UUIDs are expected to be 128-bit.
//
// Note this is not strictly enforced anywhere, this is pure for convenience.
inline constexpr size_t kUuidSizeBytes = 16;

using UuidSpan = span<std::byte, kUuidSizeBytes>;
using ConstUuidSpan = span<const std::byte, kUuidSizeBytes>;

// Reads the snapshot UUID from an in memory snapshot, if present, and returns
// the subspan of `output` that contains the read snapshot.
//
// Returns:
//   OK - UUID found, status with size indicates number of bytes written to
//     `output`.
//   RESOURCE_EXHUASTED - UUID found, but `output` was too small to fit it.
//   NOT_FOUND - No snapshot UUID found in the provided snapshot.
Result<ConstByteSpan> ReadUuidFromSnapshot(ConstByteSpan snapshot,
                                           UuidSpan output);

}  // namespace pw::snapshot
