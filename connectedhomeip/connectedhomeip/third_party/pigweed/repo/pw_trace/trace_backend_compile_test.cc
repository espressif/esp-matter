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
#include "gtest/gtest.h"
#include "pw_trace/trace.h"

namespace {

void TraceFunction() { PW_TRACE_FUNCTION(); }
void TraceFunctionGroup() { PW_TRACE_FUNCTION("FunctionGroup"); }
void TraceFunctionGroupId() {
  const uint32_t trace_id = 1;
  PW_TRACE_FUNCTION("FunctionGroup", trace_id);
}
void TraceFunctionFlag() { PW_TRACE_FUNCTION_FLAG(PW_TRACE_FLAGS); }
void TraceFunctionFlagGroup() {
  PW_TRACE_FUNCTION_FLAG(PW_TRACE_FLAGS, "FunctionGroup");
}
void TraceFunctionFlagGroupId() {
  const uint32_t trace_id = 1;
  PW_TRACE_FUNCTION_FLAG(PW_TRACE_FLAGS, "FunctionGroup", trace_id);
}

const char kSomeData[] = "SOME DATA";

}  // namespace

TEST(BasicTrace, Instant) {
  const uint32_t trace_id = 1;
  PW_TRACE_INSTANT("Test");
  PW_TRACE_INSTANT("Test", "Group");
  PW_TRACE_INSTANT("Test", "Group", trace_id);
}

TEST(BasicTrace, InstantFlag) {
  const uint32_t trace_id = 1;
  PW_TRACE_INSTANT_FLAG(PW_TRACE_FLAGS, "Test");
  PW_TRACE_INSTANT_FLAG(PW_TRACE_FLAGS, "Test", "Group");
  PW_TRACE_INSTANT_FLAG(PW_TRACE_FLAGS, "Test", "Group", trace_id);
}

TEST(BasicTrace, InstantData) {
  const uint32_t trace_id = 1;
  PW_TRACE_INSTANT_DATA("Test", "s", kSomeData, sizeof(kSomeData));
  PW_TRACE_INSTANT_DATA("Test", "Group", "s", kSomeData, sizeof(kSomeData));
  PW_TRACE_INSTANT_DATA(
      "Test", "Group", trace_id, "s", kSomeData, sizeof(kSomeData));
}

TEST(BasicTrace, InstantDataFlag) {
  const uint32_t trace_id = 1;
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
}

TEST(BasicTrace, Start) {
  const uint32_t trace_id = 1;
  PW_TRACE_START("Test");
  PW_TRACE_START("Test", "Group");
  PW_TRACE_START("Test", "Group", trace_id);
}

TEST(BasicTrace, StartFlag) {
  const uint32_t trace_id = 1;
  PW_TRACE_START_FLAG(PW_TRACE_FLAGS, "Test");
  PW_TRACE_START_FLAG(PW_TRACE_FLAGS, "Test", "Group");
  PW_TRACE_START_FLAG(PW_TRACE_FLAGS, "Test", "Group", trace_id);
}

TEST(BasicTrace, StartData) {
  const uint32_t trace_id = 1;
  PW_TRACE_START_DATA("Test", "s", kSomeData, sizeof(kSomeData));
  PW_TRACE_START_DATA("Test", "Group", "s", kSomeData, sizeof(kSomeData));
  PW_TRACE_START_DATA(
      "Test", "Group", trace_id, "s", kSomeData, sizeof(kSomeData));
}

TEST(BasicTrace, StartDataFlag) {
  const uint32_t trace_id = 1;
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
}

TEST(BasicTrace, End) {
  const uint32_t trace_id = 1;
  PW_TRACE_END("Test");
  PW_TRACE_END("Test", "Group");
  PW_TRACE_END("Test", "Group", trace_id);
}

TEST(BasicTrace, EndFlag) {
  const uint32_t trace_id = 1;
  PW_TRACE_END_FLAG(PW_TRACE_FLAGS, "Test");
  PW_TRACE_END_FLAG(PW_TRACE_FLAGS, "Test", "Group");
  PW_TRACE_END_FLAG(PW_TRACE_FLAGS, "Test", "Group", trace_id);
}

TEST(BasicTrace, EndData) {
  const uint32_t trace_id = 1;
  PW_TRACE_END_DATA("Test", "s", kSomeData, sizeof(kSomeData));
  PW_TRACE_END_DATA("Test", "Group", "s", kSomeData, sizeof(kSomeData));
  PW_TRACE_END_DATA(
      "Test", "Group", trace_id, "s", kSomeData, sizeof(kSomeData));
}

TEST(BasicTrace, EndDataFlag) {
  const uint32_t trace_id = 1;
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

TEST(BasicTrace, Scope) {
  const uint32_t trace_id = 1;
  PW_TRACE_SCOPE("scoped trace");
  PW_TRACE_SCOPE("scoped trace", "Group");
  PW_TRACE_SCOPE("scoped trace", "Group", trace_id);
  {
    PW_TRACE_SCOPE("sub scoped trace");
    PW_TRACE_SCOPE("sub scoped trace", "Group");
    PW_TRACE_SCOPE("sub scoped trace", "Group", trace_id);
  }
}

TEST(BasicTrace, ScopeFlag) {
  const uint32_t trace_id = 1;
  PW_TRACE_SCOPE_FLAG(PW_TRACE_FLAGS, "scoped trace");
  PW_TRACE_SCOPE_FLAG(PW_TRACE_FLAGS, "scoped trace", "Group");
  PW_TRACE_SCOPE_FLAG(PW_TRACE_FLAGS, "scoped trace", "Group", trace_id);
  {
    PW_TRACE_SCOPE_FLAG(PW_TRACE_FLAGS, "sub scoped trace");
    PW_TRACE_SCOPE_FLAG(PW_TRACE_FLAGS, "sub scoped trace", "Group");
    PW_TRACE_SCOPE_FLAG(PW_TRACE_FLAGS, "sub scoped trace", "Group", trace_id);
  }
}

TEST(BasicTrace, Function) {
  TraceFunction();
  TraceFunctionGroup();
  TraceFunctionGroupId();
}

TEST(BasicTrace, FunctionFlag) {
  TraceFunctionFlag();
  TraceFunctionFlagGroup();
  TraceFunctionFlagGroupId();
}
