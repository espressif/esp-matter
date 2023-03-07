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
#include "pw_bluetooth_hci/packet.h"

#include "pw_bytes/byte_builder.h"
#include "pw_bytes/endian.h"
#include "pw_status/try.h"

namespace pw::bluetooth_hci {
namespace {

using pw::bytes::ReadInOrder;

}  // namespace

Result<ConstByteSpan> CommandPacket::Encode(ByteSpan buffer,
                                            endian order) const {
  ByteBuilder builder(buffer);
  builder.PutUint16(opcode_, order);
  builder.PutUint8(parameters_.size_bytes());
  builder.append(parameters_);
  PW_TRY(builder.status());
  return ConstByteSpan(builder.data(), builder.size());
}

std::optional<CommandPacket> CommandPacket::Decode(ConstByteSpan data,
                                                   endian order) {
  if (data.size_bytes() < kHeaderSizeBytes) {
    return std::nullopt;  // Not enough data to parse the packet header.
  }

  const uint8_t parameter_total_length =
      static_cast<uint8_t>(data[kParameterTotalLengthByteOffset]);
  if (data.size_bytes() < (kHeaderSizeBytes + parameter_total_length)) {
    return std::nullopt;  // Not enough data to cover the parameter bytes.
  }

  const uint16_t opcode =
      ReadInOrder<uint16_t>(order, &data[kOpcodeByteOffset]);
  return CommandPacket(opcode,
                       data.subspan(kHeaderSizeBytes, parameter_total_length));
}

Result<ConstByteSpan> AsyncDataPacket::Encode(ByteSpan buffer,
                                              endian order) const {
  ByteBuilder builder(buffer);
  builder.PutUint16(handle_and_fragmentation_bits_, order);
  builder.PutUint16(data_.size_bytes(), order);
  builder.append(data_);
  PW_TRY(builder.status());
  return ConstByteSpan(builder.data(), builder.size());
}

std::optional<AsyncDataPacket> AsyncDataPacket::Decode(ConstByteSpan data,
                                                       endian order) {
  if (data.size_bytes() < kHeaderSizeBytes) {
    return std::nullopt;  // Not enough data to parse the packet header.
  }

  const uint16_t data_total_length =
      ReadInOrder<uint16_t>(order, &data[kDataTotalLengthByteOffset]);
  if (data.size_bytes() < (kHeaderSizeBytes + data_total_length)) {
    return std::nullopt;  // Not enough data to cover the data bytes.
  }

  const uint16_t handle_and_flag_bits = ReadInOrder<uint16_t>(
      order, &data[kHandleAndFragmentationBitsByteOffset]);
  return AsyncDataPacket(handle_and_flag_bits,
                         data.subspan(kHeaderSizeBytes, data_total_length));
}

Result<ConstByteSpan> SyncDataPacket::Encode(ByteSpan buffer,
                                             endian order) const {
  ByteBuilder builder(buffer);
  builder.PutUint16(handle_and_status_bits_, order);
  builder.PutUint8(data_.size_bytes());
  builder.append(data_);
  PW_TRY(builder.status());
  return ConstByteSpan(builder.data(), builder.size());
}

std::optional<SyncDataPacket> SyncDataPacket::Decode(ConstByteSpan data,
                                                     endian order) {
  if (data.size_bytes() < kHeaderSizeBytes) {
    return std::nullopt;  // Not enough data to parse the packet header.
  }

  const uint8_t data_total_length =
      static_cast<uint8_t>(data[kDataTotalLengthByteOffset]);
  if (data.size_bytes() < (kHeaderSizeBytes + data_total_length)) {
    return std::nullopt;  // Not enough data to cover the data bytes.
  }

  const uint16_t handle_and_status_bits =
      ReadInOrder<uint16_t>(order, &data[kHandleAndStatusBitsByteOffset]);
  return SyncDataPacket(handle_and_status_bits,
                        data.subspan(kHeaderSizeBytes, data_total_length));
}

Result<ConstByteSpan> EventPacket::Encode(ByteSpan buffer) const {
  ByteBuilder builder(buffer);
  builder.PutUint8(event_code_);
  builder.PutUint8(parameters_.size_bytes());
  builder.append(parameters_);
  PW_TRY(builder.status());
  return ConstByteSpan(builder.data(), builder.size());
}

std::optional<EventPacket> EventPacket::Decode(ConstByteSpan data) {
  if (data.size_bytes() < kHeaderSizeBytes) {
    return std::nullopt;  // Not enough data to parse the packet header.
  }

  const uint8_t parameter_total_length =
      static_cast<uint8_t>(data[kParameterTotalLengthByteOffset]);
  if (data.size_bytes() < (kHeaderSizeBytes + parameter_total_length)) {
    return std::nullopt;  // Not enough data to cover the parameter bytes.
  }

  const uint8_t event_code = static_cast<uint8_t>(data[kEventCodeByteOffset]);
  return EventPacket(event_code,
                     data.subspan(kHeaderSizeBytes, parameter_total_length));
}

}  // namespace pw::bluetooth_hci
