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

#include <chrono>

#include "pw_chrono/system_clock.h"

namespace pw::chrono::backend {

inline int64_t GetSystemClockTickCount() {
  // Note that no conversion is necessary since the steady_clock's period and
  // epoch are directly used.
  return std::chrono::steady_clock::now().time_since_epoch().count();
}

}  // namespace pw::chrono::backend
