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

#define PW_LOG_LEVEL PW_THREAD_THREADX_CONFIG_LOG_LEVEL

#include "pw_thread_threadx/snapshot.h"

#include <string_view>

#include "pw_function/function.h"
#include "pw_log/log.h"
#include "pw_protobuf/encoder.h"
#include "pw_status/status.h"
#include "pw_thread/snapshot.h"
#include "pw_thread_protos/thread.pwpb.h"
#include "pw_thread_threadx/config.h"
#include "pw_thread_threadx/util.h"
#include "tx_api.h"
#include "tx_thread.h"

namespace pw::thread::threadx {
namespace {

// TODO(amontanez): This might make unit testing codepaths that use this more
// challenging.
inline bool ThreadIsRunning(const TX_THREAD& thread) {
  const TX_THREAD* running_thread;
  TX_THREAD_GET_CURRENT(running_thread);
  return running_thread == &thread;
}

void CaptureThreadState(const TX_THREAD& thread,
                        proto::Thread::StreamEncoder& encoder) {
  if (ThreadIsRunning(thread)) {
    PW_LOG_DEBUG("Thread state: RUNNING");
    encoder.WriteState(proto::ThreadState::Enum::RUNNING);
    return;
  }

  switch (thread.tx_thread_state) {
    case TX_READY:
      PW_LOG_DEBUG("Thread state: READY");
      encoder.WriteState(proto::ThreadState::Enum::READY);
      break;
    case TX_COMPLETED:
    case TX_TERMINATED:
      PW_LOG_DEBUG("Thread state: INACTIVE");
      encoder.WriteState(proto::ThreadState::Enum::INACTIVE);
      break;
    case TX_SUSPENDED:
    case TX_SLEEP:
      PW_LOG_DEBUG("Thread state: SUSPENDED");
      encoder.WriteState(proto::ThreadState::Enum::SUSPENDED);
      break;
    case TX_QUEUE_SUSP:
    case TX_SEMAPHORE_SUSP:
    case TX_EVENT_FLAG:
    case TX_BLOCK_MEMORY:
    case TX_BYTE_MEMORY:
    case TX_IO_DRIVER:
    case TX_FILE:
    case TX_TCP_IP:
    case TX_MUTEX_SUSP:
      PW_LOG_DEBUG("Thread state: BLOCKED");
      encoder.WriteState(proto::ThreadState::Enum::BLOCKED);
      break;
    default:
      PW_LOG_DEBUG("Thread state: UNKNOWN");
      encoder.WriteState(proto::ThreadState::Enum::UNKNOWN);
  }
}

}  // namespace

Status SnapshotThreads(void* running_thread_stack_pointer,
                       proto::SnapshotThreadInfo::StreamEncoder& encoder,
                       ProcessThreadStackCallback& stack_dumper) {
  struct {
    void* running_thread_stack_pointer;
    proto::SnapshotThreadInfo::StreamEncoder* encoder;
    ProcessThreadStackCallback* stack_dumper;
    Status thread_capture_status;
  } ctx;
  ctx.running_thread_stack_pointer = running_thread_stack_pointer;
  ctx.encoder = &encoder;
  ctx.stack_dumper = &stack_dumper;

  ThreadCallback thread_capture_cb([&ctx](const TX_THREAD& thread) -> bool {
    proto::Thread::StreamEncoder thread_encoder =
        ctx.encoder->GetThreadsEncoder();
    ctx.thread_capture_status.Update(
        SnapshotThread(thread,
                       ctx.running_thread_stack_pointer,
                       thread_encoder,
                       *ctx.stack_dumper));
    // Always iterate all threads.
    return true;
  });

  if (Status status = ForEachThread(thread_capture_cb); !status.ok()) {
    PW_LOG_ERROR("Failed to iterate threads during snapshot capture: %d",
                 static_cast<int>(status.code()));
  }

  return ctx.thread_capture_status;
}

Status SnapshotThread(const TX_THREAD& thread,
                      void* running_thread_stack_pointer,
                      proto::Thread::StreamEncoder& encoder,
                      ProcessThreadStackCallback& thread_stack_callback) {
  PW_LOG_DEBUG("Capturing thread info for %s", thread.tx_thread_name);
  encoder.WriteName(as_bytes(span(std::string_view(thread.tx_thread_name))));

  CaptureThreadState(thread, encoder);

  const StackContext thread_ctx = {
      .thread_name = thread.tx_thread_name,

      // TODO(amontanez): When ThreadX is built with stack checking enabled, the
      // lowest-addressed `unsigned long` is reserved for a watermark. This
      // means in practice the stack pointer should never end up there. To be
      // conservative, behave as though TX_THREAD_STACK_CHECK is always fully
      // enabled.
      .stack_low_addr =
          reinterpret_cast<uintptr_t>(thread.tx_thread_stack_start) +
          sizeof(ULONG),

      .stack_high_addr =
          reinterpret_cast<uintptr_t>(thread.tx_thread_stack_end),

      // If the thread is active, the stack pointer in the TCB is stale.
      .stack_pointer = reinterpret_cast<uintptr_t>(
          ThreadIsRunning(thread) ? running_thread_stack_pointer
                                  : thread.tx_thread_stack_ptr),
      .stack_pointer_est_peak = std::nullopt,
  };

  return SnapshotStack(thread_ctx, encoder, thread_stack_callback);
}

}  // namespace pw::thread::threadx
