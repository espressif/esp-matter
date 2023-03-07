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

#include <cstddef>
#include <cstdint>

#include "pw_bytes/span.h"
#include "pw_protobuf/serialized_size.h"
#include "pw_rpc/internal/packet.pwpb.h"
#include "pw_span/span.h"
#include "pw_status/status_with_size.h"

namespace pw::rpc::internal {

class Packet {
 public:
  static constexpr uint32_t kUnassignedId = 0;

  // TODO(b/236156534): This can use the pwpb generated
  // pw::rpc::internal::pwpb::RpcPacket::kMaxEncodedSizeBytes once the max value
  // of enums is properly accounted for and when `status` is changed from a
  // uint32 to a StatusCode.
  static constexpr size_t kMinEncodedSizeWithoutPayload =
      protobuf::SizeOfFieldEnum(pwpb::RpcPacket::Fields::TYPE, 7) +
      protobuf::SizeOfFieldUint32(pwpb::RpcPacket::Fields::CHANNEL_ID) +
      protobuf::SizeOfFieldFixed32(pwpb::RpcPacket::Fields::SERVICE_ID) +
      protobuf::SizeOfFieldFixed32(pwpb::RpcPacket::Fields::METHOD_ID) +
      protobuf::SizeOfDelimitedFieldWithoutValue(
          pwpb::RpcPacket::Fields::PAYLOAD) +
      protobuf::SizeOfFieldUint32(pwpb::RpcPacket::Fields::STATUS,
                                  Status::Unauthenticated().code()) +
      protobuf::SizeOfFieldUint32(pwpb::RpcPacket::Fields::CALL_ID);

  // Parses a packet from a protobuf message. Missing or malformed fields take
  // their default values.
  static Result<Packet> FromBuffer(ConstByteSpan data);

  // Creates an RPC packet with the channel, service, and method ID of the
  // provided packet.
  static constexpr Packet Response(const Packet& request,
                                   Status status = OkStatus()) {
    return Packet(pwpb::PacketType::RESPONSE,
                  request.channel_id(),
                  request.service_id(),
                  request.method_id(),
                  request.call_id(),
                  {},
                  status);
  }

  // Creates a SERVER_ERROR packet with the channel, service, and method ID of
  // the provided packet.
  static constexpr Packet ServerError(const Packet& packet, Status status) {
    return Packet(pwpb::PacketType::SERVER_ERROR,
                  packet.channel_id(),
                  packet.service_id(),
                  packet.method_id(),
                  packet.call_id(),
                  {},
                  status);
  }

  // Creates a CLIENT_ERROR packet with the channel, service, and method ID of
  // the provided packet.
  static constexpr Packet ClientError(const Packet& packet, Status status) {
    return Packet(pwpb::PacketType::CLIENT_ERROR,
                  packet.channel_id(),
                  packet.service_id(),
                  packet.method_id(),
                  packet.call_id(),
                  {},
                  status);
  }

  // Creates an empty packet.
  constexpr Packet()
      : Packet(
            pwpb::PacketType{}, kUnassignedId, kUnassignedId, kUnassignedId) {}

  constexpr Packet(pwpb::PacketType type,
                   uint32_t channel_id,
                   uint32_t service_id,
                   uint32_t method_id,
                   uint32_t call_id = kUnassignedId,
                   ConstByteSpan payload = {},
                   Status status = OkStatus())
      : type_(type),
        channel_id_(channel_id),
        service_id_(service_id),
        method_id_(method_id),
        call_id_(call_id),
        payload_(payload),
        status_(status) {}

  // Encodes the packet into its wire format. Returns the encoded size.
  Result<ConstByteSpan> Encode(ByteSpan buffer) const;

  // Determines the space required to encode the packet proto fields for a
  // response, excluding the payload. This may be used to split the buffer into
  // reserved space and available space for the payload.
  //
  // This method allocates two bytes for the status. Status code 0 (OK) is not
  // encoded since 0 is the default value.
  size_t MinEncodedSizeBytes() const;

  enum Destination : bool { kServer, kClient };

  constexpr Destination destination() const {
    return static_cast<int>(type_) % 2 == 0 ? kServer : kClient;
  }

  constexpr pwpb::PacketType type() const { return type_; }
  constexpr uint32_t channel_id() const { return channel_id_; }
  constexpr uint32_t service_id() const { return service_id_; }
  constexpr uint32_t method_id() const { return method_id_; }
  constexpr uint32_t call_id() const { return call_id_; }
  constexpr const ConstByteSpan& payload() const { return payload_; }
  constexpr const Status& status() const { return status_; }

  constexpr void set_type(pwpb::PacketType type) { type_ = type; }
  constexpr void set_channel_id(uint32_t channel_id) {
    channel_id_ = channel_id;
  }
  constexpr void set_service_id(uint32_t service_id) {
    service_id_ = service_id;
  }
  constexpr void set_method_id(uint32_t method_id) { method_id_ = method_id; }
  constexpr void set_call_id(uint32_t call_id) { call_id_ = call_id; }
  constexpr void set_payload(ConstByteSpan payload) { payload_ = payload; }
  constexpr void set_status(Status status) { status_ = status; }

 private:
  pwpb::PacketType type_;
  uint32_t channel_id_;
  uint32_t service_id_;
  uint32_t method_id_;
  uint32_t call_id_;
  ConstByteSpan payload_;
  Status status_;
};

}  // namespace pw::rpc::internal
