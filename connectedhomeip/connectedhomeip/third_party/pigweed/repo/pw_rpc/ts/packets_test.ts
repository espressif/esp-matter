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

/* eslint-env browser */
import {
  PacketType,
  RpcPacket,
} from 'pigweedjs/protos/pw_rpc/internal/packet_pb';
import {Status} from 'pigweedjs/pw_status';

import * as packets from './packets';

function addTestData(packet: RpcPacket) {
  const payload = new RpcPacket();
  payload.setStatus(321);
  packet.setChannelId(1);
  packet.setServiceId(2);
  packet.setMethodId(3);
  packet.setPayload(payload.serializeBinary());
}

describe('Packets', () => {
  beforeEach(() => { });

  it('encodeRequest sets packet fields', () => {
    const goldenRequest = new RpcPacket();
    goldenRequest.setType(PacketType.REQUEST);
    addTestData(goldenRequest);

    const dataPacket = new RpcPacket();
    dataPacket.setStatus(321);
    const data = packets.encodeRequest([1, 2, 3], dataPacket);
    const packet = RpcPacket.deserializeBinary(data);

    expect(packet.toObject()).toEqual(goldenRequest.toObject());
  });

  it('encodeResponse sets packet fields', () => {
    const goldenResponse = new RpcPacket();
    goldenResponse.setType(PacketType.RESPONSE);
    addTestData(goldenResponse);

    const dataPacket = new RpcPacket();
    dataPacket.setStatus(321);
    const data = packets.encodeResponse([1, 2, 3], dataPacket);
    const packet = RpcPacket.deserializeBinary(data);

    expect(packet.toObject()).toEqual(goldenResponse.toObject());
  });

  it('encodesClientError sets packet fields', () => {
    const packet = new RpcPacket();
    packet.setType(PacketType.REQUEST);
    addTestData(packet);
    const data = packets.encodeClientError(packet, Status.NOT_FOUND);
    const errorPacket = RpcPacket.deserializeBinary(data);

    const golden = new RpcPacket();
    golden.setType(PacketType.CLIENT_ERROR);
    golden.setChannelId(1);
    golden.setServiceId(2);
    golden.setMethodId(3);
    golden.setStatus(Status.NOT_FOUND);

    expect(errorPacket.toObject()).toEqual(golden.toObject());
  });

  it('encodeCancel sets packet fields', () => {
    const goldenCancel = new RpcPacket();
    goldenCancel.setType(PacketType.CLIENT_ERROR);
    goldenCancel.setStatus(Status.CANCELLED);
    goldenCancel.setChannelId(1);
    goldenCancel.setServiceId(2);
    goldenCancel.setMethodId(3);

    const data = packets.encodeCancel([1, 2, 3]);
    const packet = RpcPacket.deserializeBinary(data);
    expect(packet.toObject()).toEqual(goldenCancel.toObject());
  });

  it('decode with serialized request returns request', () => {
    const request = new RpcPacket();
    request.setType(PacketType.REQUEST);
    addTestData(request);

    expect(request.toObject()).toEqual(
      packets.decode(request.serializeBinary()).toObject()
    );
  });

  it('forServer correctly handles RESPONSE and REQUEST types', () => {
    const request = new RpcPacket();
    request.setType(PacketType.REQUEST);
    const response = new RpcPacket();
    response.setType(PacketType.RESPONSE);

    expect(packets.forServer(request)).toBe(true);
    expect(packets.forServer(response)).toBe(false);
  });
});
