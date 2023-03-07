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

#include "pw_rpc/internal/method.h"

#include <array>

#include "gtest/gtest.h"
#include "pw_rpc/internal/packet.h"
#include "pw_rpc/internal/test_method.h"
#include "pw_rpc/method_type.h"
#include "pw_rpc/server.h"

namespace pw::rpc::internal {
namespace {

// Test the helper functions for the MethodType enum.
static_assert(!HasServerStream(MethodType::kUnary));
static_assert(HasServerStream(MethodType::kServerStreaming));
static_assert(!HasServerStream(MethodType::kClientStreaming));
static_assert(HasServerStream(MethodType::kBidirectionalStreaming));

static_assert(!HasClientStream(MethodType::kUnary));
static_assert(!HasClientStream(MethodType::kServerStreaming));
static_assert(HasClientStream(MethodType::kClientStreaming));
static_assert(HasClientStream(MethodType::kBidirectionalStreaming));

class TestService : public Service {
 public:
  TestService() : Service(5678, kMethods) {}

  static constexpr std::array<TestMethodUnion, 1> kMethods = {TestMethod(1234)};
};

const TestMethod& kTestMethod = TestService::kMethods.front().test_method();

TEST(Method, Id) { EXPECT_EQ(kTestMethod.id(), 1234u); }

TEST(Method, Invoke) {
  class NullChannelOutput final : public ChannelOutput {
   public:
    constexpr NullChannelOutput() : ChannelOutput("NullChannelOutput") {}

    Status Send(ConstByteSpan) override { return OkStatus(); }
  } channel_output;

  Channel channel(123, &channel_output);
  Server server(span(static_cast<rpc::Channel*>(&channel), 1));
  TestService service;

  const CallContext context(server, channel.id(), service, kTestMethod, 0);
  Packet empty_packet;

  EXPECT_EQ(kTestMethod.invocations(), 0u);
  rpc_lock().lock();
  kTestMethod.Invoke(context, empty_packet);
  EXPECT_EQ(kTestMethod.invocations(), 1u);
}

}  // namespace
}  // namespace pw::rpc::internal
