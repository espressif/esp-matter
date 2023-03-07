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

#include <limits>

#include "RTOS.h"

namespace pw::sync::backend {

using NativeCountingSemaphore = OS_CSEMA;
using NativeCountingSemaphoreHandle = NativeCountingSemaphore&;

inline constexpr ptrdiff_t kCountingSemaphoreMaxValue =
    std::numeric_limits<ptrdiff_t>::max() < std::numeric_limits<OS_UINT>::max()
        ? std::numeric_limits<ptrdiff_t>::max()
        : std::numeric_limits<OS_UINT>::max();

}  // namespace pw::sync::backend
