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

import bin from '../src-electron/util/bin.ts'
const { timeout } = require('./test-util.js')
const env = require('../src-electron/util/env.ts')

beforeAll(() => {
  env.setDevelopmentEnv()
})

test(
  '32-bit hex conversions',
  () => {
    let xN = 0x1234abcd
    expect(bin.int32ToHex(xN)).toEqual('1234ABCD')
    expect(bin.int32ToHex(xN, true)).toEqual('CDAB3412')
  },
  timeout.short()
)

test(
  '16-bit hex conversions',
  () => {
    let xN = 0xabcd
    expect(bin.int16ToHex(xN)).toEqual('ABCD')
    expect(bin.int16ToHex(xN, true)).toEqual('CDAB')
  },
  timeout.short()
)

test(
  '8-bit hex conversions',
  () => {
    let xN = 0xab
    expect(bin.int8ToHex(xN)).toEqual('AB')
    expect(bin.int8ToHex(xN, true)).toEqual('AB')
  },
  timeout.short()
)

test(
  'Hex to bytes conversions',
  () => {
    expect(bin.hexToCBytes('1234abcd')).toEqual('0x12, 0x34, 0xAB, 0xCD')
  },
  timeout.short()
)

test(
  'String hex conversions',
  () => {
    let xN = 'abcdABCD'
    let xS = bin.stringToHex(xN)
    expect(xS).toEqual('616263644142434400')
    expect(bin.hexToCBytes(xS)).toEqual(
      '0x61, 0x62, 0x63, 0x64, 0x41, 0x42, 0x43, 0x44, 0x00'
    )
    expect(bin.stringToHex('')).toEqual('00')
    expect(bin.hexToCBytes('00')).toEqual('0x00')
  },
  timeout.short()
)

test(
  'Hex to binary',
  () => {
    let hex = bin.int32ToHex(1234)
    expect(hex).toBe('000004D2')
    expect(bin.hexToBinary(hex)).toBe('00000000000000000000010011010010')
    expect(bin.hexToBinary('0xABCD')).toBe('1010101111001101')
    expect(bin.hexToBinary('0XABCD')).toBe('1010101111001101')
    expect(bin.hexToBinary('AbCd')).toBe('1010101111001101')
    expect(bin.hexToBinary('abcd')).toBe('1010101111001101')
    expect(bin.hexToBinary('ABCD')).toBe('1010101111001101')
    expect(bin.hexToBinary('AB CD')).toBe('1010101111001101')
  },
  timeout.short()
)

test(
  'Bit offset',
  () => {
    expect(bin.bitOffset('010')).toBe(1)
    expect(bin.bitOffset('011')).toBe(0)
    expect(bin.bitOffset(bin.hexToBinary(bin.int8ToHex(2)))).toBe(1)
    expect(bin.bitOffset(bin.hexToBinary(bin.int8ToHex(4)))).toBe(2)
  },
  timeout.short()
)

test(
  'ZCL strings',
  () => {
    let r

    r = bin.stringToOneByteLengthPrefixCBytes('Test string')
    expect(r.content).toBe(
      "11, 'T', 'e', 's', 't', ' ', 's', 't', 'r', 'i', 'n', 'g', "
    )

    r = bin.stringToTwoByteLengthPrefixCBytes('Test string')
    expect(r.content).toContain(
      "11, 0, 'T', 'e', 's', 't', ' ', 's', 't', 'r', 'i', 'n', 'g',"
    )
    expect(r.length).toBe(13)

    r = bin.stringToTwoByteLengthPrefixCBytes('x'.repeat(300), 400, false)
    expect(r.content).toContain(
      "44, 1, 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',"
    )
    expect(r.length).toBe(302)

    r = bin.stringToOneByteLengthPrefixCBytes('Test', 10, true)
    expect(r.content).toBe(
      "4, 'T', 'e', 's', 't', 0x00, 0x00, 0x00, 0x00, 0x00, "
    )
    expect(r.length).toBe(10)

    r = bin.stringToOneByteLengthPrefixCBytes('Test', 10, false)
    expect(r.content).toBe("4, 'T', 'e', 's', 't', ")
    expect(r.length).toBe(5)
  },
  timeout.short()
)

test(
  'Bit count',
  () => {
    expect(bin.bitCount(1)).toBe(1)
    expect(bin.bitCount(2)).toBe(1)
    expect(bin.bitCount(3)).toBe(2)
    expect(bin.bitCount(0x4000)).toBe(1)
    expect(bin.bitCount(0x4210)).toBe(3)
    expect(bin.bitCount(0xf0f0)).toBe(8)
  },
  timeout.short()
)
