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
#include "pw_bluetooth/address.h"

#include <array>
#include <cstdint>

#include "gtest/gtest.h"
#include "pw_span/span.h"

namespace pw::bluetooth {
namespace {

constexpr Address kAddressString{"12:34:56:78:90:ab"};
constexpr Address kAddressStringFromArray{pw::span<const uint8_t, 6>{
    std::array<const uint8_t, 6>{0xab, 0x90, 0x78, 0x56, 0x34, 0x12}}};

constexpr Address kAddressArray{pw::span<const uint8_t, 6>{
    std::array<const uint8_t, 6>{0x1a, 0x2b, 0x3c, 0x4d, 0x5e, 0x6f}}};

// Make sure these are actually constexpr.
static_assert(kAddressString != kAddressArray, "constexpr check");
static_assert(kAddressString == kAddressStringFromArray, "constexpr check");
static_assert(kAddressArray.AsSpan().size() == 6, "constexpr check");

TEST(AddressTest, ConstructorTest) {
  static_assert(kAddressString.ToString() == "12:34:56:78:90:ab",
                "constexpr check");
  static_assert(kAddressArray.ToString() == "6f:5e:4d:3c:2b:1a",
                "constexpr check");
  static_assert(kAddressArray == Address("6f:5e:4d:3c:2b:1a"),
                "constexpr check");

  static_assert(kAddressString == kAddressStringFromArray, "constexpr check");
  static_assert(kAddressString != kAddressArray, "constexpr check");

  auto addr_span = kAddressString.AsSpan();
  EXPECT_EQ(addr_span.size(), 6u);
  EXPECT_EQ(addr_span.data()[0], 0xabu);
  EXPECT_EQ(addr_span.data()[1], 0x90u);
}

}  // namespace
}  // namespace pw::bluetooth
