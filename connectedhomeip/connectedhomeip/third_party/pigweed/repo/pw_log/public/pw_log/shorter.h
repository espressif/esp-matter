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

#include "pw_log/log.h"

// These are optional very short versions of the PW_LOG macros. They are not
// prefixed with PW_*, so may collide with other macros.

// LOG is also defined by pw_log/short.h, so check if it's defined first.
#ifndef LOG
#define LOG PW_LOG
#endif  // LOG

// clang-format off
#define LOG PW_LOG
#define DBG PW_LOG_DEBUG
#define INF PW_LOG_INFO
#define WRN PW_LOG_WARN
#define ERR PW_LOG_ERROR
#define CRT PW_LOG_CRITICAL
// clang-format on
