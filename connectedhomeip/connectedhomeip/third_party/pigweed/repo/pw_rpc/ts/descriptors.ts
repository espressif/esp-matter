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

import {ProtoCollection} from 'pigweedjs/pw_protobuf_compiler';
import {
  MethodDescriptorProto,
  ServiceDescriptorProto,
} from 'google-protobuf/google/protobuf/descriptor_pb';

import {hash} from './hash';

interface ChannelOutput {
  (data: Uint8Array): void;
}

export class Channel {
  readonly id: number;
  private output: ChannelOutput;

  constructor(id: number, output: ChannelOutput = () => {}) {
    this.id = id;
    this.output = output;
  }

  send(data: Uint8Array) {
    this.output(data);
  }
}

/** Describes an RPC service. */
export class Service {
  readonly name: string;
  readonly id: number;
  readonly methods = new Map<number, Method>();
  readonly methodsByName = new Map<string, Method>();

  constructor(
    descriptor: ServiceDescriptorProto,
    protoCollection: ProtoCollection,
    packageName: string
  ) {
    this.name = packageName + '.' + descriptor.getName()!;
    this.id = hash(this.name);
    descriptor
      .getMethodList()
      .forEach((methodDescriptor: MethodDescriptorProto) => {
        const method = new Method(methodDescriptor, protoCollection, this);
        this.methods.set(method.id, method);
        this.methodsByName.set(method.name, method);
      });
  }
}

export enum MethodType {
  UNARY,
  SERVER_STREAMING,
  CLIENT_STREAMING,
  BIDIRECTIONAL_STREAMING,
}

/** Describes an RPC method. */
export class Method {
  readonly service: Service;
  readonly name: string;
  readonly id: number;
  readonly clientStreaming: boolean;
  readonly serverStreaming: boolean;
  readonly requestType: any;
  readonly responseType: any;
  readonly descriptor: MethodDescriptorProto;

  constructor(
    descriptor: MethodDescriptorProto,
    protoCollection: ProtoCollection,
    service: Service
  ) {
    this.name = descriptor.getName()!;
    this.id = hash(this.name);
    this.service = service;
    this.descriptor = descriptor;
    this.serverStreaming = descriptor.getServerStreaming()!;
    this.clientStreaming = descriptor.getClientStreaming()!;

    const requestTypePath = descriptor.getInputType()!;
    const responseTypePath = descriptor.getOutputType()!;

    // Remove leading period if it exists.
    this.requestType = protoCollection.getMessageCreator(
      requestTypePath.replace(/^\./, '')
    )!;
    this.responseType = protoCollection.getMessageCreator(
      responseTypePath.replace(/^\./, '')
    )!;
  }

  get type(): MethodType {
    if (this.clientStreaming && this.serverStreaming) {
      return MethodType.BIDIRECTIONAL_STREAMING;
    } else if (this.clientStreaming && !this.serverStreaming) {
      return MethodType.CLIENT_STREAMING;
    } else if (!this.clientStreaming && this.serverStreaming) {
      return MethodType.SERVER_STREAMING;
    } else if (!this.clientStreaming && !this.serverStreaming) {
      return MethodType.UNARY;
    }
    throw Error('Unhandled streaming condition');
  }
}
