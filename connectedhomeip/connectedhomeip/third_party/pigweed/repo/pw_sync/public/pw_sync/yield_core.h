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

// PW_SYNC_YIELD_CORE_FOR_SMT provides the architecture specific processor hint
// to allow the processor to yield in the case of SMT.
#if defined(__x86_64__) || defined(__i386__)
#include <immintrin.h>
#define PW_SYNC_YIELD_CORE_FOR_SMT() _mm_pause()

#elif defined(__aarch64__) || defined(__arm__)
#define PW_SYNC_YIELD_CORE_FOR_SMT() asm volatile("yield" ::: "memory")

#else
#error "No processor yield implementation for this architecture."

#endif  // PW_SYNC_YIELD_CORE_FOR_SMT
