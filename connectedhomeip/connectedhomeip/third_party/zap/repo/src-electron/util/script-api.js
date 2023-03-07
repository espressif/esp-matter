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

/**
 * This module contains the API functions for the post-load
 * scripting functionality.
 *
 *  @module JS API: post-import.
 */
const queryEndpoint = require('../db/query-endpoint.js')
const queryConfig = require('../db/query-config.js')
const dbEnum = require('../../src-shared/db-enum.js')
const queryPackage = require('../db/query-package.js')
const querySessionZcl = require('../db/query-session-zcl.js')
const queryZcl = require('../db/query-zcl.js')
const restApi = require('../../src-shared/rest-api.js')

/**
 * Prints a text to console.
 *
 * @param {*} text
 */
function print(text) {
  console.log(text)
}

/**
 * Prints error message to the console.
 *
 * @param {*} text
 */
function printError(text) {
  console.log(`⛔ SCRIPT API ERROR: ${text}`)
}

/**
 * Returns an array of endpoints.
 *
 * @param {*} context
 */
function endpoints(context) {
  return queryEndpoint.selectAllEndpoints(context.db, context.sessionId)
}

/**
 * Deletes an endpoint
 *
 * @param {*} context
 * @param {*} endpoint
 */
function deleteEndpoint(context, endpoint) {
  return queryEndpoint.deleteEndpoint(context.db, endpoint.id)
}

/**
 * Returns an array of clusters defined on a given endpoint.
 *
 * @param {*} context
 * @param {*} endpoint
 */
function clusters(context, endpoint) {
  return queryEndpoint.selectEndpointClusters(
    context.db,
    endpoint.endpointTypeRef
  )
}

/**
 * Returns an array of attributes for a given cluster.
 * The cluster input is required to come from a script-api in this module.
 *
 * @param {*} context
 * @param {*} endpoint
 * @param {*} cluster
 */
function attributes(context, endpoint, cluster) {
  return queryEndpoint.selectEndpointClusterAttributes(
    context.db,
    cluster.clusterId,
    cluster.side,
    endpoint.endpointTypeRef
  )
}

/**
 * Returns an array of commands for a given cluster
 * The clusters input is required to come from a script-api in this module.
 *
 * @param {*} context
 * @param {*} endpoint
 * @param {*} cluster
 */
function commands(context, endpoint, cluster) {
  return queryEndpoint.selectEndpointClusterCommands(
    context.db,
    cluster.clusterId,
    endpoint.endpointTypeRef
  )
}

/**
 * Returns array of function names available in this module.
 */
function functions() {
  return Object.keys(exports)
}

/**
 * Returns the session id in the context.
 *
 * @param {*} context
 * @returns sessionId
 */
function sessionId(context) {
  return context.sessionId
}

function dbEnums() {
  return dbEnum
}

/**
 * Returns all available clusters.
 *
 * @param {*} context
 * @returns all available clusters
 */
async function availableClusters(context) {
  return querySessionZcl.selectAllSessionClusters(context.db, context.sessionId)
}

// Finds the cluster database primary key from code,manufacturing code, and context.
// Note that by default, a standard ZCL library cluster will have manufacturing code of null
// in the database.
async function findCluster(context, code, mfgCode = null) {
  return querySessionZcl.selectSessionClusterByCode(
    context.db,
    context.sessionId,
    code,
    mfgCode
  )
}

async function findAttribute(
  context,
  clusterCode,
  side,
  attributeCode,
  mfgCode = null
) {
  return querySessionZcl.selectSessionAttributeByCode(
    context.db,
    context.sessionId,
    clusterCode,
    side,
    attributeCode,
    mfgCode
  )
}

async function findCommand(context, clusterCode, commandCode, source) {
  return querySessionZcl.selectSessionCommandByCode(
    context.db,
    context.sessionId,
    clusterCode,
    commandCode,
    source
  )
}

