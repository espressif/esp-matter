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

// A pw_boot backend is similar to a traditional assembly startup file paired
// with a linker script.

#include <stdint.h>

#include "pw_preprocessor/compiler.h"
#include "pw_preprocessor/util.h"

PW_EXTERN_C_START

// Forward declaration of main. Pigweed applications are expected to implement
// this function. An implementation of main() should NOT be provided by a
// backend.
int main(void);

// Reset handler or boot entry point.
//
// Backends must provide this method, and this method must call the user
// supplied functions below in the appropriate order, along with any other
// early initialization required by the target.
//
// A minimal implementation would be:
//
//   void pw_boot_Entry() {  // Boot entry point provided by backend.
//     pw_boot_PreStaticMemoryInit();  // User-implemented function.
//     // Static memory initialization.
//     pw_boot_PreStaticConstructorInit();  // User-implemented function.
//     // C++ static constructors are invoked.
//     pw_boot_PreMainInit();  // User-implemented function.
//     main();  // User-implemented function.
//     pw_boot_PostMain();  // User-implemented function.
//     PW_UNREACHABLE;
//   }
PW_NO_RETURN void pw_boot_Entry(void);

// pw_boot hook: Before static memory is initialized (user supplied)
//
// This is a hook function that users of pw_boot must supply. It is called
// immediately upon entry to pw_boot_Entry() and before zero initialization of
// RAM (.bss) and loading values into static memory (commonly labeled as the
// .data section in an ELF file).
// WARNING: Be EXTREMELY careful when in the context of this function as it
// violates the C spec in several ways as .bss has not yet been zero-initialized
// and static values have not yet been loaded into memory. This function should
// NOT be implemented by a pw_boot backend.
//
// Interrupts are disabled until after this function returns.
void pw_boot_PreStaticMemoryInit(void);

// pw_boot hook: Before C++ static constructors are invoked (user supplied).
//
// This is a hook function that users of pw_boot must supply. It is called just
// after zero initialization of RAM and loading values into static memory
// (commonly labeled as the .data section in an ELF file). Per the naming, this
// function is called just before C++ static constructors are invoked. It is
// safe to run C code, but NOT safe to call out to any C++ code. This function
// should NOT be implemented by a pw_boot backend.
void pw_boot_PreStaticConstructorInit(void);

// pw_boot hook: Before main is invoked (user supplied).
//
// This is a hook function that users of pw_boot must supply. It is called by
// pw_boot_Entry() after memory initialization but before main. This allows
// targets to have pre-main initialization of the device and seamlessly swap out
// the main() implementation. This function should NOT be implemented by
// a pw_boot backend.
void pw_boot_PreMainInit(void);

// pw_boot hook: After main returned (user supplied).
//
// This is a hook function that users of pw_boot must supply. It is called by
// pw_boot_Entry() after main() has returned. This function must not return!
// This function should NOT be implemented by a pw_boot backend.
PW_NO_RETURN void pw_boot_PostMain(void);

PW_EXTERN_C_END
