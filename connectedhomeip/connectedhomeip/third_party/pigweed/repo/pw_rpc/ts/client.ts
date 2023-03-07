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

/** Provides a pw_rpc client for TypeScript. */

import {ProtoCollection} from 'pigweedjs/pw_protobuf_compiler';
import {Status} from 'pigweedjs/pw_status';
import {Message} from 'google-protobuf';
import {
  PacketType,
  RpcPacket,
} from 'pigweedjs/protos/pw_rpc/internal/packet_pb';

import {Channel, Service} from './descriptors';
import {MethodStub, methodStubFactory} from './method';
import * as packets from './packets';
import {PendingCalls, Rpc} from './rpc_classes';

/**
 * Object for managing RPC service and contained methods.
 */
export class ServiceClient {
  private service: Service;
  private methods: MethodStub[] = [];
  methodsByName = new Map<string, MethodStub>();

  constructor(client: Client, channel: Channel, service: Service) {
    this.service = service;
    const methods = service.methods;
    methods.forEach(method => {
      const stub = methodStubFactory(client.rpcs, channel, method);
      this.methods.push(stub);
      this.methodsByName.set(method.name, stub);
    });
  }

  method(methodName: string): MethodStub | undefined {
    return this.methodsByName.get(methodName);
  }

  get id(): number {
    return this.service.id;
  }

  get name(): string {
    return this.service.name;
  }
}

/**
 * Object for managing RPC channel and contained services.
 */
export class ChannelClient {
  readonly channel: Channel;
  services = new Map<string, ServiceClient>();

  constructor(client: Client, channel: Channel, services: Service[]) {
    this.channel = channel;
    services.forEach(service => {
      const serviceClient = new ServiceClient(client, this.channel, service);
      this.services.set(service.name, serviceClient);
    });
  }

  /**
   * Find a service client via its full name.
   *
   * For example:
   * `service = client.channel().service('the.package.FooService');`
   */
  service(serviceName: string): ServiceClient | undefined {
    return this.services.get(serviceName);
  }

  /**
   * Find a method stub via its full name.
   *
   * For example:
   * `method = client.channel().methodStub('the.package.AService.AMethod');`
   */
  methodStub(name: string): MethodStub | undefined {
    const index = name.lastIndexOf('.');
    if (index <= 0) {
      console.error(`Malformed method name: ${name}`);
      return undefined;
    }
    const serviceName = name.slice(0, index);
    const methodName = name.slice(index + 1);
    const method = this.service(serviceName)?.method(methodName);
    if (method === undefined) {
      console.error(`Method not found: ${name}`);
      return undefined;
    }
    return method;
  }
}

/**
 * RPCs are invoked through a MethodStub. These can be found by name via
 * methodStub(string name).
 *
 * ```
 * method = client.channel(1).methodStub('the.package.FooService.SomeMethod')
 * call = method.invoke(request);
 * ```
 */
export class Client {
  private channelsById = new Map<number, ChannelClient>();
  readonly rpcs: PendingCalls;
  readonly services = new Map<number, Service>();

  constructor(channels: Channel[], services: Service[]) {
    this.rpcs = new PendingCalls();
    services.forEach(service => {
      this.services.set(service.id, service);
    });

    channels.forEach(channel => {
      this.channelsById.set(
        channel.id,
        new ChannelClient(this, channel, services)
      );
    });
  }

  /**
   * Creates a client from a set of Channels and a library of Protos.
   *
   * @param {Channel[]} channels List of possible channels to use.
   * @param {ProtoCollection} protoSet ProtoCollection containing protos
   *     defining RPC services
   * and methods.
   */
  static fromProtoSet(channels: Channel[], protoSet: ProtoCollection): Client {
    let services: Service[] = [];
    const descriptors = protoSet.fileDescriptorSet.getFileList();
    descriptors.forEach(fileDescriptor => {
      const packageName = fileDescriptor.getPackage()!;
      fileDescriptor.getServiceList().forEach(serviceDescriptor => {
        services = services.concat(
          new Service(serviceDescriptor, protoSet, packageName)
        );
      });
    });

    return new Client(channels, services);
  }

  /**
   * Finds the channel with the provided id. Returns undefined if there are no
   * channels or no channel with a matching id.
   *
   * @param {number?} id If no id is specified, returns the first channel.
   */
  channel(id?: number): ChannelClient | undefined {
    if (id === undefined) {
      return this.channelsById.values().next().value;
    }
    return this.channelsById.get(id);
  }

