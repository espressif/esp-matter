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
#include "pw_random/xor_shift.h"

#include <cinttypes>
#include <cstddef>
#include <cstdint>
#include <cstdio>

#include "gtest/gtest.h"

namespace pw::random {
namespace {

constexpr uint64_t seed1 = 5;
constexpr uint64_t result1[] = {
    0x423212e85fb37474u,
    0x96051f25a1aadc74u,
    0x8ac1f520f5595a79u,
    0x7587fe57095b7c11u,
};
constexpr int result1_count = sizeof(result1) / sizeof(result1[0]);

constexpr uint64_t seed2 = 0x21feabcd5fb37474u;
constexpr uint64_t result2[] = {
    0x568ea260a4f3e793u,
    0x5ea87d669ab04d36u,
    0x77a8675eec48ae8bu,
};
constexpr int result2_count = sizeof(result2) / sizeof(result2[0]);

TEST(XorShiftStarRng64, ValidateSeries1) {
  XorShiftStarRng64 rng(seed1);
  for (size_t i = 0; i < result1_count; ++i) {
    uint64_t val = 0;
    rng.GetInt(val);
    EXPECT_EQ(val, result1[i]);
  }
}

TEST(XorShiftStarRng64, ValidateSeries2) {
  XorShiftStarRng64 rng(seed2);
  for (size_t i = 0; i < result2_count; ++i) {
    uint64_t val = 0;
    rng.GetInt(val);
    EXPECT_EQ(val, result2[i]);
  }
}

TEST(XorShiftStarRng64, InjectEntropyBits) {
  XorShiftStarRng64 rng(seed1);
  uint64_t val = 0;
  rng.InjectEntropyBits(0x1, 1);
  rng.GetInt(val);
  EXPECT_NE(val, result1[0]);
}

TEST(XorShiftStarRng64, Inject32BitsEntropy) {
  XorShiftStarRng64 rng_1(seed1);
  uint64_t first_val = 0;
  rng_1.InjectEntropyBits(0x12345678, 32);
  rng_1.GetInt(first_val);
  EXPECT_NE(first_val, result1[0]);
}

// Ensure injecting the same entropy integer, but different bit counts causes
// the randomly generated number to differ.
TEST(XorShiftStarRng64, EntropyBitCount) {
  XorShiftStarRng64 rng_1(seed1);
  uint64_t first_val = 0;
  rng_1.InjectEntropyBits(0x1, 1);
  rng_1.GetInt(first_val);

  // Use the same starting seed.
  XorShiftStarRng64 rng_2(seed1);
  uint64_t second_val = 0;
  // Use a different number of entropy bits.
  rng_2.InjectEntropyBits(0x1, 2);
  rng_2.GetInt(second_val);

  EXPECT_NE(first_val, second_val);
}

// Ensure injecting the same integer bit-by-bit applies the same transformation
// as all in one call. This lets applications decide which is more convenient
// without worrying about algorithmic changes.
TEST(XorShiftStarRng64, IncrementalEntropy) {
  XorShiftStarRng64 rng_1(seed1);
  uint64_t first_val = 0;
  rng_1.InjectEntropyBits(0x6, 3);
  rng_1.GetInt(first_val);

  // Use the same starting seed.
  XorShiftStarRng64 rng_2(seed1);
  uint64_t second_val = 0;
  // Use a different number of injection calls. 6 = 0b110
  rng_2.InjectEntropyBits(0x1, 1);
  rng_2.InjectEntropyBits(0x1, 1);
  rng_2.InjectEntropyBits(0x0, 1);
  rng_2.GetInt(second_val);

  EXPECT_EQ(first_val, second_val);
}

TEST(XorShiftStarRng64, InjectEntropy) {
  XorShiftStarRng64 rng(seed1);
  uint64_t val = 0;
  constexpr std::array<const std::byte, 5> entropy{std::byte(0xaf),
                                                   std::byte(0x9b),
                                                   std::byte(0x33),
                                                   std::byte(0x17),
                                                   std::byte(0x02)};
  rng.InjectEntropy(entropy);
  rng.GetInt(val);
  EXPECT_NE(val, result1[0]);
}

}  // namespace
}  // namespace pw::random
