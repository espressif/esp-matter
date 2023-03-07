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

import {Decoder} from './decoder';
import * as protocol from './protocol';
import * as util from './util';

const FLAG = Uint8Array.from([protocol.FLAG]);

function withFCS(data: Uint8Array): Uint8Array {
  return util.concatenate(data, protocol.frameCheckSequence(data));
}

function withFlags(data: Uint8Array): Uint8Array {
  return util.concatenate(FLAG, data, FLAG);
}

describe('Decoder', () => {
  let decoder: Decoder;
  let textEncoder: TextEncoder;

  beforeEach(() => {
    decoder = new Decoder();
    textEncoder = new TextEncoder();
  });

  it('parses a correct UI frame peoperly', () => {
    const expectedData = textEncoder.encode('123456789');
    const expectedAddress = 128;

    const frameData = withFlags(
      withFCS(textEncoder.encode('\x00\x03\x03123456789'))
    );

    const frames = Array.from(decoder.process(frameData));

    expect(frames.length).toEqual(1);
    expect(frames[0].address).toEqual(expectedAddress);
    expect(frames[0].data).toEqual(expectedData);
    expect(frames[0].ok()).toBe(true);
  });
});
