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

#include <cstddef>
#include <cstdint>
#include <initializer_list>

#include "pw_assert/assert.h"
#include "pw_kvs/alignment.h"
#include "pw_polyfill/standard.h"
#include "pw_span/span.h"
#include "pw_status/status.h"
#include "pw_status/status_with_size.h"

#if PW_CXX_STANDARD_IS_SUPPORTED(17)  // Requires C++17 for pw::Result
#include "pw_stream/seek.h"
#include "pw_stream/stream.h"
#endif  // PW_CXX_STANDARD_IS_SUPPORTED(17)

namespace pw {
namespace kvs {

enum class PartitionPermission : bool {
  kReadOnly,
  kReadAndWrite,
};

class FlashMemory {
 public:
  // The flash address is in the range of: 0 to FlashSize.
  typedef uint32_t Address;

  // TODO(b/235149326): This can be constexpr when tokenized asserts are fixed.
  FlashMemory(size_t sector_size,
              size_t sector_count,
              size_t alignment,
              uint32_t start_address = 0,
              uint32_t sector_start = 0,
              std::byte erased_memory_content = std::byte(0xFF))
      : sector_size_(sector_size),
        flash_sector_count_(sector_count),
        alignment_(alignment),
        start_address_(start_address),
        start_sector_(sector_start),
        erased_memory_content_(erased_memory_content) {
    PW_ASSERT(alignment_ != 0u);
  }

  virtual ~FlashMemory() = default;

  virtual Status Enable() = 0;

  virtual Status Disable() = 0;

  virtual bool IsEnabled() const = 0;

  virtual Status SelfTest() { return Status::Unimplemented(); }

  // Erase num_sectors starting at a given address. Blocking call.
  // Address should be on a sector boundary. Returns:
  //
  // OK - success
  // DEADLINE_EXCEEDED - timeout
  // INVALID_ARGUMENT - address is not sector-aligned
  // OUT_OF_RANGE - erases past the end of the memory
  virtual Status Erase(Address flash_address, size_t num_sectors) = 0;

  // Reads bytes from flash into buffer. Blocking call. Returns:
  //
  // OK - success
  // DEADLINE_EXCEEDED - timeout
  // OUT_OF_RANGE - write does not fit in the flash memory
  virtual StatusWithSize Read(Address address, span<std::byte> output) = 0;

  StatusWithSize Read(Address address, void* buffer, size_t len) {
    return Read(address, span<std::byte>(static_cast<std::byte*>(buffer), len));
  }

  // Writes bytes to flash. Blocking call. Returns:
  //
  // OK - success
  // DEADLINE_EXCEEDED - timeout
  // INVALID_ARGUMENT - address or data size are not aligned
  // OUT_OF_RANGE - write does not fit in the memory
  virtual StatusWithSize Write(Address destination_flash_address,
                               span<const std::byte> data) = 0;

  StatusWithSize Write(Address destination_flash_address,
                       const void* data,
                       size_t len) {
    return Write(
        destination_flash_address,
        span<const std::byte>(static_cast<const std::byte*>(data), len));
  }

  // Convert an Address to an MCU pointer, this can be used for memory
  // mapped reads. Return NULL if the memory is not memory mapped.
  virtual std::byte* FlashAddressToMcuAddress(Address) const { return nullptr; }

  // start_sector() is useful for FlashMemory instances where the
  // sector start is not 0. (ex.: cases where there are portions of flash
  // that should be handled independently).
  constexpr uint32_t start_sector() const { return start_sector_; }

  constexpr size_t sector_size_bytes() const { return sector_size_; }

  constexpr size_t sector_count() const { return flash_sector_count_; }

  constexpr size_t alignment_bytes() const { return alignment_; }

  constexpr size_t size_bytes() const {
    return sector_size_ * flash_sector_count_;
  }

  // Address of the start of flash (the address of sector 0)
  constexpr uint32_t start_address() const { return start_address_; }

  constexpr std::byte erased_memory_content() const {
    return erased_memory_content_;
  }

