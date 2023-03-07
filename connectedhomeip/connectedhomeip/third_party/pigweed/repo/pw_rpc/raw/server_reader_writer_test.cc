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

#include "pw_rpc/raw/server_reader_writer.h"

#include "gtest/gtest.h"
#include "pw_rpc/internal/lock.h"
#include "pw_rpc/raw/fake_channel_output.h"
#include "pw_rpc/service.h"
#include "pw_rpc/writer.h"
#include "pw_rpc_test_protos/test.raw_rpc.pb.h"

namespace pw::rpc {

class TestServiceImpl final
    : public test::pw_rpc::raw::TestService::Service<TestServiceImpl> {
 public:
  static void TestUnaryRpc(ConstByteSpan, RawUnaryResponder&) {}

  void TestAnotherUnaryRpc(ConstByteSpan, RawUnaryResponder&) {}

  void TestServerStreamRpc(ConstByteSpan, RawServerWriter&) {}

  void TestClientStreamRpc(RawServerReader&) {}

  void TestBidirectionalStreamRpc(RawServerReaderWriter&) {}
};

struct ReaderWriterTestContext {
  static constexpr uint32_t kChannelId = 1;

  ReaderWriterTestContext()
      : channel(Channel::Create<kChannelId>(&output)),
        server(span(&channel, 1)) {}

