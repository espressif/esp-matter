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

#include "pw_rpc/pwpb/server_reader_writer.h"

#include "gtest/gtest.h"
#include "pw_rpc/pwpb/fake_channel_output.h"
#include "pw_rpc/pwpb/test_method_context.h"
#include "pw_rpc/service.h"
#include "pw_rpc_test_protos/test.rpc.pwpb.h"

namespace pw::rpc {
namespace {

namespace TestRequest = ::pw::rpc::test::pwpb::TestRequest;
namespace TestResponse = ::pw::rpc::test::pwpb::TestResponse;
namespace TestStreamResponse = ::pw::rpc::test::pwpb::TestStreamResponse;

class TestServiceImpl final
    : public test::pw_rpc::pwpb::TestService::Service<TestServiceImpl> {
 public:
  Status TestUnaryRpc(const TestRequest::Message&, TestResponse::Message&) {
    return OkStatus();
  }

  void TestAnotherUnaryRpc(const TestRequest::Message&,
                           PwpbUnaryResponder<TestResponse::Message>&) {}

  void TestServerStreamRpc(const TestRequest::Message&,
                           PwpbServerWriter<TestStreamResponse::Message>&) {}

  void TestClientStreamRpc(
      PwpbServerReader<TestRequest::Message, TestStreamResponse::Message>&) {}

  void TestBidirectionalStreamRpc(
      PwpbServerReaderWriter<TestRequest::Message,
                             TestStreamResponse::Message>&) {}
};

template <auto kMethod>
struct ReaderWriterTestContext {
  using Info = internal::MethodInfo<kMethod>;

  static constexpr uint32_t kChannelId = 1;

  ReaderWriterTestContext()
      : channel(Channel::Create<kChannelId>(&output)),
        server(span(&channel, 1)) {}

