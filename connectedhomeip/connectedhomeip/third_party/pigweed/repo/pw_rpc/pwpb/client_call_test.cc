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

#include <optional>

#include "gtest/gtest.h"
#include "pw_rpc/internal/test_utils.h"
#include "pw_rpc/pwpb/client_reader_writer.h"
#include "pw_rpc_pwpb_private/internal_test_utils.h"
#include "pw_rpc_test_protos/test.pwpb.h"

PW_MODIFY_DIAGNOSTICS_PUSH();
PW_MODIFY_DIAGNOSTIC(ignored, "-Wmissing-field-initializers");

namespace pw::rpc {
namespace {

using internal::ClientContextForTest;
using internal::pwpb::PacketType;

namespace TestRequest = ::pw::rpc::test::pwpb::TestRequest;
namespace TestResponse = ::pw::rpc::test::pwpb::TestResponse;
namespace TestStreamResponse = ::pw::rpc::test::pwpb::TestStreamResponse;

constexpr uint32_t kServiceId = 16;
constexpr uint32_t kUnaryMethodId = 111;
constexpr uint32_t kServerStreamingMethodId = 112;

class FakeGeneratedServiceClient {
 public:
  static PwpbUnaryReceiver<TestResponse::Message> TestUnaryRpc(
      Client& client,
      uint32_t channel_id,
      const TestRequest::Message& request,
      Function<void(const TestResponse::Message&, Status)> on_response,
      Function<void(Status)> on_error = nullptr) {
    return internal::PwpbUnaryResponseClientCall<TestResponse::Message>::Start<
        PwpbUnaryReceiver<TestResponse::Message>>(
        client,
        channel_id,
        kServiceId,
        kUnaryMethodId,
        internal::kPwpbMethodSerde<&TestRequest::kMessageFields,
                                   &TestResponse::kMessageFields>,
        std::move(on_response),
        std::move(on_error),
        request);
  }

  static PwpbUnaryReceiver<TestResponse::Message> TestAnotherUnaryRpc(
      Client& client,
      uint32_t channel_id,
      const TestRequest::Message& request,
      Function<void(const TestResponse::Message&, Status)> on_response,
      Function<void(Status)> on_error = nullptr) {
    return internal::PwpbUnaryResponseClientCall<TestResponse::Message>::Start<
        PwpbUnaryReceiver<TestResponse::Message>>(
        client,
        channel_id,
        kServiceId,
        kUnaryMethodId,
        internal::kPwpbMethodSerde<&TestRequest::kMessageFields,
                                   &TestResponse::kMessageFields>,
        std::move(on_response),
        std::move(on_error),
        request);
  }

