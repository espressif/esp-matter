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

#include <type_traits>

#include "pw_rpc/internal/hash.h"
#include "pw_rpc/internal/method_info.h"
#include "pw_rpc/method_info.h"

namespace pw::rpc::internal {

// Tests the MethodTraits specializations for test.proto for any implementation.
template <typename GeneratedClass, typename ServiceImpl>
class MethodInfoTests {
 public:
  constexpr bool Pass() const {
    return Ids().Pass() && MethodFunction().Pass() &&
           MethodRequestResponseTypes().Pass();
  }

 private:
  struct Ids {
    constexpr bool Pass() const { return true; }

#define PW_RPC_TEST_METHOD_INFO_IDS(function)                             \
  static_assert(MethodInfo<GeneratedClass::function>::kServiceId ==       \
                    Hash("pw.rpc.test.TestService"),                      \
                #function " service ID doesn't match!");                  \
  static_assert(                                                          \
      MethodInfo<GeneratedClass::function>::kMethodId == Hash(#function), \
      #function " method ID doesn't match!")

    PW_RPC_TEST_METHOD_INFO_IDS(TestUnaryRpc);
    PW_RPC_TEST_METHOD_INFO_IDS(TestAnotherUnaryRpc);
    PW_RPC_TEST_METHOD_INFO_IDS(TestServerStreamRpc);
    PW_RPC_TEST_METHOD_INFO_IDS(TestClientStreamRpc);
    PW_RPC_TEST_METHOD_INFO_IDS(TestBidirectionalStreamRpc);
#undef PW_RPC_TEST_METHOD_INFO_IDS
  };

  static_assert(MethodInfo<GeneratedClass::TestClientStreamRpc>::kServiceId !=
                    Hash("TestService"),
                "Wrong service name should not match");
  static_assert(
      MethodInfo<GeneratedClass::TestBidirectionalStreamRpc>::kMethodId !=
          Hash("TestUnaryRpc"),
      "Wrong method name should not match");

  struct MethodFunction {
    constexpr bool Pass() const { return true; }

#define PW_RPC_TEST_METHOD_INFO_FUNCTION(function)                       \
  static_assert(MethodInfo<GeneratedClass::function>::template Function< \
                    ServiceImpl>() == &ServiceImpl::function)

    PW_RPC_TEST_METHOD_INFO_FUNCTION(TestUnaryRpc);
    PW_RPC_TEST_METHOD_INFO_FUNCTION(TestAnotherUnaryRpc);
    PW_RPC_TEST_METHOD_INFO_FUNCTION(TestServerStreamRpc);
    PW_RPC_TEST_METHOD_INFO_FUNCTION(TestClientStreamRpc);
    PW_RPC_TEST_METHOD_INFO_FUNCTION(TestBidirectionalStreamRpc);
#undef PW_RPC_TEST_METHOD_INFO_FUNCTION
  };

  struct MethodRequestResponseTypes {
    constexpr bool Pass() const { return true; }

#define PW_RPC_TEST_PUBLIC_METHOD_INFO_TYPES(function)                         \
  static_assert(                                                               \
      std::is_same_v<typename MethodInfo<GeneratedClass::function>::Request,   \
                     ::pw::rpc::MethodRequestType<GeneratedClass::function>>); \
  static_assert(                                                               \
      std::is_same_v<typename MethodInfo<GeneratedClass::function>::Response,  \
                     ::pw::rpc::MethodResponseType<GeneratedClass::function>>)

    PW_RPC_TEST_PUBLIC_METHOD_INFO_TYPES(TestUnaryRpc);
    PW_RPC_TEST_PUBLIC_METHOD_INFO_TYPES(TestAnotherUnaryRpc);
    PW_RPC_TEST_PUBLIC_METHOD_INFO_TYPES(TestServerStreamRpc);
    PW_RPC_TEST_PUBLIC_METHOD_INFO_TYPES(TestClientStreamRpc);
    PW_RPC_TEST_PUBLIC_METHOD_INFO_TYPES(TestBidirectionalStreamRpc);
#undef PW_RPC_TEST_PUBLIC_METHOD_INFO_TYPES
  };
};

}  // namespace pw::rpc::internal
