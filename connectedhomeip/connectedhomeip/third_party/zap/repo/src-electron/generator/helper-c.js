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

const queryZcl = require('../db/query-zcl.js')
const queryPackage = require('../db/query-package.js')
const templateUtil = require('./template-util.js')
const bin = require('../util/bin')
const types = require('../util/types.js')
const string = require('../util/string')
const _ = require('lodash')
const dbEnum = require('../../src-shared/db-enum.js')
const envConfig = require('../util/env')

/**
 * This module contains the API for templating. For more detailed instructions, read {@tutorial template-tutorial}
 *
 * @module Templating API: C formatting helpers
 */

/**
 * Given a hex number, it prints the offset, which is the index of the first non-zero bit.
 * @param {*} hex
 */
function asOffset(number) {
  let num = parseInt(number)
  return bin.bitOffset(bin.hexToBinary(num.toString(16)))
}

/**
 * Takes a label, and delimits is on camelcasing.
 * For example:
 *    VerySimpleLabel will turn into VERY_SIMPLE_LABEL
 * @param {*} label
 */
function asDelimitedMacro(label) {
  return string.toSnakeCaseAllCaps(label)
}

/**
 * Formats label as a C hex constant.
 * If value starts as 0x or 0X it is already treated as hex,
 * otherwise it is assumed decimal and converted to hex.
 *
 * @param {*} label
 * @returns Label formatted as C hex constant.
 */
function asHex(rawValue, padding, nullValue) {
  if (rawValue == null) {
    if (nullValue != null && _.isString(nullValue)) return nullValue
    else rawValue = 0
  }
  let value = rawValue.toString()
  let ret = value.trim()
  if (ret.startsWith('0x') || ret.startsWith('0X')) {
    return `0x${value.slice(2).toUpperCase()}`
  } else {
    let val = parseInt(value)
    return `0x${val.toString(16).padStart(padding, '0').toUpperCase()}`
  }
}

/**
 * This function is a helper function for asUnderlyingType and assists in
 * returning the correct C type for the given data type
 * @param {*} dataType
 * @param {*} context
 * @param {*} packageIds
 * @returns The appropriate C type for the given data type
 */
async function asUnderlyingTypeHelper(dataType, context, packageIds) {
  try {
    if (
      dataType &&
      dataType.discriminatorName.toLowerCase() == dbEnum.zclType.bitmap
    ) {
      let bt = await queryZcl.selectBitmapByName(
        context.global.db,
        packageIds,
        dataType.name
      )
      return context.global.overridable.bitmapType(bt.size)
    } else if (
      dataType &&
      dataType.discriminatorName.toLowerCase() == dbEnum.zclType.enum
    ) {
      let et = await queryZcl.selectEnumByName(
        context.global.db,
        dataType.name,
        packageIds
      )
      return context.global.overridable.enumType(et.size, et.name)
    } else if (
      dataType &&
      dataType.discriminatorName.toLowerCase() == dbEnum.zclType.number
    ) {
      let nt = await queryZcl.selectNumberByName(
        context.global.db,
        packageIds,
        dataType.name
      )
      return context.global.overridable.numberType(
        nt.size,
        nt.isSigned,
        nt.name
      )
    } else if (
      dataType &&
      dataType.discriminatorName.toLowerCase() == dbEnum.zclType.struct
    ) {
      return dataType.name
    } else if (
      dataType &&
      dataType.discriminatorName.toLowerCase() == dbEnum.zclType.string
    ) {
      return context.global.overridable.stringType()
    } else {
      return 'uint8_t *'
    }
  } catch (err) {
    envConfig.logError(
      'Could not find the underlying type for ' + dataType.name + ' : ' + err
    )
  }
}

/**
 * Converts the actual zcl type into an underlying usable C type.
 * @param {*} value
 * @returns The appropriate C Type
 */
async function asUnderlyingType(value) {
  let dataType = null
  let packageIds = await templateUtil.ensureZclPackageIds(this)

  // Step 1: Extracting the data type based on id or name
  if (typeof value === 'number') {
    dataType = await queryZcl.selectDataTypeById(this.global.db, value)
  } else if (typeof value === 'string') {
    dataType = await queryZcl.selectDataTypeByName(
      this.global.db,
      value,
      packageIds
    )
  }

  // Step 2: Check if a type override for the data type exists in the meta-data
  if (dataType) {
    let typeOverride = await queryPackage.selectSpecificOptionValue(
      this.global.db,
      this.global.genTemplatePackageId,
      'types',
      dataType.name
    )
    if (typeOverride) {
      return typeOverride.optionLabel
    }
  }

  // Step 3: Detecting the type of the data type and returning the appropriate c
  // type through overridable
  return asUnderlyingTypeHelper(dataType, this, packageIds)
}

