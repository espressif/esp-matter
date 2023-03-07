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

#include <array>
#include <cstddef>
#include <optional>

#include "pw_bytes/span.h"
#include "pw_containers/to_array.h"
#include "pw_i2c/initiator.h"

namespace pw::i2c {

// Represents a complete parameter set for the Initiator::DoWriteReadFor().
class Transaction {
 public:
  // Same set of parameters as  Initiator::DoWriteReadFor(), with the exception
  // of optional parameter timeout.
  constexpr Transaction(
      Status expected_return_value,
      Address device_address,
      ConstByteSpan write_buffer,
      ConstByteSpan read_buffer,
      std::optional<chrono::SystemClock::duration> timeout = std::nullopt)
      : return_value_(expected_return_value),
        read_buffer_(read_buffer),
        write_buffer_(write_buffer),
        address_(device_address),
        timeout_(timeout) {}

  // Alternate Transaction constructor for use in ProbeTransaction.
  constexpr Transaction(
      Status expected_return_value,
      Address device_address,
      std::optional<chrono::SystemClock::duration> timeout = std::nullopt)
      : Transaction(expected_return_value,
                    device_address,
                    ConstByteSpan(),
                    ignored_buffer_,
                    timeout) {}

  // Gets the buffer that is virtually read.
  ConstByteSpan read_buffer() const { return read_buffer_; }

  // Gets the buffer that should be written by the driver.
  ConstByteSpan write_buffer() const { return write_buffer_; }

  // Gets the min duration for a blocking i2c transaction.
  std::optional<chrono::SystemClock::duration> timeout() const {
    return timeout_;
  }

  // Gets the i2c address that the i2c transaction is targetting.
  Address address() const { return address_; }

  // Gets the expected return value.
  Status return_value() const { return return_value_; }

 private:
  const Status return_value_;
  const ConstByteSpan read_buffer_;
  const ConstByteSpan write_buffer_;
  static constexpr std::array<std::byte, 1> ignored_buffer_ = {};
  const Address address_;
  const std::optional<chrono::SystemClock::duration> timeout_;
};

// Read transaction is a helper that constructs a read only transaction.
constexpr Transaction ReadTransaction(
    Status expected_return_value,
    Address device_address,
    ConstByteSpan read_buffer,
    std::optional<chrono::SystemClock::duration> timeout = std::nullopt) {
  return Transaction(expected_return_value,
                     device_address,
                     ConstByteSpan(),
                     read_buffer,
                     timeout);
}

// WriteTransaction is a helper that constructs a write only transaction.
constexpr Transaction WriteTransaction(
    Status expected_return_value,
    Address device_address,
    ConstByteSpan write_buffer,
    std::optional<chrono::SystemClock::duration> timeout = std::nullopt) {
  return Transaction(expected_return_value,
                     device_address,
                     write_buffer,
                     ConstByteSpan(),
                     timeout);
}

// ProbeTransaction is a helper that constructs a one-byte read transaction.
// For use in testing Probe transactions with the Mock Initiator.
constexpr Transaction ProbeTransaction(
    Status expected_return_value,
    Address device_address,
    std::optional<chrono::SystemClock::duration> timeout = std::nullopt) {
  return Transaction(expected_return_value, device_address, timeout);
}

// MockInitiator takes a series of read and/or write transactions and
// compares them against user/driver input.
//
// This mock uses Gtest to ensure that the transactions instantiated meet
// expectations. This MockedInitiator should be instantiated inside a Gtest test
// frame.
class MockInitiator : public Initiator {
 public:
  explicit constexpr MockInitiator(span<Transaction> transaction_list)
      : expected_transactions_(transaction_list),
        expected_transaction_index_(0) {}

  // Should be called at the end of the test to ensure that all expected
  // transactions have been met.
  // Returns:
  // Ok - Success.
  // OutOfRange - The mocked set of transactions has not been exhausted.
  Status Finalize() const {
    if (expected_transaction_index_ != expected_transactions_.size()) {
      return Status::OutOfRange();
    }
    return Status();
  }

  // Runs Finalize() regardless of whether it was already optionally finalized.
  ~MockInitiator() override;

 private:
  // Implements a mocked backend for the i2c initiator.
  //
  // Expects (via Gtest):
  // tx_buffer == expected_transaction_tx_buffer
  // tx_buffer.size() == expected_transaction_tx_buffer.size()
  // rx_buffer.size() == expected_transaction_rx_buffer.size()
  //
  // Asserts:
  // When the number of calls to this method exceed the number of expected
  //    transactions.
  //
  // Returns:
  // Specified transaction return type
  Status DoWriteReadFor(Address device_address,
                        ConstByteSpan tx_buffer,
                        ByteSpan rx_buffer,
                        chrono::SystemClock::duration timeout) override;

  span<Transaction> expected_transactions_;
  size_t expected_transaction_index_;
};

// Makes a new i2c transactions list.
template <size_t kSize>
constexpr std::array<Transaction, kSize> MakeExpectedTransactionArray(
    const Transaction (&transactions)[kSize]) {
  return containers::to_array(transactions);
}

}  // namespace pw::i2c
