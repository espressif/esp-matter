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

#include <cstdint>
#include <optional>

#include "pw_assert/assert.h"
#include "pw_bytes/bit.h"
#include "pw_bytes/span.h"
#include "pw_result/result.h"

namespace pw::bluetooth_hci {

// HCI Packets as defined in the Bluetooth Core Specification Version 5.3
// “Host Controller Interface Functional Specification” in Volume 2, Part E.
//
// Note that for now only the subset of the HCI packets used in the HCI UART
// Transport Layer are provided as defined in the Bluetooth Core Specification
// version 5.3 "Host Controller Interface Transport Layer" volume 4, part A.
class CommandPacket;
class AsyncDataPacket;
class SyncDataPacket;
class EventPacket;

class Packet {
 public:
  enum class Type {
    kCommandPacket,
    kAsyncDataPacket,
    kSyncDataPacket,
    kEventPacket,
  };

  constexpr Type type() const { return type_; }

  constexpr size_t size_bytes() const { return packet_size_bytes_; }

  const CommandPacket& command_packet() const {
    PW_ASSERT(type_ == Type::kCommandPacket);
    return *reinterpret_cast<const CommandPacket*>(this);
  }

  CommandPacket& command_packet() {
    PW_ASSERT(type_ == Type::kCommandPacket);
    return *reinterpret_cast<CommandPacket*>(this);
  }

  const AsyncDataPacket& async_data_packet() const {
    PW_ASSERT(type_ == Type::kAsyncDataPacket);
    return *reinterpret_cast<const AsyncDataPacket*>(this);
  }

  AsyncDataPacket& async_data_packet() {
    PW_ASSERT(type_ == Type::kAsyncDataPacket);
    return *reinterpret_cast<AsyncDataPacket*>(this);
  }

  const SyncDataPacket& sync_data_packet() const {
    PW_ASSERT(type_ == Type::kSyncDataPacket);
    return *reinterpret_cast<const SyncDataPacket*>(this);
  }

  SyncDataPacket& sync_data_packet() {
    PW_ASSERT(type_ == Type::kSyncDataPacket);
    return *reinterpret_cast<SyncDataPacket*>(this);
  }

  const EventPacket& event_packet() const {
    PW_ASSERT(type_ == Type::kEventPacket);
    return *reinterpret_cast<const EventPacket*>(this);
  }

  EventPacket& event_packet() {
    PW_ASSERT(type_ == Type::kEventPacket);
    return *reinterpret_cast<EventPacket*>(this);
  }

 protected:
  constexpr Packet(Type type, size_t packet_size_bytes)
      : type_(type), packet_size_bytes_(packet_size_bytes) {}

 private:
  Type type_;
  size_t packet_size_bytes_;
};

class CommandPacket : public Packet {
 private:
  static constexpr size_t kOpcodeByteOffset = 0;
  static constexpr size_t kParameterTotalLengthByteOffset = 2;
  static constexpr size_t kParametersByteOffset = 3;

  static constexpr size_t kOpcodeOcfOffset = 0;
  static constexpr uint16_t kOpcodeOcfMask = 0x3FF << kOpcodeOcfOffset;
  static constexpr size_t kOpcodeOgfOffset = 10;
  static constexpr uint16_t kOpcodeOgfMask = 0x3F << kOpcodeOgfOffset;

 public:
  // HCI Command Packet Format, little-endian, based on bit offsets:
  // 0           16                        24            24+8*N
  // |  Opcode   |  Parameter Total Length | Parameter N |
  // 0     10    16
  // | OCF | OGF |
  static constexpr size_t kHeaderSizeBytes = kParametersByteOffset;

  constexpr CommandPacket(uint16_t opcode,
                          const std::byte* parameters,
                          uint8_t parameters_size_bytes)
      : Packet(Type::kCommandPacket, kHeaderSizeBytes + parameters_size_bytes),
        opcode_(opcode),
        parameters_(parameters, parameters_size_bytes) {}

  // Precondition: the parameters size must be <= 255 bytes.
  constexpr CommandPacket(uint16_t opcode, ConstByteSpan parameters)
      : CommandPacket(opcode,
                      parameters.data(),
                      static_cast<uint8_t>(parameters.size_bytes())) {
    PW_ASSERT(parameters.size_bytes() <= std::numeric_limits<uint8_t>::max());
  }

