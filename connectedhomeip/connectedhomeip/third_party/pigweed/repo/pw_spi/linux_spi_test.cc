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

#include "pw_spi/linux_spi.h"

#include <array>

#include "gtest/gtest.h"
#include "pw_spi/chip_selector.h"
#include "pw_spi/device.h"
#include "pw_spi/initiator.h"
#include "pw_status/status.h"
#include "pw_sync/borrow.h"
#include "pw_sync/mutex.h"

namespace pw::spi {
namespace {

const pw::spi::Config kConfig = {.polarity = ClockPolarity::kActiveHigh,
                                 .phase = ClockPhase::kFallingEdge,
                                 .bits_per_word = BitsPerWord(8),
                                 .bit_order = BitOrder::kMsbFirst};

class LinuxSpi : public ::testing::Test {
 public:
  LinuxSpi()
      : initiator_(LinuxInitiator("/dev/spidev0.0", 1000000)),
        chip_selector_(),
        initiator_lock_(),
        borrowable_initiator_(initiator_, initiator_lock_),
        device_(borrowable_initiator_, kConfig, chip_selector_) {}

  Device& device() { return device_; }

 private:
  LinuxInitiator initiator_;
  LinuxChipSelector chip_selector_;
  sync::VirtualMutex initiator_lock_;
  sync::Borrowable<Initiator> borrowable_initiator_;
  [[maybe_unused]] Device device_;
};

TEST_F(LinuxSpi, HalfDuplexTransaction_Succeeds) {
  // arrange
  auto transaction =
      device().StartTransaction(ChipSelectBehavior::kPerWriteRead);

  // act
  std::array write_data{std::byte(1), std::byte(2), std::byte(3), std::byte(4)};
  auto write_status = transaction.Write(ConstByteSpan(write_data));

  std::array read_data{std::byte(1), std::byte(2), std::byte(3), std::byte(4)};
  auto read_status = transaction.Read(read_data);

  // assert
  EXPECT_TRUE(write_status.ok());
  EXPECT_TRUE(read_status.ok());
}

TEST_F(LinuxSpi, FullDuplexTransaction_Succeeds) {
  // arrange
  auto transaction =
      device().StartTransaction(ChipSelectBehavior::kPerWriteRead);

  // act
  std::array write_data{std::byte(1), std::byte(2), std::byte(3), std::byte(4)};
  std::array read_data{std::byte(0), std::byte(0), std::byte(0), std::byte(0)};
  auto wr_status = transaction.WriteRead(ConstByteSpan(write_data), read_data);

  // assert
  EXPECT_TRUE(wr_status.ok());
}

}  // namespace
}  // namespace pw::spi
