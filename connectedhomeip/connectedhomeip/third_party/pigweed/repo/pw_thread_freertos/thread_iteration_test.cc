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

#include "pw_thread/thread_iteration.h"

#include <cstddef>
#include <string_view>

#include "FreeRTOS.h"
#include "gtest/gtest.h"
#include "pw_bytes/span.h"
#include "pw_span/span.h"
#include "pw_string/string_builder.h"
#include "pw_string/util.h"
#include "pw_sync/thread_notification.h"
#include "pw_thread/test_threads.h"
#include "pw_thread/thread.h"
#include "pw_thread/thread_info.h"
#include "pw_thread_freertos/freertos_tsktcb.h"
#include "pw_thread_freertos_private/thread_iteration.h"

namespace pw::thread::freertos {
namespace {

sync::ThreadNotification lock_start;
sync::ThreadNotification lock_end;

void ForkedThreadEntry(void*) {
  // Release start lock to allow test thread to continue execution.
  lock_start.release();
  while (true) {
    // Return only when end lock released by test thread.
    if (lock_end.try_acquire()) {
      return;
    }
  }
}

// Tests thread iteration API by:
//  - Forking a test thread.
//  - Using iteration API to iterate over all running threads.
//  - Compares name of forked thread and current thread.
//  - Confirms thread exists and is iterated over.
TEST(ThreadIteration, ForkOneThread) {
  const auto& options = *static_cast<const pw::thread::freertos::Options*>(
      &thread::test::TestOptionsThread0());
  thread::Thread t(options, ForkedThreadEntry);

  // Blocked until thread t releases start lock.
  lock_start.acquire();

  struct {
    bool thread_exists;
    span<const std::byte> name;
  } temp_struct;

  temp_struct.thread_exists = false;
  // Max permissible length of task name including null byte.
  static constexpr size_t buffer_size = configMAX_TASK_NAME_LEN;

  std::string_view string(string::ClampedCString(options.name(), buffer_size));
  temp_struct.name = as_bytes(span(string));

  // Callback that confirms forked thread is checked by the iterator.
  auto cb = [&temp_struct](const ThreadInfo& thread_info) {
    // Compare sizes accounting for null byte.
    if (thread_info.thread_name().has_value()) {
      for (size_t i = 0; i < thread_info.thread_name().value().size(); i++) {
        // Compare character by character of span.
        if ((unsigned char)thread_info.thread_name().value().data()[i] !=
            (unsigned char)temp_struct.name.data()[i]) {
          return true;
        }
      }
      temp_struct.thread_exists = true;
    }
    // Signal to stop iteration.
    return false;
  };

  thread::ForEachThread(cb);

  // Signal to forked thread that execution is complete.
  lock_end.release();

  // Clean up the test thread context.
#if PW_THREAD_JOINING_ENABLED
  t.join();
#else
  t.detach();
  thread::test::WaitUntilDetachedThreadsCleanedUp();
#endif  // PW_THREAD_JOINING_ENABLED

  EXPECT_TRUE(temp_struct.thread_exists);
}

#if INCLUDE_uxTaskGetStackHighWaterMark
#if configRECORD_STACK_HIGH_ADDRESS

TEST(ThreadIteration, StackInfoCollector_PeakStackUsage) {
  // This is the value FreeRTOS expects, but it's worth noting that there's no
  // easy way to get this value directly from FreeRTOS.
  constexpr uint8_t tskSTACK_FILL_BYTE = 0xa5U;
  std::array<StackType_t, 128> stack;
  ByteSpan stack_bytes(as_writable_bytes(span(stack)));
  std::memset(stack_bytes.data(), tskSTACK_FILL_BYTE, stack_bytes.size_bytes());

  tskTCB fake_tcb;
  StringBuilder sb(fake_tcb.pcTaskName);
  sb.append("FakeTCB");
  fake_tcb.pxStack = stack.data();
  fake_tcb.pxEndOfStack = stack.data() + stack.size();

  // Clobber bytes as if they were used.
  constexpr size_t kBytesRemaining = 96;
#if portSTACK_GROWTH > 0
  std::memset(stack_bytes.data(),
              tskSTACK_FILL_BYTE ^ 0x2b,
              stack_bytes.size() - kBytesRemaining);
#else
  std::memset(&stack_bytes[kBytesRemaining],
              tskSTACK_FILL_BYTE ^ 0x2b,
              stack_bytes.size() - kBytesRemaining);
#endif  // portSTACK_GROWTH > 0

  ThreadCallback cb = [kBytesRemaining](const ThreadInfo& info) -> bool {
    EXPECT_TRUE(info.stack_high_addr().has_value());
    EXPECT_TRUE(info.stack_low_addr().has_value());
    EXPECT_TRUE(info.stack_peak_addr().has_value());

#if portSTACK_GROWTH > 0
    EXPECT_EQ(info.stack_high_addr().value() - info.stack_peak_addr().value(),
              kBytesRemaining);
#else
    EXPECT_EQ(info.stack_peak_addr().value() - info.stack_low_addr().value(),
              kBytesRemaining);
#endif  // portSTACK_GROWTH > 0
    return true;
  };

  EXPECT_TRUE(
      StackInfoCollector(reinterpret_cast<TaskHandle_t>(&fake_tcb), cb));
}

#endif  // INCLUDE_uxTaskGetStackHighWaterMark
#endif  // configRECORD_STACK_HIGH_ADDRESS

}  // namespace
}  // namespace pw::thread::freertos
