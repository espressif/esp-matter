// Copyright 2019 The Pigweed Authors
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

#ifdef __cplusplus

#include <cstdint>

#include "pw_preprocessor/arch.h"
#include "pw_preprocessor/compiler.h"

namespace pw::cpu_exception::cortex_m {

// The PC, LR, and PSR registers are not captured when the program stack
// pointer is in an MPU-protected or otherwise invalid memory region. In
// these situations, the registers are set to 0xFFFF'FFFF to indicate they
// are invalid.
//
// 0xFFFFFFFF is an illegal LR value, which is why it was selected for
// this purpose. PC and PSR values of 0xFFFFFFFF are dubious too, so this
// constant is clear enough at suggesting that the registers weren't
// properly captured.
constexpr uintptr_t kUndefinedPcLrOrPsrRegValue = 0xFFFF'FFFF;

// This is dictated by ARMv7-M architecture. Do not change.
struct ExceptionRegisters {
  uint32_t r0;
  uint32_t r1;
  uint32_t r2;
  uint32_t r3;
  uint32_t r12;
  uint32_t lr;   // Link register, note this may be invalid.
  uint32_t pc;   // Program counter, note this may be invalid.
  uint32_t psr;  // Program status register, note this may be invalid.
};
static_assert(sizeof(ExceptionRegisters) == (sizeof(uint32_t) * 8),
              "There's unexpected padding.");

// This is dictated by ARMv7-M architecture. Do not change.
struct ExceptionRegistersFpu {
  uint32_t s0;
  uint32_t s1;
  uint32_t s2;
  uint32_t s3;
  uint32_t s4;
  uint32_t s5;
  uint32_t s6;
  uint32_t s7;
  uint32_t s8;
  uint32_t s9;
  uint32_t s10;
  uint32_t s11;
  uint32_t s12;
  uint32_t s13;
  uint32_t s14;
  uint32_t s15;
  uint32_t fpscr;
  uint32_t reserved;
};
static_assert(sizeof(ExceptionRegistersFpu) == (sizeof(uint32_t) * 18),
              "There's unexpected padding.");

// Bit in the PSR that indicates CPU added an extra word on the stack to
// align it during context save for an exception.
inline constexpr uint32_t kPsrExtraStackAlignBit = (1 << 9);

// This is dictated by this module, and shouldn't change often.
// Note that the order of entries in this struct is very important (as the
// values are populated in assembly).
//
// NOTE: Memory mapped registers are NOT restored upon fault return!
struct ExtraRegisters {
  // Memory mapped registers.
  uint32_t cfsr;
  uint32_t mmfar;
  uint32_t bfar;
  uint32_t icsr;
  uint32_t hfsr;
  uint32_t shcsr;
  // Special registers.
  uint32_t exc_return;
  uint32_t msp;
  uint32_t psp;
  uint32_t control;
#if _PW_ARCH_ARM_V8M_MAINLINE
  uint32_t msplim;
  uint32_t psplim;
#endif  // _PW_ARCH_ARM_V8M_MAINLINE
  // General purpose registers.
  uint32_t r4;
  uint32_t r5;
  uint32_t r6;
  uint32_t r7;
  uint32_t r8;
  uint32_t r9;
  uint32_t r10;
  uint32_t r11;
};
static_assert(sizeof(ExtraRegisters) ==
#if _PW_ARCH_ARM_V8M_MAINLINE
                  (sizeof(uint32_t) * 20),
#else   // !_PW_ARCH_ARM_V8M_MAINLINE
                  (sizeof(uint32_t) * 18),
#endif  // _PW_ARCH_ARM_V8M_MAINLINE
              "There's unexpected padding.");

}  // namespace pw::cpu_exception::cortex_m

struct pw_cpu_exception_State {
  pw::cpu_exception::cortex_m::ExtraRegisters extended;
  pw::cpu_exception::cortex_m::ExceptionRegisters base;
  // TODO(amontanez): FPU registers may or may not be here as well. Make the
  // availability of the FPU registers a compile-time configuration when FPU
  // register support is added.
};

#else  // !__cplusplus

typedef struct pw_cpu_exception_State pw_cpu_exception_State;

#endif  // __cplusplus
