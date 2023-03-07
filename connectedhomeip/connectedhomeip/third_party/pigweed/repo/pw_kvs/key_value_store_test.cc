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

#define DUMP_KVS_STATE_TO_FILE 0
#define USE_MEMORY_BUFFER 1

#include "pw_kvs/key_value_store.h"

#include <array>
#include <cstdio>
#include <cstring>

#include "pw_span/span.h"

#if DUMP_KVS_STATE_TO_FILE
#include <vector>
#endif  // DUMP_KVS_STATE_TO_FILE

#include "gtest/gtest.h"
#include "pw_assert/check.h"
#include "pw_bytes/array.h"
#include "pw_checksum/crc16_ccitt.h"
#include "pw_kvs/crc16_checksum.h"
#include "pw_kvs/fake_flash_memory.h"
#include "pw_kvs/flash_memory.h"
#include "pw_kvs/internal/entry.h"
#include "pw_kvs_private/config.h"
#include "pw_log/log.h"
#include "pw_log/shorter.h"
#include "pw_status/status.h"
#include "pw_string/string_builder.h"

namespace pw::kvs {
namespace {

using internal::EntryHeader;

constexpr size_t kMaxEntries = 256;
constexpr size_t kMaxUsableSectors = 256;

// This is a self contained flash unit with both memory and a single partition.
template <uint32_t kSectorSizeBytes, uint16_t kSectorCount>
struct FlashWithPartitionFake {
  // Default to 16 byte alignment, which is common in practice.
  FlashWithPartitionFake() : FlashWithPartitionFake(16) {}
  FlashWithPartitionFake(size_t alignment_bytes)
      : memory(alignment_bytes), partition(&memory, 0, memory.sector_count()) {}

  FakeFlashMemoryBuffer<kSectorSizeBytes, kSectorCount> memory;
  FlashPartition partition;

