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
// This file contains the backend hooks and implementation details for trace.

#pragma once

#include "pw_preprocessor/arguments.h"
#include "pw_preprocessor/concat.h"
#include "pw_trace_backend/trace_backend.h"

// Default: Flag value if none set
#ifndef PW_TRACE_FLAGS_DEFAULT
#define PW_TRACE_FLAGS_DEFAULT 0
#endif  // PW_TRACE_FLAGS_DEFAULT

// Default: PW_TRACE_TRACE_ID_DEFAULT
#ifndef PW_TRACE_TRACE_ID_DEFAULT
#define PW_TRACE_TRACE_ID_DEFAULT 0
#endif  // PW_TRACE_TRACE_ID_DEFAULT

// Default: PW_TRACE_GROUP_LABEL_DEFAULT
#ifndef PW_TRACE_GROUP_LABEL_DEFAULT
#define PW_TRACE_GROUP_LABEL_DEFAULT ""
#endif  // PW_TRACE_GROUP_LABEL_DEFAULT

// These macros can be used to determine if a trace type contrains span or group
// label
#ifndef PW_TRACE_HAS_TRACE_ID
#define PW_TRACE_HAS_TRACE_ID(TRACE_TYPE)         \
  ((TRACE_TYPE) == PW_TRACE_TYPE_ASYNC_START ||   \
   (TRACE_TYPE) == PW_TRACE_TYPE_ASYNC_INSTANT || \
   (TRACE_TYPE) == PW_TRACE_TYPE_ASYNC_END)
#endif  // PW_TRACE_HAS_TRACE_ID
#ifndef PW_TRACE_HAS_GROUP_LABEL
#define PW_TRACE_HAS_GROUP_LABEL(TRACE_TYPE) (false)
#endif  // PW_TRACE_HAS_GROUP_LABEL

// Default: behaviour for unimplemented trace event types
#ifndef _PW_TRACE_DISABLED
static inline void _pw_trace_disabled(int x, ...) { (void)x; }
// `_PW_TRACE_DISABLED` must be called with at least one arg.
#define _PW_TRACE_DISABLED(...)           \
  do {                                    \
    _pw_trace_disabled(0, ##__VA_ARGS__); \
  } while (false)
#endif  // _PW_TRACE_DISABLED

// Default: label used for PW_TRACE_FUNCTION trace events
#ifndef PW_TRACE_FUNCTION_LABEL
#define PW_TRACE_FUNCTION_LABEL __PRETTY_FUNCTION__
#endif

// Control to enable/disable tracing.  If 0, no traces are emitted.
//
// Defaults to enabled.
#ifndef PW_TRACE_ENABLE
#define PW_TRACE_ENABLE 1
#endif  // PW_TRACE_ENABLE

#define _PW_TRACE_IF_ENABLED(event_type, flags, label, group_label, trace_id) \
  do {                                                                        \
    if ((PW_TRACE_ENABLE) != 0) {                                             \
      PW_TRACE(event_type, flags, label, group_label, trace_id);              \
    }                                                                         \
  } while (0)

#define _PW_TRACE_DATA_IF_ENABLED(event_type,         \
                                  flags,              \
                                  label,              \
                                  group_label,        \
                                  trace_id,           \
                                  data_format_string, \
                                  data,               \
                                  size)               \
  do {                                                \
    if ((PW_TRACE_ENABLE) != 0) {                     \
      PW_TRACE_DATA(event_type,                       \
                    flags,                            \
                    label,                            \
                    group_label,                      \
                    trace_id,                         \
                    data_format_string,               \
                    data,                             \
                    size);                            \
    }                                                 \
  } while (0)

// This block handles:
//      - PW_TRACE_INSTANT(label)
//      - PW_TRACE_INSTANT_FLAG(flag, label)
// Which creates a trace event with the type: PW_TRACE_TYPE_INSTANT
// NOTE: If this type is not defined by the backend this trace is removed.
#ifdef PW_TRACE_TYPE_INSTANT
#define _PW_TRACE_INSTANT_ARGS2(flag, label)         \
  _PW_TRACE_IF_ENABLED(PW_TRACE_TYPE_INSTANT,        \
                       flag,                         \
                       label,                        \
                       PW_TRACE_GROUP_LABEL_DEFAULT, \
                       PW_TRACE_TRACE_ID_DEFAULT)
