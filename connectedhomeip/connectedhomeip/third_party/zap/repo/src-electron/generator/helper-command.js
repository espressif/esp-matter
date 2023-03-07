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

const queryCommand = require('../db/query-command.js')
const templateUtil = require('./template-util.js')
const types = require('../util/types.js')

/**
 *
 * @param {*} commandId
 * @param {*} argument_return
 * @param {*} no_argument_return
 *
 * If the command arguments for a command exist then returns argument_return
 * else returns no_argument_return
 * Example: {{if_command_arguments_exist [command-id] "," ""}}
 * The above will return ',' if the command arguments for a command exist
 * and will return nothing if the command arguments for a command do not exist.
 *
 */
async function if_command_arguments_exist(
  commandId,
  argument_return,
  no_argument_return
) {
  let promise = queryCommand
    .selectCommandArgumentsCountByCommandId(this.global.db, commandId)
    .then((res) => {
      if (res > 0) {
        return argument_return
      } else {
        return no_argument_return
      }
    })

  return templateUtil.templatePromise(this.global, promise)
}

/**
 * If helper which checks if command arguments exist for a command or not
 * example:
 * {{#if_command_args_exist commandId}}
 *  command arguments exist for the command
 * {{else}}
 *  command arguments do not exist for the command
 * {{/if_command_args_exist}}
 *
 * @param commandId
 * @param options
 * @returns Returns content in the handlebar template based on whether the
 * command arguments are present or not.
 *
 */
async function if_command_args_exist(commandId, options) {
  let res = await queryCommand.selectCommandArgumentsCountByCommandId(
    this.global.db,
    commandId
  )
  if (res > 0) {
    return options.fn(this)
  }
  return options.inverse(this)
}

/**
 *
 * @param commandArg
 * @param trueReturn
 * @param falseReturn
 * @returns trueReturn if command argument is always present and there is a
 * presentIf condition else returns false
 */
function if_ca_always_present_with_presentif(
  commandArg,
  trueReturn,
  falseReturn
) {
  if (
    !(commandArg.introducedInRef || commandArg.removedInRef) &&
    commandArg.presentIf
  ) {
    return trueReturn
  } else {
    return falseReturn
  }
}

/**
 * If helper that checks if a command argument is always present with a
 * presentIf condition.
 * example:
 * {{#if_command_arg_always_present_with_presentif commandArg}}
 *  command argument has a presentIf condition
 * {{else}}
 *  command argument does not have a presentIf condition
 * {{/if_command_arg_always_present_with_presentif}}
 *
 * @param commandArg
 * @param options
 * @returns Returns content in the handlebar template based on the command
 * argument having a presentIf condition or not
 */
function if_command_arg_always_present_with_presentif(commandArg, options) {
  if (
    !(commandArg.introducedInRef || commandArg.removedInRef) &&
    commandArg.presentIf
  ) {
    return options.fn(this)
  } else {
    return options.inverse(this)
  }
}

/**
 *
 * @param command
 * @param commandArg
 * @param trueReturn
 * @param falseReturn
 * @returns trueReturn if command is not fixed length but command argument is
 * always present else returns falseReturn
 */
async function if_command_is_not_fixed_length_but_command_argument_is_always_present(
  commandId,
  commandArg,
  trueReturn,
  falseReturn
) {
  let commandArgs = await queryCommand.selectCommandArgumentsByCommandId(
    this.global.db,
    commandId
  )
  let isFixedLengthCommand = true
  for (let ca of commandArgs) {
    if (
      ca.isArray ||
      types.isString(ca.type) ||
      ca.introducedInRef ||
      ca.removedInRef ||
      ca.presentIf
    ) {
      isFixedLengthCommand = false
    }
  }

  if (isFixedLengthCommand) {
    return falseReturn
  } else if (
    !(
      commandArg.isArray ||
      commandArg.introducedInRef ||
      commandArg.removedInRef ||
      commandArg.presentIf
    )
  ) {
    return trueReturn
  } else {
    return falseReturn
  }
}

