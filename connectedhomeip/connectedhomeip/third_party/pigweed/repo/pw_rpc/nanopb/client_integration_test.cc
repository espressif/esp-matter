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

#include "gtest/gtest.h"
#include "pw_assert/check.h"
#include "pw_rpc/benchmark.rpc.pb.h"
#include "pw_rpc/integration_testing.h"
#include "pw_sync/binary_semaphore.h"

namespace nanopb_rpc_test {
namespace {

using namespace std::chrono_literals;
using pw::ByteSpan;
using pw::ConstByteSpan;
using pw::Function;
using pw::OkStatus;
using pw::Status;

using pw::rpc::pw_rpc::nanopb::Benchmark;

constexpr int kIterations = 10;

class PayloadReceiver {
 public:
  const char* Wait() {
    PW_CHECK(sem_.try_acquire_for(1500ms));
    return reinterpret_cast<const char*>(payload_.payload.bytes);
  }

  Function<void(const pw_rpc_Payload&, Status)> UnaryOnCompleted() {
    return [this](const pw_rpc_Payload& data, Status) { CopyPayload(data); };
  }

  Function<void(const pw_rpc_Payload&)> OnNext() {
    return [this](const pw_rpc_Payload& data) { CopyPayload(data); };
  }

 private:
  void CopyPayload(const pw_rpc_Payload& data) {
    payload_ = data;
    sem_.release();
  }

  pw::sync::BinarySemaphore sem_;
  pw_rpc_Payload payload_ = {};
};

template <size_t kSize>
pw_rpc_Payload Payload(const char (&string)[kSize]) {
  static_assert(kSize <= sizeof(pw_rpc_Payload::payload));
  pw_rpc_Payload payload{};
  std::memcpy(payload.payload.bytes, string, kSize);
  payload.payload.size = kSize;
  return payload;
}

const Benchmark::Client kClient(pw::rpc::integration_test::client(),
                                pw::rpc::integration_test::kChannelId);

TEST(NanopbRpcIntegrationTest, Unary) {
  char value[] = {"hello, world!"};

  for (int i = 0; i < kIterations; ++i) {
    PayloadReceiver receiver;

    value[0] = static_cast<char>(i);
    pw::rpc::NanopbUnaryReceiver call =
        kClient.UnaryEcho(Payload(value), receiver.UnaryOnCompleted());
    ASSERT_STREQ(receiver.Wait(), value);
  }
}

TEST(NanopbRpcIntegrationTest, Unary_ReuseCall) {
  pw::rpc::NanopbUnaryReceiver<pw_rpc_Payload> call;
  char value[] = {"O_o "};

  for (int i = 0; i < kIterations; ++i) {
    PayloadReceiver receiver;

    value[sizeof(value) - 2] = static_cast<char>(i);
    call = kClient.UnaryEcho(Payload(value), receiver.UnaryOnCompleted());
    ASSERT_STREQ(receiver.Wait(), value);
  }
}

TEST(NanopbRpcIntegrationTest, Unary_DiscardCalls) {
  constexpr int iterations = PW_RPC_USE_GLOBAL_MUTEX ? 10000 : 1;
  for (int i = 0; i < iterations; ++i) {
    kClient.UnaryEcho(Payload("O_o"));
  }
}

TEST(NanopbRpcIntegrationTest, BidirectionalStreaming_MoveCalls) {
  for (int i = 0; i < kIterations; ++i) {
    PayloadReceiver receiver;
    pw::rpc::NanopbClientReaderWriter call =
        kClient.BidirectionalEcho(receiver.OnNext());

    ASSERT_EQ(OkStatus(), call.Write(Payload("Yello")));
    ASSERT_STREQ(receiver.Wait(), "Yello");

    pw::rpc::NanopbClientReaderWriter<pw_rpc_Payload, pw_rpc_Payload> new_call =
        std::move(call);

    EXPECT_EQ(Status::FailedPrecondition(), call.Write(Payload("Dello")));

    ASSERT_EQ(OkStatus(), new_call.Write(Payload("Dello")));
    ASSERT_STREQ(receiver.Wait(), "Dello");

    call = std::move(new_call);

    EXPECT_EQ(Status::FailedPrecondition(), new_call.Write(Payload("Dello")));

    ASSERT_EQ(OkStatus(), call.Write(Payload("???")));
    ASSERT_STREQ(receiver.Wait(), "???");

    EXPECT_EQ(OkStatus(), call.Cancel());
    EXPECT_EQ(Status::FailedPrecondition(), new_call.Cancel());
  }
}

TEST(NanopbRpcIntegrationTest, BidirectionalStreaming_ReuseCall) {
  pw::rpc::NanopbClientReaderWriter<pw_rpc_Payload, pw_rpc_Payload> call;

  for (int i = 0; i < kIterations; ++i) {
    PayloadReceiver receiver;
    call = kClient.BidirectionalEcho(receiver.OnNext());

    ASSERT_EQ(OkStatus(), call.Write(Payload("Yello")));
    ASSERT_STREQ(receiver.Wait(), "Yello");

    ASSERT_EQ(OkStatus(), call.Write(Payload("Dello")));
    ASSERT_STREQ(receiver.Wait(), "Dello");

    ASSERT_EQ(OkStatus(), call.Write(Payload("???")));
    ASSERT_STREQ(receiver.Wait(), "???");
  }
}

}  // namespace
}  // namespace nanopb_rpc_test
