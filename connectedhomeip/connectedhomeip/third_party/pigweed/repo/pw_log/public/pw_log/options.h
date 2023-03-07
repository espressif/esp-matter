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

// This file defines macros used to control the behavior of pw_log statements.
// Files that use pw_log may define these macros BEFORE any headers are
// #included to customize pw_log.
//
// For example, the following sets the log module name to "Foobar" and the
// minimum log level to WARN:
//
//   #define PW_LOG_MODULE_NAME "Foobar"
//   #define PW_LOG_LEVEL PW_LOG_LEVEL_WARN
//
//   #include "foo/bar.h"
//   #include "pw_log/log.h"
//
// Users of pw_log should not include this header directly; include
// "pw_log/log.h" instead. This header is separate from "pw_log/log.h" to avoid
// circular dependencies when implementing the pw_log facade.
#pragma once

#include "pw_log/config.h"

// These configuration options differ from the options in pw_log/config.h in
// that these should be set at a module/compile unit level rather than a global
// level level.

// Default: Module name
//
// An empty string is used for the module name if it is not set. The
// PW_LOG_MODULE_NAME_DEFINED macro is set to 1 or 0 to allow pw_log backends to
// behave differently if the module name is defined. For example, a backend
// might prefix the format string with PW_LOG_MODULE_NAME ": ", but only if the
// module name is provided.
#ifdef PW_LOG_MODULE_NAME
#define PW_LOG_MODULE_NAME_DEFINED 1
#else
#define PW_LOG_MODULE_NAME ""
#define PW_LOG_MODULE_NAME_DEFINED 0
#endif  // PW_LOG_MODULE_NAME

// Default: Log level filtering
//
// All log statements have a level, and this define sets the log level to the
// globally set default if PW_LOG_LEVEL was not already set by the module.
// This is compile-time filtering if the level is a constant.
#ifndef PW_LOG_LEVEL
#define PW_LOG_LEVEL PW_LOG_LEVEL_DEFAULT
#endif  // PW_LOG_LEVEL

// Default: Flags
//
// For log statements like LOG_INFO that don't have an explicit argument, this
// is used for the flags value.
#ifndef PW_LOG_FLAGS
#define PW_LOG_FLAGS PW_LOG_FLAGS_DEFAULT
#endif  // PW_LOG_FLAGS

// DEPRECATED: Use PW_LOG_FLAGS.
// TODO(b/234876701): Remove this macro after migration.
#ifndef PW_LOG_DEFAULT_FLAGS
#define PW_LOG_DEFAULT_FLAGS PW_LOG_FLAGS
#endif  // PW_LOG_DEFAULT_FLAGS

// Default: Log enabled expression
//
// This expression determines whether or not the statement is enabled and
// should be passed to the backend.
#ifndef PW_LOG_ENABLE_IF
#define PW_LOG_ENABLE_IF(level, flags) PW_LOG_ENABLE_IF_DEFAULT(level, flags)
#endif  // PW_LOG_ENABLE_IF
