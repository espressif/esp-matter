// Copyright 2020 The Pigweed Authors
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

#include "pw_bytes/span.h"
#include "pw_checksum/crc32.h"
#include "pw_hdlc/internal/protocol.h"
#include "pw_status/status.h"
#include "pw_stream/stream.h"

namespace pw::hdlc::internal {

// Encodes and writes HDLC frames.
class Encoder {
 public:
  constexpr Encoder(stream::Writer& output) : writer_(output) {}

  // Writes the header for an I-frame. After successfully calling
  // StartInformationFrame, WriteData may be called any number of times.
  Status StartInformationFrame(uint64_t address) {
    return StartFrame(address, kUnusedControl);
  }

  // Writes the header for an U-frame. After successfully calling
  // StartUnnumberedFrame, WriteData may be called any number of times.
  Status StartUnnumberedFrame(uint64_t address) {
    return StartFrame(address, UFrameControl::UnnumberedInformation().data());
  }

  // Writes data for an ongoing frame. Must only be called after a successful
  // StartInformationFrame call, and prior to a FinishFrame() call.
  Status WriteData(ConstByteSpan data);

  // Finishes a frame. Writes the frame check sequence and a terminating flag.
  Status FinishFrame();

 private:
  // Indicates this an information packet with sequence numbers set to 0.
  static constexpr std::byte kUnusedControl = std::byte{0};

  Status StartFrame(uint64_t address, std::byte control);

  stream::Writer& writer_;
  checksum::Crc32 fcs_;
};

}  // namespace pw::hdlc::internal