  static PwpbClientReader<TestStreamResponse::Message> TestServerStreamRpc(
      Client& client,
      uint32_t channel_id,
      const TestRequest::Message& request,
      Function<void(const TestStreamResponse::Message&)> on_response,
      Function<void(Status)> on_stream_end,
      Function<void(Status)> on_error = nullptr) {
    return internal::PwpbStreamResponseClientCall<TestStreamResponse::Message>::
        Start<PwpbClientReader<TestStreamResponse::Message>>(
            client,
            channel_id,
            kServiceId,
            kServerStreamingMethodId,
            internal::kPwpbMethodSerde<&TestRequest::kMessageFields,
                                       &TestStreamResponse::kMessageFields>,
            std::move(on_response),
            std::move(on_stream_end),
            std::move(on_error),
            request);
  }
};

TEST(PwpbClientCall, Unary_SendsRequestPacket) {
  ClientContextForTest context;

  auto call = FakeGeneratedServiceClient::TestUnaryRpc(
      context.client(),
      context.channel().id(),
      {.integer = 123, .status_code = 0},
      nullptr);

  EXPECT_EQ(context.output().total_packets(), 1u);
  auto packet = context.output().last_packet();
  EXPECT_EQ(packet.channel_id(), context.channel().id());
  EXPECT_EQ(packet.service_id(), kServiceId);
  EXPECT_EQ(packet.method_id(), kUnaryMethodId);

  PW_DECODE_PB(TestRequest, sent_proto, packet.payload());
  EXPECT_EQ(sent_proto.integer, 123);
}

class UnaryClientCall : public ::testing::Test {
 protected:
  std::optional<Status> last_status_;
  std::optional<Status> last_error_;
  int responses_received_ = 0;
  int last_response_value_ = 0;
};

TEST_F(UnaryClientCall, InvokesCallbackOnValidResponse) {
  ClientContextForTest context;

  auto call = FakeGeneratedServiceClient::TestUnaryRpc(
      context.client(),
      context.channel().id(),
      {.integer = 123, .status_code = 0},
      [this](const TestResponse::Message& response, Status status) {
        ++responses_received_;
        last_status_ = status;
        last_response_value_ = response.value;
      });

  PW_ENCODE_PB(TestResponse, response, .value = 42);
  EXPECT_EQ(OkStatus(), context.SendResponse(OkStatus(), response));

  ASSERT_EQ(responses_received_, 1);
  EXPECT_EQ(last_status_, OkStatus());
  EXPECT_EQ(last_response_value_, 42);
}

TEST_F(UnaryClientCall, DoesNothingOnNullCallback) {
  ClientContextForTest context;

  auto call = FakeGeneratedServiceClient::TestUnaryRpc(
      context.client(),
      context.channel().id(),
      {.integer = 123, .status_code = 0},
      nullptr);

  PW_ENCODE_PB(TestResponse, response, .value = 42);
  EXPECT_EQ(OkStatus(), context.SendResponse(OkStatus(), response));

  ASSERT_EQ(responses_received_, 0);
}

TEST_F(UnaryClientCall, InvokesErrorCallbackOnInvalidResponse) {
  ClientContextForTest context;

  auto call = FakeGeneratedServiceClient::TestUnaryRpc(
      context.client(),
      context.channel().id(),
      {.integer = 123, .status_code = 0},
      [this](const TestResponse::Message& response, Status status) {
        ++responses_received_;
        last_status_ = status;
        last_response_value_ = response.value;
      },
      [this](Status status) { last_error_ = status; });

  constexpr std::byte bad_payload[]{
      std::byte{0xab}, std::byte{0xcd}, std::byte{0xef}};
  EXPECT_EQ(OkStatus(), context.SendResponse(OkStatus(), bad_payload));

  EXPECT_EQ(responses_received_, 0);
  ASSERT_TRUE(last_error_.has_value());
  EXPECT_EQ(last_error_, Status::DataLoss());
}

TEST_F(UnaryClientCall, InvokesErrorCallbackOnServerError) {
  ClientContextForTest context;

  auto call = FakeGeneratedServiceClient::TestUnaryRpc(
      context.client(),
      context.channel().id(),
      {.integer = 123, .status_code = 0},
      [this](const TestResponse::Message& response, Status status) {
        ++responses_received_;
        last_status_ = status;
        last_response_value_ = response.value;
      },
      [this](Status status) { last_error_ = status; });

  EXPECT_EQ(OkStatus(),
            context.SendPacket(PacketType::SERVER_ERROR, Status::NotFound()));

  EXPECT_EQ(responses_received_, 0);
  EXPECT_EQ(last_error_, Status::NotFound());
}

TEST_F(UnaryClientCall, DoesNothingOnErrorWithoutCallback) {
  ClientContextForTest context;

  auto call = FakeGeneratedServiceClient::TestUnaryRpc(
      context.client(),
      context.channel().id(),
      {.integer = 123, .status_code = 0},
      [this](const TestResponse::Message& response, Status status) {
        ++responses_received_;
        last_status_ = status;
        last_response_value_ = response.value;
      });

  constexpr std::byte bad_payload[]{
      std::byte{0xab}, std::byte{0xcd}, std::byte{0xef}};
  EXPECT_EQ(OkStatus(), context.SendResponse(OkStatus(), bad_payload));

  EXPECT_EQ(responses_received_, 0);
}

TEST_F(UnaryClientCall, OnlyReceivesOneResponse) {
  ClientContextForTest context;

  auto call = FakeGeneratedServiceClient::TestUnaryRpc(
      context.client(),
      context.channel().id(),
      {.integer = 123, .status_code = 0},
      [this](const TestResponse::Message& response, Status status) {
        ++responses_received_;
        last_status_ = status;
        last_response_value_ = response.value;
      });

  PW_ENCODE_PB(TestResponse, r1, .value = 42);
  EXPECT_EQ(OkStatus(), context.SendResponse(Status::Unimplemented(), r1));
  PW_ENCODE_PB(TestResponse, r2, .value = 44);
  EXPECT_EQ(OkStatus(), context.SendResponse(Status::OutOfRange(), r2));
  PW_ENCODE_PB(TestResponse, r3, .value = 46);
  EXPECT_EQ(OkStatus(), context.SendResponse(Status::Internal(), r3));

  EXPECT_EQ(responses_received_, 1);
  EXPECT_EQ(last_status_, Status::Unimplemented());
  EXPECT_EQ(last_response_value_, 42);
}

class ServerStreamingClientCall : public ::testing::Test {
 protected:
  bool active_ = true;
  std::optional<Status> stream_status_;
  std::optional<Status> rpc_error_;
  int responses_received_ = 0;
  int last_response_number_ = 0;
};

TEST_F(ServerStreamingClientCall, SendsRequestPacket) {
  ClientContextForTest<128, 99, kServiceId, kServerStreamingMethodId> context;

  auto call = FakeGeneratedServiceClient::TestServerStreamRpc(
      context.client(),
      context.channel().id(),
      {.integer = 71, .status_code = 0},
      nullptr,
      nullptr);

  EXPECT_EQ(context.output().total_packets(), 1u);
  auto packet = context.output().last_packet();
  EXPECT_EQ(packet.channel_id(), context.channel().id());
  EXPECT_EQ(packet.service_id(), kServiceId);
  EXPECT_EQ(packet.method_id(), kServerStreamingMethodId);

  PW_DECODE_PB(TestRequest, sent_proto, packet.payload());
  EXPECT_EQ(sent_proto.integer, 71);
}

TEST_F(ServerStreamingClientCall, InvokesCallbackOnValidResponse) {
  ClientContextForTest<128, 99, kServiceId, kServerStreamingMethodId> context;

  auto call = FakeGeneratedServiceClient::TestServerStreamRpc(
      context.client(),
      context.channel().id(),
      {.integer = 71, .status_code = 0},
      [this](const TestStreamResponse::Message& response) {
        ++responses_received_;
        last_response_number_ = response.number;
      },
      [this](Status status) {
        active_ = false;
        stream_status_ = status;
      });

  PW_ENCODE_PB(TestStreamResponse, r1, .chunk = {}, .number = 11u);
  EXPECT_EQ(OkStatus(), context.SendServerStream(r1));
  EXPECT_TRUE(active_);
  EXPECT_EQ(responses_received_, 1);
  EXPECT_EQ(last_response_number_, 11);

  PW_ENCODE_PB(TestStreamResponse, r2, .chunk = {}, .number = 22u);
  EXPECT_EQ(OkStatus(), context.SendServerStream(r2));
  EXPECT_TRUE(active_);
  EXPECT_EQ(responses_received_, 2);
  EXPECT_EQ(last_response_number_, 22);

  PW_ENCODE_PB(TestStreamResponse, r3, .chunk = {}, .number = 33u);
  EXPECT_EQ(OkStatus(), context.SendServerStream(r3));
  EXPECT_TRUE(active_);
  EXPECT_EQ(responses_received_, 3);
  EXPECT_EQ(last_response_number_, 33);
}

TEST_F(ServerStreamingClientCall, InvokesStreamEndOnFinish) {
  ClientContextForTest<128, 99, kServiceId, kServerStreamingMethodId> context;

  auto call = FakeGeneratedServiceClient::TestServerStreamRpc(
      context.client(),
      context.channel().id(),
      {.integer = 71, .status_code = 0},
      [this](const TestStreamResponse::Message& response) {
        ++responses_received_;
        last_response_number_ = response.number;
      },
      [this](Status status) {
        active_ = false;
        stream_status_ = status;
      });

  PW_ENCODE_PB(TestStreamResponse, r1, .chunk = {}, .number = 11u);
  EXPECT_EQ(OkStatus(), context.SendServerStream(r1));
  EXPECT_TRUE(active_);

  PW_ENCODE_PB(TestStreamResponse, r2, .chunk = {}, .number = 22u);
  EXPECT_EQ(OkStatus(), context.SendServerStream(r2));
  EXPECT_TRUE(active_);

  // Close the stream.
  EXPECT_EQ(OkStatus(), context.SendResponse(Status::NotFound()));

  PW_ENCODE_PB(TestStreamResponse, r3, .chunk = {}, .number = 33u);
  EXPECT_EQ(OkStatus(), context.SendServerStream(r3));
  EXPECT_FALSE(active_);

  EXPECT_EQ(responses_received_, 2);
}

TEST_F(ServerStreamingClientCall, InvokesErrorCallbackOnInvalidResponses) {
  ClientContextForTest<128, 99, kServiceId, kServerStreamingMethodId> context;

  auto call = FakeGeneratedServiceClient::TestServerStreamRpc(
      context.client(),
      context.channel().id(),
      {.integer = 71, .status_code = 0},
      [this](const TestStreamResponse::Message& response) {
        ++responses_received_;
        last_response_number_ = response.number;
      },
      nullptr,
      [this](Status error) { rpc_error_ = error; });

  PW_ENCODE_PB(TestStreamResponse, r1, .chunk = {}, .number = 11u);
  EXPECT_EQ(OkStatus(), context.SendServerStream(r1));
  EXPECT_TRUE(active_);
  EXPECT_EQ(responses_received_, 1);
  EXPECT_EQ(last_response_number_, 11);

  constexpr std::byte bad_payload[]{
      std::byte{0xab}, std::byte{0xcd}, std::byte{0xef}};
  EXPECT_EQ(OkStatus(), context.SendServerStream(bad_payload));
  EXPECT_EQ(responses_received_, 1);
  ASSERT_TRUE(rpc_error_.has_value());
  EXPECT_EQ(rpc_error_, Status::DataLoss());

  PW_ENCODE_PB(TestStreamResponse, r2, .chunk = {}, .number = 22u);
  EXPECT_EQ(OkStatus(), context.SendServerStream(r2));
  EXPECT_TRUE(active_);
  EXPECT_EQ(responses_received_, 2);
  EXPECT_EQ(last_response_number_, 22);

  EXPECT_EQ(OkStatus(),
            context.SendPacket(PacketType::SERVER_ERROR, Status::NotFound()));
  EXPECT_EQ(responses_received_, 2);
  EXPECT_EQ(rpc_error_, Status::NotFound());
}

}  // namespace
}  // namespace pw::rpc

PW_MODIFY_DIAGNOSTICS_POP();
