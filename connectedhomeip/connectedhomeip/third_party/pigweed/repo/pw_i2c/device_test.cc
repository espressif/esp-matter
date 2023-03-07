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

#include "pw_i2c/device.h"

#include <chrono>

#include "gtest/gtest.h"
#include "pw_bytes/byte_builder.h"
#include "pw_containers/algorithm.h"
#include "pw_i2c/initiator_mock.h"

using namespace std::literals::chrono_literals;

namespace pw::i2c {
namespace {

TEST(Device, WriteReadForOk) {
  constexpr Address kTestDeviceAddress = Address::SevenBit<0x3F>();

  constexpr auto kExpectWrite1 = bytes::Array<1, 2, 3>();
  constexpr auto kExpectRead1 = bytes::Array<1, 2>();

  auto expected_transactions = MakeExpectedTransactionArray({Transaction(
      OkStatus(), kTestDeviceAddress, kExpectWrite1, kExpectRead1, 2ms)});

  MockInitiator initiator(expected_transactions);

  Device device = Device(initiator, kTestDeviceAddress);

  std::array<std::byte, kExpectRead1.size()> read1;
  EXPECT_EQ(device.WriteReadFor(kExpectWrite1, read1, 2ms), OkStatus());
  EXPECT_TRUE(pw::containers::Equal(read1, kExpectRead1));
  EXPECT_EQ(initiator.Finalize(), OkStatus());
}

TEST(Device, WriteForOk) {
  constexpr Address kTestDeviceAddress = Address::SevenBit<0x3F>();

  constexpr auto kExpectWrite1 = bytes::Array<1, 2, 3>();

  auto expected_transactions = MakeExpectedTransactionArray(
      {WriteTransaction(OkStatus(), kTestDeviceAddress, kExpectWrite1, 2ms)});

  MockInitiator initiator(expected_transactions);
  Device device = Device(initiator, kTestDeviceAddress);

  EXPECT_EQ(device.WriteFor(kExpectWrite1, 2ms), OkStatus());
  EXPECT_EQ(initiator.Finalize(), OkStatus());
}

TEST(Device, ReadForOk) {
  constexpr Address kTestDeviceAddress = Address::SevenBit<0x3F>();

  constexpr auto kExpectRead1 = bytes::Array<1, 2, 3>();

  auto expected_transactions = MakeExpectedTransactionArray(
      {ReadTransaction(OkStatus(), kTestDeviceAddress, kExpectRead1, 2ms)});

  MockInitiator initiator(expected_transactions);
  Device device = Device(initiator, kTestDeviceAddress);

  std::array<std::byte, kExpectRead1.size()> read1;
  EXPECT_EQ(device.ReadFor(read1, 2ms), OkStatus());
  EXPECT_EQ(initiator.Finalize(), OkStatus());
}

TEST(Device, ProbeDeviceForOk) {
  constexpr Address kTestDeviceAddress = Address::SevenBit<0x3F>();

  auto expected_transactions = MakeExpectedTransactionArray(
      {ProbeTransaction(OkStatus(), kTestDeviceAddress, 2ms)});

  MockInitiator initiator(expected_transactions);
  Device device = Device(initiator, kTestDeviceAddress);

  EXPECT_EQ(initiator.ProbeDeviceFor(kTestDeviceAddress, 2ms), OkStatus());
  EXPECT_EQ(initiator.Finalize(), OkStatus());
}

}  // namespace
}  // namespace pw::i2c