/**
 * If helper that checks if command is not fixed lenth and that the command is
 * always present.
 * example:
 * {{#if_command_not_fixed_length_command_argument_always_present commandId}}
 *  command is not fixed length and command argument is always present
 * {{else}}
 *  either command is fixed length or command argument is not always present
 * {{/if_command_not_fixed_length_command_argument_always_present}}
 *
 * @param command
 * @param commandArg
 * @param options
 * @returns Returns content in the handlebar template based on the command being
 * fixed length or not and whether the command argument is always present
 */
async function if_command_not_fixed_length_command_argument_always_present(
  command,
  commandArg,
  options
) {
  let commandArgs = await queryCommand.selectCommandArgumentsByCommandId(
    this.global.db,
    command
  )
  let isFixedLengthCommand = true
  for (let ca of commandArgs) {
    if (
      ca.isArray ||
      types.isString(ca.type) ||
      ca.introducedInRef ||
      ca.removedInRef ||
      ca.presentIf
    ) {
      isFixedLengthCommand = false
    }
  }

  if (isFixedLengthCommand) {
    return options.inverse(this)
  } else if (
    !(
      commandArg.isArray ||
      commandArg.introducedInRef ||
      commandArg.removedInRef ||
      commandArg.presentIf
    )
  ) {
    return options.fn(this)
  } else {
    return options.inverse(this)
  }
}

/**
 *
 * @param commandArg
 * @param trueReturn
 * @param falseReturn
 * @returns trueReturn if command argument is not always present and there is no
 * presentIf condition else returns false
 */
function if_ca_not_always_present_no_presentif(
  commandArg,
  trueReturn,
  falseReturn
) {
  if (
    (commandArg.introducedInRef || commandArg.removedInRef) &&
    !commandArg.presentIf
  ) {
    return trueReturn
  }
  return falseReturn
}

/**
 * If helper that checks if a command argument is not always present because it
 * has a introduced in or removed in clause. The helper also checks that there
 * is no presentIf condition.
 * example:
 * {{#if_command_arg_not_always_present_no_presentif commandArg}}
 *  command argument is not always present and there is no presentIf condition
 * {{else}}
 *  Either command argument is always present or there is a presentIf condition
 * {{/if_command_arg_not_always_present_no_presentif}}
 *
 * @param commandArg
 * @param options
 * @returns Returns content in the handlebar template based on the command
 * argument being present and if there is a presentIf condition.
 */
function if_command_arg_not_always_present_no_presentif(commandArg, options) {
  if (
    (commandArg.introducedInRef || commandArg.removedInRef) &&
    !commandArg.presentIf
  ) {
    return options.fn(this)
  }
  return options.inverse(this)
}

/**
 *
 * @param commandArg
 * @param trueReturn
 * @param falseReturn
 * @returns trueReturn if command argument is not always present and there is a
 * presentIf condition else returns false
 */
function if_ca_not_always_present_with_presentif(
  commandArg,
  trueReturn,
  falseReturn
) {
  if (
    (commandArg.introducedInRef || commandArg.removedInRef) &&
    commandArg.presentIf
  ) {
    return trueReturn
  } else {
    return falseReturn
  }
}

/**
 * If helper that checks if a command argument is not always present because it
 * has a introduced in or removed in clause. The helper also checks that there
 * is a presentIf condition.
 * example:
 * {{#if_command_arg_not_always_present_with_presentif commandArg}}
 *  command argument is not always present and there is a presentIf condition
 * {{else}}
 *  Either command argument is always present or there is no presentIf condition
 * {{/if_command_arg_not_always_present_with_presentif}}
 *
 * @param commandArg
 * @param options
 * @returns Returns content in the handlebar template based on the command
 * argument being present and if there is a presentIf condition.
 */