#else  // PW_TRACE_TYPE_INSTANT
#define _PW_TRACE_INSTANT_ARGS2(...) _PW_TRACE_DISABLED(__VA_ARGS__)
#endif  // PW_TRACE_TYPE_INSTANT

// This block handles:
//      - PW_TRACE_INSTANT(label, group)
//      - PW_TRACE_INSTANT_FLAG(flag, label, group)
// Which creates a trace event with the type: PW_TRACE_TYPE_INSTANT_GROUP
// NOTE: If this type is not defined by the backend this trace is removed.
#ifdef PW_TRACE_TYPE_INSTANT_GROUP
#define _PW_TRACE_INSTANT_ARGS3(flag, label, group) \
  _PW_TRACE_IF_ENABLED(PW_TRACE_TYPE_INSTANT_GROUP, \
                       flag,                        \
                       label,                       \
                       group,                       \
                       PW_TRACE_TRACE_ID_DEFAULT)
#else  // PW_TRACE_TYPE_INSTANT_GROUP
#define _PW_TRACE_INSTANT_ARGS3(...) _PW_TRACE_DISABLED(__VA_ARGS__)
#endif  // PW_TRACE_TYPE_INSTANT_GROUP

// This block handles:
//      - PW_TRACE_INSTANT(label, group, trace_id)
//      - PW_TRACE_INSTANT_FLAG(flag, label, group, trace_id)
// Which creates a trace event with the type: PW_TRACE_TYPE_ASYNC_INSTANT
// NOTE: If this type is not defined by the backend this trace is removed.
#ifdef PW_TRACE_TYPE_ASYNC_INSTANT
#define _PW_TRACE_INSTANT_ARGS4(flag, label, group, trace_id) \
  _PW_TRACE_IF_ENABLED(                                       \
      PW_TRACE_TYPE_ASYNC_INSTANT, flag, label, group, trace_id)
#else  // PW_TRACE_TYPE_ASYNC_INSTANT
#define _PW_TRACE_INSTANT_ARGS4(...) _PW_TRACE_DISABLED(__VA_ARGS__)
#endif  // PW_TRACE_TYPE_ASYNC_INSTANT

// This block handles:
//      - PW_TRACE_START(label)
//      - PW_TRACE_START_FLAG(flag, label)
// Which creates a trace event with the type: PW_TRACE_TYPE_DURATION_START
// NOTE: If this type is not defined by the backend this trace is removed.
#ifdef PW_TRACE_TYPE_DURATION_START
#define _PW_TRACE_START_ARGS2(flag, label)           \
  _PW_TRACE_IF_ENABLED(PW_TRACE_TYPE_DURATION_START, \
                       flag,                         \
                       label,                        \
                       PW_TRACE_GROUP_LABEL_DEFAULT, \
                       PW_TRACE_TRACE_ID_DEFAULT)
#else  // PW_TRACE_TYPE_DURATION_START
#define _PW_TRACE_START_ARGS2(...) _PW_TRACE_DISABLED(__VA_ARGS__)
#endif  // PW_TRACE_TYPE_DURATION_START

// This block handles:
//      - PW_TRACE_START(label, group)
//      - PW_TRACE_START_FLAG(flag, label, group)
// Which creates a trace event with the type: PW_TRACE_TYPE_DURATION_GROUP_START
// NOTE: If this type is not defined by the backend this trace is removed.
#ifdef PW_TRACE_TYPE_DURATION_GROUP_START  // Disabled if backend doesn't define
                                           // this
#define _PW_TRACE_START_ARGS3(flag, label, group)          \
  _PW_TRACE_IF_ENABLED(PW_TRACE_TYPE_DURATION_GROUP_START, \
                       flag,                               \
                       label,                              \
                       group,                              \
                       PW_TRACE_TRACE_ID_DEFAULT)
