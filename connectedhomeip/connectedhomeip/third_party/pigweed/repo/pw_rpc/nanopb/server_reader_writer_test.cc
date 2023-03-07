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

#include "pw_rpc/nanopb/server_reader_writer.h"

#include "gtest/gtest.h"
#include "pw_rpc/nanopb/fake_channel_output.h"
#include "pw_rpc/nanopb/test_method_context.h"
#include "pw_rpc/service.h"
#include "pw_rpc_test_protos/test.rpc.pb.h"

namespace pw::rpc {

class TestServiceImpl final
    : public test::pw_rpc::nanopb::TestService::Service<TestServiceImpl> {
 public:
  Status TestUnaryRpc(const pw_rpc_test_TestRequest&,
                      pw_rpc_test_TestResponse&) {
    return OkStatus();
  }

  void TestAnotherUnaryRpc(const pw_rpc_test_TestRequest&,
                           NanopbUnaryResponder<pw_rpc_test_TestResponse>&) {}

  void TestServerStreamRpc(
      const pw_rpc_test_TestRequest&,
      NanopbServerWriter<pw_rpc_test_TestStreamResponse>&) {}

  void TestClientStreamRpc(
      NanopbServerReader<pw_rpc_test_TestRequest,
                         pw_rpc_test_TestStreamResponse>&) {}

  void TestBidirectionalStreamRpc(
      NanopbServerReaderWriter<pw_rpc_test_TestRequest,
                               pw_rpc_test_TestStreamResponse>&) {}
};

template <auto kMethod>
struct ReaderWriterTestContext {
  using Info = internal::MethodInfo<kMethod>;

  static constexpr uint32_t kChannelId = 1;

  ReaderWriterTestContext()
      : channel(Channel::Create<kChannelId>(&output)),
        server(span(&channel, 1)) {}

