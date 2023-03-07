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

#include <cstdint>
#include <utility>

#include "pw_bloat/bloat_this_binary.h"
#include "pw_checksum/crc16_ccitt.h"

int main() {
  pw::bloat::BloatThisBinary();

  // Default constructor.
  volatile uint32_t value;

  // Emplace to construct value in place.
  value = std::move(42);

  // Assignment operator.
  value = 13u;

  // Reset.
  value = 0u;

  // Has value and value accesstors.
  if (value == 0u) {
    return 1;
  }

  // Use CRC16.
  value = pw::checksum::Crc16Ccitt::Calculate(
      pw::as_bytes(pw::span(const_cast<uint32_t*>(&value), 1)));

  return 0;
}