 private:
  const uint32_t sector_size_;
  const uint32_t flash_sector_count_;
  const uint32_t alignment_;
  const uint32_t start_address_;
  const uint32_t start_sector_;
  const std::byte erased_memory_content_;
};

class FlashPartition {
 public:
  // The flash address is in the range of: 0 to PartitionSize.
  using Address = uint32_t;

#if PW_CXX_STANDARD_IS_SUPPORTED(17)  // Requires C++17 for pw::Result
  class Writer final : public stream::NonSeekableWriter {
   public:
    constexpr Writer(kvs::FlashPartition& partition)
        : partition_(partition), position_(0) {}

   private:
    Status DoWrite(ConstByteSpan data) override;

    size_t DoTell() override { return position_; }

    size_t ConservativeLimit(LimitType type) const override {
      return type == LimitType::kWrite ? partition_.size_bytes() - position_
                                       : 0;
    }

    FlashPartition& partition_;
    size_t position_;
  };

  class Reader final : public stream::SeekableReader {
   public:
    constexpr Reader(kvs::FlashPartition& partition)
        : partition_(partition), position_(0) {}

    Reader(const Reader&) = delete;
    Reader& operator=(const Reader&) = delete;

   private:
    StatusWithSize DoRead(ByteSpan data) override;

    size_t DoTell() override { return position_; }

    Status DoSeek(ptrdiff_t offset, Whence origin) override {
      return CalculateSeek(offset, origin, partition_.size_bytes(), position_);
    }

    size_t ConservativeLimit(LimitType type) const override {
      return type == LimitType::kRead ? partition_.size_bytes() - position_ : 0;
    }

    FlashPartition& partition_;
    size_t position_;
  };
#endif  // PW_CXX_STANDARD_IS_SUPPORTED(17)

  // Implement Output for the Write method.
  class Output final : public pw::Output {
   public:
    constexpr Output(FlashPartition& flash, FlashPartition::Address address)
        : flash_(flash), address_(address) {}

   private:
    StatusWithSize DoWrite(span<const std::byte> data) override;

    FlashPartition& flash_;
    FlashPartition::Address address_;
  };

  // Implement Input for the Read method.
  class Input final : public pw::Input {
   public:
    constexpr Input(FlashPartition& flash, FlashPartition::Address address)
        : flash_(flash), address_(address) {}

   private:
    StatusWithSize DoRead(span<std::byte> data) override;

    FlashPartition& flash_;
    FlashPartition::Address address_;
  };

  FlashPartition(
      FlashMemory* flash,
      uint32_t flash_start_sector_index,
      uint32_t flash_sector_count,
      uint32_t alignment_bytes = 0,  // Defaults to flash alignment
      PartitionPermission permission = PartitionPermission::kReadAndWrite);

  // Creates a FlashPartition that uses the entire flash with its alignment.
  FlashPartition(FlashMemory* flash)
      : FlashPartition(
            flash, 0, flash->sector_count(), flash->alignment_bytes()) {}

  FlashPartition(FlashPartition&&) = default;
  FlashPartition(const FlashPartition&) = delete;
  FlashPartition& operator=(const FlashPartition&) = delete;

  virtual ~FlashPartition() = default;

  // Performs any required partition or flash-level initialization.
  virtual Status Init() { return OkStatus(); }

  // Erase num_sectors starting at a given address. Blocking call.
  // Address must be on a sector boundary. Returns:
  //
  // OK - success.
  // TIMEOUT - on timeout.
  // INVALID_ARGUMENT - address or sector count is invalid.
  // PERMISSION_DENIED - partition is read only.
  // UNKNOWN - HAL error
  virtual Status Erase(Address address, size_t num_sectors);

  Status Erase() { return Erase(0, this->sector_count()); }

  // Reads bytes from flash into buffer. Blocking call. Returns:
  //
  // OK - success.
  // TIMEOUT - on timeout.
  // INVALID_ARGUMENT - address or length is invalid.
  // UNKNOWN - HAL error
  virtual StatusWithSize Read(Address address, span<std::byte> output);

