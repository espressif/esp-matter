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
#include "pw_analog/microvolt_input.h"

#include "gtest/gtest.h"

namespace pw {
namespace analog {
namespace {

using namespace std::chrono_literals;

constexpr int32_t kLimitsMax = 4096;
constexpr int32_t kLimitsMin = 0;
constexpr int32_t kReferenceMaxVoltageUv = 1800000;
constexpr int32_t kReferenceMinVoltageUv = 0;
constexpr chrono::SystemClock::duration kTimeout = 1ms;

constexpr int32_t kBipolarLimitsMax = 4096;
constexpr int32_t kBipolarLimitsMin = -4096;
constexpr int32_t kBipolarReferenceMaxVoltageUv = 1800000;
constexpr int32_t kBipolarReferenceMinVoltageUv = -1800000;

constexpr int32_t kCornerLimitsMax = std::numeric_limits<int32_t>::max();
constexpr int32_t kCornerLimitsMin = std::numeric_limits<int32_t>::min();
constexpr int32_t kCornerReferenceMaxVoltageUv =
    std::numeric_limits<int32_t>::max();
constexpr int32_t kCornerReferenceMinVoltageUv =
    std::numeric_limits<int32_t>::min();

constexpr int32_t kInvertedLimitsMax = std::numeric_limits<int32_t>::min();
constexpr int32_t kInvertedLimitsMin = std::numeric_limits<int32_t>::max();
constexpr int32_t kInvertedReferenceMaxVoltageUv =
    std::numeric_limits<int32_t>::min();
constexpr int32_t kInvertedReferenceMinVoltageUv =
    std::numeric_limits<int32_t>::max();

// Fake voltage input that's used for testing.
class TestMicrovoltInput : public MicrovoltInput {
 public:
  constexpr explicit TestMicrovoltInput(AnalogInput::Limits limits,
                                        MicrovoltInput::References reference)
      : sample_(0), limits_(limits), reference_(reference) {}

  void SetSampleValue(int32_t sample) { sample_ = sample; }

 private:
  Result<int32_t> TryReadUntil(chrono::SystemClock::time_point) override {
    return sample_;
  }

  Limits GetLimits() const override { return limits_; }
  References GetReferences() const override { return reference_; }

