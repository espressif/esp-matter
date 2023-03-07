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

#include "pw_rpc/pwpb/fake_channel_output.h"

#include <array>
#include <cstddef>
#include <memory>

#include "gtest/gtest.h"
#include "pw_rpc/internal/channel.h"
#include "pw_rpc/internal/packet.h"
#include "pw_rpc_test_protos/test.rpc.pwpb.h"

PW_MODIFY_DIAGNOSTICS_PUSH();
PW_MODIFY_DIAGNOSTIC(ignored, "-Wmissing-field-initializers");

namespace pw::rpc::internal::test {
namespace {

using ::pw::rpc::internal::pwpb::PacketType;
using ::pw::rpc::test::pw_rpc::pwpb::TestService;
using Info = ::pw::rpc::internal::MethodInfo<TestService::TestUnaryRpc>;

TEST(PwpbFakeChannelOutput, Requests) {
  PwpbFakeChannelOutput<1> output;

  std::byte payload_buffer[32] = {};
  constexpr Info::Request request{.integer = -100, .status_code = 5};
  const StatusWithSize payload =
      Info::serde().EncodeRequest(request, payload_buffer);
  ASSERT_TRUE(payload.ok());

  std::array<std::byte, 128> buffer;

  auto packet = Packet(PacketType::REQUEST,
                       1,
                       Info::kServiceId,
                       Info::kMethodId,
                       999,
                       span(payload_buffer, payload.size()))
                    .Encode(buffer);
  ASSERT_TRUE(packet.ok());

  ASSERT_EQ(OkStatus(), output.Send(span(buffer).first(packet->size())));

  ASSERT_TRUE(output.responses<TestService::TestUnaryRpc>().empty());
  ASSERT_EQ(output.requests<TestService::TestUnaryRpc>().size(), 1u);

  Info::Request sent = output.requests<TestService::TestUnaryRpc>().front();
  EXPECT_EQ(sent.integer, -100);
  EXPECT_EQ(sent.status_code, 5u);
}

TEST(PwpbFakeChannelOutput, Responses) {
  PwpbFakeChannelOutput<1> output;

  std::byte payload_buffer[32] = {};
  const Info::Response response{.value = -9876};
  const StatusWithSize payload =
      Info::serde().EncodeResponse(response, payload_buffer);
  ASSERT_TRUE(payload.ok());

  std::array<std::byte, 128> buffer;

  auto packet = Packet(PacketType::RESPONSE,
                       1,
                       Info::kServiceId,
                       Info::kMethodId,
                       999,
                       span(payload_buffer, payload.size()))
                    .Encode(buffer);
  ASSERT_TRUE(packet.ok());

  ASSERT_EQ(OkStatus(), output.Send(span(buffer).first(packet->size())));

  ASSERT_EQ(output.responses<TestService::TestUnaryRpc>().size(), 1u);
  ASSERT_TRUE(output.requests<TestService::TestUnaryRpc>().empty());

  Info::Response sent = output.responses<TestService::TestUnaryRpc>().front();
  EXPECT_EQ(sent.value, -9876);
}

}  // namespace
}  // namespace pw::rpc::internal::test

PW_MODIFY_DIAGNOSTICS_POP();
