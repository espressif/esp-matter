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

#include "pw_cpu_exception_cortex_m/cpu_state.h"

namespace pw::cpu_exception::cortex_m {

void LogExceptionAnalysis(const pw_cpu_exception_State& cpu_state);

enum class ProcessorMode {
  kHandlerMode,  // Handling interrupts/exceptions (msp).
  kThreadMode,   // May be on either psp or msp.
};
ProcessorMode ActiveProcessorMode(const pw_cpu_exception_State& cpu_state);

// Returns whether the msp was active in thread or handler modes.
bool MainStackActive(const pw_cpu_exception_State& cpu_state);

// Returns whether the psp was active in thread mode.
inline bool ProcessStackActive(const pw_cpu_exception_State& cpu_state) {
  return !MainStackActive(cpu_state);
}

}  // namespace pw::cpu_exception::cortex_m
