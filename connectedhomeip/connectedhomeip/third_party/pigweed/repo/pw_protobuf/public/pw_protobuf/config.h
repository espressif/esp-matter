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

// Configuration macros for the protobuf module.
#pragma once

#include <cstddef>

// When encoding nested messages, the number of bytes to reserve for the varint
// submessage length. Nested messages are limited in size to the maximum value
// that can be varint-encoded into this reserved space.
//
// The values that can be set, and their corresponding maximum submessage
// lengths, are outlined below.
//
//   1 byte  => 127
//   2 bytes => 16,383 or < 16KiB
//   3 bytes => 2,097,151 or < 2048KiB
//   4 bytes => 268,435,455 or < 256MiB
//   5 bytes => 4,294,967,295 or < 4GiB (max uint32_t)
//
#ifndef PW_PROTOBUF_CFG_MAX_VARINT_SIZE
#define PW_PROTOBUF_CFG_MAX_VARINT_SIZE 4
#endif  // PW_PROTOBUF_MAX_VARINT_SIZE

static_assert(PW_PROTOBUF_CFG_MAX_VARINT_SIZE > 0 &&
              PW_PROTOBUF_CFG_MAX_VARINT_SIZE <= 5);

namespace pw::protobuf::config {

inline constexpr size_t kMaxVarintSize = PW_PROTOBUF_CFG_MAX_VARINT_SIZE;

}  // namespace pw::protobuf::config
