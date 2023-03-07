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

#include "pw_assert/check.h"
#include "pw_log/glog_adapter_config.h"
#include "pw_log/levels.h"
#include "pw_log/log.h"
#include "pw_string/string_builder.h"

namespace pw::log::internal {

class GlogStreamingLog {
 public:
  GlogStreamingLog() = default;

  template <typename T>
  GlogStreamingLog& operator<<(const T& value) {
    string_builder_ << value;
    return *this;
  }

 protected:
  pw::StringBuffer<PW_LOG_CFG_GLOG_BUFFER_SIZE_BYTES> string_builder_;
};

}  // namespace pw::log::internal

// Declares a unique GlogStreamingLog class definition with a destructor which
// matches the desired pw_log_level.
#define _PW_LOG_GLOG_DECLARATION_PW_LOG(pw_log_level, unique)         \
  class unique : public ::pw::log::internal::GlogStreamingLog {       \
   public:                                                            \
    ~unique() {                                                       \
      PW_HANDLE_LOG(                                                  \
          pw_log_level, PW_LOG_FLAGS, "%s", string_builder_.c_str()); \
    }                                                                 \
  }

// Declares a unique GlogStreamingLog class definition with a destructor which
// invokes PW_CRASH.
#define _PW_LOG_GLOG_DECLARATION_PW_CRASH(unique)               \
  class unique : public ::pw::log::internal::GlogStreamingLog { \
   public:                                                      \
    ~unique() { PW_CRASH("%s", string_builder_.c_str()); }      \
  }

// Dispatching macros to translate the glog level to PW_LOG and PW_CRASH.
#define _PW_LOG_GLOG_DECLARATION_DEBUG(unique) \
  _PW_LOG_GLOG_DECLARATION_PW_LOG(PW_LOG_LEVEL_DEBUG, unique)

#define _PW_LOG_GLOG_DECLARATION_INFO(unique) \
  _PW_LOG_GLOG_DECLARATION_PW_LOG(PW_LOG_LEVEL_INFO, unique)

#define _PW_LOG_GLOG_DECLARATION_WARNING(unique) \
  _PW_LOG_GLOG_DECLARATION_PW_LOG(PW_LOG_LEVEL_WARN, unique)

#define _PW_LOG_GLOG_DECLARATION_ERROR(unique) \
  _PW_LOG_GLOG_DECLARATION_PW_LOG(PW_LOG_LEVEL_ERROR, unique)

#define _PW_LOG_GLOG_DECLARATION_FATAL(unique) \
  _PW_LOG_GLOG_DECLARATION_PW_CRASH(unique)

#if defined(NDEBUG)
#define _PW_LOG_GLOG_DECLARATION_DFATAL(unique) \
  _PW_LOG_GLOG_DECLARATION_PW_LOG(PW_LOG_LEVEL_ERROR, unique)
#else  // !defined(NDEBUG)
#define _PW_LOG_GLOG_DECLARATION_DFATAL(unique) \
  _PW_LOG_GLOG_DECLARATION_PW_CRASH(unique)
#endif  // defined(NDEBUG)

#define _PW_LOG_GLOG(glog_declaration, unique) \
  glog_declaration(unique);                    \
  unique()

#define _PW_LOG_GLOG_IF(glog_declaration, expr, unique) \
  glog_declaration(unique);                             \
  if (!(expr)) {                                        \
  } else                                                \
    unique()