#else  // PW_TRACE_TYPE_DURATION_GROUP_START
#define _PW_TRACE_START_ARGS3(...) _PW_TRACE_DISABLED(__VA_ARGS__)
#endif  // PW_TRACE_TYPE_DURATION_GROUP_START

// This block handles:
//      - PW_TRACE_START(label, group, trace_id)
//      - PW_TRACE_START_FLAG(flag, label, group, trace_id)
// Which creates a trace event with the type: PW_TRACE_TYPE_ASYNC_START
// NOTE: If this type is not defined by the backend this trace is removed.
#ifdef PW_TRACE_TYPE_ASYNC_START
#define _PW_TRACE_START_ARGS4(flag, label, group, trace_id) \
  _PW_TRACE_IF_ENABLED(PW_TRACE_TYPE_ASYNC_START, flag, label, group, trace_id)
#else  // PW_TRACE_TYPE_ASYNC_START
#define _PW_TRACE_START_ARGS4(...) _PW_TRACE_DISABLED(__VA_ARGS__)
#endif  // PW_TRACE_TYPE_ASYNC_START

// This block handles:
//      - PW_TRACE_END(labe)
//      - PW_TRACE_END_FLAG(flag, label)
// Which creates a trace event with the type: PW_TRACE_TYPE_DURATION_END
// NOTE: If this type is not defined by the backend this trace is removed.
#ifdef PW_TRACE_TYPE_DURATION_END
#define _PW_TRACE_END_ARGS2(flag, label)             \
  _PW_TRACE_IF_ENABLED(PW_TRACE_TYPE_DURATION_END,   \
                       flag,                         \
                       label,                        \
                       PW_TRACE_GROUP_LABEL_DEFAULT, \
                       PW_TRACE_TRACE_ID_DEFAULT)
#else  // PW_TRACE_TYPE_DURATION_END
#define _PW_TRACE_END_ARGS2(...) _PW_TRACE_DISABLED(__VA_ARGS__)
#endif  // PW_TRACE_TYPE_DURATION_END

// This block handles:
//      - PW_TRACE_END(label, group)
//      - PW_TRACE_END_FLAG(flag, label, group)
// Which creates a trace event with the type: PW_TRACE_TYPE_DURATION_GROUP_END
// NOTE: If this type is not defined by the backend this trace is removed.
#ifdef PW_TRACE_TYPE_DURATION_GROUP_END
#define _PW_TRACE_END_ARGS3(flag, label, group)          \
  _PW_TRACE_IF_ENABLED(PW_TRACE_TYPE_DURATION_GROUP_END, \
                       flag,                             \
                       label,                            \
                       group,                            \
                       PW_TRACE_TRACE_ID_DEFAULT)
#else  // PW_TRACE_TYPE_DURATION_GROUP_END
#define _PW_TRACE_END_ARGS3(...) _PW_TRACE_DISABLED(__VA_ARGS__)
#endif  // PW_TRACE_TYPE_DURATION_GROUP_END

// This block handles:
//      - PW_TRACE_END(label, group, trace_id)
//      - PW_TRACE_END_FLAG(flag, label, group, trace_id)
// Which creates a trace event with the type: PW_TRACE_TYPE_ASYNC_END
// NOTE: If this type is not defined by the backend this trace is removed.
#ifdef PW_TRACE_TYPE_ASYNC_END
#define _PW_TRACE_END_ARGS4(flag, label, group, trace_id) \
  _PW_TRACE_IF_ENABLED(PW_TRACE_TYPE_ASYNC_END, flag, label, group, trace_id)
#else  // PW_TRACE_TYPE_ASYNC_END
#define _PW_TRACE_END_ARGS4(...) _PW_TRACE_DISABLED(__VA_ARGS__)
#endif  // PW_TRACE_TYPE_ASYNC_END

