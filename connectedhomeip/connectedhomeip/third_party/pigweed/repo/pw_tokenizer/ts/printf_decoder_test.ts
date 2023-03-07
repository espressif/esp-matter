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
import {PrintfDecoder} from './printf_decoder';

function argFromString(arg: string): Uint8Array {
  const data = new TextEncoder().encode(arg);
  return new Uint8Array([data.length, ...data]);
}

function argFromStringBinary(arg: string): Uint8Array {
  return new Uint8Array(arg.split('').map(ch => ch.charCodeAt(0)));
}

function argsConcat(...args: Uint8Array[]): Uint8Array {
  let data: number[] = [];
  for (const index in args) {
    const argData = args[index];
    data = data.concat([...argData]);
  }
  return new Uint8Array(data);
}

describe('PrintfDecoder', () => {
  let printfDecoder: PrintfDecoder;

  beforeEach(() => {
    printfDecoder = new PrintfDecoder();
  });

  it('formats string correctly', () => {
    expect(printfDecoder.decode('Hello %s', argFromString('Computer'))).toEqual(
      'Hello Computer'
    );
    expect(
      printfDecoder.decode(
        'Hello %s and %s',
        argsConcat(argFromString('Mac'), argFromString('PC'))
      )
    ).toEqual('Hello Mac and PC');
  });

  it('formats varint correctly', () => {
    const arg = argFromStringBinary('\xff\xff\x03');
    expect(printfDecoder.decode('Number %d', arg)).toEqual('Number -32768');
    expect(
      printfDecoder.decode('Numbers %u and %d', argsConcat(arg, arg))
    ).toEqual('Numbers 4294934528 and -32768');
    expect(printfDecoder.decode('Growth is %u%', arg)).toEqual(
      'Growth is 4294934528%'
    );
  });

  it('formats char correctly', () => {
    expect(
      printfDecoder.decode('Battery: 100%c', argFromStringBinary('\x4a'))
    ).toEqual('Battery: 100%');
    expect(
      printfDecoder.decode('Price: %c97.99', argFromStringBinary('\x48'))
    ).toEqual('Price: $97.99');
  });

  it('formats floats correctly', () => {
    expect(
      printfDecoder.decode('Value: %f', argFromStringBinary('\xdb\x0f\x49\x40'))
    ).toEqual('Value: 3.1415927410125732');
    expect(
      printfDecoder.decode(
        'Value: %.5f',
        argFromStringBinary('\xdb\x0f\x49\x40')
      )
    ).toEqual('Value: 3.14159');
  });
});
