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

#include "gtest/gtest.h"
#include "pw_rpc/nanopb/client_server_testing_threaded.h"
#include "pw_rpc_test_protos/test.rpc.pb.h"
#include "pw_sync/binary_semaphore.h"
#include "pw_thread/test_threads.h"

namespace pw::rpc {
namespace {

using GeneratedService = ::pw::rpc::test::pw_rpc::nanopb::TestService;

class TestService final : public GeneratedService::Service<TestService> {
 public:
  Status TestUnaryRpc(const pw_rpc_test_TestRequest& request,
                      pw_rpc_test_TestResponse& response) {
    response.value = request.integer + 1;
    return static_cast<Status::Code>(request.status_code);
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

class RpcCaller {
 public:
  void BlockOnResponse(uint32_t i, Client& client, uint32_t channel_id) {
    pw_rpc_test_TestRequest request{.integer = i,
                                    .status_code = OkStatus().code()};
    auto call = GeneratedService::TestUnaryRpc(
        client,
        channel_id,
        request,
        [this](const pw_rpc_test_TestResponse&, Status) {
          semaphore_.release();
        },
        [](Status) {});

    semaphore_.acquire();
  }

 private:
  pw::sync::BinarySemaphore semaphore_;
};

TEST(NanopbClientServerTestContextThreaded, ReceivesUnaryRpcReponseThreaded) {
  NanopbClientServerTestContextThreaded<> ctx(
      thread::test::TestOptionsThread0());
  TestService service;
  ctx.server().RegisterService(service);

  RpcCaller caller;
  constexpr auto value = 1;
  caller.BlockOnResponse(value, ctx.client(), ctx.channel().id());

  const auto request =
      ctx.request<test::pw_rpc::nanopb::TestService::TestUnaryRpc>(0);
  const auto response =
      ctx.response<test::pw_rpc::nanopb::TestService::TestUnaryRpc>(0);

  EXPECT_EQ(value, request.integer);
  EXPECT_EQ(value + 1, response.value);
}

TEST(NanopbClientServerTestContextThreaded, ReceivesMultipleReponsesThreaded) {
  NanopbClientServerTestContextThreaded<> ctx(
      thread::test::TestOptionsThread0());
  TestService service;
  ctx.server().RegisterService(service);

  RpcCaller caller;
  constexpr auto value1 = 1;
  constexpr auto value2 = 2;
  caller.BlockOnResponse(value1, ctx.client(), ctx.channel().id());
  caller.BlockOnResponse(value2, ctx.client(), ctx.channel().id());

  const auto request1 =
      ctx.request<test::pw_rpc::nanopb::TestService::TestUnaryRpc>(0);
  const auto request2 =
      ctx.request<test::pw_rpc::nanopb::TestService::TestUnaryRpc>(1);
  const auto response1 =
      ctx.response<test::pw_rpc::nanopb::TestService::TestUnaryRpc>(0);
  const auto response2 =
      ctx.response<test::pw_rpc::nanopb::TestService::TestUnaryRpc>(1);

  EXPECT_EQ(value1, request1.integer);
  EXPECT_EQ(value2, request2.integer);
  EXPECT_EQ(value1 + 1, response1.value);
  EXPECT_EQ(value2 + 1, response2.value);
}

}  // namespace
}  // namespace pw::rpc