// The pigweed scope objects gets defined inline with the trace event. The
// constructor handles the start trace event, and the destructor does the end.
#ifndef _PW_TRACE_SCOPE_OBJECT
#define _PW_TRACE_SCOPE_OBJECT(                                              \
    object_name, flag, event_type_start, event_type_end, label, group)       \
  class object_name {                                                        \
   public:                                                                   \
    object_name(const object_name&) = delete;                                \
    object_name(object_name&&) = delete;                                     \
    object_name& operator=(const object_name&) = delete;                     \
    object_name& operator=(object_name&&) = delete;                          \
    object_name(uint32_t trace_id = PW_TRACE_TRACE_ID_DEFAULT)               \
        : trace_id_(trace_id) {                                              \
      _PW_TRACE_IF_ENABLED(event_type_start, flag, label, group, trace_id_); \
    }                                                                        \
    ~object_name() {                                                         \
      _PW_TRACE_IF_ENABLED(event_type_end, flag, label, group, trace_id_);   \
    }                                                                        \
                                                                             \
   private:                                                                  \
    const uint32_t trace_id_;                                                \
  }
#endif  // _PW_TRACE_SCOPE_OBJECT

// This block handles:
//      - PW_TRACE_SCOPE(label)
//      - PW_TRACE_SCOPE_FLAG(flag, label)
//      - PW_TRACE_FUNCTION()
//      - PW_TRACE_FUNCTION_FLAG(flag)
// These each generate two trace events:
//      - PW_TRACE_TYPE_DURATION_START: Where trace event appears in code.
//      - PW_TRACE_TYPE_DURATION_END: When current scope is lost.
// NOTE; If these types are not defined by the backend these traces are removed.
#if defined(PW_TRACE_TYPE_DURATION_START) && defined(PW_TRACE_TYPE_DURATION_END)
#define _PW_TRACE_SCOPE_ARGS2(flag, label)                            \
  _PW_TRACE_SCOPE_OBJECT(PW_CONCAT(_PwTraceScopeObject, __COUNTER__), \
                         flag,                                        \
                         PW_TRACE_TYPE_DURATION_START,                \
                         PW_TRACE_TYPE_DURATION_END,                  \
                         label,                                       \
                         PW_TRACE_GROUP_LABEL_DEFAULT)                \
  PW_CONCAT(_pw_trace_scope_object, __COUNTER__);
#define _PW_TRACE_FUNCTION_ARGS0() \
  _PW_TRACE_SCOPE_ARGS2(PW_TRACE_FLAGS, PW_TRACE_FUNCTION_LABEL)
#define _PW_TRACE_FUNCTION_FLAGS_ARGS1(flag) \
  _PW_TRACE_SCOPE_ARGS2(flag, PW_TRACE_FUNCTION_LABEL)
#else  // PW_TRACE_TYPE_DURATION_GROUP_END
#define _PW_TRACE_SCOPE_ARGS2(...) _PW_TRACE_DISABLED(__VA_ARGS__)
#define _PW_TRACE_FUNCTION_ARGS0()  // No need to/can't call _PW_TRACE_DISABLED
                                    // with zero args.
#define _PW_TRACE_FUNCTION_FLAGS_ARGS1(...) _PW_TRACE_DISABLED(__VA_ARGS__)
#endif  // defined(PW_TRACE_TYPE_DURATION_START) &&
        // defined(PW_TRACE_TYPE_DURATION_END)

// This block handles:
//      - PW_TRACE_SCOPE(label, group)
//      - PW_TRACE_SCOPE_FLAG(flag, label, group)
//      - PW_TRACE_FUNCTION(group)
//      - PW_TRACE_FUNCTION_FLAG(flag, group)
// These each generate two trace events:
//      - PW_TRACE_TYPE_DURATION_GROUP_START: Where trace event appears in code.
//      - PW_TRACE_TYPE_DURATION_GROUP_END: When current scope is lost.
// NOTE; If these types are not defined by the backend these traces are removed.
#if defined(PW_TRACE_TYPE_DURATION_GROUP_START) && \
    defined(PW_TRACE_TYPE_DURATION_GROUP_END)
#define _PW_TRACE_SCOPE_ARGS3(flag, label, group)                     \
  _PW_TRACE_SCOPE_OBJECT(PW_CONCAT(_PwTraceScopeObject, __COUNTER__), \
                         flag,                                        \
                         PW_TRACE_TYPE_DURATION_GROUP_START,          \
                         PW_TRACE_TYPE_DURATION_GROUP_END,            \
                         label,                                       \
                         group)                                       \
  PW_CONCAT(_pw_trace_scope_object, __COUNTER__);
