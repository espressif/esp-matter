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

namespace pw::rpc {

enum class MethodType : unsigned char {
  kUnary = 0b00,
  kServerStreaming = 0b01,
  kClientStreaming = 0b10,
  kBidirectionalStreaming = 0b11,
};

// True for unary and server streaming RPCs.
constexpr bool HasServerStream(MethodType type) {
  return (static_cast<unsigned>(type) &
          static_cast<unsigned>(MethodType::kServerStreaming)) != 0;
}

// True for client and bidirectional streaming RPCs.
constexpr bool HasClientStream(MethodType type) {
  return (static_cast<unsigned>(type) &
          static_cast<unsigned>(MethodType::kClientStreaming)) != 0;
}

}  // namespace pw::rpc
