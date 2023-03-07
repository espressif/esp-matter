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

#include <cstdint>
#include <type_traits>

namespace pw::rpc::internal {

template <auto>
constexpr std::false_type kIsGeneratedRpcFunction{};

// The MethodInfo class provides metadata about an RPC. This class is
// specialized for the static RPC client functions in the generated code. This
// makes it possible for pw_rpc to access method metadata with a simple API. The
// user passes the method name (e.g. pw_rpc::raw::MyService::MyMethod) as a
// template parameter, and pw_rpc can extract information like method and
// service IDs at compile time.
template <auto kRpcFunction>
struct MethodInfo {
  // MethodInfo specializations always provide the service and method IDs and a
  // function that returns the implementation method for this RPC given a
  // service implementation class.
  static constexpr uint32_t kServiceId = 0;
  static constexpr uint32_t kMethodId = 0;

  template <typename ServiceImpl>
  static constexpr void Function() {}

  static_assert(kIsGeneratedRpcFunction<kRpcFunction>,
                "The provided argument is not a generated pw_rpc function. "
                "Pass a pw_rpc function such as "
                "my_pkg::pw_rpc::raw:::MyService::MyMethod instead.");
};

}  // namespace pw::rpc::internal