#define _PW_TRACE_FUNCTION_ARGS1(group) \
  _PW_TRACE_SCOPE_ARGS3(PW_TRACE_FLAGS, PW_TRACE_FUNCTION_LABEL, group)
#define _PW_TRACE_FUNCTION_FLAGS_ARGS2(flag, group) \
  _PW_TRACE_SCOPE_ARGS3(flag, PW_TRACE_FUNCTION_LABEL, group)
#else  // PW_TRACE_TYPE_DURATION_GROUP_END
#define _PW_TRACE_SCOPE_ARGS3(...) _PW_TRACE_DISABLED(__VA_ARGS__)
#define _PW_TRACE_FUNCTION_ARGS1(...) _PW_TRACE_DISABLED(__VA_ARGS__)
#define _PW_TRACE_FUNCTION_FLAGS_ARGS2(...) _PW_TRACE_DISABLED(__VA_ARGS__)
#endif  // defined(PW_TRACE_TYPE_DURATION_GROUP_START) &&
        // defined(PW_TRACE_TYPE_DURATION_GROUP_END)

// This block handles:
//      - PW_TRACE_SCOPE(label, group, trace_id)
//      - PW_TRACE_SCOPE_FLAG(flag, label, group, trace_id)
//      - PW_TRACE_FUNCTION(group, trace_id)
//      - PW_TRACE_FUNCTION_FLAG(flag, group, trace_id)
// These each generate two trace events:
//      - PW_TRACE_TYPE_ASYNC_START: Where trace event appears in code.
//      - PW_TRACE_TYPE_ASYNC_END: When current scope is lost.
// NOTE: If these types are not defined by the backend these traces are removed.
#if defined(PW_TRACE_TYPE_ASYNC_START) && defined(PW_TRACE_TYPE_ASYNC_END)
#define _PW_TRACE_SCOPE_ARGS4(flag, label, group, trace_id)           \
  _PW_TRACE_SCOPE_OBJECT(PW_CONCAT(_PwTraceScopeObject, __COUNTER__), \
                         flag,                                        \
                         PW_TRACE_TYPE_ASYNC_START,                   \
                         PW_TRACE_TYPE_ASYNC_END,                     \
                         label,                                       \
                         group)                                       \
  PW_CONCAT(_pw_trace_scope_object, __COUNTER__)(trace_id);
#define _PW_TRACE_FUNCTION_ARGS2(group, trace_id) \
  _PW_TRACE_SCOPE_ARGS4(                          \
      PW_TRACE_FLAGS, PW_TRACE_FUNCTION_LABEL, group, trace_id)
#define _PW_TRACE_FUNCTION_FLAGS_ARGS3(flag, group, trace_id) \
  _PW_TRACE_SCOPE_ARGS4(flag, PW_TRACE_FUNCTION_LABEL, group, trace_id)
#else  // PW_TRACE_TYPE_DURATION_GROUP_END
#define _PW_TRACE_SCOPE_ARGS4(...) _PW_TRACE_DISABLED(__VA_ARGS__)
#define _PW_TRACE_FUNCTION_ARGS2(...) _PW_TRACE_DISABLED(__VA_ARGS__)
#define _PW_TRACE_FUNCTION_FLAGS_ARGS3(...) _PW_TRACE_DISABLED(__VA_ARGS__)
#endif  // defined(PW_TRACE_TYPE_ASYNC_START) &&
        // defined(PW_TRACE_TYPE_ASYNC_END)

