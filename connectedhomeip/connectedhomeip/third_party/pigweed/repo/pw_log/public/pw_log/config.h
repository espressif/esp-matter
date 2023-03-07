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

// These configuration options differ from the options in pw_log/options.h in
// that these should be set at a global level in the build system rather than
// at a module or compile unit level.

// PW_LOG_LEVEL_DEFAULT controls the default value of PW_LOG_LEVEL.
//
// Defaults to PW_LOG_LEVEL_DEBUG.
#ifndef PW_LOG_LEVEL_DEFAULT
#define PW_LOG_LEVEL_DEFAULT PW_LOG_LEVEL_DEBUG
#endif  // PW_LOG_LEVEL_DEFAULT

// PW_LOG_FLAGS_DEFAULT controls the default value of PW_LOG_FLAGS.
//
// For log statements like LOG_INFO that don't have an explicit argument, this
// is used for the flags value.
#ifndef PW_LOG_FLAGS_DEFAULT
#define PW_LOG_FLAGS_DEFAULT 0
#endif  // PW_LOG_FLAGS_DEFAULT

// PW_LOG_ENABLE_IF_DEFAULT controls the default value of PW_LOG_ENABLE_IF.
//
// This expression determines whether or not the statement is enabled and
// should be passed to the backend.
#ifndef PW_LOG_ENABLE_IF_DEFAULT
#define PW_LOG_ENABLE_IF_DEFAULT(level, flags) ((level) >= PW_LOG_LEVEL)
#endif  // PW_LOG_ENABLE_IF_DEFAULT
