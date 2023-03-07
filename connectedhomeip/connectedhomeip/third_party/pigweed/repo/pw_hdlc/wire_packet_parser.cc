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

#include "pw_hdlc/wire_packet_parser.h"

#include "pw_bytes/endian.h"
#include "pw_checksum/crc32.h"
#include "pw_hdlc/decoder.h"
#include "pw_hdlc/internal/protocol.h"

namespace pw::hdlc {

bool WirePacketParser::Parse(ConstByteSpan packet) {
  if (packet.size_bytes() < Frame::kMinContentSizeBytes) {
    return false;
  }

  if (packet.front() != kFlag || packet.back() != kFlag) {
    return false;
  }

  // Partially decode into a buffer with space only for the address and control
  // fields of the frame. The decoder will verify the frame's FCS field.
  std::array<std::byte, 16> buffer = {};
  Decoder decoder(buffer);
  Status status = Status::Unknown();

  decoder.Process(packet, [&status](const Result<Frame>& result) {
    status = result.status();
  });

  Result<Frame> result = Frame::Parse(buffer);
  if (!result.ok()) {
    return false;
  }

  address_ = result.value().address();

  // RESOURCE_EXHAUSTED is expected as the buffer is too small for the packet.
  return status.ok() || status.IsResourceExhausted();
}

}  // namespace pw::hdlc