  /**
   * Creates a new RPC object holding channel, method, and service info.
   * Returns undefined if the service or method does not exist.
   */
  private rpc(
    packet: RpcPacket,
    channelClient: ChannelClient
  ): Rpc | undefined {
    const service = this.services.get(packet.getServiceId());
    if (service == undefined) {
      return undefined;
    }
    const method = service.methods.get(packet.getMethodId());
    if (method == undefined) {
      return undefined;
    }
    return new Rpc(channelClient.channel, service, method);
  }

  private decodeStatus(rpc: Rpc, packet: RpcPacket): Status | undefined {
    if (packet.getType() === PacketType.SERVER_STREAM) {
      return;
    }
    return packet.getStatus();
  }

  private decodePayload(rpc: Rpc, packet: RpcPacket): Message | undefined {
    if (packet.getType() === PacketType.SERVER_ERROR) {
      return undefined;
    }

    if (
      packet.getType() === PacketType.RESPONSE &&
      rpc.method.serverStreaming
    ) {
      return undefined;
    }

    const payload = packet.getPayload_asU8();
    return packets.decodePayload(payload, rpc.method.responseType);
  }

  private sendClientError(
    client: ChannelClient,
    packet: RpcPacket,
    error: Status
  ) {
    client.channel.send(packets.encodeClientError(packet, error));
  }

  /**
   * Processes an incoming packet.
   *
   * @param {Uint8Array} rawPacketData binary data for a pw_rpc packet.
   * @return {Status} The status of processing the packet.
   *    - OK: the packet was processed by the client
   *    - DATA_LOSS: the packet could not be decoded
   *    - INVALID_ARGUMENT: the packet is for a server, not a client
   *    - NOT_FOUND: the packet's channel ID is not known to this client
   */
  processPacket(rawPacketData: Uint8Array): Status {
    let packet;
    try {
      packet = packets.decode(rawPacketData);
    } catch (err) {
      console.warn(`Failed to decode packet: ${err}`);
      console.debug(`Raw packet: ${rawPacketData}`);
      return Status.DATA_LOSS;
    }

    if (packets.forServer(packet)) {
      return Status.INVALID_ARGUMENT;
    }

    const channelClient = this.channelsById.get(packet.getChannelId());
    if (channelClient == undefined) {
      console.warn(`Unrecognized channel ID: ${packet.getChannelId()}`);
      return Status.NOT_FOUND;
    }

    const rpc = this.rpc(packet, channelClient);
    if (rpc == undefined) {
      this.sendClientError(channelClient, packet, Status.NOT_FOUND);
      console.warn('rpc service/method not found');
      return Status.OK;
    }

    if (
      packet.getType() !== PacketType.RESPONSE &&
      packet.getType() !== PacketType.SERVER_STREAM &&
      packet.getType() !== PacketType.SERVER_ERROR
    ) {
      console.error(`${rpc}: Unexpected packet type ${packet.getType()}`);
      console.debug(`Packet: ${packet}`);
      return Status.OK;
    }

    let status = this.decodeStatus(rpc, packet);
    let payload;
    try {
      payload = this.decodePayload(rpc, packet);
    } catch (error) {
      this.sendClientError(channelClient, packet, Status.DATA_LOSS);
      console.warn(`Failed to decode response: ${error}`);
      console.debug(`Raw payload: ${packet.getPayload()}`);

      // Make this an error packet so the error handler is called.
      packet.setType(PacketType.SERVER_ERROR);
      status = Status.DATA_LOSS;
    }

    const call = this.rpcs.getPending(rpc, status);
    if (call === undefined) {
      this.sendClientError(channelClient, packet, Status.FAILED_PRECONDITION);
      console.debug(`Discarding response for ${rpc}, which is not pending`);
      return Status.OK;
    }

    if (packet.getType() === PacketType.SERVER_ERROR) {
      if (status === Status.OK) {
        throw 'Unexpected OK status on SERVER_ERROR';
      }
      if (status === undefined) {
        throw 'Missing status on SERVER_ERROR';
      }
      console.warn(`${rpc}: invocation failed with status: ${Status[status]}`);
      call.handleError(status);
      return Status.OK;
    }

    if (payload !== undefined) {
      call.handleResponse(payload);
    }
    if (status !== undefined) {
      call.handleCompletion(status);
    }
    return Status.OK;
  }
}
