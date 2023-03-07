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
#pragma once

#include <cstdint>
#include <cstring>

#include "pw_rpc/internal/lock.h"
#include "pw_rpc/internal/method.h"
#include "pw_rpc/internal/method_union.h"
#include "pw_rpc/internal/packet.h"
#include "pw_rpc/internal/server_call.h"
#include "pw_rpc/method_type.h"
#include "pw_span/span.h"
#include "pw_status/status_with_size.h"

namespace pw::rpc::internal {

// This is a fake RPC method implementation for testing only. It stores the
// channel ID, request, and payload buffer, and optionally provides a response.
class TestMethod : public Method {
 public:
  class FakeServerCall : public ServerCall {
   public:
    constexpr FakeServerCall() = default;
    FakeServerCall(const LockedCallContext& context, MethodType type)
        PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock())
        : ServerCall(context, type) {}

    FakeServerCall(FakeServerCall&&) = default;
    FakeServerCall& operator=(FakeServerCall&&) = default;

    using internal::Call::set_on_error;
  };

  constexpr TestMethod(uint32_t id, MethodType type = MethodType::kUnary)
      : Method(id, GetInvoker(type)),
        last_channel_id_(0),
        invocations_(0),
        move_to_call_(nullptr) {}

  uint32_t last_channel_id() const { return last_channel_id_; }
  const Packet& last_request() const { return last_request_; }
  size_t invocations() const { return invocations_; }

  // Sets a call object into which to move the call object when the RPC is
  // invoked. This keeps the RPC active until the provided call object is
  // finished or goes out of scope.
  void keep_call_active(FakeServerCall& move_to_call) const {
    move_to_call_ = &move_to_call;
  }

 private:
  template <MethodType kType>
  static void InvokeForTest(const CallContext& context, const Packet& request)
      PW_UNLOCK_FUNCTION(rpc_lock()) {
    const auto& test_method = static_cast<const TestMethod&>(context.method());
    test_method.last_channel_id_ = context.channel_id();
    test_method.last_request_ = request;
    test_method.invocations_ += 1;

    // Create a call object so it registers / unregisters with the server.
    FakeServerCall fake_call(context.ClaimLocked(), kType);

    rpc_lock().unlock();

    if (test_method.move_to_call_ != nullptr) {
      *test_method.move_to_call_ = std::move(fake_call);
    }
  }

  static constexpr Invoker GetInvoker(MethodType type) {
    switch (type) {
      case MethodType::kUnary:
        return InvokeForTest<MethodType::kUnary>;
      case MethodType::kServerStreaming:
        return InvokeForTest<MethodType::kServerStreaming>;
      case MethodType::kClientStreaming:
        return InvokeForTest<MethodType::kClientStreaming>;
      case MethodType::kBidirectionalStreaming:
        return InvokeForTest<MethodType::kBidirectionalStreaming>;
    };
  }

  // Make these mutable so they can be set in the Invoke method, which is const.
  // The Method class is used exclusively in tests. Having these members mutable
  // allows tests to verify that the Method is invoked correctly.
  mutable uint32_t last_channel_id_;
  mutable Packet last_request_;
  mutable size_t invocations_;
  mutable FakeServerCall* move_to_call_;

  span<const std::byte> response_;
  Status response_status_;
};

class TestMethodUnion : public MethodUnion {
 public:
  constexpr TestMethodUnion(TestMethod&& method) : impl_({.test = method}) {}

  constexpr const Method& method() const { return impl_.method; }
  constexpr const TestMethod& test_method() const { return impl_.test; }

 private:
  union {
    Method method;
    TestMethod test;
  } impl_;
};

}  // namespace pw::rpc::internal
