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
#include "pw_rpc/pwpb/client_server_testing.h"
#include "pw_rpc_test_protos/test.rpc.pwpb.h"

namespace pw::rpc {
namespace {

namespace TestRequest = ::pw::rpc::test::pwpb::TestRequest;
namespace TestResponse = ::pw::rpc::test::pwpb::TestResponse;
namespace TestStreamResponse = ::pw::rpc::test::pwpb::TestStreamResponse;

}  // namespace

namespace test {

using GeneratedService = ::pw::rpc::test::pw_rpc::pwpb::TestService;

class TestService final : public GeneratedService::Service<TestService> {
 public:
  Status TestUnaryRpc(const TestRequest::Message& request,
                      TestResponse::Message& response) {
    response.value = request.integer + 1;
    return static_cast<Status::Code>(request.status_code);
  }

  void TestAnotherUnaryRpc(const TestRequest::Message&,
                           PwpbUnaryResponder<TestResponse::Message>&) {}

  static void TestServerStreamRpc(const TestRequest::Message&,
                                  ServerWriter<TestStreamResponse::Message>&) {}

  void TestClientStreamRpc(
      ServerReader<TestRequest::Message, TestStreamResponse::Message>&) {}

  void TestBidirectionalStreamRpc(
      ServerReaderWriter<TestRequest::Message, TestStreamResponse::Message>&) {}
};

}  // namespace test

namespace {

TEST(PwpbClientServerTestContext, ReceivesUnaryRpcReponse) {
  PwpbClientServerTestContext<> ctx;
  test::TestService service;
  ctx.server().RegisterService(service);

  TestResponse::Message response = {};
  auto handler = [&response](const TestResponse::Message& server_response,
                             pw::Status) { response = server_response; };

  TestRequest::Message request{.integer = 1, .status_code = OkStatus().code()};
  auto call = test::GeneratedService::TestUnaryRpc(
      ctx.client(), ctx.channel().id(), request, handler);
  // Force manual forwarding of packets as context is not threaded
  ctx.ForwardNewPackets();

  const auto sent_request =
      ctx.request<test::pw_rpc::pwpb::TestService::TestUnaryRpc>(0);
  const auto sent_response =
      ctx.response<test::pw_rpc::pwpb::TestService::TestUnaryRpc>(0);

  EXPECT_EQ(response.value, sent_response.value);
  EXPECT_EQ(response.value, request.integer + 1);
  EXPECT_EQ(request.integer, sent_request.integer);
}

TEST(PwpbClientServerTestContext, ReceivesMultipleReponses) {
  PwpbClientServerTestContext<> ctx;
  test::TestService service;
  ctx.server().RegisterService(service);

  TestResponse::Message response1 = {};
  TestResponse::Message response2 = {};
  auto handler1 = [&response1](const TestResponse::Message& server_response,
                               pw::Status) { response1 = server_response; };
  auto handler2 = [&response2](const TestResponse::Message& server_response,
                               pw::Status) { response2 = server_response; };

  TestRequest::Message request1{.integer = 1, .status_code = OkStatus().code()};
  TestRequest::Message request2{.integer = 2, .status_code = OkStatus().code()};
  const auto call1 = test::GeneratedService::TestUnaryRpc(
      ctx.client(), ctx.channel().id(), request1, handler1);
  // Force manual forwarding of packets as context is not threaded
  ctx.ForwardNewPackets();
  const auto call2 = test::GeneratedService::TestUnaryRpc(
      ctx.client(), ctx.channel().id(), request2, handler2);
  // Force manual forwarding of packets as context is not threaded
  ctx.ForwardNewPackets();

  const auto sent_request1 =
      ctx.request<test::pw_rpc::pwpb::TestService::TestUnaryRpc>(0);
  const auto sent_request2 =
      ctx.request<test::pw_rpc::pwpb::TestService::TestUnaryRpc>(1);
  const auto sent_response1 =
      ctx.response<test::pw_rpc::pwpb::TestService::TestUnaryRpc>(0);
  const auto sent_response2 =
      ctx.response<test::pw_rpc::pwpb::TestService::TestUnaryRpc>(1);

  EXPECT_EQ(response1.value, request1.integer + 1);
  EXPECT_EQ(response2.value, request2.integer + 1);
  EXPECT_EQ(response1.value, sent_response1.value);
  EXPECT_EQ(response2.value, sent_response2.value);
  EXPECT_EQ(request1.integer, sent_request1.integer);
  EXPECT_EQ(request2.integer, sent_request2.integer);
}

}  // namespace
}  // namespace pw::rpc
