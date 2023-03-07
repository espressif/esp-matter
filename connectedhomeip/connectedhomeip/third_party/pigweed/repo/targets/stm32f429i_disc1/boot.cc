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

#include "pw_boot/boot.h"

#include "pw_boot_cortex_m/boot.h"
#include "pw_malloc/malloc.h"
#include "pw_preprocessor/compiler.h"
#include "pw_sys_io_baremetal_stm32f429/init.h"

// Note that constexpr is used inside of this function instead of using a static
// constexpr or declaring it outside of this function in an anonymous namespace,
// because constexpr makes it available for the compiler to evaluate during
// compile time but does NOT require it to be evaluated at compile time and we
// have to be incredibly careful that this does not end up in the .data section.
void pw_boot_PreStaticMemoryInit() {
  // TODO(pwbug/17): Optionally enable Replace when Pigweed config system is
  // added.
#if PW_ARMV7M_ENABLE_FPU
  // Enable FPU if built using hardware FPU instructions.
  // CPCAR mask that enables FPU. (ARMv7-M Section B3.2.20)
  constexpr uint32_t kFpuEnableMask = (0xFu << 20);

  // Memory mapped register to enable FPU. (ARMv7-M Section B3.2.2, Table B3-4)
  volatile uint32_t& arm_v7m_cpacr =
      *reinterpret_cast<volatile uint32_t*>(0xE000ED88u);
  arm_v7m_cpacr |= kFpuEnableMask;

  // Ensure the FPU configuration is committed and enabled before continuing and
  // potentially executing any FPU instructions, however rare that may be during
  // startup.
  asm volatile(
      " dsb \n"
      " isb \n"
      // clang-format off
      : /*output=*/
      : /*input=*/
      : /*clobbers=*/"memory"
      // clang-format on
  );
#endif  // PW_ARMV7M_ENABLE_FPU
}

void pw_boot_PreStaticConstructorInit() {
#if PW_MALLOC_ACTIVE
  pw_MallocInit(&pw_boot_heap_low_addr, &pw_boot_heap_high_addr);
#endif  // PW_MALLOC_ACTIVE
}

void pw_boot_PreMainInit() { pw_sys_io_stm32f429_Init(); }

PW_NO_RETURN void pw_boot_PostMain() {
  // In case main() returns, just sit here until the device is reset.
  while (true) {
  }
  PW_UNREACHABLE;
}
