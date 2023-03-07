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

#include "FreeRTOS.h"
#include "pw_polyfill/language_feature_macros.h"
#include "pw_sync/interrupt_spin_lock.h"
#include "pw_sync/lock_annotations.h"
#include "task.h"

namespace pw::sync::backend {

struct NativeThreadNotification {
  TaskHandle_t blocked_thread PW_GUARDED_BY(shared_spin_lock);
  bool notified PW_GUARDED_BY(shared_spin_lock);
  // We use a global ISL for all thread notifications because these backends
  // only support uniprocessor targets and ergo we reduce the memory cost for
  // all ISL instances without any risk of spin contention between different
  // instances.
  PW_CONSTINIT inline static InterruptSpinLock shared_spin_lock = {};
};
using NativeThreadNotificationHandle = NativeThreadNotification&;

}  // namespace pw::sync::backend
