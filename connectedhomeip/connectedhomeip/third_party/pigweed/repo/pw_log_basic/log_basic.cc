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

// This is a very basic direct output log implementation with no buffering.

#include "pw_log_basic/log_basic.h"

#include <cstring>

#include "pw_log/levels.h"
#include "pw_log_basic_private/config.h"
#include "pw_string/string_builder.h"
#include "pw_sys_io/sys_io.h"

// ANSI color constants to control the terminal. Not Windows compatible.
// clang-format off
#define MAGENTA   "\033[35m"
#define YELLOW    "\033[33m"
#define RED       "\033[31m"
#define GREEN     "\033[32m"
#define BLUE      "\033[96m"
#define BLACK     "\033[30m"
#define YELLOW_BG "\033[43m"
#define WHITE_BG  "\033[47m"
#define RED_BG    "\033[41m"
#define BOLD      "\033[1m"
#define RESET     "\033[0m"
// clang-format on

namespace pw::log_basic {
namespace {

const char* LogLevelToLogLevelName(int level) {
  switch (level) {
    // clang-format off
#if PW_EMOJI
    case PW_LOG_LEVEL_DEBUG    : return "👾" RESET;
    case PW_LOG_LEVEL_INFO     : return "ℹ️ " RESET;
    case PW_LOG_LEVEL_WARN     : return "⚠️ " RESET;
    case PW_LOG_LEVEL_ERROR    : return "❌" RESET;
    case PW_LOG_LEVEL_CRITICAL : return "☠️ " RESET;
    default: return "❔" RESET;
#else
    case PW_LOG_LEVEL_DEBUG    : return BLUE     BOLD        "DBG" RESET;
    case PW_LOG_LEVEL_INFO     : return MAGENTA  BOLD        "INF" RESET;
    case PW_LOG_LEVEL_WARN     : return YELLOW   BOLD        "WRN" RESET;
    case PW_LOG_LEVEL_ERROR    : return RED      BOLD        "ERR" RESET;
    case PW_LOG_LEVEL_CRITICAL : return BLACK    BOLD RED_BG "FTL" RESET;
    default                    : return GREEN    BOLD        "UNK" RESET;
#endif
      // clang-format on
  }
}

#if PW_LOG_SHOW_FILENAME
const char* GetFileBasename(const char* filename) {
  int length = std::strlen(filename);
  if (length == 0) {
    return filename;
  }

  // Start on the last character.
  // TODO(pwbug/38): This part of the function doesn't work for Windows paths.
  const char* basename = filename + std::strlen(filename) - 1;
  while (basename != filename && *basename != '/') {
    basename--;
  }
  if (*basename == '/') {
    basename++;
  }
  return basename;
}
#endif  // PW_LOG_SHOW_FILENAME

void (*write_log)(std::string_view) = [](std::string_view log) {
  sys_io::WriteLine(log)
      .IgnoreError();  // TODO(b/242598609): Handle Status properly
};

}  // namespace

// This is a fully loaded, inefficient-at-the-callsite, log implementation.
extern "C" void pw_Log(int level,
                       unsigned int flags,
                       const char* module_name,
                       const char* file_name,
                       int line_number,
                       const char* function_name,
                       const char* message,
                       ...) {
  // Accumulate the log message in this buffer, then output it.
  pw::StringBuffer<PW_LOG_BASIC_ENTRY_SIZE> buffer;

  // Column: Timestamp
  // Note that this macro method defaults to a no-op.
  PW_LOG_APPEND_TIMESTAMP(buffer);

  // Column: Filename
#if PW_LOG_SHOW_FILENAME
  buffer.Format(" %-30s:%4d |", GetFileBasename(file_name), line_number);
#else
  static_cast<void>(file_name);
  static_cast<void>(line_number);
#endif

  // Column: Function
#if PW_LOG_SHOW_FUNCTION
  buffer.Format(" %20s |", function_name);
#else
  static_cast<void>(function_name);
#endif

  // Column: Module
#if PW_LOG_SHOW_MODULE
  buffer << " " BOLD;
  buffer.Format("%3s", module_name);
  buffer << RESET " ";
#else
  static_cast<void>(module_name);
#endif  // PW_LOG_SHOW_MODULE

  // Column: Flag
#if PW_LOG_SHOW_FLAG
#if PW_EMOJI
  buffer << (flags ? "🚩" : "  ");
#else
  buffer << (flags ? "*" : "|");
#endif  // PW_EMOJI
  buffer << " ";
#else
  static_cast<void>(flags);
#endif  // PW_LOG_SHOW_FLAG

  // Column: Level
  buffer << LogLevelToLogLevelName(level) << "  ";

  // Column: Message
  va_list args;
  va_start(args, message);
  buffer.FormatVaList(message, args);
  va_end(args);

  // All done; flush the log.
  write_log(buffer);
}

void SetOutput(void (*log_output)(std::string_view log)) {
  write_log = log_output;
}

}  // namespace pw::log_basic
