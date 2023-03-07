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

#define PW_TRACE_MODULE_NAME "TST"
#include "gtest/gtest.h"
#include "pw_trace/trace.h"
#include "pw_trace_test/fake_backend.h"

using namespace trace_fake_backend;

// Used by fake backend
LastEvent LastEvent::instance_;

namespace {

void TraceFunction() {
  PW_TRACE_FUNCTION();
  // Can't check label, since might change depending on compiler.
  EXPECT_TRUE(LastEvent::Instance().Get().IsEqualIgnoreLabel(
      Event(DurationStart,
            PW_TRACE_FLAGS_DEFAULT,
            nullptr,
            PW_TRACE_GROUP_LABEL_DEFAULT,
            PW_TRACE_TRACE_ID_DEFAULT)));
}

void TraceFunctionGroup() {
  PW_TRACE_FUNCTION("FunctionGroup");
  // Can't check label, since might change depending on compiler.
  EXPECT_TRUE(LastEvent::Instance().Get().IsEqualIgnoreLabel(
      Event(DurationGroupStart,
            PW_TRACE_FLAGS_DEFAULT,
            nullptr,
            "FunctionGroup",
            PW_TRACE_TRACE_ID_DEFAULT)));
}

const char kSomeData[] = "SOME DATA";

}  // namespace

TEST(BasicTrace, Instant) {
  PW_TRACE_INSTANT("Test");
  EXPECT_EQ(LastEvent::Instance().Get(),
            Event(Instantaneous,
                  PW_TRACE_FLAGS_DEFAULT,
                  "Test",
                  PW_TRACE_GROUP_LABEL_DEFAULT,
                  PW_TRACE_TRACE_ID_DEFAULT));
}

TEST(BasicTrace, InstantGroup) {
  PW_TRACE_INSTANT("Test", "group");
  EXPECT_EQ(LastEvent::Instance().Get(),
            Event(InstantaneousGroup,
                  PW_TRACE_FLAGS_DEFAULT,
                  "Test",
                  "group",
                  PW_TRACE_TRACE_ID_DEFAULT));
}

TEST(BasicTrace, Duration) {
  PW_TRACE_START("Test");
  EXPECT_EQ(LastEvent::Instance().Get(),
            Event(DurationStart,
                  PW_TRACE_FLAGS_DEFAULT,
                  "Test",
                  PW_TRACE_GROUP_LABEL_DEFAULT,
                  PW_TRACE_TRACE_ID_DEFAULT));
  PW_TRACE_END("Test");
  EXPECT_EQ(LastEvent::Instance().Get(),
            Event(DurationEnd,
                  PW_TRACE_FLAGS_DEFAULT,
                  "Test",
                  PW_TRACE_GROUP_LABEL_DEFAULT,
                  PW_TRACE_TRACE_ID_DEFAULT));
}

TEST(BasicTrace, DurationGroup) {
  PW_TRACE_START("Parent", "group");
  EXPECT_EQ(LastEvent::Instance().Get(),
            Event(DurationGroupStart,
                  PW_TRACE_FLAGS_DEFAULT,
                  "Parent",
                  "group",
                  PW_TRACE_TRACE_ID_DEFAULT));
  PW_TRACE_START("Child", "group");
  EXPECT_EQ(LastEvent::Instance().Get(),
            Event(DurationGroupStart,
                  PW_TRACE_FLAGS_DEFAULT,
                  "Child",
                  "group",
                  PW_TRACE_TRACE_ID_DEFAULT));
  PW_TRACE_END("Child", "group");
  EXPECT_EQ(LastEvent::Instance().Get(),
            Event(DurationGroupEnd,
                  PW_TRACE_FLAGS_DEFAULT,
                  "Child",
                  "group",
                  PW_TRACE_TRACE_ID_DEFAULT));
  PW_TRACE_START("Other Child", "group");
  EXPECT_EQ(LastEvent::Instance().Get(),
            Event(DurationGroupStart,
                  PW_TRACE_FLAGS_DEFAULT,
                  "Other Child",
                  "group",
                  PW_TRACE_TRACE_ID_DEFAULT));
  PW_TRACE_END("Other Child", "group");
  EXPECT_EQ(LastEvent::Instance().Get(),
            Event(DurationGroupEnd,
                  PW_TRACE_FLAGS_DEFAULT,
                  "Other Child",
                  "group",
                  PW_TRACE_TRACE_ID_DEFAULT));
  PW_TRACE_END("Parent", "group");
  EXPECT_EQ(LastEvent::Instance().Get(),
            Event(DurationGroupEnd,
                  PW_TRACE_FLAGS_DEFAULT,
                  "Parent",
                  "group",
                  PW_TRACE_TRACE_ID_DEFAULT));
}

