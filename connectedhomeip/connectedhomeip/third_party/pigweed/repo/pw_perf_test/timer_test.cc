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

#include "pw_perf_test/internal/timer.h"

#include "gtest/gtest.h"

namespace pw::perf_test::internal {
namespace {

TEST(TimerTest, DurationIsPositive) {
  Timestamp start = GetCurrentTimestamp();
  for (volatile int i = 0; i < 1000; i = i + 1) {
  }
  Timestamp end = GetCurrentTimestamp();
  int64_t duration = GetDuration(start, end);
  EXPECT_GT(duration, 0);
}

}  // namespace
}  // namespace pw::perf_test::internal