  uint32_t sample_;
  const Limits limits_;
  const References reference_;
};

TEST(MicrovoltInputTest, Construction) {
  TestMicrovoltInput voltage_input =
      TestMicrovoltInput({.min = kLimitsMin, .max = kLimitsMax},
                         {.max_voltage_uv = kReferenceMaxVoltageUv,
                          .min_voltage_uv = kReferenceMinVoltageUv});
}

TEST(MicrovoltInputTest, ReadMicrovoltsWithSampleAtMin) {
  TestMicrovoltInput voltage_input =
      TestMicrovoltInput({.min = kLimitsMin, .max = kLimitsMax},
                         {.max_voltage_uv = kReferenceMaxVoltageUv,
                          .min_voltage_uv = kReferenceMinVoltageUv});
  voltage_input.SetSampleValue(kLimitsMin);

  Result<int32_t> result = voltage_input.TryReadMicrovoltsFor(kTimeout);
  ASSERT_TRUE(result.status().ok());

  EXPECT_EQ(result.value(), 0);
}

TEST(MicrovoltInputTest, ReadMicrovoltsWithSampleAtMax) {
  TestMicrovoltInput voltage_input =
      TestMicrovoltInput({.min = kLimitsMin, .max = kLimitsMax},
                         {.max_voltage_uv = kReferenceMaxVoltageUv,
                          .min_voltage_uv = kReferenceMinVoltageUv});
  voltage_input.SetSampleValue(kLimitsMax);

  Result<int32_t> result = voltage_input.TryReadMicrovoltsFor(kTimeout);
  ASSERT_TRUE(result.status().ok());

  EXPECT_EQ(result.value(), kReferenceMaxVoltageUv);
}

TEST(MicrovoltInputTest, ReadMicrovoltsWithSampleAtHalf) {
  TestMicrovoltInput voltage_input =
      TestMicrovoltInput({.min = kLimitsMin, .max = kLimitsMax},
                         {.max_voltage_uv = kReferenceMaxVoltageUv,
                          .min_voltage_uv = kReferenceMinVoltageUv});
  voltage_input.SetSampleValue(kLimitsMax / 2);

  Result<int32_t> result = voltage_input.TryReadMicrovoltsFor(kTimeout);
  ASSERT_TRUE(result.status().ok());

  EXPECT_EQ(result.value(), kReferenceMaxVoltageUv / 2);
}

TEST(MicrovoltInputTest, ReadMicrovoltsWithBipolarAdcAtZero) {
  TestMicrovoltInput voltage_input =
      TestMicrovoltInput({.min = kBipolarLimitsMin, .max = kBipolarLimitsMax},
                         {.max_voltage_uv = kBipolarReferenceMaxVoltageUv,
                          .min_voltage_uv = kBipolarReferenceMinVoltageUv});
  voltage_input.SetSampleValue(0);

  Result<int32_t> result = voltage_input.TryReadMicrovoltsFor(kTimeout);
  ASSERT_TRUE(result.status().ok());

  EXPECT_EQ(result.value(), 0);
}

TEST(MicrovoltInputTest, ReadMicrovoltsWithBipolarAdcAtMin) {
  TestMicrovoltInput voltage_input =
      TestMicrovoltInput({.min = kBipolarLimitsMin, .max = kBipolarLimitsMax},
                         {.max_voltage_uv = kBipolarReferenceMaxVoltageUv,
                          .min_voltage_uv = kBipolarReferenceMinVoltageUv});
  voltage_input.SetSampleValue(kBipolarLimitsMin);

  Result<int32_t> result = voltage_input.TryReadMicrovoltsFor(kTimeout);
  ASSERT_TRUE(result.status().ok());

  EXPECT_EQ(result.value(), kBipolarReferenceMinVoltageUv);
}

TEST(MicrovoltInputTest, ReadMicrovoltsWithBipolarAdcAtMax) {
  TestMicrovoltInput voltage_input =
      TestMicrovoltInput({.min = kBipolarLimitsMin, .max = kBipolarLimitsMax},
                         {.max_voltage_uv = kBipolarReferenceMaxVoltageUv,
                          .min_voltage_uv = kBipolarReferenceMinVoltageUv});
  voltage_input.SetSampleValue(kBipolarLimitsMax);

  Result<int32_t> result = voltage_input.TryReadMicrovoltsFor(kTimeout);
  ASSERT_TRUE(result.status().ok());

  EXPECT_EQ(result.value(), kBipolarReferenceMaxVoltageUv);
}

TEST(MicrovoltInputTest, ReadMicrovoltsWithBipolarAdcAtUpperHalf) {
  TestMicrovoltInput voltage_input =
      TestMicrovoltInput({.min = kBipolarLimitsMin, .max = kBipolarLimitsMax},
                         {.max_voltage_uv = kBipolarReferenceMaxVoltageUv,
                          .min_voltage_uv = kBipolarReferenceMinVoltageUv});
  voltage_input.SetSampleValue(kBipolarLimitsMax / 2);

  Result<int32_t> result = voltage_input.TryReadMicrovoltsFor(kTimeout);
  ASSERT_TRUE(result.status().ok());

  EXPECT_EQ(result.value(), kBipolarReferenceMaxVoltageUv / 2);
}

TEST(MicrovoltInputTest, ReadMicrovoltsWithBipolarAdcAtLowerHalf) {
  TestMicrovoltInput voltage_input =
      TestMicrovoltInput({.min = kBipolarLimitsMin, .max = kBipolarLimitsMax},
                         {.max_voltage_uv = kBipolarReferenceMaxVoltageUv,
                          .min_voltage_uv = kBipolarReferenceMinVoltageUv});
  voltage_input.SetSampleValue(kBipolarLimitsMin / 2);

  Result<int32_t> result = voltage_input.TryReadMicrovoltsFor(kTimeout);
  ASSERT_TRUE(result.status().ok());

  EXPECT_EQ(result.value(), kBipolarReferenceMinVoltageUv / 2);
}

TEST(MicrovoltInputTest, ReadMicrovoltsWithBipolarReferenceAtZero) {
  TestMicrovoltInput voltage_input =
      TestMicrovoltInput({.min = kLimitsMin, .max = kLimitsMax},
                         {.max_voltage_uv = kBipolarReferenceMaxVoltageUv,
                          .min_voltage_uv = kBipolarReferenceMinVoltageUv});
  voltage_input.SetSampleValue(0);

  Result<int32_t> result = voltage_input.TryReadMicrovoltsFor(kTimeout);
  ASSERT_TRUE(result.status().ok());

  EXPECT_EQ(result.value(), kBipolarReferenceMinVoltageUv);
}

TEST(MicrovoltInputTest, ReadMicrovoltsWithBipolarReferenceAtMin) {
  TestMicrovoltInput voltage_input =
      TestMicrovoltInput({.min = kLimitsMin, .max = kLimitsMax},
                         {.max_voltage_uv = kBipolarReferenceMaxVoltageUv,
                          .min_voltage_uv = kBipolarReferenceMinVoltageUv});
  voltage_input.SetSampleValue(kLimitsMin);

  Result<int32_t> result = voltage_input.TryReadMicrovoltsFor(kTimeout);
  ASSERT_TRUE(result.status().ok());

  EXPECT_EQ(result.value(), kBipolarReferenceMinVoltageUv);
}

TEST(MicrovoltInputTest, ReadMicrovoltsWithBipolarReferenceAtMax) {
  TestMicrovoltInput voltage_input =
      TestMicrovoltInput({.min = kLimitsMin, .max = kLimitsMax},
                         {.max_voltage_uv = kBipolarReferenceMaxVoltageUv,
                          .min_voltage_uv = kBipolarReferenceMinVoltageUv});
  voltage_input.SetSampleValue(kLimitsMax);

  Result<int32_t> result = voltage_input.TryReadMicrovoltsFor(kTimeout);
  ASSERT_TRUE(result.status().ok());

  EXPECT_EQ(result.value(), kBipolarReferenceMaxVoltageUv);
}

TEST(MicrovoltInputTest, ReadMicrovoltsWithBipolarReferenceAtHalf) {
  TestMicrovoltInput voltage_input =
      TestMicrovoltInput({.min = kLimitsMin, .max = kLimitsMax},
                         {.max_voltage_uv = kBipolarReferenceMaxVoltageUv,
                          .min_voltage_uv = kBipolarReferenceMinVoltageUv});
  voltage_input.SetSampleValue(kLimitsMax / 2);

  Result<int32_t> result = voltage_input.TryReadMicrovoltsFor(kTimeout);
  ASSERT_TRUE(result.status().ok());

  EXPECT_EQ(result.value(), 0);
}

TEST(MicrovoltInputTest, ReadMicrovoltsWithSampleAtMinCornerCase) {
  TestMicrovoltInput voltage_input =
      TestMicrovoltInput({.min = kCornerLimitsMin, .max = kCornerLimitsMax},
                         {.max_voltage_uv = kCornerReferenceMaxVoltageUv,
                          .min_voltage_uv = kCornerReferenceMinVoltageUv});
  voltage_input.SetSampleValue(kCornerLimitsMin);

  Result<int32_t> result = voltage_input.TryReadMicrovoltsFor(kTimeout);
  ASSERT_EQ(result.status(), pw::Status::Internal());
}

TEST(MicrovoltInputTest, ReadMicrovoltsWithSampleAtMaxCornerCase) {
  TestMicrovoltInput voltage_input =
      TestMicrovoltInput({.min = kCornerLimitsMin, .max = kCornerLimitsMax},
                         {.max_voltage_uv = kCornerReferenceMaxVoltageUv,
                          .min_voltage_uv = kCornerReferenceMinVoltageUv});
  voltage_input.SetSampleValue(kCornerLimitsMax);

  Result<int32_t> result = voltage_input.TryReadMicrovoltsFor(kTimeout);
  ASSERT_EQ(result.status(), pw::Status::Internal());
}

TEST(MicrovoltInputTest, ReadMicrovoltsWithInvertedReferenceAtMax) {
  TestMicrovoltInput voltage_input =
      TestMicrovoltInput({.min = kInvertedLimitsMin, .max = kInvertedLimitsMax},
                         {.max_voltage_uv = kInvertedReferenceMaxVoltageUv,
                          .min_voltage_uv = kInvertedReferenceMinVoltageUv});
  voltage_input.SetSampleValue(kInvertedLimitsMax);

  Result<int32_t> result = voltage_input.TryReadMicrovoltsFor(kTimeout);
  ASSERT_EQ(result.status(), pw::Status::Internal());
}

TEST(MicrovoltInputTest, ReadMicrovoltsWithInvertedReferenceAtMin) {
  TestMicrovoltInput voltage_input =
      TestMicrovoltInput({.min = kInvertedLimitsMin, .max = kInvertedLimitsMax},
                         {.max_voltage_uv = kInvertedReferenceMaxVoltageUv,
                          .min_voltage_uv = kInvertedReferenceMinVoltageUv});
  voltage_input.SetSampleValue(kInvertedLimitsMin);

  Result<int32_t> result = voltage_input.TryReadMicrovoltsFor(kTimeout);
  ASSERT_EQ(result.status(), pw::Status::Internal());
}

}  // namespace
}  // namespace analog
}  // namespace pw