/**
 * Formats label as a C type.
 *
 * @param {*} label
 * @returns Label formatted as C type.
 */
function asType(value) {
  return value.replace(/[ |-]/g, '')
}

/**
 * Formats label as a C symbol.
 *
 * @param {*} label
 * @returns Label formatted as C symbol.
 */
function asSymbol(value) {
  return value
}

// Formats the default value into an attribute of a given length
function formatValue(value, length, type) {
  let out = ''
  if (length < 0) {
    out = out.concat(value.length)
    for (let i = 0; i < value.length; i++) {
      let ch = value.charAt(i)
      out = out.concat(",'").concat(ch).concat("'")
    }
  } else {
    let val = 0
    if (value.startsWith('0x') || value.startsWith('0X')) {
      val = parseInt(value.slice(2), 16)
    } else {
      val = parseInt(value)
    }
    if (Number.isNaN(val)) {
      val = 0
    }
    switch (length) {
      case 1:
        out = out.concat(bin.hexToCBytes(bin.int8ToHex(val)))
        break
      case 2:
        out = out.concat(bin.hexToCBytes(bin.int16ToHex(val)))
        break
      case 4:
        out = out.concat(bin.hexToCBytes(bin.int32ToHex(val)))
        break
    }
  }
  return out
}

/**
 * Given a default value of attribute, this method converts it into bytes
 *
 * @param {*} value
 */
async function asBytes(value, type) {
  if (type == null) {
    return value
  } else {
    return templateUtil
      .ensureZclPackageIds(this)
      .then((packageIds) =>
        queryZcl.selectSizeFromType(this.global.db, packageIds, type)
      )
      .then((x) => {
        if (x == null) {
          if (value == null) {
            return '0x00'
          } else {
            return bin.hexToCBytes(bin.stringToHex(value))
          }
        } else {
          return formatValue(value, x, type)
        }
      })
  }
}

/**
 * Given a string convert it into a camelCased string
 *
 * @param {*} str
 * @returns a spaced out string in lowercase
 */
function asCamelCased(label, firstLower = true) {
  return string.toCamelCase(label, firstLower)
}

/**
 * returns a string after converting ':' and '-' into '_'
 * @param {*} label
 */
function cleanseLabel(label) {
  return string.toCleanSymbol(label)
}

/**
 * Given a camel case string, convert it into one with underscore and lowercase
 *
 * @param {*} str
 * @returns String in lowercase with underscores
 */
function asUnderscoreLowercase(str) {
  return string.toSnakeCase(str)
}

/**
 * returns a string after converting ':' and ' ' into '-'
 * @param {*} label
 */
function cleanseLabelAsKebabCase(label) {
  return string.toCleanSymbolAsKebabCase(label)
}

/**
 * Given a camel case string convert it into one with space and lowercase
 *
 * @param {*} str
 * @returns a spaced out string in lowercase
 */
function asSpacedLowercase(str) {
  return string.toSpacedLowercase(str)
}

/**
 * Given a camel case string convert it into one with underscore and uppercase
 *
 * @param {*} str
 * @returns String in uppercase with underscores
 */
function asUnderscoreUppercase(str) {
  let label = str.replace(/\.?([A-Z][a-z])/g, function (x, y) {
    return '_' + y
  })
  label = cleanseLabel(label)
  if (label.startsWith('_')) {
    label = label.substring(1)
  }
  return label.toUpperCase()
}

/**
 * Returns the cli type representation.
 *
 * @param size
 * @param isSigned
 * @returns the type representation required for CLI.
 */
function asCliType(size, isSigned) {
  return 'SL_CLI_ARG_' + (isSigned ? 'INT' + size * 8 : 'UINT' + size * 8)
}

/**
 *
 * @param str
 * @param optional
 * @param isSigned
 * Return the data type of zcl cli
 */
function as_zcl_cli_type(str, optional, isSigned) {
  let zclType = ''
  switch (str) {
    case 1:
      zclType = 'SL_CLI_ARG_' + (isSigned ? '' : 'U') + 'INT8'
      break
    case 2:
      zclType = 'SL_CLI_ARG_' + (isSigned ? '' : 'U') + 'INT16'
      break
    case 3:
    case 4:
      zclType = 'SL_CLI_ARG_' + (isSigned ? '' : 'U') + 'INT32'
      break
    case dbEnum.zclType.string:
    default:
      zclType = 'SL_CLI_ARG_STRING'
      break
  }
  if (optional) {
    zclType += 'OPT'
  }
  return zclType
}

