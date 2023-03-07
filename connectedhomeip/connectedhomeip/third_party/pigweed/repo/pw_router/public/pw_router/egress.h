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

#include <optional>

#include "pw_bytes/span.h"
#include "pw_router/packet_parser.h"
#include "pw_span/span.h"
#include "pw_status/status.h"

namespace pw::router {

// Data egress for a router to send packets over some transport system.
class Egress {
 public:
  virtual ~Egress() = default;

  // Sends a complete packet/frame over the transport. Returns OK on success, or
  // an error status on failure. Invoked with the packet data and the parser
  // from the RoutePacket() call.
  //
  // TODO(frolv): Document possible return values.
  virtual Status SendPacket(ConstByteSpan packet,
                            const PacketParser& parser) = 0;
};

}  // namespace pw::router