// This block handles:
//      - PW_TRACE_INSTANT_DATA(label,
//                              data_format_string,
//                              data,
//                              size)
//      - PW_TRACE_INSTANT_DATA_FLAG(flag,
//                                   label,
//                                   data_format_string,
//                                   data,
//                                   size)
// Which creates a trace event with the type: PW_TRACE_TYPE_INSTANT
// NOTE: If this type or PW_TRACE_DATA is not defined by the backend this trace
// is removed.
#if defined(PW_TRACE_DATA) && defined(PW_TRACE_TYPE_INSTANT)
#define _PW_TRACE_INSTANT_DATA_ARGS5(                     \
    flag, label, data_format_string, data, size)          \
  _PW_TRACE_DATA_IF_ENABLED(PW_TRACE_TYPE_INSTANT,        \
                            flag,                         \
                            label,                        \
                            PW_TRACE_GROUP_LABEL_DEFAULT, \
                            PW_TRACE_TRACE_ID_DEFAULT,    \
                            data_format_string,           \
                            data,                         \
                            size)
#else  // defined(PW_TRACE_DATA) && defined(PW_TRACE_TYPE_INSTANT)
#define _PW_TRACE_INSTANT_DATA_ARGS5(...) _PW_TRACE_DISABLED(__VA_ARGS__)
#endif  // defined(PW_TRACE_DATA) && defined(PW_TRACE_TYPE_INSTANT)

// This block handles:
//      - PW_TRACE_INSTANT_DATA(label,
//                              group,
//                              data_format_string,
//                              data,
//                              size)
//      - PW_TRACE_INSTANT_DATA_FLAG(flag,
//                                   label,
//                                   group,
//                                   data_format_string,
//                                   data,
//                                   size)
// Which creates a trace event with the type: PW_TRACE_TYPE_INSTANT_GROUP
// NOTE: If this type or PW_TRACE_DATA is not defined by the backend this trace
// is removed.
#if defined(PW_TRACE_DATA) && defined(PW_TRACE_TYPE_INSTANT_GROUP)
#define _PW_TRACE_INSTANT_DATA_ARGS6(                    \
    flag, label, group, data_format_string, data, size)  \
  _PW_TRACE_DATA_IF_ENABLED(PW_TRACE_TYPE_INSTANT_GROUP, \
                            flag,                        \
                            label,                       \
                            group,                       \
                            PW_TRACE_TRACE_ID_DEFAULT,   \
                            data_format_string,          \
                            data,                        \
                            size)
#else  // defined(PW_TRACE_DATA) && defined(PW_TRACE_TYPE_INSTANT_GROUP)
#define _PW_TRACE_INSTANT_DATA_ARGS6(...) _PW_TRACE_DISABLED(__VA_ARGS__)
#endif  // defined(PW_TRACE_DATA) && defined(PW_TRACE_TYPE_INSTANT_GROUP)

// This block handles:
//      - PW_TRACE_INSTANT_DATA(label,
//                              group,
//                              trace_id
//                              data_format_string,
//                              data,
//                              size)
//      - PW_TRACE_INSTANT_DATA_FLAG(flag,
//                                   label,
//                                   group,
//                                   trace_id
//                                   data_format_string,
//                                   data,
//                                   size)
// Which creates a trace event with the type: PW_TRACE_TYPE_ASYNC_INSTANT
// NOTE: If this type or PW_TRACE_DATA is not defined by the backend this trace
// is removed.
#if defined(PW_TRACE_DATA) && defined(PW_TRACE_TYPE_ASYNC_INSTANT)
#define _PW_TRACE_INSTANT_DATA_ARGS7(                             \
    flag, label, group, trace_id, data_format_string, data, size) \
  _PW_TRACE_DATA_IF_ENABLED(PW_TRACE_TYPE_ASYNC_INSTANT,          \
                            flag,                                 \
                            label,                                \
                            group,                                \
                            trace_id,                             \
                            data_format_string,                   \
                            data,                                 \
                            size)
#else  // defined(PW_TRACE_DATA) && defined(PW_TRACE_TYPE_ASYNC_INSTANT)
#define _PW_TRACE_INSTANT_DATA_ARGS7(...) _PW_TRACE_DISABLED(__VA_ARGS__)
#endif  // defined(PW_TRACE_DATA) && defined(PW_TRACE_TYPE_ASYNC_INSTANT)