TEST(BasicTrace, Async) {
  uint32_t trace_id = 1;
  PW_TRACE_START("async", "group", trace_id);
  EXPECT_EQ(
      LastEvent::Instance().Get(),
      Event(AsyncStart, PW_TRACE_FLAGS_DEFAULT, "async", "group", trace_id));
  PW_TRACE_INSTANT("step", "group", trace_id);
  EXPECT_EQ(
      LastEvent::Instance().Get(),
      Event(AsyncStep, PW_TRACE_FLAGS_DEFAULT, "step", "group", trace_id));
  PW_TRACE_END("async", "group", trace_id);
  EXPECT_EQ(
      LastEvent::Instance().Get(),
      Event(AsyncEnd, PW_TRACE_FLAGS_DEFAULT, "async", "group", trace_id));
}

TEST(BasicTrace, Scope) {
  {
    PW_TRACE_SCOPE("scoped trace");
    EXPECT_EQ(LastEvent::Instance().Get(),
              Event(DurationStart,
                    PW_TRACE_FLAGS_DEFAULT,
                    "scoped trace",
                    PW_TRACE_GROUP_LABEL_DEFAULT,
                    PW_TRACE_TRACE_ID_DEFAULT));
  }
  EXPECT_EQ(LastEvent::Instance().Get(),
            Event(DurationEnd,
                  PW_TRACE_FLAGS_DEFAULT,
                  "scoped trace",
                  PW_TRACE_GROUP_LABEL_DEFAULT,
                  PW_TRACE_TRACE_ID_DEFAULT));
}

TEST(BasicTrace, ScopeGroup) {
  {
    PW_TRACE_SCOPE("scoped group trace", "group");
    EXPECT_EQ(LastEvent::Instance().Get(),
              Event(DurationGroupStart,
                    PW_TRACE_FLAGS_DEFAULT,
                    "scoped group trace",
                    "group",
                    PW_TRACE_TRACE_ID_DEFAULT));
    {
      PW_TRACE_SCOPE("sub scoped group trace", "group");
      EXPECT_EQ(LastEvent::Instance().Get(),
                Event(DurationGroupStart,
                      PW_TRACE_FLAGS_DEFAULT,
                      "sub scoped group trace",
                      "group",
                      PW_TRACE_TRACE_ID_DEFAULT));
    }
    EXPECT_EQ(LastEvent::Instance().Get(),
              Event(DurationGroupEnd,
                    PW_TRACE_FLAGS_DEFAULT,
                    "sub scoped group trace",
                    "group",
                    PW_TRACE_TRACE_ID_DEFAULT));
  }
  EXPECT_EQ(LastEvent::Instance().Get(),
            Event(DurationGroupEnd,
                  PW_TRACE_FLAGS_DEFAULT,
                  "scoped group trace",
                  "group",
                  PW_TRACE_TRACE_ID_DEFAULT));
}

TEST(BasicTrace, ScopeTraceId) {
  static constexpr uint32_t kTraceId = 5;
  {
    PW_TRACE_SCOPE("scoped trace id", "group", kTraceId);
    EXPECT_EQ(LastEvent::Instance().Get(),
              Event(AsyncStart,
                    PW_TRACE_FLAGS_DEFAULT,
                    "scoped trace id",
                    "group",
                    kTraceId));
  }
  EXPECT_EQ(LastEvent::Instance().Get(),
            Event(AsyncEnd,
                  PW_TRACE_FLAGS_DEFAULT,
                  "scoped trace id",
                  "group",
                  kTraceId));
}

TEST(BasicTrace, Function) {
  TraceFunction();
  // Can't check label, since might change depending on compiler.
  EXPECT_TRUE(LastEvent::Instance().Get().IsEqualIgnoreLabel(
      Event(DurationEnd,
            PW_TRACE_FLAGS_DEFAULT,
            nullptr,
            PW_TRACE_GROUP_LABEL_DEFAULT,
            PW_TRACE_TRACE_ID_DEFAULT)));
}

TEST(BasicTrace, FunctionGroup) {
  TraceFunctionGroup();
  // Can't check label, since might change depending on compiler.
  EXPECT_TRUE(LastEvent::Instance().Get().IsEqualIgnoreLabel(
      Event(DurationGroupEnd,
            PW_TRACE_FLAGS_DEFAULT,
            nullptr,
            "FunctionGroup",
            PW_TRACE_TRACE_ID_DEFAULT)));
}

