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

const queryPackage = require('../db/query-package.js')
const queryEndpointType = require('../db/query-endpoint-type.js')
const dbEnum = require('../../src-shared/db-enum.js')
const env = require('../util/env')
const _ = require('lodash')

/**
 * @module JS API: generator logic
 */

/**
 * All promises used by the templates should be synchronizable.
 *
 * @param {*} promise
 */
function makeSynchronizablePromise(promise) {
  // If promise is already synchronizable, just return it.
  if (promise.isResolved) return promise

  // Set initial state of flags
  let isPending = true
  let isRejected = false
  let isResolved = false

  // Resolve the promise, observing its rejection or resolution.
  let synchronizablePromise = promise.then(
    function (resolutionValue) {
      isResolved = true
      isPending = false
      return resolutionValue
    },
    function (rejectionError) {
      isRejected = true
      isPending = false
      throw rejectionError
    }
  )

  // Inject check functions.
  synchronizablePromise.isResolved = function () {
    return isResolved
  }
  synchronizablePromise.isPending = function () {
    return isPending
  }
  synchronizablePromise.isRejected = function () {
    return isRejected
  }
  return synchronizablePromise
}

/**
 * Helpful function that collects the individual blocks by using elements of an array as a context,
 * executing promises for each, and collecting them into the outgoing string.
 *
 * @param {*} resultArray
 * @param {*} options Options passed from a block helper.
 * @param {*} context The context from within this was called.
 * @returns Promise that resolves with a content string.
 */
async function collectBlocks(resultArray, options, context) {
  let promises = []
  let index = 0

  resultArray.forEach((element) => {
    let newContext = {
      global: context.global,
      parent: context,
      index: index++,
      count: resultArray.length,
      ...element,
    }
    let block = options.fn(newContext)
    promises.push(block)
  })

  // The else block gets executed if the list is empty.
  if (resultArray.length == 0) {
    promises.push(
      options.inverse({
        global: context.global,
        parent: context,
      })
    )
  }

  return Promise.all(promises).then((blocks) => {
    let ret = ''
    blocks.forEach((b) => {
      ret = ret.concat(b)
    })
    return ret
  })
}

/**
 * Returns the promise that resolves with the ZCL properties package id.
 *
 * @param {*} context
 * @returns promise that resolves with the package id.
 */
async function ensureZclPackageId(context) {
  if ('zclPackageId' in context.global) {
    return context.global.zclPackageId
  } else {
    let pkgs = await queryPackage.getSessionPackagesByType(
      context.global.db,
      context.global.sessionId,
      dbEnum.packageType.zclProperties
    )
    if (pkgs.length == 0) {
      return null
    } else {
      context.global.zclPackageId = pkgs[0].id
      return pkgs[0].id
    }
  }
}

/**
 * Returns the promise that resolves with all ZCL package id specific to current session.
 *
 * @param {*} context
 * @returns promise that resolves with a list of package id.
 */
async function ensureZclPackageIds(context) {
  if ('zclPackageIds' in context.global) {
    return context.global.zclPackageIds
  } else {
    let pkgs = await queryPackage.getSessionZclPackageIds(
      context.global.db,
      context.global.sessionId
    )
    context.global.zclPackageIds = pkgs

    return pkgs
  }
}

/**
 * Returns the promise that resolves with the ZCL properties package id.
 *
 * @param {*} context
 * @returns promise that resolves with the package id.
 */
async function ensureTemplatePackageId(context) {
  if ('templatePackageId' in context.global) {
    return context.global.templatePackageId
  } else {
    let pkgs = await queryPackage.getSessionPackagesByType(
      context.global.db,
      context.global.sessionId,
      dbEnum.packageType.genTemplatesJson
    )
    if (pkgs.length == 0) {
      return null
    } else {
      context.global.templatePackageId = pkgs[0].id
      return pkgs[0].id
    }
  }
}

/**
 * Populate the endpoint type ids into the global context.
 * @param {*} context
 * @returns endpoint type ids
 */
async function ensureEndpointTypeIds(context) {
  if ('endpointTypeIds' in context.global) {
    return context.global.endpointTypeIds
  } else {
    let epts = await queryEndpointType.selectEndpointTypeIds(
      context.global.db,
      context.global.sessionId
    )
    context.global.endpointTypeIds = epts
    return epts
  }
}

/**
 * Resolves with cached cluster extensions, but if they don't
 * exist, it will populate them.
 *
 * @param {*} context
 * @param {*} templatePackageId
 * @returns promise that resolves with cluster extensions.
 */
async function ensureZclClusterSdkExtensions(context, templatePackageId) {
  if ('zclClusterSdkExtension' in context.global) {
    return context.global.zclClusterSdkExtension
  } else {
    let extensions = await queryPackage.selectPackageExtension(
      context.global.db,
      templatePackageId,
      dbEnum.packageExtensionEntity.cluster
    )
    context.global.zclClusterSdkExtension = extensions
    return extensions
  }
}

/**
 * Resolves with cached cluster extensions, but if they don't
 * exist, it will populate them.
 *
 * @param {*} context
 * @param {*} templatePackageId
 * @returns promise that resolves with cluster extensions.
 */
async function ensureZclDeviceTypeSdkExtensions(context, templatePackageId) {
  if ('zclDeviceTypeExtension' in context.global) {
    return context.global.zclDeviceTypeExtension
  } else {
    let extensions = await queryPackage.selectPackageExtension(
      context.global.db,
      templatePackageId,
      dbEnum.packageExtensionEntity.deviceType
    )
    context.global.zclDeviceTypeExtension = extensions
    return extensions
  }
}

