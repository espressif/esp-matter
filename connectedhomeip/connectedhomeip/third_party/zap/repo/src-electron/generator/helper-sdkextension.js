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

const dbEnum = require('../../src-shared/db-enum.js')
const templateUtil = require('./template-util.js')
const util = require('../util/util.js')

/**
 * This module contains the API for accessing SDK extensions.
 *
 * @module Templating API: C formatting helpers
 */

async function clusterExtension(context, prop, clusterCode, role) {
  return templateUtil
    .ensureTemplatePackageId(context)
    .then((packageId) =>
      templateUtil.ensureZclClusterSdkExtensions(context, packageId)
    )
    .then((extensions) =>
      util.getClusterExtensionDefault(extensions, prop, clusterCode, role)
    )
}

/**
 * When inside a context that contains 'code', this
 * helper will output the value of the cluster extension
 * specified by property="propName" attribute.
 *
 * @param {*} options
 * @returns Value of the cluster extension property.
 */
function cluster_extension(options) {
  let prop = options.hash.property
  let role = options.hash.role
  let code = options.hash.code
  if (code == null) {
    code = 'code'
  }
  if (prop == null) {
    return ''
  } else {
    return clusterExtension(this, prop, this[code], role)
  }
}

/**
 * When inside a context that contains 'code', this
 * helper will output the value of the cluster extension
 * specified by property="propName" attribute.
 *
 * @param {*} options
 * @returns Value of the cluster extension property.
 */
async function device_type_extension(options) {
  let prop = options.hash.property
  if (prop == null) return ''

  let packageId = await templateUtil.ensureTemplatePackageId(this)
  let extensions = await templateUtil.ensureZclDeviceTypeSdkExtensions(
    this,
    packageId
  )

  let f = extensions.filter((x) => x.property == prop)
  if (f.length == 0) {
    return ''
  } else {
    let val = null
    f[0].defaults.forEach((d) => {
      if (d.entityCode == this.code) val = d.value
      if (d.entityCode == this.label) val = d.value
    })
    if (val == null) val = f[0].globalDefault
    if (val == null) val = ''
    return val
  }
}

/**
 * When inside a context that contains 'type', this
 * helper will output the value of the attribute type extension
 * specified by property="propName" attribute.
 *
 * @param {*} options
 * @returns Value of the attribute type extension property.
 */
async function attribute_type_extension(options) {
  let prop = options.hash.property
  if (prop == null) return ''

  let packageId = await templateUtil.ensureTemplatePackageId(this)
  let extensions = await templateUtil.ensureZclAttributeTypeSdkExtensions(
    this,
    packageId
  )
  let f = extensions.filter((x) => x.property == prop)
  if (f.length == 0) {
    return ''
  } else {
    let val = null
    f[0].defaults.forEach((d) => {
      if (d.entityQualifier == this.type) val = d.value
    })
    if (val == null) val = f[0].globalDefault
    if (val == null) val = ''
    return val
  }
}

async function subentityExtension(context, prop, entityType) {
  if (prop == null) return ''

  let packageId = await templateUtil.ensureTemplatePackageId(context)

  let extensions
  if (entityType == dbEnum.packageExtensionEntity.attribute) {
    extensions = await templateUtil.ensureZclAttributeSdkExtensions(
      context,
      packageId
    )
  } else if (entityType == dbEnum.packageExtensionEntity.command) {
    extensions = await templateUtil.ensureZclCommandSdkExtensions(
      context,
      packageId
    )
  } else if (entityType == dbEnum.packageExtensionEntity.event) {
    extensions = await templateUtil.ensureZclEventSdkExtensions(
      context,
      packageId
    )
  } else {
    throw new Error(`Invalid subentity: ${entityType}`)
  }

  let f = extensions.filter((x) => x.property == prop)
  if (f.length == 0) return ''

  let val = null

  // Iterate over all the extension defaults for this property
  f[0].defaults.forEach((d) => {
    let clusterCode = context.clusterCode

    // ClusterCode may be on the parent.
    if (clusterCode == null && context.parent) clusterCode = context.parent.code

    if (d.entityCode == context.code && d.parentCode == clusterCode) {
      // Now let's deal with qualifier:
      if (
        d.entityQualifier != null &&
        entityType == dbEnum.packageExtensionEntity.command
      ) {
        if (
          d.entityQualifier == context.source ||
          d.entityQualifier == context.commandSource
        ) {
          val = d.value
        }
      } else {
        // No special conditions, we match
        if (val == null) val = d.value
      }
    }
  })
  // Wasn't set, set global default
  if (val == null) val = f[0].globalDefault

  // No global default either, use empty string.
  if (val == null) val = ''

  return val
}

