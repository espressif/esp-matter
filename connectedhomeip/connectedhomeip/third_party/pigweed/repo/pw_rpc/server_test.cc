// Copyright 2020 The Pigweed Authors
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

#include "pw_rpc/server.h"

#include <array>
#include <cstdint>

#include "gtest/gtest.h"
#include "pw_assert/check.h"
#include "pw_rpc/internal/call.h"
#include "pw_rpc/internal/method.h"
#include "pw_rpc/internal/packet.h"
#include "pw_rpc/internal/test_method.h"
#include "pw_rpc/internal/test_utils.h"
#include "pw_rpc/service.h"
#include "pw_rpc_private/fake_server_reader_writer.h"

namespace pw::rpc {
namespace {

using std::byte;

using internal::Packet;
using internal::TestMethod;
using internal::TestMethodUnion;
using internal::pwpb::PacketType;

class TestService : public Service {
 public:
  TestService(uint32_t service_id)
      : Service(service_id, methods_),
        methods_{
            TestMethod(100, MethodType::kBidirectionalStreaming),
            TestMethod(200),
        } {}

  const TestMethod& method(uint32_t id) {
    for (TestMethodUnion& method : methods_) {
      if (method.method().id() == id) {
        return method.test_method();
      }
    }

    PW_CRASH("Invalid method ID %u", static_cast<unsigned>(id));
  }

 private:
  std::array<TestMethodUnion, 2> methods_;
};

class EmptyService : public Service {
 public:
  constexpr EmptyService() : Service(200, methods_) {}

 private:
  static constexpr std::array<TestMethodUnion, 0> methods_ = {};
};

uint32_t kDefaultCallId = 24601;

class BasicServer : public ::testing::Test {
 protected:
  static constexpr byte kDefaultPayload[] = {
      byte(0x82), byte(0x02), byte(0xff), byte(0xff)};

  BasicServer()
      : channels_{
            Channel::Create<1>(&output_),
            Channel::Create<2>(&output_),
            Channel(),  // available for assignment
        },
        server_(channels_),
        service_1_(1),
        service_42_(42) {
    server_.RegisterService(service_1_, service_42_, empty_service_);
  }

  span<const byte> EncodePacket(PacketType type,
                                uint32_t channel_id,
                                uint32_t service_id,
                                uint32_t method_id,
                                uint32_t call_id = kDefaultCallId) {
    return EncodePacketWithBody(type,
                                channel_id,
                                service_id,
                                method_id,
                                call_id,
                                kDefaultPayload,
                                OkStatus());
  }

  span<const byte> EncodeCancel(uint32_t channel_id = 1,
                                uint32_t service_id = 42,
                                uint32_t method_id = 100,
                                uint32_t call_id = kDefaultCallId) {
    return EncodePacketWithBody(PacketType::CLIENT_ERROR,
                                channel_id,
                                service_id,
                                method_id,
                                call_id,
                                {},
                                Status::Cancelled());
  }

  template <typename T = ConstByteSpan>
  ConstByteSpan PacketForRpc(PacketType type,
                             Status status = OkStatus(),
                             T&& payload = {},
                             uint32_t call_id = kDefaultCallId) {
    return EncodePacketWithBody(
        type, 1, 42, 100, call_id, as_bytes(span(payload)), status);
  }

  RawFakeChannelOutput<2> output_;
  std::array<Channel, 3> channels_;
  Server server_;
  TestService service_1_;
  TestService service_42_;
  EmptyService empty_service_;

 private:
  byte request_buffer_[64];

