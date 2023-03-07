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

#include "pw_rpc/internal/call.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>

#include "gtest/gtest.h"
#include "pw_rpc/internal/test_method.h"
#include "pw_rpc/internal/test_utils.h"
#include "pw_rpc/service.h"
#include "pw_rpc_private/fake_server_reader_writer.h"

namespace pw::rpc {

class TestService : public Service {
 public:
  constexpr TestService(uint32_t id) : Service(id, method) {}

  static constexpr internal::TestMethodUnion method = internal::TestMethod(8);
};

namespace internal {
namespace {

constexpr Packet kPacket(pwpb::PacketType::REQUEST, 99, 16, 8);

using ::pw::rpc::internal::test::FakeServerReader;
using ::pw::rpc::internal::test::FakeServerReaderWriter;
using ::pw::rpc::internal::test::FakeServerWriter;
using ::std::byte;
using ::testing::Test;

class ServerWriterTest : public Test {
 public:
  ServerWriterTest() : context_(TestService::method.method()) {
    rpc_lock().lock();
    FakeServerWriter writer_temp(context_.get().ClaimLocked());
    rpc_lock().unlock();
    writer_ = std::move(writer_temp);
  }

  ServerContextForTest<TestService> context_;
  FakeServerWriter writer_;
};

TEST_F(ServerWriterTest, ConstructWithContext_StartsOpen) {
  EXPECT_TRUE(writer_.active());
}

TEST_F(ServerWriterTest, Move_ClosesOriginal) {
  FakeServerWriter moved(std::move(writer_));

#ifndef __clang_analyzer__
  EXPECT_FALSE(writer_.active());
#endif  // ignore use-after-move
  EXPECT_TRUE(moved.active());
}

TEST_F(ServerWriterTest, DefaultConstruct_Closed) {
  FakeServerWriter writer;
  EXPECT_FALSE(writer.active());
}

TEST_F(ServerWriterTest, Construct_RegistersWithServer) {
  LockGuard lock(rpc_lock());
  Call* call = context_.server().FindCall(kPacket);
  ASSERT_NE(call, nullptr);
  EXPECT_EQ(static_cast<void*>(call), static_cast<void*>(&writer_));
}

TEST_F(ServerWriterTest, Destruct_RemovesFromServer) {
  ServerContextForTest<TestService> context(TestService::method.method());
  {
    // Note `lock_guard` cannot be used here, because while the constructor
    // of `FakeServerWriter` requires the lock be held, the destructor acquires
    // it!
    rpc_lock().lock();
    FakeServerWriter writer(context.get().ClaimLocked());
    rpc_lock().unlock();
  }

  LockGuard lock(rpc_lock());
  EXPECT_EQ(context.server().FindCall(kPacket), nullptr);
}

TEST_F(ServerWriterTest, Finish_RemovesFromServer) {
  EXPECT_EQ(OkStatus(), writer_.Finish());
  LockGuard lock(rpc_lock());
  EXPECT_EQ(context_.server().FindCall(kPacket), nullptr);
}

TEST_F(ServerWriterTest, Finish_SendsResponse) {
  EXPECT_EQ(OkStatus(), writer_.Finish());

  ASSERT_EQ(context_.output().total_packets(), 1u);
  const Packet& packet = context_.output().last_packet();
  EXPECT_EQ(packet.type(), pwpb::PacketType::RESPONSE);
  EXPECT_EQ(packet.channel_id(), context_.channel_id());
  EXPECT_EQ(packet.service_id(), context_.service_id());
  EXPECT_EQ(packet.method_id(), context_.get().method().id());
  EXPECT_TRUE(packet.payload().empty());
  EXPECT_EQ(packet.status(), OkStatus());
}

TEST_F(ServerWriterTest, Finish_ReturnsStatusFromChannelSend) {
  context_.output().set_send_status(Status::Unauthenticated());

  // All non-OK statuses are remapped to UNKNOWN.
  EXPECT_EQ(Status::Unknown(), writer_.Finish());
}

TEST_F(ServerWriterTest, Finish) {
  ASSERT_TRUE(writer_.active());
  EXPECT_EQ(OkStatus(), writer_.Finish());
  EXPECT_FALSE(writer_.active());
  EXPECT_EQ(Status::FailedPrecondition(), writer_.Finish());
}

TEST_F(ServerWriterTest, Open_SendsPacketWithPayload) {
  constexpr byte data[] = {byte{0xf0}, byte{0x0d}};
  ASSERT_EQ(OkStatus(), writer_.Write(data));

  byte encoded[64];
  auto result = context_.server_stream(data).Encode(encoded);
  ASSERT_EQ(OkStatus(), result.status());

  ConstByteSpan payload = context_.output().last_packet().payload();
  EXPECT_EQ(sizeof(data), payload.size());
  EXPECT_EQ(0, std::memcmp(data, payload.data(), sizeof(data)));
}

TEST_F(ServerWriterTest, Closed_IgnoresFinish) {
  EXPECT_EQ(OkStatus(), writer_.Finish());
  EXPECT_EQ(Status::FailedPrecondition(), writer_.Finish());
}

TEST_F(ServerWriterTest, DefaultConstructor_NoClientStream) {
  FakeServerWriter writer;
  LockGuard lock(rpc_lock());
  EXPECT_FALSE(writer.as_server_call().has_client_stream());
  EXPECT_FALSE(writer.as_server_call().client_stream_open());
}

TEST_F(ServerWriterTest, Open_NoClientStream) {
  LockGuard lock(rpc_lock());
  EXPECT_FALSE(writer_.as_server_call().has_client_stream());
  EXPECT_FALSE(writer_.as_server_call().client_stream_open());
}

class ServerReaderTest : public Test {
 public:
  ServerReaderTest() : context_(TestService::method.method()) {
    rpc_lock().lock();
    FakeServerReader reader_temp(context_.get().ClaimLocked());
    rpc_lock().unlock();
    reader_ = std::move(reader_temp);
  }

