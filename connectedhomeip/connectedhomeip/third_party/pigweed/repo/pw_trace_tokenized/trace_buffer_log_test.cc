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

#include "pw_trace_tokenized/trace_buffer_log.h"

#include "gtest/gtest.h"
#include "pw_trace/trace.h"

namespace pw::trace {
namespace {

TEST(TokenizedTrace, DumpSmallBuffer) {
  // TODO(b/235283406): This test only verifies that the dump function does not
  // crash, and requires manual inspection to confirm that the log output is
  // correct. When there is support to mock and verify the calls to pw_log,
  // these tests should be improved to validate the output.
  PW_TRACE_SET_ENABLED(true);
  PW_TRACE_INSTANT("test1");
  PW_TRACE_INSTANT("test2");
  ASSERT_EQ(OkStatus(), pw::trace::DumpTraceBufferToLog());
}

TEST(TokenizedTrace, DumpLargeBuffer) {
  // TODO(b/235283406): This test only verifies that the dump function does not
  // crash, and requires manual inspection to confirm that the log output is
  // correct. When there is support to mock and verify the calls to pw_log,
  // these tests should be improved to validate the output.
  PW_TRACE_SET_ENABLED(true);
  for (int i = 0; i < 100; i++) {
    PW_TRACE_INSTANT("test");
  }
  ASSERT_EQ(OkStatus(), pw::trace::DumpTraceBufferToLog());
}

}  // namespace
}  // namespace pw::trace
