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
// Configuration macros for the tokenizer module.
#pragma once

#include "FreeRTOS.h"

// Just like FreeRTOS Stream and Message Buffers, by default the optimized
// pw::sync::ThreadNotification uses the task notification at array index 0.
// This optimized backend is compatible with sharing the notification with
// Stream and Message Buffers and any other users which ONLY notify the task
// while the task is blocked and the task consumes the notification state
// before returning.
//
// The task's TCB uses a ucNotifyState which captures notification state even
// when the task is not waiting (taskNOT_WAITING_NOTIFICATION vs
// task_NOTIFICATION_RECEIVED). This notification state is used to determine
// whether the next task notification wait call should block, irrespective of
// the notification value. This means that one must ensure not just that
// the bits in the ulNotifiedValue are mutually exclusive, but also that the
// notification state is mutually exclusive!
#ifndef PW_SYNC_FREERTOS_CONFIG_THREAD_NOTIFICATION_INDEX
#define PW_SYNC_FREERTOS_CONFIG_THREAD_NOTIFICATION_INDEX 0
#endif  // PW_SYNC_FREERTOS_CONFIG_THREAD_NOTIFICATION_INDEX

namespace pw::sync::freertos::config {

inline constexpr UBaseType_t kThreadNotificationIndex =
    PW_SYNC_FREERTOS_CONFIG_THREAD_NOTIFICATION_INDEX;
#ifdef configTASK_NOTIFICATION_ARRAY_ENTRIES
// Ensure the index fits within the FreeRTOS configuration of the task
// notification array.
static_assert(kThreadNotificationIndex < configTASK_NOTIFICATION_ARRAY_ENTRIES);
#else   // !defined(configTASK_NOTIFICATION_ARRAY_ENTRIES)
// This version of FreeRTOS does not support multiple task notifications, ensure
// the index is the default of 0.
static_assert(kThreadNotificationIndex == 0);
#endif  // configTASK_NOTIFICATION_ARRAY_ENTRIES

}  // namespace pw::sync::freertos::config
