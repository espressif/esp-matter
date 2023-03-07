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

#define PW_LOG_LEVEL PW_THREAD_FREERTOS_CONFIG_LOG_LEVEL

#include "pw_thread_freertos/snapshot.h"

#include <string_view>

#include "FreeRTOS.h"
#include "pw_function/function.h"
#include "pw_log/log.h"
#include "pw_protobuf/encoder.h"
#include "pw_span/span.h"
#include "pw_status/status.h"
#include "pw_thread/snapshot.h"
#include "pw_thread_freertos/config.h"
#include "pw_thread_freertos/freertos_tsktcb.h"
#include "pw_thread_freertos/util.h"
#include "pw_thread_protos/thread.pwpb.h"
#include "task.h"

namespace pw::thread::freertos {
namespace {

// The externed function is an internal FreeRTOS kernel function from
// FreeRTOS/Source/tasks.c needed in order to calculate a thread's stack usage
// from interrupts which the native APIs do not permit.
#if ((configUSE_TRACE_FACILITY == 1) || \
     (INCLUDE_uxTaskGetStackHighWaterMark == 1))
extern "C" uint16_t prvTaskCheckFreeStackSpace(const uint8_t* pucStackByte);
#endif  // ((configUSE_TRACE_FACILITY == 1) ||
        // (INCLUDE_uxTaskGetStackHighWaterMark == 1))

void CaptureThreadState(eTaskState thread_state,
                        proto::Thread::StreamEncoder& encoder) {
  switch (thread_state) {
    case eRunning:
      PW_LOG_DEBUG("Thread state: RUNNING");
      encoder.WriteState(proto::ThreadState::Enum::RUNNING).IgnoreError();
      return;

    case eReady:
      PW_LOG_DEBUG("Thread state: READY");
      encoder.WriteState(proto::ThreadState::Enum::READY).IgnoreError();
      return;

    case eBlocked:
      PW_LOG_DEBUG("Thread state: BLOCKED");
      encoder.WriteState(proto::ThreadState::Enum::BLOCKED).IgnoreError();
      return;

    case eSuspended:
      PW_LOG_DEBUG("Thread state: SUSPENDED");
      encoder.WriteState(proto::ThreadState::Enum::SUSPENDED).IgnoreError();
      return;

    case eDeleted:
      PW_LOG_DEBUG("Thread state: INACTIVE");
      encoder.WriteState(proto::ThreadState::Enum::INACTIVE).IgnoreError();
      return;

    case eInvalid:
    default:
      PW_LOG_DEBUG("Thread state: UNKNOWN");
      encoder.WriteState(proto::ThreadState::Enum::UNKNOWN).IgnoreError();
      return;
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
  ctx.thread_capture_status = OkStatus();

  ThreadCallback thread_capture_cb(
      [&ctx](TaskHandle_t thread, eTaskState thread_state) -> bool {
        proto::Thread::StreamEncoder thread_encoder =
            ctx.encoder->GetThreadsEncoder();
        ctx.thread_capture_status.Update(
            SnapshotThread(thread,
                           thread_state,
                           ctx.running_thread_stack_pointer,
                           thread_encoder,
                           *ctx.stack_dumper));
        return true;  // Iterate through all threads.
      });
  if (const Status status = ForEachThread(thread_capture_cb);
      !status.ok() && !status.IsFailedPrecondition()) {
    PW_LOG_ERROR("Failed to iterate threads during snapshot capture: %d",
                 status.code());
  }
  return ctx.thread_capture_status;
}

Status SnapshotThread(
    TaskHandle_t thread,
    eTaskState thread_state,
    void* running_thread_stack_pointer,
    proto::Thread::StreamEncoder& encoder,
    [[maybe_unused]] ProcessThreadStackCallback& thread_stack_callback) {
  const tskTCB& tcb = *reinterpret_cast<tskTCB*>(thread);

  PW_LOG_DEBUG("Capturing thread info for %s", tcb.pcTaskName);
  PW_TRY(encoder.WriteName(as_bytes(span(std::string_view(tcb.pcTaskName)))));

  CaptureThreadState(thread_state, encoder);

  // TODO(b/234890430): Update this once we add support for ascending stacks.
  static_assert(portSTACK_GROWTH < 0, "Ascending stacks are not yet supported");

  // If the thread is active, the stack pointer in the TCB is stale.
  const uintptr_t stack_pointer = reinterpret_cast<uintptr_t>(
      thread_state == eRunning ? running_thread_stack_pointer
                               : tcb.pxTopOfStack);
  const uintptr_t stack_low_addr = reinterpret_cast<uintptr_t>(tcb.pxStack);

#if ((portSTACK_GROWTH > 0) || (configRECORD_STACK_HIGH_ADDRESS == 1))
  const uintptr_t stack_high_addr =
      reinterpret_cast<uintptr_t>(tcb.pxEndOfStack);
  const StackContext thread_ctx = {
    .thread_name = tcb.pcTaskName,
    .stack_low_addr = stack_low_addr,
    .stack_high_addr = stack_high_addr,
    .stack_pointer = stack_pointer,
#if ((configUSE_TRACE_FACILITY == 1) || \
     (INCLUDE_uxTaskGetStackHighWaterMark == 1))
#if (portSTACK_GROWTH > 0)
    .stack_pointer_est_peak =
        stack_high_addr -
        (sizeof(StackType_t) *
         prvTaskCheckFreeStackSpace(
             reinterpret_cast<const uint8_t*>(stack_high_addr))),
#else
    .stack_pointer_est_peak =
        stack_low_addr +
        (sizeof(StackType_t) *
         prvTaskCheckFreeStackSpace(
             reinterpret_cast<const uint8_t*>(stack_low_addr))),
#endif  // (portSTACK_GROWTH > 0)
#else
    .stack_pointer_est_peak = std::nullopt,
#endif  // ((configUSE_TRACE_FACILITY == 1) ||
        // (INCLUDE_uxTaskGetStackHighWaterMark == 1))
  };
  return SnapshotStack(thread_ctx, encoder, thread_stack_callback);
#else
  encoder.WriteStackEndPointer(stack_low_addr);
  encoder.WriteStackPointer(stack_pointer);
  return encoder.status();
#endif  // ((portSTACK_GROWTH > 0) || (configRECORD_STACK_HIGH_ADDRESS == 1))
}

}  // namespace pw::thread::freertos
