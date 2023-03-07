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

#include "pw_cpu_exception_cortex_m/util.h"

#include <cinttypes>

#include "pw_cpu_exception_cortex_m/cpu_state.h"
#include "pw_cpu_exception_cortex_m_private/config.h"
#include "pw_cpu_exception_cortex_m_private/cortex_m_constants.h"
#include "pw_log/log.h"
#include "pw_preprocessor/arch.h"

namespace pw::cpu_exception::cortex_m {
namespace {

[[maybe_unused]] void LogCfsrAnalysis(const uint32_t cfsr) {
  if (cfsr == 0) {
    return;
  }

  PW_LOG_INFO("Active CFSR fields:");

  // Memory managment fault fields.
  if (cfsr & kCfsrIaccviolMask) {
    PW_LOG_ERROR("  IACCVIOL: MPU violation on instruction fetch");
  }
  if (cfsr & kCfsrDaccviolMask) {
    PW_LOG_ERROR("  DACCVIOL: MPU violation on memory read/write");
  }
  if (cfsr & kCfsrMunstkerrMask) {
    PW_LOG_ERROR("  MUNSTKERR: 'MPU violation on exception return");
  }
  if (cfsr & kCfsrMstkerrMask) {
    PW_LOG_ERROR("  MSTKERR: MPU violation on exception entry");
  }
  if (cfsr & kCfsrMlsperrMask) {
    PW_LOG_ERROR("  MLSPERR: MPU violation on lazy FPU state preservation");
  }
  if (cfsr & kCfsrMmarvalidMask) {
    PW_LOG_ERROR("  MMARVALID: MMFAR register is valid");
  }

  // Bus fault fields.
  if (cfsr & kCfsrIbuserrMask) {
    PW_LOG_ERROR("  IBUSERR: Bus fault on instruction fetch");
  }
  if (cfsr & kCfsrPreciserrMask) {
    PW_LOG_ERROR("  PRECISERR: Precise bus fault");
  }
  if (cfsr & kCfsrImpreciserrMask) {
    PW_LOG_ERROR("  IMPRECISERR: Imprecise bus fault");
  }
  if (cfsr & kCfsrUnstkerrMask) {
    PW_LOG_ERROR("  UNSTKERR: Derived bus fault on exception context save");
  }
  if (cfsr & kCfsrStkerrMask) {
    PW_LOG_ERROR("  STKERR: Derived bus fault on exception context restore");
  }
  if (cfsr & kCfsrLsperrMask) {
    PW_LOG_ERROR("  LSPERR: Derived bus fault on lazy FPU state preservation");
  }
  if (cfsr & kCfsrBfarvalidMask) {
    PW_LOG_ERROR("  BFARVALID: BFAR register is valid");
  }

  // Usage fault fields.
  if (cfsr & kCfsrUndefinstrMask) {
    PW_LOG_ERROR("  UNDEFINSTR: Encountered invalid instruction");
  }
  if (cfsr & kCfsrInvstateMask) {
    PW_LOG_ERROR(
        "  INVSTATE: Attempted to execute an instruction with an invalid "
        "Execution Program Status Register (EPSR) value");
  }
  if (cfsr & kCfsrInvpcMask) {
    PW_LOG_ERROR("  INVPC: Program Counter (PC) is not legal");
  }
  if (cfsr & kCfsrNocpMask) {
    PW_LOG_ERROR("  NOCP: Coprocessor disabled or not present");
  }
  if (cfsr & kCfsrUnalignedMask) {
    PW_LOG_ERROR("  UNALIGNED: Unaligned memory access");
  }
  if (cfsr & kCfsrDivbyzeroMask) {
    PW_LOG_ERROR("  DIVBYZERO: Division by zero");
  }
#if _PW_ARCH_ARM_V8M_MAINLINE
  if (cfsr & kCfsrStkofMask) {
    PW_LOG_ERROR("  STKOF: Stack overflowed");
  }
#endif  // _PW_ARCH_ARM_V8M_MAINLINE
}

}  // namespace

void LogExceptionAnalysis(const pw_cpu_exception_State& cpu_state) {
  // This provides a high-level assessment of the cause of the exception.
  // These conditionals are ordered by priority to ensure the most critical
  // issues are highlighted first. These are not mutually exclusive; a bus fault
  // could occur during the handling of a MPU violation, causing a nested fault.
  if (cpu_state.extended.hfsr & kHfsrForcedMask) {
    PW_LOG_CRITICAL("Encountered a nested CPU fault (See active CFSR fields)");
  }
#if _PW_ARCH_ARM_V8M_MAINLINE
  if (cpu_state.extended.cfsr & kCfsrStkofMask) {
    if (ProcessStackActive(cpu_state)) {
      PW_LOG_CRITICAL("Encountered process stack overflow (psp)");
    } else {
      PW_LOG_CRITICAL("Encountered main stack overflow (msp)");
    }
  }
#endif  // _PW_ARCH_ARM_V8M_MAINLINE
  if (cpu_state.extended.cfsr & kCfsrMemFaultMask) {
    if (cpu_state.extended.cfsr & kCfsrMmarvalidMask) {
      PW_LOG_CRITICAL(
          "Encountered Memory Protection Unit (MPU) violation at 0x%08" PRIx32,
          cpu_state.extended.mmfar);
    } else {
      PW_LOG_CRITICAL("Encountered Memory Protection Unit (MPU) violation");
    }
  }
  if (cpu_state.extended.cfsr & kCfsrBusFaultMask) {
    if (cpu_state.extended.cfsr & kCfsrBfarvalidMask) {
      PW_LOG_CRITICAL("Encountered bus fault at 0x%08" PRIx32,
                      cpu_state.extended.bfar);
    } else {
      PW_LOG_CRITICAL("Encountered bus fault");
    }
  }
  if (cpu_state.extended.cfsr & kCfsrUsageFaultMask) {
    PW_LOG_CRITICAL("Encountered usage fault (See active CFSR fields)");
  }
  if ((cpu_state.extended.icsr & kIcsrVectactiveMask) == kNmiIsrNum) {
    PW_LOG_INFO("Encountered non-maskable interrupt (NMI)");
  }
#if PW_CPU_EXCEPTION_CORTEX_M_EXTENDED_CFSR_DUMP
  LogCfsrAnalysis(cpu_state.extended.cfsr);
#endif  // PW_CPU_EXCEPTION_CORTEX_M_EXTENDED_CFSR_DUMP
}

ProcessorMode ActiveProcessorMode(const pw_cpu_exception_State& cpu_state) {
  // See ARMv7-M Architecture Reference Manual Section B1.5.8 for the exception
  // return values, in particular bits 0:3.
  // Bits 0:3 of EXC_RETURN:
  // 0b0001 - 0x1 Handler mode Main
  // 0b1001 - 0x9 Thread mode Main
  // 0b1101 - 0xD Thread mode Process
  //   ^
  if (cpu_state.extended.exc_return & kExcReturnModeMask) {
    return ProcessorMode::kThreadMode;
  }
  return ProcessorMode::kHandlerMode;
}

bool MainStackActive(const pw_cpu_exception_State& cpu_state) {
  // See ARMv7-M Architecture Reference Manual Section B1.5.8 for the exception
  // return values, in particular bits 0:3.
  // Bits 0:3 of EXC_RETURN:
  // 0b0001 - 0x1 Handler mode Main
  // 0b1001 - 0x9 Thread mode Main
  // 0b1101 - 0xD Thread mode Process
  //    ^
  return (cpu_state.extended.exc_return & kExcReturnStackMask) == 0;
}

}  // namespace pw::cpu_exception::cortex_m
