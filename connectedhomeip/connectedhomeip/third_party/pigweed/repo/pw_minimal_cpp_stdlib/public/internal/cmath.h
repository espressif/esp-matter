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

#include <math.h>

#include "pw_polyfill/standard_library/namespace.h"

_PW_POLYFILL_BEGIN_NAMESPACE_STD

// The integer overloads of these functions are not provided.

// This is not technically correct, but avoids ambiguous calls to an overloaded
// abs function.
template <typename T>
inline T abs(T value) {
  return value < 0 ? -value : value;
}

#ifdef isfinite
#undef isfinite
#endif  // isfinite

inline bool isfinite(float value) { return __builtin_isfinite(value); }
inline bool isfinite(double value) { return __builtin_isfinite(value); }
inline bool isfinite(long double value) { return __builtin_isfinite(value); }

#ifdef isnan
#undef isnan
#endif  // isnan

inline bool isnan(float value) { return __builtin_isnan(value); }
inline bool isnan(double value) { return __builtin_isnan(value); }
inline bool isnan(long double value) { return __builtin_isnan(value); }

#ifdef signbit
#undef signbit
#endif  // signbit

inline bool signbit(float value) { return __builtin_signbit(value); }
inline bool signbit(double value) { return __builtin_signbit(value); }
inline bool signbit(long double value) { return __builtin_signbit(value); }

using ::round;

_PW_POLYFILL_END_NAMESPACE_STD