 public:
#if DUMP_KVS_STATE_TO_FILE
  Status Dump(const char* filename) {
    std::FILE* out_file = std::fopen(filename, "w+");
    if (out_file == nullptr) {
      PW_LOG_ERROR("Failed to dump to %s", filename);
      return Status::DataLoss();
    }
    std::vector<std::byte> out_vec(memory.size_bytes());
    Status status =
        memory.Read(0, span<std::byte>(out_vec.data(), out_vec.size()));
    if (status != OkStatus()) {
      fclose(out_file);
      return status;
    }

    size_t written =
        std::fwrite(out_vec.data(), 1, memory.size_bytes(), out_file);
    if (written != memory.size_bytes()) {
      PW_LOG_ERROR("Failed to dump to %s, written=%u",
                   filename,
                   static_cast<unsigned>(written));
      status = Status::DataLoss();
    } else {
      PW_LOG_INFO("Dumped to %s", filename);
      status = OkStatus();
    }

    fclose(out_file);
    return status;
  }
#else
  Status Dump(const char*) { return OkStatus(); }
#endif  // DUMP_KVS_STATE_TO_FILE
};

typedef FlashWithPartitionFake<4 * 128 /*sector size*/, 6 /*sectors*/> Flash;

FakeFlashMemoryBuffer<1024, 60> large_test_flash(8);
FlashPartition large_test_partition(&large_test_flash,
                                    0,
                                    large_test_flash.sector_count());

constexpr std::array<const char*, 3> keys{"TestKey1", "Key2", "TestKey3"};

ChecksumCrc16 checksum;
// For KVS magic value always use a random 32 bit integer rather than a
// human readable 4 bytes. See pw_kvs/format.h for more information.
constexpr EntryFormat default_format{.magic = 0xa6cb3c16,
                                     .checksum = &checksum};

}  // namespace

TEST(InitCheck, TooFewSectors) {
  // Use test flash with 1 x 4k sectors, 16 byte alignment
  FakeFlashMemoryBuffer<4 * 1024, 1> test_flash(16);
  FlashPartition test_partition(&test_flash, 0, test_flash.sector_count());

  // For KVS magic value always use a random 32 bit integer rather than a
  // human readable 4 bytes. See pw_kvs/format.h for more information.
  constexpr EntryFormat format{.magic = 0x89bb14d2, .checksum = nullptr};
  KeyValueStoreBuffer<kMaxEntries, kMaxUsableSectors> kvs(&test_partition,
                                                          format);

  EXPECT_EQ(kvs.Init(), Status::FailedPrecondition());
}

TEST(InitCheck, ZeroSectors) {
  // Use test flash with 1 x 4k sectors, 16 byte alignment
  FakeFlashMemoryBuffer<4 * 1024, 1> test_flash(16);

  // Set FlashPartition to have 0 sectors.
  FlashPartition test_partition(&test_flash, 0, 0);

  // For KVS magic value always use a random 32 bit integer rather than a
  // human readable 4 bytes. See pw_kvs/format.h for more information.
  constexpr EntryFormat format{.magic = 0xd1da57c1, .checksum = nullptr};
  KeyValueStoreBuffer<kMaxEntries, kMaxUsableSectors> kvs(&test_partition,
                                                          format);

  EXPECT_EQ(kvs.Init(), Status::FailedPrecondition());
}

TEST(InitCheck, TooManySectors) {
  // Use test flash with 1 x 4k sectors, 16 byte alignment
  FakeFlashMemoryBuffer<4 * 1024, 5> test_flash(16);

  // Set FlashPartition to have 0 sectors.
  FlashPartition test_partition(&test_flash, 0, test_flash.sector_count());

  // For KVS magic value always use a random 32 bit integer rather than a
  // human readable 4 bytes. See pw_kvs/format.h for more information.
  constexpr EntryFormat format{.magic = 0x610f6d17, .checksum = nullptr};
  KeyValueStoreBuffer<kMaxEntries, 2> kvs(&test_partition, format);

  EXPECT_EQ(kvs.Init(), Status::FailedPrecondition());
}

#define ASSERT_OK(expr) ASSERT_EQ(OkStatus(), expr)
#define EXPECT_OK(expr) EXPECT_EQ(OkStatus(), expr)

TEST(InMemoryKvs, WriteOneKeyMultipleTimes) {
  // Create and erase the fake flash. It will persist across reloads.
  Flash flash;
  ASSERT_OK(flash.partition.Erase());

  int num_reloads = 2;
  for (int reload = 0; reload < num_reloads; ++reload) {
    DBG("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    DBG("xxx                                      xxxx");
    DBG("xxx               Reload %2d              xxxx", reload);
    DBG("xxx                                      xxxx");
    DBG("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

    // Create and initialize the KVS. For KVS magic value always use a random 32
    // bit integer rather than a human readable 4 bytes. See pw_kvs/format.h for
    // more information.
    constexpr EntryFormat format{.magic = 0x83a9257, .checksum = nullptr};
    KeyValueStoreBuffer<kMaxEntries, kMaxUsableSectors> kvs(&flash.partition,
                                                            format);
    ASSERT_OK(kvs.Init());

    // Write the same entry many times.
    const char* key = "abcd";
    const size_t num_writes = 99;
    uint32_t written_value;
    EXPECT_EQ(kvs.size(), (reload == 0) ? 0 : 1u);
    for (uint32_t i = 0; i < num_writes; ++i) {
      DBG("PUT #%zu for key %s with value %zu", size_t(i), key, size_t(i));

      written_value = i + 0xfc;  // Prevent accidental pass with zero.
      EXPECT_OK(kvs.Put(key, written_value));
      EXPECT_EQ(kvs.size(), 1u);
    }

    // Verify that we can read the value back.
    DBG("GET final value for key: %s", key);
    uint32_t actual_value;
    EXPECT_OK(kvs.Get(key, &actual_value));
    EXPECT_EQ(actual_value, written_value);

    char fname_buf[64] = {'\0'};
    snprintf(&fname_buf[0],
             sizeof(fname_buf),
             "WriteOneKeyMultipleTimes_%d.bin",
             reload);
    ASSERT_EQ(OkStatus(), flash.Dump(fname_buf));
  }
}

TEST(InMemoryKvs, WritingMultipleKeysIncreasesSize) {
  // Create and erase the fake flash.
  Flash flash;
  ASSERT_OK(flash.partition.Erase());

  // Create and initialize the KVS. For KVS magic value always use a random 32
  // bit integer rather than a human readable 4 bytes. See pw_kvs/format.h for
  // more information.
  constexpr EntryFormat format{.magic = 0x2ed3a058, .checksum = nullptr};
  KeyValueStoreBuffer<kMaxEntries, kMaxUsableSectors> kvs(&flash.partition,
                                                          format);
  ASSERT_OK(kvs.Init());

  // Write the same entry many times.
  const size_t num_writes = 10;
  EXPECT_EQ(kvs.size(), 0u);
  for (size_t i = 0; i < num_writes; ++i) {
    StringBuffer<150> key;
    key << "key_" << i;
    DBG("PUT #%zu for key %s with value %zu", i, key.c_str(), i);

    size_t value = i + 77;  // Prevent accidental pass with zero.
    EXPECT_OK(kvs.Put(key.view(), value));
    EXPECT_EQ(kvs.size(), i + 1);
  }
  ASSERT_EQ(OkStatus(), flash.Dump("WritingMultipleKeysIncreasesSize.bin"));
}

TEST(InMemoryKvs, WriteAndReadOneKey) {
  // Create and erase the fake flash.
  Flash flash;
  ASSERT_OK(flash.partition.Erase());

  // Create and initialize the KVS.
  // For KVS magic value always use a random 32 bit integer rather than a
  // human readable 4 bytes. See pw_kvs/format.h for more information.
  constexpr EntryFormat format{.magic = 0x5d70896, .checksum = nullptr};
  KeyValueStoreBuffer<kMaxEntries, kMaxUsableSectors> kvs(&flash.partition,
                                                          format);
  ASSERT_OK(kvs.Init());

  // Add one entry.
  const char* key = "Key1";
  DBG("PUT value for key: %s", key);
  uint8_t written_value = 0xDA;
  ASSERT_OK(kvs.Put(key, written_value));
  EXPECT_EQ(kvs.size(), 1u);

  DBG("GET value for key: %s", key);
  uint8_t actual_value;
  ASSERT_OK(kvs.Get(key, &actual_value));
  EXPECT_EQ(actual_value, written_value);

  EXPECT_EQ(kvs.size(), 1u);
}

TEST(InMemoryKvs, WriteOneKeyValueMultipleTimes) {
  // Create and erase the fake flash.
  Flash flash;
  ASSERT_OK(flash.partition.Erase());

  // Create and initialize the KVS.
  KeyValueStoreBuffer<kMaxEntries, kMaxUsableSectors> kvs(&flash.partition,
                                                          default_format);
  ASSERT_OK(kvs.Init());

  // Add one entry, with the same key and value, multiple times.
  const char* key = "Key1";
  uint8_t written_value = 0xDA;
  for (int i = 0; i < 50; i++) {
    DBG("PUT [%d] value for key: %s", i, key);
    ASSERT_OK(kvs.Put(key, written_value));
    EXPECT_EQ(kvs.size(), 1u);
  }

  DBG("GET value for key: %s", key);
  uint8_t actual_value;
  ASSERT_OK(kvs.Get(key, &actual_value));
  EXPECT_EQ(actual_value, written_value);

  // Verify that only one entry was written to the KVS.
  EXPECT_EQ(kvs.size(), 1u);
  EXPECT_EQ(kvs.transaction_count(), 1u);
  KeyValueStore::StorageStats stats = kvs.GetStorageStats();
  EXPECT_EQ(stats.reclaimable_bytes, 0u);
}

TEST(InMemoryKvs, Basic) {
  const char* key1 = "Key1";
  const char* key2 = "Key2";

  // Create and erase the fake flash.
  Flash flash;
  ASSERT_EQ(OkStatus(), flash.partition.Erase());

  // Create and initialize the KVS.
  // For KVS magic value always use a random 32 bit integer rather than a
  // human readable 4 bytes. See pw_kvs/format.h for more information.
  constexpr EntryFormat format{.magic = 0x7bf19895, .checksum = nullptr};
  KeyValueStoreBuffer<kMaxEntries, kMaxUsableSectors> kvs(&flash.partition,
                                                          format);
  ASSERT_OK(kvs.Init());

  // Add two entries with different keys and values.
  uint8_t value1 = 0xDA;
  ASSERT_OK(kvs.Put(key1, as_bytes(span(&value1, sizeof(value1)))));
  EXPECT_EQ(kvs.size(), 1u);

  uint32_t value2 = 0xBAD0301f;
  ASSERT_OK(kvs.Put(key2, value2));
  EXPECT_EQ(kvs.size(), 2u);

  // Verify data
  uint32_t test2;
  EXPECT_OK(kvs.Get(key2, &test2));

  uint8_t test1;
  ASSERT_OK(kvs.Get(key1, &test1));

  EXPECT_EQ(test1, value1);
  EXPECT_EQ(test2, value2);

  EXPECT_EQ(kvs.size(), 2u);
}

TEST(InMemoryKvs, CallingEraseTwice_NothingWrittenToFlash) {
  // Create and erase the fake flash.
  Flash flash;
  ASSERT_EQ(OkStatus(), flash.partition.Erase());

  // Create and initialize the KVS.
  KeyValueStoreBuffer<kMaxEntries, kMaxUsableSectors> kvs(&flash.partition,
                                                          default_format);
  ASSERT_OK(kvs.Init());

  const uint8_t kValue = 0xDA;
  ASSERT_EQ(OkStatus(), kvs.Put(keys[0], kValue));
  ASSERT_EQ(OkStatus(), kvs.Delete(keys[0]));

  // Compare before / after checksums to verify that nothing was written.
  const uint16_t crc = checksum::Crc16Ccitt::Calculate(flash.memory.buffer());

  EXPECT_EQ(kvs.Delete(keys[0]), Status::NotFound());

  EXPECT_EQ(crc, checksum::Crc16Ccitt::Calculate(flash.memory.buffer()));
}

class LargeEmptyInitializedKvs : public ::testing::Test {
 protected:
  LargeEmptyInitializedKvs() : kvs_(&large_test_partition, default_format) {
    PW_CHECK_OK(large_test_partition.Erase());
    PW_CHECK_OK(kvs_.Init());
  }

  KeyValueStoreBuffer<kMaxEntries, kMaxUsableSectors> kvs_;
};

TEST_F(LargeEmptyInitializedKvs, Basic) {
  const uint8_t kValue1 = 0xDA;
  const uint8_t kValue2 = 0x12;
  uint8_t value;
  ASSERT_EQ(OkStatus(), kvs_.Put(keys[0], kValue1));
  EXPECT_EQ(kvs_.size(), 1u);
  ASSERT_EQ(OkStatus(), kvs_.Delete(keys[0]));
  EXPECT_EQ(kvs_.Get(keys[0], &value), Status::NotFound());
  ASSERT_EQ(OkStatus(), kvs_.Put(keys[1], kValue1));
  ASSERT_EQ(OkStatus(), kvs_.Put(keys[2], kValue2));
  ASSERT_EQ(OkStatus(), kvs_.Delete(keys[1]));
  EXPECT_EQ(OkStatus(), kvs_.Get(keys[2], &value));
  EXPECT_EQ(kValue2, value);
  ASSERT_EQ(kvs_.Get(keys[1], &value), Status::NotFound());
  EXPECT_EQ(kvs_.size(), 1u);
}

TEST_F(LargeEmptyInitializedKvs, FullMaintenance) {
  const uint8_t kValue1 = 0xDA;
  const uint8_t kValue2 = 0x12;

  // Write a key and write again with a different value, resulting in a stale
  // entry from the first write.
  ASSERT_EQ(OkStatus(), kvs_.Put(keys[0], kValue1));
  ASSERT_EQ(OkStatus(), kvs_.Put(keys[0], kValue2));
  EXPECT_EQ(kvs_.size(), 1u);

  KeyValueStore::StorageStats stats = kvs_.GetStorageStats();
  EXPECT_EQ(stats.sector_erase_count, 0u);
  EXPECT_GT(stats.reclaimable_bytes, 0u);

  // Do regular FullMaintenance, which should not touch the sector with valid
  // data.
  EXPECT_EQ(OkStatus(), kvs_.FullMaintenance());
  stats = kvs_.GetStorageStats();
  EXPECT_EQ(stats.sector_erase_count, 0u);
  EXPECT_GT(stats.reclaimable_bytes, 0u);

  // Do aggressive FullMaintenance, which should GC the sector with valid data,
  // resulting in no reclaimable bytes and an erased sector.
  EXPECT_EQ(OkStatus(), kvs_.HeavyMaintenance());
  stats = kvs_.GetStorageStats();
  EXPECT_EQ(stats.sector_erase_count, 1u);
  EXPECT_EQ(stats.reclaimable_bytes, 0u);
}

TEST_F(LargeEmptyInitializedKvs, KeyDeletionMaintenance) {
  const uint8_t kValue1 = 0xDA;
  const uint8_t kValue2 = 0x12;
  uint8_t val = 0;

  // Write and delete a key. The key should be gone, but the size should be 1.
  ASSERT_EQ(OkStatus(), kvs_.Put(keys[0], kValue1));
  ASSERT_EQ(kvs_.size(), 1u);
  ASSERT_EQ(OkStatus(), kvs_.Delete(keys[0]));

  // Ensure the key is indeed gone and the size is 1 before continuing.
  ASSERT_EQ(kvs_.Get(keys[0], &val), Status::NotFound());
  ASSERT_EQ(kvs_.size(), 0u);
  ASSERT_EQ(kvs_.total_entries_with_deleted(), 1u);

  KeyValueStore::StorageStats stats = kvs_.GetStorageStats();
  EXPECT_EQ(stats.sector_erase_count, 0u);
  EXPECT_GT(stats.reclaimable_bytes, 0u);

  // Do aggressive FullMaintenance, which should GC the sector with valid data,
  // resulting in no reclaimable bytes and an erased sector.
  EXPECT_EQ(OkStatus(), kvs_.HeavyMaintenance());
  stats = kvs_.GetStorageStats();
  EXPECT_EQ(stats.reclaimable_bytes, 0u);
  ASSERT_EQ(kvs_.size(), 0u);

  if (PW_KVS_REMOVE_DELETED_KEYS_IN_HEAVY_MAINTENANCE) {
    EXPECT_GT(stats.sector_erase_count, 1u);
    ASSERT_EQ(kvs_.total_entries_with_deleted(), 0u);
  } else {  // The deleted entries are only removed if that feature is enabled.
    EXPECT_EQ(stats.sector_erase_count, 1u);
    ASSERT_EQ(kvs_.total_entries_with_deleted(), 1u);
  }

  // Do it again but with 2 keys and keep one.
  ASSERT_EQ(OkStatus(), kvs_.Put(keys[0], kValue1));
  ASSERT_EQ(OkStatus(), kvs_.Put(keys[1], kValue2));
  ASSERT_EQ(kvs_.size(), 2u);
  ASSERT_EQ(OkStatus(), kvs_.Delete(keys[0]));

  // Ensure the key is indeed gone and the size is 1 before continuing.
  ASSERT_EQ(kvs_.Get(keys[0], &val), Status::NotFound());
  ASSERT_EQ(kvs_.size(), 1u);
  ASSERT_EQ(kvs_.total_entries_with_deleted(), 2u);

  // Do aggressive FullMaintenance, which should GC the sector with valid data,
  // resulting in no reclaimable bytes and an erased sector.
  EXPECT_EQ(OkStatus(), kvs_.HeavyMaintenance());
  stats = kvs_.GetStorageStats();
  ASSERT_EQ(kvs_.size(), 1u);

  if (PW_KVS_REMOVE_DELETED_KEYS_IN_HEAVY_MAINTENANCE) {
    ASSERT_EQ(kvs_.total_entries_with_deleted(), 1u);
  } else {  // The deleted entries are only removed if that feature is enabled.
    ASSERT_EQ(kvs_.total_entries_with_deleted(), 2u);
  }

  // Read back the second key to make sure it is still valid.
  ASSERT_EQ(kvs_.Get(keys[1], &val), OkStatus());
  ASSERT_EQ(val, kValue2);
}

TEST(InMemoryKvs, Put_MaxValueSize) {
  // Create and erase the fake flash.
  Flash flash;
  ASSERT_EQ(OkStatus(), flash.partition.Erase());

  // Create and initialize the KVS.
  KeyValueStoreBuffer<kMaxEntries, kMaxUsableSectors> kvs(&flash.partition,
                                                          default_format);
  ASSERT_OK(kvs.Init());

  size_t max_key_value_size = kvs.max_key_value_size_bytes();
  EXPECT_EQ(max_key_value_size,
            KeyValueStore::max_key_value_size_bytes(
                flash.partition.sector_size_bytes()));

  size_t max_value_size =
      flash.partition.sector_size_bytes() - sizeof(EntryHeader) - 1;
  EXPECT_EQ(max_key_value_size, (max_value_size + 1));

  // Use the large_test_flash as a big chunk of data for the Put statement.
  ASSERT_GT(sizeof(large_test_flash), max_value_size + 2 * sizeof(EntryHeader));
  auto big_data = as_bytes(span(&large_test_flash, 1));

  EXPECT_EQ(OkStatus(), kvs.Put("K", big_data.subspan(0, max_value_size)));

  // Larger than maximum is rejected.
  EXPECT_EQ(Status::InvalidArgument(),
            kvs.Put("K", big_data.subspan(0, max_value_size + 1)));
  EXPECT_EQ(Status::InvalidArgument(), kvs.Put("K", big_data));
}

}  // namespace pw::kvs
