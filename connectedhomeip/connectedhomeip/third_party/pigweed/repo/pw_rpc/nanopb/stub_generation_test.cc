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

// This macro is used to remove the generated stubs from the proto files. Define
// so that the generated stubs can be tested.
#define _PW_RPC_COMPILE_GENERATED_SERVICE_STUBS

#include "gtest/gtest.h"
#include "pw_rpc_test_protos/test.rpc.pb.h"

namespace {

TEST(NanopbServiceStub, GeneratedStubCompiles) {
  ::pw::rpc::test::TestService test_service;
  EXPECT_STREQ(test_service.name(), "TestService");
}

}  // namespace
