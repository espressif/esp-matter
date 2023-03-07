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

#include "pw_kvs/internal/entry_cache.h"

#include "gtest/gtest.h"
#include "pw_bytes/array.h"
#include "pw_kvs/fake_flash_memory.h"
#include "pw_kvs/flash_memory.h"
#include "pw_kvs/internal/hash.h"
#include "pw_kvs/internal/key_descriptor.h"

namespace pw::kvs::internal {
namespace {

using std::byte;

class EmptyEntryCache : public ::testing::Test {
 protected:
  static constexpr size_t kMaxEntries = 32;
  static constexpr size_t kRedundancy = 3;

  EmptyEntryCache() : entries_(descriptors_, addresses_, kRedundancy) {}

  Vector<KeyDescriptor, kMaxEntries> descriptors_;
  EntryCache::AddressList<kMaxEntries, kRedundancy> addresses_;

  EntryCache entries_;
};

constexpr char kTheKey[] = "The Key";

constexpr KeyDescriptor kDescriptor = {.key_hash = Hash(kTheKey),
                                       .transaction_id = 123,
                                       .state = EntryState::kValid};

TEST_F(EmptyEntryCache, AddNew) {
  EntryMetadata metadata = entries_.AddNew(kDescriptor, 5);
  EXPECT_EQ(kDescriptor.key_hash, metadata.hash());
  EXPECT_EQ(kDescriptor.transaction_id, metadata.transaction_id());
  EXPECT_EQ(kDescriptor.state, metadata.state());

  EXPECT_EQ(5u, metadata.first_address());
  EXPECT_EQ(1u, metadata.addresses().size());
}

TEST_F(EmptyEntryCache, EntryMetadata_AddNewAddress) {
  EntryMetadata metadata = entries_.AddNew(kDescriptor, 100);

  metadata.AddNewAddress(999);

  EXPECT_EQ(2u, metadata.addresses().size());
  EXPECT_EQ(100u, metadata.first_address());
  EXPECT_EQ(100u, metadata.addresses()[0]);
  EXPECT_EQ(999u, metadata.addresses()[1]);
}

TEST_F(EmptyEntryCache, EntryMetadata_Reset) {
  EntryMetadata metadata = entries_.AddNew(kDescriptor, 100);
  metadata.AddNewAddress(999);

  metadata.Reset(
      {.key_hash = 987, .transaction_id = 5, .state = EntryState::kDeleted},
      8888);

  EXPECT_EQ(987u, metadata.hash());
  EXPECT_EQ(5u, metadata.transaction_id());
  EXPECT_EQ(EntryState::kDeleted, metadata.state());
  EXPECT_EQ(1u, metadata.addresses().size());
  EXPECT_EQ(8888u, metadata.first_address());
  EXPECT_EQ(8888u, metadata.addresses()[0]);
}

TEST_F(EmptyEntryCache, AddNewOrUpdateExisting_NewEntry) {
  ASSERT_EQ(OkStatus(),
            entries_.AddNewOrUpdateExisting(kDescriptor, 1000, 2000));

  EXPECT_EQ(1u, entries_.present_entries());

  for (const EntryMetadata& entry : entries_) {
    EXPECT_EQ(1000u, entry.first_address());
    EXPECT_EQ(kDescriptor.key_hash, entry.hash());
    EXPECT_EQ(kDescriptor.transaction_id, entry.transaction_id());
  }
}

TEST_F(EmptyEntryCache, AddNewOrUpdateExisting_NewEntry_Full) {
  for (uint32_t i = 0; i < kMaxEntries; ++i) {
    ASSERT_EQ(  // Fill up the cache
        OkStatus(),
        entries_.AddNewOrUpdateExisting({i, i, EntryState::kValid}, i, 1));
  }
  ASSERT_EQ(kMaxEntries, entries_.total_entries());
  ASSERT_TRUE(entries_.full());

  EXPECT_EQ(Status::ResourceExhausted(),
            entries_.AddNewOrUpdateExisting(kDescriptor, 1000, 1));
  EXPECT_EQ(kMaxEntries, entries_.total_entries());
}

TEST_F(EmptyEntryCache, AddNewOrUpdateExisting_UpdatedEntry) {
  KeyDescriptor kd = kDescriptor;
  kd.transaction_id += 3;

  ASSERT_EQ(OkStatus(), entries_.AddNewOrUpdateExisting(kd, 3210, 2000));

  EXPECT_EQ(1u, entries_.present_entries());

  for (const EntryMetadata& entry : entries_) {
    EXPECT_EQ(3210u, entry.first_address());
    EXPECT_EQ(kDescriptor.key_hash, entry.hash());
    EXPECT_EQ(kDescriptor.transaction_id + 3, entry.transaction_id());
  }
}

TEST_F(EmptyEntryCache, AddNewOrUpdateExisting_AddDuplicateEntry) {
  ASSERT_EQ(OkStatus(),
            entries_.AddNewOrUpdateExisting(kDescriptor, 1000, 2000));
  ASSERT_EQ(OkStatus(),
            entries_.AddNewOrUpdateExisting(kDescriptor, 3000, 2000));
  ASSERT_EQ(OkStatus(),
            entries_.AddNewOrUpdateExisting(kDescriptor, 7000, 2000));

  // Duplicates beyond the redundancy are ignored.
  ASSERT_EQ(OkStatus(),
            entries_.AddNewOrUpdateExisting(kDescriptor, 9000, 2000));

  EXPECT_EQ(1u, entries_.present_entries());

  for (const EntryMetadata& entry : entries_) {
    EXPECT_EQ(3u, entry.addresses().size());
    EXPECT_EQ(1000u, entry.addresses()[0]);
    EXPECT_EQ(3000u, entry.addresses()[1]);
    EXPECT_EQ(7000u, entry.addresses()[2]);

    EXPECT_EQ(kDescriptor.key_hash, entry.hash());
    EXPECT_EQ(kDescriptor.transaction_id, entry.transaction_id());
  }
}

TEST_F(EmptyEntryCache, AddNewOrUpdateExisting_AddDuplicateEntryInSameSector) {
  ASSERT_EQ(OkStatus(),
            entries_.AddNewOrUpdateExisting(kDescriptor, 1000, 1000));
  EXPECT_EQ(Status::DataLoss(),
            entries_.AddNewOrUpdateExisting(kDescriptor, 1950, 1000));

  EXPECT_EQ(1u, entries_.present_entries());

  for (const EntryMetadata& entry : entries_) {
    EXPECT_EQ(1u, entry.addresses().size());
    EXPECT_EQ(1000u, entry.addresses()[0]);

    EXPECT_EQ(kDescriptor.key_hash, entry.hash());
    EXPECT_EQ(kDescriptor.transaction_id, entry.transaction_id());
  }
}

TEST_F(EmptyEntryCache, Iterator_MutableFromConst_CanModify) {
  entries_.AddNew(kDescriptor, 1);
  EntryCache::iterator it = static_cast<const EntryCache&>(entries_).begin();

  static_assert(kRedundancy > 1);
  it->AddNewAddress(1234);

  EXPECT_EQ(1u, it->first_address());
  EXPECT_EQ(1u, (*it).addresses()[0]);
  EXPECT_EQ(1234u, it->addresses()[1]);
}

TEST_F(EmptyEntryCache, Iterator_Const) {
  entries_.AddNew(kDescriptor, 99);
  EntryCache::const_iterator it = entries_.cbegin();

  EXPECT_EQ(99u, (*it).first_address());
  EXPECT_EQ(99u, it->first_address());
}

TEST_F(EmptyEntryCache, Iterator_Const_CanBeAssignedFromMutable) {
  entries_.AddNew(kDescriptor, 99);
  EntryCache::const_iterator it = entries_.begin();

  EXPECT_EQ(99u, (*it).first_address());
  EXPECT_EQ(99u, it->first_address());
}

constexpr size_t kSectorSize = 64;
constexpr uint32_t kMagic = 0xa14ae726;
// For KVS entry magic value always use a random 32 bit integer rather than a
// human readable 4 bytes. See pw_kvs/format.h for more information.
constexpr auto kTheEntry =
    bytes::Concat(uint32_t(kMagic),              // magic
                  uint32_t(0),                   // checksum
                  uint8_t(0),                    // alignment (16 B)
                  uint8_t(sizeof(kTheKey) - 1),  // key length
                  uint16_t(0),                   // value size
                  uint32_t(123),                 // transaction ID
                  bytes::String(kTheKey));
constexpr std::array<byte, kSectorSize - kTheEntry.size() % kSectorSize>
    kPadding1{};
constexpr size_t kSize1 = kTheEntry.size() + kPadding1.size();

constexpr char kCollision1[] = "9FDC";
constexpr char kCollision2[] = "axzzK";

// For KVS entry magic value always use a random 32 bit integer rather than a
// human readable 4 bytes. See pw_kvs/format.h for more information.
constexpr auto kCollisionEntry =
    bytes::Concat(uint32_t(kMagic),                  // magic
                  uint32_t(0),                       // checksum
                  uint8_t(0),                        // alignment (16 B)
                  uint8_t(sizeof(kCollision1) - 1),  // key length
                  uint16_t(0),                       // value size
                  uint32_t(123),                     // transaction ID
                  bytes::String(kCollision1));
constexpr std::array<byte, kSectorSize - kCollisionEntry.size() % kSectorSize>
    kPadding2{};
constexpr size_t kSize2 = kCollisionEntry.size() + kPadding2.size();

// For KVS entry magic value always use a random 32 bit integer rather than a
// human readable 4 bytes. See pw_kvs/format.h for more information.
constexpr auto kDeletedEntry =
    bytes::Concat(uint32_t(kMagic),                 // magic
                  uint32_t(0),                      // checksum
                  uint8_t(0),                       // alignment (16 B)
                  uint8_t(sizeof("delorted") - 1),  // key length
                  uint16_t(0xffff),                 // value size (deleted)
                  uint32_t(123),                    // transaction ID
                  bytes::String("delorted"));
constexpr std::array<byte, kSectorSize - kDeletedEntry.size() % kSectorSize>
    kPadding3{};

// For KVS entry magic value always use a random 32 bit integer rather than a
// human readable 4 bytes. See pw_kvs/format.h for more information.
constexpr EntryFormat kFormat{.magic = uint32_t(kMagic), .checksum = nullptr};

class InitializedEntryCache : public EmptyEntryCache {
 protected:
  static_assert(Hash(kCollision1) == Hash(kCollision2));

