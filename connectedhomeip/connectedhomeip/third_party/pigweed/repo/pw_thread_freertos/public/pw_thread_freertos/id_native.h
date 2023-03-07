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

#include "FreeRTOS.h"
#include "task.h"

namespace pw::thread::backend {

// Instead of using a pw::thread::freertos specific identifier, the FreeRTOS
// task handle is used as this means pw::this_thread::id works correctly on
// threads started with the native FreeRTOS APIs as well as those started
// using the pw::thread APIs.
class NativeId {
 public:
  constexpr NativeId(TaskHandle_t task_handle = nullptr)
      : task_handle_(task_handle) {}

  constexpr bool operator==(NativeId other) const {
    return task_handle_ == other.task_handle_;
  }
  constexpr bool operator!=(NativeId other) const {
    return task_handle_ != other.task_handle_;
  }
  constexpr bool operator<(NativeId other) const {
    return task_handle_ < other.task_handle_;
  }
  constexpr bool operator<=(NativeId other) const {
    return task_handle_ <= other.task_handle_;
  }
  constexpr bool operator>(NativeId other) const {
    return task_handle_ > other.task_handle_;
  }
  constexpr bool operator>=(NativeId other) const {
    return task_handle_ >= other.task_handle_;
  }

 private:
  TaskHandle_t task_handle_;
};

}  // namespace pw::thread::backend