// Non-public, common function to modify cluster.
async function modifyCluster(context, endpoint, code, side, enabled) {
  let cluster = await findCluster(context, code)
  if (cluster == null) {
    printError(`Cluster 0x${code.toString(16)} does not exist.`)
    return null
  }
  return queryConfig.insertOrReplaceClusterState(
    context.db,
    endpoint.endpointTypeRef,
    cluster.id,
    side,
    enabled
  )
}

// Non-public, common function to modify attribute.
async function modifyAttribute(
  context,
  endpoint,
  clusterCode,
  attributeCode,
  side,
  enable
) {
  let cluster = await findCluster(context, clusterCode)
  if (cluster == null) {
    printError(`Cluster 0x${clusterCode.toString(16)} does not exist.`)
    return null
  }
  let attribute = await findAttribute(context, clusterCode, side, attributeCode)
  if (attribute == null) {
    printError(
      `Attribute 0x${attributeCode.toString(
        16
      )} in cluster 0x${clusterCode.toString(16)} does not exist.`
    )
    return null
  }

  let params = [
    {
      key: restApi.updateKey.attributeSelected,
      value: enable,
    },
  ]
  return queryConfig.insertOrUpdateAttributeState(
    context.db,
    endpoint.endpointTypeRef,
    cluster.id,
    side,
    attribute.id,
    params,
    attribute.reportMinInterval,
    attribute.reportMaxInterval,
    attribute.reportableChange
  )
}

// Non-public, common function to modify command.
async function modifyCommand(
  context,
  endpoint,
  clusterCode,
  commandCode,
  source,
  isIncoming,
  enable
) {
  let cluster = await findCluster(context, clusterCode)
  if (cluster == null) {
    printError(`Cluster 0x${clusterCode.toString(16)} does not exist.`)
    return null
  }
  let command = await findCommand(context, clusterCode, commandCode, source)
  if (command == null) {
    printError(
      `Command 0x${commandCode.toString(
        16
      )} in cluster 0x${clusterCode.toString(16)} does not exist.`
    )
    return null
  }
  return queryConfig.insertOrUpdateCommandState(
    context.db,
    endpoint.endpointTypeRef,
    cluster.id,
    command.source,
    command.id,
    enable ? 1 : 0,
    isIncoming
  )
}

/**
 * Disables the client cluster on an endpoint.
 * @param {*} context
 * @param {*} endpoint
 * @param {*} code
 */
async function disableClientCluster(context, endpoint, code) {
  return modifyCluster(context, endpoint, code, dbEnum.side.client, false)
}

/**
 * Disables the server cluster on an endpoint.
 * @param {*} context
 * @param {*} endpoint
 * @param {*} code
 */
async function disableServerCluster(context, endpoint, code) {
  return modifyCluster(context, endpoint, code, dbEnum.side.server, false)
}

/**
 * Enables the client cluster on an endpoint.
 * @param {*} context
 * @param {*} endpoint
 * @param {*} code
 */
async function enableClientCluster(context, endpoint, code) {
  return modifyCluster(context, endpoint, code, dbEnum.side.client, true)
}

/**
 * Enables the server cluster on an endpoint.
 * @param {*} context
 * @param {*} endpoint
 * @param {*} code
 */
async function enableServerCluster(context, endpoint, code) {
  return modifyCluster(context, endpoint, code, dbEnum.side.server, true)
}

/**
 * Disable client attribute.
 *
 * @param {*} context
 * @param {*} endpoint
 * @param {*} clusterCode
 * @param {*} attributeCode
 */
async function disableClientAttribute(
  context,
  endpoint,
  clusterCode,
  attributeCode
) {
  return modifyAttribute(
    context,
    endpoint,
    clusterCode,
    attributeCode,
    dbEnum.side.client,
    false
  )
}

/**
 * Enable client attribute.
 *
 * @param {*} context
 * @param {*} endpoint
 * @param {*} clusterCode
 * @param {*} attributeCode
 */
async function enableClientAttribute(
  context,
  endpoint,
  clusterCode,
  attributeCode
) {
  return modifyAttribute(
    context,
    endpoint,
    clusterCode,
    attributeCode,
    dbEnum.side.client,
    true
  )
}

