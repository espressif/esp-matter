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

#include "pw_varint/varint.h"

namespace pw::hdlc {

inline constexpr std::byte kFlag = std::byte{0x7E};
inline constexpr std::byte kEscape = std::byte{0x7D};
inline constexpr std::byte kEscapeConstant = std::byte{0x20};

inline constexpr std::array<std::byte, 2> kEscapedFlag = {kEscape,
                                                          std::byte{0x5E}};
inline constexpr std::array<std::byte, 2> kEscapedEscape = {kEscape,
                                                            std::byte{0x5D}};

// This implementation also only supports addresses up to the maximum value of a
// 64-bit unsigned integer.
inline constexpr size_t kMinAddressSize = 1;
inline constexpr size_t kMaxAddressSize =
    varint::EncodedSize(std::numeric_limits<uint64_t>::max());

// The biggest on-the-wire size of a FCS after HDLC escaping. If, for example,
// the FCS is 0x7e7e7e7e the encoded value will be:
//   [ 0x7e, 0x5e, 0x7e, 0x5e, 0x7e, 0x5e, 0x7e, 0x5e ]
inline constexpr size_t kMaxEscapedFcsSize = 8;

// The biggest on-the-wire size of an HDLC address after escaping. This number
// is not just kMaxAddressSize * 2 because the kFlag and kEscape bytes are not
// valid terminating bytes of a one-terminated least significant varint. This
// means that the practical biggest varint BEFORE escaping looks like this:
//   [ 0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 0x03 ]
// The first nine bytes will be escaped, but the tenth never will be escaped
// since any values other than 0x03 would cause a uint64_t to overflow.
inline constexpr size_t kMaxEscapedVarintAddressSize =
    (kMaxAddressSize * 2) - 1;

// This implementation does not support extended control fields.
inline constexpr size_t kControlSize = 1;

// The control byte may need to be escaped. When it is, this is its maximum
// size.
inline constexpr size_t kMaxEscapedControlSize = 2;

// This implementation only supports a 32-bit CRC-32 Frame Check Sequence (FCS).
inline constexpr size_t kFcsSize = sizeof(uint32_t);

inline constexpr varint::Format kAddressFormat =
    varint::Format::kOneTerminatedLeastSignificant;

constexpr bool NeedsEscaping(std::byte b) {
  return (b == kFlag || b == kEscape);
}

constexpr std::byte Escape(std::byte b) { return b ^ kEscapeConstant; }

// Class that manages the 1-byte control field of an HDLC U-frame.
class UFrameControl {
 public:
  static constexpr UFrameControl UnnumberedInformation() {
    return UFrameControl(kUnnumberedInformation);
  }

  constexpr std::byte data() const { return data_; }

 private:
  // Types of HDLC U-frames and their bit patterns.
  enum Type : uint8_t {
    kUnnumberedInformation = 0x00,
  };

  constexpr UFrameControl(Type type)
      : data_(kUFramePattern | std::byte{type}) {}

  // U-frames are identified by having the bottom two control bits set.
  static constexpr std::byte kUFramePattern = std::byte{0x03};

  std::byte data_;
};

}  // namespace pw::hdlc
