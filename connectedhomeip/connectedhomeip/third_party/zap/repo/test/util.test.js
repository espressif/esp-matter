/**
 *
 *    Copyright (c) 2020 Silicon Labs
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 *
 *
 * @jest-environment node
 */

const util = require('../src/util/util.js')
const { timeout } = require('./test-util.js')

test(
  'Clean symbol',
  () => {
    expect(util.asHex('-1', 4)).toEqual('-1')
    expect(util.asHex(null, 4)).toEqual('')
    expect(util.asHex('123', 4)).toEqual('0x007B')
    expect(util.asHex('0x123', 4)).toEqual('0x0123')
    expect(util.asHex(123, 4)).toEqual('0x007B')
  },
  timeout.short()
)
