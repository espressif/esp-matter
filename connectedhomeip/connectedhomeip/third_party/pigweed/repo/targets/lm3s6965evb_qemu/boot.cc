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

#include "pw_preprocessor/compiler.h"
#include "pw_sys_io_baremetal_lm3s6965evb/init.h"

// Note that constexpr is used inside of this function instead of using a static
// constexpr or declaring it outside of this function in an anonymous namespace,
// because constexpr makes it available for the compiler to evaluate during
// compile time but does NOT require it to be evaluated at compile time and we
// have to be incredibly careful that this does not end up in the .data section.
void pw_boot_PreStaticMemoryInit() {
  // Force RCC to be at default at boot.
  constexpr uint32_t kRccDefault = 0x078E3AD1U;
  volatile uint32_t& rcc = *reinterpret_cast<volatile uint32_t*>(0x400FE070U);
  rcc = kRccDefault;
  constexpr uint32_t kRcc2Default = 0x07802810U;
  volatile uint32_t& rcc2 = *reinterpret_cast<volatile uint32_t*>(0x400FE070U);
  rcc2 = kRcc2Default;
}

void pw_boot_PreStaticConstructorInit() {}

void pw_boot_PreMainInit() { pw_sys_io_lm3s6965evb_Init(); }

PW_NO_RETURN void pw_boot_PostMain() {
  // QEMU requires a special command to tell the VM to shut down.
  volatile uint32_t* aircr = (uint32_t*)(0xE000ED0CU);
  *aircr = 0x5fa0004;

  // In case main() returns, just sit here until the device is reset.
  while (true) {
  }
  PW_UNREACHABLE;
}
