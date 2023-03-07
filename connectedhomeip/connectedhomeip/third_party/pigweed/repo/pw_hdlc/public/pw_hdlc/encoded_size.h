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

#include <cstddef>
#include <cstdint>
#include <type_traits>

#include "pw_bytes/span.h"
#include "pw_hdlc/internal/protocol.h"
#include "pw_span/span.h"
#include "pw_varint/varint.h"

namespace pw::hdlc {

// Calculates the size of a series of bytes after HDLC escaping.
constexpr size_t EscapedSize(ConstByteSpan data) {
  size_t count = 0;
  for (std::byte b : data) {
    count += NeedsEscaping(b) ? 2 : 1;
  }
  return count;
}

template <typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
constexpr size_t EscapedSize(T val) {
  return EscapedSize(as_bytes(span(&val, 1)));
}

// Calculate the buffer space required for encoding an HDLC frame with a given
// payload size. This uses worst-case escaping cost as part of the calculation,
// which is extremely expensive but guarantees the payload will always fit in
// the frame AND the value can be evaluated at compile-time.
//
// This is NOT a perfect inverse of MaxSafePayloadSize()! This is because
// increasing the frame size by one doesn't mean another payload byte can safely
// fit since it might need to be escaped.
constexpr size_t MaxEncodedFrameSize(size_t max_payload_size) {
  return 2 * sizeof(kFlag) + kMaxEscapedVarintAddressSize + kMaxEscapedFcsSize +
         kMaxEscapedControlSize + max_payload_size * 2;
}

// Calculates a maximum the on-the-wire encoded size for a given payload
// destined for the provided address. Because payload escaping and some of the
// address is accounted for, there's significantly less wasted space when
// compared to MaxEncodedFrameSize(). However, because the checksum, address,
// and control fields are not precisely calculated, there's up to 17 bytes of
// potentially unused overhead left over by this estimation. This is done to
// improve the speed of this calculation, since precisely calculating all of
// this information isn't nearly as efficient.
constexpr size_t MaxEncodedFrameSize(uint64_t address, ConstByteSpan payload) {
  // The largest on-the-wire escaped varint will never exceed
  // kMaxEscapedVarintAddressSize since the 10th varint byte can never be an
  // byte that needs escaping.
  const size_t max_encoded_address_size =
      std::min(varint::EncodedSize(address) * 2, kMaxEscapedVarintAddressSize);
  return 2 * sizeof(kFlag) + max_encoded_address_size + kMaxEscapedFcsSize +
         kMaxEscapedControlSize + EscapedSize(payload);
}

// Calculates the maximum safe payload size of an HDLC-encoded frame. As long as
// a payload does not exceed this value, it will always be safe to encode it
// as an HDLC frame in a buffer of size max_frame_size.
//
// When max_frame_size is too small to safely fit any payload data, this
// function returns zero.
//
// This is NOT a perfect inverse of MaxEncodedFrameSize()! This is because
// increasing the frame size by one doesn't mean another payload byte can safely
// fit since it might need to be escaped.
constexpr size_t MaxSafePayloadSize(size_t max_frame_size) {
  constexpr size_t kMaxConstantOverhead =
      2 * sizeof(kFlag) + kMaxEscapedVarintAddressSize + kMaxEscapedFcsSize +
      kMaxEscapedControlSize;
  return max_frame_size < kMaxConstantOverhead
             ? 0
             : (max_frame_size - kMaxConstantOverhead) / 2;
}

}  // namespace pw::hdlc
