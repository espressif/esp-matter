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

#include <string_view>

#include "pw_preprocessor/compiler.h"

namespace pw::rpc::internal {

// This is the hash function pw_rpc uses internally to calculate IDs from
// service and method names.
//
// This is the same hash function that is used in pw_tokenizer, with the maximum
// length removed. It is chosen due to its simplicity. The tokenizer code is
// duplicated here to avoid unnecessary dependencies between modules.
constexpr uint32_t Hash(std::string_view string)
    PW_NO_SANITIZE("unsigned-integer-overflow") {
  constexpr uint32_t kHashConstant = 65599;

  // The length is hashed as if it were the first character.
  uint32_t hash = string.size();
  uint32_t coefficient = kHashConstant;

  // Hash all of the characters in the string as unsigned ints.
  // The coefficient calculation is done modulo 0x100000000, so the unsigned
  // integer overflows are intentional.
  for (uint8_t ch : string) {
    hash += coefficient * ch;
    coefficient *= kHashConstant;
  }

  return hash;
}

}  // namespace pw::rpc::internal
