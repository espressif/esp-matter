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
#include "pw_analog/analog_input.h"

#include "gtest/gtest.h"

namespace pw {
namespace analog {
namespace {

constexpr int32_t kLimitsMax = 4096;
constexpr int32_t kLimitsMin = 0;

// Fake test analog input that's used for testing.
class TestAnalogInput : public AnalogInput {
 public:
  TestAnalogInput()
      : limits_({
            .min = kLimitsMin,
            .max = kLimitsMax,
        }) {}

  Result<int32_t> TryReadUntil(chrono::SystemClock::time_point) override {
    return Status::Unimplemented();
  }

  Limits GetLimits() const override { return limits_; }

 private:
  const Limits limits_;
};

TEST(AnalogInputTest, Construction) {
  TestAnalogInput analog_input = TestAnalogInput();
}

TEST(AnalogInputTest, GetLimits) {
  TestAnalogInput analog_input = TestAnalogInput();
  AnalogInput::Limits limits = analog_input.GetLimits();
  EXPECT_EQ(limits.min, kLimitsMin);
  EXPECT_EQ(limits.max, kLimitsMax);
}

}  // namespace
}  // namespace analog
}  // namespace pw
