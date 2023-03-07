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
#include <array>
#include <cstddef>
#include <cstring>

#include "pw_containers/vector.h"
#include "pw_kvs/flash_memory.h"
#include "pw_span/span.h"
#include "pw_status/status.h"

namespace pw::kvs {

class FlashError {
 public:
  static constexpr FlashMemory::Address kAnyAddress = FlashMemory::Address(-1);
  static constexpr size_t kAlways = size_t(-1);

  // Creates a FlashError that always triggers on the next operation.
  static constexpr FlashError Unconditional(Status status,
                                            size_t times = kAlways,
                                            size_t delay = 0) {
    return FlashError(status, kAnyAddress, 0, times, delay);
  }

  // Creates a FlashError that triggers for particular addresses.
  static constexpr FlashError InRange(Status status,
                                      FlashMemory::Address address,
                                      size_t size = 1,
                                      size_t times = kAlways,
                                      size_t delay = 0) {
    return FlashError(status, address, size, times, delay);
  }

  // Determines if this FlashError applies to the operation.
  Status Check(FlashMemory::Address start_address, size_t size);

  // Determines if any of a series of FlashErrors applies to the operation.
  static Status Check(span<FlashError> errors,
                      FlashMemory::Address address,
                      size_t size);

 private:
  constexpr FlashError(Status status,
                       FlashMemory::Address address,
                       size_t size,  // not used if address is kAnyAddress
                       size_t times,
                       size_t delay)
      : status_(status),
        begin_(address),
        end_(address + size),  // not used if address is kAnyAddress
        delay_(delay),
        remaining_(times) {}

  const Status status_;

  const FlashMemory::Address begin_;
  const FlashMemory::Address end_;  // exclusive

  size_t delay_;
  size_t remaining_;
};

// This uses a buffer to mimic the behaviour of flash (requires erase before
// write, checks alignments, and is addressed in sectors). The underlying buffer
// is not initialized.
class FakeFlashMemory : public FlashMemory {
 public:
  // Default to 8-bit alignment.
  static constexpr size_t kDefaultAlignmentBytes = 1;

  static constexpr std::byte kErasedValue = std::byte{0xff};

  FakeFlashMemory(span<std::byte> buffer,
                  size_t sector_size,
                  size_t sector_count,
                  size_t alignment_bytes = kDefaultAlignmentBytes,
                  Vector<FlashError>& read_errors = no_errors_,
                  Vector<FlashError>& write_errors = no_errors_)
      : FlashMemory(sector_size, sector_count, alignment_bytes),
        buffer_(buffer),
        read_errors_(read_errors),
        write_errors_(write_errors) {}

  // The fake flash is always enabled.
  Status Enable() override { return OkStatus(); }

  Status Disable() override { return OkStatus(); }

  bool IsEnabled() const override { return true; }

  // Erase num_sectors starting at a given address.
  Status Erase(Address address, size_t num_sectors) override;

  // Reads bytes from flash into buffer.
  StatusWithSize Read(Address address, span<std::byte> output) override;

  // Writes bytes to flash.
  StatusWithSize Write(Address address, span<const std::byte> data) override;

  std::byte* FlashAddressToMcuAddress(Address) const override;

  // Testing API

  // Access the underlying buffer for testing purposes. Not part of the
  // FlashMemory API.
  span<std::byte> buffer() const { return buffer_; }

  bool InjectReadError(const FlashError& error) {
    if (read_errors_.full()) {
      return false;
    }
    read_errors_.push_back(error);
    return true;
  }

  bool InjectWriteError(const FlashError& error) {
    if (write_errors_.full()) {
      return false;
    }
    write_errors_.push_back(error);
    return true;
  }

 private:
  static inline Vector<FlashError, 0> no_errors_;

  const span<std::byte> buffer_;
  Vector<FlashError>& read_errors_;
  Vector<FlashError>& write_errors_;
};

// Creates an FakeFlashMemory backed by a std::array. The array is initialized
// to the erased value. A byte array to which to initialize the memory may be
// provided.
template <size_t kSectorSize, size_t kSectorCount, size_t kInjectedErrors = 8>
class FakeFlashMemoryBuffer : public FakeFlashMemory {
 public:
  // Creates a flash memory with no data written.
  explicit FakeFlashMemoryBuffer(
      size_t alignment_bytes = kDefaultAlignmentBytes)
      : FakeFlashMemory(buffer_,
                        kSectorSize,
                        kSectorCount,
                        alignment_bytes,
                        read_errors_,
                        write_errors_) {
    std::memset(buffer_.data(), int(kErasedValue), buffer_.size());
  }

  // Creates a flash memory initialized to the provided contents.
  explicit FakeFlashMemoryBuffer(
      span<const std::byte> contents,
      size_t alignment_bytes = kDefaultAlignmentBytes)
      : FakeFlashMemoryBuffer(alignment_bytes) {
    std::memcpy(buffer_.data(),
                contents.data(),
                std::min(contents.size(), buffer_.size()));
  }

 private:
  std::array<std::byte, kSectorCount * kSectorSize> buffer_;
  Vector<FlashError, kInjectedErrors> read_errors_;
  Vector<FlashError, kInjectedErrors> write_errors_;
};

}  // namespace pw::kvs