  span<const byte> EncodePacketWithBody(PacketType type,
                                        uint32_t channel_id,
                                        uint32_t service_id,
                                        uint32_t method_id,
                                        uint32_t call_id,
                                        span<const byte> payload,
                                        Status status) {
    auto result =
        Packet(
            type, channel_id, service_id, method_id, call_id, payload, status)
            .Encode(request_buffer_);
    EXPECT_EQ(OkStatus(), result.status());
    return result.value_or(ConstByteSpan());
  }
};

TEST_F(BasicServer, ProcessPacket_ValidMethodInService1_InvokesMethod) {
  EXPECT_EQ(
      OkStatus(),
      server_.ProcessPacket(EncodePacket(PacketType::REQUEST, 1, 1, 100)));

  const TestMethod& method = service_1_.method(100);
  EXPECT_EQ(1u, method.last_channel_id());
  ASSERT_EQ(sizeof(kDefaultPayload), method.last_request().payload().size());
  EXPECT_EQ(std::memcmp(kDefaultPayload,
                        method.last_request().payload().data(),
                        method.last_request().payload().size()),
            0);
}

TEST_F(BasicServer, ProcessPacket_ValidMethodInService42_InvokesMethod) {
  EXPECT_EQ(
      OkStatus(),
      server_.ProcessPacket(EncodePacket(PacketType::REQUEST, 1, 42, 200)));

  const TestMethod& method = service_42_.method(200);
  EXPECT_EQ(1u, method.last_channel_id());
  ASSERT_EQ(sizeof(kDefaultPayload), method.last_request().payload().size());
  EXPECT_EQ(std::memcmp(kDefaultPayload,
                        method.last_request().payload().data(),
                        method.last_request().payload().size()),
            0);
}

TEST_F(BasicServer, UnregisterService_CannotCallMethod) {
  const uint32_t kCallId = 8675309;
  server_.UnregisterService(service_1_, service_42_);

  EXPECT_EQ(OkStatus(),
            server_.ProcessPacket(
                EncodePacket(PacketType::REQUEST, 1, 1, 100, kCallId)));

  const Packet& packet =
      static_cast<internal::test::FakeChannelOutput&>(output_).last_packet();
  EXPECT_EQ(packet.type(), PacketType::SERVER_ERROR);
  EXPECT_EQ(packet.channel_id(), 1u);
  EXPECT_EQ(packet.service_id(), 1u);
  EXPECT_EQ(packet.method_id(), 100u);
  EXPECT_EQ(packet.call_id(), kCallId);
  EXPECT_EQ(packet.status(), Status::NotFound());
}

TEST_F(BasicServer, UnregisterService_AlreadyUnregistered_DoesNothing) {
  server_.UnregisterService(service_42_, service_42_, service_42_);
  server_.UnregisterService(service_42_);

  EXPECT_EQ(
      OkStatus(),
      server_.ProcessPacket(EncodePacket(PacketType::REQUEST, 1, 1, 100)));

  const TestMethod& method = service_1_.method(100);
  EXPECT_EQ(1u, method.last_channel_id());
  ASSERT_EQ(sizeof(kDefaultPayload), method.last_request().payload().size());
  EXPECT_EQ(std::memcmp(kDefaultPayload,
                        method.last_request().payload().data(),
                        method.last_request().payload().size()),
            0);
}

TEST_F(BasicServer, ProcessPacket_IncompletePacket_NothingIsInvoked) {
  EXPECT_EQ(
      Status::DataLoss(),
      server_.ProcessPacket(EncodePacket(PacketType::REQUEST, 0, 42, 101)));
  EXPECT_EQ(
      Status::DataLoss(),
      server_.ProcessPacket(EncodePacket(PacketType::REQUEST, 1, 0, 101)));
  EXPECT_EQ(Status::DataLoss(),
            server_.ProcessPacket(EncodePacket(PacketType::REQUEST, 1, 42, 0)));

  EXPECT_EQ(0u, service_42_.method(100).last_channel_id());
  EXPECT_EQ(0u, service_42_.method(200).last_channel_id());
}

TEST_F(BasicServer, ProcessPacket_NoChannel_SendsNothing) {
  EXPECT_EQ(
      Status::DataLoss(),
      server_.ProcessPacket(EncodePacket(PacketType::REQUEST, 0, 42, 101)));

  EXPECT_EQ(output_.total_packets(), 0u);
}

TEST_F(BasicServer, ProcessPacket_NoService_SendsNothing) {
  EXPECT_EQ(
      Status::DataLoss(),
      server_.ProcessPacket(EncodePacket(PacketType::REQUEST, 1, 0, 101)));

  EXPECT_EQ(output_.total_packets(), 0u);
}

TEST_F(BasicServer, ProcessPacket_NoMethod_SendsNothing) {
  EXPECT_EQ(Status::DataLoss(),
            server_.ProcessPacket(EncodePacket(PacketType::REQUEST, 1, 42, 0)));

  EXPECT_EQ(output_.total_packets(), 0u);
}

TEST_F(BasicServer, ProcessPacket_InvalidMethod_NothingIsInvoked) {
  EXPECT_EQ(
      OkStatus(),
      server_.ProcessPacket(EncodePacket(PacketType::REQUEST, 1, 42, 101)));

  EXPECT_EQ(0u, service_42_.method(100).last_channel_id());
  EXPECT_EQ(0u, service_42_.method(200).last_channel_id());
}

TEST_F(BasicServer, ProcessPacket_ClientErrorWithInvalidMethod_NoResponse) {
  EXPECT_EQ(OkStatus(),
            server_.ProcessPacket(
                EncodePacket(PacketType::CLIENT_ERROR, 1, 42, 101)));

  EXPECT_EQ(0u, output_.total_packets());
}

TEST_F(BasicServer, ProcessPacket_InvalidMethod_SendsError) {
  EXPECT_EQ(
      OkStatus(),
      server_.ProcessPacket(EncodePacket(PacketType::REQUEST, 1, 42, 27)));

  const Packet& packet =
      static_cast<internal::test::FakeChannelOutput&>(output_).last_packet();
  EXPECT_EQ(packet.type(), PacketType::SERVER_ERROR);
  EXPECT_EQ(packet.channel_id(), 1u);
  EXPECT_EQ(packet.service_id(), 42u);
  EXPECT_EQ(packet.method_id(), 27u);  // No method ID 27
  EXPECT_EQ(packet.status(), Status::NotFound());
}

TEST_F(BasicServer, ProcessPacket_InvalidService_SendsError) {
  EXPECT_EQ(
      OkStatus(),
      server_.ProcessPacket(EncodePacket(PacketType::REQUEST, 1, 43, 27)));

  const Packet& packet =
      static_cast<internal::test::FakeChannelOutput&>(output_).last_packet();
  EXPECT_EQ(packet.type(), PacketType::SERVER_ERROR);
  EXPECT_EQ(packet.channel_id(), 1u);
  EXPECT_EQ(packet.service_id(), 43u);  // No service ID 43
  EXPECT_EQ(packet.method_id(), 27u);
  EXPECT_EQ(packet.status(), Status::NotFound());
}

TEST_F(BasicServer, ProcessPacket_UnassignedChannel) {
  EXPECT_EQ(Status::Unavailable(),
            server_.ProcessPacket(
                EncodePacket(PacketType::REQUEST, /*channel_id=*/99, 42, 27)));
}

TEST_F(BasicServer, ProcessPacket_ClientErrorOnUnassignedChannel_NoResponse) {
  channels_[2] = Channel::Create<3>(&output_);  // Occupy only available channel

  EXPECT_EQ(Status::Unavailable(),
            server_.ProcessPacket(EncodePacket(
                PacketType::CLIENT_ERROR, /*channel_id=*/99, 42, 27)));

  EXPECT_EQ(0u, output_.total_packets());
}

TEST_F(BasicServer, ProcessPacket_Cancel_MethodNotActive_SendsNothing) {
  // Set up a fake ServerWriter representing an ongoing RPC.
  EXPECT_EQ(OkStatus(), server_.ProcessPacket(EncodeCancel(1, 42, 100)));

  EXPECT_EQ(output_.total_packets(), 0u);
}

const Channel* GetChannel(internal::Endpoint& endpoint, uint32_t id) {
  internal::LockGuard lock(internal::rpc_lock());
  return endpoint.GetInternalChannel(id);
}

TEST_F(BasicServer, CloseChannel_NoCalls) {
  EXPECT_NE(nullptr, GetChannel(server_, 2));
  EXPECT_EQ(OkStatus(), server_.CloseChannel(2));
  EXPECT_EQ(nullptr, GetChannel(server_, 2));
  ASSERT_EQ(output_.total_packets(), 0u);
}

TEST_F(BasicServer, CloseChannel_UnknownChannel) {
  ASSERT_EQ(nullptr, GetChannel(server_, 13579));
  EXPECT_EQ(Status::NotFound(), server_.CloseChannel(13579));
}

TEST_F(BasicServer, CloseChannel_PendingCall) {
  EXPECT_NE(nullptr, GetChannel(server_, 1));
  EXPECT_EQ(static_cast<internal::Endpoint&>(server_).active_call_count(), 0u);

  internal::TestMethod::FakeServerCall call;
  service_42_.method(100).keep_call_active(call);

  EXPECT_EQ(
      OkStatus(),
      server_.ProcessPacket(EncodePacket(PacketType::REQUEST, 1, 42, 100)));

  Status on_error_status;
  call.set_on_error(
      [&on_error_status](Status error) { on_error_status = error; });

  ASSERT_TRUE(call.active());
  EXPECT_EQ(static_cast<internal::Endpoint&>(server_).active_call_count(), 1u);

  EXPECT_EQ(OkStatus(), server_.CloseChannel(1));
  EXPECT_EQ(nullptr, GetChannel(server_, 1));

  EXPECT_EQ(static_cast<internal::Endpoint&>(server_).active_call_count(), 0u);

  // Should call on_error, but not send a packet since the channel is closed.
  EXPECT_EQ(Status::Aborted(), on_error_status);
  ASSERT_EQ(output_.total_packets(), 0u);
}

TEST_F(BasicServer, OpenChannel_UnusedSlot) {
  const span request = EncodePacket(PacketType::REQUEST, 9, 42, 100);
  EXPECT_EQ(Status::Unavailable(), server_.ProcessPacket(request));

  EXPECT_EQ(OkStatus(), server_.OpenChannel(9, output_));
  EXPECT_EQ(OkStatus(), server_.ProcessPacket(request));

  const Packet& packet =
      static_cast<internal::test::FakeChannelOutput&>(output_).last_packet();
  EXPECT_EQ(packet.type(), PacketType::RESPONSE);
  EXPECT_EQ(packet.channel_id(), 9u);
  EXPECT_EQ(packet.service_id(), 42u);
  EXPECT_EQ(packet.method_id(), 100u);
}

TEST_F(BasicServer, OpenChannel_AlreadyExists) {
  ASSERT_NE(nullptr, GetChannel(server_, 1));
  EXPECT_EQ(Status::AlreadyExists(), server_.OpenChannel(1, output_));
}

TEST_F(BasicServer, OpenChannel_AdditionalSlot) {
  EXPECT_EQ(OkStatus(), server_.OpenChannel(3, output_));

  constexpr Status kExpected =
      PW_RPC_DYNAMIC_ALLOCATION == 0 ? Status::ResourceExhausted() : OkStatus();
  EXPECT_EQ(kExpected, server_.OpenChannel(19823, output_));
}

class BidiMethod : public BasicServer {
 protected:
  BidiMethod() {
    internal::rpc_lock().lock();
    internal::CallContext context(server_,
                                  channels_[0].id(),
                                  service_42_,
                                  service_42_.method(100),
                                  kDefaultCallId);
    // A local temporary is required since the constructor requires a lock,
    // but the *move* constructor takes out the lock.
    internal::test::FakeServerReaderWriter responder_temp(
        context.ClaimLocked());
    internal::rpc_lock().unlock();
    responder_ = std::move(responder_temp);
    PW_CHECK(responder_.active());
  }