  // Decodes the packet based on the specified endianness.
  static std::optional<CommandPacket> Decode(ConstByteSpan data,
                                             endian order = endian::little);

  // Encodes the packet based on the specified endianness.
  //
  // Returns:
  //   OK - returns the encoded packet.
  //   RESOURCE_EXHAUSTED - The input buffer is too small for this packet.
  Result<ConstByteSpan> Encode(ByteSpan buffer,
                               endian order = endian::little) const;

  constexpr uint16_t opcode() const { return opcode_; }

  constexpr uint16_t opcode_command_field() const {
    return static_cast<uint16_t>((opcode_ & kOpcodeOcfMask) >>
                                 kOpcodeOcfOffset);
  }

  constexpr uint8_t opcode_group_field() const {
    return static_cast<uint8_t>((opcode_ & kOpcodeOgfMask) >> kOpcodeOgfOffset);
  }

  constexpr const ConstByteSpan& parameters() const { return parameters_; }

 private:
  uint16_t opcode_;
  ConstByteSpan parameters_;
};

class AsyncDataPacket : public Packet {
 private:
  static constexpr size_t kHandleAndFragmentationBitsByteOffset = 0;
  static constexpr size_t kDataTotalLengthByteOffset = 2;
  static constexpr size_t kDataByteOffset = 4;

  static constexpr size_t kHandleOffset = 0;
  static constexpr uint16_t kHandleMask = 0xFFF << kHandleOffset;
  static constexpr size_t kPbFlagOffset = 12;
  static constexpr uint16_t kPbFlagMask = 0x3 << kPbFlagOffset;
  static constexpr size_t kBcFlagOffset = 14;
  static constexpr uint16_t kBcFlagMask = 0x3 << kBcFlagOffset;

 public:
  // HCI ACL Data Packet Format, little-endian, based on bit offsets:
  // 0        12        14        16                  32       32+8*N
  // | Handle | PB Flag | BC Flag | Data Total Length | Data N |
  static constexpr size_t kHeaderSizeBytes = kDataByteOffset;

  constexpr AsyncDataPacket(uint16_t handle_and_fragmentation_bits,
                            const std::byte* data,
                            uint16_t data_size_bytes)
      : Packet(Type::kAsyncDataPacket, kHeaderSizeBytes + data_size_bytes),
        handle_and_fragmentation_bits_(handle_and_fragmentation_bits),
        data_(data, data_size_bytes) {}

  // Precondition: the parameters size must be <= 65535 bytes.
  constexpr AsyncDataPacket(uint16_t handle_and_fragmentation_bits,
                            ConstByteSpan data)
      : AsyncDataPacket(handle_and_fragmentation_bits,
                        data.data(),
                        static_cast<uint16_t>(data.size_bytes())) {
    PW_ASSERT(data.size_bytes() <= std::numeric_limits<uint16_t>::max());
  }

  // Decodes the packet based on the specified endianness.
  static std::optional<AsyncDataPacket> Decode(ConstByteSpan data,
                                               endian order = endian::little);

  // Encodes the packet based on the specified endianness.
  //
  // Returns:
  //   OK - returns the encoded packet.
  //   RESOURCE_EXHAUSTED - The input buffer is too small for this packet.
  Result<ConstByteSpan> Encode(ByteSpan buffer,
                               endian order = endian::little) const;

  constexpr uint16_t handle_and_fragmentation_bits() const {
    return handle_and_fragmentation_bits_;
  }

  constexpr uint16_t handle() const {
    return (handle_and_fragmentation_bits_ & kHandleMask) >> kHandleOffset;
  }

  constexpr uint8_t pb_flag() const {
    return static_cast<uint8_t>(
        (handle_and_fragmentation_bits_ & kPbFlagMask) >> kPbFlagOffset);
  }

  constexpr uint8_t bc_flag() const {
    return static_cast<uint8_t>(
        (handle_and_fragmentation_bits_ & kBcFlagMask) >> kBcFlagOffset);
  }

  constexpr const ConstByteSpan& data() const { return data_; }

 private:
  uint16_t handle_and_fragmentation_bits_;
  ConstByteSpan data_;
};

class SyncDataPacket : public Packet {
 private:
  static constexpr size_t kHandleAndStatusBitsByteOffset = 0;
  static constexpr size_t kDataTotalLengthByteOffset = 2;
  static constexpr size_t kDataByteOffset = 3;

