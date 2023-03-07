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

#include "pw_rpc/synchronous_call.h"

#include <chrono>

#include "gtest/gtest.h"
#include "pw_chrono/system_clock.h"
#include "pw_rpc/channel.h"
#include "pw_rpc/internal/packet.h"
#include "pw_rpc/pwpb/fake_channel_output.h"
#include "pw_rpc_test_protos/test.rpc.pwpb.h"
#include "pw_status/status.h"
#include "pw_status/status_with_size.h"
#include "pw_thread/thread.h"
#include "pw_work_queue/test_thread.h"
#include "pw_work_queue/work_queue.h"

namespace pw::rpc::test {
namespace {

using pw::rpc::test::pw_rpc::pwpb::TestService;
using MethodInfo = internal::MethodInfo<TestService::TestUnaryRpc>;

namespace TestRequest = ::pw::rpc::test::pwpb::TestRequest;
namespace TestResponse = ::pw::rpc::test::pwpb::TestResponse;

class SynchronousCallTest : public ::testing::Test {
 public:
  SynchronousCallTest()
      : channels_({{Channel::Create<42>(&fake_output_)}}), client_(channels_) {}

  void SetUp() override {
    work_thread_ =
        thread::Thread(work_queue::test::WorkQueueThreadOptions(), work_queue_);
  }

  void TearDown() override {
    work_queue_.RequestStop();
    work_thread_.join();
  }

 protected:
  using FakeChannelOutput = PwpbFakeChannelOutput<2>;

  void OnSend(span<const std::byte> buffer, Status status) {
    if (!status.ok()) {
      return;
    }
    auto result = internal::Packet::FromBuffer(buffer);
    EXPECT_TRUE(result.ok());
    request_packet_ = *result;

    EXPECT_TRUE(work_queue_.PushWork([this]() { SendResponse(); }).ok());
  }

  void SendResponse() {
    std::array<std::byte, 256> buffer;
    std::array<std::byte, 32> payload_buffer;

    StatusWithSize size_status =
        MethodInfo::serde().EncodeResponse(response_, payload_buffer);
    EXPECT_TRUE(size_status.ok());

    auto response =
        internal::Packet::Response(request_packet_, response_status_);
    response.set_payload({payload_buffer.data(), size_status.size()});
    EXPECT_TRUE(client_.ProcessPacket(response.Encode(buffer).value()).ok());
  }

  void set_response(const TestResponse::Message& response,
                    Status response_status = OkStatus()) {
    response_ = response;
    response_status_ = response_status;
    output().set_on_send([this](span<const std::byte> buffer, Status status) {
      OnSend(buffer, status);
    });
  }

  MethodInfo::GeneratedClient generated_client() {
    return MethodInfo::GeneratedClient(client(), channel().id());
  }

  FakeChannelOutput& output() { return fake_output_; }
  const Channel& channel() const { return channels_.front(); }
  Client& client() { return client_; }

