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

// These are optional shortened versions of the PW_LOG macros. They are not
// prefixed with PW_*, so may collide with other macros.

// LOG is also defined by pw_log/shorter.h, so check if it's defined first.
#ifndef LOG
#define LOG PW_LOG
#endif  // LOG

// clang-format off
#define LOG_DEBUG    PW_LOG_DEBUG
#define LOG_INFO     PW_LOG_INFO
#define LOG_WARN     PW_LOG_WARN
#define LOG_ERROR    PW_LOG_ERROR
#define LOG_CRITICAL PW_LOG_CRITICAL
// clang-format on
