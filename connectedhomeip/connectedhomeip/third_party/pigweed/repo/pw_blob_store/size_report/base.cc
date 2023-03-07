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

#include <cstring>

#include "pw_assert/check.h"
#include "pw_bloat/bloat_this_binary.h"
#include "pw_kvs/flash_test_partition.h"
#include "pw_kvs/key_value_store.h"
#include "pw_log/log.h"
#include "pw_sync/borrow.h"
#include "pw_sync/virtual_basic_lockable.h"

char working_buffer[256];
volatile bool is_set;

constexpr size_t kMaxSectorCount = 64;
constexpr size_t kKvsMaxEntries = 32;

// For KVS magic value always use a random 32 bit integer rather than a human
// readable 4 bytes. See pw_kvs/format.h for more information.
static constexpr pw::kvs::EntryFormat kvs_format = {.magic = 0x22d3f8a0,
                                                    .checksum = nullptr};

volatile size_t kvs_entry_count;

pw::kvs::KeyValueStoreBuffer<kKvsMaxEntries, kMaxSectorCount> test_kvs(
    &pw::kvs::FlashTestPartition(), kvs_format);
pw::sync::Borrowable<pw::kvs::KeyValueStore> borrowable_kvs(
    test_kvs, pw::sync::NoOpLock::Instance());

int volatile* unoptimizable;

int main() {
  pw::bloat::BloatThisBinary();

  // Start of base **********************
  // Ensure we are paying the cost for log and assert.
  PW_CHECK_INT_GE(*unoptimizable, 0, "Ensure this CHECK logic stays");
  PW_LOG_INFO("We care about optimizing: %d", *unoptimizable);

  void* result =
      std::memset((void*)working_buffer, 0x55, sizeof(working_buffer));
  is_set = (result != nullptr);

  {
    pw::sync::BorrowedPointer<pw::kvs::KeyValueStore> kvs =
        borrowable_kvs.acquire();

    kvs->Init().IgnoreError();

    unsigned kvs_value = 42;
    kvs->Put("example_key", kvs_value).IgnoreError();

    kvs_entry_count = kvs->size();

    unsigned read_value = 0;
    kvs->Get("example_key", &read_value).IgnoreError();
    kvs->Delete("example_key").IgnoreError();
  }

  auto val = pw::kvs::FlashTestPartition().PartitionAddressToMcuAddress(0);
  PW_LOG_INFO("Use the variable. %u", unsigned(*val));

  std::array<std::byte, 32> blob_source_buffer;
  pw::ConstByteSpan write_data = pw::span(blob_source_buffer);
  char name[16] = "BLOB";
  std::array<std::byte, 32> read_buffer;
  pw::ByteSpan read_span = read_buffer;
  PW_LOG_INFO("Do something so variables are used. %u, %c, %u",
              unsigned(write_data.size()),
              name[0],
              unsigned(read_span.size()));
  // End of base **********************
  return 0;
}
