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

import {Frame, Encoder, Decoder} from 'pigweedjs/pw_hdlc';
import {Detokenizer} from './detokenizer';

const CSV = `
64636261,          ,"regular token"
86fc33f3,          ,"base64 token"
0d6bd33c,          ,"Regular Token: %s and Nested Token: %s"
97185e6f,          ,"(token: %s, string: %s, int: %d, float: %f)"
451d86ed,          ,"Cat"
`;

function generateFrame(text: string): Frame {
  const uintArray = new TextEncoder().encode(text);
  const encodedFrame = new Encoder().uiFrame(1, uintArray);
  const decodedFrames = Array.from(new Decoder().process(encodedFrame));
  return decodedFrames[0];
}

describe('Detokenizer', () => {
  let detokenizer: Detokenizer;

  beforeEach(() => {
    detokenizer = new Detokenizer(CSV);
  });

  it('parses a base64 correct frame properly', () => {
    const frame = generateFrame('$8zP8hg==');
    expect(detokenizer.detokenizeBase64(frame)).toEqual('base64 token');
  });
  it('parses a correct frame properly', () => {
    const frame = generateFrame('abcde');
    expect(detokenizer.detokenize(frame)).toEqual('regular token');
  });
  it('failure to detokenize returns original string', () => {
    expect(detokenizer.detokenize(generateFrame('aabbcc'))).toEqual('aabbcc');
    expect(detokenizer.detokenizeBase64(generateFrame('$8zP7hg=='))).toEqual(
      '$8zP7hg=='
    );
  });
  it('recursive detokenize all nested base64 tokens', () => {
    expect(
      detokenizer.detokenizeBase64(
        generateFrame(
          '$PNNrDQkkN1lZZFJRPT0lJGIxNFlsd2trTjFsWlpGSlJQVDBGUTJGdFpXeFlwSENkUHc9PQ=='
        )
      )
    ).toEqual(
      'Regular Token: Cat and Nested Token: (token: Cat, string: Camel, int: 44, float: 1.2300000190734863)'
    );
  });

  it('recursion detokenize with limits on max recursion', () => {
    expect(
      detokenizer.detokenizeBase64(
        generateFrame(
          '$PNNrDQkkN1lZZFJRPT0lJGIxNFlsd2trTjFsWlpGSlJQVDBGUTJGdFpXeFlwSENkUHc9PQ=='
        ),
        1
      )
    ).toEqual(
      'Regular Token: Cat and Nested Token: (token: $7YYdRQ==, string: Camel, int: 44, float: 1.2300000190734863)'
    );
  });
});
