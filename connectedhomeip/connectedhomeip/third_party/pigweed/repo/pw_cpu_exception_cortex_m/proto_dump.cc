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
#include "pw_cpu_exception_cortex_m/cpu_state.h"
#include "pw_cpu_exception_cortex_m_protos/cpu_state.pwpb.h"
#include "pw_preprocessor/compiler.h"
#include "pw_protobuf/encoder.h"

namespace pw::cpu_exception::cortex_m {

Status DumpCpuStateProto(protobuf::StreamEncoder& dest,
                         const pw_cpu_exception_State& cpu_state) {
  // Note that the encoder's status is checked at the end and ignored at the
  // Write*() calls to reduce the number of branches.
  cortex_m::ArmV7mCpuState::StreamEncoder& state_encoder =
      *static_cast<cortex_m::ArmV7mCpuState::StreamEncoder*>(&dest);

  const ExceptionRegisters& base = cpu_state.base;
  const ExtraRegisters& extended = cpu_state.extended;

  // Special and mem-mapped registers.
  if (base.pc != kUndefinedPcLrOrPsrRegValue) {
    state_encoder.WritePc(base.pc).IgnoreError();
  }
  if (base.lr != kUndefinedPcLrOrPsrRegValue) {
    state_encoder.WriteLr(base.lr).IgnoreError();
  }
  if (base.psr != kUndefinedPcLrOrPsrRegValue) {
    state_encoder.WritePsr(base.psr).IgnoreError();
  }
  state_encoder.WriteMsp(extended.msp).IgnoreError();
  state_encoder.WritePsp(extended.psp).IgnoreError();
  state_encoder.WriteExcReturn(extended.exc_return).IgnoreError();
  state_encoder.WriteCfsr(extended.cfsr).IgnoreError();
#if _PW_ARCH_ARM_V8M_MAINLINE
  state_encoder.WriteMsplim(extended.msplim).IgnoreError();
  state_encoder.WritePsplim(extended.psplim).IgnoreError();
#endif  // _PW_ARCH_ARM_V8M_MAINLINE
  state_encoder.WriteMmfar(extended.mmfar).IgnoreError();
  state_encoder.WriteBfar(extended.bfar).IgnoreError();
  state_encoder.WriteIcsr(extended.icsr).IgnoreError();
  state_encoder.WriteHfsr(extended.hfsr).IgnoreError();
  state_encoder.WriteShcsr(extended.shcsr).IgnoreError();
  state_encoder.WriteControl(extended.control).IgnoreError();

  // General purpose registers.
  state_encoder.WriteR0(base.r0).IgnoreError();
  state_encoder.WriteR1(base.r1).IgnoreError();
  state_encoder.WriteR2(base.r2).IgnoreError();
  state_encoder.WriteR3(base.r3).IgnoreError();
  state_encoder.WriteR4(extended.r4).IgnoreError();
  state_encoder.WriteR5(extended.r5).IgnoreError();
  state_encoder.WriteR6(extended.r6).IgnoreError();
  state_encoder.WriteR7(extended.r7).IgnoreError();
  state_encoder.WriteR8(extended.r8).IgnoreError();
  state_encoder.WriteR9(extended.r9).IgnoreError();
  state_encoder.WriteR10(extended.r10).IgnoreError();
  state_encoder.WriteR11(extended.r11).IgnoreError();
  state_encoder.WriteR12(base.r12).IgnoreError();

  // If the encode buffer was exhausted in an earlier write, it will be
  // reflected here.
  if (const Status status = state_encoder.status(); !status.ok()) {
    return status.IsResourceExhausted() ? Status::ResourceExhausted()
                                        : Status::Unknown();
  }
  return OkStatus();
}

}  // namespace pw::cpu_exception::cortex_m
