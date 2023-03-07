// Copyright 2022 The Pigweed Authors
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
#include "pw_spi/initiator.h"

namespace pw::spi {

// Represents a complete parameter set for the Initiator::WriteRead().
class MockTransaction {
 public:
  // Same set of parameters as Initiator::WriteRead().
  constexpr MockTransaction(Status expected_return_value,
                            ConstByteSpan write_buffer,
                            ConstByteSpan read_buffer)
      : return_value_(expected_return_value),
        read_buffer_(read_buffer),
        write_buffer_(write_buffer) {}

  // Gets the buffer that is virtually read.
  ConstByteSpan read_buffer() const { return read_buffer_; }

  // Gets the buffer that should be written by the driver.
  ConstByteSpan write_buffer() const { return write_buffer_; }

  // Gets the expected return value.
  Status return_value() const { return return_value_; }

 private:
  const Status return_value_;
  const ConstByteSpan read_buffer_;
  const ConstByteSpan write_buffer_;
};

// Read transaction is a helper that constructs a read only transaction.
constexpr MockTransaction MockReadTransaction(Status expected_return_value,
                                              ConstByteSpan read_buffer) {
  return MockTransaction(expected_return_value, ConstByteSpan(), read_buffer);
}

// WriteTransaction is a helper that constructs a write only transaction.
constexpr MockTransaction MockWriteTransaction(Status expected_return_value,
                                               ConstByteSpan write_buffer) {
  return MockTransaction(expected_return_value, write_buffer, ConstByteSpan());
}

// MockInitiator takes a series of read and/or write transactions and
// compares them against user/driver input.
//
// This mock uses Gtest to ensure that the transactions instantiated meet
// expectations. This MockedInitiator should be instantiated inside a Gtest test
// frame.
class MockInitiator : public pw::spi::Initiator {
 public:
  explicit constexpr MockInitiator(span<MockTransaction> transaction_list)
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

  // Implements a mocked backend for the SPI initiator.
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
  pw::Status WriteRead(pw::ConstByteSpan, pw::ByteSpan) override;

  pw::Status Configure(const pw::spi::Config& /*config */) override {
    return pw::OkStatus();
  }

 private:
  span<MockTransaction> expected_transactions_;
  size_t expected_transaction_index_;
};

// Makes a new SPI transactions list.
template <size_t kSize>
constexpr std::array<MockTransaction, kSize> MakeExpectedTransactionArray(
    const MockTransaction (&transactions)[kSize]) {
  return containers::to_array(transactions);
}

}  // namespace pw::spi
