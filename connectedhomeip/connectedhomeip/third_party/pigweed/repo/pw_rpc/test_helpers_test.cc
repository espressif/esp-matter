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

#include "pw_rpc/test_helpers.h"

#include <mutex>

#include "gtest/gtest.h"
#include "pw_chrono/system_clock.h"
#include "pw_containers/vector.h"
#include "pw_result/result.h"
#include "pw_rpc/echo.pwpb.h"
#include "pw_rpc/echo.rpc.pwpb.h"
#include "pw_rpc/pwpb/client_testing.h"
#include "pw_rpc/pwpb/server_reader_writer.h"
#include "pw_status/status.h"
#include "pw_sync/interrupt_spin_lock.h"
#include "pw_sync/lock_annotations.h"
#include "pw_sync/timed_thread_notification.h"

namespace pw::rpc::test {
namespace {
using namespace std::chrono_literals;

constexpr auto kWaitForEchoTimeout =
    pw::chrono::SystemClock::for_at_least(100ms);

// Class that we want to test.
//
// It's main purpose is to ask EchoService for Echo and provide its result
// through WaitForEcho/LastEcho pair to the user.
class EntityUnderTest {
 public:
  explicit EntityUnderTest(pw_rpc::pwpb::EchoService::Client& echo_client)
      : echo_client_(echo_client) {}

  void AskForEcho() {
    call_ = echo_client_.Echo(
        pwpb::EchoMessage::Message{},
        [this](const pwpb::EchoMessage::Message& response, pw::Status status) {
          lock_.lock();
          if (status.ok()) {
            last_echo_ = response.msg;
          } else {
            last_echo_ = status;
          }
          lock_.unlock();
          notifier_.release();
        },
        [this](pw::Status status) {
          lock_.lock();
          last_echo_ = status;
          lock_.unlock();
          notifier_.release();
        });
  }

  bool WaitForEcho(pw::chrono::SystemClock::duration duration) {
    return notifier_.try_acquire_for(duration);
  }

  pw::Result<pw::InlineString<64>> LastEcho() const {
    std::lock_guard<pw::sync::InterruptSpinLock> lock(lock_);
    return last_echo_;
  }

 private:
  pw_rpc::pwpb::EchoService::Client& echo_client_;
  PwpbUnaryReceiver<pwpb::EchoMessage::Message> call_;
  pw::sync::TimedThreadNotification notifier_;
  pw::Result<pw::InlineString<64>> last_echo_ PW_GUARDED_BY(lock_);
  mutable pw::sync::InterruptSpinLock lock_;
};

TEST(RpcTestHelpersTest, SendResponseIfCalledOk) {
  PwpbClientTestContext client_context;
  pw_rpc::pwpb::EchoService::Client client(client_context.client(),
                                           client_context.channel().id());
  EntityUnderTest entity(client);

  // We need to call the function that will initiate the request before we can
  // send the response back.
  entity.AskForEcho();

  // SendResponseIfCalled blocks until request is received by the service (it is
  // sent by AskForEcho to EchoService in this case) and responds to it with the
  // response.
  //
  // SendResponseIfCalled will timeout if no request were sent in the `timeout`
  // interval (see SendResponseIfCalledWithoutRequest test for the example).
  ASSERT_EQ(SendResponseIfCalled<pw_rpc::pwpb::EchoService::Echo>(
                client_context, {.msg = "Hello"}),
            OkStatus());

  // After SendResponseIfCalled returned OkStatus client should have received
  // the response back in the RPC thread, so we can check it here. Because it is
  // a separate thread we still need to wait with the timeout.
  ASSERT_TRUE(entity.WaitForEcho(kWaitForEchoTimeout));

  pw::Result<pw::InlineString<64>> result = entity.LastEcho();
  ASSERT_TRUE(result.ok());
  EXPECT_EQ(result.value(), "Hello");
}

TEST(RpcTestHelpersTest, SendResponseIfCalledNotOk) {
  PwpbClientTestContext client_context;
  pw_rpc::pwpb::EchoService::Client client(client_context.client(),
                                           client_context.channel().id());
  EntityUnderTest entity(client);

  // We need to call the function that will initiate the request before we can
  // send the response back.
  entity.AskForEcho();

  // SendResponseIfCalled also can be used to respond with failures. In this
  // case we are sending back pw::Status::InvalidArgument and expect to see it
  // on the client side.
  //
  // SendResponseIfCalled result status is not the same status as it sends to
  // the client, so we still are expecting the OkStatus here.
  ASSERT_EQ(SendResponseIfCalled<pw_rpc::pwpb::EchoService::Echo>(
                client_context, {}, pw::Status::InvalidArgument()),
            OkStatus());

  // After SendResponseIfCalled returned OkStatus client should have received
  // the response back in the RPC thread, so we can check it here. Because it is
  // a separate thread we still need to wait with the timeout.
  ASSERT_TRUE(entity.WaitForEcho(kWaitForEchoTimeout));

  EXPECT_EQ(entity.LastEcho().status(), Status::InvalidArgument());
}

TEST(RpcTestHelpersTest, SendResponseIfCalledNotOkShortcut) {
  PwpbClientTestContext client_context;
  pw_rpc::pwpb::EchoService::Client client(client_context.client(),
                                           client_context.channel().id());
  EntityUnderTest entity(client);

  // We need to call the function that will initiate the request before we can
  // send the response back.
  entity.AskForEcho();

  // SendResponseIfCalled shortcut version exists to respond with failures. It
  // works exactly the same, but doesn't have the response argument. In this
  // case we are sending back pw::Status::InvalidArgument and expect to see it
  // on the client side.
  //
  // SendResponseIfCalled result status is not the same status as it sends to
  // the client, so we still are expecting the OkStatus here.
  ASSERT_EQ(SendResponseIfCalled<pw_rpc::pwpb::EchoService::Echo>(
                client_context, pw::Status::InvalidArgument()),
            OkStatus());

  // After SendResponseIfCalled returned OkStatus client should have received
  // the response back in the RPC thread, so we can check it here. Because it is
  // a separate thread we still need to wait with the timeout.
  ASSERT_TRUE(entity.WaitForEcho(kWaitForEchoTimeout));

  EXPECT_EQ(entity.LastEcho().status(), Status::InvalidArgument());
}

TEST(RpcTestHelpersTest, SendResponseIfCalledWithoutRequest) {
  PwpbClientTestContext client_context;
  pw_rpc::pwpb::EchoService::Client client(client_context.client(),
                                           client_context.channel().id());

  // We don't send any request in this test and SendResponseIfCalled is expected
  // to fail on waiting for the request with pw::Status::FailedPrecondition.

  const auto start_time = pw::chrono::SystemClock::now();
  auto status = SendResponseIfCalled<pw_rpc::pwpb::EchoService::Echo>(
      client_context,
      {.msg = "Hello"},
      pw::OkStatus(),
      /*timeout=*/pw::chrono::SystemClock::for_at_least(10ms));

  // We set our timeout for SendResponseIfCalled to 10ms, so it should be at
  // least 10ms since we called the SendResponseIfCalled.
  EXPECT_GE(pw::chrono::SystemClock::now() - start_time,
            pw::chrono::SystemClock::for_at_least(10ms));

  // We expect SendResponseIfCalled to fail, because there were no request sent
  // for the given method.
  EXPECT_EQ(status, Status::DeadlineExceeded());
}

}  // namespace
}  // namespace pw::rpc::test
