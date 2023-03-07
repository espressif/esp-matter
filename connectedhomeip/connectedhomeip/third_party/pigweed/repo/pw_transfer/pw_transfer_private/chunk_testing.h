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

#include "pw_bytes/span.h"
#include "pw_containers/vector.h"
#include "pw_transfer/internal/chunk.h"

namespace pw::transfer::test {

Vector<std::byte, 64> EncodeChunk(const internal::Chunk& chunk) {
  Vector<std::byte, 64> buffer(64);
  auto result = chunk.Encode(buffer);
  EXPECT_EQ(result.status(), OkStatus());
  buffer.resize(result.value().size());
  return buffer;
}

internal::Chunk DecodeChunk(ConstByteSpan buffer) {
  auto result = internal::Chunk::Parse(buffer);
  EXPECT_EQ(result.status(), OkStatus());
  return *result;
}

}  // namespace pw::transfer::test
