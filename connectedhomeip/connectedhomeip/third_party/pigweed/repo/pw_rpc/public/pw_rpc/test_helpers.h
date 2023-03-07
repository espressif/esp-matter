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
#pragma once

#include <chrono>

#include "pw_assert/assert.h"
#include "pw_chrono/system_clock.h"
#include "pw_rpc/internal/fake_channel_output.h"
#include "pw_rpc/method_info.h"
#include "pw_status/status.h"
#include "pw_sync/counting_semaphore.h"
#include "pw_thread/yield.h"

namespace pw::rpc::test {

// Wait until the provided RawFakeChannelOutput, NanopbFakeChannelOutput or
// PwpbFakeChannelOutput receives the specified number of packets.
template <unsigned kTimeoutSeconds = 10, typename Function>
void WaitForPackets(internal::test::FakeChannelOutput& output,
                    int count,
                    Function&& run_before) {
  sync::CountingSemaphore sem;
  output.set_on_send([&sem](ConstByteSpan, Status) { sem.release(); });

  run_before();

  for (int i = 0; i < count; ++i) {
    PW_ASSERT(sem.try_acquire_for(std::chrono::seconds(kTimeoutSeconds)));
  }

  output.set_on_send(nullptr);
}

// Checks that kMethod was called in client_context (which is a
// PwpbClientTestContext or a NanopbClientTestContext) and sends the response
// and status back.
//
// If no kMethod was called in timeout duration - returns
// DEADLINE_EXCEEDED. Otherwise returns OK.
//
// Example: Let's say we are testing an RPC service (my::MyService) that as part
// of the call (GetData) handling does another RPC call to a different service
// (other::OtherService::GetPart). my::MyService constructor accepts the
// other::OtherService::Client as an argument. To be able to test it we need to
// provide a prepared response when request is sent.
//
//   pw::rpc::PwpbClientTestContext client_context;
//   other::pw_rpc::pwpb::OtherService::Client other_service_client(
//       client_context.client(), client_context.channel().id());
//
//   PW_PWPB_TEST_METHOD_CONTEXT(MyService, GetData)
//   context(other_service_client);
//   context.call({});
//
//   ASSERT_EQ(pw::rpc::test::SendResponseIfCalled<
//             other::pw_rpc::pwpb::OtherService::GetPart>(client_context,
//                                                         {.value = 42}),
//             pw::OkStatus());
//
//   // At this point we have GetData handler received the response for GetPart.
template <auto kMethod, typename Context>
Status SendResponseIfCalled(
    Context& client_context,
    const MethodResponseType<kMethod>& response,
    Status status = OkStatus(),
    chrono::SystemClock::duration timeout =
        chrono::SystemClock::for_at_least(std::chrono::milliseconds(100))) {
  const auto start_time = chrono::SystemClock::now();
  while (chrono::SystemClock::now() - start_time < timeout) {
    const auto count =
        client_context.output().template total_payloads<kMethod>();
    if (count > 0) {
      client_context.server().template SendResponse<kMethod>(response, status);
      return OkStatus();
    }
    this_thread::yield();
  }
  return Status::DeadlineExceeded();
}

// Shortcut for SendResponseIfCalled(client_context, {}, status, timeout).
template <auto kMethod, typename Context>
Status SendResponseIfCalled(
    Context& client_context,
    Status status = OkStatus(),
    chrono::SystemClock::duration timeout =
        chrono::SystemClock::for_at_least(std::chrono::milliseconds(100))) {
  return SendResponseIfCalled<kMethod, Context>(
      client_context, {}, status, timeout);
}

}  // namespace pw::rpc::test