function if_command_arg_not_always_present_with_presentif(commandArg, options) {
  if (
    (commandArg.introducedInRef || commandArg.removedInRef) &&
    commandArg.presentIf
  ) {
    return options.fn(this)
  } else {
    return options.inverse(this)
  }
}

/**
 *
 * @param commandId
 * @param fixedLengthReturn
 * @param notFixedLengthReturn
 * Returns fixedLengthReturn or notFixedLengthReturn based on whether the
 * command is fixed length or not. Also checks if the command arguments are
 * always present or not.
 */
async function if_command_is_fixed_length(
  commandId,
  fixedLengthReturn,
  notFixedLengthReturn
) {
  let commandArgs = await queryCommand.selectCommandArgumentsByCommandId(
    this.global.db,
    commandId
  )

  let fixedLength = true
  for (let commandArg of commandArgs) {
    if (
      commandArg.isArray ||
      types.isString(commandArg.type) ||
      commandArg.introducedInRef ||
      commandArg.removedInRef ||
      commandArg.presentIf
    ) {
      fixedLength = false
      break
    }
  }

  if (fixedLength) {
    return fixedLengthReturn
  } else {
    return notFixedLengthReturn
  }
}

/**
 * If helper which checks if a command is fixed length or not
 *
 * example:
 * {{#if_command_fixed_length commandId}}
 * command is fixed length
 * {{else}}
 * command is not fixed length
 * {{/if_command_fixed_length}}
 *
 * @param commandId
 * @param options
 * Returns content in the handlebar template based on the command being fixed
 * length or not as shown in the example above.
 */
async function if_command_fixed_length(commandId, options) {
  let commandArgs = await queryCommand.selectCommandArgumentsByCommandId(
    this.global.db,
    commandId
  )
  for (let commandArg of commandArgs) {
    if (
      commandArg.isArray ||
      types.isString(commandArg.type) ||
      commandArg.introducedInRef ||
      commandArg.removedInRef ||
      commandArg.presentIf
    ) {
      return options.inverse(this)
    }
  }
  return options.fn(this)
}

// WARNING! WARNING! WARNING! WARNING! WARNING! WARNING!
//
// Note: these exports are public API. Templates that might have been created in the past and are
// available in the wild might depend on these names.
// If you rename the functions, you need to still maintain old exports list.
const dep = templateUtil.deprecatedHelper

exports.if_command_is_fixed_length = dep(if_command_is_fixed_length, {
  to: 'if_command_fixed_length',
})
exports.if_command_arguments_exist = dep(if_command_arguments_exist, {
  to: 'if_command_args_exist',
})
exports.if_ca_always_present_with_presentif = dep(
  if_ca_always_present_with_presentif,
  { to: 'if_command_arg_always_present_with_presentif' }
)
exports.if_command_is_not_fixed_length_but_command_argument_is_always_present =
  dep(if_command_is_not_fixed_length_but_command_argument_is_always_present, {
    to: 'if_command_not_fixed_length_command_argument_always_present',
  })
exports.if_ca_not_always_present_no_presentif = dep(
  if_ca_not_always_present_no_presentif,
  { to: 'if_command_arg_not_always_present_no_presentif' }
)
exports.if_ca_not_always_present_with_presentif = dep(
  if_ca_not_always_present_with_presentif,
  { to: 'if_command_arg_not_always_present_with_presentif' }
)
exports.if_command_fixed_length = if_command_fixed_length
exports.if_command_not_fixed_length_command_argument_always_present =
  if_command_not_fixed_length_command_argument_always_present
exports.if_command_arg_not_always_present_no_presentif =
  if_command_arg_not_always_present_no_presentif
exports.if_command_arg_not_always_present_with_presentif =
  if_command_arg_not_always_present_with_presentif
exports.if_command_arg_always_present_with_presentif =
  if_command_arg_always_present_with_presentif
exports.if_command_args_exist = if_command_args_exist
