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

#include "pw_rpc/internal/packet.h"

#include "pw_protobuf/decoder.h"

namespace pw::rpc::internal {

using pwpb::PacketType;

namespace RpcPacket = pwpb::RpcPacket;

Result<Packet> Packet::FromBuffer(ConstByteSpan data) {
  Packet packet;
  Status status;
  protobuf::Decoder decoder(data);

  while ((status = decoder.Next()).ok()) {
    RpcPacket::Fields field =
        static_cast<RpcPacket::Fields>(decoder.FieldNumber());

    switch (field) {
      case RpcPacket::Fields::TYPE: {
        uint32_t value;
        // A decode error will propagate from Next() and terminate the loop.
        decoder.ReadUint32(&value).IgnoreError();
        packet.set_type(static_cast<PacketType>(value));
        break;
      }

      case RpcPacket::Fields::CHANNEL_ID:
        // A decode error will propagate from Next() and terminate the loop.
        decoder.ReadUint32(&packet.channel_id_).IgnoreError();
        break;

      case RpcPacket::Fields::SERVICE_ID:
        // A decode error will propagate from Next() and terminate the loop.
        decoder.ReadFixed32(&packet.service_id_).IgnoreError();
        break;

      case RpcPacket::Fields::METHOD_ID:
        // A decode error will propagate from Next() and terminate the loop.
        decoder.ReadFixed32(&packet.method_id_).IgnoreError();
        break;

      case RpcPacket::Fields::PAYLOAD:
        // A decode error will propagate from Next() and terminate the loop.
        decoder.ReadBytes(&packet.payload_).IgnoreError();
        break;

      case RpcPacket::Fields::STATUS: {
        uint32_t value;
        // A decode error will propagate from Next() and terminate the loop.
        decoder.ReadUint32(&value).IgnoreError();
        packet.set_status(static_cast<Status::Code>(value));
        break;
      }

      case RpcPacket::Fields::CALL_ID:
        // A decode error will propagate from Next() and terminate the loop.
        decoder.ReadUint32(&packet.call_id_).IgnoreError();
        break;
    }
  }

  if (status.IsDataLoss()) {
    return status;
  }

  // TODO(b/234879973): CANCEL is equivalent to CLIENT_ERROR with status
  //     CANCELLED. Remove this workaround when CANCEL is removed.
  if (packet.type() == PacketType::DEPRECATED_CANCEL) {
    packet.set_status(Status::Cancelled());
  }

  return packet;
}

Result<ConstByteSpan> Packet::Encode(ByteSpan buffer) const {
  RpcPacket::MemoryEncoder rpc_packet(buffer);

  // The payload is encoded first, as it may share the encode buffer.
  if (!payload_.empty()) {
    rpc_packet.WritePayload(payload_).IgnoreError();
  }

  rpc_packet.WriteType(type_).IgnoreError();
  rpc_packet.WriteChannelId(channel_id_).IgnoreError();
  rpc_packet.WriteServiceId(service_id_).IgnoreError();
  rpc_packet.WriteMethodId(method_id_).IgnoreError();

  // Status code 0 is OK. In protobufs, 0 is the default int value, so skip
  // encoding it to save two bytes in the output.
  if (status_.code() != 0) {
    rpc_packet.WriteStatus(status_.code()).IgnoreError();
  }

  if (call_id_ != 0) {
    rpc_packet.WriteCallId(call_id_).IgnoreError();
  }

  if (rpc_packet.status().ok()) {
    return ConstByteSpan(rpc_packet);
  }
  return rpc_packet.status();
}

size_t Packet::MinEncodedSizeBytes() const {
  size_t reserved_size = 0;

  reserved_size += 1;  // channel_id key
  reserved_size += varint::EncodedSize(channel_id());
  reserved_size += 1 + sizeof(uint32_t);  // service_id key and fixed32
  reserved_size += 1 + sizeof(uint32_t);  // method_id key and fixed32

  // Packet type always takes two bytes to encode (varint key + varint enum).
  reserved_size += 2;

  // Status field takes up to two bytes to encode (varint key + varint status).
  reserved_size += 2;

  // Payload field takes at least two bytes to encode (varint key + length).
  reserved_size += 2;

  return reserved_size;
}

}  // namespace pw::rpc::internal
