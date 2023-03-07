// Copyright 2022 The Pigweed Authors
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

#include <log/log.h>

// This backend supports PW_LOG_MODULE_NAME as a fallback for Android logging's
// LOG_TAG if and only if LOG_TAG is not already set.
#if !defined(LOG_TAG) && defined(PW_LOG_MODULE_NAME)
#define LOG_TAG PW_LOG_MODULE_NAME
#endif

// #define PW_LOG_LEVEL_DEBUG    1
#define _PW_LOG_ANDROID_LEVEL_1(...) ALOGD(__VA_ARGS__)

// #define PW_LOG_LEVEL_INFO    2
#define _PW_LOG_ANDROID_LEVEL_2(...) ALOGI(__VA_ARGS__)

// #define PW_LOG_LEVEL_WARN    3
#define _PW_LOG_ANDROID_LEVEL_3(...) ALOGW(__VA_ARGS__)

// #define PW_LOG_LEVEL_ERROR    4
#define _PW_LOG_ANDROID_LEVEL_4(...) ALOGE(__VA_ARGS__)

// #define PW_LOG_LEVEL_CRITICAL    5
#define _PW_LOG_ANDROID_LEVEL_5(...) ALOGE(__VA_ARGS__)

// #define PW_LOG_LEVEL_FATAL    7
#define _PW_LOG_ANDROID_LEVEL_7(...) LOG_ALWAYS_FATAL(__VA_ARGS__)

#define _PW_HANDLE_LOG(level, flags, ...) \
  _PW_LOG_ANDROID_LEVEL_##level(__VA_ARGS__)

// The indirection through _PW_HANDLE_LOG ensures the `level` argument is
// expanded.
#define PW_HANDLE_LOG(...) _PW_HANDLE_LOG(__VA_ARGS__)