TEST(BasicTrace, InstantData) {
  PW_TRACE_INSTANT_DATA("Test", "s", kSomeData, sizeof(kSomeData));
  EXPECT_EQ(LastEvent::Instance().Get(),
            Event(Instantaneous,
                  PW_TRACE_FLAGS_DEFAULT,
                  "Test",
                  PW_TRACE_GROUP_LABEL_DEFAULT,
                  PW_TRACE_TRACE_ID_DEFAULT,
                  "s",
                  kSomeData,
                  sizeof(kSomeData)));
}

TEST(BasicTrace, InstantGroupData) {
  PW_TRACE_INSTANT_DATA("Test", "Group", "s", kSomeData, sizeof(kSomeData));
  EXPECT_EQ(LastEvent::Instance().Get(),
            Event(InstantaneousGroup,
                  PW_TRACE_FLAGS_DEFAULT,
                  "Test",
                  "Group",
                  PW_TRACE_TRACE_ID_DEFAULT,
                  "s",
                  kSomeData,
                  sizeof(kSomeData)));
}

TEST(BasicTrace, DurationData) {
  PW_TRACE_START_DATA("Test", "s", kSomeData, sizeof(kSomeData));
  EXPECT_EQ(LastEvent::Instance().Get(),
            Event(DurationStart,
                  PW_TRACE_FLAGS_DEFAULT,
                  "Test",
                  PW_TRACE_GROUP_LABEL_DEFAULT,
                  PW_TRACE_TRACE_ID_DEFAULT,
                  "s",
                  kSomeData,
                  sizeof(kSomeData)));
  PW_TRACE_END_DATA("Test", "s", kSomeData, sizeof(kSomeData));
  EXPECT_EQ(LastEvent::Instance().Get(),
            Event(DurationEnd,
                  PW_TRACE_FLAGS_DEFAULT,
                  "Test",
                  PW_TRACE_GROUP_LABEL_DEFAULT,
                  PW_TRACE_TRACE_ID_DEFAULT,
                  "s",
                  kSomeData,
                  sizeof(kSomeData)));
}

TEST(BasicTrace, DurationGroupData) {
  PW_TRACE_START_DATA("Parent", "group", "s", kSomeData, sizeof(kSomeData));
  EXPECT_EQ(LastEvent::Instance().Get(),
            Event(DurationGroupStart,
                  PW_TRACE_FLAGS_DEFAULT,
                  "Parent",
                  "group",
                  PW_TRACE_TRACE_ID_DEFAULT,
                  "s",
                  kSomeData,
                  sizeof(kSomeData)));
  PW_TRACE_START_DATA("Child", "group", "s", kSomeData, sizeof(kSomeData));
  EXPECT_EQ(LastEvent::Instance().Get(),
            Event(DurationGroupStart,
                  PW_TRACE_FLAGS_DEFAULT,
                  "Child",
                  "group",
                  PW_TRACE_TRACE_ID_DEFAULT,
                  "s",
                  kSomeData,
                  sizeof(kSomeData)));
  PW_TRACE_END_DATA("Child", "group", "s", kSomeData, sizeof(kSomeData));
  EXPECT_EQ(LastEvent::Instance().Get(),
            Event(DurationGroupEnd,
                  PW_TRACE_FLAGS_DEFAULT,
                  "Child",
                  "group",
                  PW_TRACE_TRACE_ID_DEFAULT,
                  "s",
                  kSomeData,
                  sizeof(kSomeData)));
  PW_TRACE_END_DATA("Parent", "group", "s", kSomeData, sizeof(kSomeData));
  EXPECT_EQ(LastEvent::Instance().Get(),
            Event(DurationGroupEnd,
                  PW_TRACE_FLAGS_DEFAULT,
                  "Parent",
                  "group",
                  PW_TRACE_TRACE_ID_DEFAULT,
                  "s",
                  kSomeData,
                  sizeof(kSomeData)));
}

