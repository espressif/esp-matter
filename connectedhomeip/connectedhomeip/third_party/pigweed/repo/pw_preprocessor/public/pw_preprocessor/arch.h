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

// TODO(b/234887943): arch.h should be refactored out of pw_preprocessor as the
// scope is outside of the module. The intended scope of arch.h is only to
// provide architecture targeting and not any added utilities and capabilities.
// Perhaps it should be placed under pw_compiler along with b/234877280, e.g.
// pw_compiler/arch.h?
// Regardless, the arch defines should likely move to a trait system in Pigweed
// before making them public defines for others to use.

// WARNING: These are all internal to Pigweed, do not use these in downstream
// projects as they expected to move and be renamed in the near future.

// _PW_ARCH_ARM_V6M should be set to 1 for Cortex M0, M0+, M1.
//
// Defaults to 0 unless __ARM_ARCH_6M__ is defined as provided by GCC, Clang,
// CMSIS's headers, etc.
#if !defined(_PW_ARCH_ARM_V6M) && defined(__ARM_ARCH_6M__)
#define _PW_ARCH_ARM_V6M 1
#else
#define _PW_ARCH_ARM_V6M 0
#endif  // _PW_ARCH_ARM_V6M

// _PW_ARCH_ARM_V7M should be set to 1 for Cortex M3.
//
// Defaults to 0 unless __ARM_ARCH_7M__ is defined as provided by GCC, Clang,
// CMSIS's headers, etc.
#if !defined(_PW_ARCH_ARM_V7M) && defined(__ARM_ARCH_7M__)
#define _PW_ARCH_ARM_V7M 1
#else
#define _PW_ARCH_ARM_V7M 0
#endif  // _PW_ARCH_ARM_V7M

// _PW_ARCH_ARM_V7EM should be set to 1 for Cortex M4, M7.
//
// Defaults to 0 unless __ARM_ARCH_7EM__ is defined as provided by GCC, Clang,
// CMSIS's headers, etc.
#if !defined(_PW_ARCH_ARM_V7EM) && defined(__ARM_ARCH_7EM__)
#define _PW_ARCH_ARM_V7EM 1
#else
#define _PW_ARCH_ARM_V7EM 0
#endif  // _PW_ARCH_ARM_V7EM

// _PW_ARCH_ARM_V8M_BASELINE should be set to 1 for Cortex M23.
//
// Defaults to 0 unless __ARM_ARCH_8M_BASE__ is defined as provided by GCC,
// Clang, CMSIS's headers, etc.
#if !defined(_PW_ARCH_ARM_V8M_BASELINE) && defined(__ARM_ARCH_8M_BASE__)
#define _PW_ARCH_ARM_V8M_BASELINE 1
#else
#define _PW_ARCH_ARM_V8M_BASELINE 0
#endif  // _PW_ARCH_ARM_V8M_BASELINE

// _PW_ARCH_ARM_V8M_MAINLINE should be set to 1 for Cortex M33, M33P.
//
// Defaults to 0 unless __ARM_ARCH_8M_MAIN__ is defined as provided by GCC,
// Clang, CMSIS's headers, etc.
#if !defined(_PW_ARCH_ARM_V8M_MAINLINE) && defined(__ARM_ARCH_8M_MAIN__)
#define _PW_ARCH_ARM_V8M_MAINLINE 1
#else
#define _PW_ARCH_ARM_V8M_MAINLINE 0
#endif  // _PW_ARCH_ARM_V8M_MAINLINE

// _PW_ARCH_ARM_V8_1M_MAINLINE should be set to 1 for Cortex M55.
//
// Defaults to 0 unless __ARM_ARCH_8_1M_MAIN__ is defined as provided by GCC,
// Clang, CMSIS's headers, etc.
#if !defined(_PW_ARCH_ARM_V8_1M_MAINLINE) && defined(__ARM_ARCH_8_1M_MAIN__)
#define _PW_ARCH_ARM_V8_1M_MAINLINE 1
#else
#define _PW_ARCH_ARM_V8_1M_MAINLINE 0
#endif  // _PW_ARCH_ARM_V8_1M_MAINLINE

#define _PW_ARCH_ARM_CORTEX_M_ACTIVE_COUNT                   \
  (_PW_ARCH_ARM_V6M + _PW_ARCH_ARM_V7M + _PW_ARCH_ARM_V7EM + \
   _PW_ARCH_ARM_V8M_BASELINE + _PW_ARCH_ARM_V8M_MAINLINE +   \
   _PW_ARCH_ARM_V8_1M_MAINLINE)
#if _PW_ARCH_ARM_CORTEX_M_ACTIVE_COUNT > 1
#error "More than one ARM Cortex M architecture is active."
#elif _PW_ARCH_ARM_CORTEX_M_ACTIVE_COUNT == 1
#define _PW_ARCH_ARM_CORTEX_M 1
#endif
