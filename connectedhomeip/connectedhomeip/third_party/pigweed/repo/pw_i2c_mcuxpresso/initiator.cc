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
#include "pw_i2c_mcuxpresso/initiator.h"

#include <mutex>

#include "fsl_i2c.h"
#include "pw_chrono/system_clock.h"
#include "pw_status/status.h"
#include "pw_status/try.h"

namespace pw::i2c {
namespace {

Status HalStatusToPwStatus(status_t status) {
  switch (status) {
    case kStatus_Success:
      return OkStatus();
    case kStatus_I2C_Nak:
    case kStatus_I2C_Addr_Nak:
      return Status::Unavailable();
    case kStatus_I2C_InvalidParameter:
      return Status::InvalidArgument();
    case kStatus_I2C_Timeout:
      return Status::DeadlineExceeded();
    default:
      return Status::Unknown();
  }
}
}  // namespace

// inclusive-language: disable
McuxpressoInitiator::McuxpressoInitiator(I2C_Type* base,
                                         uint32_t baud_rate_bps,
                                         uint32_t src_clock_hz)
    : base_(base) {
  i2c_master_config_t master_config;
  I2C_MasterGetDefaultConfig(&master_config);
  master_config.baudRate_Bps = baud_rate_bps;
  I2C_MasterInit(base_, &master_config, src_clock_hz);

  // Create the handle for the non-blocking transfer and register callback.
  I2C_MasterTransferCreateHandle(
      base_, &handle_, McuxpressoInitiator::TransferCompleteCallback, this);
}

McuxpressoInitiator::~McuxpressoInitiator() { I2C_MasterDeinit(base_); }

void McuxpressoInitiator::TransferCompleteCallback(I2C_Type* base,
                                                   i2c_master_handle_t* handle,
                                                   status_t status,
                                                   void* initiator_ptr) {
  McuxpressoInitiator& initiator =
      *static_cast<McuxpressoInitiator*>(initiator_ptr);
  initiator.callback_isl_.lock();
  initiator.transfer_status_ = status;
  initiator.callback_isl_.unlock();
  initiator.callback_complete_notification_.release();
}

Status McuxpressoInitiator::InitiateNonBlockingTransfer(
    chrono::SystemClock::duration rw_timeout, i2c_master_transfer_t* transfer) {
  const status_t status =
      I2C_MasterTransferNonBlocking(base_, &handle_, transfer);
  if (status != kStatus_Success) {
    return HalStatusToPwStatus(status);
  }

  if (!callback_complete_notification_.try_acquire_for(rw_timeout)) {
    I2C_MasterTransferAbort(base_, &handle_);
    return Status::DeadlineExceeded();
  }

  callback_isl_.lock();
  const status_t transfer_status = transfer_status_;
  callback_isl_.unlock();

  return HalStatusToPwStatus(transfer_status);
}

// Performs non-blocking I2C write, read and read-after-write depending on the
// tx and rx buffer states.
Status McuxpressoInitiator::DoWriteReadFor(
    Address device_address,
    ConstByteSpan tx_buffer,
    ByteSpan rx_buffer,
    chrono::SystemClock::duration timeout) {
  if (timeout <= chrono::SystemClock::duration::zero()) {
    return Status::DeadlineExceeded();
  }

  const uint8_t address = device_address.GetSevenBit();
  std::lock_guard lock(mutex_);

  if (!tx_buffer.empty() && rx_buffer.empty()) {
    i2c_master_transfer_t transfer{kI2C_TransferDefaultFlag,
                                   address,
                                   kI2C_Write,
                                   0,
                                   0,
                                   const_cast<std::byte*>(tx_buffer.data()),
                                   tx_buffer.size()};
    return InitiateNonBlockingTransfer(timeout, &transfer);
  } else if (tx_buffer.empty() && !rx_buffer.empty()) {
    i2c_master_transfer_t transfer{kI2C_TransferDefaultFlag,
                                   address,
                                   kI2C_Read,
                                   0,
                                   0,
                                   rx_buffer.data(),
                                   rx_buffer.size()};
    return InitiateNonBlockingTransfer(timeout, &transfer);
  } else if (!tx_buffer.empty() && !rx_buffer.empty()) {
    i2c_master_transfer_t w_transfer{kI2C_TransferNoStopFlag,
                                     address,
                                     kI2C_Write,
                                     0,
                                     0,
                                     const_cast<std::byte*>(tx_buffer.data()),
                                     tx_buffer.size()};
    const chrono::SystemClock::time_point deadline =
        chrono::SystemClock::TimePointAfterAtLeast(timeout);
    PW_TRY(InitiateNonBlockingTransfer(timeout, &w_transfer));
    i2c_master_transfer_t r_transfer{kI2C_TransferRepeatedStartFlag,
                                     address,
                                     kI2C_Read,
                                     0,
                                     0,
                                     rx_buffer.data(),
                                     rx_buffer.size()};
    const chrono::SystemClock::duration time_remaining =
        deadline - chrono::SystemClock::now();
    if (time_remaining <= chrono::SystemClock::duration::zero()) {
      // Abort transfer in an unlikely scenario of timeout even with
      // successful write.
      I2C_MasterTransferAbort(base_, &handle_);
      return Status::DeadlineExceeded();
    }
    return InitiateNonBlockingTransfer(time_remaining, &r_transfer);
  } else {
    return Status::InvalidArgument();
  }
}
// inclusive-language: enable
}  // namespace pw::i2c