// This block handles:
//      - PW_TRACE_START_DATA(label,
//                            data_format_string,
//                            data,
//                            size)
//      - PW_TRACE_START_DATA_FLAG(flag,
//                                 label,
//                                 data_format_string,
//                                 data,
//                                 size)
// Which creates a trace event with the type: PW_TRACE_TYPE_DURATION_START
// NOTE: If this type or PW_TRACE_DATA is not defined by the backend this trace
// is removed.
#if defined(PW_TRACE_DATA) && defined(PW_TRACE_TYPE_DURATION_START)
#define _PW_TRACE_START_DATA_ARGS5(                       \
    flag, label, data_format_string, data, size)          \
  _PW_TRACE_DATA_IF_ENABLED(PW_TRACE_TYPE_DURATION_START, \
                            flag,                         \
                            label,                        \
                            PW_TRACE_GROUP_LABEL_DEFAULT, \
                            PW_TRACE_TRACE_ID_DEFAULT,    \
                            data_format_string,           \
                            data,                         \
                            size)
#else  // defined(PW_TRACE_DATA) && defined(PW_TRACE_TYPE_DURATION_START)
#define _PW_TRACE_START_DATA_ARGS5(...) _PW_TRACE_DISABLED(__VA_ARGS__)
#endif  // defined(PW_TRACE_DATA) && defined(PW_TRACE_TYPE_DURATION_START)

// This block handles:
//      - PW_TRACE_START_DATA(label,
//                            group,
//                            data_format_string,
//                            data,
//                            size)
//      - PW_TRACE_START_DATA_FLAG(flag,
//                                 label,
//                                 group,
//                                 data_format_string,
//                                 data,
//                                 size)
// Which creates a trace event with the type: PW_TRACE_TYPE_DURATION_GROUP_START
// NOTE: If this type or PW_TRACE_DATA is not defined by the backend this trace
// is removed.
#if defined(PW_TRACE_DATA) && defined(PW_TRACE_TYPE_DURATION_GROUP_START)
#define _PW_TRACE_START_DATA_ARGS6(                             \
    flag, label, group, data_format_string, data, size)         \
  _PW_TRACE_DATA_IF_ENABLED(PW_TRACE_TYPE_DURATION_GROUP_START, \
                            flag,                               \
                            label,                              \
                            group,                              \
                            PW_TRACE_TRACE_ID_DEFAULT,          \
                            data_format_string,                 \
                            data,                               \
                            size)
#else  // defined(PW_TRACE_DATA) && defined(PW_TRACE_TYPE_DURATION_START)
#define _PW_TRACE_START_DATA_ARGS6(...) _PW_TRACE_DISABLED(__VA_ARGS__)
#endif  // defined(PW_TRACE_DATA) && defined(PW_TRACE_TYPE_DURATION_START)

// This block handles:
//      - PW_TRACE_START_DATA(label,
//                            group,
//                            trace_id
//                            data_format_string,
//                            data,
//                            size)
//      - PW_TRACE_START_DATA_FLAG(flag,
//                                 label,
//                                 group,
//                                 trace_id
//                                 data_format_string,
//                                 data,
//                                 size)
// Which creates a trace event with the type: PW_TRACE_TYPE_ASYNC_START
// NOTE: If this type or PW_TRACE_DATA is not defined by the backend this trace
// is removed.
#if defined(PW_TRACE_DATA) && defined(PW_TRACE_TYPE_ASYNC_START)
#define _PW_TRACE_START_DATA_ARGS7(                               \
    flag, label, group, trace_id, data_format_string, data, size) \
  _PW_TRACE_DATA_IF_ENABLED(PW_TRACE_TYPE_ASYNC_START,            \
                            flag,                                 \
                            label,                                \
                            group,                                \
                            trace_id,                             \
                            data_format_string,                   \
                            data,                                 \
                            size)
#else  // defined(PW_TRACE_DATA) && defined(PW_TRACE_TYPE_ASYNC_START)
#define _PW_TRACE_START_DATA_ARGS7(...) _PW_TRACE_DISABLED(__VA_ARGS__)
#endif  // defined(PW_TRACE_DATA) && defined(PW_TRACE_TYPE_ASYNC_START)

