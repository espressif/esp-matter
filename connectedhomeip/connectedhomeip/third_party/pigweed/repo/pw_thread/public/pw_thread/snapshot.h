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

#include <optional>
#include <string_view>

#include "pw_bytes/span.h"
#include "pw_function/function.h"
#include "pw_protobuf/encoder.h"
#include "pw_status/status.h"
#include "pw_thread_protos/thread.pwpb.h"

namespace pw::thread {

// Stack dump callback functions populate a thread's raw_backtrace or raw_stack
// field. This should encode either raw_backtrace or raw_stack to the provided
// Thread stream encoder.
using ProcessThreadStackCallback =
    Function<Status(proto::Thread::StreamEncoder&, ConstByteSpan)>;

struct StackContext {
  std::string_view thread_name;
  uintptr_t stack_low_addr;
  uintptr_t stack_high_addr;
  uintptr_t stack_pointer;
  std::optional<uintptr_t> stack_pointer_est_peak;
};

// Takes the provided StackContext, and writes stack context to the provided
// Thread encoder. After stack context is captured, the thread_stack_callback is
// invoked to capture either the raw_stack or raw_backtrace to the same encoder.
//
// Captures the following proto fields:
//   pw.thread.Thread:
//     stack_start_pointer
//     stack_end_pointer
//     stack_pointer
Status SnapshotStack(const StackContext& stack,
                     proto::Thread::StreamEncoder& encoder,
                     const ProcessThreadStackCallback& thread_stack_callback);

}  // namespace pw::thread