  TestServiceImpl service;
  PwpbFakeChannelOutput<4> output;
  Channel channel;
  Server server;
};

using test::pw_rpc::pwpb::TestService;

TEST(PwpbUnaryResponder, DefaultConstructed) {
  PwpbUnaryResponder<TestResponse::Message> call;

  ASSERT_FALSE(call.active());
  EXPECT_EQ(call.channel_id(), Channel::kUnassignedChannelId);

  EXPECT_EQ(Status::FailedPrecondition(), call.Finish({}, OkStatus()));

  call.set_on_error([](Status) {});
}

TEST(PwpbServerWriter, DefaultConstructed) {
  PwpbServerWriter<TestStreamResponse::Message> call;

  ASSERT_FALSE(call.active());
  EXPECT_EQ(call.channel_id(), Channel::kUnassignedChannelId);

  EXPECT_EQ(Status::FailedPrecondition(), call.Write({}));
  EXPECT_EQ(Status::FailedPrecondition(), call.Finish(OkStatus()));

  call.set_on_error([](Status) {});
}

TEST(PwpbServerReader, DefaultConstructed) {
  PwpbServerReader<TestRequest::Message, TestStreamResponse::Message> call;

  ASSERT_FALSE(call.active());
  EXPECT_EQ(call.channel_id(), Channel::kUnassignedChannelId);

  EXPECT_EQ(Status::FailedPrecondition(), call.Finish({}, OkStatus()));

  call.set_on_next([](const TestRequest::Message&) {});
  call.set_on_error([](Status) {});
}

TEST(PwpbServerReaderWriter, DefaultConstructed) {
  PwpbServerReaderWriter<TestRequest::Message, TestStreamResponse::Message>
      call;

  ASSERT_FALSE(call.active());
  EXPECT_EQ(call.channel_id(), Channel::kUnassignedChannelId);

  EXPECT_EQ(Status::FailedPrecondition(), call.Write({}));
  EXPECT_EQ(Status::FailedPrecondition(), call.Finish(OkStatus()));

  call.set_on_next([](const TestRequest::Message&) {});
  call.set_on_error([](Status) {});
}

TEST(PwpbUnaryResponder, Closed) {
  ReaderWriterTestContext<TestService::TestUnaryRpc> ctx;
  PwpbUnaryResponder call = PwpbUnaryResponder<TestResponse::Message>::Open<
      TestService::TestUnaryRpc>(ctx.server, ctx.channel.id(), ctx.service);
  ASSERT_EQ(OkStatus(), call.Finish({}, OkStatus()));

  ASSERT_FALSE(call.active());
  EXPECT_EQ(call.channel_id(), Channel::kUnassignedChannelId);

  EXPECT_EQ(Status::FailedPrecondition(), call.Finish({}, OkStatus()));

  call.set_on_error([](Status) {});
}

TEST(PwpbServerWriter, Closed) {
  ReaderWriterTestContext<TestService::TestServerStreamRpc> ctx;
  PwpbServerWriter call = PwpbServerWriter<TestStreamResponse::Message>::Open<
      TestService::TestServerStreamRpc>(
      ctx.server, ctx.channel.id(), ctx.service);
  ASSERT_EQ(OkStatus(), call.Finish(OkStatus()));

  ASSERT_FALSE(call.active());
  EXPECT_EQ(call.channel_id(), Channel::kUnassignedChannelId);

  EXPECT_EQ(Status::FailedPrecondition(), call.Write({}));
  EXPECT_EQ(Status::FailedPrecondition(), call.Finish(OkStatus()));

  call.set_on_error([](Status) {});
}

TEST(PwpbServerReader, Closed) {
  ReaderWriterTestContext<TestService::TestClientStreamRpc> ctx;
  PwpbServerReader call =
      PwpbServerReader<TestRequest::Message, TestStreamResponse::Message>::Open<
          TestService::TestClientStreamRpc>(
          ctx.server, ctx.channel.id(), ctx.service);
  ASSERT_EQ(OkStatus(), call.Finish({}, OkStatus()));

  ASSERT_FALSE(call.active());
  EXPECT_EQ(call.channel_id(), Channel::kUnassignedChannelId);

  EXPECT_EQ(Status::FailedPrecondition(), call.Finish({}, OkStatus()));

  call.set_on_next([](const TestRequest::Message&) {});
  call.set_on_error([](Status) {});
}

TEST(PwpbServerReaderWriter, Closed) {
  ReaderWriterTestContext<TestService::TestBidirectionalStreamRpc> ctx;
  PwpbServerReaderWriter call =
      PwpbServerReaderWriter<TestRequest::Message,
                             TestStreamResponse::Message>::
          Open<TestService::TestBidirectionalStreamRpc>(
              ctx.server, ctx.channel.id(), ctx.service);
  ASSERT_EQ(OkStatus(), call.Finish(OkStatus()));

  ASSERT_FALSE(call.active());
  EXPECT_EQ(call.channel_id(), Channel::kUnassignedChannelId);

  EXPECT_EQ(Status::FailedPrecondition(), call.Write({}));
  EXPECT_EQ(Status::FailedPrecondition(), call.Finish(OkStatus()));

  call.set_on_next([](const TestRequest::Message&) {});
  call.set_on_error([](Status) {});
}

TEST(PwpbUnaryResponder, Open_ReturnsUsableResponder) {
  ReaderWriterTestContext<TestService::TestUnaryRpc> ctx;
  PwpbUnaryResponder responder =
      PwpbUnaryResponder<TestResponse::Message>::Open<
          TestService::TestUnaryRpc>(ctx.server, ctx.channel.id(), ctx.service);

  ASSERT_EQ(OkStatus(),
            responder.Finish({.value = 4321, .repeated_field = {}}));

  EXPECT_EQ(ctx.output.last_response<TestService::TestUnaryRpc>().value, 4321);
  EXPECT_EQ(ctx.output.last_status(), OkStatus());
}

TEST(PwpbServerWriter, Open_ReturnsUsableWriter) {
  ReaderWriterTestContext<TestService::TestServerStreamRpc> ctx;
  PwpbServerWriter responder =
      PwpbServerWriter<TestStreamResponse::Message>::Open<
          TestService::TestServerStreamRpc>(
          ctx.server, ctx.channel.id(), ctx.service);

  ASSERT_EQ(OkStatus(), responder.Write({.chunk = {}, .number = 321}));
  ASSERT_EQ(OkStatus(), responder.Finish());

  EXPECT_EQ(ctx.output.last_response<TestService::TestServerStreamRpc>().number,
            321u);
  EXPECT_EQ(ctx.output.last_status(), OkStatus());
}

TEST(PwpbServerReader, Open_ReturnsUsableReader) {
  ReaderWriterTestContext<TestService::TestClientStreamRpc> ctx;
  PwpbServerReader responder =
      PwpbServerReader<TestRequest::Message, TestStreamResponse::Message>::Open<
          TestService::TestClientStreamRpc>(
          ctx.server, ctx.channel.id(), ctx.service);

  ASSERT_EQ(OkStatus(), responder.Finish({.chunk = {}, .number = 321}));

  EXPECT_EQ(ctx.output.last_response<TestService::TestClientStreamRpc>().number,
            321u);
}

TEST(PwpbServerReaderWriter, Open_ReturnsUsableReaderWriter) {
  ReaderWriterTestContext<TestService::TestBidirectionalStreamRpc> ctx;
  PwpbServerReaderWriter responder =
      PwpbServerReaderWriter<TestRequest::Message,
                             TestStreamResponse::Message>::
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

  PwpbServerReaderWriter call =
      PwpbServerReaderWriter<TestRequest::Message,
                             TestStreamResponse::Message>::
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

TEST(PwpbServerReader, CallbacksMoveCorrectly) {
  PW_PWPB_TEST_METHOD_CONTEXT(TestServiceImpl, TestClientStreamRpc) ctx;

  PwpbServerReader call_1 = ctx.reader();

  ASSERT_TRUE(call_1.active());

  TestRequest::Message received_request = {.integer = 12345678,
                                           .status_code = 1};

  call_1.set_on_next([&received_request](const TestRequest::Message& value) {
    received_request = value;
  });

  PwpbServerReader<TestRequest::Message, TestStreamResponse::Message> call_2;
  call_2 = std::move(call_1);

  constexpr TestRequest::Message request{.integer = 600613, .status_code = 2};
  ctx.SendClientStream(request);
  EXPECT_EQ(request.integer, received_request.integer);
  EXPECT_EQ(request.status_code, received_request.status_code);
}

}  // namespace
}  // namespace pw::rpc
