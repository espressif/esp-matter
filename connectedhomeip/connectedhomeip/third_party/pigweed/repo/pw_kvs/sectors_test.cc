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

#include "pw_kvs/internal/sectors.h"

#include "gtest/gtest.h"
#include "pw_kvs/fake_flash_memory.h"

namespace pw::kvs::internal {
namespace {

class SectorsTest : public ::testing::Test {
 protected:
  SectorsTest()
      : partition_(&flash_),
        sectors_(sector_descriptors_, partition_, nullptr) {
    EXPECT_EQ(0u, sectors_.size());
    sectors_.Reset();
  }

  FakeFlashMemoryBuffer<128, 16> flash_;
  FlashPartition partition_;
  Vector<SectorDescriptor, 32> sector_descriptors_;
  Sectors sectors_;
};

TEST_F(SectorsTest, Reset) {
  // Reset is done by the fixture.
  EXPECT_EQ(partition_.sector_count(), sectors_.size());
  EXPECT_EQ(32u, sectors_.max_size());
  EXPECT_EQ(sectors_.begin(), sectors_.last_new());
}

TEST_F(SectorsTest, LastNew) {
  sectors_.set_last_new_sector(130);
  EXPECT_EQ(128u, sectors_.BaseAddress(*sectors_.last_new()));
}

TEST_F(SectorsTest, AddressInSector) {
  SectorDescriptor& sector = sectors_.FromAddress(128);

  EXPECT_FALSE(sectors_.AddressInSector(sector, 127));
  for (size_t address = 128; address < 256; ++address) {
    EXPECT_TRUE(sectors_.AddressInSector(sector, address));
  }
  EXPECT_FALSE(sectors_.AddressInSector(sector, 256));
  EXPECT_FALSE(sectors_.AddressInSector(sector, 1025));
}

TEST_F(SectorsTest, BaseAddressAndFromAddress) {
  for (size_t address = 0; address < 128; ++address) {
    EXPECT_EQ(0u, sectors_.BaseAddress(sectors_.FromAddress(address)));
  }
  for (size_t address = 128; address < 256; ++address) {
    EXPECT_EQ(128u, sectors_.BaseAddress(sectors_.FromAddress(address)));
  }
  for (size_t address = 256; address < 384; ++address) {
    EXPECT_EQ(256u, sectors_.BaseAddress(sectors_.FromAddress(address)));
  }
}

TEST_F(SectorsTest, NextWritableAddress_EmptySector) {
  EXPECT_EQ(0u, sectors_.NextWritableAddress(*sectors_.begin()));
}

TEST_F(SectorsTest, NextWritableAddress_PartiallyWrittenSector) {
  sectors_.begin()->RemoveWritableBytes(123);
  EXPECT_EQ(123u, sectors_.NextWritableAddress(*sectors_.begin()));
}

// TODO(hepler): Add tests for FindSpace, FindSpaceDuringGarbageCollection, and
// FindSectorToGarbageCollect.

}  // namespace
}  // namespace pw::kvs::internal