/**
 * Returns the type of bitmap based on the bitmap's name
 *
 * @param {*} db
 * @param {*} bitmap_name
 * @param {*} packageIds
 */
function dataTypeForBitmap(db, bitmap_name, packageIds) {
  return queryZcl.selectBitmapByName(db, packageIds, bitmap_name).then((bm) => {
    if (bm == null) {
      return `!!Invalid bitmap: ${bitmap_name}`
    } else {
      return asCliType(bm.size, false)
    }
  })
}

/**
 * Returns the type of enum
 *
 * @param {*} db
 * @param {*} enum_name
 * @param {*} packageIds
 */
function dataTypeForEnum(db, enum_name, packageIds) {
  return queryZcl.selectEnumByName(db, enum_name, packageIds).then((e) => {
    if (e == null) {
      return `!!Invalid enum: ${enum_name}`
    } else {
      return asCliType(e.size, false)
    }
  })
}

/**
 * Returns the number by adding 1 to it.
 * @param {*} number
 */
function addOne(number) {
  return number + 1
}

/**
 * Return true if number1 is greater than number2
 * @param num1
 * @param num2
 * @returns true if num1 is greater than num2 else returns false
 */
function is_number_greater_than(num1, num2) {
  return num1 > num2
}

const dep = templateUtil.deprecatedHelper

// WARNING! WARNING! WARNING! WARNING! WARNING! WARNING!
//
// Note: these exports are public API. Templates that might have been created in the past and are
// available in the wild might depend on these names.
// If you rename the functions, you need to still maintain old exports list.
exports.as_hex = asHex
exports.asHex = dep(asHex, { to: 'as_hex' })
exports.as_type = asType
exports.asType = dep(asType, { to: 'as_type' })
exports.as_symbol = asSymbol
exports.asSymbol = dep(asSymbol, { to: 'as_symbol' })
exports.as_bytes = asBytes
exports.asBytes = dep(asBytes, { to: 'as_bytes' })
exports.as_delimited_macro = asDelimitedMacro
exports.asDelimitedMacro = dep(asDelimitedMacro, { to: 'as_delimited_macro' })
exports.as_offset = asOffset
exports.asOffset = dep(asOffset, { to: 'as_offset' })
exports.as_underlying_type = asUnderlyingType
exports.asUnderlyingType = dep(asUnderlyingType, { to: 'as_underlying_type' })
exports.as_camel_cased = asCamelCased
exports.asCamelCased = dep(asCamelCased, { to: 'as_camel_cased' })

exports.cleanse_label = cleanseLabel
exports.cleanseLabel = dep(cleanseLabel, { to: 'cleanse_label' })

exports.as_snake_case = asUnderscoreLowercase
exports.as_underscore_lowercase = asUnderscoreLowercase
exports.asUnderscoreLowercase = dep(asUnderscoreLowercase, {
  to: 'as_underscore_lowercase',
})
exports.as_spaced_lowercase = asSpacedLowercase
exports.asSpacedLowercase = dep(asSpacedLowercase, {
  to: 'as_spaced_lowercase',
})
exports.as_underscore_uppercase = asUnderscoreUppercase
exports.asUnderscoreUppercase = dep(asUnderscoreUppercase, {
  to: 'as_underscore_uppercase',
})
exports.as_cli_type = asCliType
exports.asCliType = dep(asCliType, { to: 'as_cli_type' })

exports.data_type_for_bitmap = dataTypeForBitmap
exports.dataTypeForBitmap = dep(dataTypeForBitmap, {
  to: 'data_type_for_bitmap',
})
exports.data_type_for_enum = dataTypeForEnum
exports.dataTypeForEnum = dep(dataTypeForEnum, { to: 'data_type_for_enum' })
exports.add_one = addOne
exports.addOne = dep(addOne, { to: 'add_one' })
exports.cleanse_label_as_kebab_case = cleanseLabelAsKebabCase
exports.cleanseLabelAsKebabCase = dep(cleanseLabelAsKebabCase, {
  to: 'cleanse_label_as_kebab_case',
})
exports.format_value = formatValue
exports.formatValue = dep(formatValue, { to: 'format_value' })
exports.as_zcl_cli_type = as_zcl_cli_type
exports.is_number_greater_than = is_number_greater_than
