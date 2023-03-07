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

/** Low-level HDLC protocol features. */
import {Buffer} from 'buffer';
import {crc32} from 'crc';

/** Special flag character for delimiting HDLC frames. */
export const FLAG = 0x7e;

/** Special character for escaping other special characters in a frame. */
export const ESCAPE = 0x7d;

/** Characters allowed after a 0x7d escape character. */
export const VALID_ESCAPED_BYTES = [0x5d, 0x5e];

/** Frame control for unnumbered information */
export const UI_FRAME_CONTROL = frameControl(0x00);

/** Maximum allowed HDLC address (uint64_t in C++). */
const MAX_ADDRESS = 2 ** 64 - 1;

/**
 * Bitwise OR operation on numbers up to MAX_ADDRESS size.
 * Native bitwise operators only support signed Int32.
 */
function bitwiseOr(x: number, y: number) {
  const highMask = 0x80000000;
  const lowMask = 0x7fffffff;
  const highX = ~~(x / highMask);
  const highY = ~~(y / highMask);
  const lowX = x & lowMask;
  const lowY = y & lowMask;
  const highOr = highX | highY;
  const lowOr = lowX | lowY;
  return highOr * highMask + lowOr;
}

/** Calculates the CRC32 of |data| */
export function frameCheckSequence(data: Uint8Array): Uint8Array {
  const crc = crc32(Buffer.from(data.buffer, data.byteOffset, data.byteLength));
  const arr = new ArrayBuffer(4);
  const view = new DataView(arr);
  view.setUint32(0, crc, true); // litteEndian = true
  return new Uint8Array(arr);
}

/** Escapes or unescapes a byte, which should have been preceeded by 0x7d */
export function escape(byte: number): number {
  return byte ^ 0x20;
}

/** Encodes an HDLC address as a one-terminated LSB varint. */
export function encodeAddress(address: number): Uint8Array {
  const byteList = [];
  while (true) {
    byteList.push((address & 0x7f) << 1);
    address >>= 7;
    if (address === 0) {
      break;
    }
  }

  const result = Uint8Array.from(byteList);
  result[result.length - 1] |= 0x1;
  return result;
}

/** Decodes an HDLC address from a frame, returning it and its size. */
export function decodeAddress(frame: Uint8Array): [number, number] {
  let result = 0;
  let length = 0;

  while (length < frame.length) {
    const byte = frame[length];
    const shift = (byte >> 1) * 2 ** (length * 7);
    result = bitwiseOr(result, shift);
    length += 1;

    if (shift > MAX_ADDRESS || result > MAX_ADDRESS) {
      return [-1, 0];
    }
    if ((byte & 0x1) === 0x1) {
      break;
    }
  }
  return [result, length];
}

function frameControl(frameType: number): Uint8Array {
  return Uint8Array.from([0x03 | frameType]);
}
