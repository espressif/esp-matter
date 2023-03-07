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

#define PW_LOG_LEVEL PW_THREAD_CONFIG_LOG_LEVEL

#include "pw_thread/snapshot.h"

#include <cinttypes>
#include <string_view>

#include "pw_bytes/span.h"
#include "pw_function/function.h"
#include "pw_log/log.h"
#include "pw_protobuf/encoder.h"
#include "pw_status/status.h"
#include "pw_thread/config.h"
#include "pw_thread_protos/thread.pwpb.h"

namespace pw::thread {

Status SnapshotStack(const StackContext& stack,
                     proto::Thread::StreamEncoder& encoder,
                     const ProcessThreadStackCallback& thread_stack_callback) {
  // TODO(b/234890430): Add support for ascending stacks.
  encoder.WriteStackStartPointer(stack.stack_high_addr).IgnoreError();
  encoder.WriteStackEndPointer(stack.stack_low_addr).IgnoreError();
  encoder.WriteStackPointer(stack.stack_pointer).IgnoreError();
  // The PRIxPTR is an appropriate format specifier for hex uintptr_t values
  // https://stackoverflow.com/a/5796039/1224002
  PW_LOG_DEBUG("Active stack: 0x%08" PRIxPTR "-0x%08" PRIxPTR " (%ld bytes)",
               stack.stack_high_addr,
               stack.stack_pointer,
               static_cast<long>(stack.stack_high_addr) -
                   static_cast<long>(stack.stack_pointer));
  if (stack.stack_pointer_est_peak.has_value()) {
    const uintptr_t stack_pointer_est_peak =
        stack.stack_pointer_est_peak.value();
    encoder.WriteStackPointerEstPeak(stack_pointer_est_peak).IgnoreError();
    PW_LOG_DEBUG("Est peak stack: 0x%08" PRIxPTR "-0x%08" PRIxPTR
                 " (%ld bytes)",
                 stack.stack_high_addr,
                 stack_pointer_est_peak,
                 static_cast<long>(stack.stack_high_addr) -
                     static_cast<long>(stack_pointer_est_peak));
  }
  PW_LOG_DEBUG("Stack Limits: 0x%08" PRIxPTR "-0x%08" PRIxPTR " (%ld bytes)",
               stack.stack_low_addr,
               stack.stack_high_addr,
               static_cast<long>(stack.stack_high_addr) -
                   static_cast<long>(stack.stack_low_addr));

  if (stack.stack_pointer > stack.stack_high_addr) {
    PW_LOG_ERROR("%s's stack underflowed by %lu bytes",
                 stack.thread_name.data(),
                 static_cast<long unsigned>(stack.stack_pointer -
                                            stack.stack_high_addr));
    return Status::OutOfRange();
  }

  // Log an error, but don't prevent the capture.
  if (stack.stack_pointer < stack.stack_low_addr) {
    PW_LOG_ERROR(
        "%s's stack overflowed by %lu bytes",
        stack.thread_name.data(),
        static_cast<long unsigned>(stack.stack_low_addr - stack.stack_pointer));
  }

  return thread_stack_callback(
      encoder,
      ConstByteSpan(reinterpret_cast<const std::byte*>(stack.stack_pointer),
                    stack.stack_high_addr - stack.stack_pointer));
}

}  // namespace pw::thread
