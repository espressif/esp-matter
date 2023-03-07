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

#include "pw_bytes/endian.h"
#include "pw_bytes/span.h"
#include "pw_chrono/system_clock.h"
#include "pw_i2c/address.h"
#include "pw_i2c/device.h"
#include "pw_i2c/initiator.h"
#include "pw_result/result.h"
#include "pw_status/status.h"
#include "pw_status/try.h"

namespace pw {
namespace i2c {

enum class RegisterAddressSize {
  k1Byte = 1,
  k2Bytes = 2,
  k4Bytes = 4,
};

// RegisterDevice is used to write/read registers, chunks of data
// or just an array of bytes over a bus to a device.
//
// DISCLAIMER:
// It is important to note that bulk write/read may not be supported for every
// device and that it's up to the user to know the capabilities of their device.
// Users should also be aware of the register and address size and use the
// appropriate methods for their device.
//
//  - WriteRegisters*
//       Write to a set of registers starting at a specific address/offset.
//       Endianness will be applied to data that's read or written.
//
//  - WriteRegister*
//       Write data to a register where the max register size is 4 bytes.
//       Endianness will be applied to data that's read or written.
//
//  - ReadRegisters*
//       Read a set of registers starting at a specific address/offset.
//       Endianness will be applied to data that's read or written.
//
//  - ReadRegister*
//       Read data to a register where the max register size is 4 bytes.
//       Endianness will be applied to data that's read or written.
class RegisterDevice : public Device {
 public:
  // Args:
  //   initiator: I2C initiator for the bus the device is on.
  //   address: I2C device address.
  //   register_address_order: Endianness of the register address.
  //   data_order: Endianness of the data.
  //   register_address_size: Size of the register address.
  constexpr RegisterDevice(Initiator& initiator,
                           Address address,
                           endian register_address_order,
                           endian data_order,
                           RegisterAddressSize register_address_size)
      : Device(initiator, address),
        register_address_order_(register_address_order),
        data_order_(data_order),
        register_address_size_(register_address_size) {}

  // Args:
  //   initiator: I2C initiator for the bus the device is on.
  //   address: I2C device address.
  //   order: Endianness of the register address and data.
  //   register_address_size: Size of the register address.
  constexpr RegisterDevice(Initiator& initiator,
                           Address address,
                           endian order,
                           RegisterAddressSize register_address_size)
      : Device(initiator, address),
        register_address_order_(order),
        data_order_(order),
        register_address_size_(register_address_size) {}

  // Writes data to multiple contiguous registers starting at specific register.
  // WriteRegisters has byte addressable capabilities and it is up to the user
  // to determine the appropriate size based on the features of the device. The
  // amount of data to write is the size of the span. Endianness is taken into
  // account if register_data_size is 2 bytes or 4 bytes. Both address and
  // data will use the same endianness provided by the constructor.
  //
  // It is important to note that bulk write may not be supported for every
  // device and that it's up to the user to know the capabilities of their
  // device. Args:
  //   register_address: Register address to send.
  //   register_data: Data to write.
  //   buffer: Since we need a buffer to construct the write data that consists
  //           of the register address and the register data, the buffer should
  //           be big enough such that the two can be concatenated.
  //   timeout: timeout that's used for both lock and transaction.
  // Returns:
  //   Ok: Successful.
  //   DeadlineExceeded: Unable to acquire exclusive Initiator access and
  //                     complete the I2C transaction in time.
  //   FailedPrecondition: Interface is not initialized and/or enabled.
  //   Internal: Building data for the write buffer has an issue.
  //   InvalidArgument: Device_address is larger than the 10 bit address space.
  //   OutOfRange: if buffer size is too small for data and register_address.
  //   Unavailable: if NACK and device did not respond in time.
  Status WriteRegisters(uint32_t register_address,
                        ConstByteSpan register_data,
                        ByteSpan buffer,
                        chrono::SystemClock::duration timeout);

  Status WriteRegisters8(uint32_t register_address,
                         span<const uint8_t> register_data,
                         ByteSpan buffer,
                         chrono::SystemClock::duration timeout);

  Status WriteRegisters16(uint32_t register_address,
                          span<const uint16_t> register_data,
                          ByteSpan buffer,
                          chrono::SystemClock::duration timeout);

  Status WriteRegisters32(uint32_t register_address,
                          span<const uint32_t> register_data,
                          ByteSpan buffer,
                          chrono::SystemClock::duration timeout);

