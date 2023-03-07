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

#include "pw_bytes/span.h"
#include "pw_rpc/internal/method_union.h"
#include "pw_rpc/raw/internal/method.h"

namespace pw::rpc::internal {

// MethodUnion which stores only a raw method. For use in fully raw RPC
// services, without any additional memory overhead.
class RawMethodUnion : public MethodUnion {
 public:
  constexpr RawMethodUnion(RawMethod&& method)
      : impl_({.raw = std::move(method)}) {}

  constexpr const Method& method() const { return impl_.method; }
  constexpr const RawMethod& raw_method() const { return impl_.raw; }

 private:
  union {
    Method method;
    RawMethod raw;
  } impl_;
};

// Deduces the type of an implemented service method from its signature, and
// returns the appropriate MethodUnion object to invoke it.
template <auto kMethod, MethodType kType>
constexpr RawMethod GetRawMethodFor(uint32_t id) {
  if constexpr (RawMethod::matches<kMethod>()) {
    return GetMethodFor<kMethod, RawMethod, kType>(id);
  } else {
    return InvalidMethod<kMethod, kType, RawMethod>(id);
  }
}

}  // namespace pw::rpc::internal
