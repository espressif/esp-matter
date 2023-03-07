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

#include "pw_sync/thread_notification.h"

namespace pw::sync {

inline ThreadNotification::ThreadNotification() : native_type_() {}

inline ThreadNotification::~ThreadNotification() {}

inline void ThreadNotification::acquire() { native_type_.acquire(); }

inline bool ThreadNotification::try_acquire() {
  return native_type_.try_acquire();
}

inline void ThreadNotification::release() { native_type_.release(); }

inline ThreadNotification::native_handle_type
ThreadNotification::native_handle() {
  return native_type_;
}

}  // namespace pw::sync
