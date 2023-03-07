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

#include <cstdint>
#include <optional>

#include "pw_bytes/span.h"
#include "pw_span/span.h"

namespace pw::router {

// A PacketParser is an abstract interface for extracting data from different
// kinds of transport layer packets or frames. It is used by routers to examine
// fields within packets to know how to route them.
class PacketParser {
 public:
  virtual ~PacketParser() = default;

  // Parses a packet, storing its data for subsequent calls to Get* functions.
  // Any currently stored packet is cleared. Returns true if successful, or
  // false if the packet is incomplete or corrupt.
  //
  // The raw binary data passed to this function is guaranteed to remain valid
  // through all subsequent Get* calls made for the packet's information, so
  // implementations may store and use it directly.
  virtual bool Parse(ConstByteSpan packet) = 0;

  // Extracts the destination address the last parsed packet, if it exists.
  //
  // Guaranteed to only be called if Parse() succeeded and while the data passed
  // to Parse() is valid.
  virtual std::optional<uint32_t> GetDestinationAddress() const = 0;
};

}  // namespace pw::router
