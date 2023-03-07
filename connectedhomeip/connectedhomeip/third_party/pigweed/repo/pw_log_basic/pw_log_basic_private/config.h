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

// Replaces log levels and flag presence indicator with emoji.
#ifndef PW_EMOJI
#define PW_EMOJI 0
#endif  // PW_EMOJI

// With all the following flags enabled except for the optional user provided
// PW_LOG_APPEND_TIMESTAMP, log messages look like this:
//
// clang-format off
//  my_file.cc                    :  42 |                Foo | TST | INF  Hello, world!
//  buggy.cc                      :2145 |    ReadBuggyBuffer |     * ERR  No, BAD!
//
// With emoji:
//  my_file.cc                    :  42 |                Foo | TST    ℹ️  Hello, world!
//  buggy.cc                      :2145 |    ReadBuggyBuffer |     🚩 ❌  No, BAD!
// clang-format on

// Prints the name of the file that emitted the log message.
#ifndef PW_LOG_SHOW_FILENAME
#define PW_LOG_SHOW_FILENAME 0
#endif  // PW_LOG_SHOW_FILENAME

// Prints the name of the function that emitted the log message.
#ifndef PW_LOG_SHOW_FUNCTION
#define PW_LOG_SHOW_FUNCTION 0
#endif  // PW_LOG_SHOW_FUNCTION

// Prints an indicator for whether or not there are any active flags for a given
// log statement.
#ifndef PW_LOG_SHOW_FLAG
#define PW_LOG_SHOW_FLAG 0
#endif  // PW_LOG_SHOW_FLAG

// Prints the module name associated with a log statement. This is provided by
// defining PW_LOG_MODULE_NAME inside module source files, it is not implied by
// module structure or file path magic.
#ifndef PW_LOG_SHOW_MODULE
#define PW_LOG_SHOW_MODULE 0
#endif  // PW_LOG_SHOW_MODULE

// Optional user provided macro to append a prefixing timestamp string.
// For example this could be implemented as:
// #define PW_LOG_APPEND_TIMESTAMP(buffer) AppendSecSinceEpoch(buffer)
//
// void AppendSecSinceEpoch(pw::StringBuilder& builder) {
//    const std::chrono::duration<float> float_s_since_epoch =
//        pw::chrono::SystemClock::now().time_since_epoch();
//    builder << float_s_since_epoch.count() << " ";
// }
#ifndef PW_LOG_APPEND_TIMESTAMP
#define PW_LOG_APPEND_TIMESTAMP(buffer) \
  do {                                  \
  } while (0)
#endif  // PW_LOG_APPEND_TIMESTAMP

// Maximum size of the encoded log message. Log messages that would result in a
// larger entry are truncated to this size. The message buffer is allocated on
// the stack on every log call.
#ifndef PW_LOG_BASIC_ENTRY_SIZE
#define PW_LOG_BASIC_ENTRY_SIZE 150
#endif  // PW_LOG_BASIC_ENTRY_SIZE
