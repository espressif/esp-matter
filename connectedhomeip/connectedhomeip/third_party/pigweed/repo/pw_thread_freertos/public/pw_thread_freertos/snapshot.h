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
#pragma once

#include "FreeRTOS.h"
#include "pw_protobuf/encoder.h"
#include "pw_span/span.h"
#include "pw_status/status.h"
#include "pw_thread/snapshot.h"
#include "pw_thread_protos/thread.pwpb.h"
#include "task.h"

namespace pw::thread::freertos {

// Captures all FreeRTOS threads in a system as part of a snapshot.
//
// Note: this requires the pw_thread_freertos:freertos_tskcb backend to be
// set in order to access the stack limits inside of tskTCB.
//
// An updated running_thread_stack_pointer must be provided in order for the
// running thread's context to reflect the running state. In addition a task
// status buffer must be provided which can fit uxTaskGetNumberOfTasks()
// entries. For ARM, you might do something like this:
//
//    // Capture PSP.
//    void* stack_ptr = 0;
//    asm volatile("mrs %0, psp\n" : "=r"(stack_ptr));
//    pw::thread::ProcessThreadStackCallback cb =
//        [](pw::thread::proto::Thread::StreamEncoder& encoder,
//           pw::ConstByteSpan stack) -> pw::Status {
//      return encoder.WriteRawStack(stack);
//    };
//    pw::thread::freertos::SnapshotThread(stack_ptr, snapshot_encoder, cb,
//                                         task_status_buffer);
//
// Warning: This is only safe to use when the scheduler and interrupts are
// disabled.
Status SnapshotThreads(void* running_thread_stack_pointer,
                       proto::SnapshotThreadInfo::StreamEncoder& encoder,
                       ProcessThreadStackCallback& thread_stack_callback);

// Captures only the provided thread handle as a pw::thread::Thread proto
// message.
//
// An updated running_thread_stack_pointer must be provided in order for the
// running thread's context to reflect the current state. If the thread being
// captured is not the running thread, the value is ignored. Note that the
// stack pointer in the thread handle is almost always stale on the running
// thread.
//
// Note: this requires the pw_thread_freertos:freertos_tskcb backend to be
// set in order to access the stack limits inside of tskTCB.
//
// Captures the following proto fields:
//
//   pw.thread.Thread:
//     name
//     state
//     stack_start_pointer
//     stack_end_pointer (if (portSTACK_GROWTH > 0) ||
//                           (configRECORD_STACK_HIGH_ADDRESS == 1))
//     stack_pointer
//
Status SnapshotThread(TaskHandle_t thread,
                      eTaskState thread_state,
                      void* running_thread_stack_pointer,
                      proto::Thread::StreamEncoder& encoder,
                      ProcessThreadStackCallback& thread_stack_callback);

}  // namespace pw::thread::freertos