  StatusWithSize Read(Address address, size_t length, void* output) {
    return Read(address,
                span<std::byte>(static_cast<std::byte*>(output), length));
  }

  // Writes bytes to flash. Address and data.size_bytes() must both be a
  // multiple of alignment_bytes(). Blocking call. Returns:
  //
  // OK - success.
  // TIMEOUT - on timeout.
  // INVALID_ARGUMENT - address or length is invalid.
  // PERMISSION_DENIED - partition is read only.
  // UNKNOWN - HAL error
  virtual StatusWithSize Write(Address address, span<const std::byte> data);

  // Check to see if chunk of flash partition is erased. Address and len need to
  // be aligned with FlashMemory. Returns:
  //
  // OK - success.
  // TIMEOUT - on timeout.
  // INVALID_ARGUMENT - address or length is invalid.
  // UNKNOWN - HAL error
  // TODO(hepler): Result<bool>
  virtual Status IsRegionErased(Address source_flash_address,
                                size_t length,
                                bool* is_erased);

  // Check if the entire partition is erased.
  // Returns: same as IsRegionErased().
  Status IsErased(bool* is_erased) {
    return IsRegionErased(0, this->size_bytes(), is_erased);
  }

  // Checks to see if the data appears to be erased. No reads or writes occur;
  // the FlashPartition simply compares the data to
  // flash_.erased_memory_content().
  bool AppearsErased(span<const std::byte> data) const;

  // Optionally overridden by derived classes. The reported sector size is space
  // available to users of FlashPartition. This reported size can be smaller or
  // larger than the sector size of the backing FlashMemory.
  //
  // Possible reasons for size to be different from the backing FlashMemory
  // could be due to space reserved in the sector for FlashPartition to store
  // metadata or due to logical FlashPartition sectors that combine several
  // FlashMemory sectors.
  virtual size_t sector_size_bytes() const {
    return flash_.sector_size_bytes();
  }

  // Optionally overridden by derived classes. The reported sector count is
  // sectors available to users of FlashPartition. This reported count can be
  // same or smaller than the given flash_sector_count of the backing
  // FlashMemory for the same region of flash.
  //
  // Possible reasons for count to be different from the backing FlashMemory
  // could be due to space reserved in the FlashPartition to store metadata or
  // due to logical FlashPartition sectors that combine several FlashMemory
  // sectors.
  virtual size_t sector_count() const { return flash_sector_count_; }

  size_t size_bytes() const { return sector_count() * sector_size_bytes(); }

  // Alignment required for write address and write size.
  size_t alignment_bytes() const { return alignment_bytes_; }

  // Convert a FlashMemory::Address to an MCU pointer, this can be used for
  // memory mapped reads. Return NULL if the memory is not memory mapped.
  std::byte* PartitionAddressToMcuAddress(Address address) const {
    return flash_.FlashAddressToMcuAddress(PartitionToFlashAddress(address));
  }

  // Converts an address from the partition address space to the flash address
  // space. If the partition reserves additional space in the sector, the flash
  // address space may not be contiguous, and this conversion accounts for that.
  virtual FlashMemory::Address PartitionToFlashAddress(Address address) const {
    return flash_.start_address() +
           (flash_start_sector_index_ - flash_.start_sector()) *
               flash_.sector_size_bytes() +
           address;
  }

  bool writable() const {
    return permission_ == PartitionPermission::kReadAndWrite;
  }

  constexpr std::byte erased_memory_content() const {
    return flash_.erased_memory_content();
  }

  uint32_t start_sector_index() const { return flash_start_sector_index_; }

 protected:
  Status CheckBounds(Address address, size_t len) const;

  FlashMemory& flash() const { return flash_; }

  FlashMemory& flash_;
  const uint32_t flash_sector_count_;

 private:
  const uint32_t flash_start_sector_index_;
  const uint32_t alignment_bytes_;
  const PartitionPermission permission_;
};

}  // namespace kvs
}  // namespace pw
