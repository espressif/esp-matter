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

import objectPath from 'object-path';
import {Decoder, Encoder} from 'pigweedjs/pw_hdlc';
import {
  Client,
  Channel,
  ServiceClient,
  UnaryMethodStub,
  MethodStub,
  ServerStreamingMethodStub
} from 'pigweedjs/pw_rpc';
import {WebSerialTransport} from '../transport/web_serial_transport';
import {ProtoCollection} from 'pigweedjs/pw_protobuf_compiler';

function protoFieldToMethodName(string) {
  return string.split("_").map(titleCase).join("");
}
function titleCase(string) {
  return string.charAt(0).toUpperCase() + string.slice(1);
}

export class Device {
  private protoCollection: ProtoCollection;
  private transport: WebSerialTransport;
  private decoder: Decoder;
  private encoder: Encoder;
  private rpcAddress: number;
  private nameToMethodArgumentsMap: any;
  client: Client;
  rpcs: any

  constructor(
    protoCollection: ProtoCollection,
    transport: WebSerialTransport = new WebSerialTransport(),
    rpcAddress: number = 82) {
    this.transport = transport;
    this.rpcAddress = rpcAddress;
    this.protoCollection = protoCollection;
    this.decoder = new Decoder();
    this.encoder = new Encoder();
    this.nameToMethodArgumentsMap = {};
    const channels = [
      new Channel(1, (bytes) => {
        const hdlcBytes = this.encoder.uiFrame(this.rpcAddress, bytes);
        this.transport.sendChunk(hdlcBytes);
      })];
    this.client =
      Client.fromProtoSet(channels, this.protoCollection);

    this.setupRpcs();
  }

  async connect() {
    await this.transport.connect();
    this.transport.chunks.subscribe((item) => {
      const decoded = this.decoder.process(item);
      for (const frame of decoded) {
        if (frame.address === this.rpcAddress) {
          this.client.processPacket(frame.data);
        }
      }
    });
  }

  getMethodArguments(fullPath) {
    return this.nameToMethodArgumentsMap[fullPath];
  }

  private setupRpcs() {
    let rpcMap = {};
    let channel = this.client.channel();
    let servicesKeys = Array.from(channel.services.keys());
    servicesKeys.forEach((serviceKey) => {
      objectPath.set(rpcMap, serviceKey,
        this.mapServiceMethods(channel.services.get(serviceKey))
      );
    });
    this.rpcs = rpcMap;
  }

  private mapServiceMethods(service: ServiceClient) {
    let methodMap = {};
    let methodKeys = Array.from(service.methodsByName.keys());
    methodKeys
      .filter((method: any) =>
        service.methodsByName.get(method) instanceof UnaryMethodStub
        || service.methodsByName.get(method) instanceof ServerStreamingMethodStub)
      .forEach(key => {
        let fn = this.createMethodWrapper(
          service.methodsByName.get(key),
          key,
          `${service.name}.${key}`
        );
        methodMap[key] = fn;
      });
    return methodMap;
  }

  private createMethodWrapper(
    realMethod: MethodStub,
    methodName: string,
    fullMethodPath: string) {
    if (realMethod instanceof UnaryMethodStub) {
      return this.createUnaryMethodWrapper(
        realMethod,
        methodName,
        fullMethodPath);
    }
    else if (realMethod instanceof ServerStreamingMethodStub) {
      return this.createServerStreamingMethodWrapper(
        realMethod,
        methodName,
        fullMethodPath);
    }
  }

  private createUnaryMethodWrapper(
    realMethod: UnaryMethodStub,
    methodName: string,
    fullMethodPath: string) {
    const requestType =
      realMethod.method.descriptor.getInputType().replace(/^\./, '');
    const requestProtoDescriptor =
      this.protoCollection.getDescriptorProto(requestType);
    const requestFields = requestProtoDescriptor.getFieldList();
    const functionArguments = requestFields
      .map(field => field.getName())
      .concat(
        'return this(arguments);'
      );

    // We store field names so REPL can show hints in autocomplete using these.
    this.nameToMethodArgumentsMap[fullMethodPath] = requestFields
      .map(field => field.getName());

    // We create a new JS function dynamically here that takes
    // proto message fields as arguments and calls the actual RPC method.
    let fn = new Function(...functionArguments).bind((args) => {
      const request = new realMethod.method.requestType();
      requestFields.forEach((field, index) => {
        request[`set${titleCase(field.getName())}`](args[index]);
      })
      return realMethod.call(request);
    });
    return fn;
  }

  private createServerStreamingMethodWrapper(
    realMethod: ServerStreamingMethodStub,
    methodName: string,
    fullMethodPath: string) {
    const requestType = realMethod.method.descriptor.getInputType().replace(/^\./, '');
    const requestProtoDescriptor =
      this.protoCollection.getDescriptorProto(requestType);
    const requestFields = requestProtoDescriptor.getFieldList();
    const functionArguments = requestFields
      .map(field => field.getName())
      .concat(
        [
          'onNext',
          'onComplete',
          'onError',
          'return this(arguments);'
        ]
      );

    // We store field names so REPL can show hints in autocomplete using these.
    this.nameToMethodArgumentsMap[fullMethodPath] = requestFields
      .map(field => field.getName());

    // We create a new JS function dynamically here that takes
    // proto message fields as arguments and calls the actual RPC method.
    let fn = new Function(...functionArguments).bind((args) => {
      const request = new realMethod.method.requestType();
      requestFields.forEach((field, index) => {
        request[`set${protoFieldToMethodName(field.getName())}`](args[index]);
      })
      const callbacks = Array.from(args).slice(requestFields.length);
      // @ts-ignore
      return realMethod.invoke(request, callbacks[0], callbacks[1], callbacks[2]);
    });
    return fn;
  }
}
