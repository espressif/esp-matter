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

/** Convenience methods for working with Uint8Array buffers */

/**
 * Returns a new array where all instances of target have been replaced by the
 * provided substitue.
 */
export function replace(
  data: Uint8Array,
  target: number,
  substitute: number[]
): Uint8Array {
  const result: number[] = [];
  data.forEach(value => {
    if (value === target) {
      result.push(...substitute);
    } else {
      result.push(value);
    }
  });
  return Uint8Array.from(result);
}

/** Flattens the provided list of Uint8Arrays into a single array. */
export function concatenate(...byteList: Uint8Array[]): Uint8Array {
  const length = byteList.reduce(
    (accumulator, bytes) => accumulator + bytes.length,
    0
  );
  const result = new Uint8Array(length);
  let offset = 0;
  byteList.forEach(value => {
    result.set(value, offset);
    offset += value.length;
  });
  return result;
}
