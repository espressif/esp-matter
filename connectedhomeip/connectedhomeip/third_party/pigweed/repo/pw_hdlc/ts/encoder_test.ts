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

import {Encoder} from './encoder';
import * as protocol from './protocol';
import * as util from './util';

const FLAG = Uint8Array.from([protocol.FLAG]);

function withFCS(data: Uint8Array): Uint8Array {
  return util.concatenate(data, protocol.frameCheckSequence(data));
}

function withFlags(data: Uint8Array): Uint8Array {
  return util.concatenate(FLAG, data, FLAG);
}

describe('Encoder', () => {
  let encoder: Encoder;
  let textEncoder: TextEncoder;

  beforeEach(() => {
    encoder = new Encoder();
    textEncoder = new TextEncoder();
  });

  it('creates frame for empty data', () => {
    const data = textEncoder.encode('');
    expect(encoder.uiFrame(0, data)).toEqual(
      withFlags(withFCS(new Uint8Array([0x01, 0x03])))
    );
    expect(encoder.uiFrame(0x1a, data)).toEqual(
      withFlags(withFCS(new Uint8Array([0x35, 0x03])))
    );
    expect(encoder.uiFrame(0x1a, data)).toEqual(
      withFlags(withFCS(textEncoder.encode('\x35\x03')))
    );
  });

  it('creates frame for one byte', () => {
    const data = textEncoder.encode('A');
    expect(encoder.uiFrame(0, data)).toEqual(
      withFlags(withFCS(textEncoder.encode('\x01\x03A')))
    );
  });

  it('creates frame for multibyte data', () => {
    const data = textEncoder.encode('123456789');
    expect(encoder.uiFrame(0, data)).toEqual(
      withFlags(withFCS(textEncoder.encode('\x01\x03123456789')))
    );
  });

  it('creates frame for multibyte data with address', () => {
    const data = textEncoder.encode('123456789');
    expect(encoder.uiFrame(128, data)).toEqual(
      withFlags(withFCS(textEncoder.encode('\x00\x03\x03123456789')))
    );
  });

  it('creates frame for data with escape sequence', () => {
    const data = textEncoder.encode('\x7d');
    const expectedContent = util.concatenate(
      textEncoder.encode('\x7d\x5d\x03\x7d\x5d'),
      protocol.frameCheckSequence(textEncoder.encode('\x7d\x03\x7d'))
    );
    expect(encoder.uiFrame(0x3e, data)).toEqual(withFlags(expectedContent));

    const data2 = textEncoder.encode('A\x7e\x7dBC');
    const expectedContent2 = util.concatenate(
      textEncoder.encode('\x7d\x5d\x03A\x7d\x5e\x7d\x5dBC'),
      protocol.frameCheckSequence(textEncoder.encode('\x7d\x03A\x7e\x7dBC'))
    );
    expect(encoder.uiFrame(0x3e, data2)).toEqual(withFlags(expectedContent2));
  });
});