TEST(BasicTrace, AsyncData) {
  uint32_t trace_id = 1;
  PW_TRACE_START_DATA(
      "label for start", "group", trace_id, "s", kSomeData, sizeof(kSomeData));
  EXPECT_EQ(LastEvent::Instance().Get(),
            Event(AsyncStart,
                  PW_TRACE_FLAGS_DEFAULT,
                  "label for start",
                  "group",
                  trace_id,
                  "s",
                  kSomeData,
                  sizeof(kSomeData)));
  PW_TRACE_INSTANT_DATA(
      "label for step", "group", trace_id, "s", kSomeData, sizeof(kSomeData));
  EXPECT_EQ(LastEvent::Instance().Get(),
            Event(AsyncStep,
                  PW_TRACE_FLAGS_DEFAULT,
                  "label for step",
                  "group",
                  trace_id,
                  "s",
                  kSomeData,
                  sizeof(kSomeData)));
  PW_TRACE_END_DATA(
      "label for end", "group", trace_id, "s", kSomeData, sizeof(kSomeData));
  EXPECT_EQ(LastEvent::Instance().Get(),
            Event(AsyncEnd,
                  PW_TRACE_FLAGS_DEFAULT,
                  "label for end",
                  "group",
                  trace_id,
                  "s",
                  kSomeData,
                  sizeof(kSomeData)));
}

TEST(BasicTrace, ProvideFlag) {
  PW_TRACE_INSTANT_FLAG(5, "Test");
  EXPECT_EQ(LastEvent::Instance().Get(),
            Event(Instantaneous,
                  5,
                  "Test",
                  PW_TRACE_GROUP_LABEL_DEFAULT,
                  PW_TRACE_TRACE_ID_DEFAULT));
}

TEST(BasicTrace, MacroFlag) {
#undef PW_TRACE_FLAGS
#define PW_TRACE_FLAGS 6
  PW_TRACE_INSTANT("Test");
  EXPECT_EQ(LastEvent::Instance().Get(),
            Event(Instantaneous,
                  6,
                  "Test",
                  PW_TRACE_GROUP_LABEL_DEFAULT,
                  PW_TRACE_TRACE_ID_DEFAULT));
#undef PW_TRACE_FLAGS
#define PW_TRACE_FLAGS PW_TRACE_FLAGS_DEFAULT
  PW_TRACE_INSTANT("Test");
  EXPECT_EQ(LastEvent::Instance().Get(),
            Event(Instantaneous,
                  PW_TRACE_FLAGS_DEFAULT,
                  "Test",
                  PW_TRACE_GROUP_LABEL_DEFAULT,
                  PW_TRACE_TRACE_ID_DEFAULT));
}

TEST(DisableTrace, Instant) {
  PW_TRACE_INSTANT("Test");
  EXPECT_EQ(LastEvent::Instance().Get(),
            Event(Instantaneous,
                  PW_TRACE_FLAGS_DEFAULT,
                  "Test",
                  PW_TRACE_GROUP_LABEL_DEFAULT,
                  PW_TRACE_TRACE_ID_DEFAULT));
#undef PW_TRACE_ENABLE
#define PW_TRACE_ENABLE 0
  PW_TRACE_INSTANT("TestDisabled");

  EXPECT_EQ(LastEvent::Instance().Get(),
            Event(Instantaneous,
                  PW_TRACE_FLAGS_DEFAULT,
                  "Test",
                  PW_TRACE_GROUP_LABEL_DEFAULT,
                  PW_TRACE_TRACE_ID_DEFAULT));
#undef PW_TRACE_ENABLE
#define PW_TRACE_ENABLE 1
}

TEST(DisableTrace, InstantData) {
  PW_TRACE_INSTANT_DATA("Test", "s", kSomeData, sizeof(kSomeData));
  EXPECT_EQ(LastEvent::Instance().Get(),
            Event(Instantaneous,
                  PW_TRACE_FLAGS_DEFAULT,
                  "Test",
                  PW_TRACE_GROUP_LABEL_DEFAULT,
                  PW_TRACE_TRACE_ID_DEFAULT,
                  "s",
                  kSomeData,
                  sizeof(kSomeData)));

#undef PW_TRACE_ENABLE
#define PW_TRACE_ENABLE 0
  PW_TRACE_INSTANT_DATA("TestDisabled", "s", kSomeData, sizeof(kSomeData));
  EXPECT_EQ(LastEvent::Instance().Get(),
            Event(Instantaneous,
                  PW_TRACE_FLAGS_DEFAULT,
                  "Test",
                  PW_TRACE_GROUP_LABEL_DEFAULT,
                  PW_TRACE_TRACE_ID_DEFAULT,
                  "s",
                  kSomeData,
                  sizeof(kSomeData)));
#undef PW_TRACE_ENABLE
#define PW_TRACE_ENABLE 1
}
