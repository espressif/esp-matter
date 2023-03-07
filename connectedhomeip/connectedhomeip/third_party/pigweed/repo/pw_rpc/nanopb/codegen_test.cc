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

#include "gtest/gtest.h"
#include "pw_preprocessor/compiler.h"
#include "pw_rpc/internal/hash.h"
#include "pw_rpc/internal/test_utils.h"
#include "pw_rpc/nanopb/test_method_context.h"
#include "pw_rpc_nanopb_private/internal_test_utils.h"
#include "pw_rpc_test_protos/test.rpc.pb.h"

PW_MODIFY_DIAGNOSTICS_PUSH();
PW_MODIFY_DIAGNOSTIC(ignored, "-Wmissing-field-initializers");

namespace pw::rpc {
namespace test {

class TestService final
    : public pw_rpc::nanopb::TestService::Service<TestService> {
 public:
  Status TestUnaryRpc(const pw_rpc_test_TestRequest& request,
                      pw_rpc_test_TestResponse& response) {
    response.value = request.integer + 1;
    return static_cast<Status::Code>(request.status_code);
  }

  void TestAnotherUnaryRpc(
      const pw_rpc_test_TestRequest& request,
      NanopbUnaryResponder<pw_rpc_test_TestResponse>& responder) {
    pw_rpc_test_TestResponse response{};
    EXPECT_EQ(OkStatus(),
              responder.Finish(response, TestUnaryRpc(request, response)));
  }

  static void TestServerStreamRpc(
      const pw_rpc_test_TestRequest& request,
      ServerWriter<pw_rpc_test_TestStreamResponse>& writer) {
    for (int i = 0; i < request.integer; ++i) {
      EXPECT_EQ(
          OkStatus(),
          writer.Write({.chunk = {}, .number = static_cast<uint32_t>(i)}));
    }

    EXPECT_EQ(OkStatus(),
              writer.Finish(static_cast<Status::Code>(request.status_code)));
  }

  void TestClientStreamRpc(
      ServerReader<pw_rpc_test_TestRequest, pw_rpc_test_TestStreamResponse>&
          new_reader) {
    reader = std::move(new_reader);
  }

  void TestBidirectionalStreamRpc(
      ServerReaderWriter<pw_rpc_test_TestRequest,
                         pw_rpc_test_TestStreamResponse>& new_reader_writer) {
    reader_writer = std::move(new_reader_writer);
  }

  ServerReader<pw_rpc_test_TestRequest, pw_rpc_test_TestStreamResponse> reader;
  ServerReaderWriter<pw_rpc_test_TestRequest, pw_rpc_test_TestStreamResponse>
      reader_writer;
};

}  // namespace test

namespace {

using internal::ClientContextForTest;

TEST(NanopbCodegen, CompilesProperly) {
  test::TestService service;
  EXPECT_EQ(internal::UnwrapServiceId(service.service_id()),
            internal::Hash("pw.rpc.test.TestService"));
  EXPECT_STREQ(service.name(), "TestService");
}

TEST(NanopbCodegen, Server_InvokeUnaryRpc) {
  PW_NANOPB_TEST_METHOD_CONTEXT(test::TestService, TestUnaryRpc) context;

  EXPECT_EQ(OkStatus(),
            context.call({.integer = 123, .status_code = OkStatus().code()}));

  EXPECT_EQ(124, context.response().value);

  EXPECT_EQ(Status::InvalidArgument(),
            context.call({.integer = 999,
                          .status_code = Status::InvalidArgument().code()}));
  EXPECT_EQ(1000, context.response().value);
}

TEST(NanopbCodegen, Server_InvokeAsyncUnaryRpc) {
  PW_NANOPB_TEST_METHOD_CONTEXT(test::TestService, TestAnotherUnaryRpc) context;

  context.call({.integer = 123, .status_code = OkStatus().code()});

  EXPECT_EQ(OkStatus(), context.status());
  EXPECT_EQ(124, context.response().value);

  context.call(
      {.integer = 999, .status_code = Status::InvalidArgument().code()});
  EXPECT_EQ(Status::InvalidArgument(), context.status());
  EXPECT_EQ(1000, context.response().value);
}

TEST(NanopbCodegen, Server_InvokeServerStreamingRpc) {
  PW_NANOPB_TEST_METHOD_CONTEXT(test::TestService, TestServerStreamRpc) context;

  context.call({.integer = 0, .status_code = Status::Aborted().code()});

  EXPECT_EQ(Status::Aborted(), context.status());
  EXPECT_TRUE(context.done());
  EXPECT_EQ(context.total_responses(), 0u);

  context.call({.integer = 4, .status_code = OkStatus().code()});

  ASSERT_EQ(4u, context.responses().size());

  for (size_t i = 0; i < context.responses().size(); ++i) {
    EXPECT_EQ(context.responses()[i].number, i);
  }

  EXPECT_EQ(OkStatus().code(), context.status());
}

TEST(NanopbCodegen, Server_InvokeServerStreamingRpc_ManualWriting) {
  PW_NANOPB_TEST_METHOD_CONTEXT(test::TestService, TestServerStreamRpc, 4)
  context;

  ASSERT_EQ(4u, context.max_packets());

  auto writer = context.writer();

  EXPECT_EQ(OkStatus(), writer.Write({.chunk = {}, .number = 3}));
  EXPECT_EQ(OkStatus(), writer.Write({.chunk = {}, .number = 6}));
  EXPECT_EQ(OkStatus(), writer.Write({.chunk = {}, .number = 9}));

  EXPECT_FALSE(context.done());

  EXPECT_EQ(OkStatus(), writer.Finish(Status::Cancelled()));
  ASSERT_TRUE(context.done());
  EXPECT_EQ(Status::Cancelled(), context.status());

  ASSERT_EQ(3u, context.responses().size());

  EXPECT_EQ(context.responses()[0].number, 3u);
  EXPECT_EQ(context.responses()[1].number, 6u);
  EXPECT_EQ(context.responses()[2].number, 9u);
}

TEST(NanopbCodegen, Server_InvokeClientStreamingRpc) {
  PW_NANOPB_TEST_METHOD_CONTEXT(test::TestService, TestClientStreamRpc) context;

  context.call();

  pw_rpc_test_TestRequest request = {};
  context.service().reader.set_on_next(
      [&request](const pw_rpc_test_TestRequest& req) { request = req; });

  context.SendClientStream({.integer = -99, .status_code = 10});
  EXPECT_EQ(request.integer, -99);
  EXPECT_EQ(request.status_code, 10u);

  ASSERT_EQ(OkStatus(),
            context.service().reader.Finish({.chunk = {}, .number = 3},
                                            Status::Unimplemented()));
  EXPECT_EQ(Status::Unimplemented(), context.status());
  EXPECT_EQ(context.response().number, 3u);
}

TEST(NanopbCodegen, Server_InvokeBidirectionalStreamingRpc) {
  PW_NANOPB_TEST_METHOD_CONTEXT(test::TestService, TestBidirectionalStreamRpc)
  context;

  context.call();

  pw_rpc_test_TestRequest request = {};
  context.service().reader_writer.set_on_next(
      [&request](const pw_rpc_test_TestRequest& req) { request = req; });

  context.SendClientStream({.integer = -99, .status_code = 10});
  EXPECT_EQ(request.integer, -99);
  EXPECT_EQ(request.status_code, 10u);

  ASSERT_EQ(OkStatus(),
            context.service().reader_writer.Write({.chunk = {}, .number = 2}));
  EXPECT_EQ(context.responses()[0].number, 2u);

  ASSERT_EQ(OkStatus(),
            context.service().reader_writer.Finish(Status::NotFound()));
  EXPECT_EQ(Status::NotFound(), context.status());
}

TEST(NanopbCodegen, ClientCall_DefaultConstructor) {
  NanopbUnaryReceiver<pw_rpc_test_TestResponse> unary_call;
  NanopbClientReader<pw_rpc_test_TestStreamResponse> server_streaming_call;
}

using TestServiceClient = test::pw_rpc::nanopb::TestService::Client;

TEST(NanopbCodegen, Client_InvokesUnaryRpcWithCallback) {
  constexpr uint32_t kServiceId = internal::Hash("pw.rpc.test.TestService");
  constexpr uint32_t kMethodId = internal::Hash("TestUnaryRpc");

  ClientContextForTest<128, 99, kServiceId, kMethodId> context;

  TestServiceClient test_client(context.client(), context.channel().id());

  struct {
    Status last_status = Status::Unknown();
    int response_value = -1;
  } result;

  auto call = test_client.TestUnaryRpc(
      {.integer = 123, .status_code = 0},
      [&result](const pw_rpc_test_TestResponse& response, Status status) {
        result.last_status = status;
        result.response_value = response.value;
      });

  EXPECT_TRUE(call.active());

  EXPECT_EQ(context.output().total_packets(), 1u);
  auto packet =
      static_cast<const internal::test::FakeChannelOutput&>(context.output())
          .last_packet();
  EXPECT_EQ(packet.channel_id(), context.channel().id());
  EXPECT_EQ(packet.service_id(), kServiceId);
  EXPECT_EQ(packet.method_id(), kMethodId);
  PW_DECODE_PB(pw_rpc_test_TestRequest, sent_proto, packet.payload());
  EXPECT_EQ(sent_proto.integer, 123);

  PW_ENCODE_PB(pw_rpc_test_TestResponse, response, .value = 42);
  EXPECT_EQ(OkStatus(), context.SendResponse(OkStatus(), response));
  EXPECT_EQ(result.last_status, OkStatus());
  EXPECT_EQ(result.response_value, 42);

  EXPECT_FALSE(call.active());
}

TEST(NanopbCodegen, Client_InvokesServerStreamingRpcWithCallback) {
  constexpr uint32_t kServiceId = internal::Hash("pw.rpc.test.TestService");
  constexpr uint32_t kMethodId = internal::Hash("TestServerStreamRpc");

  ClientContextForTest<128, 99, kServiceId, kMethodId> context;

  TestServiceClient test_client(context.client(), context.channel().id());

  struct {
    bool active = true;
    Status stream_status = Status::Unknown();
    int response_value = -1;
  } result;

  auto call = test_client.TestServerStreamRpc(
      {.integer = 123, .status_code = 0},
      [&result](const pw_rpc_test_TestStreamResponse& response) {
        result.active = true;
        result.response_value = response.number;
      },
      [&result](Status status) {
        result.active = false;
        result.stream_status = status;
      });

  EXPECT_TRUE(call.active());

  EXPECT_EQ(context.output().total_packets(), 1u);
  auto packet =
      static_cast<const internal::test::FakeChannelOutput&>(context.output())
          .last_packet();
  EXPECT_EQ(packet.channel_id(), context.channel().id());
  EXPECT_EQ(packet.service_id(), kServiceId);
  EXPECT_EQ(packet.method_id(), kMethodId);
  PW_DECODE_PB(pw_rpc_test_TestRequest, sent_proto, packet.payload());
  EXPECT_EQ(sent_proto.integer, 123);

  PW_ENCODE_PB(
      pw_rpc_test_TestStreamResponse, response, .chunk = {}, .number = 11u);
  EXPECT_EQ(OkStatus(), context.SendServerStream(response));
  EXPECT_TRUE(result.active);
  EXPECT_EQ(result.response_value, 11);

  EXPECT_EQ(OkStatus(), context.SendResponse(Status::NotFound()));
  EXPECT_FALSE(result.active);
  EXPECT_EQ(result.stream_status, Status::NotFound());
}

TEST(NanopbCodegen, Client_StaticMethod_InvokesUnaryRpcWithCallback) {
  constexpr uint32_t kServiceId = internal::Hash("pw.rpc.test.TestService");
  constexpr uint32_t kMethodId = internal::Hash("TestUnaryRpc");

  ClientContextForTest<128, 99, kServiceId, kMethodId> context;

  struct {
    Status last_status = Status::Unknown();
    int response_value = -1;
  } result;

  auto call = test::pw_rpc::nanopb::TestService::TestUnaryRpc(
      context.client(),
      context.channel().id(),
      {.integer = 123, .status_code = 0},
      [&result](const pw_rpc_test_TestResponse& response, Status status) {
        result.last_status = status;
        result.response_value = response.value;
      });

  EXPECT_TRUE(call.active());

  EXPECT_EQ(context.output().total_packets(), 1u);
  auto packet =
      static_cast<const internal::test::FakeChannelOutput&>(context.output())
          .last_packet();
  EXPECT_EQ(packet.channel_id(), context.channel().id());
  EXPECT_EQ(packet.service_id(), kServiceId);
  EXPECT_EQ(packet.method_id(), kMethodId);
  PW_DECODE_PB(pw_rpc_test_TestRequest, sent_proto, packet.payload());
  EXPECT_EQ(sent_proto.integer, 123);

  PW_ENCODE_PB(pw_rpc_test_TestResponse, response, .value = 42);
  EXPECT_EQ(OkStatus(), context.SendResponse(OkStatus(), response));
  EXPECT_EQ(result.last_status, OkStatus());
  EXPECT_EQ(result.response_value, 42);
}

TEST(NanopbCodegen, Client_StaticMethod_InvokesServerStreamingRpcWithCallback) {
  constexpr uint32_t kServiceId = internal::Hash("pw.rpc.test.TestService");
  constexpr uint32_t kMethodId = internal::Hash("TestServerStreamRpc");

  ClientContextForTest<128, 99, kServiceId, kMethodId> context;

  struct {
    bool active = true;
    Status stream_status = Status::Unknown();
    int response_value = -1;
  } result;

  auto call = test::pw_rpc::nanopb::TestService::TestServerStreamRpc(
      context.client(),
      context.channel().id(),
      {.integer = 123, .status_code = 0},
      [&result](const pw_rpc_test_TestStreamResponse& response) {
        result.active = true;
        result.response_value = response.number;
      },
      [&result](Status status) {
        result.active = false;
        result.stream_status = status;
      });

  EXPECT_TRUE(call.active());

  EXPECT_EQ(context.output().total_packets(), 1u);
  auto packet =
      static_cast<const internal::test::FakeChannelOutput&>(context.output())
          .last_packet();
  EXPECT_EQ(packet.channel_id(), context.channel().id());
  EXPECT_EQ(packet.service_id(), kServiceId);
  EXPECT_EQ(packet.method_id(), kMethodId);
  PW_DECODE_PB(pw_rpc_test_TestRequest, sent_proto, packet.payload());
  EXPECT_EQ(sent_proto.integer, 123);

  PW_ENCODE_PB(
      pw_rpc_test_TestStreamResponse, response, .chunk = {}, .number = 11u);
  EXPECT_EQ(OkStatus(), context.SendServerStream(response));
  EXPECT_TRUE(result.active);
  EXPECT_EQ(result.response_value, 11);

  EXPECT_EQ(OkStatus(), context.SendResponse(Status::NotFound()));
  EXPECT_FALSE(result.active);
  EXPECT_EQ(result.stream_status, Status::NotFound());
}

}  // namespace
}  // namespace pw::rpc

PW_MODIFY_DIAGNOSTICS_POP();
