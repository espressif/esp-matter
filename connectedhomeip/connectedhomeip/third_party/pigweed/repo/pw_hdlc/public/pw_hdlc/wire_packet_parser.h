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

#include "pw_router/packet_parser.h"

namespace pw::hdlc {

// HDLC frame parser for routers that operates on wire-encoded frames.
//
// This allows routing HDLC frames through Pigweed routers without having to
// first decode them from their wire format.
class WirePacketParser : public router::PacketParser {
 public:
  constexpr WirePacketParser() : address_(0) {}

  // Verifies and parses an HDLC frame. Packet passed in is expected to be a
  // single, complete, wire-encoded frame, starting and ending with a flag.
  bool Parse(ConstByteSpan packet) final;

  std::optional<uint32_t> GetDestinationAddress() const override {
    return address_;
  }

 protected:
  constexpr uint64_t address() const { return address_; }

 private:
  uint64_t address_;
};

}  // namespace pw::hdlc
