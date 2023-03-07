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

#include "pw_rpc/internal/method_info.h"

#include "gtest/gtest.h"
#include "pw_rpc/internal/method_info_tester.h"
#include "pw_rpc_test_protos/test.rpc.pb.h"

namespace pw::rpc::internal {
namespace {

using GeneratedService = pw::rpc::test::pw_rpc::nanopb::TestService;

class TestService final : public GeneratedService::Service<TestService> {
 public:
  Status TestUnaryRpc(const pw_rpc_test_TestRequest&,
                      pw_rpc_test_TestResponse&) {
    return OkStatus();
  }

  void TestAnotherUnaryRpc(const pw_rpc_test_TestRequest&,
                           NanopbUnaryResponder<pw_rpc_test_TestResponse>&) {}

  static void TestServerStreamRpc(
      const pw_rpc_test_TestRequest&,
      ServerWriter<pw_rpc_test_TestStreamResponse>&) {}

  void TestClientStreamRpc(
      ServerReader<pw_rpc_test_TestRequest, pw_rpc_test_TestStreamResponse>&) {}

  void TestBidirectionalStreamRpc(
      ServerReaderWriter<pw_rpc_test_TestRequest,
                         pw_rpc_test_TestStreamResponse>&) {}
};

static_assert(MethodInfoTests<GeneratedService, TestService>().Pass());

}  // namespace
}  // namespace pw::rpc::internal
