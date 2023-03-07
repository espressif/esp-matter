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
#pragma once

#include <cstdint>

namespace pw::transfer {

enum class ProtocolVersion {
  // Protocol version not known or not set.
  kUnknown,

  // The original transfer protocol, prior to transfer start/end handshakes.
  kLegacy,

  // Second version of the transfer protocol. Guarantees type fields on all
  // chunks, deprecates pending_bytes in favor of window_end_offset, splits
  // transfer resource IDs from ephemeral session IDs, and adds a handshake
  // to the start and end of all transfer sessions.
  kVersionTwo,

  // Alias to the most up-to-date version of the transfer protocol.
  kLatest = kVersionTwo,
};

constexpr bool ValidProtocolVersion(ProtocolVersion version) {
  return version > ProtocolVersion::kUnknown &&
         version <= ProtocolVersion::kLatest;
}

constexpr bool ValidProtocolVersion(uint32_t version) {
  return ValidProtocolVersion(static_cast<ProtocolVersion>(version));
}

}  // namespace pw::transfer
