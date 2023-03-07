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

#include <cstring>

#include "pw_bytes/array.h"
#include "pw_log/log.h"
#include "pw_preprocessor/compiler.h"
#include "pw_result/result.h"
#include "pw_span/span.h"

namespace {

// clang-format off
constexpr auto kArray = pw::bytes::Array<
    0x0a, 0x14, 0x00, 0x00, 0x00, 0x00, 0x32, 0x00,
    0x00, 0x00, 0x64, 0x00, 0x00, 0x00, 0x96, 0x00,
    0x00, 0x00, 0xc8, 0x00, 0x00, 0x00, 0x12, 0x08,
    0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01>();
// clang-format on

PW_NO_INLINE pw::Result<pw::span<const std::byte>> Read(size_t offset,
                                                        size_t size) {
  if (offset + size >= std::size(kArray)) {
    return pw::Status::OutOfRange();
  }

  return pw::span<const std::byte>(std::data(kArray) + offset, size);
}

}  // namespace

size_t volatile* unoptimizable;

int main() {
  pw::Result result = Read(*unoptimizable, *unoptimizable);
  if (!result.ok()) {
    return 1;
  }

  PW_LOG_INFO("Read %u bytes", static_cast<unsigned>(result.value().size()));
  return 0;
}
