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

#include "pw_i2c/register_device.h"

#include "pw_assert/check.h"
#include "pw_bytes/byte_builder.h"

namespace pw {
namespace i2c {
namespace {

// Puts the register address data into the buffer based on the size of the
// register address.
void PutRegisterAddressInByteBuilder(
    ByteBuilder& byte_builder,
    const uint32_t register_address,
    const endian order,
    RegisterAddressSize register_address_size) {
  // TODO(b/185952662): Simplify the call site by extending the byte builder
  //                   and endian API.
  switch (register_address_size) {
    case RegisterAddressSize::k1Byte:
      byte_builder.PutUint8(static_cast<uint8_t>(register_address));
      break;

    case RegisterAddressSize::k2Bytes:
      byte_builder.PutUint16(static_cast<uint16_t>(register_address), order);
      break;

    case RegisterAddressSize::k4Bytes:
      byte_builder.PutUint32(register_address, order);
      break;

    default:
      PW_CRASH("Invalid address size being put in byte buffer");
  }
}

void PutRegisterData16InByteBuilder(ByteBuilder& byte_builder,
                                    ConstByteSpan register_data,
                                    const endian order) {
  uint32_t data_pointer_index = 0;

  while (data_pointer_index < register_data.size()) {
    const uint16_t data = *reinterpret_cast<const uint16_t*>(
        register_data.data() + data_pointer_index);
    byte_builder.PutUint16(data, order);
    data_pointer_index += sizeof(data);
  }
}

Status PutRegisterData32InByteBuilder(ByteBuilder& byte_builder,
                                      ConstByteSpan register_data,
                                      const endian order) {
  uint32_t data_pointer_index = 0;

  while (data_pointer_index < register_data.size()) {
    const uint32_t data = *reinterpret_cast<const uint32_t*>(
        register_data.data() + data_pointer_index);
    byte_builder.PutUint32(data, order);
    data_pointer_index += sizeof(data);
  }

  if (data_pointer_index == register_data.size()) {
    return pw::OkStatus();
  } else {
    // The write data that was given doesn't align with the expected register
    // data size.
    return Status::InvalidArgument();
  }
}

}  // namespace

Status RegisterDevice::WriteRegisters(const uint32_t register_address,
                                      ConstByteSpan register_data,
                                      const size_t register_data_size,
                                      ByteSpan buffer,
                                      chrono::SystemClock::duration timeout) {
  // Make sure the buffer is big enough to handle the address and data.
  if (buffer.size() <
      register_data.size() + static_cast<uint32_t>(register_address_size_)) {
    return pw::Status::OutOfRange();
  }

  ByteBuilder builder = ByteBuilder(buffer);
  PutRegisterAddressInByteBuilder(builder,
                                  register_address,
                                  register_address_order_,
                                  register_address_size_);

  switch (register_data_size) {
    case 1:
      builder.append(register_data.data(), register_data.size());
      break;

    case 2:
      PutRegisterData16InByteBuilder(builder, register_data, data_order_);
      break;

    case 4:
      PutRegisterData32InByteBuilder(builder, register_data, data_order_)
          .IgnoreError();  // TODO(b/242598609): Handle Status properly
      break;

    default:
      PW_CRASH("Invalid data size being put in byte buffer");
  }

  if (!builder.ok()) {
    return pw::Status::Internal();
  }

  ConstByteSpan write_buffer(builder.data(), builder.size());
  return WriteFor(write_buffer, timeout);
}

Status RegisterDevice::ReadRegisters(uint32_t register_address,
                                     ByteSpan return_data,
                                     chrono::SystemClock::duration timeout) {
  ByteBuffer<sizeof(register_address)> byte_buffer;

  PutRegisterAddressInByteBuilder(byte_buffer,
                                  register_address,
                                  register_address_order_,
                                  register_address_size_);

  if (!byte_buffer.ok()) {
    return pw::Status::Internal();
  }

  return WriteReadFor(byte_buffer.data(),
                      byte_buffer.size(),
                      return_data.data(),
                      return_data.size(),
                      timeout);
}

}  // namespace i2c
}  // namespace pw
