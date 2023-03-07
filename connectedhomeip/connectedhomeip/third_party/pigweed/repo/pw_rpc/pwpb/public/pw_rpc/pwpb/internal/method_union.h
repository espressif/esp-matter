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

#include "pw_rpc/internal/method_union.h"
#include "pw_rpc/pwpb/internal/common.h"
#include "pw_rpc/pwpb/internal/method.h"
#include "pw_rpc/raw/internal/method_union.h"

namespace pw::rpc::internal {

// MethodUnion which holds a pw_protobuf method or a raw method.
class PwpbMethodUnion : public MethodUnion {
 public:
  constexpr PwpbMethodUnion(RawMethod&& method)
      : impl_({.raw = std::move(method)}) {}
  constexpr PwpbMethodUnion(PwpbMethod&& method)
      : impl_({.pwpb = std::move(method)}) {}

  constexpr const Method& method() const { return impl_.method; }
  constexpr const RawMethod& raw_method() const { return impl_.raw; }
  constexpr const PwpbMethod& pwpb_method() const { return impl_.pwpb; }

 private:
  union {
    Method method;
    RawMethod raw;
    PwpbMethod pwpb;
  } impl_;
};

// Deduces the type of an implemented service method from its signature, and
// returns the appropriate MethodUnion object to invoke it.
template <auto kMethod, MethodType kType, typename Request, typename Response>
constexpr auto GetPwpbOrRawMethodFor(uint32_t id,
                                     const PwpbMethodSerde& serde) {
  if constexpr (RawMethod::matches<kMethod>()) {
    return GetMethodFor<kMethod, RawMethod, kType>(id);
  } else if constexpr (PwpbMethod::matches<kMethod, Request, Response>()) {
    return GetMethodFor<kMethod, PwpbMethod, kType>(id, serde);
  } else {
    return InvalidMethod<kMethod, kType, RawMethod>(id);
  }
}

}  // namespace pw::rpc::internal