/**
 * Disable server attribute.
 *
 * @param {*} context
 * @param {*} endpoint
 * @param {*} clusterCode
 * @param {*} attributeCode
 */
async function disableServerAttribute(
  context,
  endpoint,
  clusterCode,
  attributeCode
) {
  return modifyAttribute(
    context,
    endpoint,
    clusterCode,
    attributeCode,
    dbEnum.side.server,
    false
  )
}

/**
 * Enable server attribute.
 *
 * @param {*} context
 * @param {*} endpoint
 * @param {*} clusterCode
 * @param {*} attributeCode
 */
async function enableServerAttribute(
  context,
  endpoint,
  clusterCode,
  attributeCode
) {
  return modifyAttribute(
    context,
    endpoint,
    clusterCode,
    attributeCode,
    dbEnum.side.server,
    true
  )
}

/**
 * Disable incoming commands.
 * Source should be derived from dbEnums().source.client/server
 *
 * @param {*} context
 * @param {*} endpoint
 * @param {*} clusterCode
 * @param {*} commandCode
 * @param {*} source
 */
async function disableIncomingCommand(
  context,
  endpoint,
  clusterCode,
  commandCode,
  source
) {
  return modifyCommand(
    context,
    endpoint,
    clusterCode,
    commandCode,
    source,
    true,
    false
  )
}

/**
 * Enable incoming commands.
 * Source should be derived from dbEnums().source.client/server
 *
 * @param {*} context
 * @param {*} endpoint
 * @param {*} clusterCode
 * @param {*} commandCode
 * @param {*} source
 */
async function enableIncomingCommand(
  context,
  endpoint,
  clusterCode,
  commandCode,
  source
) {
  return modifyCommand(
    context,
    endpoint,
    clusterCode,
    commandCode,
    source,
    true,
    true
  )
}

/**
 * Disable outgoing commands.
 * Source should be derived from dbEnums().source.client/server
 *
 * @param {*} context
 * @param {*} endpoint
 * @param {*} clusterCode
 * @param {*} commandCode
 * @param {*} source
 */
async function disableOutgoingCommand(
  context,
  endpoint,
  clusterCode,
  commandCode,
  source
) {
  return modifyCommand(
    context,
    endpoint,
    clusterCode,
    commandCode,
    source,
    false,
    false
  )
}

/**
 * Enable outgoing commands.
 * Source should be derived from dbEnums().source.client/server
 *
 * @param {*} context
 * @param {*} endpoint
 * @param {*} clusterCode
 * @param {*} commandCode
 * @param {*} source
 */
async function enableOutgoingCommand(
  context,
  endpoint,
  clusterCode,
  commandCode,
  source
) {
  return modifyCommand(
    context,
    endpoint,
    clusterCode,
    commandCode,
    source,
    false,
    true
  )
}

exports.availableClusters = availableClusters

exports.print = print
exports.functions = functions
exports.sessionId = sessionId
exports.dbEnums = dbEnums

exports.endpoints = endpoints
exports.deleteEndpoint = deleteEndpoint

exports.clusters = clusters
exports.attributes = attributes
exports.commands = commands

exports.findCluster = findCluster

exports.disableClientCluster = disableClientCluster
exports.disableServerCluster = disableServerCluster
exports.enableClientCluster = enableClientCluster
exports.enableServerCluster = enableServerCluster

exports.disableClientAttribute = disableClientAttribute
exports.enableClientAttribute = enableClientAttribute
exports.disableServerAttribute = disableServerAttribute
exports.enableServerAttribute = enableServerAttribute

exports.disableClientAttribute = disableClientAttribute
exports.enableClientAttribute = enableClientAttribute
exports.disableServerAttribute = disableServerAttribute
exports.enableServerAttribute = enableServerAttribute

exports.disableIncomingCommand = disableIncomingCommand
exports.enableIncomingCommand = enableIncomingCommand
exports.disableOutgoingCommand = disableOutgoingCommand
exports.enableOutgoingCommand = enableOutgoingCommand

// Constants that are used a lot
exports.client = dbEnum.source.client
exports.server = dbEnum.source.server
