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

/** Functions for working with pw_rpc packets. */

import {Message} from 'google-protobuf';
import {MethodDescriptorProto} from 'google-protobuf/google/protobuf/descriptor_pb';
import * as packetPb from 'pigweedjs/protos/pw_rpc/internal/packet_pb';
import {Status} from 'pigweedjs/pw_status';

// Channel, Service, Method
type idSet = [number, number, number];

export function decode(data: Uint8Array): packetPb.RpcPacket {
  return packetPb.RpcPacket.deserializeBinary(data);
}

export function decodePayload(payload: Uint8Array, payloadType: any): Message {
  const message = payloadType.deserializeBinary(payload);
  return message;
}

export function forServer(packet: packetPb.RpcPacket): boolean {
  return packet.getType() % 2 == 0;
}

export function encodeClientError(
  packet: packetPb.RpcPacket,
  status: Status
): Uint8Array {
  const errorPacket = new packetPb.RpcPacket();
  errorPacket.setType(packetPb.PacketType.CLIENT_ERROR);
  errorPacket.setChannelId(packet.getChannelId());
  errorPacket.setMethodId(packet.getMethodId());
  errorPacket.setServiceId(packet.getServiceId());
  errorPacket.setStatus(status);
  return errorPacket.serializeBinary();
}

export function encodeClientStream(ids: idSet, message: Message): Uint8Array {
  const streamPacket = new packetPb.RpcPacket();
  streamPacket.setType(packetPb.PacketType.CLIENT_STREAM);
  streamPacket.setChannelId(ids[0]);
  streamPacket.setServiceId(ids[1]);
  streamPacket.setMethodId(ids[2]);
  streamPacket.setPayload(message.serializeBinary());
  return streamPacket.serializeBinary();
}

export function encodeClientStreamEnd(ids: idSet): Uint8Array {
  const streamEnd = new packetPb.RpcPacket();
  streamEnd.setType(packetPb.PacketType.CLIENT_STREAM_END);
  streamEnd.setChannelId(ids[0]);
  streamEnd.setServiceId(ids[1]);
  streamEnd.setMethodId(ids[2]);
  return streamEnd.serializeBinary();
}

export function encodeRequest(ids: idSet, request?: Message): Uint8Array {
  const payload: Uint8Array =
    typeof request !== 'undefined'
      ? request.serializeBinary()
      : new Uint8Array();

  const packet = new packetPb.RpcPacket();
  packet.setType(packetPb.PacketType.REQUEST);
  packet.setChannelId(ids[0]);
  packet.setServiceId(ids[1]);
  packet.setMethodId(ids[2]);
  packet.setPayload(payload);
  return packet.serializeBinary();
}

export function encodeResponse(ids: idSet, response: Message): Uint8Array {
  const packet = new packetPb.RpcPacket();
  packet.setType(packetPb.PacketType.RESPONSE);
  packet.setChannelId(ids[0]);
  packet.setServiceId(ids[1]);
  packet.setMethodId(ids[2]);
  packet.setPayload(response.serializeBinary());
  return packet.serializeBinary();
}

export function encodeCancel(ids: idSet): Uint8Array {
  const packet = new packetPb.RpcPacket();
  packet.setType(packetPb.PacketType.CLIENT_ERROR);
  packet.setStatus(Status.CANCELLED);
  packet.setChannelId(ids[0]);
  packet.setServiceId(ids[1]);
  packet.setMethodId(ids[2]);
  return packet.serializeBinary();
}
