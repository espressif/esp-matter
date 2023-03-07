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
//
// This is a simple test that ensures a provided trace backend compiles.

#define PW_TRACE_MODULE_NAME "TST"

#include "pw_trace/trace.h"

#ifdef __cplusplus
#error "This file must be compiled as plain C to verify C compilation works."
#endif  // __cplusplus

void BasicTraceTestPlainC(void) {
  const uint32_t trace_id = 1;
  const char kSomeData[] = "SOME DATA";

  PW_TRACE_INSTANT("Test");
  PW_TRACE_INSTANT("Test", "Group");
  PW_TRACE_INSTANT("Test", "Group", trace_id);

  PW_TRACE_INSTANT_FLAG(PW_TRACE_FLAGS, "Test");
  PW_TRACE_INSTANT_FLAG(PW_TRACE_FLAGS, "Test", "Group");
  PW_TRACE_INSTANT_FLAG(PW_TRACE_FLAGS, "Test", "Group", trace_id);

  PW_TRACE_INSTANT_DATA("Test", "s", kSomeData, sizeof(kSomeData));
  PW_TRACE_INSTANT_DATA("Test", "Group", "s", kSomeData, sizeof(kSomeData));
  PW_TRACE_INSTANT_DATA(
      "Test", "Group", trace_id, "s", kSomeData, sizeof(kSomeData));

  PW_TRACE_INSTANT_DATA_FLAG(
      PW_TRACE_FLAGS, "Test", "s", kSomeData, sizeof(kSomeData));
  PW_TRACE_INSTANT_DATA_FLAG(
      PW_TRACE_FLAGS, "Test", "Group", "s", kSomeData, sizeof(kSomeData));
  PW_TRACE_INSTANT_DATA_FLAG(PW_TRACE_FLAGS,
                             "Test",
                             "Group",
                             trace_id,
                             "s",
                             kSomeData,
                             sizeof(kSomeData));
  PW_TRACE_START("Test");
  PW_TRACE_START("Test", "Group");
  PW_TRACE_START("Test", "Group", trace_id);

  PW_TRACE_START_FLAG(PW_TRACE_FLAGS, "Test");
  PW_TRACE_START_FLAG(PW_TRACE_FLAGS, "Test", "Group");
  PW_TRACE_START_FLAG(PW_TRACE_FLAGS, "Test", "Group", trace_id);

  PW_TRACE_START_DATA("Test", "s", kSomeData, sizeof(kSomeData));
  PW_TRACE_START_DATA("Test", "Group", "s", kSomeData, sizeof(kSomeData));
  PW_TRACE_START_DATA(
      "Test", "Group", trace_id, "s", kSomeData, sizeof(kSomeData));

  PW_TRACE_START_DATA_FLAG(
      PW_TRACE_FLAGS, "Test", "s", kSomeData, sizeof(kSomeData));
  PW_TRACE_START_DATA_FLAG(
      PW_TRACE_FLAGS, "Test", "Group", "s", kSomeData, sizeof(kSomeData));
  PW_TRACE_START_DATA_FLAG(PW_TRACE_FLAGS,
                           "Test",
                           "Group",
                           trace_id,
                           "s",
                           kSomeData,
                           sizeof(kSomeData));

  PW_TRACE_END("Test");
  PW_TRACE_END("Test", "Group");
  PW_TRACE_END("Test", "Group", trace_id);

  PW_TRACE_END_FLAG(PW_TRACE_FLAGS, "Test");
  PW_TRACE_END_FLAG(PW_TRACE_FLAGS, "Test", "Group");
  PW_TRACE_END_FLAG(PW_TRACE_FLAGS, "Test", "Group", trace_id);

  PW_TRACE_END_DATA("Test", "s", kSomeData, sizeof(kSomeData));
  PW_TRACE_END_DATA("Test", "Group", "s", kSomeData, sizeof(kSomeData));
  PW_TRACE_END_DATA(
      "Test", "Group", trace_id, "s", kSomeData, sizeof(kSomeData));

  PW_TRACE_END_DATA_FLAG(
      PW_TRACE_FLAGS, "Test", "s", kSomeData, sizeof(kSomeData));
  PW_TRACE_END_DATA_FLAG(
      PW_TRACE_FLAGS, "Test", "Group", "s", kSomeData, sizeof(kSomeData));
  PW_TRACE_END_DATA_FLAG(PW_TRACE_FLAGS,
                         "Test",
                         "Group",
                         trace_id,
                         "s",
                         kSomeData,
                         sizeof(kSomeData));
}
