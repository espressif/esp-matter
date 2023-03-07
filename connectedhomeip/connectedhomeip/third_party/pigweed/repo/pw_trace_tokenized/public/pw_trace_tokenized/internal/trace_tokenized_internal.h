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
//==============================================================================
//

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "pw_preprocessor/arguments.h"

// Because __FUNCTION__ is not a string literal to the preprocessor it can't be
// tokenized. So this backend redefines the implementation to instead use the
// file name and line number.
// TODO(rgoliver): A build step could be added which checks the token dictionary
// for these and uses add2line to replace the database with the function names.
#define PW_TRACE_FUNCTION_LABEL_FILE_LINE(file, line) \
  "[" PW_STRINGIFY(file) ":" PW_STRINGIFY(line) "]"
#define PW_TRACE_FUNCTION_LABEL \
  PW_TRACE_FUNCTION_LABEL_FILE_LINE(__FILE__, __LINE__)

// Enable these trace types
#define PW_TRACE_TYPE_INSTANT PW_TRACE_EVENT_TYPE_INSTANT
#define PW_TRACE_TYPE_INSTANT_GROUP PW_TRACE_EVENT_TYPE_INSTANT_GROUP

#define PW_TRACE_TYPE_DURATION_START PW_TRACE_EVENT_TYPE_DURATION_START
#define PW_TRACE_TYPE_DURATION_END PW_TRACE_EVENT_TYPE_DURATION_END

#define PW_TRACE_TYPE_DURATION_GROUP_START \
  PW_TRACE_EVENT_TYPE_DURATION_GROUP_START
#define PW_TRACE_TYPE_DURATION_GROUP_END PW_TRACE_EVENT_TYPE_DURATION_GROUP_END

#define PW_TRACE_TYPE_ASYNC_START PW_TRACE_EVENT_TYPE_ASYNC_START
#define PW_TRACE_TYPE_ASYNC_INSTANT PW_TRACE_EVENT_TYPE_ASYNC_STEP
#define PW_TRACE_TYPE_ASYNC_END PW_TRACE_EVENT_TYPE_ASYNC_END

PW_EXTERN_C_START
typedef enum {
  PW_TRACE_EVENT_TYPE_INVALID = 0,
  PW_TRACE_EVENT_TYPE_INSTANT = 1,
  PW_TRACE_EVENT_TYPE_INSTANT_GROUP = 2,
  PW_TRACE_EVENT_TYPE_ASYNC_START = 3,
  PW_TRACE_EVENT_TYPE_ASYNC_STEP = 4,
  PW_TRACE_EVENT_TYPE_ASYNC_END = 5,
  PW_TRACE_EVENT_TYPE_DURATION_START = 6,
  PW_TRACE_EVENT_TYPE_DURATION_END = 7,
  PW_TRACE_EVENT_TYPE_DURATION_GROUP_START = 8,
  PW_TRACE_EVENT_TYPE_DURATION_GROUP_END = 9,
} pw_trace_EventType;

// This should not be called directly, instead use the PW_TRACE_* macros.
void pw_trace_TraceEvent(uint32_t trace_token,
                         pw_trace_EventType event_type,
                         const char* module,
                         uint32_t trace_id,
                         uint8_t flags,
                         const void* data_buffer,
                         size_t data_size);

// This should not be called directly, insted: PW_TRACE_SET_ENABLED
void pw_trace_Enable(bool enabled);

// Returns true if tracing is currently enabled.
bool pw_trace_IsEnabled(void);

PW_EXTERN_C_END

// These are what the facade actually calls.
#define PW_TRACE(event_type, flags, label, group, trace_id)                  \
  do {                                                                       \
    static const uint32_t kLabelToken =                                      \
        PW_TRACE_REF(event_type, PW_TRACE_MODULE_NAME, label, flags, group); \
    pw_trace_TraceEvent(kLabelToken,                                         \
                        event_type,                                          \
                        PW_TRACE_MODULE_NAME,                                \
                        trace_id,                                            \
                        flags,                                               \
                        NULL,                                                \
                        0);                                                  \
  } while (0)

#define PW_TRACE_DATA(                                                \
    event_type, flags, label, group, trace_id, type, data, size)      \
  do {                                                                \
    static const uint32_t kLabelToken = PW_TRACE_REF_DATA(            \
        event_type, PW_TRACE_MODULE_NAME, label, flags, group, type); \
    pw_trace_TraceEvent(kLabelToken,                                  \
                        event_type,                                   \
                        PW_TRACE_MODULE_NAME,                         \
                        trace_id,                                     \
                        flags,                                        \
                        data,                                         \
                        size);                                        \
  } while (0)
