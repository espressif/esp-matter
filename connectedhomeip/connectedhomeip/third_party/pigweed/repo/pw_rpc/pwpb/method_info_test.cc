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

#include "pw_rpc/internal/method_info.h"

#include "gtest/gtest.h"
#include "pw_rpc/internal/method_info_tester.h"
#include "pw_rpc_test_protos/test.rpc.pwpb.h"
#include "pw_status/status.h"

namespace pw::rpc {

namespace test {

namespace TestRequest = ::pw::rpc::test::pwpb::TestRequest;
namespace TestResponse = ::pw::rpc::test::pwpb::TestResponse;
namespace TestStreamResponse = ::pw::rpc::test::pwpb::TestStreamResponse;

}  // namespace test

namespace {

class TestService final
    : public test::pw_rpc::pwpb::TestService::Service<TestService> {
 public:
  Status TestUnaryRpc(const test::TestRequest::Message&,
                      test::TestResponse::Message&) {
    return OkStatus();
  }

  void TestAnotherUnaryRpc(const test::TestRequest::Message&,
                           PwpbUnaryResponder<test::TestResponse::Message>&) {}

  static void TestServerStreamRpc(
      const test::TestRequest::Message&,
      ServerWriter<test::TestStreamResponse::Message>&) {}

  void TestClientStreamRpc(ServerReader<test::TestRequest::Message,
                                        test::TestStreamResponse::Message>&) {}

  void TestBidirectionalStreamRpc(
      ServerReaderWriter<test::TestRequest::Message,
                         test::TestStreamResponse::Message>&) {}
};

static_assert(
    internal::MethodInfoTests<test::pw_rpc::pwpb::TestService, TestService>()
        .Pass());

}  // namespace
}  // namespace pw::rpc
