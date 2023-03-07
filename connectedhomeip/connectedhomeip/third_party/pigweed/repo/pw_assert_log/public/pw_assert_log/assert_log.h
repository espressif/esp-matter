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

#include "pw_log/levels.h"
#include "pw_log/log.h"
#include "pw_log/options.h"
#include "pw_preprocessor/compiler.h"

// Forward directly to the backend PW_HANDLE_LOG() macro rather than PW_LOG().
// PW_LOG() checks the user-overridable PW_LOG_LEVEL macro, which may not work
// in headers without additional handling.
#define PW_ASSERT_HANDLE_FAILURE(condition_string)                             \
  do {                                                                         \
    PW_HANDLE_LOG(                                                             \
        PW_LOG_LEVEL_FATAL, PW_LOG_FLAGS, "Assert failed: " condition_string); \
    PW_UNREACHABLE;                                                            \
  } while (0)
