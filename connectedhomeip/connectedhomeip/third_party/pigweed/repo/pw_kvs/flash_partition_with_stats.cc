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

#define PW_LOG_MODULE_NAME "PW_FLASH"
#define PW_LOG_LEVEL PW_KVS_LOG_LEVEL

#include "pw_kvs/flash_partition_with_stats.h"

#include <cstdio>

#include "pw_kvs/flash_memory.h"
#include "pw_kvs_private/config.h"
#include "pw_log/log.h"

namespace pw::kvs {

Status FlashPartitionWithStats::SaveStorageStats(const KeyValueStore& kvs,
                                                 const char* label) {
  // If empty, saving stats is disabled so do not save any stats.
  if (sector_counters_.empty()) {
    return OkStatus();
  }

  KeyValueStore::StorageStats stats = kvs.GetStorageStats();
  size_t utilization_percentage = (stats.in_use_bytes * 100) / size_bytes();

  const char* file_name = "flash_stats.csv";
  std::FILE* out_file = std::fopen(file_name, "a+");
  if (out_file == nullptr) {
    PW_LOG_ERROR("Failed to dump to %s", file_name);
    return Status::NotFound();
  }

  // If file is empty add the header row.
  std::fseek(out_file, 0, SEEK_END);
  if (std::ftell(out_file) == 0) {
    std::fprintf(out_file,
                 "Test Name,Total Erases,Utilization Percentage,Transaction "
                 "Count,Entry Count");
    for (size_t i = 0; i < sector_counters_.size(); i++) {
      std::fprintf(out_file, ",Sector %zu", i);
    }
    std::fprintf(out_file, "\n");
  }

  std::fprintf(out_file, "\"%s\",%zu", label, total_erase_count());
  std::fprintf(out_file,
               ",%zu,%u,%zu",
               utilization_percentage,
               unsigned(kvs.transaction_count()),
               kvs.size());

  for (size_t counter : sector_erase_counters()) {
    std::fprintf(out_file, ",%zu", counter);
  }

  std::fprintf(out_file, "\n");
  std::fclose(out_file);
  return OkStatus();
}

Status FlashPartitionWithStats::Erase(Address address, size_t num_sectors) {
  size_t base_index = address / FlashPartition::sector_size_bytes();
  if (base_index < sector_counters_.size()) {
    num_sectors = std::min(num_sectors, (sector_counters_.size() - base_index));
    for (size_t i = 0; i < num_sectors; i++) {
      sector_counters_[base_index + i]++;
    }
  }

  return FlashPartition::Erase(address, num_sectors);
}

}  // namespace pw::kvs