  // Reads data chunk starting at specific offset or register.
  // ReadRegisters has byte addressable capabilities and it is up to the user
  // to determine the appropriate size based on the features of the device. The
  // amount of data to read is the size of the span. Endianness is taken into
  // account for the *16 and *32 bit methods.  Both address and data will use
  // the same endianness provided by the constructor.
  //
  // It is important to note that bulk read may not be supported for every
  // device and that it's up to the user to know the capabilities of their
  // device. Args:
  //   register_address: Register address to send.
  //   return_data: Area to read data to.
  //   timeout: Timeout that's used for both lock and transaction.
  // Returns:
  //   Ok: Successful.
  //   DeadlineExceeded: Unable to acquire exclusive Initiator access and
  //                     complete the I2C transaction in time.
  //   FailedPrecondition: Interface is not initialized and/or enabled.
  //   Internal: Building data for the write buffer has an issue.
  //   InvalidArgument: Device_address is larger than the 10 bit address space.
  //   Unavailable: if NACK and device did not respond in time.
  Status ReadRegisters(uint32_t register_address,
                       ByteSpan return_data,
                       chrono::SystemClock::duration timeout);

  Status ReadRegisters8(uint32_t register_address,
                        span<uint8_t> return_data,
                        chrono::SystemClock::duration timeout);

  Status ReadRegisters16(uint32_t register_address,
                         span<uint16_t> return_data,
                         chrono::SystemClock::duration timeout);

  Status ReadRegisters32(uint32_t register_address,
                         span<uint32_t> return_data,
                         chrono::SystemClock::duration timeout);

  // Writes the register address first before data.
  // User should be careful which WriteRegister* API is used and should use
  // the one that matches their register data size if not byte addressable.
  //
  // Both address and data will use the same endianness provided by the
  // constructor.
  // Args:
  //   register_address: Register address to send.
  //   register_data: Data to write.
  //   timeout: Timeout that's used for both lock and transaction.
  // Returns:
  //   Ok: Successful.
  //   DeadlineExceeded: Unable to acquire exclusive Initiator access and
  //                     complete the I2C transaction in time.
  //   FailedPrecondition: Interface is not initialized and/or enabled.
  //   Internal: Building data for the write buffer has an issue.
  //   InvalidArgument: Device_address is larger than the 10 bit address space.
  //   Unavailable: if NACK and device did not respond in time.
  Status WriteRegister(uint32_t register_address,
                       std::byte register_data,
                       chrono::SystemClock::duration timeout);

  Status WriteRegister8(uint32_t register_address,
                        uint8_t register_data,
                        chrono::SystemClock::duration timeout);

  Status WriteRegister16(uint32_t register_address,
                         uint16_t register_data,
                         chrono::SystemClock::duration timeout);

  Status WriteRegister32(uint32_t register_address,
                         uint32_t register_data,
                         chrono::SystemClock::duration timeout);

  // Reads data from the device after sending the register address first.
  // User should be careful which ReadRegister* API is used and should use
  // the one that matches their register data size if not byte addressable.
  //
  // Both address and data will use the same endianness provided by the
  // constructor.
  // Args:
  //   register_address: Register address to send.
  //   timeout: Timeout that's used for both lock and transaction.
  // Returns:
  //   Ok: Successful.
  //   DeadlineExceeded: Unable to acquire exclusive Initiator access and
  //                     complete the I2C transaction in time.
  //   FailedPrecondition: Interface is not initialized and/or enabled.
  //   Internal: Building data for the write buffer has an issue.
  //   InvalidArgument: Device_address is larger than the 10 bit address space.
  //   Unavailable: if NACK and device did not respond in time.
  Result<std::byte> ReadRegister(uint32_t register_address,
                                 chrono::SystemClock::duration timeout);

  Result<uint8_t> ReadRegister8(uint32_t register_address,
                                chrono::SystemClock::duration timeout);

  Result<uint16_t> ReadRegister16(uint32_t register_address,
                                  chrono::SystemClock::duration timeout);

  Result<uint32_t> ReadRegister32(uint32_t register_address,
                                  chrono::SystemClock::duration timeout);

 private:
  // Helper write registers.
  Status WriteRegisters(uint32_t register_address,
                        ConstByteSpan register_data,
                        const size_t register_data_size,
                        ByteSpan buffer,
                        chrono::SystemClock::duration timeout);

