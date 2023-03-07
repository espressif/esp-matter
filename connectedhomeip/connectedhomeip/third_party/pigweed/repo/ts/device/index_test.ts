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
import {SerialMock} from '../transport/serial_mock';
import {Device} from "./"
import {ProtoCollection} from 'pigweedjs/protos/collection';
import {WebSerialTransport} from '../transport/web_serial_transport';
import {Serial} from 'pigweedjs/types/serial';
import {Message} from 'google-protobuf';
import {RpcPacket, PacketType} from 'pigweedjs/protos/pw_rpc/internal/packet_pb';
import {Method, ServerStreamingMethodStub} from 'pigweedjs/pw_rpc';
import {Status} from 'pigweedjs/pw_status';
import {
  Response,
} from 'pigweedjs/protos/pw_rpc/ts/test_pb';

describe('WebSerialTransport', () => {
  let device: Device;
  let serialMock: SerialMock;

  function newResponse(payload = '._.'): Message {
    const response = new Response();
    response.setPayload(payload);
    return response;
  }

  function generateResponsePacket(
    channelId: number,
    method: Method,
    status: Status,
    response?: Message
  ) {
    const packet = new RpcPacket();
    packet.setType(PacketType.RESPONSE);
    packet.setChannelId(channelId);
    packet.setServiceId(method.service.id);
    packet.setMethodId(method.id);
    packet.setStatus(status);
    if (response === undefined) {
      packet.setPayload(new Uint8Array());
    } else {
      packet.setPayload(response.serializeBinary());
    }
    return packet.serializeBinary();
  }

  function generateStreamingPacket(
    channelId: number,
    method: Method,
    response: Message,
    status: Status = Status.OK
  ) {
    const packet = new RpcPacket();
    packet.setType(PacketType.SERVER_STREAM);
    packet.setChannelId(channelId);
    packet.setServiceId(method.service.id);
    packet.setMethodId(method.id);
    packet.setPayload(response.serializeBinary());
    packet.setStatus(status);
    return packet.serializeBinary();
  }

  beforeEach(() => {
    serialMock = new SerialMock();
    device = new Device(new ProtoCollection(), new WebSerialTransport(serialMock as Serial));
  });

  it('has rpcs defined', () => {
    expect(device.rpcs).toBeDefined();
    expect(device.rpcs.pw.rpc.EchoService.Echo).toBeDefined();
  });

  it('has method arguments data', () => {
    expect(device.getMethodArguments("pw.rpc.EchoService.Echo")).toStrictEqual(["msg"]);
    expect(device.getMethodArguments("pw.test2.Alpha.Unary")).toStrictEqual(['magic_number']);
  });

  it('unary rpc sends request to serial', async () => {
    const helloResponse = new Uint8Array([
      126, 165, 3, 42, 7, 10, 5, 104,
      101, 108, 108, 111, 8, 1, 16, 1,
      29, 82, 208, 251, 20, 37, 233, 14,
      71, 139, 109, 127, 108, 165, 126]);

    await device.connect();
    serialMock.dataFromDevice(helloResponse);
    const [status, response] = await device.rpcs.pw.rpc.EchoService.Echo("hello");
    expect(response.getMsg()).toBe("hello");
    expect(status).toBe(0);
  });

  it('server streaming rpc sends response', async () => {
    await device.connect();
    const response1 = newResponse('!!!');
    const response2 = newResponse('?');
    const serverStreaming = device.client
      .channel()
      ?.methodStub(
        'pw.rpc.test1.TheTestService.SomeServerStreaming'
      )! as ServerStreamingMethodStub;
    const onNext = jest.fn();
    const onCompleted = jest.fn();
    const onError = jest.fn();

    device.rpcs.pw.rpc.test1.TheTestService.SomeServerStreaming(4, onNext, onCompleted, onError);
    device.client.processPacket(generateStreamingPacket(1, serverStreaming.method, response1));
    device.client.processPacket(generateStreamingPacket(1, serverStreaming.method, response2));
    device.client.processPacket(generateResponsePacket(1, serverStreaming.method, Status.ABORTED));

    expect(onNext).toBeCalledWith(response1);
    expect(onNext).toBeCalledWith(response2);
    expect(onCompleted).toBeCalledWith(Status.ABORTED);
  });
});
