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

#include "pw_kvs/fake_flash_memory.h"

#include "pw_kvs_private/config.h"
#include "pw_log/log.h"

namespace pw::kvs {

Status FlashError::Check(span<FlashError> errors,
                         FlashMemory::Address address,
                         size_t size) {
  for (auto& error : errors) {
    if (Status status = error.Check(address, size); !status.ok()) {
      return status;
    }
  }

  return OkStatus();
}

Status FlashError::Check(FlashMemory::Address start_address, size_t size) {
  // Check if the event overlaps with this address range.
  if (begin_ != kAnyAddress &&
      (start_address >= end_ || (start_address + size) <= begin_)) {
    return OkStatus();
  }

  if (delay_ > 0u) {
    delay_ -= 1;
    return OkStatus();
  }

  if (remaining_ == 0u) {
    return OkStatus();
  }

  if (remaining_ != kAlways) {
    remaining_ -= 1;
  }

  return status_;
}

Status FakeFlashMemory::Erase(Address address, size_t num_sectors) {
  if (address % sector_size_bytes() != 0) {
    PW_LOG_ERROR(
        "Attempted to erase sector at non-sector aligned boundary; address %x",
        unsigned(address));
    return Status::InvalidArgument();
  }
  const size_t sector_id = address / sector_size_bytes();
  if (address / sector_size_bytes() + num_sectors > sector_count()) {
    PW_LOG_ERROR(
        "Tried to erase a sector at an address past flash end; "
        "address: %x, sector implied: %u",
        unsigned(address),
        unsigned(sector_id));
    return Status::OutOfRange();
  }

  std::memset(
      &buffer_[address], int(kErasedValue), sector_size_bytes() * num_sectors);
  return OkStatus();
}

StatusWithSize FakeFlashMemory::Read(Address address, span<std::byte> output) {
  if (address + output.size() >= sector_count() * size_bytes()) {
    return StatusWithSize::OutOfRange();
  }

  // Check for injected read errors
  Status status = FlashError::Check(read_errors_, address, output.size());
  std::memcpy(output.data(), &buffer_[address], output.size());
  return StatusWithSize(status, output.size());
}

StatusWithSize FakeFlashMemory::Write(Address address,
                                      span<const std::byte> data) {
  if (address % alignment_bytes() != 0 ||
      data.size() % alignment_bytes() != 0) {
    PW_LOG_ERROR("Unaligned write; address %x, size %u B, alignment %u",
                 unsigned(address),
                 unsigned(data.size()),
                 unsigned(alignment_bytes()));
    return StatusWithSize::InvalidArgument();
  }

  if (address + data.size() > sector_count() * sector_size_bytes()) {
    PW_LOG_ERROR(
        "Write beyond end of memory; address %x, size %u B, max address %x",
        unsigned(address),
        unsigned(data.size()),
        unsigned(sector_count() * sector_size_bytes()));
    return StatusWithSize::OutOfRange();
  }

  // Check in erased state
  for (unsigned i = 0; i < data.size(); i++) {
    if (buffer_[address + i] != kErasedValue) {
      PW_LOG_ERROR("Writing to previously written address: %x",
                   unsigned(address));
      return StatusWithSize::Unknown();
    }
  }

  // Check for any injected write errors
  Status status = FlashError::Check(write_errors_, address, data.size());
  std::memcpy(&buffer_[address], data.data(), data.size());
  return StatusWithSize(status, data.size());
}

std::byte* FakeFlashMemory::FlashAddressToMcuAddress(Address address) const {
  if (address > sector_count() * sector_size_bytes()) {
    PW_LOG_ERROR(
        "FlashAddressToMcuAddress beyond end of memory; address %x, max "
        "address %x",
        unsigned(address),
        unsigned(sector_count() * sector_size_bytes()));
    return nullptr;
  }
  return buffer_.data() + address;
}

}  // namespace pw::kvs
