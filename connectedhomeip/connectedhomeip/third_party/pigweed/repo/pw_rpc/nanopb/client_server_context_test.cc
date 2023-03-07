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
#include "pw_rpc/nanopb/client_server_testing.h"
#include "pw_rpc_test_protos/test.rpc.pb.h"

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

TEST(NanopbClientServerTestContext, ReceivesUnaryRpcReponse) {
  NanopbClientServerTestContext<> ctx;
  TestService service;
  ctx.server().RegisterService(service);

  pw_rpc_test_TestResponse response pw_rpc_test_TestResponse_init_default;
  auto handler = [&response](const pw_rpc_test_TestResponse& server_response,
                             pw::Status) { response = server_response; };

  pw_rpc_test_TestRequest request{.integer = 1,
                                  .status_code = OkStatus().code()};
  auto call = GeneratedService::TestUnaryRpc(
      ctx.client(), ctx.channel().id(), request, handler);
  // Force manual forwarding of packets as context is not threaded
  ctx.ForwardNewPackets();

  const auto sent_request =
      ctx.request<test::pw_rpc::nanopb::TestService::TestUnaryRpc>(0);
  const auto sent_response =
      ctx.response<test::pw_rpc::nanopb::TestService::TestUnaryRpc>(0);

  EXPECT_EQ(response.value, sent_response.value);
  EXPECT_EQ(response.value, request.integer + 1);
  EXPECT_EQ(request.integer, sent_request.integer);
}

TEST(NanopbClientServerTestContext, ReceivesMultipleReponses) {
  NanopbClientServerTestContext<> ctx;
  TestService service;
  ctx.server().RegisterService(service);

  pw_rpc_test_TestResponse response1 pw_rpc_test_TestResponse_init_default;
  pw_rpc_test_TestResponse response2 pw_rpc_test_TestResponse_init_default;
  auto handler1 = [&response1](const pw_rpc_test_TestResponse& server_response,
                               pw::Status) { response1 = server_response; };
  auto handler2 = [&response2](const pw_rpc_test_TestResponse& server_response,
                               pw::Status) { response2 = server_response; };

  pw_rpc_test_TestRequest request1{.integer = 1,
                                   .status_code = OkStatus().code()};
  pw_rpc_test_TestRequest request2{.integer = 2,
                                   .status_code = OkStatus().code()};
  const auto call1 = GeneratedService::TestUnaryRpc(
      ctx.client(), ctx.channel().id(), request1, handler1);
  // Force manual forwarding of packets as context is not threaded
  ctx.ForwardNewPackets();
  const auto call2 = GeneratedService::TestUnaryRpc(
      ctx.client(), ctx.channel().id(), request2, handler2);
  // Force manual forwarding of packets as context is not threaded
  ctx.ForwardNewPackets();

  const auto sent_request1 =
      ctx.request<test::pw_rpc::nanopb::TestService::TestUnaryRpc>(0);
  const auto sent_request2 =
      ctx.request<test::pw_rpc::nanopb::TestService::TestUnaryRpc>(1);
  const auto sent_response1 =
      ctx.response<test::pw_rpc::nanopb::TestService::TestUnaryRpc>(0);
  const auto sent_response2 =
      ctx.response<test::pw_rpc::nanopb::TestService::TestUnaryRpc>(1);

  EXPECT_EQ(response1.value, request1.integer + 1);
  EXPECT_EQ(response2.value, request2.integer + 1);
  EXPECT_EQ(response1.value, sent_response1.value);
  EXPECT_EQ(response2.value, sent_response2.value);
  EXPECT_EQ(request1.integer, sent_request1.integer);
  EXPECT_EQ(request2.integer, sent_request2.integer);
}

}  // namespace
}  // namespace pw::rpc
