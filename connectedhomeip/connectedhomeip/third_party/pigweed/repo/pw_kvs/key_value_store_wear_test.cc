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

// Always use stats, these tests depend on it.
#define PW_KVS_RECORD_PARTITION_STATS 1

#include "gtest/gtest.h"
#include "pw_kvs/fake_flash_memory.h"
#include "pw_kvs/flash_memory.h"
#include "pw_kvs/flash_partition_with_stats.h"
#include "pw_kvs/key_value_store.h"
#include "pw_log/log.h"

namespace pw::kvs {
namespace {

// For KVS magic value always use a random 32 bit integer rather than a
// human readable 4 bytes. See pw_kvs/format.h for more information.
constexpr EntryFormat format{.magic = 0x1bce4ad5, .checksum = nullptr};

class WearTest : public ::testing::Test {
 protected:
  WearTest()
      : flash_(internal::Entry::kMinAlignmentBytes),
        partition_(&flash_, 0, flash_.sector_count()),
        kvs_(&partition_, format) {
    EXPECT_EQ(OkStatus(), kvs_.Init());
  }

  static constexpr size_t kSectors = 16;
  static constexpr size_t kMaxEntries = 256;
  static constexpr size_t kTestPartitionSectorSize = 512;

  FakeFlashMemoryBuffer<kTestPartitionSectorSize, kSectors> flash_;
  FlashPartitionWithStatsBuffer<kSectors> partition_;

  KeyValueStoreBuffer<kMaxEntries, kSectors> kvs_;
};

// Block of data to use for entry value. Sized to 470 so the total entry results
// in using most of the 512 byte sector.
uint8_t test_data[470] = {1, 2, 3, 4, 5, 6};

// Write a large key (i.e. only one entry fits in each sector) enough times to
// fill up the KVS multiple times, and ensure every sector was garbage collected
// multiple additional times.
TEST_F(WearTest, RepeatedLargeEntry) {
  // Initialize an empty KVS, erasing flash and all tracked sector erase counts.
  partition_.ResetCounters();

  // Add enough large entries to fill the entire KVS several times.
  for (size_t i = 0; i < kSectors * 10; ++i) {
    // modify the value to ensure a key-value different than was previously
    // written.
    test_data[0]++;

    EXPECT_TRUE(kvs_.Put("large_entry", span(test_data)).ok());
  }

  // Ensure every sector has been erased at several times due to garbage
  // collection.
  EXPECT_GE(partition_.min_erase_count(), 7u);
  EXPECT_LE(partition_.max_erase_count(), partition_.min_erase_count() + 1u);

  // Ignore error to allow test to pass on platforms where writing out the stats
  // is not possible.
  partition_.SaveStorageStats(kvs_, "WearTest RepeatedLargeEntry")
      .IgnoreError();
}

// Test a KVS with a number of entries, several sectors that are nearly full
// of stale (reclaimable) space, and not enough writable (free) space to add a
// redundant copy for any of the entries. Tests that the add redundancy step of
// repair is able to use garbage collection to free up space needed for the new
// copies.
TEST_F(WearTest, TwoPassFillWithLargeAndLarger) {
  partition_.ResetCounters();

  // Add a large entry that will only fit once per sector enough times to fill
  // the KVS with mostly stale data.
  for (size_t i = 0; i < kSectors; i++) {
    // modify the value to ensure a key-value different than was previously
    // written.
    test_data[0]++;

    EXPECT_EQ(
        OkStatus(),
        kvs_.Put("key", as_bytes(span(test_data, sizeof(test_data) - 70))));
  }

  // Add many copies of a differently sized entry that is larger than the
  // previous entry.
  for (size_t i = 0; i < kSectors * 200; i++) {
    // Modify the value to ensure a key-value different than was previously
    // written.
    test_data[0]++;

    PW_LOG_DEBUG("Add entry %zu\n", i);
    EXPECT_EQ(OkStatus(), kvs_.Put("big_key", test_data));
  }

  EXPECT_EQ(2u, kvs_.size());
  EXPECT_LT(partition_.max_erase_count(),
            2u * partition_.average_erase_count());
}

}  // namespace
}  // namespace pw::kvs
