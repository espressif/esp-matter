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

// Configuration macros for the function module.
#pragma once

#include <cstddef>

// The maximum size of a callable that can be inlined within a function. This is
// also the size of the Function object itself. Callables larger than this are
// stored externally to the function.
//
// This defaults to 1 pointer, which is capable of storing common callables
// such as function pointers and lambdas with a single capture.
#ifndef PW_FUNCTION_INLINE_CALLABLE_SIZE
#define PW_FUNCTION_INLINE_CALLABLE_SIZE (sizeof(void*))
#endif  // PW_FUNCTION_INLINE_CALLABLE_SIZE

static_assert(PW_FUNCTION_INLINE_CALLABLE_SIZE > 0 &&
              PW_FUNCTION_INLINE_CALLABLE_SIZE % alignof(void*) == 0);

// Whether functions should allocate memory dynamically (using operator new) if
// a callable is larger than the inline size.
#ifndef PW_FUNCTION_ENABLE_DYNAMIC_ALLOCATION
#define PW_FUNCTION_ENABLE_DYNAMIC_ALLOCATION 0
#endif  // PW_FUNCTION_ENABLE_DYNAMIC_ALLOCATION

namespace pw::function_internal::config {

inline constexpr size_t kInlineCallableSize = PW_FUNCTION_INLINE_CALLABLE_SIZE;
inline constexpr bool kEnableDynamicAllocation =
    PW_FUNCTION_ENABLE_DYNAMIC_ALLOCATION;

}  // namespace pw::function_internal::config