function if_command_extension_true(options) {
  let prop = options.hash.property
  if (prop == '') return ''

  return subentityExtension(
    this,
    prop,
    dbEnum.packageExtensionEntity.command
  ).then((val) => {
    if (val == true || val == 1) {
      return options.fn(this)
    } else {
      return ''
    }
  })
}

function if_command_extension_false(options) {
  let prop = options.hash.property
  if (prop == '') return ''

  return subentityExtension(
    this,
    prop,
    dbEnum.packageExtensionEntity.command
  ).then((val) => {
    if (val == false || val == 0) {
      return options.fn(this)
    } else {
      return ''
    }
  })
}

function if_cluster_extension_true(options) {
  let prop = options.hash.property
  let code = options.hash.code
  if (prop == '') return ''
  if (code == null) code = 'clusterCode'

  return clusterExtension(this, prop, this[code], null).then((val) => {
    if (val == true || val == 1) {
      return options.fn(this)
    } else {
      return ''
    }
  })
}

function if_cluster_extension_false(options) {
  let prop = options.hash.property
  let code = options.hash.code
  if (prop == '') return ''
  if (code == null) code = 'clusterCode'

  return clusterExtension(this, prop, this[code], null).then((val) => {
    if (val == false || val == 0) {
      return options.fn(this)
    } else {
      return ''
    }
  })
}

/**
 * When inside a context that contains 'code' and parent 'code', this
 * helper will output the value of the attribute extension
 * specified by property="propName" attribute.
 *
 * @param {*} options
 * @returns Value of the attribute extension property.
 */
function attribute_extension(options) {
  let prop = options.hash.property
  return subentityExtension(this, prop, dbEnum.packageExtensionEntity.attribute)
}

/**
 * When inside a context that contains 'code' and parent 'code', this
 * helper will output the value of the command extension
 * specified by property="propName" attribute.
 *
 * @param {*} options
 * @returns Value of the command extension property.
 */
function command_extension(options) {
  let prop = options.hash.property
  return subentityExtension(this, prop, dbEnum.packageExtensionEntity.command)
}

/**
 * When inside a context that contains 'code' and parent 'code', this
 * helper will output the value of the command extension
 * specified by property="propName" attribute.
 *
 * @param {*} options
 * @returns Value of the command extension property.
 */
function event_extension(options) {
  let prop = options.hash.property
  return subentityExtension(this, prop, dbEnum.packageExtensionEntity.event)
}

// WARNING! WARNING! WARNING! WARNING! WARNING! WARNING!
//
// Note: these exports are public API. Templates that might have been created in the past and are
// available in the wild might depend on these names.
// If you rename the functions, you need to still maintain old exports list.

exports.cluster_extension = cluster_extension
exports.command_extension = command_extension
exports.event_extension = event_extension
exports.attribute_extension = attribute_extension
exports.attribute_type_extension = attribute_type_extension
exports.device_type_extension = device_type_extension
exports.if_command_extension_true = if_command_extension_true
exports.if_command_extension_false = if_command_extension_false
exports.if_cluster_extension_true = if_cluster_extension_true
exports.if_cluster_extension_false = if_cluster_extension_false
