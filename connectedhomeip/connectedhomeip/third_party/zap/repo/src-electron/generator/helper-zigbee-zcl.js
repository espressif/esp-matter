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

const queryZcl = require('../db/query-zcl')
const dbEnum = require('../../src-shared/db-enum')
const templateUtil = require('./template-util')

/**
 *
 * @param {*} size
 * @param {*} type
 * @param {*} allowZclTypes
 * @returns The size in bits for a cli type based on allowZclTypes
 */
async function get_cli_size(size, type, allowZclTypes) {
  let res = size ? size * 8 : type.toLowerCase().match(/\d+/g).join('')
  if (res == 24 && !allowZclTypes) {
    res = 32
  }
  return res
}
/**
 *
 * @param {*} type
 * @param {*} cliPrefix
 * @param {*} context
 * @param {*} options
 * @returns the zcl cli data type string with the cli prefix given
 */
async function zcl_command_argument_type_to_cli_data_type_util(
  type,
  cliPrefix,
  allowZclTypes,
  context,
  options
) {
  const packageIds = await templateUtil.ensureZclPackageIds(context)
  let arrayExtension = 'isArray' in context && context.isArray ? 'OPT' : ''
  let dataType = await queryZcl.selectDataTypeByName(
    context.global.db,
    type,
    packageIds
  )
  if (dataType) {
    if (dataType.discriminatorName.toLowerCase() == dbEnum.zclType.bitmap) {
      let bitmap = await queryZcl.selectBitmapByName(
        context.global.db,
        packageIds,
        dataType.name
      )
      let bitmapSize = await get_cli_size(bitmap.size, type, allowZclTypes)
      return cliPrefix + '_UINT' + bitmapSize + arrayExtension
    } else if (
      dataType.discriminatorName.toLowerCase() == dbEnum.zclType.enum
    ) {
      let en = await queryZcl.selectEnumByName(
        context.global.db,
        dataType.name,
        packageIds
      )
      let enumSize = await get_cli_size(en.size, type, allowZclTypes)
      return cliPrefix + '_UINT' + enumSize + arrayExtension
    } else if (
      dataType.discriminatorName.toLowerCase() == dbEnum.zclType.number
    ) {
      let number = await queryZcl.selectNumberByName(
        context.global.db,
        packageIds,
        dataType.name
      )
      let numSize = await get_cli_size(number.size, type, allowZclTypes)
      if (numSize > 32) {
        return cliPrefix + '_HEX' + arrayExtension
      }
      let numType = '_' + (number.isSigned ? '' : 'U') + 'INT' + numSize
      return cliPrefix + numType + arrayExtension
    } else if (
      dataType.discriminatorName.toLowerCase() == dbEnum.zclType.struct
    ) {
      return cliPrefix + '_HEX' + arrayExtension
    } else if (
      dataType.discriminatorName.toLowerCase() == dbEnum.zclType.string
    ) {
      return cliPrefix + '_STRING' + arrayExtension
    } else {
      return ''
    }
  } else {
    return ''
  }
}

/**
 * Helper that deals with the type of the argument.
 *
 * @param {*} typeName
 * @param {*} options
 */
async function zcl_command_argument_type_to_cli_data_type(type, options) {
  return zcl_command_argument_type_to_cli_data_type_util(
    type,
    'SL_CLI_ARG',
    false,
    this,
    options
  )
}

/**
 * Helper that deals with the type of the argument.
 *
 * @param {*} typeName
 * @param {*} options
 */
async function zcl_command_argument_type_to_zcl_cli_data_type(type, options) {
  return zcl_command_argument_type_to_cli_data_type_util(
    type,
    'SL_ZCL_CLI_ARG',
    true,
    this,
    options
  )
}

exports.zcl_command_argument_type_to_cli_data_type =
  zcl_command_argument_type_to_cli_data_type
exports.zcl_command_argument_type_to_zcl_cli_data_type =
  zcl_command_argument_type_to_zcl_cli_data_type
