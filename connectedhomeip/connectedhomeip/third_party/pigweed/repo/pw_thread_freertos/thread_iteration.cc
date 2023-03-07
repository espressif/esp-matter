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
#include "pw_span/span.h"
#include "pw_status/status.h"
#include "pw_thread/thread_info.h"
#include "pw_thread_freertos/freertos_tsktcb.h"
#include "pw_thread_freertos/util.h"
#include "pw_thread_freertos_private/thread_iteration.h"

namespace pw::thread {
namespace freertos {

bool StackInfoCollector(TaskHandle_t current_thread,
                        const pw::thread::ThreadCallback& cb) {
  const tskTCB& tcb = *reinterpret_cast<tskTCB*>(current_thread);
  ThreadInfo thread_info;

  span<const std::byte> current_name =
      as_bytes(span(std::string_view(tcb.pcTaskName)));
  thread_info.set_thread_name(current_name);

  thread_info.set_stack_pointer(reinterpret_cast<uintptr_t>(tcb.pxTopOfStack));

  // Current thread stack bounds.
  thread_info.set_stack_low_addr(reinterpret_cast<uintptr_t>(tcb.pxStack));
#if configRECORD_STACK_HIGH_ADDRESS
  thread_info.set_stack_high_addr(
      reinterpret_cast<uintptr_t>(tcb.pxEndOfStack));
#if INCLUDE_uxTaskGetStackHighWaterMark
// Walk through the stack from start to end to measure the current peak
// using high-water marked stack data.
#if (portSTACK_GROWTH > 0)
  thread_info.set_stack_peak_addr(
      thread_info.stack_high_addr().value() -
      (sizeof(StackType_t) * uxTaskGetStackHighWaterMark(current_thread)));
#else
  thread_info.set_stack_peak_addr(
      thread_info.stack_low_addr().value() +
      (sizeof(StackType_t) * uxTaskGetStackHighWaterMark(current_thread)));
#endif  // portSTACK_GROWTH > 0
#endif  // INCLUDE_uxTaskGetStackHighWaterMark
#endif  // configRECORD_STACK_HIGH_ADDRESS

  return cb(thread_info);
}

}  // namespace freertos

// This will disable the scheduler.
Status ForEachThread(const pw::thread::ThreadCallback& cb) {
  pw::thread::freertos::ThreadCallback adapter_cb =
      [&cb](TaskHandle_t current_thread, eTaskState) -> bool {
    return freertos::StackInfoCollector(current_thread, cb);
  };
  // Suspend scheduler.
  vTaskSuspendAll();
  Status status = pw::thread::freertos::ForEachThread(adapter_cb);
  // Resume scheduler.
  xTaskResumeAll();
  return status;
}

}  // namespace pw::thread
