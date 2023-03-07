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
 */

// Binary utilities to deal with hex numbers and such.

import byteBuffer from 'bytebuffer'
/**
 * Takes an int8 value and turns it into a hex.
 *
 * @param {*} value
 * @returns hex string, 2 characters long without '0x'
 */
function int8ToHex(value: number, littleEndian = false) {
  let bb = new byteBuffer(1, littleEndian, byteBuffer.DEFAULT_NOASSERT)
  bb.writeInt8(value)
  bb.flip()
  return bb.toHex().toUpperCase()
}

/**
 * Takes an int16 value and turns it into a hex.
 *
 * @param {*} value
 * @returns hex string, 4 characters long without '0x'
 */
function int16ToHex(value: number, littleEndian = false) {
  let bb = new byteBuffer(2, littleEndian, byteBuffer.DEFAULT_NOASSERT)
  bb.writeInt16(value)
  bb.flip()
  return bb.toHex().toUpperCase()
}

/**
 * Takes an int32 value and turns it into a hex.
 *
 * @param {*} value
 * @returns hex string, 8 characters long without '0x'
 */
function int32ToHex(value: number, littleEndian = false) {
  let bb = new byteBuffer(4, littleEndian, byteBuffer.DEFAULT_NOASSERT)
  bb.writeInt32(value)
  bb.flip()
  return bb.toHex().toUpperCase()
}

/**
 * Converts a string to the hex value.
 *
 * @param {*} value
 * @returns hex string, value.length * 2 + 2 characters long. It appends the terminating NULL, so 0x00 is at the end.
 */
function stringToHex(value: string) {
  let bb = new byteBuffer(value.length, false, byteBuffer.DEFAULT_NOASSERT)
  bb.writeCString(value)
  bb.flip()
  return bb.toHex().toUpperCase()
}

/**
 * Given a number, this function returns the number of bits set in the number
 *
 * @param {*} n
 * @returns number of bits set.
 */
function bitCount(n: number) {
  n = n - ((n >> 1) & 0x55555555)
  n = (n & 0x33333333) + ((n >> 2) & 0x33333333)
  return (((n + (n >> 4)) & 0xf0f0f0f) * 0x1010101) >> 24
}

/**
 * Takes the raw hex string, such as `abcd` and
 * converts it into a C constant array, such as
 * `0xAB, 0xCD`.
 *
 * @param {*} value
 * @returns C byte array
 */
function hexToCBytes(value: string) {
  let out = ''
  let infix = ''
  for (let i = 0; i < value.length; i += 2) {
    out += infix
    out += '0x'.concat(value.substring(i, i + 2).toUpperCase())
    infix = ', '
  }
  return out
}

/**
 * Getting a binary string ("0001101010010") it returns the number of zero bits at the end.
 * @param {*} binary
 */
function bitOffset(binary: string) {
  let lastIndex = binary.lastIndexOf('1')
  return binary.length - lastIndex - 1
}

/**
 * Convert a hex number to a binary. Hex has to be in a format
 * as obtained by intToHex methods above: no '0x' prefix and upper-case
 * letters, as in "12AB".
 *
 * @param {*} hex
 */
function hexToBinary(hex: string) {
  let cleansedHex = hex
  if (cleansedHex.startsWith('0x') || cleansedHex.startsWith('0X'))
    cleansedHex = cleansedHex.slice(2)
  cleansedHex = cleansedHex.toUpperCase()

  cleansedHex = cleansedHex.replace(/[^0-F]/g, '')

  return parseInt(cleansedHex, 16)
    .toString(2)
    .padStart(cleansedHex.length * 4, '0')
}

/**
 * Returns string as C bytes, prefixed with one-byte length.
 * If maxLength is greater than length of value, then
 * the resulting array is padded with 0x00.
 *
 * @param {*} value
 * @param {*} maxLength the maximum length of the used memory in bytes
 * @param {*} pad If true, then pad with 0x00 until maxLength bytes.
 * @returns Object containing 'length' and 'content', where length
 * is number of bytes used and content is actual content in C format.
 */
function stringToOneByteLengthPrefixCBytes(
  value: string,
  maxLength: number,
  pad = true
) {
  let len = value.length
  let ret = `${len}, `
  for (let i = 0; i < len; i++) {
    ret = ret.concat(`'${value[i]}', `)
  }
  let totalBytes = len + 1
  if (pad && maxLength > len + 1) {
    for (let i = 0; i < maxLength - (len + 1); i++) {
      ret = ret.concat('0x00, ')
      totalBytes++
    }
  }
  return {
    content: ret,
    length: totalBytes,
  }
}

/**
 * Returns string as C bytes, prefixed with two-byte length
 * If maxLength is greater than length of value, then
 * the resulting array is padded with 0x00.
 *
 * @param {*} value
 * @param {*} maxLength the maximum length of the used memory in bytes
 * @param {*} pad If true, then pad with 0x00 until maxLength bytes.
 * @returns Object containing 'length' and 'content', where length
 * is number of bytes used and content is actual content in C format.
 */
function stringToTwoByteLengthPrefixCBytes(
  value: string,
  maxLength: number,
  pad = true
) {
  let len = value.length
  let ret = `${len & 0xff}, `
  ret = ret.concat(`${(len >> 8) & 0xff}, `)
  for (let i = 0; i < len; i++) {
    ret = ret.concat(`'${value[i]}', `)
  }
  let totalBytes = len + 2
  if (pad && maxLength > len + 2) {
    for (let i = 0; i < maxLength - (len + 2); i++) {
      ret = ret.concat('0x00, ')
      totalBytes++
    }
  }
  return {
    content: ret,
    length: totalBytes,
  }
}

exports.int32ToHex = int32ToHex
exports.int16ToHex = int16ToHex
exports.int8ToHex = int8ToHex
exports.stringToHex = stringToHex
exports.hexToCBytes = hexToCBytes
exports.hexToBinary = hexToBinary
exports.bitOffset = bitOffset
exports.stringToOneByteLengthPrefixCBytes = stringToOneByteLengthPrefixCBytes
exports.stringToTwoByteLengthPrefixCBytes = stringToTwoByteLengthPrefixCBytes
exports.bitCount = bitCount