  InitializedEntryCache()
      : flash_(bytes::Concat(kTheEntry,
                             kPadding1,
                             kTheEntry,
                             kPadding1,
                             kCollisionEntry,
                             kPadding2,
                             kDeletedEntry,
                             kPadding3)),
        partition_(&flash_),
        sectors_(sector_descriptors_, partition_, nullptr),
        format_(kFormat) {
    sectors_.Reset();
    size_t address = 0;
    auto entry = entries_.AddNew(kDescriptor, address);

    address += kSize1;
    entry.AddNewAddress(kSize1);

    address += kSize1;
    entries_.AddNew({.key_hash = Hash(kCollision1),
                     .transaction_id = 125,
                     .state = EntryState::kDeleted},
                    address);

    address += kSize2;
    entries_.AddNew({.key_hash = Hash("delorted"),
                     .transaction_id = 256,
                     .state = EntryState::kDeleted},
                    address);
  }

  void CheckForCorruptSectors(SectorDescriptor* sector1 = nullptr,
                              SectorDescriptor* sector2 = nullptr) {
    for (const auto& sector : sectors_) {
      bool expect_corrupt =
          ((sector1 && &sector == sector1) || (sector2 && &sector == sector2));
      EXPECT_EQ(expect_corrupt, sector.corrupt());
    }
  }

