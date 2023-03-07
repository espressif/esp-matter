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

#include "tx_api.h"

// Whether thread joining is enabled. By default this is disabled.
//
// We suggest only enabling this when thread joining is required to minimize
// the RAM and ROM cost of threads.
//
// Enabling this grows the RAM footprint of every pw::thread::Thread as it adds
// a TX_EVENT_FLAGS_GROUP to every thread's pw::thread::threadx::Context. In
// addition, there is a minute ROM cost to construct and destroy this added
// object.
//
// PW_THREAD_JOINING_ENABLED gets set to this value.
#ifndef PW_THREAD_THREADX_CONFIG_JOINING_ENABLED
#define PW_THREAD_THREADX_CONFIG_JOINING_ENABLED 0
#endif  // PW_THREAD_THREADX_CONFIG_JOINING_ENABLED
#define PW_THREAD_JOINING_ENABLED PW_THREAD_THREADX_CONFIG_JOINING_ENABLED

// The default stack size in words. By default this uses the minimal ThreadX
// stack size.
#ifndef PW_THREAD_THREADX_CONFIG_DEFAULT_STACK_SIZE_WORDS
#define PW_THREAD_THREADX_CONFIG_DEFAULT_STACK_SIZE_WORDS \
  (TX_MINIMUM_STACK / sizeof(ULONG))
#endif  // PW_THREAD_THREADX_CONFIG_DEFAULT_STACK_SIZE_WORDS

// The maximum length of a thread's name, not including null termination. By
// default this is arbitrarily set to 15. This results in an array of characters
// which is this length + 1 bytes in every pw::thread::Thread's context.
#ifndef PW_THREAD_THREADX_CONFIG_MAX_THREAD_NAME_LEN
#define PW_THREAD_THREADX_CONFIG_MAX_THREAD_NAME_LEN 15
#endif  // PW_THREAD_THREADX_CONFIG_MAX_THREAD_NAME_LEN

// The round robin time slice tick interval for threads at the same priority.
// By default this is disabled as not all ports support this, using a value of 0
// ticks.
#ifndef PW_THREAD_THREADX_CONFIG_DEFAULT_TIME_SLICE_INTERVAL
#define PW_THREAD_THREADX_CONFIG_DEFAULT_TIME_SLICE_INTERVAL TX_NO_TIME_SLICE
#endif  // PW_THREAD_THREADX_CONFIG_DEFAULT_TIME_SLICE_INTERVAL

// The minimum priority level, this is normally based on the number of priority
// levels.
#ifndef PW_THREAD_THREADX_CONFIG_MIN_PRIORITY
#define PW_THREAD_THREADX_CONFIG_MIN_PRIORITY (TX_MAX_PRIORITIES - 1)
#endif  // PW_THREAD_THREADX_CONFIG_MIN_PRIORITY

// The default priority level. By default this uses the minimal ThreadX
// priority level, given that 0 is the highest priority.
#ifndef PW_THREAD_THREADX_CONFIG_DEFAULT_PRIORITY
#define PW_THREAD_THREADX_CONFIG_DEFAULT_PRIORITY \
  PW_THREAD_THREADX_CONFIG_MIN_PRIORITY
#endif  // PW_THREAD_THREADX_CONFIG_DEFAULT_PRIORITY

// The log level to use for this module. Logs below this level are omitted.
#ifndef PW_THREAD_THREADX_CONFIG_LOG_LEVEL
#define PW_THREAD_THREADX_CONFIG_LOG_LEVEL PW_LOG_LEVEL_DEBUG
#endif  // PW_THREAD_THREADX_CONFIG_LOG_LEVEL

namespace pw::thread::threadx::config {

inline constexpr size_t kMaximumNameLength =
    PW_THREAD_THREADX_CONFIG_MAX_THREAD_NAME_LEN;
inline constexpr size_t kMinimumStackSizeWords =
    TX_MINIMUM_STACK / sizeof(ULONG);
inline constexpr size_t kDefaultStackSizeWords =
    PW_THREAD_THREADX_CONFIG_DEFAULT_STACK_SIZE_WORDS;
inline constexpr UINT kMinimumPriority = PW_THREAD_THREADX_CONFIG_MIN_PRIORITY;
inline constexpr UINT kDefaultPriority =
    PW_THREAD_THREADX_CONFIG_DEFAULT_PRIORITY;
inline constexpr ULONG kDefaultTimeSliceInterval =
    PW_THREAD_THREADX_CONFIG_DEFAULT_TIME_SLICE_INTERVAL;

}  // namespace pw::thread::threadx::config
