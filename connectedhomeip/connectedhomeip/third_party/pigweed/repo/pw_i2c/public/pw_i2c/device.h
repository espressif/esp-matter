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
//
#pragma once

#include "pw_bytes/span.h"
#include "pw_chrono/system_clock.h"
#include "pw_i2c/address.h"
#include "pw_i2c/initiator.h"
#include "pw_status/status.h"

namespace pw {
namespace i2c {

// Device is used to write/read arbitrary chunks of data over a bus to a device.
// This object essentially just wrap the Initiator API with a fixed I2C device
// address.
class Device {
 public:
  constexpr Device(Initiator& initiator, Address device_address)
      : initiator_(initiator), device_address_(device_address) {}

  Device(const Device&) = delete;
  ~Device() = default;

  // Write bytes and then read bytes as either one atomic or two independent I2C
  // transaction.
  // The signal on the bus should appear as follows:
  // 1) Write Only:
  //   START + I2C Address + WRITE(0) + TX_BUFFER_BYTES + STOP
  // 2) Read Only:
  //   START + I2C Address + READ(1) + RX_BUFFER_BYTES + STOP
  // 3A) Write + Read (atomic):
  //   START + I2C Address + WRITE(0) + TX_BUFFER_BYTES +
  //   START + I2C Address + READ(1) + RX_BUFFER_BYTES + STOP
  // 3B) Write + Read (separate):
  //   START + I2C Address + WRITE(0) + TX_BUFFER_BYTES + STOP
  //   START + I2C Address + READ(1) + RX_BUFFER_BYTES + STOP
  //
  // The timeout defines the minimum duration one may block waiting for both
  // exclusive bus access and the completion of the I2C transaction.
  //
  // Preconditions:
  // The Address must be supported by the Initiator, i.e. do not use a 10
  //     address if the Initiator only supports 7 bit. This will assert.
  //
  // Returns:
  // Ok - Success.
  // InvalidArgument - device_address is larger than the 10 bit address space.
  // DeadlineExceeded - Was unable to acquire exclusive Initiator access
  //   and complete the I2C transaction in time.
  // Unavailable - NACK condition occurred, meaning the addressed device did
  //   not respond or was unable to process the request.
  // FailedPrecondition - The interface is not currently initialized and/or
  //    enabled.
  Status WriteReadFor(ConstByteSpan tx_buffer,
                      ByteSpan rx_buffer,
                      chrono::SystemClock::duration timeout) {
    return initiator_.WriteReadFor(
        device_address_, tx_buffer, rx_buffer, timeout);
  }
  Status WriteReadFor(const void* tx_buffer,
                      size_t tx_size_bytes,
                      void* rx_buffer,
                      size_t rx_size_bytes,
                      chrono::SystemClock::duration timeout) {
    return initiator_.WriteReadFor(device_address_,
                                   tx_buffer,
                                   tx_size_bytes,
                                   rx_buffer,
                                   rx_size_bytes,
                                   timeout);
  }

  // Write bytes. The signal on the bus should appear as follows:
  //   START + I2C Address + WRITE(0) + TX_BUFFER_BYTES + STOP
  //
  // The timeout defines the minimum duration one may block waiting for both
  // exclusive bus access and the completion of the I2C transaction.
  //
  // Preconditions:
  // The Address must be supported by the Initiator, i.e. do not use a 10
  //     address if the Initiator only supports 7 bit. This will assert.
  //
  // Returns:
  // Ok - Success.
  // InvalidArgument - device_address is larger than the 10 bit address space.
  // DeadlineExceeded - Was unable to acquire exclusive Initiator access
  //   and complete the I2C transaction in time.
  // Unavailable - NACK condition occurred, meaning the addressed device did
  //   not respond or was unable to process the request.
  // FailedPrecondition - The interface is not currently initialized and/or
  //    enabled.
  Status WriteFor(ConstByteSpan tx_buffer,
                  chrono::SystemClock::duration timeout) {
    return initiator_.WriteFor(device_address_, tx_buffer, timeout);
  }
  Status WriteFor(const void* tx_buffer,
                  size_t tx_size_bytes,
                  chrono::SystemClock::duration timeout) {
    return initiator_.WriteFor(
        device_address_, tx_buffer, tx_size_bytes, timeout);
  }

  // Read bytes. The signal on the bus should appear as follows:
  //   START + I2C Address + READ(1) + RX_BUFFER_BYTES + STOP
  //
  // The timeout defines the minimum duration one may block waiting for both
  // exclusive bus access and the completion of the I2C transaction.
  //
  // Preconditions:
  // The Address must be supported by the Initiator, i.e. do not use a 10
  //     address if the Initiator only supports 7 bit. This will assert.
  //
  // Returns:
  // Ok - Success.
  // InvalidArgument - device_address is larger than the 10 bit address space.
  // DeadlineExceeded - Was unable to acquire exclusive Initiator access
  //   and complete the I2C transaction in time.
  // Unavailable - NACK condition occurred, meaning the addressed device did
  //   not respond or was unable to process the request.
  // FailedPrecondition - The interface is not currently initialized and/or
  //    enabled.
  Status ReadFor(ByteSpan rx_buffer, chrono::SystemClock::duration timeout) {
    return initiator_.ReadFor(device_address_, rx_buffer, timeout);
  }
  Status ReadFor(void* rx_buffer,
                 size_t rx_size_bytes,
                 chrono::SystemClock::duration timeout) {
    return initiator_.ReadFor(
        device_address_, rx_buffer, rx_size_bytes, timeout);
  }

  // Probes the device for an I2C ACK after only writing the address.
  // This is done by attempting to read a single byte from the specified device.
  //
  // The timeout defines the minimum duration one may block waiting for both
  // exclusive bus access and the completion of the I2C transaction.
  //
  // Preconditions:
  // The Address must be supported by the Initiator, i.e. do not use a 10
  //     address if the Initiator only supports 7 bit. This will assert.
  //
  // Returns:
  // Ok - Success.
  // InvalidArgument - device_address is larger than the 10 bit address space.
  // DeadlineExceeded - Was unable to acquire exclusive Initiator access
  //   and complete the I2C transaction in time.
  // Unavailable - NACK condition occurred, meaning the addressed device did
  //   not respond or was unable to process the request.
  // FailedPrecondition - The interface is not currently initialized and/or
  //    enabled.
  Status ProbeFor(chrono::SystemClock::duration timeout) {
    return initiator_.ProbeDeviceFor(device_address_, timeout);
  }

 private:
  Initiator& initiator_;
  const Address device_address_;
};

}  // namespace i2c
}  // namespace pw
