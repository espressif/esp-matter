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

#include <cstdint>

#include "pw_cpu_exception_cortex_m/cpu_state.h"
#include "pw_cpu_exception_cortex_m_protos/cpu_state.pwpb.h"
#include "pw_protobuf/encoder.h"
#include "pw_status/status.h"
#include "pw_thread/snapshot.h"
#include "pw_thread_protos/thread.pwpb.h"

namespace pw::cpu_exception::cortex_m {

// Takes the provided pw_cpu_exception_State, and writes the cpu register state
// to the provided SnapshotCpuStateOverlay encoder.
//
// Captures the pw.cpu_exception.cortex_m.ArmV7mCpuState proto field.
Status SnapshotCpuState(
    const pw_cpu_exception_State& cpu_state,
    cpu_exception::cortex_m::SnapshotCpuStateOverlay::StreamEncoder& encoder);

// Captures the main stack thread if active as part of a snapshot based on a
// previously captured cpu_state.
Status SnapshotMainStackThread(
    const pw_cpu_exception_State& cpu_state,
    uintptr_t stack_low_addr,
    uintptr_t stack_high_addr,
    thread::proto::SnapshotThreadInfo::StreamEncoder& encoder,
    thread::ProcessThreadStackCallback& thread_stack_callback);

// Captures the main stack thread if active as part of a snapshot based on the
// current context.
//
// Note: This is NOT the recommended way to capture the main stack as it will
// include the snapshot handling as part of the main stack capture.
Status SnapshotMainStackThread(
    uintptr_t stack_low_addr,
    uintptr_t stack_high_addr,
    thread::proto::SnapshotThreadInfo::StreamEncoder& encoder,
    thread::ProcessThreadStackCallback& thread_stack_callback);

// Captures the main stack thread if active as part of the cpu register state if
// optionally provided, else the current context is used.
//
// Note: This is NOT the recommended way to capture the main stack as it will
// include the snapshot handling as part of the main stack capture.
inline Status SnapshotMainStackThread(
    const pw_cpu_exception_State* optional_cpu_state,
    uintptr_t stack_low_addr,
    uintptr_t stack_high_addr,
    thread::proto::SnapshotThreadInfo::StreamEncoder& encoder,
    thread::ProcessThreadStackCallback& thread_stack_callback) {
  if (optional_cpu_state != nullptr) {
    return SnapshotMainStackThread(*optional_cpu_state,
                                   stack_low_addr,
                                   stack_high_addr,
                                   encoder,
                                   thread_stack_callback);
  }
  return SnapshotMainStackThread(
      stack_low_addr, stack_high_addr, encoder, thread_stack_callback);
}

}  // namespace pw::cpu_exception::cortex_m
