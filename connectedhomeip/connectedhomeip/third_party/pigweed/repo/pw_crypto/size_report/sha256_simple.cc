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

#include <cstring>

#include "pw_bloat/bloat_this_binary.h"
#include "pw_crypto/sha256.h"

namespace {
#define MESSAGE "Hello, Pigweed!"
#define STR_TO_BYTES(s) pw::as_bytes(pw::span(s, std::strlen(s)))
}  // namespace

int main() {
  pw::bloat::BloatThisBinary();

  std::byte digest[32];

  return pw::crypto::sha256::Hash(STR_TO_BYTES(MESSAGE), digest).ok() ? 0 : 1;
}
