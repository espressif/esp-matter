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

#include "pw_rpc/service.h"

#include <cstddef>
#include <type_traits>

namespace pw::rpc {

const internal::Method* Service::FindMethod(uint32_t method_id) const {
  const internal::MethodUnion* method_impl = methods_;

  for (size_t i = 0; i < method_count_; ++i) {
    const internal::Method* method = &method_impl->method();
    if (method->id() == method_id) {
      return method;
    }

    const auto raw = reinterpret_cast<const std::byte*>(method_impl);
    method_impl =
        reinterpret_cast<const internal::MethodUnion*>(raw + method_size_);
  }

  return nullptr;
}

}  // namespace pw::rpc
