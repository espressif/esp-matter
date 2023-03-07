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

#include <cstdint>

#include "pw_bytes/span.h"
#include "pw_chrono/system_clock.h"
#include "pw_i2c/address.h"
#include "pw_status/status.h"

namespace pw::i2c {

// Base driver interface for I2C initiating I2C transactions in a thread safe
// manner. Other documentation sources may call this style of interface an I2C
// "master", "central" or "controller".  // inclusive-language: ignore
//
// The Initiator is not required to support 10bit addressing. If only 7bit
// addressing is supported, the Initiator will assert when given an address
// which is out of 7bit address range.
//
// The implementer of this pure virtual interface is responsible for ensuring
// thread safety and enabling functionality such as initialization,
// configuration, enabling/disabling, unsticking SDA, and detecting device
// address registration collisions.
class Initiator {
 public:
  virtual ~Initiator() = default;

  // Write bytes and then read bytes as either one atomic or two independent I2C
  // transaction. If the I2C bus is a multi-initiator bus then the implementer
  // MUST ensure it is a single atomic I2C transaction.
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
  Status WriteReadFor(Address device_address,
                      ConstByteSpan tx_buffer,
                      ByteSpan rx_buffer,
                      chrono::SystemClock::duration timeout) {
    return DoWriteReadFor(device_address, tx_buffer, rx_buffer, timeout);
  }
  Status WriteReadFor(Address device_address,
                      const void* tx_buffer,
                      size_t tx_size_bytes,
                      void* rx_buffer,
                      size_t rx_size_bytes,
                      chrono::SystemClock::duration timeout) {
    return WriteReadFor(
        device_address,
        span(static_cast<const std::byte*>(tx_buffer), tx_size_bytes),
        span(static_cast<std::byte*>(rx_buffer), rx_size_bytes),
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
  Status WriteFor(Address device_address,
                  ConstByteSpan tx_buffer,
                  chrono::SystemClock::duration timeout) {
    return WriteReadFor(device_address, tx_buffer, ByteSpan(), timeout);
  }
  Status WriteFor(Address device_address,
                  const void* tx_buffer,
                  size_t tx_size_bytes,
                  chrono::SystemClock::duration timeout) {
    return WriteFor(
        device_address,
        span(static_cast<const std::byte*>(tx_buffer), tx_size_bytes),
        timeout);
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
  Status ReadFor(Address device_address,
                 ByteSpan rx_buffer,
                 chrono::SystemClock::duration timeout) {
    return WriteReadFor(device_address, ConstByteSpan(), rx_buffer, timeout);
  }
  Status ReadFor(Address device_address,
                 void* rx_buffer,
                 size_t rx_size_bytes,
                 chrono::SystemClock::duration timeout) {
    return ReadFor(device_address,
                   span(static_cast<std::byte*>(rx_buffer), rx_size_bytes),
                   timeout);
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
  Status ProbeDeviceFor(Address device_address,
                        chrono::SystemClock::duration timeout) {
    std::byte ignored_buffer[1] = {};  // Read a byte to probe.
    return WriteReadFor(
        device_address, ConstByteSpan(), ignored_buffer, timeout);
  }

 private:
  virtual Status DoWriteReadFor(Address device_address,
                                ConstByteSpan tx_buffer,
                                ByteSpan rx_buffer,
                                chrono::SystemClock::duration timeout) = 0;
};

}  // namespace pw::i2c
