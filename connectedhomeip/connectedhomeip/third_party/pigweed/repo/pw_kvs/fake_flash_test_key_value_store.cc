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

#include "pw_kvs/crc16_checksum.h"
#include "pw_kvs/fake_flash_memory.h"
#include "pw_kvs/flash_memory.h"
#include "pw_kvs/key_value_store.h"
#include "pw_kvs/test_key_value_store.h"
#include "pw_sync/borrow.h"
#include "pw_sync/virtual_basic_lockable.h"

namespace pw::kvs {

namespace {

#ifndef PW_FLASH_TEST_SECTORS
#define PW_FLASH_TEST_SECTORS 8U
#endif  // PW_FLASH_TEST_SECTORS

#ifndef PW_FLASH_TEST_SECTOR_SIZE
#define PW_FLASH_TEST_SECTOR_SIZE (4 * 1024U)
#endif  // PW_FLASH_TEST_SECTOR_SIZE

#ifndef PW_FLASH_TEST_ALIGNMENT
#define PW_FLASH_TEST_ALIGNMENT 16U
#endif  // PW_FLASH_TEST_ALIGNMENT

#ifndef PW_KVS_TEST_MAX_ENTIRES
#define PW_KVS_TEST_MAX_ENTIRES 32U
#endif  // PW_KVS_TEST_MAX_ENTIRES

#ifndef PW_KVS_TEST_REDUNDANCY
#define PW_KVS_TEST_REDUNDANCY 1U
#endif  // PW_KVS_TEST_REDUNDANCY

constexpr size_t kFlashTestSectors = PW_FLASH_TEST_SECTORS;
constexpr size_t kFlashTestSectorSize = PW_FLASH_TEST_SECTOR_SIZE;
constexpr size_t kFlashTestAlignment = PW_FLASH_TEST_ALIGNMENT;

constexpr size_t kKvsTestMaxEntries = PW_KVS_TEST_MAX_ENTIRES;
constexpr size_t kKvsTestRedundancy = PW_KVS_TEST_REDUNDANCY;

// Default to 8 x 4k sectors, 16 byte alignment.
FakeFlashMemoryBuffer<kFlashTestSectorSize, kFlashTestSectors> test_flash(
    kFlashTestAlignment);
FlashPartition test_partition(&test_flash);

ChecksumCrc16 kvs_checksum;

// For KVS magic value always use a random 32 bit integer rather than a human
// readable 4 bytes. See pw_kvs/format.h for more information.
constexpr EntryFormat kvs_format = {.magic = 0xc40fd8a8,
                                    .checksum = &kvs_checksum};

KeyValueStoreBuffer<kKvsTestMaxEntries, kFlashTestSectors, kKvsTestRedundancy>
    test_kvs(&test_partition, kvs_format);
sync::Borrowable<KeyValueStore> borrowable_kvs(test_kvs,
                                               sync::NoOpLock::Instance());

}  // namespace

sync::Borrowable<KeyValueStore>& TestKvs() {
  if (!test_kvs.initialized()) {
    test_kvs.Init().IgnoreError();  // TODO(b/242598609): Handle Status properly
  }

  return borrowable_kvs;
}
}  // namespace pw::kvs