  TestServiceImpl service;
  RawFakeChannelOutput<4> output;
  Channel channel;
  Server server;
};

using test::pw_rpc::raw::TestService;

TEST(RawUnaryResponder, DefaultConstructed) {
  RawUnaryResponder call;

  ASSERT_FALSE(call.active());
  EXPECT_EQ(call.channel_id(), Channel::kUnassignedChannelId);

  EXPECT_EQ(Status::FailedPrecondition(), call.Finish({}, OkStatus()));

  call.set_on_error([](Status) {});
}

TEST(RawServerWriter, DefaultConstructed) {
  RawServerWriter call;

  ASSERT_FALSE(call.active());
  EXPECT_EQ(call.channel_id(), Channel::kUnassignedChannelId);

  EXPECT_EQ(Status::FailedPrecondition(), call.Write({}));
  EXPECT_EQ(Status::FailedPrecondition(), call.Finish(OkStatus()));

  call.set_on_error([](Status) {});
}

TEST(RawServerReader, DefaultConstructed) {
  RawServerReader call;

  ASSERT_FALSE(call.active());
  EXPECT_EQ(call.channel_id(), Channel::kUnassignedChannelId);

  EXPECT_EQ(Status::FailedPrecondition(), call.Finish({}, OkStatus()));

  call.set_on_next([](ConstByteSpan) {});
  call.set_on_error([](Status) {});
}

TEST(RawServerReaderWriter, DefaultConstructed) {
  RawServerReaderWriter call;

  ASSERT_FALSE(call.active());
  EXPECT_EQ(call.channel_id(), Channel::kUnassignedChannelId);

  EXPECT_EQ(Status::FailedPrecondition(), call.Write({}));
  EXPECT_EQ(Status::FailedPrecondition(), call.Finish(Status::Cancelled()));

  call.set_on_next([](ConstByteSpan) {});
  call.set_on_error([](Status) {});
}

TEST(RawUnaryResponder, Closed) {
  ReaderWriterTestContext ctx;
  RawUnaryResponder call = RawUnaryResponder::Open<TestService::TestUnaryRpc>(
      ctx.server, ctx.channel.id(), ctx.service);
  ASSERT_EQ(OkStatus(), call.Finish({}, OkStatus()));

  ASSERT_FALSE(call.active());
  EXPECT_EQ(call.channel_id(), Channel::kUnassignedChannelId);

  EXPECT_EQ(Status::FailedPrecondition(), call.Finish({}, OkStatus()));

  call.set_on_error([](Status) {});
}

TEST(RawServerWriter, Closed) {
  ReaderWriterTestContext ctx;
  RawServerWriter call =
      RawServerWriter::Open<TestService::TestServerStreamRpc>(
          ctx.server, ctx.channel.id(), ctx.service);
  ASSERT_EQ(OkStatus(), call.Finish(OkStatus()));

  ASSERT_FALSE(call.active());
  EXPECT_EQ(call.channel_id(), Channel::kUnassignedChannelId);

  EXPECT_EQ(Status::FailedPrecondition(), call.Write({}));
  EXPECT_EQ(Status::FailedPrecondition(), call.Finish(OkStatus()));

  call.set_on_error([](Status) {});
}

TEST(RawServerReader, Closed) {
  ReaderWriterTestContext ctx;
  RawServerReader call =
      RawServerReader::Open<TestService::TestClientStreamRpc>(
          ctx.server, ctx.channel.id(), ctx.service);
  ASSERT_EQ(OkStatus(), call.Finish({}, OkStatus()));

  ASSERT_FALSE(call.active());
  EXPECT_EQ(call.channel_id(), Channel::kUnassignedChannelId);

  EXPECT_EQ(Status::FailedPrecondition(), call.Finish({}, OkStatus()));

  call.set_on_next([](ConstByteSpan) {});
  call.set_on_error([](Status) {});
}

TEST(RawServerReaderWriter, Closed) {
  ReaderWriterTestContext ctx;
  RawServerReaderWriter call =
      RawServerReaderWriter::Open<TestService::TestBidirectionalStreamRpc>(
          ctx.server, ctx.channel.id(), ctx.service);
  ASSERT_EQ(OkStatus(), call.Finish(OkStatus()));

  ASSERT_FALSE(call.active());
  EXPECT_EQ(call.channel_id(), Channel::kUnassignedChannelId);

  EXPECT_EQ(Status::FailedPrecondition(), call.Write({}));
  EXPECT_EQ(Status::FailedPrecondition(), call.Finish(Status::Cancelled()));

  call.set_on_next([](ConstByteSpan) {});
  call.set_on_error([](Status) {});
}

TEST(RawUnaryResponder, Open_ReturnsUsableResponder) {
  ReaderWriterTestContext ctx;
  RawUnaryResponder call = RawUnaryResponder::Open<TestService::TestUnaryRpc>(
      ctx.server, ctx.channel.id(), ctx.service);

  EXPECT_EQ(call.channel_id(), ctx.channel.id());
  EXPECT_EQ(OkStatus(), call.Finish(as_bytes(span("hello from pw_rpc"))));

  EXPECT_STREQ(
      reinterpret_cast<const char*>(
          ctx.output.payloads<TestService::TestUnaryRpc>().back().data()),
      "hello from pw_rpc");
}

TEST(RawServerReaderWriter, Open_UnknownChannel) {
  ReaderWriterTestContext ctx;
  ASSERT_EQ(OkStatus(), ctx.server.CloseChannel(ctx.kChannelId));

  RawServerReaderWriter call =
      RawServerReaderWriter::Open<TestService::TestBidirectionalStreamRpc>(
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

TEST(RawUnaryResponder, Open_MultipleTimes_CancelsPrevious) {
  ReaderWriterTestContext ctx;

  RawUnaryResponder one = RawUnaryResponder::Open<TestService::TestUnaryRpc>(
      ctx.server, ctx.channel.id(), ctx.service);

  ASSERT_TRUE(one.active());

  RawUnaryResponder two = RawUnaryResponder::Open<TestService::TestUnaryRpc>(
      ctx.server, ctx.channel.id(), ctx.service);

  ASSERT_FALSE(one.active());
  ASSERT_TRUE(two.active());
}

TEST(RawServerWriter, Open_ReturnsUsableWriter) {
  ReaderWriterTestContext ctx;
  RawServerWriter call =
      RawServerWriter::Open<TestService::TestServerStreamRpc>(
          ctx.server, ctx.channel.id(), ctx.service);

  EXPECT_EQ(call.channel_id(), ctx.channel.id());
  EXPECT_EQ(OkStatus(), call.Write(as_bytes(span("321"))));

  EXPECT_STREQ(reinterpret_cast<const char*>(
                   ctx.output.payloads<TestService::TestServerStreamRpc>()
                       .back()
                       .data()),
               "321");
}

TEST(RawServerReader, Open_ReturnsUsableReader) {
  ReaderWriterTestContext ctx;
  RawServerReader call =
      RawServerReader::Open<TestService::TestClientStreamRpc>(
          ctx.server, ctx.channel.id(), ctx.service);

  EXPECT_EQ(call.channel_id(), ctx.channel.id());
  EXPECT_EQ(OkStatus(), call.Finish(as_bytes(span("This is a message"))));

  EXPECT_STREQ(reinterpret_cast<const char*>(
                   ctx.output.payloads<TestService::TestClientStreamRpc>()
                       .back()
                       .data()),
               "This is a message");
}

TEST(RawServerReaderWriter, Open_ReturnsUsableReaderWriter) {
  ReaderWriterTestContext ctx;
  RawServerReaderWriter call =
      RawServerReaderWriter::Open<TestService::TestBidirectionalStreamRpc>(
          ctx.server, ctx.channel.id(), ctx.service);

  EXPECT_EQ(call.channel_id(), ctx.channel.id());
  EXPECT_EQ(OkStatus(), call.Write(as_bytes(span("321"))));

  EXPECT_STREQ(
      reinterpret_cast<const char*>(
          ctx.output.payloads<TestService::TestBidirectionalStreamRpc>()
              .back()
              .data()),
      "321");
}

TEST(RawUnaryResponder, Move_FinishesActiveCall) {
  ReaderWriterTestContext ctx;
  RawUnaryResponder active_call =
      RawUnaryResponder::Open<TestService::TestUnaryRpc>(
          ctx.server, ctx.channel.id(), ctx.service);

  RawUnaryResponder inactive_call;

  active_call = std::move(inactive_call);

  const auto completions = ctx.output.completions<TestService::TestUnaryRpc>();
  ASSERT_EQ(completions.size(), 1u);
  EXPECT_EQ(completions.back(), OkStatus());
}

TEST(RawUnaryResponder, Move_DifferentActiveCalls_ClosesFirstOnly) {
  ReaderWriterTestContext ctx;
  RawUnaryResponder active_call =
      RawUnaryResponder::Open<TestService::TestUnaryRpc>(
          ctx.server, ctx.channel.id(), ctx.service);

  RawUnaryResponder new_active_call =
      RawUnaryResponder::Open<TestService::TestAnotherUnaryRpc>(
          ctx.server, ctx.channel.id(), ctx.service);

  EXPECT_TRUE(active_call.active());
  EXPECT_TRUE(new_active_call.active());

  active_call = std::move(new_active_call);

  const auto completions = ctx.output.completions<TestService::TestUnaryRpc>();
  ASSERT_EQ(completions.size(), 1u);
  EXPECT_EQ(completions.back(), OkStatus());

  EXPECT_TRUE(
      ctx.output.completions<TestService::TestAnotherUnaryRpc>().empty());
}

TEST(RawUnaryResponder, ReplaceActiveCall_DoesNotFinishCall) {
  ReaderWriterTestContext ctx;
  RawUnaryResponder active_call =
      RawUnaryResponder::Open<TestService::TestUnaryRpc>(
          ctx.server, ctx.channel.id(), ctx.service);

  RawUnaryResponder new_active_call =
      RawUnaryResponder::Open<TestService::TestUnaryRpc>(
          ctx.server, ctx.channel.id(), ctx.service);

  active_call = std::move(new_active_call);

  ASSERT_TRUE(ctx.output.completions<TestService::TestUnaryRpc>().empty());

  constexpr const char kData[] = "Some data!";
  EXPECT_EQ(
      OkStatus(),
      active_call.Finish(as_bytes(span(kData)), Status::InvalidArgument()));

  EXPECT_STREQ(
      reinterpret_cast<const char*>(
          ctx.output.payloads<TestService::TestUnaryRpc>().back().data()),
      kData);

  const auto completions = ctx.output.completions<TestService::TestUnaryRpc>();
  ASSERT_EQ(completions.size(), 1u);
  EXPECT_EQ(completions.back(), Status::InvalidArgument());
}

TEST(RawUnaryResponder, OutOfScope_FinishesActiveCall) {
  ReaderWriterTestContext ctx;

  {
    RawUnaryResponder call = RawUnaryResponder::Open<TestService::TestUnaryRpc>(
        ctx.server, ctx.channel.id(), ctx.service);
    ASSERT_TRUE(ctx.output.completions<TestService::TestUnaryRpc>().empty());
  }

  const auto completions = ctx.output.completions<TestService::TestUnaryRpc>();
  ASSERT_EQ(completions.size(), 1u);
  EXPECT_EQ(completions.back(), OkStatus());
}

TEST(RawServerWriter, Move_InactiveToActive_FinishesActiveCall) {
  ReaderWriterTestContext ctx;
  RawServerWriter active_call =
      RawServerWriter::Open<TestService::TestServerStreamRpc>(
          ctx.server, ctx.channel.id(), ctx.service);

  RawServerWriter inactive_call;

  active_call = std::move(inactive_call);

  const auto completions =
      ctx.output.completions<TestService::TestServerStreamRpc>();
  ASSERT_EQ(completions.size(), 1u);
  EXPECT_EQ(completions.back(), OkStatus());
}

TEST(RawServerWriter, ReplaceActiveCall_DoesNotFinishCall) {
  ReaderWriterTestContext ctx;
  RawServerWriter active_call =
      RawServerWriter::Open<TestService::TestServerStreamRpc>(
          ctx.server, ctx.channel.id(), ctx.service);

  RawServerWriter new_active_call =
      RawServerWriter::Open<TestService::TestServerStreamRpc>(
          ctx.server, ctx.channel.id(), ctx.service);

  active_call = std::move(new_active_call);

  ASSERT_TRUE(
      ctx.output.completions<TestService::TestServerStreamRpc>().empty());

  constexpr const char kData[] = "Some data!";
  EXPECT_EQ(OkStatus(), active_call.Write(as_bytes(span(kData))));

  EXPECT_STREQ(reinterpret_cast<const char*>(
                   ctx.output.payloads<TestService::TestServerStreamRpc>()
                       .back()
                       .data()),
               kData);
}

constexpr const char kWriterData[] = "20X6";

void WriteAsWriter(Writer& writer) {
  ASSERT_TRUE(writer.active());
  ASSERT_EQ(writer.channel_id(), ReaderWriterTestContext::kChannelId);

  EXPECT_EQ(OkStatus(), writer.Write(as_bytes(span(kWriterData))));
}

TEST(RawServerWriter, UsableAsWriter) {
  ReaderWriterTestContext ctx;
  RawServerWriter call =
      RawServerWriter::Open<TestService::TestServerStreamRpc>(
          ctx.server, ctx.channel.id(), ctx.service);

  WriteAsWriter(call);

  EXPECT_STREQ(reinterpret_cast<const char*>(
                   ctx.output.payloads<TestService::TestServerStreamRpc>()
                       .back()
                       .data()),
               kWriterData);
}

TEST(RawServerReaderWriter, UsableAsWriter) {
  ReaderWriterTestContext ctx;
  RawServerReaderWriter call =
      RawServerReaderWriter::Open<TestService::TestBidirectionalStreamRpc>(
          ctx.server, ctx.channel.id(), ctx.service);

  WriteAsWriter(call);

  EXPECT_STREQ(
      reinterpret_cast<const char*>(
          ctx.output.payloads<TestService::TestBidirectionalStreamRpc>()
              .back()
              .data()),
      kWriterData);
}

}  // namespace pw::rpc
