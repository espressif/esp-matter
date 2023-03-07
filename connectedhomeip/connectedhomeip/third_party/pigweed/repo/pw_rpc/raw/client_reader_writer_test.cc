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

#include "pw_rpc/raw/client_reader_writer.h"

#include "gtest/gtest.h"
#include "pw_rpc/raw/client_testing.h"
#include "pw_rpc/writer.h"
#include "pw_rpc_test_protos/test.raw_rpc.pb.h"

namespace pw::rpc {
namespace {

using test::pw_rpc::raw::TestService;

void FailIfCalled(Status) { FAIL(); }
void FailIfOnNextCalled(ConstByteSpan) { FAIL(); }
void FailIfOnCompletedCalled(ConstByteSpan, Status) { FAIL(); }

TEST(RawUnaryReceiver, DefaultConstructed) {
  RawUnaryReceiver call;

  ASSERT_FALSE(call.active());
  EXPECT_EQ(call.channel_id(), Channel::kUnassignedChannelId);

  EXPECT_EQ(Status::FailedPrecondition(), call.Cancel());

  call.set_on_completed([](ConstByteSpan, Status) {});
  call.set_on_error([](Status) {});
}

TEST(RawClientWriter, DefaultConstructed) {
  RawClientWriter call;

  ASSERT_FALSE(call.active());
  EXPECT_EQ(call.channel_id(), Channel::kUnassignedChannelId);

  EXPECT_EQ(Status::FailedPrecondition(), call.Write({}));
  EXPECT_EQ(Status::FailedPrecondition(), call.Cancel());
  EXPECT_EQ(Status::FailedPrecondition(), call.CloseClientStream());

  call.set_on_completed([](ConstByteSpan, Status) {});
  call.set_on_error([](Status) {});
}

TEST(RawClientReader, DefaultConstructed) {
  RawClientReader call;

  ASSERT_FALSE(call.active());
  EXPECT_EQ(call.channel_id(), Channel::kUnassignedChannelId);

  EXPECT_EQ(Status::FailedPrecondition(), call.Cancel());

  call.set_on_completed([](Status) {});
  call.set_on_next([](ConstByteSpan) {});
  call.set_on_error([](Status) {});
}

TEST(RawClientReaderWriter, DefaultConstructed) {
  RawClientReaderWriter call;

  ASSERT_FALSE(call.active());
  EXPECT_EQ(call.channel_id(), Channel::kUnassignedChannelId);

  EXPECT_EQ(Status::FailedPrecondition(), call.Write({}));
  EXPECT_EQ(Status::FailedPrecondition(), call.Cancel());
  EXPECT_EQ(Status::FailedPrecondition(), call.CloseClientStream());

  call.set_on_completed([](Status) {});
  call.set_on_next([](ConstByteSpan) {});
  call.set_on_error([](Status) {});
}

TEST(RawUnaryReceiver, Closed) {
  RawClientTestContext ctx;
  RawUnaryReceiver call = TestService::TestUnaryRpc(ctx.client(),
                                                    ctx.channel().id(),
                                                    {},
                                                    FailIfOnCompletedCalled,
                                                    FailIfCalled);
  ASSERT_EQ(OkStatus(), call.Cancel());

  ASSERT_FALSE(call.active());
  EXPECT_EQ(call.channel_id(), Channel::kUnassignedChannelId);

  EXPECT_EQ(Status::FailedPrecondition(), call.Cancel());

  call.set_on_completed([](ConstByteSpan, Status) {});
  call.set_on_error([](Status) {});
}

TEST(RawClientWriter, Closed) {
  RawClientTestContext ctx;
  RawClientWriter call = TestService::TestClientStreamRpc(
      ctx.client(), ctx.channel().id(), FailIfOnCompletedCalled, FailIfCalled);
  ASSERT_EQ(OkStatus(), call.Cancel());

  ASSERT_FALSE(call.active());
  EXPECT_EQ(call.channel_id(), Channel::kUnassignedChannelId);

  EXPECT_EQ(Status::FailedPrecondition(), call.Write({}));
  EXPECT_EQ(Status::FailedPrecondition(), call.Cancel());
  EXPECT_EQ(Status::FailedPrecondition(), call.CloseClientStream());

  call.set_on_completed([](ConstByteSpan, Status) {});
  call.set_on_error([](Status) {});
}

TEST(RawClientReader, Closed) {
  RawClientTestContext ctx;
  RawClientReader call = TestService::TestServerStreamRpc(ctx.client(),
                                                          ctx.channel().id(),
                                                          {},
                                                          FailIfOnNextCalled,
                                                          FailIfCalled,
                                                          FailIfCalled);
  ASSERT_EQ(OkStatus(), call.Cancel());

  ASSERT_FALSE(call.active());
  EXPECT_EQ(call.channel_id(), Channel::kUnassignedChannelId);

  EXPECT_EQ(Status::FailedPrecondition(), call.Cancel());

  call.set_on_completed([](Status) {});
  call.set_on_next([](ConstByteSpan) {});
  call.set_on_error([](Status) {});
}

TEST(RawClientReaderWriter, Closed) {
  RawClientTestContext ctx;
  RawClientReaderWriter call =
      TestService::TestBidirectionalStreamRpc(ctx.client(),
                                              ctx.channel().id(),
                                              FailIfOnNextCalled,
                                              FailIfCalled,
                                              FailIfCalled);
  ASSERT_EQ(OkStatus(), call.Cancel());

  ASSERT_FALSE(call.active());
  EXPECT_EQ(call.channel_id(), Channel::kUnassignedChannelId);

  EXPECT_EQ(Status::FailedPrecondition(), call.Write({}));
  EXPECT_EQ(Status::FailedPrecondition(), call.Cancel());
  EXPECT_EQ(Status::FailedPrecondition(), call.CloseClientStream());

  call.set_on_completed([](Status) {});
  call.set_on_next([](ConstByteSpan) {});
  call.set_on_error([](Status) {});
}

TEST(RawClientReaderWriter, Move_InactiveToActive_EndsClientStream) {
  RawClientTestContext ctx;

  RawClientReaderWriter active_call =
      TestService::TestBidirectionalStreamRpc(ctx.client(),
                                              ctx.channel().id(),
                                              FailIfOnNextCalled,
                                              FailIfCalled,
                                              FailIfCalled);

  ASSERT_EQ(ctx.output().total_packets(), 1u);  // Sent the request

  RawClientReaderWriter inactive_call;

  active_call = std::move(inactive_call);

  EXPECT_EQ(ctx.output().total_packets(), 2u);  // Sent CLIENT_STREAM_END
  EXPECT_EQ(
      ctx.output()
          .client_stream_end_packets<TestService::TestBidirectionalStreamRpc>(),
      1u);

  EXPECT_FALSE(active_call.active());
  // NOLINTNEXTLINE(bugprone-use-after-move)
  EXPECT_FALSE(inactive_call.active());
}

TEST(RawUnaryReceiver, Move_InactiveToActive_SilentlyCloses) {
  RawClientTestContext ctx;

  RawUnaryReceiver active_call =
      TestService::TestUnaryRpc(ctx.client(),
                                ctx.channel().id(),
                                {},
                                FailIfOnCompletedCalled,
                                FailIfCalled);

  ASSERT_EQ(ctx.output().total_packets(), 1u);  // Sent the request

  RawUnaryReceiver inactive_call;

  active_call = std::move(inactive_call);

  EXPECT_EQ(ctx.output().total_packets(), 1u);  // No more packets

  EXPECT_FALSE(active_call.active());
  // NOLINTNEXTLINE(bugprone-use-after-move)
  EXPECT_FALSE(inactive_call.active());
}

TEST(RawUnaryReceiver, Move_ActiveToActive) {
  RawClientTestContext ctx;

  RawUnaryReceiver active_call_1 =
      TestService::TestUnaryRpc(ctx.client(), ctx.channel().id(), {});

  RawUnaryReceiver active_call_2 =
      TestService::TestAnotherUnaryRpc(ctx.client(), ctx.channel().id(), {});

  ASSERT_EQ(ctx.output().total_packets(), 2u);  // Sent the requests
  ASSERT_TRUE(active_call_1.active());
  ASSERT_TRUE(active_call_2.active());

  active_call_2 = std::move(active_call_1);

  EXPECT_EQ(ctx.output().total_packets(), 2u);  // No more packets

  // NOLINTNEXTLINE(bugprone-use-after-move)
  EXPECT_FALSE(active_call_1.active());
  EXPECT_TRUE(active_call_2.active());
}

TEST(RawClientReader, NoClientStream_OutOfScope_SilentlyCloses) {
  RawClientTestContext ctx;

  {
    RawClientReader call = TestService::TestServerStreamRpc(ctx.client(),
                                                            ctx.channel().id(),
                                                            {},
                                                            FailIfOnNextCalled,
                                                            FailIfCalled,
                                                            FailIfCalled);
    ASSERT_EQ(ctx.output().total_packets(), 1u);  // Sent the request
  }

  EXPECT_EQ(ctx.output().total_packets(), 1u);  // No more packets
}

TEST(RawClientWriter, WithClientStream_OutOfScope_SendsClientStreamEnd) {
  RawClientTestContext ctx;

  {
    RawClientWriter call =
        TestService::TestClientStreamRpc(ctx.client(),
                                         ctx.channel().id(),
                                         FailIfOnCompletedCalled,
                                         FailIfCalled);
    ASSERT_EQ(ctx.output().total_packets(), 1u);  // Sent the request
  }

  EXPECT_EQ(ctx.output().total_packets(), 2u);  // Sent CLIENT_STREAM_END
  EXPECT_EQ(ctx.output()
                .client_stream_end_packets<TestService::TestClientStreamRpc>(),
            1u);
}

constexpr const char kWriterData[] = "20X6";

void WriteAsWriter(Writer& writer) {
  ASSERT_TRUE(writer.active());
  ASSERT_EQ(writer.channel_id(), RawClientTestContext<>::kDefaultChannelId);

  EXPECT_EQ(OkStatus(), writer.Write(as_bytes(span(kWriterData))));
}

TEST(RawClientWriter, UsableAsWriter) {
  RawClientTestContext ctx;
  RawClientWriter call = TestService::TestClientStreamRpc(
      ctx.client(), ctx.channel().id(), FailIfOnCompletedCalled, FailIfCalled);

  WriteAsWriter(call);

  EXPECT_STREQ(reinterpret_cast<const char*>(
                   ctx.output()
                       .payloads<TestService::TestClientStreamRpc>()
                       .back()
                       .data()),
               kWriterData);
}

TEST(RawClientReaderWriter, UsableAsWriter) {
  RawClientTestContext ctx;
  RawClientReaderWriter call =
      TestService::TestBidirectionalStreamRpc(ctx.client(),
                                              ctx.channel().id(),
                                              FailIfOnNextCalled,
                                              FailIfCalled,
                                              FailIfCalled);

  WriteAsWriter(call);

  EXPECT_STREQ(reinterpret_cast<const char*>(
                   ctx.output()
                       .payloads<TestService::TestBidirectionalStreamRpc>()
                       .back()
                       .data()),
               kWriterData);
}

const char* span_as_cstr(ConstByteSpan span) {
  return reinterpret_cast<const char*>(span.data());
}

TEST(RawClientReaderWriter,
     MultipleCallsToSameMethodOkAndReceiveSeparateResponses) {
  RawClientTestContext ctx;

  ConstByteSpan data_1 = as_bytes(span("data_1_unset"));
  ConstByteSpan data_2 = as_bytes(span("data_2_unset"));

  Status error;
  auto set_error = [&error](Status status) { error.Update(status); };
  RawClientReaderWriter active_call_1 = TestService::TestBidirectionalStreamRpc(
      ctx.client(),
      ctx.channel().id(),
      [&data_1](ConstByteSpan payload) { data_1 = payload; },
      FailIfCalled,
      set_error);

  EXPECT_TRUE(active_call_1.active());

  RawClientReaderWriter active_call_2 = TestService::TestBidirectionalStreamRpc(
      ctx.client(),
      ctx.channel().id(),
      [&data_2](ConstByteSpan payload) { data_2 = payload; },
      FailIfCalled,
      set_error);

  EXPECT_TRUE(active_call_1.active());
  EXPECT_TRUE(active_call_2.active());
  EXPECT_EQ(error, OkStatus());

  ConstByteSpan message_1 = as_bytes(span("hello_1"));
  ConstByteSpan message_2 = as_bytes(span("hello_2"));

  ctx.server().SendServerStream<TestService::TestBidirectionalStreamRpc>(
      message_2, active_call_2.id());
  EXPECT_STREQ(span_as_cstr(data_2), span_as_cstr(message_2));
  ctx.server().SendServerStream<TestService::TestBidirectionalStreamRpc>(
      message_1, active_call_1.id());
  EXPECT_STREQ(span_as_cstr(data_1), span_as_cstr(message_1));
}

}  // namespace
}  // namespace pw::rpc
