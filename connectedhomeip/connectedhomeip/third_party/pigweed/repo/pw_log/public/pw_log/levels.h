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
#pragma once

// Standard log levels. Values are limited to 3 bits, to fit within the protobuf
// definition of LogEntry's line_level in pw_log_rpc. These levels correspond
// with the log levels from Python's logging library, but have different values.
//
// clang-format off
#define PW_LOG_LEVEL_DEBUG    1
#define PW_LOG_LEVEL_INFO     2
#define PW_LOG_LEVEL_WARN     3
#define PW_LOG_LEVEL_ERROR    4
#define PW_LOG_LEVEL_CRITICAL 5

#define PW_LOG_LEVEL_FATAL    7

// Number of bits required to represent the log level
#define PW_LOG_LEVEL_BITS     3
#define PW_LOG_LEVEL_BITMASK  7  // 0b111
// clang-format on
