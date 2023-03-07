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
#include "pw_bluetooth/uuid.h"

#include <array>
#include <cstdint>

#include "gtest/gtest.h"
#include "pw_span/span.h"

namespace pw::bluetooth {
namespace {

// 16-bit and 32-bit short form UUID.
constexpr Uuid kShortUuid16{0x1234};
constexpr Uuid kShortUuid32{0xabcd1234};

// UUID initialized from a span.
constexpr Uuid kLongUuidArray{
    pw::span<const uint8_t, 16>(std::array<uint8_t, 16>{
        1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16})};

// UUID initialized from a string. This is the same as kLongUuidArray but in
// string form.
constexpr Uuid kLongUuidString{"100f0e0d-0c0b-0a09-0807-060504030201"};

// UUID initialized combining another UUID with a 32-bit value.
constexpr Uuid kLongUuidComposed1 = Uuid(0xabcd, kLongUuidString);
constexpr Uuid kLongUuidComposed2 = Uuid(0x1234abcd, kLongUuidString);

// Make sure that all these values are actually constexpr.
static_assert(kShortUuid16 != kShortUuid32, "constexpr check");
static_assert(kLongUuidArray == kLongUuidString, "constexpr check");
static_assert(kLongUuidComposed2.ToString() ==
                  "1234abcd-0c0b-0a09-0807-060504030201",
              "constexpr check");

TEST(UuidTest, ConstructorTest) {
  // Compare 16-bit with 128-bit.
  EXPECT_EQ(kShortUuid16, Uuid("00001234-0000-1000-8000-00805f9b34fb"));
  EXPECT_EQ(kShortUuid32, Uuid("abcd1234-0000-1000-8000-00805f9b34fb"));

  EXPECT_EQ(kShortUuid16.ToString(), "00001234-0000-1000-8000-00805f9b34fb");
  auto short_span = kShortUuid16.As16BitSpan();
  EXPECT_EQ(2u, short_span.size());
  EXPECT_EQ(short_span[0], 0x34);  // UUIDs are encoded little endian.
  EXPECT_EQ(short_span[1], 0x12);

  EXPECT_EQ(kLongUuidArray.ToString(), "100f0e0d-0c0b-0a09-0807-060504030201");
  auto long_span = kLongUuidArray.As128BitSpan();
  EXPECT_EQ(16u, long_span.size());
  EXPECT_EQ(long_span[12], 0x0d);
  EXPECT_EQ(long_span[13], 0x0e);

  // These two are the same UUID initialized in different ways.
  EXPECT_EQ(kLongUuidArray, kLongUuidString);

  // Composed UUID always set the 32-bits in the first groups, regardless of
  // whether we pass a 16-bit or 32-bit value, thus the first 0 chars in this
  // string are 0.
  static_assert(
      kLongUuidComposed1.ToString() == "0000abcd-0c0b-0a09-0807-060504030201",
      "constexpr check");
  static_assert(
      kLongUuidComposed2.ToString() == "1234abcd-0c0b-0a09-0807-060504030201",
      "constexpr check");

  // Check that the standard Bluetooth Base is correct.
  static_assert(Uuid::BluetoothBase().ToString() ==
                    "00000000-0000-1000-8000-00805f9b34fb",
                "constexpr check");
}

TEST(UuidTest, CombineShortUuidTest) {
  // The short 16-bit value can be represented as 16 or 32 bit, but the 32-bit
  // one can only be represented as 32-bits.
  EXPECT_TRUE(kShortUuid16.Is16BitUuid());
  EXPECT_TRUE(kShortUuid16.Is32BitUuid());
  EXPECT_FALSE(kShortUuid32.Is16BitUuid());
  EXPECT_TRUE(kShortUuid32.Is32BitUuid());

  // The composed UUID is not a standard one, but matches the base it was used
  // to construct it.
  EXPECT_FALSE(kLongUuidComposed1.Is16BitUuid());
  EXPECT_FALSE(kLongUuidComposed1.Is32BitUuid());

  EXPECT_TRUE(kLongUuidComposed1.Same96BitBase(kLongUuidString));
  EXPECT_TRUE(kLongUuidComposed2.Same96BitBase(kLongUuidString));

  // Composing always uses 32-bit values (setting the high bits as 0s) so they
  // don't match the base, unless it is zero extended.
  EXPECT_FALSE(kLongUuidComposed1.Same112BitBase(kLongUuidString));
  EXPECT_FALSE(kLongUuidComposed2.Same112BitBase(kLongUuidString));

  EXPECT_TRUE(kLongUuidComposed1.Same112BitBase(Uuid(0, kLongUuidString)));
}

}  // namespace
}  // namespace pw::bluetooth
