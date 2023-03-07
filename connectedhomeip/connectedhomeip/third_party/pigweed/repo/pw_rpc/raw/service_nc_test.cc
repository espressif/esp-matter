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

#include "pw_compilation_testing/negative_compilation.h"
#include "pw_rpc_test_protos/test.raw_rpc.pb.h"

namespace pw::rpc {
namespace test {

#if PW_NC_TEST(NoMethods)
PW_NC_EXPECT("TestUnaryRpc");

class TestService final
    : public pw_rpc::raw::TestService::Service<TestService> {
 public:
};

#else

class TestService {};

#endif  // PW_NC_TEST

TestService test_service;

}  // namespace test
}  // namespace pw::rpc