  internal::test::FakeServerReaderWriter responder_;
};

TEST_F(BidiMethod, DuplicateCallId_CancelsExistingThenCallsAgain) {
  int cancelled = 0;
  responder_.set_on_error([&cancelled](Status error) {
    if (error.IsCancelled()) {
      cancelled += 1;
    }
  });

  const TestMethod& method = service_42_.method(100);
  ASSERT_EQ(method.invocations(), 0u);

  EXPECT_EQ(OkStatus(),
            server_.ProcessPacket(PacketForRpc(PacketType::REQUEST)));

  EXPECT_EQ(cancelled, 1);
  EXPECT_EQ(method.invocations(), 1u);
}

TEST_F(BidiMethod, DuplicateMethodDifferentCallId_NotCancelled) {
  int cancelled = 0;
  responder_.set_on_error([&cancelled](Status error) {
    if (error.IsCancelled()) {
      cancelled += 1;
    }
  });

  const uint32_t kSecondCallId = 1625;
  EXPECT_EQ(OkStatus(),
            server_.ProcessPacket(PacketForRpc(
                PacketType::REQUEST, OkStatus(), {}, kSecondCallId)));

  EXPECT_EQ(cancelled, 0);
}

const char* span_as_cstr(ConstByteSpan span) {
  return reinterpret_cast<const char*>(span.data());
}

TEST_F(BidiMethod, DuplicateMethodDifferentCallIdEachCallGetsSeparateResponse) {
  const uint32_t kSecondCallId = 1625;

  internal::rpc_lock().lock();
  internal::test::FakeServerReaderWriter responder_2(
      internal::CallContext(server_,
                            channels_[0].id(),
                            service_42_,
                            service_42_.method(100),
                            kSecondCallId)
          .ClaimLocked());
  internal::rpc_lock().unlock();

  ConstByteSpan data_1 = as_bytes(span("data_1_unset"));
  responder_.set_on_next(
      [&data_1](ConstByteSpan payload) { data_1 = payload; });

  ConstByteSpan data_2 = as_bytes(span("data_2_unset"));
  responder_2.set_on_next(
      [&data_2](ConstByteSpan payload) { data_2 = payload; });

  const char* kMessage1 = "hello_1";
  const char* kMessage2 = "hello_2";

  EXPECT_EQ(
      OkStatus(),
      server_.ProcessPacket(PacketForRpc(
          PacketType::CLIENT_STREAM, OkStatus(), "hello_2", kSecondCallId)));

  EXPECT_STREQ(span_as_cstr(data_2), kMessage2);

  EXPECT_EQ(
      OkStatus(),
      server_.ProcessPacket(PacketForRpc(
          PacketType::CLIENT_STREAM, OkStatus(), "hello_1", kDefaultCallId)));

  EXPECT_STREQ(span_as_cstr(data_1), kMessage1);
}

TEST_F(BidiMethod, Cancel_ClosesServerWriter) {
  EXPECT_EQ(OkStatus(), server_.ProcessPacket(EncodeCancel()));

  EXPECT_FALSE(responder_.active());
}

TEST_F(BidiMethod, Cancel_SendsNoResponse) {
  EXPECT_EQ(OkStatus(), server_.ProcessPacket(EncodeCancel()));

  EXPECT_EQ(output_.total_packets(), 0u);
}

TEST_F(BidiMethod, ClientError_ClosesServerWriterWithoutResponse) {
  ASSERT_EQ(OkStatus(),
            server_.ProcessPacket(PacketForRpc(PacketType::CLIENT_ERROR)));

  EXPECT_FALSE(responder_.active());
  EXPECT_EQ(output_.total_packets(), 0u);
}

TEST_F(BidiMethod, ClientError_CallsOnErrorCallback) {
  Status status = Status::Unknown();
  responder_.set_on_error([&status](Status error) { status = error; });

  ASSERT_EQ(OkStatus(),
            server_.ProcessPacket(PacketForRpc(PacketType::CLIENT_ERROR,
                                               Status::Unauthenticated())));

  EXPECT_EQ(status, Status::Unauthenticated());
}

TEST_F(BidiMethod, Cancel_CallsOnErrorCallback) {
  Status status = Status::Unknown();
  responder_.set_on_error([&status](Status error) { status = error; });

  ASSERT_EQ(OkStatus(), server_.ProcessPacket(EncodeCancel()));
  EXPECT_EQ(status, Status::Cancelled());
}

TEST_F(BidiMethod, Cancel_IncorrectChannel_SendsNothing) {
  EXPECT_EQ(OkStatus(), server_.ProcessPacket(EncodeCancel(2, 42, 100)));

  EXPECT_EQ(output_.total_packets(), 0u);
  EXPECT_TRUE(responder_.active());
}

TEST_F(BidiMethod, Cancel_IncorrectService_SendsNothing) {
  EXPECT_EQ(OkStatus(), server_.ProcessPacket(EncodeCancel(1, 43, 100)));
  EXPECT_EQ(output_.total_packets(), 0u);
  EXPECT_TRUE(responder_.active());
}

TEST_F(BidiMethod, Cancel_IncorrectMethod_SendsNothing) {
  EXPECT_EQ(OkStatus(), server_.ProcessPacket(EncodeCancel(1, 42, 101)));
  EXPECT_EQ(output_.total_packets(), 0u);
  EXPECT_TRUE(responder_.active());
}

TEST_F(BidiMethod, ClientStream_CallsCallback) {
  ConstByteSpan data = as_bytes(span("?"));
  responder_.set_on_next([&data](ConstByteSpan payload) { data = payload; });

  ASSERT_EQ(OkStatus(),
            server_.ProcessPacket(
                PacketForRpc(PacketType::CLIENT_STREAM, {}, "hello")));

  EXPECT_EQ(output_.total_packets(), 0u);
  EXPECT_STREQ(span_as_cstr(data), "hello");
}

TEST_F(BidiMethod, ClientStream_CallsCallbackOnCallWithOpenId) {
  ConstByteSpan data = as_bytes(span("?"));
  responder_.set_on_next([&data](ConstByteSpan payload) { data = payload; });

  ASSERT_EQ(
      OkStatus(),
      server_.ProcessPacket(PacketForRpc(
          PacketType::CLIENT_STREAM, {}, "hello", internal::kOpenCallId)));

  EXPECT_EQ(output_.total_packets(), 0u);
  EXPECT_STREQ(span_as_cstr(data), "hello");
}

TEST_F(BidiMethod, ClientStream_CallsOpenIdOnCallWithDifferentId) {
  const uint32_t kSecondCallId = 1625;
  internal::CallContext context(server_,
                                channels_[0].id(),
                                service_42_,
                                service_42_.method(100),
                                internal::kOpenCallId);
  internal::rpc_lock().lock();
  auto temp_responder =
      internal::test::FakeServerReaderWriter(context.ClaimLocked());
  internal::rpc_lock().unlock();
  responder_ = std::move(temp_responder);

  ConstByteSpan data = as_bytes(span("?"));
  responder_.set_on_next([&data](ConstByteSpan payload) { data = payload; });

  ASSERT_EQ(OkStatus(),
            server_.ProcessPacket(PacketForRpc(
                PacketType::CLIENT_STREAM, {}, "hello", kSecondCallId)));

  EXPECT_EQ(output_.total_packets(), 0u);
  EXPECT_STREQ(span_as_cstr(data), "hello");

  internal::LockGuard lock(internal::rpc_lock());
  EXPECT_EQ(responder_.as_server_call().id(), kSecondCallId);
}

TEST_F(BidiMethod, UnregsiterService_AbortsActiveCalls) {
  ASSERT_TRUE(responder_.active());

  Status on_error_status = OkStatus();
  responder_.set_on_error(
      [&on_error_status](Status status) { on_error_status = status; });

  server_.UnregisterService(service_42_);

  EXPECT_FALSE(responder_.active());
  EXPECT_EQ(Status::Aborted(), on_error_status);
}

#if PW_RPC_CLIENT_STREAM_END_CALLBACK

TEST_F(BidiMethod, ClientStreamEnd_CallsCallback) {
  bool called = false;
  responder_.set_on_client_stream_end([&called]() { called = true; });

  ASSERT_EQ(OkStatus(),
            server_.ProcessPacket(PacketForRpc(PacketType::CLIENT_STREAM_END)));

  EXPECT_EQ(output_.total_packets(), 0u);
  EXPECT_TRUE(called);
}

TEST_F(BidiMethod, ClientStreamEnd_ErrorWhenClosed) {
  const auto end = PacketForRpc(PacketType::CLIENT_STREAM_END);
  ASSERT_EQ(OkStatus(), server_.ProcessPacket(end));

  bool called = false;
  responder_.set_on_client_stream_end([&called]() { called = true; });

  ASSERT_EQ(OkStatus(), server_.ProcessPacket(end));

  ASSERT_EQ(output_.total_packets(), 1u);
  const Packet& packet =
      static_cast<internal::test::FakeChannelOutput&>(output_).last_packet();
  EXPECT_EQ(packet.type(), PacketType::SERVER_ERROR);
  EXPECT_EQ(packet.status(), Status::FailedPrecondition());
}

#endif  // PW_RPC_CLIENT_STREAM_END_CALLBACK

class ServerStreamingMethod : public BasicServer {
 protected:
  ServerStreamingMethod() {
    internal::CallContext context(server_,
                                  channels_[0].id(),
                                  service_42_,
                                  service_42_.method(100),
                                  kDefaultCallId);
    internal::rpc_lock().lock();
    internal::test::FakeServerWriter responder_temp(context.ClaimLocked());
    internal::rpc_lock().unlock();
    responder_ = std::move(responder_temp);
    PW_CHECK(responder_.active());
  }

  internal::test::FakeServerWriter responder_;
};

TEST_F(ServerStreamingMethod, ClientStream_InvalidArgumentError) {
  ASSERT_EQ(OkStatus(),
            server_.ProcessPacket(PacketForRpc(PacketType::CLIENT_STREAM)));

  ASSERT_EQ(output_.total_packets(), 1u);
  const Packet& packet =
      static_cast<internal::test::FakeChannelOutput&>(output_).last_packet();
  EXPECT_EQ(packet.type(), PacketType::SERVER_ERROR);
  EXPECT_EQ(packet.status(), Status::InvalidArgument());
}

TEST_F(ServerStreamingMethod, ClientStreamEnd_InvalidArgumentError) {
  ASSERT_EQ(OkStatus(),
            server_.ProcessPacket(PacketForRpc(PacketType::CLIENT_STREAM_END)));

  ASSERT_EQ(output_.total_packets(), 1u);
  const Packet& packet =
      static_cast<internal::test::FakeChannelOutput&>(output_).last_packet();
  EXPECT_EQ(packet.type(), PacketType::SERVER_ERROR);
  EXPECT_EQ(packet.status(), Status::InvalidArgument());
}

}  // namespace
}  // namespace pw::rpc