/**
 * Resolves with cached attribute extensions, but if they don't
 * exist, it will populate them.
 *
 * @param {*} context
 * @param {*} templatePackageId
 * @returns promise that resolves with attribute extensions.
 */
async function ensureZclAttributeSdkExtensions(context, templatePackageId) {
  if ('zclAttributeSdkExtension' in context.global) {
    return context.global.zclAttributeSdkExtension
  } else {
    let extensions = await queryPackage.selectPackageExtension(
      context.global.db,
      templatePackageId,
      dbEnum.packageExtensionEntity.attribute
    )
    context.global.zclAttributeSdkExtension = extensions
    return extensions
  }
}

/**
 * Resolves with cached attribute type extensions, but if they don't
 * exist, it will populate them.
 *
 * @param {*} context
 * @param {*} templatePackageId
 * @returns promise that resolves with attribute type extensions.
 */
async function ensureZclAttributeTypeSdkExtensions(context, templatePackageId) {
  if ('zclAttributeTypeSdkExtension' in context.global) {
    return context.global.zclAttributeTypeSdkExtension
  } else {
    let extensions = await queryPackage.selectPackageExtension(
      context.global.db,
      templatePackageId,
      dbEnum.packageExtensionEntity.attributeType
    )
    context.global.zclAttributeTypeSdkExtension = extensions
    return extensions
  }
}

/**
 * Resolves with cached command extensions, but if they don't
 * exist, it will populate them.
 *
 * @param {*} context
 * @param {*} templatePackageId
 * @returns promise that resolves with command extensions.
 */
async function ensureZclCommandSdkExtensions(context, templatePackageId) {
  if ('zclCommandSdkExtension' in context.global) {
    return context.global.zclCommandSdkExtension
  } else {
    let extensions = await queryPackage.selectPackageExtension(
      context.global.db,
      templatePackageId,
      dbEnum.packageExtensionEntity.command
    )
    context.global.zclCommandSdkExtension = extensions
    return extensions
  }
}

/**
 * Resolves with cached command extensions, but if they don't
 * exist, it will populate them.
 *
 * @param {*} context
 * @param {*} templatePackageId
 * @returns promise that resolves with command extensions.
 */
async function ensureZclEventSdkExtensions(context, templatePackageId) {
  if ('zclEventSdkExtension' in context.global) {
    return context.global.zclEventSdkExtension
  } else {
    let extensions = await queryPackage.selectPackageExtension(
      context.global.db,
      templatePackageId,
      dbEnum.packageExtensionEntity.event
    )
    context.global.zclEventSdkExtension = extensions
    return extensions
  }
}

/**
 * Every helper that returns a promise, should
 * not return the promise directly. So instead of
 * returning the promise directly, it should return:
 *    return templatePromise(this.global, promise)
 *
 * This will ensure that after tag works as expected.
 *
 * @param {*} global
 * @param {*} promise
 */
function templatePromise(global, promise) {
  let syncPromise = makeSynchronizablePromise(promise)
  global.promises.push(syncPromise)
  return syncPromise
}
/**
 * Function wrapper that can be used when a helper is deprecated.
 *
 * @param {*} fn
 * @param {*} explanation can contain `text`, or `from`/`to`, or just be a string message itself.
 * @returns a function that wraps the original function, with deprecation message.
 */
function deprecatedHelper(fn, explanation) {
  let msg
  let to = null
  if (explanation == null) {
    msg = `Deprecated helper resolved into ${fn.name}. Please use the new helper directly.`
  } else if (_.isString(explanation)) {
    msg = explanation
  } else if ('text' in explanation) {
    msg = explanation.text
  } else if ('from' in explanation && 'to' in explanation) {
    msg = `Helper ${explanation.from} is deprecated. Use ${explanation.to} instead.`
    to = explanation.to
  } else if ('to' in explanation) {
    msg = `Helper ${fn.name} is deprecated. Use ${explanation.to} instead.`
    to = explanation.to
  } else if ('from' in explanation) {
    msg = `Helper ${explanation.from} is deprecated. Use ${fn.name} instead.`
  } else {
    msg = `Deprecated helper resolved into ${fn.name}. Please use the new helper directly.`
  }
  let f = function () {
    if (
      this.global != undefined &&
      this.global.disableDeprecationWarnings != true &&
      this.global.deprecationWarnings != undefined &&
      this.global.deprecationWarnings[fn.name] == null
    ) {
      this.global.deprecationWarnings[fn.name] = true
      env.logWarning(`${this.global.templatePath} : ${msg}`)
    }
    return fn.apply(this, arguments)
  }
  f.originalHelper = fn.name
  f.isDeprecated = true
  if (to != null) f.replacementHelper = to
  return f
}

exports.collectBlocks = collectBlocks
exports.ensureZclPackageId = ensureZclPackageId
exports.ensureZclPackageIds = ensureZclPackageIds
exports.ensureTemplatePackageId = ensureTemplatePackageId
exports.ensureZclClusterSdkExtensions = ensureZclClusterSdkExtensions
exports.ensureZclAttributeSdkExtensions = ensureZclAttributeSdkExtensions
exports.ensureZclAttributeTypeSdkExtensions =
  ensureZclAttributeTypeSdkExtensions
exports.ensureZclCommandSdkExtensions = ensureZclCommandSdkExtensions
exports.ensureZclEventSdkExtensions = ensureZclEventSdkExtensions
exports.ensureZclDeviceTypeSdkExtensions = ensureZclDeviceTypeSdkExtensions
exports.ensureEndpointTypeIds = ensureEndpointTypeIds
exports.makeSynchronizablePromise = makeSynchronizablePromise
exports.templatePromise = templatePromise
exports.deprecatedHelper = deprecatedHelper