  TestServiceImpl service;
  NanopbFakeChannelOutput<4> output;
  Channel channel;
  Server server;
};

using test::pw_rpc::nanopb::TestService;

TEST(NanopbUnaryResponder, DefaultConstructed) {
  NanopbUnaryResponder<pw_rpc_test_TestResponse> call;

  ASSERT_FALSE(call.active());
  EXPECT_EQ(call.channel_id(), Channel::kUnassignedChannelId);

  EXPECT_EQ(Status::FailedPrecondition(), call.Finish({}, OkStatus()));

  call.set_on_error([](Status) {});
}

TEST(NanopbServerWriter, DefaultConstructed) {
  NanopbServerWriter<pw_rpc_test_TestStreamResponse> call;

  ASSERT_FALSE(call.active());
  EXPECT_EQ(call.channel_id(), Channel::kUnassignedChannelId);

  EXPECT_EQ(Status::FailedPrecondition(), call.Write({}));
  EXPECT_EQ(Status::FailedPrecondition(), call.Finish(OkStatus()));

  call.set_on_error([](Status) {});
}

TEST(NanopbServerReader, DefaultConstructed) {
  NanopbServerReader<pw_rpc_test_TestRequest, pw_rpc_test_TestStreamResponse>
      call;

  ASSERT_FALSE(call.active());
  EXPECT_EQ(call.channel_id(), Channel::kUnassignedChannelId);

  EXPECT_EQ(Status::FailedPrecondition(), call.Finish({}, OkStatus()));

  call.set_on_next([](const pw_rpc_test_TestRequest&) {});
  call.set_on_error([](Status) {});
}

TEST(NanopbServerReaderWriter, DefaultConstructed) {
  NanopbServerReaderWriter<pw_rpc_test_TestRequest,
                           pw_rpc_test_TestStreamResponse>
      call;

  ASSERT_FALSE(call.active());
  EXPECT_EQ(call.channel_id(), Channel::kUnassignedChannelId);

  EXPECT_EQ(Status::FailedPrecondition(), call.Write({}));
  EXPECT_EQ(Status::FailedPrecondition(), call.Finish(OkStatus()));

  call.set_on_next([](const pw_rpc_test_TestRequest&) {});
  call.set_on_error([](Status) {});
}

TEST(NanopbUnaryResponder, Closed) {
  ReaderWriterTestContext<TestService::TestUnaryRpc> ctx;
  NanopbUnaryResponder call =
      NanopbUnaryResponder<pw_rpc_test_TestResponse>::Open<
          TestService::TestUnaryRpc>(ctx.server, ctx.channel.id(), ctx.service);
  ASSERT_EQ(OkStatus(), call.Finish({}, OkStatus()));

  ASSERT_FALSE(call.active());
  EXPECT_EQ(call.channel_id(), Channel::kUnassignedChannelId);

  EXPECT_EQ(Status::FailedPrecondition(), call.Finish({}, OkStatus()));

  call.set_on_error([](Status) {});
}

TEST(NanopbServerWriter, Closed) {
  ReaderWriterTestContext<TestService::TestServerStreamRpc> ctx;
  NanopbServerWriter call =
      NanopbServerWriter<pw_rpc_test_TestStreamResponse>::Open<
          TestService::TestServerStreamRpc>(
          ctx.server, ctx.channel.id(), ctx.service);
  ASSERT_EQ(OkStatus(), call.Finish(OkStatus()));

  ASSERT_FALSE(call.active());
  EXPECT_EQ(call.channel_id(), Channel::kUnassignedChannelId);

  EXPECT_EQ(Status::FailedPrecondition(), call.Write({}));
  EXPECT_EQ(Status::FailedPrecondition(), call.Finish(OkStatus()));

  call.set_on_error([](Status) {});
}

TEST(NanopbServerReader, Closed) {
  ReaderWriterTestContext<TestService::TestClientStreamRpc> ctx;
  NanopbServerReader call = NanopbServerReader<pw_rpc_test_TestRequest,
                                               pw_rpc_test_TestStreamResponse>::
      Open<TestService::TestClientStreamRpc>(
          ctx.server, ctx.channel.id(), ctx.service);
  ASSERT_EQ(OkStatus(), call.Finish({}, OkStatus()));

  ASSERT_FALSE(call.active());
  EXPECT_EQ(call.channel_id(), Channel::kUnassignedChannelId);

  EXPECT_EQ(Status::FailedPrecondition(), call.Finish({}, OkStatus()));

  call.set_on_next([](const pw_rpc_test_TestRequest&) {});
  call.set_on_error([](Status) {});
}

TEST(NanopbServerReaderWriter, Closed) {
  ReaderWriterTestContext<TestService::TestBidirectionalStreamRpc> ctx;
  NanopbServerReaderWriter call =
      NanopbServerReaderWriter<pw_rpc_test_TestRequest,
                               pw_rpc_test_TestStreamResponse>::
          Open<TestService::TestBidirectionalStreamRpc>(
              ctx.server, ctx.channel.id(), ctx.service);
  ASSERT_EQ(OkStatus(), call.Finish(OkStatus()));

  ASSERT_FALSE(call.active());
  EXPECT_EQ(call.channel_id(), Channel::kUnassignedChannelId);

  EXPECT_EQ(Status::FailedPrecondition(), call.Write({}));
  EXPECT_EQ(Status::FailedPrecondition(), call.Finish(OkStatus()));

  call.set_on_next([](const pw_rpc_test_TestRequest&) {});
  call.set_on_error([](Status) {});
}

TEST(NanopbUnaryResponder, Open_ReturnsUsableResponder) {
  ReaderWriterTestContext<TestService::TestUnaryRpc> ctx;
  NanopbUnaryResponder responder =
      NanopbUnaryResponder<pw_rpc_test_TestResponse>::Open<
          TestService::TestUnaryRpc>(ctx.server, ctx.channel.id(), ctx.service);

  ASSERT_EQ(OkStatus(),
            responder.Finish({.value = 4321, .repeated_field = {}}));

  EXPECT_EQ(ctx.output.last_response<TestService::TestUnaryRpc>().value, 4321);
  EXPECT_EQ(ctx.output.last_status(), OkStatus());
}

TEST(NanopbServerWriter, Open_ReturnsUsableWriter) {
  ReaderWriterTestContext<TestService::TestServerStreamRpc> ctx;
  NanopbServerWriter responder =
      NanopbServerWriter<pw_rpc_test_TestStreamResponse>::Open<
          TestService::TestServerStreamRpc>(
          ctx.server, ctx.channel.id(), ctx.service);

  ASSERT_EQ(OkStatus(), responder.Write({.chunk = {}, .number = 321}));
  ASSERT_EQ(OkStatus(), responder.Finish());

  EXPECT_EQ(ctx.output.last_response<TestService::TestServerStreamRpc>().number,
            321u);
  EXPECT_EQ(ctx.output.last_status(), OkStatus());
}

TEST(NanopbServerReader, Open_ReturnsUsableReader) {
  ReaderWriterTestContext<TestService::TestClientStreamRpc> ctx;
  NanopbServerReader responder =
      NanopbServerReader<pw_rpc_test_TestRequest,
                         pw_rpc_test_TestStreamResponse>::
          Open<TestService::TestClientStreamRpc>(
              ctx.server, ctx.channel.id(), ctx.service);

  ASSERT_EQ(OkStatus(), responder.Finish({.chunk = {}, .number = 321}));

  EXPECT_EQ(ctx.output.last_response<TestService::TestClientStreamRpc>().number,
            321u);
}

TEST(NanopbServerReaderWriter, Open_ReturnsUsableReaderWriter) {
  ReaderWriterTestContext<TestService::TestBidirectionalStreamRpc> ctx;
  NanopbServerReaderWriter responder =
      NanopbServerReaderWriter<pw_rpc_test_TestRequest,
                               pw_rpc_test_TestStreamResponse>::
          Open<TestService::TestBidirectionalStreamRpc>(
              ctx.server, ctx.channel.id(), ctx.service);

  ASSERT_EQ(OkStatus(), responder.Write({.chunk = {}, .number = 321}));
  ASSERT_EQ(OkStatus(), responder.Finish(Status::NotFound()));

  EXPECT_EQ(ctx.output.last_response<TestService::TestBidirectionalStreamRpc>()
                .number,
            321u);
  EXPECT_EQ(ctx.output.last_status(), Status::NotFound());
}

TEST(RawServerReaderWriter, Open_UnknownChannel) {
  ReaderWriterTestContext<TestService::TestBidirectionalStreamRpc> ctx;
  ASSERT_EQ(OkStatus(), ctx.server.CloseChannel(ctx.kChannelId));

  NanopbServerReaderWriter call =
      NanopbServerReaderWriter<pw_rpc_test_TestRequest,
                               pw_rpc_test_TestStreamResponse>::
          Open<TestService::TestBidirectionalStreamRpc>(
              ctx.server, ctx.kChannelId, ctx.service);

  EXPECT_TRUE(call.active());
  EXPECT_EQ(call.channel_id(), ctx.kChannelId);
  EXPECT_EQ(Status::Unavailable(), call.Write({}));

  ASSERT_EQ(OkStatus(), ctx.server.OpenChannel(ctx.kChannelId, ctx.output));

  EXPECT_EQ(OkStatus(), call.Write({}));
  EXPECT_TRUE(call.active());

  EXPECT_EQ(OkStatus(), call.Finish());
  EXPECT_FALSE(call.active());
  EXPECT_EQ(call.channel_id(), Channel::kUnassignedChannelId);
}

TEST(NanopbServerReader, CallbacksMoveCorrectly) {
  PW_NANOPB_TEST_METHOD_CONTEXT(TestServiceImpl, TestClientStreamRpc) ctx;

  NanopbServerReader call_1 = ctx.reader();

  ASSERT_TRUE(call_1.active());

  pw_rpc_test_TestRequest received_request = {.integer = 12345678,
                                              .status_code = 1};

  call_1.set_on_next([&received_request](const pw_rpc_test_TestRequest& value) {
    received_request = value;
  });

  NanopbServerReader<pw_rpc_test_TestRequest, pw_rpc_test_TestStreamResponse>
      call_2;
  call_2 = std::move(call_1);

  constexpr pw_rpc_test_TestRequest request{.integer = 600613,
                                            .status_code = 2};
  ctx.SendClientStream(request);
  EXPECT_EQ(request.integer, received_request.integer);
  EXPECT_EQ(request.status_code, received_request.status_code);
}

}  // namespace pw::rpc
