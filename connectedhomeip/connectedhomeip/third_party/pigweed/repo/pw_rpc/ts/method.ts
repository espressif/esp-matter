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

import {Status} from 'pigweedjs/pw_status';
import {Message} from 'google-protobuf';

import {
  BidirectionalStreamingCall,
  Call,
  Callback,
  ClientStreamingCall,
  ServerStreamingCall,
  UnaryCall,
} from './call';
import {Channel, Method, MethodType, Service} from './descriptors';
import {PendingCalls, Rpc} from './rpc_classes';

export function methodStubFactory(
  rpcs: PendingCalls,
  channel: Channel,
  method: Method
): MethodStub {
  switch (method.type) {
    case MethodType.BIDIRECTIONAL_STREAMING:
      return new BidirectionalStreamingMethodStub(rpcs, channel, method);
    case MethodType.CLIENT_STREAMING:
      return new ClientStreamingMethodStub(rpcs, channel, method);
    case MethodType.SERVER_STREAMING:
      return new ServerStreamingMethodStub(rpcs, channel, method);
    case MethodType.UNARY:
      return new UnaryMethodStub(rpcs, channel, method);
  }
}

export abstract class MethodStub {
  readonly method: Method;
  readonly rpcs: PendingCalls;
  readonly rpc: Rpc;
  private channel: Channel;

  constructor(rpcs: PendingCalls, channel: Channel, method: Method) {
    this.method = method;
    this.rpcs = rpcs;
    this.channel = channel;
    this.rpc = new Rpc(channel, method.service, method);
  }

  get id(): number {
    return this.method.id;
  }
}

export class UnaryMethodStub extends MethodStub {
  invoke(
    request: Message,
    onNext: Callback = () => {},
    onCompleted: Callback = () => {},
    onError: Callback = () => {}
  ): UnaryCall {
    const call = new UnaryCall(
      this.rpcs,
      this.rpc,
      onNext,
      onCompleted,
      onError
    );
    call.invoke(request);
    return call;
  }

  open(
    request: Message,
    onNext: Callback = () => {},
    onCompleted: Callback = () => {},
    onError: Callback = () => {}
  ): UnaryCall {
    const call = new UnaryCall(
      this.rpcs,
      this.rpc,
      onNext,
      onCompleted,
      onError
    );
    call.invoke(request, true);
    return call;
  }

  async call(request: Message, timeout?: number): Promise<[Status, Message]> {
    return await this.invoke(request).complete(timeout);
  }
}

export class ServerStreamingMethodStub extends MethodStub {
  invoke(
    request?: Message,
    onNext: Callback = () => {},
    onCompleted: Callback = () => {},
    onError: Callback = () => {}
  ): ServerStreamingCall {
    const call = new ServerStreamingCall(
      this.rpcs,
      this.rpc,
      onNext,
      onCompleted,
      onError
    );
    call.invoke(request);
    return call;
  }

  open(
    request: Message,
    onNext: Callback = () => {},
    onCompleted: Callback = () => {},
    onError: Callback = () => {}
  ): UnaryCall {
    const call = new UnaryCall(
      this.rpcs,
      this.rpc,
      onNext,
      onCompleted,
      onError
    );
    call.invoke(request, true);
    return call;
  }

  call(request?: Message, timeout?: number): Promise<[Status, Message[]]> {
    return this.invoke(request).complete(timeout);
  }
}

export class ClientStreamingMethodStub extends MethodStub {
  invoke(
    onNext: Callback = () => {},
    onCompleted: Callback = () => {},
    onError: Callback = () => {}
  ): ClientStreamingCall {
    const call = new ClientStreamingCall(
      this.rpcs,
      this.rpc,
      onNext,
      onCompleted,
      onError
    );
    call.invoke();
    return call;
  }

  open(
    onNext: Callback = () => {},
    onCompleted: Callback = () => {},
    onError: Callback = () => {}
  ): ClientStreamingCall {
    const call = new ClientStreamingCall(
      this.rpcs,
      this.rpc,
      onNext,
      onCompleted,
      onError
    );
    call.invoke(undefined, true);
    return call;
  }

  async call(requests: Array<Message> = [], timeout?: number) {
    return this.invoke().finishAndWait(requests, timeout);
  }
}

export class BidirectionalStreamingMethodStub extends MethodStub {
  invoke(
    onNext: Callback = () => {},
    onCompleted: Callback = () => {},
    onError: Callback = () => {}
  ): BidirectionalStreamingCall {
    const call = new BidirectionalStreamingCall(
      this.rpcs,
      this.rpc,
      onNext,
      onCompleted,
      onError
    );
    call.invoke();
    return call;
  }

  open(
    onNext: Callback = () => {},
    onCompleted: Callback = () => {},
    onError: Callback = () => {}
  ): BidirectionalStreamingCall {
    const call = new BidirectionalStreamingCall(
      this.rpcs,
      this.rpc,
      onNext,
      onCompleted,
      onError
    );
    call.invoke(undefined, true);
    return call;
  }

  async call(requests: Array<Message> = [], timeout?: number) {
    return this.invoke().finishAndWait(requests, timeout);
  }
}