// This block handles:
//      - PW_TRACE_END_DATA(label,
//                          data_format_string,
//                          data,
//                          size)
//      - PW_TRACE_END_DATA_FLAG(flag,
//                               label,
//                               data_format_string,
//                               data,
//                               size)
// Which creates a trace event with the type: PW_TRACE_TYPE_DURATION_END
// NOTE: If this type or PW_TRACE_DATA is not defined by the backend this trace
// is removed.
#if defined(PW_TRACE_DATA) && defined(PW_TRACE_TYPE_DURATION_END)
#define _PW_TRACE_END_DATA_ARGS5(flag, label, data_format_string, data, size) \
  _PW_TRACE_DATA_IF_ENABLED(PW_TRACE_TYPE_DURATION_END,                       \
                            flag,                                             \
                            label,                                            \
                            PW_TRACE_GROUP_LABEL_DEFAULT,                     \
                            PW_TRACE_TRACE_ID_DEFAULT,                        \
                            data_format_string,                               \
                            data,                                             \
                            size)
#else  // defined(PW_TRACE_DATA) && defined(PW_TRACE_TYPE_DURATION_START)
#define _PW_TRACE_END_DATA_ARGS5(...) _PW_TRACE_DISABLED(__VA_ARGS__)
#endif  // defined(PW_TRACE_DATA) && defined(PW_TRACE_TYPE_DURATION_START)

// This block handles:
//      - PW_TRACE_END_DATA(label,
//                          group,
//                          data_format_string,
//                          data,
//                          size)
//      - PW_TRACE_END_DATA_FLAG(flag,
//                               label,
//                               group,
//                               data_format_string,
//                               data,
//                               size)
// Which creates a trace event with the type: PW_TRACE_TYPE_DURATION_GROUP_END
// NOTE: If this type or PW_TRACE_DATA is not defined by the backend this trace
// is removed.
#if defined(PW_TRACE_DATA) && defined(PW_TRACE_TYPE_DURATION_GROUP_END)
#define _PW_TRACE_END_DATA_ARGS6(                             \
    flag, label, group, data_format_string, data, size)       \
  _PW_TRACE_DATA_IF_ENABLED(PW_TRACE_TYPE_DURATION_GROUP_END, \
                            flag,                             \
                            label,                            \
                            group,                            \
                            PW_TRACE_TRACE_ID_DEFAULT,        \
                            data_format_string,               \
                            data,                             \
                            size)
#else  // defined(PW_TRACE_DATA) && defined(PW_TRACE_TYPE_DURATION_GROUP_END)
#define _PW_TRACE_END_DATA_ARGS6(...) _PW_TRACE_DISABLED(__VA_ARGS__)
#endif  // defined(PW_TRACE_DATA) && defined(PW_TRACE_TYPE_DURATION_GROUP_END)

// This block handles:
//      - PW_TRACE_END_DATA(label,
//                          group,
//                          trace_id
//                          data_format_string,
//                          data,
//                          size)
//      - PW_TRACE_END_DATA_FLAG(flag,
//                               label,
//                               group,
//                               trace_id
//                               data_format_string,
//                               data,
//                               size)
// Which creates a trace event with the type: PW_TRACE_TYPE_ASYNC_END
// NOTE: If this type or PW_TRACE_DATA is not defined by the backend this trace
// is removed.
#if defined(PW_TRACE_DATA) && defined(PW_TRACE_TYPE_ASYNC_END)
#define _PW_TRACE_END_DATA_ARGS7(                                 \
    flag, label, group, trace_id, data_format_string, data, size) \
  _PW_TRACE_DATA_IF_ENABLED(PW_TRACE_TYPE_ASYNC_END,              \
                            flag,                                 \
                            label,                                \
                            group,                                \
                            trace_id,                             \
                            data_format_string,                   \
                            data,                                 \
                            size)
#else  // defined(PW_TRACE_DATA) && defined(PW_TRACE_TYPE_ASYNC_END)
#define _PW_TRACE_END_DATA_ARGS7(...) _PW_TRACE_DISABLED(__VA_ARGS__)
#endif  // defined(PW_TRACE_DATA) && defined(PW_TRACE_TYPE_ASYNC_END)