  ServerContextForTest<TestService> context_;
  FakeServerReader reader_;
};

TEST_F(ServerReaderTest, DefaultConstructor_ClientStreamClosed) {
  FakeServerReader reader;
  EXPECT_FALSE(reader.as_server_call().active());
  LockGuard lock(rpc_lock());
  EXPECT_FALSE(reader.as_server_call().client_stream_open());
}

TEST_F(ServerReaderTest, Open_ClientStreamStartsOpen) {
  LockGuard lock(rpc_lock());
  EXPECT_TRUE(reader_.as_server_call().has_client_stream());
  EXPECT_TRUE(reader_.as_server_call().client_stream_open());
}

TEST_F(ServerReaderTest, Close_ClosesClientStream) {
  EXPECT_TRUE(reader_.as_server_call().active());
  rpc_lock().lock();
  EXPECT_TRUE(reader_.as_server_call().client_stream_open());
  rpc_lock().unlock();
  EXPECT_EQ(OkStatus(),
            reader_.as_server_call().CloseAndSendResponse(OkStatus()));

  EXPECT_FALSE(reader_.as_server_call().active());
  LockGuard lock(rpc_lock());
  EXPECT_FALSE(reader_.as_server_call().client_stream_open());
}

TEST_F(ServerReaderTest, EndClientStream_OnlyClosesClientStream) {
  EXPECT_TRUE(reader_.active());
  rpc_lock().lock();
  EXPECT_TRUE(reader_.as_server_call().client_stream_open());
  reader_.as_server_call().HandleClientStreamEnd();

  EXPECT_TRUE(reader_.active());
  LockGuard lock(rpc_lock());
  EXPECT_FALSE(reader_.as_server_call().client_stream_open());
}

class ServerReaderWriterTest : public Test {
 public:
  ServerReaderWriterTest() : context_(TestService::method.method()) {
    rpc_lock().lock();
    FakeServerReaderWriter reader_writer_temp(context_.get().ClaimLocked());
    rpc_lock().unlock();
    reader_writer_ = std::move(reader_writer_temp);
  }

  ServerContextForTest<TestService> context_;
  FakeServerReaderWriter reader_writer_;
};

TEST_F(ServerReaderWriterTest, Move_MaintainsClientStream) {
  FakeServerReaderWriter destination;

  rpc_lock().lock();
  EXPECT_FALSE(destination.as_server_call().client_stream_open());
  rpc_lock().unlock();

  destination = std::move(reader_writer_);
  LockGuard lock(rpc_lock());
  EXPECT_TRUE(destination.as_server_call().has_client_stream());
  EXPECT_TRUE(destination.as_server_call().client_stream_open());
}

TEST_F(ServerReaderWriterTest, Move_MovesCallbacks) {
  int calls = 0;
  reader_writer_.set_on_error([&calls](Status) { calls += 1; });
  reader_writer_.set_on_next([&calls](ConstByteSpan) { calls += 1; });

#if PW_RPC_CLIENT_STREAM_END_CALLBACK
  reader_writer.set_on_client_stream_end([&calls]() { calls += 1; });
#endif  // PW_RPC_CLIENT_STREAM_END_CALLBACK

  FakeServerReaderWriter destination(std::move(reader_writer_));
  rpc_lock().lock();
  destination.as_server_call().HandlePayload({});
  rpc_lock().lock();
  destination.as_server_call().HandleClientStreamEnd();
  rpc_lock().lock();
  destination.as_server_call().HandleError(Status::Unknown());

  EXPECT_EQ(calls, 2 + PW_RPC_CLIENT_STREAM_END_CALLBACK);
}

}  // namespace
}  // namespace internal
}  // namespace pw::rpc