  static constexpr size_t kTotalSectors = 128;
  FakeFlashMemoryBuffer<kSectorSize, kTotalSectors> flash_;
  FlashPartition partition_;

  Vector<SectorDescriptor, kTotalSectors> sector_descriptors_;
  Sectors sectors_;

  EntryFormats format_;
};

TEST_F(InitializedEntryCache, EntryCounts) {
  EXPECT_EQ(3u, entries_.total_entries());
  EXPECT_EQ(1u, entries_.present_entries());
  EXPECT_EQ(kMaxEntries, entries_.max_entries());
}

TEST_F(InitializedEntryCache, Reset_ClearsEntryCounts) {
  entries_.Reset();

  EXPECT_EQ(0u, entries_.total_entries());
  EXPECT_EQ(0u, entries_.present_entries());
  EXPECT_EQ(kMaxEntries, entries_.max_entries());
}

TEST_F(InitializedEntryCache, Find_PresentEntry) {
  EntryMetadata metadata;

  StatusWithSize result =
      entries_.Find(partition_, sectors_, format_, kTheKey, &metadata);

  ASSERT_EQ(OkStatus(), result.status());
  EXPECT_EQ(0u, result.size());
  EXPECT_EQ(Hash(kTheKey), metadata.hash());
  EXPECT_EQ(EntryState::kValid, metadata.state());
  CheckForCorruptSectors();
}

TEST_F(InitializedEntryCache, Find_PresentEntryWithSingleReadError) {
  // Inject 2 read errors so that the initial key read and the follow-up full
  // read of the first entry fail.
  flash_.InjectReadError(FlashError::Unconditional(Status::Internal(), 2));

  EntryMetadata metadata;

  StatusWithSize result =
      entries_.Find(partition_, sectors_, format_, kTheKey, &metadata);

  ASSERT_EQ(OkStatus(), result.status());
  EXPECT_EQ(1u, result.size());
  EXPECT_EQ(Hash(kTheKey), metadata.hash());
  EXPECT_EQ(EntryState::kValid, metadata.state());
  CheckForCorruptSectors(&sectors_.FromAddress(0));
}

TEST_F(InitializedEntryCache, Find_PresentEntryWithMultiReadError) {
  flash_.InjectReadError(FlashError::Unconditional(Status::Internal(), 4));

  EntryMetadata metadata;

  StatusWithSize result =
      entries_.Find(partition_, sectors_, format_, kTheKey, &metadata);

  ASSERT_EQ(Status::DataLoss(), result.status());
  EXPECT_EQ(1u, result.size());
  CheckForCorruptSectors(&sectors_.FromAddress(0),
                         &sectors_.FromAddress(kSize1));
}

TEST_F(InitializedEntryCache, Find_DeletedEntry) {
  EntryMetadata metadata;

  StatusWithSize result =
      entries_.Find(partition_, sectors_, format_, "delorted", &metadata);

  ASSERT_EQ(OkStatus(), result.status());
  EXPECT_EQ(0u, result.size());
  EXPECT_EQ(Hash("delorted"), metadata.hash());
  EXPECT_EQ(EntryState::kDeleted, metadata.state());
  CheckForCorruptSectors();
}

TEST_F(InitializedEntryCache, Find_MissingEntry) {
  EntryMetadata metadata;

  StatusWithSize result =
      entries_.Find(partition_, sectors_, format_, "3.141", &metadata);

  ASSERT_EQ(Status::NotFound(), result.status());
  EXPECT_EQ(0u, result.size());
  CheckForCorruptSectors();
}

TEST_F(InitializedEntryCache, Find_Collision) {
  EntryMetadata metadata;

  StatusWithSize result =
      entries_.Find(partition_, sectors_, format_, kCollision2, &metadata);
  EXPECT_EQ(Status::AlreadyExists(), result.status());
  EXPECT_EQ(0u, result.size());
  CheckForCorruptSectors();
}

}  // namespace
}  // namespace pw::kvs::internal
