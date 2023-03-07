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

#include "pw_spi/initiator_mock.h"

#include <array>

#include "gtest/gtest.h"
#include "pw_bytes/array.h"
#include "pw_bytes/span.h"
#include "pw_containers/algorithm.h"
#include "pw_span/span.h"

namespace pw::spi {
namespace {

TEST(Transaction, Read) {
  constexpr auto kExpectRead1 = bytes::Array<1, 2, 3, 4, 5>();
  constexpr auto kExpectRead2 = bytes::Array<3, 4, 5>();

  auto expected_transactions = MakeExpectedTransactionArray(
      {MockReadTransaction(OkStatus(), kExpectRead1),
       MockReadTransaction(OkStatus(), kExpectRead2)});

  MockInitiator mocked_spi(expected_transactions);

  std::array<std::byte, kExpectRead1.size()> read1;
  EXPECT_EQ(mocked_spi.WriteRead(ConstByteSpan(), read1), OkStatus());
  EXPECT_TRUE(pw::containers::Equal(read1, kExpectRead1));

  std::array<std::byte, kExpectRead2.size()> read2;
  EXPECT_EQ(mocked_spi.WriteRead(ConstByteSpan(), read2), OkStatus());
  EXPECT_TRUE(pw::containers::Equal(read2, kExpectRead2));

  EXPECT_EQ(mocked_spi.Finalize(), OkStatus());
}

TEST(Transaction, Write) {
  constexpr auto kExpectWrite1 = bytes::Array<1, 2, 3, 4, 5>();
  constexpr auto kExpectWrite2 = bytes::Array<3, 4, 5>();

  auto expected_transactions = MakeExpectedTransactionArray(
      {MockWriteTransaction(OkStatus(), kExpectWrite1),
       MockWriteTransaction(OkStatus(), kExpectWrite2)});

  MockInitiator mocked_spi(expected_transactions);

  EXPECT_EQ(mocked_spi.WriteRead(kExpectWrite1, ByteSpan()), OkStatus());

  EXPECT_EQ(mocked_spi.WriteRead(kExpectWrite2, ByteSpan()), OkStatus());

  EXPECT_EQ(mocked_spi.Finalize(), OkStatus());
}

TEST(Transaction, WriteRead) {
  constexpr auto kExpectWrite1 = bytes::Array<1, 2, 3, 4, 5>();
  constexpr auto kExpectRead1 = bytes::Array<1, 2>();

  constexpr auto kExpectWrite2 = bytes::Array<3, 4, 5>();
  constexpr auto kExpectRead2 = bytes::Array<3, 4>();

  auto expected_transactions = MakeExpectedTransactionArray({
      MockTransaction(OkStatus(), kExpectWrite1, kExpectRead1),
      MockTransaction(OkStatus(), kExpectWrite2, kExpectRead2),
  });

  MockInitiator mocked_spi(expected_transactions);

  std::array<std::byte, kExpectRead1.size()> read1;
  EXPECT_EQ(mocked_spi.WriteRead(kExpectWrite1, read1), OkStatus());
  EXPECT_TRUE(pw::containers::Equal(read1, kExpectRead1));

  std::array<std::byte, kExpectRead2.size()> read2;
  EXPECT_EQ(mocked_spi.WriteRead(kExpectWrite2, read2), OkStatus());
  EXPECT_TRUE(pw::containers::Equal(read2, kExpectRead2));

  EXPECT_EQ(mocked_spi.Finalize(), OkStatus());
}

}  // namespace
}  // namespace pw::spi
