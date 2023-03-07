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

#include "pw_spi/initiator_mock.h"

#include "gtest/gtest.h"
#include "pw_assert/check.h"
#include "pw_containers/algorithm.h"

namespace pw::spi {

Status MockInitiator::WriteRead(ConstByteSpan tx_buffer, ByteSpan rx_buffer) {
  PW_CHECK_INT_LT(expected_transaction_index_, expected_transactions_.size());

  ConstByteSpan expected_tx_buffer =
      expected_transactions_[expected_transaction_index_].write_buffer();
  EXPECT_TRUE(pw::containers::Equal(expected_tx_buffer, tx_buffer));

  ConstByteSpan expected_rx_buffer =
      expected_transactions_[expected_transaction_index_].read_buffer();
  PW_CHECK_INT_EQ(expected_rx_buffer.size(), rx_buffer.size());

  std::copy(
      expected_rx_buffer.begin(), expected_rx_buffer.end(), rx_buffer.begin());

  // Do not directly return this value as expected_transaction_index_ should be
  // incremented.
  const Status expected_return_value =
      expected_transactions_[expected_transaction_index_].return_value();

  expected_transaction_index_ += 1;

  return expected_return_value;
}

MockInitiator::~MockInitiator() { EXPECT_EQ(Finalize(), OkStatus()); }

}  // namespace pw::spi
