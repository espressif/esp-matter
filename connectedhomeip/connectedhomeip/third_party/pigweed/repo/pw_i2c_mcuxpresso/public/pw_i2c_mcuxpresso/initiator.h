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

#include "fsl_i2c.h"
#include "pw_i2c/initiator.h"
#include "pw_sync/interrupt_spin_lock.h"
#include "pw_sync/lock_annotations.h"
#include "pw_sync/mutex.h"
#include "pw_sync/timed_thread_notification.h"

namespace pw::i2c {

// Initiator interface implementation based on I2C driver in NXP MCUXpresso SDK.
// Currently supports only devices with 7 bit adresses.
class McuxpressoInitiator final : public Initiator {
 public:
  McuxpressoInitiator(I2C_Type* base,
                      uint32_t baud_rate_bps,
                      uint32_t src_clock_hz);

  ~McuxpressoInitiator();

 private:
  Status DoWriteReadFor(Address device_address,
                        ConstByteSpan tx_buffer,
                        ByteSpan rx_buffer,
                        chrono::SystemClock::duration timeout) override
      PW_LOCKS_EXCLUDED(mutex_);

  // inclusive-language: disable
  Status InitiateNonBlockingTransfer(chrono::SystemClock::duration rw_timeout,
                                     i2c_master_transfer_t* transfer)
      PW_LOCKS_EXCLUDED(callback_isl_);

  // Non-blocking I2C transfer callback.
  static void TransferCompleteCallback(I2C_Type* base,
                                       i2c_master_handle_t* handle,
                                       status_t status,
                                       void* initiator_ptr)
      PW_GUARDED_BY(callback_isl_);
  // inclusive-language: enable

  sync::Mutex mutex_;
  I2C_Type* base_ PW_GUARDED_BY(mutex_);

  // Transfer completion status for non-blocking I2C transfer.
  sync::TimedThreadNotification callback_complete_notification_;
  sync::InterruptSpinLock callback_isl_;
  status_t transfer_status_ PW_GUARDED_BY(callback_isl_);

  // inclusive-language: disable
  i2c_master_handle_t handle_;
  // inclusive-language: enable
};

}  // namespace pw::i2c