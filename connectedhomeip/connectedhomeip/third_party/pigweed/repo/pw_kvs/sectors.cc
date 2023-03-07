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

#define PW_LOG_MODULE_NAME "KVS"
#define PW_LOG_LEVEL PW_KVS_LOG_LEVEL

#include "pw_kvs/internal/sectors.h"

#include "pw_kvs_private/config.h"
#include "pw_log/shorter.h"

namespace pw::kvs::internal {
namespace {

// Returns true if the container conatins the value.
// TODO(hepler): At some point move this to pw_containers, along with adding
// tests.
template <typename Container, typename T>
bool Contains(const Container& container, const T& value) {
  return std::find(std::begin(container), std::end(container), value) !=
         std::end(container);
}

}  // namespace

Status Sectors::Find(FindMode find_mode,
                     SectorDescriptor** found_sector,
                     size_t size,
                     span<const Address> addresses_to_skip,
                     span<const Address> reserved_addresses) {
  SectorDescriptor* first_empty_sector = nullptr;
  bool at_least_two_empty_sectors = (find_mode == kGarbageCollect);

  // Used for the GC reclaimable bytes check
  SectorDescriptor* non_empty_least_reclaimable_sector = nullptr;
  const size_t sector_size_bytes = partition_.sector_size_bytes();

  // Build a list of sectors to avoid.
  //
  // This is overly strict. reserved_addresses is populated when there are
  // sectors reserved for a new entry. It is safe to garbage collect into
  // these sectors, as long as there remains room for the pending entry. These
  // reserved sectors could also be garbage collected if they have recoverable
  // space. For simplicitly, avoid both the relocating key's redundant entries
  // (addresses_to_skip) and the sectors reserved for pending writes
  // (reserved_addresses).
  // TODO(hepler): Look into improving garbage collection.
  size_t sectors_to_skip = 0;
  for (Address address : addresses_to_skip) {
    temp_sectors_to_skip_[sectors_to_skip++] = &FromAddress(address);
  }
  for (Address address : reserved_addresses) {
    temp_sectors_to_skip_[sectors_to_skip++] = &FromAddress(address);
  }

  DBG("Find sector with %u bytes available, starting with sector %u, %s",
      unsigned(size),
      Index(last_new_),
      (find_mode == kAppendEntry) ? "Append" : "GC");
  for (size_t i = 0; i < sectors_to_skip; ++i) {
    DBG("  Skip sector %u", Index(temp_sectors_to_skip_[i]));
  }

  // last_new_ is the sector that was last selected as the "new empty sector" to
  // write to. This last new sector is used as the starting point for the next
  // "find a new empty sector to write to" operation. By using the last new
  // sector as the start point we will cycle which empty sector is selected
  // next, spreading the wear across all the empty sectors and get a wear
  // leveling benefit, rather than putting more wear on the lower number
  // sectors.
  SectorDescriptor* sector = last_new_;

  // Look for a sector to use with enough space. The search uses a 3 priority
  // tier process.
  //
  // Tier 1 is sector that already has valid data. During GC only select a
  // sector that has no reclaimable bytes. Immediately use the first matching
  // sector that is found.
  //
  // Tier 2 is find sectors that are empty/erased. While scanning for a partial
  // sector, keep track of the first empty sector and if a second empty sector
  // was seen. If during GC then count the second empty sector as always seen.
  //
  // Tier 3 is during garbage collection, find sectors with enough space that
  // are not empty but have recoverable bytes. Pick the sector with the least
  // recoverable bytes to minimize the likelyhood of this sector needing to be
  // garbage collected soon.
  for (size_t j = 0; j < descriptors_.size(); j++) {
    sector += 1;
    if (sector == descriptors_.end()) {
      sector = descriptors_.begin();
    }

    // Skip sectors in the skip list.
    if (Contains(span(temp_sectors_to_skip_, sectors_to_skip), sector)) {
      continue;
    }

    if (!sector->Empty(sector_size_bytes) && sector->HasSpace(size)) {
      if ((find_mode == kAppendEntry) ||
          (sector->RecoverableBytes(sector_size_bytes) == 0)) {
        *found_sector = sector;
        return OkStatus();
      } else {
        if ((non_empty_least_reclaimable_sector == nullptr) ||
            (non_empty_least_reclaimable_sector->RecoverableBytes(
                 sector_size_bytes) <
             sector->RecoverableBytes(sector_size_bytes))) {
          non_empty_least_reclaimable_sector = sector;
        }
      }
    }

    if (sector->Empty(sector_size_bytes)) {
      if (first_empty_sector == nullptr) {
        first_empty_sector = sector;
      } else {
        at_least_two_empty_sectors = true;
      }
    }
  }

  // Tier 2 check: If the scan for a partial sector does not find a suitable
  // sector, use the first empty sector that was found. Normally it is required
  // to keep 1 empty sector after the sector found here, but that rule does not
  // apply during GC.
  if (first_empty_sector != nullptr && at_least_two_empty_sectors) {
    DBG("  Found a usable empty sector; returning the first found (%u)",
        Index(first_empty_sector));
    last_new_ = first_empty_sector;
    *found_sector = first_empty_sector;
    return OkStatus();
  }

  // Tier 3 check: If we got this far, use the sector with least recoverable
  // bytes
  if (non_empty_least_reclaimable_sector != nullptr) {
    *found_sector = non_empty_least_reclaimable_sector;
    DBG("  Found a usable sector %u, with %u B recoverable, in GC",
        Index(*found_sector),
        unsigned((*found_sector)->RecoverableBytes(sector_size_bytes)));
    return OkStatus();
  }

  // No sector was found.
  DBG("  Unable to find a usable sector");
  *found_sector = nullptr;
  return Status::ResourceExhausted();
}

SectorDescriptor& Sectors::WearLeveledSectorFromIndex(size_t idx) const {
  return descriptors_[(Index(last_new_) + 1 + idx) % descriptors_.size()];
}

// TODO(hepler): Consider breaking this function into smaller sub-chunks.
SectorDescriptor* Sectors::FindSectorToGarbageCollect(
    span<const Address> reserved_addresses) const {
  const size_t sector_size_bytes = partition_.sector_size_bytes();
  SectorDescriptor* sector_candidate = nullptr;
  size_t candidate_bytes = 0;

  // Build a vector of sectors to avoid.
  for (size_t i = 0; i < reserved_addresses.size(); ++i) {
    temp_sectors_to_skip_[i] = &FromAddress(reserved_addresses[i]);
    DBG("    Skip sector %u", Index(reserved_addresses[i]));
  }
  const span sectors_to_skip(temp_sectors_to_skip_, reserved_addresses.size());

  // Step 1: Try to find a sectors with stale keys and no valid keys (no
  // relocation needed). Use the first such sector found, as that will help the
  // KVS "rotate" around the partition. Initially this would select the sector
  // with the most reclaimable space, but that can cause GC sector selection to
  // "ping-pong" between two sectors when updating large keys.
  for (size_t i = 0; i < descriptors_.size(); ++i) {
    SectorDescriptor& sector = WearLeveledSectorFromIndex(i);
    if ((sector.valid_bytes() == 0) &&
        (sector.RecoverableBytes(sector_size_bytes) > 0) &&
        !Contains(sectors_to_skip, &sector)) {
      sector_candidate = &sector;
      break;
    }
  }

  // Step 2: If step 1 yields no sectors, just find the sector with the most
  // reclaimable bytes but no addresses to avoid.
  if (sector_candidate == nullptr) {
    for (size_t i = 0; i < descriptors_.size(); ++i) {
      SectorDescriptor& sector = WearLeveledSectorFromIndex(i);
      if ((sector.RecoverableBytes(sector_size_bytes) > candidate_bytes) &&
          !Contains(sectors_to_skip, &sector)) {
        sector_candidate = &sector;
        candidate_bytes = sector.RecoverableBytes(sector_size_bytes);
      }
    }
  }

  // Step 3: If no sectors with reclaimable bytes, select the sector with the
  // most free bytes. This at least will allow entries of existing keys to get
  // spread to other sectors, including sectors that already have copies of the
  // current key being written.
  if (sector_candidate == nullptr) {
    for (size_t i = 0; i < descriptors_.size(); ++i) {
      SectorDescriptor& sector = WearLeveledSectorFromIndex(i);
      if ((sector.valid_bytes() > candidate_bytes) &&
          !Contains(sectors_to_skip, &sector)) {
        sector_candidate = &sector;
        candidate_bytes = sector.valid_bytes();
        DBG("    Doing GC on sector with no reclaimable bytes!");
      }
    }
  }

  if (sector_candidate != nullptr) {
    DBG("Found sector %u to Garbage Collect, %u recoverable bytes",
        Index(sector_candidate),
        unsigned(sector_candidate->RecoverableBytes(sector_size_bytes)));
  } else {
    DBG("Unable to find sector to garbage collect!");
  }
  return sector_candidate;
}

}  // namespace pw::kvs::internal
