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

/** This module defines the string to ID hash used in pw_rpc. */

const HASH_CONSTANT = 65599;

/**
 * Hash by 65599
 *
 * This is the same hash function that is used in Python pw_rpc. It is chosen
 * due to its simplicity.
 */
export function hash(input: string): number {
  let hashValue = input.length;
  let coefficient = HASH_CONSTANT;
  for (let i = 0; i < input.length; i++) {
    const ord = input.charCodeAt(i);
    hashValue = (hashValue + coefficient * ord) % 2 ** 32;
    coefficient = (coefficient * HASH_CONSTANT) % 2 ** 32;
  }
  return hashValue;
}
