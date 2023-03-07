/**
 *
 *    Copyright (c) 2021 Silicon Labs
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
function listComma(obj) {
  if (obj.index == obj.count - 1) return ''
  return ','
}

// Hack because ZAP is not parsing the bitmap types properly
function asUnderlyingTypeCustom(type) {
  switch (type) {
    case 'bool':
      return 'bool'
    case 'map8':
      return 'uint8_t'
    case 'map16':
      return 'uint16_t'
    case 'map32':
      return 'uint32_t'
    case 'uint8':
      return 'uint8_t'
    case 'uint16':
      return 'uint16_t'
    case 'uint24':
      return 'uint32_t'
    case 'uint32':
      return 'uint32_t'
    case 'uint64':
      return 'uint64_t'
    case 'int8':
      return 'int8_t'
    case 'int16':
      return 'int16_t'
    case 'int32':
      return 'int32_t'
    case 'int64':
      return 'int64_t'
    case 'octstr':
      return 'const char*'
    case 'string':
      return 'const char*'
    case 'attribId':
      return 'uint16_t'
    case 'bitmap8':
      return 'uint8_t'
    case 'key128':
      return 'const char*'
    case 'single':
      return 'float'
    case 'enum8':
      return 'uint8_t'
    case 'enum16':
      return 'uint16_t'
    case 'enum32':
      return 'uint32_t'
    case 'unk':
      return '/*Unknown default*/uint8_t'
    default:
      return type
  }
}

// Hack because ZAP is not parsing the bitmap types properly
function notStandardType(type) {
  switch (type) {
    case 'bool':
      return 'noBitmap'
    case 'map8':
      return this.name
    case 'map16':
      return this.name
    case 'map32':
      return this.name
    case 'uint8':
      return 'noBitmap'
    case 'uint16':
      return 'noBitmap'
    case 'uint24':
      return 'noBitmap'
    case 'uint32':
      return 'noBitmap'
    case 'uint64':
      return 'noBitmap'
    case 'int8':
      return 'noBitmap'
    case 'int16':
      return 'noBitmap'
    case 'int32':
      return 'noBitmap'
    case 'int64':
      return 'noBitmap'
    case 'octstr':
      return 'noBitmap'
    case 'string':
      return 'noBitmap'
    case 'attribId':
      return 'noBitmap'
    case 'bitmap8':
      return 'noBitmap'
    case 'key128':
      return 'noBitmap'
    case 'single':
      return 'noBitmap'
    case 'enum8':
      return 'noBitmap'
    case 'enum16':
      return 'noBitmap'
    case 'enum32':
      return 'noBitmap'
    case 'unk':
      return 'noBitmap'
    default:
      return type
  }
}

function supportedCluster(clusterName) {
  switch (clusterName) {
    case 'TouchlinkCommissioning':
      return false
    case 'IASACE':
      return false
    case 'Commissioning':
      return false
    default:
      return true
  }
}

function dotdot_prefix() {
  return 'uic_mqtt_dotdot'
}

function isEnum(type) {
  if (type.startsWith('enum')) {
    return true
  }
  return false
}

function isString(type) {
  switch (type) {
    case 'string':
      return true
    case 'octstr':
      return true
    default:
      return false
  }
}

function isMap(type) {
  if (type.startsWith('map')) {
    return true
  }
  return false
}

function isSpecialType(type) {
  if (type == 'SHDCFGDirection' || type == 'IaswdLevel') {
    return true
  }
  return false
}

function isBool(type) {
  switch (type) {
    case 'bool':
      return true
    default:
      return false
  }
}

function isSameString(string1, string2) {
  return string1 == string2
}

function isSubstringInString(sub_string, string) {
  return string.includes(sub_string)
}

function asCleanSymbol(label) {
  let l = label.trim()
  l = l.replace(' ', '_')
  l = l.replace(' ', '_')
  l = l.replace(/[:/-]/g, '_')
  l = l.replace(/__+/g, '_').toLowerCase()
  return l
}

function asSnakeCaseLower(str) {
  if (!str) {
    return str
  }
  let label = str.replace(/\.?([A-Z][a-z])/g, function (x, y) {
    return '_' + y
  })
  label = asCleanSymbol(label)
  if (label.startsWith('_')) {
    label = label.substring(1)
  }
  return label
}

function asSnakeCaseUpper(str) {
  return asSnakeCaseLower(str).toUpperCase()
}

function asAttributeIdDefine(parentLabel, label) {
  return (
    'DOTDOT_ATTRIBUTE_ID_' +
    asSnakeCaseUpper(parentLabel) +
    '_' +
    asSnakeCaseUpper(label)
  )
}

function asAttributeGetFunction(parentLabel, label) {
  return (
    'dotdot_get_' +
    asSnakeCaseLower(parentLabel) +
    '_' +
    asSnakeCaseLower(label)
  )
}

function asAttributeSetFunction(parentLabel, label) {
  return (
    'dotdot_set_' +
    asSnakeCaseLower(parentLabel) +
    '_' +
    asSnakeCaseLower(label)
  )
}

function asAttributeIsSupportedFunction(parentLabel, label) {
  return (
    'dotdot_is_supported_' +
    asSnakeCaseLower(parentLabel) +
    '_' +
    asSnakeCaseLower(label)
  )
}

function asCommandCallback(zclCommand) {
  return (
    dotdot_prefix() +
    '_' +
    asSnakeCaseLower(zclCommand.parent.label) +
    '_' +
    asSnakeCaseLower(zclCommand.label) +
    '_callback'
  )
}

/**
 * Helper function to build the hex representation of an attribute, the argument
 * must be the attribute javascript object
 */
function attributeID(attribute) {
  if (attribute.parent.code == 0) {
    // Shift cluster id 0 (Basic) to 0xBA5C as the 0-space is used for Attribute Type and Z-Wave CC attributes
    attribute.parent.code = 0xba5c
  }
  return (attribute.parent.code * 0x10000 + attribute.code)
    .toString(16)
    .padStart(8, 0)
}

exports.isBool = isBool
exports.isString = isString
exports.isEnum = isEnum
exports.isMap = isMap
exports.notStandardType = notStandardType
exports.isSubstringInString = isSubstringInString
exports.asUnderlyingTypeCustom = asUnderlyingTypeCustom
exports.isSpecialType = isSpecialType
exports.isSameString = isSameString
exports.dotdot_prefix = dotdot_prefix
exports.listComma = listComma
exports.supportedCluster = supportedCluster
exports.asSnakeCaseLower = asSnakeCaseLower
exports.asSnakeCaseUpper = asSnakeCaseUpper
exports.asAttributeIdDefine = asAttributeIdDefine
exports.asAttributeGetFunction = asAttributeGetFunction
exports.asAttributeSetFunction = asAttributeSetFunction
exports.asAttributeIsSupportedFunction = asAttributeIsSupportedFunction
exports.attributeID = attributeID
exports.asCommandCallback = asCommandCallback
