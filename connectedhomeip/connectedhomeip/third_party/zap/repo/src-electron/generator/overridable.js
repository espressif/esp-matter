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

/**
 * This module contains the API for templating. For more detailed instructions, read {@tutorial template-tutorial}
 *
 * @module Templating API: Overridable functions.
 */

// Local utility function
function cleanseUints(uint, size, signed) {
  if (size > 32) {
    return signed ? 'int8_t *' : 'uint8_t *'
  } else if (size == 24) {
    return signed ? 'int32_t' : 'uint32_t'
  } else {
    return uint
  }
}

/**
 * Returns the name of a fall-through non-atomic type.
 * This method will be used unless the override is
 * providing a different implementation.
 *
 * @param {*} arg
 */
function nonAtomicType(arg = { name: 'unknown', isStruct: false }) {
  if (arg.isStruct) {
    return arg.name
  } else {
    return `EmberAf${arg.name}`
  }
}

/**
 * Returns the default atomic C type for a given atomic from
 * the database. These values are used unless there is an
 * override in template package json file. (Not yet fully
 * implemented, but the plan is for template pkg to be able
 * to override these.)
 *
 * @param {*} arg Object containing name and size
 */
function atomicType(arg = { name: 'unknown', size: 0 }) {
  let name = arg.name
  let size = arg.size
  if (name.startsWith('int')) {
    let signed
    if (name.endsWith('s')) signed = true
    else signed = false

    let ret = `${signed ? '' : 'u'}int${size * 8}_t`

    // few exceptions
    ret = cleanseUints(ret, size * 8, signed)
    return ret
  } else if (name.startsWith('enum') || name.startsWith('data')) {
    return cleanseUints(`uint${name.slice(4)}_t`, name.slice(4), false)
  } else if (name.startsWith('bitmap')) {
    return cleanseUints(`uint${name.slice(6)}_t`, name.slice(6), false)
  } else {
    switch (name) {
      case 'utc_time':
      case 'date':
      case 'time_of_day':
      case 'bacnet_oid':
        return 'uint32_t'
      case 'attribute_id':
      case 'cluster_id':
        return 'uint16_t'
      case 'no_data':
      case 'octet_string':
      case 'char_string':
      case 'ieee_address':
        return 'uint8_t *'
      case 'boolean':
        return 'uint8_t'
      case 'array':
        return `/* TYPE WARNING: ${name} array defaults to */ uint8_t * `
      default:
        return `/* TYPE WARNING: ${name} defaults to */ uint8_t * `
    }
  }
}

/**
 *
 * @param size
 * @param name
 * @returns The appropriate c type for an enum
 */
function enumType(size, name) {
  if (name && !name.toLowerCase().includes('enum')) {
    return 'EmberAf' + name
  } else {
    let enumSize = size ? size * 8 : 8
    return 'uint' + enumSize + '_t'
  }
}

/**
 *
 * @param size
 * @returns The appropriate c type for a bitmap
 */
function bitmapType(size) {
  let bitmapSize = size ? size * 8 : 8
  if (size == 3) {
    bitmapSize = (size + 1) * 8
  }
  return 'uint' + bitmapSize + '_t'
}

/**
 *
 *
 * @param size
 * @param isSigned
 * @param name
 * @returns The appropriate c type for a number
 */
function numberType(size, isSigned, name) {
  let prefix = isSigned ? 'int' : 'uint'
  if (name == 'no_data') {
    return 'uint8_t *'
  }
  if (size > 4) {
    if (isSigned) {
      return 'int8_t *'
    } else {
      return 'uint8_t *'
    }
  }
  let numberSize = size ? size * 8 : 8
  if (size == 3) {
    numberSize = (size + 1) * 8
  }
  return prefix + numberSize + '_t'
}

/**
 *
 * @returns 'uint8_t *'
 */
function stringType() {
  return 'uint8_t *'
}

// WARNING! WARNING! WARNING! WARNING! WARNING! WARNING!
//
// Note: these exports are public API. Templates that might have been created in the past and are
// available in the wild might depend on these names.
// If you rename the functions, you need to still maintain old exports list.
exports.atomicType = atomicType
exports.nonAtomicType = nonAtomicType
exports.enumType = enumType
exports.bitmapType = bitmapType
exports.numberType = numberType
exports.stringType = stringType
