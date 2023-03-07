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

#include "pw_cpu_exception/entry.h"

#include <cstdint>
#include <cstring>

#include "pw_cpu_exception/handler.h"
#include "pw_cpu_exception_cortex_m/cpu_state.h"
#include "pw_cpu_exception_cortex_m/util.h"
#include "pw_cpu_exception_cortex_m_private/cortex_m_constants.h"
#include "pw_preprocessor/arch.h"
#include "pw_preprocessor/compiler.h"

// TODO(pwbug/311): Deprecated naming.
PW_EXTERN_C PW_NO_PROLOGUE __attribute__((alias("pw_cpu_exception_Entry"))) void
pw_CpuExceptionEntry(void);

namespace pw::cpu_exception::cortex_m {
namespace {

// Checks exc_return to determine if FPU state was pushed to the stack in
// addition to the base CPU context frame.
bool FpuStateWasPushed(const pw_cpu_exception_State& cpu_state) {
  return !(cpu_state.extended.exc_return & kExcReturnBasicFrameMask);
}

// If the CPU successfully pushed context on exception, copy it into cpu_state.
//
// For more information see (See ARMv7-M Section B1.5.11, derived exceptions
// on exception entry).
void CloneBaseRegistersFromPsp(pw_cpu_exception_State* cpu_state) {
  // If CPU succeeded in pushing context to PSP, copy it to the MSP.
  if (!(cpu_state->extended.cfsr & kCfsrStkerrMask) &&
#if _PW_ARCH_ARM_V8M_MAINLINE
      !(cpu_state->extended.cfsr & kCfsrStkofMask) &&
#endif  // _PW_ARCH_ARM_V8M_MAINLINE
      !(cpu_state->extended.cfsr & kCfsrMstkerrMask)) {
    // TODO(amontanez): {r0-r3,r12} are captured in pw_cpu_exception_Entry(),
    //                  so this only really needs to copy pc, lr, and psr. Could
    //                  (possibly) improve speed, but would add marginally more
    //                  complexity.
    std::memcpy(&cpu_state->base,
                reinterpret_cast<void*>(cpu_state->extended.psp),
                sizeof(ExceptionRegisters));
  } else {
    // If CPU context wasn't pushed to stack on exception entry, we can't
    // recover psr, lr, and pc from exception-time. Make these values clearly
    // invalid.
    cpu_state->base.lr = kUndefinedPcLrOrPsrRegValue;
    cpu_state->base.pc = kUndefinedPcLrOrPsrRegValue;
    cpu_state->base.psr = kUndefinedPcLrOrPsrRegValue;
  }
}

// If the CPU successfully pushed context on exception, restore it from
// cpu_state. Otherwise, don't attempt to restore state.
//
// For more information see (See ARMv7-M Section B1.5.11, derived exceptions
// on exception entry).
void RestoreBaseRegistersToPsp(pw_cpu_exception_State* cpu_state) {
  // If CPU succeeded in pushing context to PSP on exception entry, restore the
  // contents of cpu_state to the CPU-pushed register frame so the CPU can
  // continue. Otherwise, don't attempt as we'll likely end up in an escalated
  // hard fault.
  if (!(cpu_state->extended.cfsr & kCfsrStkerrMask) &&
#if _PW_ARCH_ARM_V8M_MAINLINE
      !(cpu_state->extended.cfsr & kCfsrStkofMask) &&
#endif  // _PW_ARCH_ARM_V8M_MAINLINE
      !(cpu_state->extended.cfsr & kCfsrMstkerrMask)) {
    std::memcpy(reinterpret_cast<void*>(cpu_state->extended.psp),
                &cpu_state->base,
                sizeof(ExceptionRegisters));
  }
}

// Determines the size of the CPU-pushed context frame.
uint32_t CpuContextSize(const pw_cpu_exception_State& cpu_state) {
  uint32_t cpu_context_size = sizeof(ExceptionRegisters);
  if (FpuStateWasPushed(cpu_state)) {
    cpu_context_size += sizeof(ExceptionRegistersFpu);
  }
  if (cpu_state.base.psr & kPsrExtraStackAlignBit) {
    // Account for the extra 4-bytes the processor
    // added to keep the stack pointer 8-byte aligned
    cpu_context_size += 4;
  }

  return cpu_context_size;
}

// On exception entry, the Program Stack Pointer is patched to reflect the state
// at exception-time. On exception return, it is restored to the appropriate
// location. This calculates the delta that is used for these patch operations.
uint32_t CalculatePspDelta(const pw_cpu_exception_State& cpu_state) {
  // If CPU context was not pushed to program stack (because program stack
  // wasn't in use, or an error occurred when pushing context), the PSP doesn't
  // need to be shifted.
  if (!ProcessStackActive(cpu_state) ||
      (cpu_state.extended.cfsr & kCfsrStkerrMask) ||
#if _PW_ARCH_ARM_V8M_MAINLINE
      (cpu_state.extended.cfsr & kCfsrStkofMask) ||
#endif  // _PW_ARCH_ARM_V8M_MAINLINE
      (cpu_state.extended.cfsr & kCfsrMstkerrMask)) {
    return 0;
  }

  return CpuContextSize(cpu_state);
}

// On exception entry, the Main Stack Pointer is patched to reflect the state
// at exception-time. On exception return, it is restored to the appropriate
// location. This calculates the delta that is used for these patch operations.
uint32_t CalculateMspDelta(const pw_cpu_exception_State& cpu_state) {
  if (ProcessStackActive(cpu_state)) {
    // TODO(amontanez): Since FPU state isn't captured at this time, we ignore
    //                  it when patching MSP. To add FPU capture support,
    //                  delete this if block as CpuContextSize() will include
    //                  FPU context size in the calculation.
    return sizeof(ExceptionRegisters) + sizeof(ExtraRegisters);
  }

  return CpuContextSize(cpu_state) + sizeof(ExtraRegisters);
}

}  // namespace

extern "C" {

// Collect remaining CPU state (memory mapped registers), populate memory mapped
// registers, and call application exception handler.
PW_USED void pw_PackageAndHandleCpuException(
    pw_cpu_exception_State* cpu_state) {
  // Capture memory mapped registers.
  cpu_state->extended.cfsr = cortex_m_cfsr;
  cpu_state->extended.mmfar = cortex_m_mmfar;
  cpu_state->extended.bfar = cortex_m_bfar;
  cpu_state->extended.icsr = cortex_m_icsr;
  cpu_state->extended.hfsr = cortex_m_hfsr;
  cpu_state->extended.shcsr = cortex_m_shcsr;

  // CPU may have automatically pushed state to the program stack. If it did,
  // the values can be copied into in the pw_cpu_exception_State struct that is
  // passed to HandleCpuException(). The cpu_state passed to the handler is
  // ALWAYS stored on the main stack (MSP).
  if (ProcessStackActive(*cpu_state)) {
    CloneBaseRegistersFromPsp(cpu_state);
    // If PSP wasn't active, this delta is 0.
    cpu_state->extended.psp += CalculatePspDelta(*cpu_state);
  }

  // Patch captured stack pointers so they reflect the state at exception time.
  cpu_state->extended.msp += CalculateMspDelta(*cpu_state);

  // Call application-level exception handler.
  pw_cpu_exception_HandleException(cpu_state);

  // Restore program stack pointer so exception return can restore state if
  // needed.
  // Note: The default behavior of NOT subtracting a delta from MSP is
  // intentional. This simplifies the assembly to pop the exception state
  // off the main stack on exception return (since MSP currently reflects
  // exception-time state).
  cpu_state->extended.psp -= CalculatePspDelta(*cpu_state);

  // If PSP was active and the CPU pushed a context frame, we must copy the
  // potentially modified state from cpu_state back to the PSP so the CPU can
  // resume execution with the modified values.
  if (ProcessStackActive(*cpu_state)) {
    // In this case, there's no need to touch the MSP as it's at the location
    // before we entering the exception (effectively popping the state initially
    // pushed to the main stack).
    RestoreBaseRegistersToPsp(cpu_state);
  } else {
    // Since we're restoring context from MSP, we DO need to adjust MSP to point
    // to CPU-pushed context frame so it can be properly restored.
    // No need to adjust PSP since nothing was pushed to program stack.
    cpu_state->extended.msp -= CpuContextSize(*cpu_state);
  }
}

// Captures faulting CPU state on the main stack (MSP), then calls the exception
// handlers.
// This function should be called immediately after an exception.
void pw_cpu_exception_Entry(void) {
  asm volatile(
      // clang-format off
      // If PSP was in use at the time of exception, it's possible the CPU
      // wasn't able to push CPU state. To be safe, this first captures scratch
      // registers before moving forward.
      //
      // Stack flag is bit index 2 (0x4) of exc_return value stored in lr. When
      // this bit is set, the Process Stack Pointer (PSP) was in use. Otherwise,
      // the Main Stack Pointer (MSP) was in use. (See ARMv7-M Section B1.5.8
      // for more details)
      // The following block of assembly is equivalent to:
      //   if (lr & (1 << 2)) {
      //     msp -= sizeof(ExceptionRegisters);
      //     ExceptionRegisters* state =
      //         (ExceptionRegisters*) msp;
      //     state->r0 = r0;
      //     state->r1 = r1;
      //     state->r2 = r2;
      //     state->r3 = r3;
      //     state->r12 = r12;
      //   }
      //
      " tst lr, #(1 << 2)                                     \n"
      " itt ne                                                \n"
      " subne sp, sp, %[base_state_size]                      \n"
      " stmne sp, {r0-r3, r12}                                \n"

      // Reserve stack space for additional registers. Since we're in exception
      // handler mode, the main stack pointer is currently in use.
      // r0 will temporarily store the end of captured_cpu_state to simplify
      // assembly for copying additional registers.
      " mrs r0, msp                                           \n"
      " sub sp, sp, %[extra_state_size]                       \n"

      // Store GPRs to stack.
      " stmdb r0!, {r4-r11}                                   \n"

      // Load special registers.
      " mov r1, lr                                            \n"
      " mrs r2, msp                                           \n"
      " mrs r3, psp                                           \n"
      " mrs r4, control                                       \n"

#if _PW_ARCH_ARM_V7M || _PW_ARCH_ARM_V7EM
      // Store special registers to stack.
      " stmdb r0!, {r1-r4}                                    \n"

#elif _PW_ARCH_ARM_V8M_MAINLINE
      // Load ARMv8-M specific special registers.
      " mrs r5, msplim                                        \n"
      " mrs r6, psplim                                        \n"

      // Store special registers to stack.
      " stmdb r0!, {r1-r6}                                    \n"
#else
#error "Support required for your Cortex-M Arch"
#endif  // defined(PW_CPU_EXCEPTION_CORTEX_M_ARMV7M)

      // Store a pointer to the beginning of special registers in r4 so they can
      // be restored later.
      " mov r4, r0                                            \n"

      // Restore captured_cpu_state pointer to r0. This makes adding more
      // memory mapped registers easier in the future since they're skipped in
      // this assembly.
      " mrs r0, msp                                           \n"

      // Call intermediate handler that packages data.
      " ldr r3, =pw_PackageAndHandleCpuException              \n"
      " blx r3                                                \n"

      // Restore state and exit exception handler.
      // Pointer to saved CPU state was stored in r4.
      " mov r0, r4                                            \n"

      // Restore special registers.
      " ldm r0!, {r1-r4}                                      \n"
      " mov lr, r1                                            \n"
      " msr control, r4                                       \n"

      // Restore GPRs.
      " ldm r0, {r4-r11}                                      \n"

      // Restore stack pointers.
      " msr msp, r2                                           \n"
      " msr psp, r3                                           \n"

      // Exit exception.
      " bx lr                                                 \n"
      : /*output=*/
      : /*input=*/[base_state_size]"i"(sizeof(ExceptionRegisters)),
                  [extra_state_size]"i"(sizeof(ExtraRegisters))
      // clang-format on
  );
}

}  // extern "C"
}  // namespace pw::cpu_exception::cortex_m
