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

#include "pw_kvs/fake_flash_memory.h"
#include "pw_kvs/flash_memory.h"
#include "pw_kvs/flash_test_partition.h"

namespace pw::kvs {

namespace {

#if !defined(PW_FLASH_TEST_SECTORS) || (PW_FLASH_TEST_SECTORS <= 0)
#error PW_FLASH_TEST_SECTORS must be defined and > 0
#endif  // PW_FLASH_TEST_SECTORS

#if !defined(PW_FLASH_TEST_SECTOR_SIZE) || (PW_FLASH_TEST_SECTOR_SIZE <= 0)
#error PW_FLASH_TEST_SECTOR_SIZE must be defined and > 0
#endif  // PW_FLASH_TEST_SECTOR_SIZE

#if !defined(PW_FLASH_TEST_ALIGNMENT) || (PW_FLASH_TEST_ALIGNMENT <= 0)
#error PW_FLASH_TEST_ALIGNMENT must be defined and > 0
#endif  // PW_FLASH_TEST_ALIGNMENT

constexpr size_t kFlashTestSectors = PW_FLASH_TEST_SECTORS;
constexpr size_t kFlashTestSectorSize = PW_FLASH_TEST_SECTOR_SIZE;
constexpr size_t kFlashTestAlignment = PW_FLASH_TEST_ALIGNMENT;

// Use PW_FLASH_TEST_SECTORS x PW_FLASH_TEST_SECTOR_SIZE sectors,
// PW_FLASH_TEST_ALIGNMENT byte alignment.
FakeFlashMemoryBuffer<kFlashTestSectorSize, kFlashTestSectors> test_flash(
    kFlashTestAlignment);
FlashPartition test_partition(&test_flash);

}  // namespace

FlashPartition& FlashTestPartition() { return test_partition; }

}  // namespace pw::kvs
