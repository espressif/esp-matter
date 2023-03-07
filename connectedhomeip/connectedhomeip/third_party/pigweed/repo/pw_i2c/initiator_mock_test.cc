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

#include "pw_i2c/initiator_mock.h"

#include <array>
#include <chrono>

#include "gtest/gtest.h"
#include "pw_bytes/array.h"
#include "pw_bytes/span.h"
#include "pw_containers/algorithm.h"
#include "pw_i2c/address.h"
#include "pw_span/span.h"

using namespace std::literals::chrono_literals;

namespace pw::i2c {
namespace {

TEST(Transaction, Read) {
  static constexpr Address kAddress1 = Address::SevenBit<0x01>();
  constexpr auto kExpectRead1 = bytes::Array<1, 2, 3, 4, 5>();

  static constexpr Address kAddress2 = Address::SevenBit<0x02>();
  constexpr auto kExpectRead2 = bytes::Array<3, 4, 5>();

  auto expected_transactions = MakeExpectedTransactionArray(
      {ReadTransaction(OkStatus(), kAddress1, kExpectRead1, 2ms),
       ReadTransaction(OkStatus(), kAddress2, kExpectRead2, 2ms)});

  MockInitiator mocked_i2c(expected_transactions);

  std::array<std::byte, kExpectRead1.size()> read1;
  EXPECT_EQ(mocked_i2c.ReadFor(kAddress1, read1, 2ms), OkStatus());
  EXPECT_TRUE(pw::containers::Equal(read1, kExpectRead1));

  std::array<std::byte, kExpectRead2.size()> read2;
  EXPECT_EQ(mocked_i2c.ReadFor(kAddress2, read2, 2ms), OkStatus());
  EXPECT_TRUE(pw::containers::Equal(read2, kExpectRead2));

  EXPECT_EQ(mocked_i2c.Finalize(), OkStatus());
}

TEST(Transaction, Write) {
  static constexpr Address kAddress1 = Address::SevenBit<0x01>();
  constexpr auto kExpectWrite1 = bytes::Array<1, 2, 3, 4, 5>();

  static constexpr Address kAddress2 = Address::SevenBit<0x02>();
  constexpr auto kExpectWrite2 = bytes::Array<3, 4, 5>();

  auto expected_transactions = MakeExpectedTransactionArray(
      {WriteTransaction(OkStatus(), kAddress1, kExpectWrite1, 2ms),
       WriteTransaction(OkStatus(), kAddress2, kExpectWrite2, 2ms)});

  MockInitiator mocked_i2c(expected_transactions);

  EXPECT_EQ(mocked_i2c.WriteFor(kAddress1, kExpectWrite1, 2ms), OkStatus());

  EXPECT_EQ(mocked_i2c.WriteFor(kAddress2, kExpectWrite2, 2ms), OkStatus());

  EXPECT_EQ(mocked_i2c.Finalize(), OkStatus());
}

TEST(Transaction, WriteRead) {
  static constexpr Address kAddress1 = Address::SevenBit<0x01>();
  constexpr auto kExpectWrite1 = bytes::Array<1, 2, 3, 4, 5>();
  constexpr auto kExpectRead1 = bytes::Array<1, 2>();

  static constexpr Address kAddress2 = Address::SevenBit<0x02>();
  constexpr auto kExpectWrite2 = bytes::Array<3, 4, 5>();
  constexpr const auto kExpectRead2 = bytes::Array<3, 4>();

  auto expected_transactions = MakeExpectedTransactionArray({
      Transaction(OkStatus(), kAddress1, kExpectWrite1, kExpectRead1, 2ms),
      Transaction(OkStatus(), kAddress2, kExpectWrite2, kExpectRead2, 2ms),
  });

  MockInitiator mocked_i2c(expected_transactions);

  std::array<std::byte, kExpectRead1.size()> read1;
  EXPECT_EQ(mocked_i2c.WriteReadFor(kAddress1, kExpectWrite1, read1, 2ms),
            OkStatus());
  EXPECT_TRUE(pw::containers::Equal(read1, kExpectRead1));

  std::array<std::byte, kExpectRead1.size()> read2;
  EXPECT_EQ(mocked_i2c.WriteReadFor(kAddress2, kExpectWrite2, read2, 2ms),
            OkStatus());
  EXPECT_TRUE(pw::containers::Equal(read2, kExpectRead2));

  EXPECT_EQ(mocked_i2c.Finalize(), OkStatus());
}

TEST(Transaction, Probe) {
  static constexpr Address kAddress1 = Address::SevenBit<0x01>();

  auto expected_transactions = MakeExpectedTransactionArray({
      ProbeTransaction(OkStatus(), kAddress1, 2ms),
  });

  MockInitiator mock_initiator(expected_transactions);

  EXPECT_EQ(mock_initiator.ProbeDeviceFor(kAddress1, 2ms), OkStatus());
  EXPECT_EQ(mock_initiator.Finalize(), OkStatus());
}

}  // namespace
}  // namespace pw::i2c