  const endian register_address_order_;
  const endian data_order_;
  const RegisterAddressSize register_address_size_;
};

inline Status RegisterDevice::WriteRegisters(
    uint32_t register_address,
    ConstByteSpan register_data,
    ByteSpan buffer,
    chrono::SystemClock::duration timeout) {
  return WriteRegisters(register_address,
                        register_data,
                        sizeof(decltype(register_data)::value_type),
                        buffer,
                        timeout);
}

inline Status RegisterDevice::WriteRegisters8(
    uint32_t register_address,
    span<const uint8_t> register_data,
    ByteSpan buffer,
    chrono::SystemClock::duration timeout) {
  return WriteRegisters(register_address,
                        as_bytes(register_data),
                        sizeof(decltype(register_data)::value_type),
                        buffer,
                        timeout);
}

inline Status RegisterDevice::WriteRegisters16(
    uint32_t register_address,
    span<const uint16_t> register_data,
    ByteSpan buffer,
    chrono::SystemClock::duration timeout) {
  return WriteRegisters(register_address,
                        as_bytes(register_data),
                        sizeof(decltype(register_data)::value_type),
                        buffer,
                        timeout);
}

inline Status RegisterDevice::WriteRegisters32(
    uint32_t register_address,
    span<const uint32_t> register_data,
    ByteSpan buffer,
    chrono::SystemClock::duration timeout) {
  return WriteRegisters(register_address,
                        as_bytes(register_data),
                        sizeof(decltype(register_data)::value_type),
                        buffer,
                        timeout);
}

inline Status RegisterDevice::WriteRegister(
    uint32_t register_address,
    std::byte register_data,
    chrono::SystemClock::duration timeout) {
  std::array<std::byte, sizeof(register_data) + sizeof(register_address)>
      byte_buffer;
  return WriteRegisters(register_address,
                        span(&register_data, 1),
                        sizeof(register_data),
                        byte_buffer,
                        timeout);
}

inline Status RegisterDevice::WriteRegister8(
    uint32_t register_address,
    uint8_t register_data,
    chrono::SystemClock::duration timeout) {
  std::array<std::byte, sizeof(register_data) + sizeof(register_address)>
      byte_buffer;
  return WriteRegisters(register_address,
                        as_bytes(span(&register_data, 1)),
                        sizeof(register_data),
                        byte_buffer,
                        timeout);
}

inline Status RegisterDevice::WriteRegister16(
    uint32_t register_address,
    uint16_t register_data,
    chrono::SystemClock::duration timeout) {
  std::array<std::byte, sizeof(register_data) + sizeof(register_address)>
      byte_buffer;
  return WriteRegisters(register_address,
                        as_bytes(span(&register_data, 1)),
                        sizeof(register_data),
                        byte_buffer,
                        timeout);
}

inline Status RegisterDevice::WriteRegister32(
    uint32_t register_address,
    uint32_t register_data,
    chrono::SystemClock::duration timeout) {
  std::array<std::byte, sizeof(register_data) + sizeof(register_address)>
      byte_buffer;
  return WriteRegisters(register_address,
                        as_bytes(span(&register_data, 1)),
                        sizeof(register_data),
                        byte_buffer,
                        timeout);
}

inline Status RegisterDevice::ReadRegisters8(
    uint32_t register_address,
    span<uint8_t> return_data,
    chrono::SystemClock::duration timeout) {
  // For a single byte, there's no endian data, and so we can return the
  // data as is.
  return ReadRegisters(
      register_address, as_writable_bytes(return_data), timeout);
}

inline Status RegisterDevice::ReadRegisters16(
    uint32_t register_address,
    span<uint16_t> return_data,
    chrono::SystemClock::duration timeout) {
  PW_TRY(
      ReadRegisters(register_address, as_writable_bytes(return_data), timeout));

  // Post process endian information.
  for (uint16_t& register_value : return_data) {
    register_value = bytes::ReadInOrder<uint16_t>(data_order_, &register_value);
  }

  return pw::OkStatus();
}

inline Status RegisterDevice::ReadRegisters32(
    uint32_t register_address,
    span<uint32_t> return_data,
    chrono::SystemClock::duration timeout) {
  PW_TRY(
      ReadRegisters(register_address, as_writable_bytes(return_data), timeout));

  // TODO(b/185952662): Extend endian in pw_byte to support this conversion
  //                    as optimization.
  // Post process endian information.
  for (uint32_t& register_value : return_data) {
    register_value = bytes::ReadInOrder<uint32_t>(data_order_, &register_value);
  }

  return pw::OkStatus();
}

inline Result<std::byte> RegisterDevice::ReadRegister(
    uint32_t register_address, chrono::SystemClock::duration timeout) {
  std::byte data = {};
  PW_TRY(ReadRegisters(register_address, span(&data, 1), timeout));
  return data;
}

inline Result<uint8_t> RegisterDevice::ReadRegister8(
    uint32_t register_address, chrono::SystemClock::duration timeout) {
  uint8_t data = 0;
  PW_TRY(ReadRegisters8(register_address, span(&data, 1), timeout));
  return data;
}

inline Result<uint16_t> RegisterDevice::ReadRegister16(
    uint32_t register_address, chrono::SystemClock::duration timeout) {
  std::array<uint16_t, 1> data = {};
  PW_TRY(ReadRegisters16(register_address, data, timeout));
  return data[0];
}

inline Result<uint32_t> RegisterDevice::ReadRegister32(
    uint32_t register_address, chrono::SystemClock::duration timeout) {
  std::array<uint32_t, 1> data = {};
  PW_TRY(ReadRegisters32(register_address, data, timeout));
  return data[0];
}

}  // namespace i2c
}  // namespace pw

// TODO (zengk): Register modification.
