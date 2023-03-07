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

#include "pw_polyfill/standard.h"

#if PW_CXX_STANDARD_IS_SUPPORTED(20)

#include <span>

#include "gtest/gtest.h"
#include "pw_span/span.h"

namespace {

constexpr int kCArray[5] = {0, 1, 2, 3, 4};

void TakesPwSpan(pw::span<const int>) {}
void TakesStdSpan(std::span<const int>) {}

TEST(SpanCompatibility, CallFunction) {
  TakesPwSpan(std::span<const int>(kCArray));
  TakesStdSpan(pw::span<const int>(kCArray));
}

TEST(SpanCompatibility, StdToPwConversions) {
  std::span<const int> std_span(kCArray);
  pw::span<const int> pw_span(std_span);

  EXPECT_EQ(std_span.data(), pw_span.data());
  EXPECT_EQ(std_span.size(), pw_span.size());

  pw_span = std_span;

  EXPECT_EQ(std_span.data(), pw_span.data());
  EXPECT_EQ(std_span.size(), pw_span.size());
}

TEST(SpanCompatibility, PwToStdConversions) {
  pw::span<const int> pw_span(kCArray);
  std::span<const int> std_span(pw_span);

  EXPECT_EQ(std_span.data(), pw_span.data());
  EXPECT_EQ(std_span.size(), pw_span.size());

  std_span = pw_span;

  EXPECT_EQ(std_span.data(), pw_span.data());
  EXPECT_EQ(std_span.size(), pw_span.size());
}

TEST(SpanCompatibility, SameArray) {
  pw::span<const int> pw_span(kCArray);
  std::span<const int> std_span(kCArray);

  EXPECT_EQ(std_span.data(), pw_span.data());
  EXPECT_EQ(std_span.size(), pw_span.size());

  EXPECT_EQ(std_span[0], 0);
  EXPECT_EQ(pw_span[0], 0);
  EXPECT_EQ(std_span[4], 4);
  EXPECT_EQ(pw_span[4], 4);
}

}  // namespace

#endif  // PW_CXX_STANDARD_IS_SUPPORTED(20)