 private:
  FakeChannelOutput fake_output_;
  std::array<Channel, 1> channels_;
  Client client_;
  thread::Thread work_thread_;
  work_queue::WorkQueueWithBuffer<1> work_queue_;
  TestResponse::Message response_{};
  Status response_status_ = OkStatus();
  internal::Packet request_packet_;
};

TEST_F(SynchronousCallTest, SynchronousCallSuccess) {
  TestRequest::Message request{.integer = 5, .status_code = 0};
  TestResponse::Message response{.value = 42, .repeated_field{}};

  set_response(response, OkStatus());

  auto result = SynchronousCall<TestService::TestUnaryRpc>(
      client(), channel().id(), request);
  EXPECT_TRUE(result.ok());
  EXPECT_EQ(result.response().value, 42);
}

TEST_F(SynchronousCallTest, SynchronousCallServerError) {
  TestRequest::Message request{.integer = 5, .status_code = 0};
  TestResponse::Message response{.value = 42, .repeated_field{}};

  set_response(response, Status::Internal());

  auto result = SynchronousCall<TestService::TestUnaryRpc>(
      client(), channel().id(), request);
  EXPECT_TRUE(result.is_error());
  EXPECT_EQ(result.status(), Status::Internal());

  // We should still receive the response
  EXPECT_TRUE(result.is_server_response());
  EXPECT_EQ(result.response().value, 42);
}

TEST_F(SynchronousCallTest, SynchronousCallRpcError) {
  TestRequest::Message request{.integer = 5, .status_code = 0};

  // Internally, if Channel receives a non-ok status from the
  // ChannelOutput::Send, it will always return Unknown.
  output().set_send_status(Status::Unknown());

  auto result = SynchronousCall<TestService::TestUnaryRpc>(
      client(), channel().id(), request);
  EXPECT_TRUE(result.is_rpc_error());
  EXPECT_EQ(result.status(), Status::Unknown());
}

TEST_F(SynchronousCallTest, SynchronousCallForTimeoutError) {
  TestRequest::Message request{.integer = 5, .status_code = 0};

  auto result = SynchronousCallFor<TestService::TestUnaryRpc>(
      client(),
      channel().id(),
      request,
      chrono::SystemClock::for_at_least(std::chrono::milliseconds(1)));

  EXPECT_TRUE(result.is_timeout());
  EXPECT_EQ(result.status(), Status::DeadlineExceeded());
}

TEST_F(SynchronousCallTest, SynchronousCallUntilTimeoutError) {
  TestRequest::Message request{.integer = 5, .status_code = 0};

  auto result = SynchronousCallUntil<TestService::TestUnaryRpc>(
      client(), channel().id(), request, chrono::SystemClock::now());

  EXPECT_TRUE(result.is_timeout());
  EXPECT_EQ(result.status(), Status::DeadlineExceeded());
}

TEST_F(SynchronousCallTest, GeneratedClientSynchronousCallSuccess) {
  TestRequest::Message request{.integer = 5, .status_code = 0};
  TestResponse::Message response{.value = 42, .repeated_field{}};

  set_response(response, OkStatus());

  auto result =
      SynchronousCall<TestService::TestUnaryRpc>(generated_client(), request);
  EXPECT_TRUE(result.ok());
  EXPECT_EQ(result.response().value, 42);
}

TEST_F(SynchronousCallTest, GeneratedClientSynchronousCallServerError) {
  TestRequest::Message request{.integer = 5, .status_code = 0};
  TestResponse::Message response{.value = 42, .repeated_field{}};

  set_response(response, Status::Internal());

  auto result =
      SynchronousCall<TestService::TestUnaryRpc>(generated_client(), request);
  EXPECT_TRUE(result.is_error());
  EXPECT_EQ(result.status(), Status::Internal());

  // We should still receive the response
  EXPECT_TRUE(result.is_server_response());
  EXPECT_EQ(result.response().value, 42);
}

TEST_F(SynchronousCallTest, GeneratedClientSynchronousCallRpcError) {
  TestRequest::Message request{.integer = 5, .status_code = 0};

  // Internally, if Channel receives a non-ok status from the
  // ChannelOutput::Send, it will always return Unknown.
  output().set_send_status(Status::Unknown());

  auto result =
      SynchronousCall<TestService::TestUnaryRpc>(generated_client(), request);
  EXPECT_TRUE(result.is_rpc_error());
  EXPECT_EQ(result.status(), Status::Unknown());
}

TEST_F(SynchronousCallTest, GeneratedClientSynchronousCallForTimeoutError) {
  TestRequest::Message request{.integer = 5, .status_code = 0};

  auto result = SynchronousCallFor<TestService::TestUnaryRpc>(
      generated_client(),
      request,
      chrono::SystemClock::for_at_least(std::chrono::milliseconds(1)));

  EXPECT_TRUE(result.is_timeout());
  EXPECT_EQ(result.status(), Status::DeadlineExceeded());
}

TEST_F(SynchronousCallTest, GeneratedClientSynchronousCallUntilTimeoutError) {
  TestRequest::Message request{.integer = 5, .status_code = 0};

  auto result = SynchronousCallUntil<TestService::TestUnaryRpc>(
      generated_client(), request, chrono::SystemClock::now());

  EXPECT_TRUE(result.is_timeout());
  EXPECT_EQ(result.status(), Status::DeadlineExceeded());
}
}  // namespace
}  // namespace pw::rpc::test
