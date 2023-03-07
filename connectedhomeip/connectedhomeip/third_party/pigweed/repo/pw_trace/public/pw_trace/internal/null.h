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

#include <stddef.h>
#include <stdint.h>

#include "pw_preprocessor/compiler.h"
#include "pw_preprocessor/util.h"

// Enable traces
#define PW_TRACE_TYPE_INSTANT 1
#define PW_TRACE_TYPE_INSTANT_GROUP 1
#define PW_TRACE_TYPE_DURATION_START 1
#define PW_TRACE_TYPE_DURATION_END 1
#define PW_TRACE_TYPE_DURATION_GROUP_START 1
#define PW_TRACE_TYPE_DURATION_GROUP_END 1
#define PW_TRACE_TYPE_ASYNC_START 1
#define PW_TRACE_TYPE_ASYNC_INSTANT 1
#define PW_TRACE_TYPE_ASYNC_END 1

PW_EXTERN_C_START

// Empty function for compiling out trace statements. Since the function is
// empty and inline, it should be completely compiled out. This function
// accomplishes following:
//
//   - Uses the arguments to PW_TRACE, which avoids "unused variable" warnings.
//   - Executes expressions passed to PW_TRACE, so that the behavior is
//     consistent between this null backend and an actual backend.
//
// These two functions are used in PW_TRACE and PW_TRACE_DATA.

static inline void pw_trace_Ignored(int event_type,
                                    uint8_t flags,
                                    const char* label,
                                    const char* group,
                                    uint32_t trace_id) {
  (void)event_type;
  (void)flags;
  (void)label;
  (void)group;
  (void)trace_id;
}

static inline void pw_trace_data_Ignored(int event_type,
                                         uint8_t flags,
                                         const char* label,
                                         const char* group,
                                         uint32_t trace_id,
                                         const char* type,
                                         const char* data,
                                         size_t size) {
  (void)event_type;
  (void)flags;
  (void)label;
  (void)group;
  (void)trace_id;
  (void)type;
  (void)data;
  (void)size;
}

PW_EXTERN_C_END

#define PW_TRACE(event_type, flags, label, group, trace_id) \
  pw_trace_Ignored(event_type, flags, label, group, trace_id)

#define PW_TRACE_DATA(                                           \
    event_type, flags, label, group, trace_id, type, data, size) \
  pw_trace_data_Ignored(                                         \
      event_type, flags, label, group, trace_id, type, data, size)