  static constexpr size_t kHandleOffset = 0;
  static constexpr uint16_t kHandleMask = 0xFFF << kHandleOffset;
  static constexpr size_t kPacketStatusFlagOffset = 12;
  static constexpr uint16_t kPacketStatusFlagMask = 0x3
                                                    << kPacketStatusFlagOffset;

 public:
  // HCI SCO Data Packet Format, little-endian, based on bit offsets:
  // 0        12                   14         16                  24      24+8*N
  // | Handle | Packet Status Flag | Reserved | Data Total Length | Data N |
  static constexpr size_t kHeaderSizeBytes = 3;

  constexpr SyncDataPacket(uint16_t handle_and_status_bits,
                           const std::byte* data,
                           uint8_t data_size_bytes)
      : Packet(Type::kSyncDataPacket, kHeaderSizeBytes + data_size_bytes),
        handle_and_status_bits_(handle_and_status_bits),
        data_(data, data_size_bytes) {}

  // Precondition: the parameters size must be <= 255 bytes.
  constexpr SyncDataPacket(uint16_t handle_and_status_bits, ConstByteSpan data)
      : SyncDataPacket(handle_and_status_bits,
                       data.data(),
                       static_cast<uint8_t>(data.size_bytes())) {
    PW_ASSERT(data.size_bytes() <= std::numeric_limits<uint8_t>::max());
  }

  // Decodes the packet based on the specified endianness.
  static std::optional<SyncDataPacket> Decode(ConstByteSpan data,
                                              endian order = endian::little);

  // Encodes the packet based on the specified endianness.
  //
  // Returns:
  //   OK - returns the encoded packet.
  //   RESOURCE_EXHAUSTED - The input buffer is too small for this packet.
  Result<ConstByteSpan> Encode(ByteSpan buffer,
                               endian order = endian::little) const;

  constexpr uint16_t handle_and_status_bits() const {
    return handle_and_status_bits_;
  }

  constexpr uint16_t handle() const {
    return (handle_and_status_bits_ & kHandleMask) >> kHandleOffset;
  }

  constexpr uint8_t packet_status_flag() const {
    return static_cast<uint8_t>(
        (handle_and_status_bits_ & kPacketStatusFlagMask) >>
        kPacketStatusFlagOffset);
  }

  constexpr const ConstByteSpan& data() const { return data_; }

 private:
  uint16_t handle_and_status_bits_;
  ConstByteSpan data_;
};

class EventPacket : public Packet {
 private:
  static constexpr size_t kEventCodeByteOffset = 0;
  static constexpr size_t kParameterTotalLengthByteOffset = 1;
  static constexpr size_t kParametersByteOffset = 2;

 public:
  // HCI SCO Data Packet Format, little-endian, based on bit offsets:
  // 0            8                        16            16+8*N
  // | Event Code | Parameter Total Length | Parameter N |
  static constexpr size_t kHeaderSizeBytes = kParametersByteOffset;

  constexpr EventPacket(uint8_t event_code,
                        const std::byte* parameters,
                        uint8_t parameters_size_bytes)
      : Packet(Type::kEventPacket, kHeaderSizeBytes + parameters_size_bytes),
        event_code_(event_code),
        parameters_(parameters, parameters_size_bytes) {}

  // Precondition: the parameters size must be <= 255 bytes.
  constexpr EventPacket(uint16_t event_code, ConstByteSpan parameters)
      : EventPacket(event_code,
                    parameters.data(),
                    static_cast<uint8_t>(parameters.size_bytes())) {
    PW_ASSERT(parameters.size_bytes() <= std::numeric_limits<uint8_t>::max());
  }

  // Decodes the packet based on the specified endianness.
  static std::optional<EventPacket> Decode(ConstByteSpan data);

  // Encodes the packet based on the specified endianness.
  //
  // Returns:
  //   OK - returns the encoded packet.
  //   RESOURCE_EXHAUSTED - The input buffer is too small for this packet.
  Result<ConstByteSpan> Encode(ByteSpan buffer) const;

  constexpr uint8_t event_code() const { return event_code_; }

  constexpr const ConstByteSpan& parameters() const { return parameters_; }

 private:
  uint8_t event_code_;
  ConstByteSpan parameters_;
};

}  // namespace pw::bluetooth_hci
