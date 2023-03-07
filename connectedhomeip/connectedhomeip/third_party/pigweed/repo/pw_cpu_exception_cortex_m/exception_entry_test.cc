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

#include <cstdint>
#include <type_traits>

#include "gtest/gtest.h"
#include "pw_cpu_exception/entry.h"
#include "pw_cpu_exception/handler.h"
#include "pw_cpu_exception/support.h"
#include "pw_cpu_exception_cortex_m/cpu_state.h"
#include "pw_cpu_exception_cortex_m_private/cortex_m_constants.h"
#include "pw_span/span.h"

namespace pw::cpu_exception::cortex_m {
namespace {

using pw::cpu_exception::RawFaultingCpuState;

// CMSIS/Cortex-M/ARMv7 related constants.
// These values are from the ARMv7-M Architecture Reference Manual DDI 0403E.b.
// https://static.docs.arm.com/ddi0403/e/DDI0403E_B_armv7m_arm.pdf

// CCR flags. (ARMv7-M Section B3.2.8)
constexpr uint32_t kUnalignedTrapEnableMask = 0x1u << 3;
constexpr uint32_t kDivByZeroTrapEnableMask = 0x1u << 4;

// Masks for individual bits of SHCSR. (ARMv7-M Section B3.2.13)
constexpr uint32_t kMemFaultEnableMask = 0x1 << 16;
constexpr uint32_t kBusFaultEnableMask = 0x1 << 17;
constexpr uint32_t kUsageFaultEnableMask = 0x1 << 18;

// CPCAR mask that enables FPU. (ARMv7-M Section B3.2.20)
constexpr uint32_t kFpuEnableMask = (0xFu << 20);

// Memory mapped registers. (ARMv7-M Section B3.2.2, Table B3-4)
volatile uint32_t& cortex_m_vtor =
    *reinterpret_cast<volatile uint32_t*>(0xE000ED08u);
volatile uint32_t& cortex_m_ccr =
    *reinterpret_cast<volatile uint32_t*>(0xE000ED14u);
volatile uint32_t& cortex_m_cpacr =
    *reinterpret_cast<volatile uint32_t*>(0xE000ED88u);

// Begin a critical section that must not be interrupted.
// This function disables interrupts to prevent any sort of context switch until
// the critical section ends. This is done by setting PRIMASK to 1 using the cps
// instruction.
//
// Returns the state of PRIMASK before it was disabled.
inline uint32_t BeginCriticalSection() {
  uint32_t previous_state;
  asm volatile(
      " mrs %[previous_state], primask              \n"
      " cpsid i                                     \n"
      // clang-format off
      : /*output=*/[previous_state]"=r"(previous_state)
      : /*input=*/
      : /*clobbers=*/"memory"
      // clang-format on
  );
  return previous_state;
}

// Ends a critical section.
// Restore previous previous state produced by BeginCriticalSection().
// Note: This does not always re-enable interrupts.
inline void EndCriticalSection(uint32_t previous_state) {
  asm volatile(
      // clang-format off
      "msr primask, %0"
      : /*output=*/
      : /*input=*/"r"(previous_state)
      : /*clobbers=*/"memory"
      // clang-format on
  );
}

void EnableFpu() {
#if defined(PW_ARMV7M_ENABLE_FPU) && PW_ARMV7M_ENABLE_FPU == 1
  // TODO(pwbug/17): Replace when Pigweed config system is added.
  cortex_m_cpacr |= kFpuEnableMask;
#endif  // defined(PW_ARMV7M_ENABLE_FPU) && PW_ARMV7M_ENABLE_FPU == 1
}

void DisableFpu() {
#if defined(PW_ARMV7M_ENABLE_FPU) && PW_ARMV7M_ENABLE_FPU == 1
  // TODO(pwbug/17): Replace when Pigweed config system is added.
  cortex_m_cpacr &= ~kFpuEnableMask;
#endif  // defined(PW_ARMV7M_ENABLE_FPU) && PW_ARMV7M_ENABLE_FPU == 1
}

// Counter that is incremented if the test's exception handler correctly handles
// a triggered exception.
size_t exceptions_handled = 0;

// Global variable that triggers a single nested fault on a fault.
bool trigger_nested_fault = false;

// Allow up to kMaxFaultDepth faults before determining the device is
// unrecoverable.
constexpr size_t kMaxFaultDepth = 2;

// Variable to prevent more than kMaxFaultDepth nested crashes.
size_t current_fault_depth = 0;

// Faulting pw_cpu_exception_State is copied here so values can be validated
// after exiting exception handler.
pw_cpu_exception_State captured_states[kMaxFaultDepth] = {};
pw_cpu_exception_State& captured_state = captured_states[0];

// Flag used to check if the contents of span matches the captured state.
bool span_matches = false;

// Variable to be manipulated by function that uses floating
// point to test that exceptions push Fpu state correctly.
// Note: don't use double because a cortex-m4f with fpv4-sp-d16
// will result in gcc generating code to use the software floating
// point support for double.
volatile float float_test_value;

// Magic pattern to help identify if the exception handler's
// pw_cpu_exception_State pointer was pointing to captured CPU state that was
// pushed onto the stack when the faulting context uses the VFP. Has to be
// computed at runtime because it uses values only available at link time.
const float kFloatTestPattern = 12.345f * 67.89f;

volatile float fpu_lhs_val = 12.345f;
volatile float fpu_rhs_val = 67.89f;

// This macro provides a calculation that equals kFloatTestPattern.
#define _PW_TEST_FPU_OPERATION (fpu_lhs_val * fpu_rhs_val)

// Magic pattern to help identify if the exception handler's
// pw_cpu_exception_State pointer was pointing to captured CPU state that was
// pushed onto the stack.
constexpr uint32_t kMagicPattern = 0xDEADBEEF;

// This pattern serves a purpose similar to kMagicPattern, but is used for
// testing a nested fault to ensure both pw_cpu_exception_State objects are
// correctly captured.
constexpr uint32_t kNestedMagicPattern = 0x900DF00D;

// The manually captured PC won't be the exact same as the faulting PC. This is
// the maximum tolerated distance between the two to allow the test to pass.
constexpr int32_t kMaxPcDistance = 4;

// In-memory interrupt service routine vector table.
using InterruptVectorTable = std::aligned_storage_t<512, 512>;
InterruptVectorTable ram_vector_table;

// Forward declaration of the exception handler.
void TestingExceptionHandler(pw_cpu_exception_State*);

// Populate the device's registers with testable values, then trigger exception.
void BeginBaseFaultTest() {
  // Make sure divide by zero causes a fault.
  cortex_m_ccr |= kDivByZeroTrapEnableMask;
  uint32_t magic = kMagicPattern;
  asm volatile(
      " mov r0, %[magic]                                      \n"
      " mov r1, #0                                            \n"
      " mov r2, pc                                            \n"
      " mov r3, lr                                            \n"
      // This instruction divides by zero.
      " udiv r1, r1, r1                                       \n"
      // clang-format off
      : /*output=*/
      : /*input=*/[magic]"r"(magic)
      : /*clobbers=*/"r0", "r1", "r2", "r3"
      // clang-format on
  );

  // Check that the stack align bit was not set.
  EXPECT_EQ(captured_state.base.psr & kPsrExtraStackAlignBit, 0u);
}

// Populate the device's registers with testable values, then trigger exception.
void BeginNestedFaultTest() {
  // Make sure divide by zero causes a fault.
  cortex_m_ccr |= kUnalignedTrapEnableMask;
  volatile uint32_t magic = kNestedMagicPattern;
  asm volatile(
      " mov r0, %[magic]                                      \n"
      " mov r1, #0                                            \n"
      " mov r2, pc                                            \n"
      " mov r3, lr                                            \n"
      // This instruction does an unaligned read.
      " ldrh r1, [%[magic_addr], 1]                           \n"
      // clang-format off
      : /*output=*/
      : /*input=*/[magic]"r"(magic), [magic_addr]"r"(&magic)
      : /*clobbers=*/"r0", "r1", "r2", "r3"
      // clang-format on
  );
}

// Populate the device's registers with testable values, then trigger exception.
// This version causes stack to not be 4-byte aligned initially, testing
// the fault handlers correction for psp.
void BeginBaseFaultUnalignedStackTest() {
  // Make sure divide by zero causes a fault.
  cortex_m_ccr |= kDivByZeroTrapEnableMask;
  uint32_t magic = kMagicPattern;
  asm volatile(
      // Push one register to cause $sp to be no longer 8-byte aligned,
      // assuming it started 8-byte aligned as expected.
      " push {r0}                                             \n"
      " mov r0, %[magic]                                      \n"
      " mov r1, #0                                            \n"
      " mov r2, pc                                            \n"
      " mov r3, lr                                            \n"
      // This instruction divides by zero. Our fault handler should
      // ultimately advance the pc to the pop instruction.
      " udiv r1, r1, r1                                       \n"
      " pop {r0}                                              \n"
      // clang-format off
      : /*output=*/
      : /*input=*/[magic]"r"(magic)
      : /*clobbers=*/"r0", "r1", "r2", "r3"
      // clang-format on
  );

  // Check that the stack align bit was set.
  EXPECT_EQ(captured_state.base.psr & kPsrExtraStackAlignBit,
            kPsrExtraStackAlignBit);
}

// Populate some of the extended set of captured registers, then trigger
// exception.
void BeginExtendedFaultTest() {
  // Make sure divide by zero causes a fault.
  cortex_m_ccr |= kDivByZeroTrapEnableMask;
  uint32_t magic = kMagicPattern;
  volatile uint32_t local_msp = 0;
  volatile uint32_t local_psp = 0;
  asm volatile(
      " mov r4, %[magic]                                      \n"
      " mov r5, #0                                            \n"
      " mov r11, %[magic]                                     \n"
      " mrs %[local_msp], msp                                 \n"
      " mrs %[local_psp], psp                                 \n"
      // This instruction divides by zero.
      " udiv r5, r5, r5                                       \n"
      // clang-format off
      : /*output=*/[local_msp]"=r"(local_msp), [local_psp]"=r"(local_psp)
      : /*input=*/[magic]"r"(magic)
      : /*clobbers=*/"r0", "r4", "r5", "r11", "memory"
      // clang-format on
  );

  // Check that the stack align bit was not set.
  EXPECT_EQ(captured_state.base.psr & kPsrExtraStackAlignBit, 0u);

  // Check that the captured stack pointers matched the ones in the context of
  // the fault.
  EXPECT_EQ(static_cast<uint32_t>(captured_state.extended.msp), local_msp);
  EXPECT_EQ(static_cast<uint32_t>(captured_state.extended.psp), local_psp);
}

// Populate some of the extended set of captured registers, then trigger
// exception.
// This version causes stack to not be 4-byte aligned initially, testing
// the fault handlers correction for psp.
void BeginExtendedFaultUnalignedStackTest() {
  // Make sure divide by zero causes a fault.
  cortex_m_ccr |= kDivByZeroTrapEnableMask;
  uint32_t magic = kMagicPattern;
  volatile uint32_t local_msp = 0;
  volatile uint32_t local_psp = 0;
  asm volatile(
      // Push one register to cause $sp to be no longer 8-byte aligned,
      // assuming it started 8-byte aligned as expected.
      " push {r0}                                             \n"
      " mov r4, %[magic]                                      \n"
      " mov r5, #0                                            \n"
      " mov r11, %[magic]                                     \n"
      " mrs %[local_msp], msp                                 \n"
      " mrs %[local_psp], psp                                 \n"
      // This instruction divides by zero. Our fault handler should
      // ultimately advance the pc to the pop instruction.
      " udiv r5, r5, r5                                       \n"
      " pop {r0}                                              \n"
      // clang-format off
      : /*output=*/[local_msp]"=r"(local_msp), [local_psp]"=r"(local_psp)
      : /*input=*/[magic]"r"(magic)
      : /*clobbers=*/"r0", "r4", "r5", "r11", "memory"
      // clang-format on
  );

  // Check that the stack align bit was set.
  EXPECT_EQ(captured_state.base.psr & kPsrExtraStackAlignBit,
            kPsrExtraStackAlignBit);

  // Check that the captured stack pointers matched the ones in the context of
  // the fault.
  EXPECT_EQ(static_cast<uint32_t>(captured_state.extended.msp), local_msp);
  EXPECT_EQ(static_cast<uint32_t>(captured_state.extended.psp), local_psp);
}

void InstallVectorTableEntries() {
  uint32_t prev_state = BeginCriticalSection();
  // If vector table is installed already, this is done.
  if (cortex_m_vtor == reinterpret_cast<uint32_t>(&ram_vector_table)) {
    EndCriticalSection(prev_state);
    return;
  }
  // Copy table to new location since it's not guaranteed that we can write to
  // the original one.
  std::memcpy(&ram_vector_table,
              reinterpret_cast<uint32_t*>(cortex_m_vtor),
              sizeof(ram_vector_table));

  // Override exception handling vector table entries.
  uint32_t* exception_entry_addr =
      reinterpret_cast<uint32_t*>(pw_cpu_exception_Entry);
  uint32_t** interrupts = reinterpret_cast<uint32_t**>(&ram_vector_table);
  interrupts[kHardFaultIsrNum] = exception_entry_addr;
  interrupts[kMemFaultIsrNum] = exception_entry_addr;
  interrupts[kBusFaultIsrNum] = exception_entry_addr;
  interrupts[kUsageFaultIsrNum] = exception_entry_addr;

  // Update Vector Table Offset Register (VTOR) to point to new vector table.
  cortex_m_vtor = reinterpret_cast<uint32_t>(&ram_vector_table);
  EndCriticalSection(prev_state);
}

void EnableAllFaultHandlers() {
  cortex_m_shcsr |=
      kMemFaultEnableMask | kBusFaultEnableMask | kUsageFaultEnableMask;
}

void Setup(bool use_fpu) {
  if (use_fpu) {
    EnableFpu();
  } else {
    DisableFpu();
  }
  pw_cpu_exception_SetHandler(TestingExceptionHandler);
  EnableAllFaultHandlers();
  InstallVectorTableEntries();
  exceptions_handled = 0;
  current_fault_depth = 0;
  captured_state = {};
  float_test_value = 0.0f;
  trigger_nested_fault = false;
}

TEST(FaultEntry, BasicFault) {
  Setup(/*use_fpu=*/false);
  BeginBaseFaultTest();
  ASSERT_EQ(exceptions_handled, 1u);
  // captured_state values must be cast since they're in a packed struct.
  EXPECT_EQ(static_cast<uint32_t>(captured_state.base.r0), kMagicPattern);
  EXPECT_EQ(static_cast<uint32_t>(captured_state.base.r1), 0u);
  // PC is manually saved in r2 before the exception occurs (where PC is also
  // stored). Ensure these numbers are within a reasonable distance.
  int32_t captured_pc_distance =
      captured_state.base.pc - captured_state.base.r2;
  EXPECT_LT(captured_pc_distance, kMaxPcDistance);
  EXPECT_EQ(static_cast<uint32_t>(captured_state.base.r3),
            static_cast<uint32_t>(captured_state.base.lr));
}

TEST(FaultEntry, BasicUnalignedStackFault) {
  Setup(/*use_fpu=*/false);
  BeginBaseFaultUnalignedStackTest();
  ASSERT_EQ(exceptions_handled, 1u);
  // captured_state values must be cast since they're in a packed struct.
  EXPECT_EQ(static_cast<uint32_t>(captured_state.base.r0), kMagicPattern);
  EXPECT_EQ(static_cast<uint32_t>(captured_state.base.r1), 0u);
  // PC is manually saved in r2 before the exception occurs (where PC is also
  // stored). Ensure these numbers are within a reasonable distance.
  int32_t captured_pc_distance =
      captured_state.base.pc - captured_state.base.r2;
  EXPECT_LT(captured_pc_distance, kMaxPcDistance);
  EXPECT_EQ(static_cast<uint32_t>(captured_state.base.r3),
            static_cast<uint32_t>(captured_state.base.lr));
}

TEST(FaultEntry, ExtendedFault) {
  Setup(/*use_fpu=*/false);
  BeginExtendedFaultTest();
  ASSERT_EQ(exceptions_handled, 1u);
  ASSERT_TRUE(span_matches);
  const ExtraRegisters& extended_registers = captured_state.extended;
  // captured_state values must be cast since they're in a packed struct.
  EXPECT_EQ(static_cast<uint32_t>(extended_registers.r4), kMagicPattern);
  EXPECT_EQ(static_cast<uint32_t>(extended_registers.r5), 0u);
  EXPECT_EQ(static_cast<uint32_t>(extended_registers.r11), kMagicPattern);

  // Check expected values for this crash.
  EXPECT_EQ(static_cast<uint32_t>(extended_registers.cfsr),
            static_cast<uint32_t>(kCfsrDivbyzeroMask));
  EXPECT_EQ((extended_registers.icsr & 0x1FFu), kUsageFaultIsrNum);
}

TEST(FaultEntry, ExtendedUnalignedStackFault) {
  Setup(/*use_fpu=*/false);
  BeginExtendedFaultUnalignedStackTest();
  ASSERT_EQ(exceptions_handled, 1u);
  ASSERT_TRUE(span_matches);
  const ExtraRegisters& extended_registers = captured_state.extended;
  // captured_state values must be cast since they're in a packed struct.
  EXPECT_EQ(static_cast<uint32_t>(extended_registers.r4), kMagicPattern);
  EXPECT_EQ(static_cast<uint32_t>(extended_registers.r5), 0u);
  EXPECT_EQ(static_cast<uint32_t>(extended_registers.r11), kMagicPattern);

  // Check expected values for this crash.
  EXPECT_EQ(static_cast<uint32_t>(extended_registers.cfsr),
            static_cast<uint32_t>(kCfsrDivbyzeroMask));
  EXPECT_EQ((extended_registers.icsr & 0x1FFu), kUsageFaultIsrNum);
}

TEST(FaultEntry, NestedFault) {
  // Due to the way nesting is handled, captured_states[0] is the nested fault
  // since that fault must be handled *FIRST*. After that fault is handled, the
  // original fault can be correctly handled afterwards (captured into
  // captured_states[1]).

  Setup(/*use_fpu=*/false);
  trigger_nested_fault = true;
  BeginBaseFaultTest();
  ASSERT_EQ(exceptions_handled, 2u);

  // captured_state values must be cast since they're in a packed struct.
  EXPECT_EQ(static_cast<uint32_t>(captured_states[1].base.r0), kMagicPattern);
  EXPECT_EQ(static_cast<uint32_t>(captured_states[1].base.r1), 0u);
  // PC is manually saved in r2 before the exception occurs (where PC is also
  // stored). Ensure these numbers are within a reasonable distance.
  int32_t captured_pc_distance =
      captured_states[1].base.pc - captured_states[1].base.r2;
  EXPECT_LT(captured_pc_distance, kMaxPcDistance);
  EXPECT_EQ(static_cast<uint32_t>(captured_states[1].base.r3),
            static_cast<uint32_t>(captured_states[1].base.lr));

  // NESTED STATE
  // captured_state values must be cast since they're in a packed struct.
  EXPECT_EQ(static_cast<uint32_t>(captured_states[0].base.r0),
            kNestedMagicPattern);
  EXPECT_EQ(static_cast<uint32_t>(captured_states[0].base.r1), 0u);
  // PC is manually saved in r2 before the exception occurs (where PC is also
  // stored). Ensure these numbers are within a reasonable distance.
  captured_pc_distance =
      captured_states[0].base.pc - captured_states[0].base.r2;
  EXPECT_LT(captured_pc_distance, kMaxPcDistance);
  EXPECT_EQ(static_cast<uint32_t>(captured_states[0].base.r3),
            static_cast<uint32_t>(captured_states[0].base.lr));
}

// TODO(pwbug/17): Replace when Pigweed config system is added.
// Disable tests that rely on hardware FPU if this module wasn't built with
// hardware FPU support.
#if defined(PW_ARMV7M_ENABLE_FPU) && PW_ARMV7M_ENABLE_FPU == 1

// Populate some of the extended set of captured registers, then trigger
// exception. This function uses floating point to validate float context
// is pushed correctly.
void BeginExtendedFaultFloatTest() {
  float_test_value = _PW_TEST_FPU_OPERATION;
  BeginExtendedFaultTest();
}

// Populate some of the extended set of captured registers, then trigger
// exception.
// This version causes stack to not be 4-byte aligned initially, testing
// the fault handlers correction for psp.
// This function uses floating point to validate float context
// is pushed correctly.
void BeginExtendedFaultUnalignedStackFloatTest() {
  float_test_value = _PW_TEST_FPU_OPERATION;
  BeginExtendedFaultUnalignedStackTest();
}

TEST(FaultEntry, FloatFault) {
  Setup(/*use_fpu=*/true);
  BeginExtendedFaultFloatTest();
  ASSERT_EQ(exceptions_handled, 1u);
  const ExtraRegisters& extended_registers = captured_state.extended;
  // captured_state values must be cast since they're in a packed struct.
  EXPECT_EQ(static_cast<uint32_t>(extended_registers.r4), kMagicPattern);
  EXPECT_EQ(static_cast<uint32_t>(extended_registers.r5), 0u);
  EXPECT_EQ(static_cast<uint32_t>(extended_registers.r11), kMagicPattern);

  // Check expected values for this crash.
  EXPECT_EQ(static_cast<uint32_t>(extended_registers.cfsr),
            static_cast<uint32_t>(kCfsrDivbyzeroMask));
  EXPECT_EQ((extended_registers.icsr & 0x1FFu), kUsageFaultIsrNum);

  // Check fpu state was pushed during exception
  EXPECT_FALSE(extended_registers.exc_return & kExcReturnBasicFrameMask);

  // Check float_test_value is correct
  EXPECT_EQ(float_test_value, kFloatTestPattern);
}

TEST(FaultEntry, FloatUnalignedStackFault) {
  Setup(/*use_fpu=*/true);
  BeginExtendedFaultUnalignedStackFloatTest();
  ASSERT_EQ(exceptions_handled, 1u);
  ASSERT_TRUE(span_matches);
  const ExtraRegisters& extended_registers = captured_state.extended;
  // captured_state values must be cast since they're in a packed struct.
  EXPECT_EQ(static_cast<uint32_t>(extended_registers.r4), kMagicPattern);
  EXPECT_EQ(static_cast<uint32_t>(extended_registers.r5), 0u);
  EXPECT_EQ(static_cast<uint32_t>(extended_registers.r11), kMagicPattern);

  // Check expected values for this crash.
  EXPECT_EQ(static_cast<uint32_t>(extended_registers.cfsr),
            static_cast<uint32_t>(kCfsrDivbyzeroMask));
  EXPECT_EQ((extended_registers.icsr & 0x1FFu), kUsageFaultIsrNum);

  // Check fpu state was pushed during exception.
  EXPECT_FALSE(extended_registers.exc_return & kExcReturnBasicFrameMask);

  // Check float_test_value is correct
  EXPECT_EQ(float_test_value, kFloatTestPattern);
}

#endif  // defined(PW_ARMV7M_ENABLE_FPU) && PW_ARMV7M_ENABLE_FPU == 1

void TestingExceptionHandler(pw_cpu_exception_State* state) {
  if (++current_fault_depth > kMaxFaultDepth) {
    volatile bool loop = true;
    while (loop) {
      // Hit unexpected nested crash, prevent further nesting.
    }
  }

  if (trigger_nested_fault) {
    // Disable nesting before triggering the nested fault to prevent infinite
    // recursive crashes.
    trigger_nested_fault = false;
    BeginNestedFaultTest();
  }
  // Logging may require FPU (fpu instructions in vsnprintf()), so re-enable
  // asap.
  EnableFpu();

  // Disable traps. Must be disabled before EXPECT, as memcpy() can do unaligned
  // operations.
  cortex_m_ccr &= ~kUnalignedTrapEnableMask;
  cortex_m_ccr &= ~kDivByZeroTrapEnableMask;

  // Clear HFSR forced (nested) hard fault mask if set. This will only be
  // set by the nested fault test.
  EXPECT_EQ(state->extended.hfsr, cortex_m_hfsr);
  if (cortex_m_hfsr & kHfsrForcedMask) {
    cortex_m_hfsr = kHfsrForcedMask;
  }

  if (cortex_m_cfsr & kCfsrUnalignedMask) {
    // Copy captured state to check later.
    std::memcpy(&captured_states[exceptions_handled],
                state,
                sizeof(pw_cpu_exception_State));

    // Disable unaligned read/write trapping to "handle" exception.
    cortex_m_cfsr = kCfsrUnalignedMask;
    exceptions_handled++;
    return;
  } else if (cortex_m_cfsr & kCfsrDivbyzeroMask) {
    // Copy captured state to check later.
    std::memcpy(&captured_states[exceptions_handled],
                state,
                sizeof(pw_cpu_exception_State));

    // Ensure span compares to be the same.
    span<const uint8_t> state_span = RawFaultingCpuState(*state);
    EXPECT_EQ(state_span.size(), sizeof(pw_cpu_exception_State));
    if (std::memcmp(state, state_span.data(), state_span.size()) == 0) {
      span_matches = true;
    } else {
      span_matches = false;
    }

    // Disable divide-by-zero trapping to "handle" exception.
    cortex_m_cfsr = kCfsrDivbyzeroMask;
    exceptions_handled++;
    return;
  }

  EXPECT_EQ(state->extended.shcsr, cortex_m_shcsr);

  // If an unexpected exception occurred, just enter an infinite loop.
  while (true) {
  }
}

}  // namespace
}  // namespace pw::cpu_exception::cortex_m
