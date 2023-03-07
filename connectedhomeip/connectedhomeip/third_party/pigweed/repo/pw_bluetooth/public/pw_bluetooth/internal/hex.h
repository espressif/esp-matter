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

#include <cstdint>

namespace pw::bluetooth::internal {

// Parse a hexadecimal character to a 0-15 number. If the hex char is invalid
// returns 0x100.
constexpr uint16_t HexToNibble(char hex) {
  if (hex >= '0' && hex <= '9') {
    return hex - '0';
  }
  if (hex >= 'A' && hex <= 'F') {
    return hex - 'A' + 10;
  }
  if (hex >= 'a' && hex <= 'f') {
    return hex - 'a' + 10;
  }
  return 0x100;
}

// Convert a nibble (0 to 15 value) to its lowercase hexadecimal representation.
constexpr char NibbleToHex(uint8_t value) {
  if (value < 10) {
    return '0' + value;
  }
  if (value < 16) {
    return 'a' + value - 10;
  }
  return '?';
}

}  // namespace pw::bluetooth::internal
