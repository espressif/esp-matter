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
#pragma once

#include <algorithm>
#include <numeric>

#include "pw_containers/vector.h"
#include "pw_kvs/flash_memory.h"
#include "pw_kvs/key_value_store.h"
#include "pw_status/status.h"

#ifndef PW_KVS_RECORD_PARTITION_STATS
// PW_KVS_RECORD_PARTITION_STATS enables saving stats.
#define PW_KVS_RECORD_PARTITION_STATS 0
#endif  // PW_KVS_RECORD_PARTITION_STATS

namespace pw::kvs {

class FlashPartitionWithStats : public FlashPartition {
 public:
  // Save flash partition and KVS storage stats. Does not save if
  // sector_counters_ is zero.
  Status SaveStorageStats(const KeyValueStore& kvs, const char* label);

  using FlashPartition::Erase;

  Status Erase(Address address, size_t num_sectors) override;

  span<size_t> sector_erase_counters() {
    return span(sector_counters_.data(), sector_counters_.size());
  }

  size_t min_erase_count() const {
    if (sector_counters_.empty()) {
      return 0;
    }
    return *std::min_element(sector_counters_.begin(), sector_counters_.end());
  }

  size_t max_erase_count() const {
    if (sector_counters_.empty()) {
      return 0;
    }
    return *std::max_element(sector_counters_.begin(), sector_counters_.end());
  }

  size_t average_erase_count() const {
    return sector_counters_.empty()
               ? 0
               : total_erase_count() / sector_counters_.size();
  }

  size_t total_erase_count() const {
    return std::accumulate(
        sector_counters_.begin(), sector_counters_.end(), 0ul);
  }

  void ResetCounters() { sector_counters_.assign(sector_count(), 0); }

 protected:
  FlashPartitionWithStats(
      Vector<size_t>& sector_counters,
      FlashMemory* flash,
      uint32_t flash_start_sector_index,
      uint32_t flash_sector_count,
      uint32_t alignment_bytes = 0,  // Defaults to flash alignment
      PartitionPermission permission = PartitionPermission::kReadAndWrite)
      : FlashPartition(flash,
                       flash_start_sector_index,
                       flash_sector_count,
                       alignment_bytes,
                       permission),
        sector_counters_(sector_counters) {
    sector_counters_.assign(FlashPartition::sector_count(), 0);
  }

 private:
  Vector<size_t>& sector_counters_;
};

template <size_t kMaxSectors>
class FlashPartitionWithStatsBuffer : public FlashPartitionWithStats {
 public:
  FlashPartitionWithStatsBuffer(
      FlashMemory* flash,
      uint32_t flash_start_sector_index,
      uint32_t flash_sector_count,
      uint32_t alignment_bytes = 0,  // Defaults to flash alignment
      PartitionPermission permission = PartitionPermission::kReadAndWrite)
      : FlashPartitionWithStats(sector_counters_,
                                flash,
                                flash_start_sector_index,
                                flash_sector_count,
                                alignment_bytes,
                                permission) {}

  FlashPartitionWithStatsBuffer(FlashMemory* flash)
      : FlashPartitionWithStatsBuffer(
            flash, 0, flash->sector_count(), flash->alignment_bytes()) {}

 private:
  // If PW_KVS_RECORD_PARTITION_STATS is not set, use zero size vector which
  // will not save any stats.
  Vector<size_t, PW_KVS_RECORD_PARTITION_STATS ? kMaxSectors : 0>
      sector_counters_;
};

}  // namespace pw::kvs
