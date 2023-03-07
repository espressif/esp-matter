// Copyright 2020 The Pigweed Authors
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

#include "pw_preprocessor/arch.h"

namespace pw::interrupt {

#if !_PW_ARCH_ARM_CORTEX_M
#error You can only build this for ARM Cortex-M architectures. If you are \
       trying to do this and are still seeing this error, see \
       pw_preprocessor/arch.h
#endif  // !_PW_ARCH_ARM_CORTEX_M

#if _PW_ARCH_ARM_V6M || _PW_ARCH_ARM_V7M || _PW_ARCH_ARM_V7EM || \
    _PW_ARCH_ARM_V8M_BASELINE || _PW_ARCH_ARM_V8M_MAINLINE ||    \
    _PW_ARCH_ARM_V8_1M_MAINLINE
inline bool InInterruptContext() {
  // ARMv7M Reference manual section B1.4.2 describes how the Interrupt
  // Program Status Register (IPSR) is zero if there is no exception (interrupt)
  // being processed.
  uint32_t ipsr;
  asm volatile("MRS %0, ipsr" : "=r"(ipsr));
  return ipsr != 0;
}
#else
#error "Your selected ARM Cortex-M arch is not yet supported by this module."
#endif

}  // namespace pw::interrupt
