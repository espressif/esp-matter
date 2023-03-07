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

import {Message} from 'google-protobuf';
import {Status} from 'pigweedjs/pw_status';

import {Call} from './call';
import {Channel, Method, Service} from './descriptors';
import * as packets from './packets';

/** Data class for a pending RPC call. */
export class Rpc {
  readonly channel: Channel;
  readonly service: Service;
  readonly method: Method;

  constructor(channel: Channel, service: Service, method: Method) {
    this.channel = channel;
    this.service = service;
    this.method = method;
  }

  /** Returns channel service method id tuple */
  get idSet(): [number, number, number] {
    return [this.channel.id, this.service.id, this.method.id];
  }

  /**
   * Returns a string sequence to uniquely identify channel, service, and
   * method. This can be used to hash the Rpc.
   *
   * For example: "12346789.23452345.12341234"
   */
  get idString(): string {
    return `${this.channel.id}.${this.service.id}.${this.method.id}`;
  }

  toString(): string {
    return (
      `${this.service.name}.${this.method.name} on channel ` +
      `${this.channel.id}`
    );
  }
}

/** Tracks pending RPCs and encodes outgoing RPC packets. */
export class PendingCalls {
  pending: Map<string, Call> = new Map();

  /** Starts the provided RPC and returns the encoded packet to send. */
  request(rpc: Rpc, request: Message, call: Call): Uint8Array {
    this.open(rpc, call);
    console.log(`Starting ${rpc}`);
    return packets.encodeRequest(rpc.idSet, request);
  }

  /** Calls request and sends the resulting packet to the channel. */
  sendRequest(
    rpc: Rpc,
    call: Call,
    ignoreError: boolean,
    request?: Message
  ): Call | undefined {
    const previous = this.open(rpc, call);
    const packet = packets.encodeRequest(rpc.idSet, request);
    try {
      rpc.channel.send(packet);
    } catch (error) {
      if (!ignoreError) {
        throw error;
      }
    }
    return previous;
  }

  /**
   * Creates a call for an RPC, but does not invoke it.
   *
   * open() can be used to receive streaming responses to an RPC that was not
   * invoked by this client. For example, a server may stream logs with a
   * server streaming RPC prior to any clients invoking it.
   */
  open(rpc: Rpc, call: Call): Call | undefined {
    console.debug(`Starting ${rpc}`);
    const previous = this.pending.get(rpc.idString);
    this.pending.set(rpc.idString, call);
    return previous;
  }

  sendClientStream(rpc: Rpc, message: Message) {
    if (this.getPending(rpc) === undefined) {
      throw new Error(`Attempt to send client stream for inactive RPC: ${rpc}`);
    }
    rpc.channel.send(packets.encodeClientStream(rpc.idSet, message));
  }

  sendClientStreamEnd(rpc: Rpc) {
    if (this.getPending(rpc) === undefined) {
      throw new Error(`Attempt to send client stream for inactive RPC: ${rpc}`);
    }
    rpc.channel.send(packets.encodeClientStreamEnd(rpc.idSet));
  }

  /** Cancels the RPC. Returns the CANCEL packet to send. */
  cancel(rpc: Rpc): Uint8Array | undefined {
    console.debug(`Cancelling ${rpc}`);
    this.pending.delete(rpc.idString);
    if (rpc.method.clientStreaming && rpc.method.serverStreaming) {
      return undefined;
    }
    return packets.encodeCancel(rpc.idSet);
  }

  /** Calls cancel and sends the cancel packet, if any, to the channel. */
  sendCancel(rpc: Rpc): boolean {
    let packet: Uint8Array | undefined;
    try {
      packet = this.cancel(rpc);
    } catch (err) {
      return false;
    }

    if (packet !== undefined) {
      rpc.channel.send(packet);
    }
    return true;
  }

  /** Gets the pending RPC's call. If status is set, clears the RPC. */
  getPending(rpc: Rpc, status?: Status): Call | undefined {
    if (status === undefined) {
      return this.pending.get(rpc.idString);
    }

    const call = this.pending.get(rpc.idString);
    this.pending.delete(rpc.idString);
    return call;
  }
}
