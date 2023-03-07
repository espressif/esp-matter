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

import {SomeMessage} from 'pigweedjs/protos/pw_rpc/ts/test2_pb';

import {Call} from './call';
import {Channel, Method, Service} from './descriptors';
import {PendingCalls, Rpc} from './rpc_classes';

class FakeRpc {
  readonly channel: any = undefined;
  readonly service: any = undefined;
  readonly method: any = undefined;

  idSet: [number, number, number] = [1, 2, 3];
  idString = '1.2.3';
}

describe('Call', () => {
  let call: Call;

  beforeEach(() => {
    const noop = () => { };
    const pendingCalls = new PendingCalls();
    const rpc = new FakeRpc();
    call = new Call(pendingCalls, rpc, noop, noop, noop);
  });

  function newMessage(magicNumber = 1): SomeMessage {
    const message = new SomeMessage();
    message.setMagicNumber(magicNumber);
    return message;
  }

  it('getResponse returns all responses.', async () => {
    const message1 = newMessage(1);
    const message2 = newMessage(2);
    const message3 = newMessage(3);

    // Queue three responses
    call.handleResponse(message1);
    call.handleResponse(message2);
    call.handleResponse(message3);

    let responses = call.getResponses(2);
    expect((await responses.next()).value).toEqual(message1);
    expect((await responses.next()).value).toEqual(message2);
    expect((await responses.next()).done).toEqual(true);

    responses = call.getResponses(1);
    expect((await responses.next()).value).toEqual(message3);
    expect((await responses.next()).done).toEqual(true);
  });

  it('getResponse early returns on stream end.', async () => {
    const message = newMessage();
    const responses = call.getResponses(2);

    // Queue one response and an early completion.
    call.handleResponse(message);
    call.handleCompletion(0);

    expect((await responses.next()).value).toEqual(message);
    expect((await responses.next()).done).toEqual(true);
  });

  it('getResponse promise is rejected on stream error.', async () => {
    expect.assertions(2);
    const message = newMessage();
    const responses = call.getResponses(3);

    call.handleResponse(message);
    expect((await responses.next()).value).toEqual(message);

    call.handleResponse(message);
    call.handleError(1);

    // Promise is rejected as soon as an error is received, even if there is a
    // response in the queue.
    responses.next().catch((e: Error) => {
      expect(e.name).toEqual('TypeError');
    });
  });

  it('getResponse waits if queue is empty', async () => {
    const message1 = newMessage(1);
    const message2 = newMessage(2);
    const responses = call.getResponses(2);

    // Queue two responses after a small delay
    setTimeout(() => {
      call.handleResponse(message1);
      call.handleResponse(message2);
      call.handleCompletion(0);
      expect(call.completed).toEqual(true);
    }, 200);

    expect(call.completed).toEqual(false);
    expect((await responses.next()).value).toEqual(message1);
    expect((await responses.next()).value).toEqual(message2);
    expect((await responses.next()).done).toEqual(true);
  });

  it('getResponse without count fetches all results', async () => {
    const message1 = newMessage(1);
    const message2 = newMessage(2);
    const responses = call.getResponses();

    call.handleResponse(message1);
    expect((await responses.next()).value).toEqual(message1);

    setTimeout(() => {
      call.handleResponse(message2);
      call.handleCompletion(0);
      expect(call.completed).toEqual(true);
    }, 200);

    expect(call.completed).toEqual(false);
    expect((await responses.next()).value).toEqual(message2);
    expect((await responses.next()).done).toEqual(true);
  });
});
