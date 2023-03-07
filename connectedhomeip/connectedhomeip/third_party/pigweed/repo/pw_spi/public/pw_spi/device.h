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

#pragma once

#include <utility>

#include "pw_bytes/span.h"
#include "pw_spi/chip_selector.h"
#include "pw_spi/initiator.h"
#include "pw_status/status.h"
#include "pw_status/try.h"
#include "pw_sync/borrow.h"

namespace pw::spi {

// The Device class enables data transfer with a specific SPI peripheral.
// This class combines an Initiator (representing the physical SPI bus), its
// configuration data, and the ChipSelector object to uniquely address a device.
// Transfers to a selected initiator are guarded against concurrent access
// through the use of the `Borrowable` object.
class Device {
 public:
  Device(sync::Borrowable<Initiator>& initiator,
         const Config config,
         ChipSelector& selector)
      : initiator_(initiator), config_(config), selector_(selector) {}

  ~Device() = default;

  // Synchronously read data from the SPI peripheral until the provided
  // `read_buffer` is full.
  // This call will configure the bus and activate/deactivate chip select
  // for the transfer
  //
  // Note: This call will block in the event that other clients are currently
  // performing transactions using the same SPI Initiator.
  // Returns OkStatus() on success, and implementation-specific values on
  // failure.
  Status Read(ByteSpan read_buffer) { return WriteRead({}, read_buffer); }

  // Synchronously write the contents of `write_buffer` to the SPI peripheral.
  // This call will configure the bus and activate/deactivate chip select
  // for the transfer
  //
  // Note: This call will block in the event that other clients are currently
  // performing transactions using the same SPI Initiator.
  // Returns OkStatus() on success, and implementation-specific values on
  // failure.
  Status Write(ConstByteSpan write_buffer) {
    return WriteRead(write_buffer, {});
  }

  // Perform a synchronous read/write transfer with the SPI peripheral. Data
  // from the `write_buffer` object is written to the bus, while the
  // `read_buffer` is populated with incoming data on the bus.  In the event
  // the read buffer is smaller than the write buffer (or zero-size), any
  // additional input bytes are discarded. In the event the write buffer is
  // smaller than the read buffer (or zero size), the output is padded with
  // 0-bits for the remainder of the transfer.
  // This call will configure the bus and activate/deactivate chip select
  // for the transfer
  //
  // Note: This call will block in the event that other clients
  // are currently performing transactions using the same SPI Initiator.
  // Returns OkStatus() on success, and implementation-specific values on
  // failure.
  Status WriteRead(ConstByteSpan write_buffer, ByteSpan read_buffer) {
    return StartTransaction(ChipSelectBehavior::kPerWriteRead)
        .WriteRead(write_buffer, read_buffer);
  }

  // RAII Object providing exclusive access to the SPI device.  Enables
  // thread-safe Read()/Write()/WriteRead() operations, as well as composite
  // operations consisting of multiple, uninterrupted transfers, with
  // configurable chip-select behavior.
  class Transaction final {
   public:
    Transaction() = delete;
    ~Transaction() {
      if ((selector_ != nullptr) &&
          (behavior_ == ChipSelectBehavior::kPerTransaction) &&
          (!first_write_read_)) {
        selector_->Deactivate()
            .IgnoreError();  // TODO(b/242598609): Handle Status properly
      }
    }

    // Transaction objects are moveable but not copyable
    Transaction(Transaction&& other)
        : initiator_(std::move(other.initiator_)),
          config_(other.config_),
          selector_(other.selector_),
          behavior_(other.behavior_),
          first_write_read_(other.first_write_read_) {
      other.selector_ = nullptr;
    }

    Transaction& operator=(Transaction&& other) {
      initiator_ = std::move(other.initiator_);
      config_ = other.config_;
      selector_ = other.selector_;
      other.selector_ = nullptr;
      behavior_ = other.behavior_;
      first_write_read_ = other.first_write_read_;
      return *this;
    }

    Transaction(const Transaction&) = delete;
    Transaction& operator=(const Transaction&) = delete;

    // Synchronously read data from the SPI peripheral until the provided
    // `read_buffer` is full.
    //
    // Returns OkStatus() on success, and implementation-specific values on
    // failure.
    Status Read(ByteSpan read_buffer) { return WriteRead({}, read_buffer); }

    // Synchronously write the contents of `write_buffer` to the SPI peripheral
    //
    // Returns OkStatus() on success, and implementation-specific values on
    // failure.
    Status Write(ConstByteSpan write_buffer) {
      return WriteRead(write_buffer, {});
    }

    // Perform a synchronous read/write transfer on the SPI bus.  Data from the
    // `write_buffer` object is written to the bus, while the `read_buffer` is
    // populated with incoming data on the bus.  The operation will ensure that
    // all requested data is written-to and read-from the bus. In the event the
    // read buffer is smaller than the write buffer (or zero-size), any
    // additional input bytes are discarded. In the event the write buffer is
    // smaller than the read buffer (or zero size), the output is padded with
    // 0-bits for the remainder of the transfer.
    //
    // Returns OkStatus() on success, and implementation-specific values on
    // failure.
    Status WriteRead(ConstByteSpan write_buffer, ByteSpan read_buffer) {
      // Lazy-init: Configure the SPI bus when performing the first transfer in
      // a transaction.
      if (first_write_read_) {
        PW_TRY(initiator_->Configure(config_));
      }

      if ((behavior_ == ChipSelectBehavior::kPerWriteRead) ||
          (first_write_read_)) {
        PW_TRY(selector_->Activate());
        first_write_read_ = false;
      }

      auto status = initiator_->WriteRead(write_buffer, read_buffer);

      if (behavior_ == ChipSelectBehavior::kPerWriteRead) {
        PW_TRY(selector_->Deactivate());
      }

      return status;
    }

   private:
    friend Device;
    explicit Transaction(sync::BorrowedPointer<Initiator> initiator,
                         const Config& config,
                         ChipSelector& selector,
                         ChipSelectBehavior& behavior)
        : initiator_(std::move(initiator)),
          config_(config),
          selector_(&selector),
          behavior_(behavior),
          first_write_read_(true) {}

    sync::BorrowedPointer<Initiator> initiator_;
    Config config_;
    ChipSelector* selector_;
    ChipSelectBehavior behavior_;
    bool first_write_read_;
  };

  // Begin a transaction with the SPI device.  This creates an RAII
  // `Transaction` object that ensures that only one entity can access the
  // underlying SPI bus (Initiator) for the object's duration. The `behavior`
  // parameter provides a means for a client to select how the chip-select
  // signal will be applied on Read/Write/WriteRead calls taking place with the
  // Transaction object. A value of `kPerWriteRead` will activate/deactivate
  // chip-select on each operation, while `kPerTransaction` will hold the
  // chip-select active for the duration of the Transaction object.
  Transaction StartTransaction(ChipSelectBehavior behavior) {
    return Transaction(initiator_.acquire(), config_, selector_, behavior);
  }

 private:
  sync::Borrowable<Initiator>& initiator_;
  const Config config_;
  ChipSelector& selector_;
};

}  // namespace pw::spi
