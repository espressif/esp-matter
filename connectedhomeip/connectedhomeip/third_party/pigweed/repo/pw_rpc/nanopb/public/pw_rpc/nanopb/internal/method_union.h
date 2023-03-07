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
#include "pw_rpc/nanopb/internal/common.h"
#include "pw_rpc/nanopb/internal/method.h"
#include "pw_rpc/raw/internal/method_union.h"

namespace pw::rpc::internal {

// Method union which holds either a nanopb or a raw method.
class NanopbMethodUnion : public MethodUnion {
 public:
  constexpr NanopbMethodUnion(RawMethod&& method)
      : impl_({.raw = std::move(method)}) {}
  constexpr NanopbMethodUnion(NanopbMethod&& method)
      : impl_({.nanopb = std::move(method)}) {}

  constexpr const Method& method() const { return impl_.method; }
  constexpr const RawMethod& raw_method() const { return impl_.raw; }
  constexpr const NanopbMethod& nanopb_method() const { return impl_.nanopb; }

 private:
  union {
    Method method;
    RawMethod raw;
    NanopbMethod nanopb;
  } impl_;
};

// Returns either a raw or nanopb method object, depending on the implemented
// function's signature.
template <auto kMethod, MethodType kType, typename Request, typename Response>
constexpr auto GetNanopbOrRawMethodFor(
    uint32_t id, [[maybe_unused]] const NanopbMethodSerde& serde) {
  if constexpr (RawMethod::matches<kMethod>()) {
    return GetMethodFor<kMethod, RawMethod, kType>(id);
  } else if constexpr (NanopbMethod::matches<kMethod, Request, Response>()) {
    return GetMethodFor<kMethod, NanopbMethod, kType>(id, serde);
  } else {
    return InvalidMethod<kMethod, kType, RawMethod>(id);
  }
}

}  // namespace pw::rpc::internal
