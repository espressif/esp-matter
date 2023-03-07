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

#include "pw_rpc/client_server.h"

#include "gtest/gtest.h"
#include "pw_rpc/internal/packet.h"
#include "pw_rpc/internal/test_utils.h"
#include "pw_rpc/raw/fake_channel_output.h"
#include "pw_rpc/raw/internal/method_union.h"
#include "pw_rpc/service.h"

namespace pw::rpc::internal {
namespace {

constexpr uint32_t kFakeChannelId = 1;
constexpr uint32_t kFakeServiceId = 3;
constexpr uint32_t kFakeMethodId = 10;

RawFakeChannelOutput<1> output;
rpc::Channel channels[] = {Channel::Create<kFakeChannelId>(&output)};

void FakeMethod(ConstByteSpan, RawUnaryResponder& responder) {
  ASSERT_EQ(OkStatus(), responder.Finish({}, Status::Unimplemented()));
}

class FakeService : public Service {
 public:
  FakeService(uint32_t id) : Service(id, kMethods) {}

  static constexpr std::array<RawMethodUnion, 1> kMethods = {
      RawMethod::AsynchronousUnary<FakeMethod>(kFakeMethodId),
  };
};

FakeService service(kFakeServiceId);

TEST(ClientServer, ProcessPacket_CallsServer) {
  ClientServer client_server(channels);
  client_server.server().RegisterService(service);

  Packet packet(
      pwpb::PacketType::REQUEST, kFakeChannelId, kFakeServiceId, kFakeMethodId);
  std::array<std::byte, 32> buffer;
  Result result = packet.Encode(buffer);
  EXPECT_EQ(result.status(), OkStatus());

  EXPECT_EQ(client_server.ProcessPacket(result.value()), OkStatus());
}

TEST(ClientServer, ProcessPacket_CallsClient) {
  ClientServer client_server(channels);
  client_server.server().RegisterService(service);

  // Same packet as above, but type RESPONSE will skip the server and call into
  // the client.
  Packet packet(pwpb::PacketType::RESPONSE,
                kFakeChannelId,
                kFakeServiceId,
                kFakeMethodId);
  std::array<std::byte, 32> buffer;
  Result result = packet.Encode(buffer);
  EXPECT_EQ(result.status(), OkStatus());

  // No calls are registered on the client, so nothing should happen. The
  // ProcessPacket call still returns OK since the client handled it.
  EXPECT_EQ(client_server.ProcessPacket(result.value()), OkStatus());
}

TEST(ClientServer, ProcessPacket_BadData) {
  ClientServer client_server(channels);
  EXPECT_EQ(client_server.ProcessPacket({}), Status::DataLoss());
}

}  // namespace
}  // namespace pw::rpc::internal
