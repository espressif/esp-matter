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
 * @module Templating API: user-data specific helpers
 */
const templateUtil = require('./template-util.js')
const queryImpexp = require('../db/query-impexp.js')
const queryCluster = require('../db/query-cluster.js')
const queryEndpointType = require('../db/query-endpoint-type.js')
const queryCommand = require('../db/query-command.js')
const queryAttribute = require('../db/query-attribute.js')
const queryConfig = require('../db/query-config.js')
const querySession = require('../db/query-session.js')
const helperZcl = require('./helper-zcl.js')
const dbEnum = require('../../src-shared/db-enum.js')
const iteratorUtil = require('../util/iterator-util.js')

/**
 * Creates block iterator over the endpoints.
 *
 * @param {*} options
 */
function user_endpoints(options) {
  let promise = templateUtil
    .ensureEndpointTypeIds(this)
    .then((endpointTypes) =>
      queryImpexp.exportEndpoints(
        this.global.db,
        this.global.sessionId,
        endpointTypes
      )
    )
    .then((endpoints) =>
      endpoints.map((x) => {
        x.endpointTypeId = x.endpointTypeRef
        return x
      })
    )
    .then((endpoints) => templateUtil.collectBlocks(endpoints, options, this))
  return templateUtil.templatePromise(this.global, promise)
}

/**
 * Creates block iterator helper over the endpoint types.
 *
 * @tutorial template-tutorial
 * @param {*} options
 */
function user_endpoint_types(options) {
  let promise = queryImpexp
    .exportEndpointTypes(this.global.db, this.global.sessionId)
    .then((endpointTypes) =>
      templateUtil.collectBlocks(endpointTypes, options, this)
    )
  return templateUtil.templatePromise(this.global, promise)
}

/**
 * Creates cluster iterator over the endpoint types.
 * This works ony inside user_endpoint_types.
 *
 * @param {*} options
 */
function user_clusters(options) {
  let promise = queryImpexp
    .exportClustersFromEndpointType(this.global.db, this.endpointTypeId)
    .then((endpointClusters) =>
      templateUtil.collectBlocks(endpointClusters, options, this)
    )
  return templateUtil.templatePromise(this.global, promise)
}

/**
 * Creates endpoint type cluster attribute iterator. This works only
 * inside user_clusters.
 *
 * @param {*} options
 * @returns Promise of the resolved blocks iterating over cluster attributes.
 */
function user_cluster_attributes(options) {
  let promise = queryImpexp
    .exportAttributesFromEndpointTypeCluster(
      this.global.db,
      this.parent.endpointTypeId,
      this.endpointClusterId
    )
    .then((endpointAttributes) =>
      templateUtil.collectBlocks(endpointAttributes, options, this)
    )
  return templateUtil.templatePromise(this.global, promise)
}

/**
 * Creates endpoint type cluster command iterator. This works only inside
 * user_clusters.
 *
 * @param {*} options
 * @returns Promise of the resolved blocks iterating over cluster commands.
 */
function user_cluster_commands(options) {
  let promise = queryImpexp
    .exportCommandsFromEndpointTypeCluster(
      this.global.db,
      this.parent.endpointTypeId,
      this.endpointClusterId
    )
    .then((endpointAttributes) =>
      templateUtil.collectBlocks(endpointAttributes, options, this)
    )
  return templateUtil.templatePromise(this.global, promise)
}

/**
 * Creates endpoint type cluster event iterator. This works only inside
 * user_clusters.
 *
 * @param {*} options
 * @returns Promise of the resolved blocks iterating over cluster events.
 */
function user_cluster_events(options) {
  let promise = queryImpexp
    .exportEventsFromEndpointTypeCluster(
      this.global.db,
      this.parent.endpointTypeId,
      this.endpointClusterId
    )
    .then((endpointEvents) =>
      templateUtil.collectBlocks(endpointEvents, options, this)
    )
  return templateUtil.templatePromise(this.global, promise)
}

function user_endpoint_type_count() {
  let promise = queryConfig.selectEndpointTypeCount(
    this.global.db,
    this.global.sessionId
  )
  return templateUtil.templatePromise(this.global, promise)
}

/**
 * Retrieve the number of endpoints which possess the specified
 * cluster type
 *
 * @param {*} clusterTypeId
 * @return Promise of the number of endpoint
 */
function user_endpoint_count_by_cluster(clusterTypeId, side) {
  let promise = queryConfig.selectEndpointTypeCountByCluster(
    this.global.db,
    this.global.sessionId,
    clusterTypeId,
    side
  )
  return templateUtil.templatePromise(this.global, promise)
}

/**
 * Iterates over all attributes required by the user configuration.
 *
 * @param {*} options
 * @return Promise of the resolved blocks iterating over cluster commands.
 */
function user_all_attributes(options) {
  let promise = queryConfig
    .selectAllSessionAttributes(this.global.db, this.global.sessionId)
    .then((atts) => templateUtil.collectBlocks(atts, options, this))
  return templateUtil.templatePromise(this.global, promise)
}

/**
 * Creates endpoint type cluster command iterator. This fetches all
 * commands which have been enabled on added endpoints
 *
 * @param {*} options
 * @returns Promise of the resolved blocks iterating over cluster commands.
 */
function all_user_cluster_commands(options) {
  let promise = iteratorUtil.all_user_cluster_commands_helper
    .call(this, options)
    .then((endpointCommands) =>
      templateUtil.collectBlocks(endpointCommands, options, this)
    )
  return promise
}

/**
 *
 * @param name
 * @param side
 * @param options
 * @param currentContext
 * @param isManufacturingSpecific
 * @param isIrrespectiveOfManufacturingSpecification
 * Returns: Promise of the resolved blocks iterating over manufacturing specific,
 * non-manufacturing specific or both of the cluster commands.
 */
async function all_user_cluster_command_util(
  name,
  side,
  options,
  currentContext,
  isManufacturingSpecific,
  isIrrespectiveOfManufacturingSpecification = false
) {
  let endpointTypes = await templateUtil.ensureEndpointTypeIds(currentContext)
  let packageIds = await templateUtil.ensureZclPackageIds(currentContext)
  let endpointsAndClusters =
    await queryEndpointType.selectClustersAndEndpointDetailsFromEndpointTypes(
      currentContext.global.db,
      endpointTypes
    )
  let endpointCommands
  if (isIrrespectiveOfManufacturingSpecification) {
    endpointCommands =
      await queryCommand.selectCommandDetailsFromAllEndpointTypesAndClusters(
        currentContext.global.db,
        endpointsAndClusters,
        true,
        packageIds
      )
  } else if (isManufacturingSpecific) {
    endpointCommands =
      await queryCommand.selectManufacturerSpecificCommandDetailsFromAllEndpointTypesAndClusters(
        currentContext.global.db,
        endpointsAndClusters,
        packageIds
      )
  } else {
    endpointCommands =
      await queryCommand.selectNonManufacturerSpecificCommandDetailsFromAllEndpointTypesAndClusters(
        currentContext.global.db,
        endpointsAndClusters,
        packageIds
      )
  }

  let availableCommands = []
  for (let i = 0; i < endpointCommands.length; i++) {
    if (helperZcl.isStrEqual(name, endpointCommands[i].clusterName)) {
      if (
        helperZcl.isCommandAvailable(
          side,
          endpointCommands[i].incoming,
          endpointCommands[i].outgoing,
          endpointCommands[i].commandSource,
          endpointCommands[i].name
        )
      ) {
        availableCommands.push(endpointCommands[i])
      }
    }
  }
  return templateUtil.collectBlocks(availableCommands, options, currentContext)
}

async function all_user_cluster_attribute_util(
  name,
  side,
  options,
  currentContext,
  isManufacturingSpecific,
  isIrrespectiveOfManufacturingSpecification = false
) {
  let packageIds = await templateUtil.ensureZclPackageIds(currentContext)
  let endpointTypes = await templateUtil.ensureEndpointTypeIds(currentContext)
  let endpointsAndClusters =
    await queryEndpointType.selectClustersAndEndpointDetailsFromEndpointTypes(
      currentContext.global.db,
      endpointTypes
    )

  let endpointAttributes

  if (isIrrespectiveOfManufacturingSpecification) {
    endpointAttributes =
      await queryAttribute.selectAllAttributeDetailsFromEnabledClusters(
        currentContext.global.db,
        endpointsAndClusters,
        packageIds
      )
  } else if (isManufacturingSpecific) {
    endpointAttributes =
      await queryAttribute.selectManufacturerSpecificAttributeDetailsFromAllEndpointTypesAndClusters(
        currentContext.global.db,
        endpointsAndClusters,
        packageIds
      )
  } else {
    endpointAttributes =
      await queryAttribute.selectNonManufacturerSpecificAttributeDetailsFromAllEndpointTypesAndClusters(
        currentContext.global.db,
        endpointsAndClusters,
        packageIds
      )
  }

  let availableAttributes = []
  for (let i = 0; i < endpointAttributes.length; i++) {
    if (helperZcl.isStrEqual(name, endpointAttributes[i].clusterName)) {
      availableAttributes.push(endpointAttributes[i])
    }
  }
  return templateUtil.collectBlocks(
    availableAttributes,
    options,
    currentContext
  )
}

/**
 * Creates endpoint type cluster command iterator. This fetches all
 * manufacturing specific commands which have been enabled on added endpoints
 *
 * @param options
 * @returns Promise of the resolved blocks iterating over manufacturing specific
 * cluster commands.
 */
function all_user_cluster_manufacturer_specific_commands(name, side, options) {
  return all_user_cluster_command_util(name, side, options, this, true)
}

/**
 * Creates endpoint type cluster command iterator. This fetches all
 * non-manufacturing specific commands which have been enabled on added endpoints
 *
 * @param options
 * @returns Promise of the resolved blocks iterating over non-manufacturing specific
 * cluster commands.
 */
function all_user_cluster_non_manufacturer_specific_commands(
  name,
  side,
  options
) {
  return all_user_cluster_command_util(name, side, options, this, false)
}

/**
 * Creates endpoint type cluster command iterator. This fetches all
 * manufacturing specific commands which have been enabled on added endpoints
 *
 * @param options
 * @returns Promise of the resolved blocks iterating over manufacturing specific
 * cluster commands.
 */
function all_user_cluster_manufacturer_specific_attributes(
  name,
  side,
  options
) {
  return all_user_cluster_attribute_util(name, side, options, this, true)
}

/**
 * Creates endpoint type cluster command iterator. This fetches all
 * non-manufacturing specific commands which have been enabled on added endpoints
 *
 * @param options
 * @returns Promise of the resolved blocks iterating over non-manufacturing specific
 * cluster commands.
 */
function all_user_cluster_non_manufacturer_specific_attributes(
  name,
  side,
  options
) {
  return all_user_cluster_attribute_util(name, side, options, this, false)
}

/**
 * Creates endpoint type cluster command iterator. This fetches all
 * commands which have been enabled on added endpoints
 *
 * @param {*} options
 * @returns Promise of the resolved blocks iterating over cluster commands.
 */
async function all_commands_for_user_enabled_clusters(options) {
  let packageIds = await templateUtil.ensureZclPackageIds(this)
  let endpointTypes = await templateUtil.ensureEndpointTypeIds(this)
  let endpointsAndClusters =
    await queryEndpointType.selectClustersAndEndpointDetailsFromEndpointTypes(
      this.global.db,
      endpointTypes
    )
  let endpointCommands =
    await queryCommand.selectAllCommandDetailsFromEnabledClusters(
      this.global.db,
      endpointsAndClusters,
      packageIds
    )
  return templateUtil.collectBlocks(endpointCommands, options, this)
}
/**
 * This helper returns all commands which have cli within the list of enabled
 * clusters.
 *
 * @param options
 * @returns all commands with cli from the list of enabled clusters
 *
 */
async function all_cli_commands_for_user_enabled_clusters(options) {
  let endpointTypes = await templateUtil.ensureEndpointTypeIds(this)
  let packageIds = await templateUtil.ensureZclPackageIds(this)
  let endpointsAndClusters =
    await queryEndpointType.selectClustersAndEndpointDetailsFromEndpointTypes(
      this.global.db,
      endpointTypes
    )

  let endpointCommands =
    await queryCommand.selectAllCliCommandDetailsFromEnabledClusters(
      this.global.db,
      endpointsAndClusters,
      packageIds
    )
  return templateUtil.collectBlocks(endpointCommands, options, this)
}

/**
 * Creates cluster iterator for all endpoints.
 *
 * @param {*} options
 * @returns Promise of the resolved blocks iterating over cluster commands.
 */
async function all_user_clusters(options) {
  let endpointTypes = await templateUtil.ensureEndpointTypeIds(this)

  let clusters =
    await queryEndpointType.selectAllClustersDetailsFromEndpointTypes(
      this.global.db,
      endpointTypes
    )

  return templateUtil.collectBlocks(clusters, options, this)
}

/**
 * Creates cluster command iterator for all endpoints.
 *
 * @param {*} options
 * @returns Promise of the resolved blocks iterating over cluster commands.
 */
async function all_user_clusters_irrespective_of_side(options) {
  let endpointTypes = await templateUtil.ensureEndpointTypeIds(this)

  let clusters =
    await queryEndpointType.selectAllClustersDetailsIrrespectiveOfSideFromEndpointTypes(
      this.global.db,
      endpointTypes
    )

  return templateUtil.collectBlocks(clusters, options, this)
}

/**
 * Creates cluster command iterator for all endpoints whitout any duplicates
 * cause by cluster side
 *
 * @param {*} options
 * @returns Promise of the resolved blocks iterating over cluster commands.
 */
async function all_user_clusters_names(options) {
  let endpointTypes = await templateUtil.ensureEndpointTypeIds(this)
  let clusters =
    await queryEndpointType.selectAllClustersNamesFromEndpointTypes(
      this.global.db,
      endpointTypes
    )

  return templateUtil.collectBlocks(clusters, options, this)
}

/**
 * Get the count of the number of clusters commands with cli for a cluster.
 * This is used under a cluster block helper
 */
async function user_cluster_command_count_with_cli() {
  let endpointTypes = await templateUtil.ensureEndpointTypeIds(this)
  let packageIds = await templateUtil.ensureZclPackageIds(this)
  return queryCommand.selectCliCommandCountFromEndpointTypeCluster(
    this.global.db,
    endpointTypes,
    this.endpointClusterId,
    packageIds
  )
}

/**
 * This helper works within the the cluster block helpers. It is used to get
 * all commands of the cluster which have cli associated with them.
 *
 * param options
 * Returns: all commands with cli for a cluster
 *
 * Example:
 * {{#all_user_clusters_irrespective_of_side}}
 *  {{#user_cluster_commands_with_cli}}
 *  {{/user_cluster_commands_with_cli}}
 * {{/all_user_clusters_irrespective_of_side}}
 */
async function user_cluster_commands_with_cli(options) {
  let packageIds = await templateUtil.ensureZclPackageIds(this)
  let cliCommands = await queryCommand.selectCliCommandsFromCluster(
    this.global.db,
    this.id,
    packageIds
  )
  return templateUtil.collectBlocks(cliCommands, options, this)
}

/**
 * Creates endpoint type cluster command iterator. This works only inside
 * cluster block helpers.
 *
 * @param options
 * Returns: Promise of the resolved blocks iterating over cluster commands.
 */
async function user_cluster_commands_all_endpoints(options) {
  let endpointTypes = await templateUtil.ensureEndpointTypeIds(this)
  let packageIds = await templateUtil.ensureZclPackageIds(this)
  let endpointCommands =
    await queryEndpointType.selectCommandDetailsFromAllEndpointTypeCluster(
      this.global.db,
      endpointTypes,
      this.endpointClusterId,
      packageIds
    )
  return endpointsAndClusterstemplateUtil.collectBlocks(
    endpointCommands,
    options,
    this
  )
}

/**
 * Check if the cluster (name) has any enabled commands. This works only inside
 * cluster block helpers.
 *
 * @param {*} name : Cluster name
 * @param {*} side : Cluster side
 * @returns True if cluster has enabled commands otherwise false
 */
async function user_cluster_has_enabled_command(name, side) {
  let endpointTypes = await templateUtil.ensureEndpointTypeIds(this)
  let packageIds = await templateUtil.ensureZclPackageIds(this)
  let endpointsAndClusters =
    await queryEndpointType.selectClustersAndEndpointDetailsFromEndpointTypes(
      this.global.db,
      endpointTypes
    )

  let endpointCommands =
    await queryCommand.selectCommandDetailsFromAllEndpointTypesAndClusters(
      this.global.db,
      endpointsAndClusters,
      false,
      packageIds
    )
  let cmdCount = 0
  endpointCommands.forEach((command) => {
    if (helperZcl.isStrEqual(name, command.clusterName)) {
      if (
        helperZcl.isCommandAvailable(
          side,
          command.incoming,
          command.outgoing,
          command.commandSource,
          command.name
        )
      ) {
        cmdCount++
      }
    }
  })
  return cmdCount != 0
}

/**
 * Creates endpoint type cluster command iterator. This fetches all
 * manufacturing and non-manufaturing specific commands which have been enabled
 * on added endpoints
 *
 * @param options
 * @returns Promise of the resolved blocks iterating over manufacturing specific
 * and non-manufacturing specific cluster commands.
 */
function all_user_cluster_commands_irrespective_of_manufaturing_specification(
  name,
  side,
  options
) {
  return all_user_cluster_command_util(name, side, options, this, false, true)
}

/**
 * Creates endpoint type cluster attribute iterator. This fetches all
 * manufacturing and non-manufaturing specific attributes which have been enabled
 * on added endpoints
 *
 * @param options
 * @returns Promise of the resolved blocks iterating over manufacturing specific
 * and non-manufacturing specific cluster attributes.
 */
function all_user_cluster_attributes_irrespective_of_manufatucuring_specification(
  name,
  side,
  options
) {
  return all_user_cluster_attribute_util(name, side, options, this, false, true)
}

/**
 * Helper that resolves into a user session key value.
 *
 * @param {*} options
 * @returns Promise of value of the session key or undefined.
 */
async function user_session_key(options) {
  let key = options.hash.key
  let value = await querySession.getSessionKeyValue(
    this.global.db,
    this.global.sessionId,
    key
  )
  if (options.hash.toupper == 'true' && value != null)
    return value.toUpperCase()
  else return value
}

/**
 * If helper that checks if command discovery is enabled
 *
 * example:
 * {{#if_command_discovery_enabled}}
 * command discovery is enabled
 * {{else}}
 * command discovery is not enabled
 * {{/if_command_discovery_enabled}}
 */
async function if_command_discovery_enabled(options) {
  const key = 'commandDiscovery'
  let value = await querySession.getSessionKeyValue(
    this.global.db,
    this.global.sessionId,
    key
  )
  if (value == 1) {
    return options.fn(this)
  } else {
    return options.inverse(this)
  }
}

async function user_manufacturer_code(options) {
  let value = await querySession.getSessionKeyValue(
    this.global.db,
    this.global.sessionId,
    dbEnum.sessionOption.manufacturerCodes
  )
  if (options.hash.toupper == 'true' && value != null)
    return value.toUpperCase()
  else return value
}

async function user_default_response_policy(options) {
  let value = await querySession.getSessionKeyValue(
    this.global.db,
    this.global.sessionId,
    dbEnum.sessionOption.defaultResponsePolicy
  )
  if (options.hash.toupper == 'true' && value != null)
    return value.toUpperCase()
  else return value
}

/**
 * An if helper to check if default response for a command is enabled or not.
 * @param {*} command
 * @param {*} options
 * @returns true if the the default response policy is either always or
 * when the policy is not never and the command has the disable default
 * response policy set to false(not true)
 */
async function is_command_default_response_enabled(command, options) {
  let defaultRespPolicy = await querySession.getSessionKeyValue(
    this.global.db,
    this.global.sessionId,
    dbEnum.sessionOption.defaultResponsePolicy
  )
  if (
    defaultRespPolicy.toUpperCase() == 'ALWAYS' ||
    (defaultRespPolicy.toUpperCase() != 'NEVER' &&
      command.isDefaultResponseEnabled)
  ) {
    return options.fn(this)
  } else {
    return options.inverse(this)
  }
}

/**
 * An if helper to check if default response for a command is disabled or not.
 * @param {*} command
 * @param {*} options
 * @returns true if the the default response policy is either never or
 * when the policy is not always and the command has the disable default
 * response policy set to true(for eg disableDefaultResponse="true" in xml).
 */
async function is_command_default_response_disabled(command, options) {
  let defaultRespPolicy = await querySession.getSessionKeyValue(
    this.global.db,
    this.global.sessionId,
    dbEnum.sessionOption.defaultResponsePolicy
  )
  if (
    defaultRespPolicy.toUpperCase() == 'NEVER' ||
    (defaultRespPolicy.toUpperCase() != 'ALWAYS' &&
      !command.isDefaultResponseEnabled)
  ) {
    return options.fn(this)
  } else {
    return options.inverse(this)
  }
}

/*
 * @param {*} endpointTypeId
 * Returns the endpoint type identifier for an endpoint type
 */
async function endpoint_type_identifier(endpointTypeId) {
  let endpointTypes = await templateUtil.ensureEndpointTypeIds(this)
  let endpoints = await queryImpexp.exportEndpoints(
    this.global.db,
    this.global.sessionId,
    endpointTypes
  )
  for (let i = 0; i < endpoints.length; i++) {
    if (endpointTypeId == endpoints[i].endpointTypeRef) {
      if (endpoints[i].endpointId == null) {
        return '0'
      } else {
        return `${endpoints[i].endpointId}`
      }
    }
  }
  return '0'
}

/*
 * @param {*} endpointTypeId
 * Returns the index of the endpoint whose endpointTypeId is endpointTypeId
 * Will return -1 if the given endpoint type is not present.
 */
async function endpoint_type_index(endpointTypeId) {
  let endpointTypes = await templateUtil.ensureEndpointTypeIds(this)
  let endpoints = await queryImpexp.exportEndpoints(
    this.global.db,
    this.global.sessionId,
    endpointTypes
  )
  for (let i = 0; i < endpoints.length; i++) {
    if (endpointTypeId == endpoints[i].endpointTypeRef) {
      return i
    }
  }
  return -1
}

/**
 * Default values for the attributes longer than a pointer.
 * All attribute values with size greater than 2 bytes.
 * Excluding 0 values and externally saved values
 *
 * @param name
 * @param side
 * @param options
 * @returns Attribute values greater than 2 bytes and not 0 nor externally saved.
 */
async function all_user_cluster_attributes_for_generated_defaults(options) {
  let packageIds = await templateUtil.ensureZclPackageIds(this)
  let endpointTypes = await templateUtil.ensureEndpointTypeIds(this)
  let endpointsAndClusters =
    await queryEndpointType.selectClustersAndEndpointDetailsFromEndpointTypes(
      this.global.db,
      endpointTypes
    )
  let endpointAttributes = await queryAttribute.selectAttributeBoundDetails(
    this.global.db,
    endpointsAndClusters,
    packageIds
  )
  return templateUtil.collectBlocks(endpointAttributes, options, this)
}

/**
 * Entails the list of all attributes which have been enabled. Given the
 * cluster is enabled as well. The helper retrieves the attributes across
 * all endpoints.
 * @param options
 * @returns enabled attributes
 */
async function all_user_cluster_generated_attributes(options) {
  let packageIds = await templateUtil.ensureZclPackageIds(this)
  let endpointTypes = await templateUtil.ensureEndpointTypeIds(this)
  let endpointsAndClusters =
    await queryEndpointType.selectClustersAndEndpointDetailsFromEndpointTypes(
      this.global.db,
      endpointTypes
    )
  let endpointAttributes =
    await queryAttribute.selectAttributeDetailsFromEnabledClusters(
      this.global.db,
      endpointsAndClusters,
      packageIds
    )
  return templateUtil.collectBlocks(endpointAttributes, options, this)
}

/**
 * Entails the list of reportable attributes which have been enabled. Given the
 * cluster is enabled as well. The helper retrieves the reportable attributes
 * per endpoint per cluster.
 * @param options
 * @returns Reportable attributes
 */
async function all_user_reportable_attributes(options) {
  let packageIds = await templateUtil.ensureZclPackageIds(this)
  let endpointTypes = await templateUtil.ensureEndpointTypeIds(this)
  let endpointsAndClusters =
    await queryEndpointType.selectClustersAndEndpointDetailsFromEndpointTypes(
      this.global.db,
      endpointTypes
    )
  let endpointAttributes =
    await queryAttribute.selectReportableAttributeDetailsFromEnabledClustersAndEndpoints(
      this.global.db,
      endpointsAndClusters,
      packageIds
    )
  return templateUtil.collectBlocks(endpointAttributes, options, this)
}

/**
 * All available cluster commands across all endpoints and clusters.
 * @param options
 * @returns All available cluster commands across all endpoints and clusters
 */
async function all_user_cluster_generated_commands(options) {
  let endpointTypes = await queryEndpointType.selectUsedEndpointTypeIds(
    this.global.db,
    this.global.sessionId
  )
  let packageIds = await templateUtil.ensureZclPackageIds(this)
  let endpointCommands =
    await queryCommand.selectAllAvailableClusterCommandDetailsFromEndpointTypes(
      this.global.db,
      endpointTypes,
      packageIds
    )
  return templateUtil.collectBlocks(endpointCommands, options, this)
}

/**
 * Util function for all clusters with side that have available incoming or
 * outgiong commands across all endpoints.
 * @param options
 * @param is_incoming boolean to check if commands are incoming or outgoing
 * @returns All clusters with side that have available incoming or outgiong
 * commands across all endpoints.
 */
async function all_user_clusters_with_incoming_or_outgoing_commands(
  options,
  currentContext,
  isIncoming
) {
  let packageIds = await templateUtil.ensureZclPackageIds(currentContext)
  let endpointTypes = await queryEndpointType.selectUsedEndpointTypeIds(
    currentContext.global.db,
    currentContext.global.sessionId
  )
  if (isIncoming) {
    if (
      'uniqueClusterCodes' in options.hash &&
      options.hash.uniqueClusterCodes == 'true'
    ) {
      let clustersWithIncomingCommands =
        await queryCommand.selectAllClustersWithIncomingCommands(
          currentContext.global.db,
          endpointTypes,
          true,
          packageIds
        )
      return templateUtil.collectBlocks(
        clustersWithIncomingCommands,
        options,
        currentContext
      )
    } else {
      let clustersWithIncomingCommands =
        await queryCommand.selectAllClustersWithIncomingCommands(
          currentContext.global.db,
          endpointTypes,
          false,
          packageIds
        )
      return templateUtil.collectBlocks(
        clustersWithIncomingCommands,
        options,
        currentContext
      )
    }
  } else {
    if (
      'uniqueClusterCodes' in options.hash &&
      options.hash.uniqueClusterCodes == 'true'
    ) {
      let clustersWithOutgoingCommands =
        await queryCommand.selectAllClustersWithOutgoingCommands(
          currentContext.global.db,
          endpointTypes,
          true,
          packageIds
        )
      return templateUtil.collectBlocks(
        clustersWithOutgoingCommands,
        options,
        currentContext
      )
    } else {
      let clustersWithOutgoingCommands =
        await queryCommand.selectAllClustersWithOutgoingCommands(
          currentContext.global.db,
          endpointTypes,
          false,
          packageIds
        )
      return templateUtil.collectBlocks(
        clustersWithOutgoingCommands,
        options,
        currentContext
      )
    }
  }
}

/**
 * All clusters with side that have available incoming commands
 * @param options
 * @returns All clusters with side that have available incoming commands across
 * all endpoints.
 */
function all_user_clusters_with_incoming_commands(options) {
  return all_user_clusters_with_incoming_or_outgoing_commands(
    options,
    this,
    true
  )
}

/**
 * All clusters with side that have available outgoing commands
 * @param options
 * @returns All clusters with side that have available outgoing commands across
 * all endpoints.
 */
async function all_user_clusters_with_outgoing_commands(options) {
  return all_user_clusters_with_incoming_or_outgoing_commands(
    options,
    this,
    false
  )
}

/**
 * Provide all manufacturing specific clusters that have incoming commands with
 * the given cluster code.
 * @param clusterCode
 * @param options
 * @returns Details of manufacturing specific clusters that have incoming
 * commands with the given cluster code
 */
async function manufacturing_clusters_with_incoming_commands(
  clusterCode,
  options
) {
  let packageIds = await templateUtil.ensureZclPackageIds(this)
  return queryEndpointType
    .selectUsedEndpointTypeIds(this.global.db, this.global.sessionId)
    .then((endpointTypes) =>
      queryCommand.selectMfgClustersWithIncomingCommandsForClusterCode(
        this.global.db,
        endpointTypes,
        clusterCode,
        packageIds
      )
    )
    .then((clustersWithIncomingCommands) =>
      templateUtil.collectBlocks(clustersWithIncomingCommands, options, this)
    )
}

/**
 * All clusters that have available incoming commands.
 * If there is a client and server enabled on the endpoint, this combines them
 * into a single entry.
 * @param options
 * @returns All clusters that have available incoming commands across
 * all endpoints.
 */
async function all_user_clusters_with_incoming_commands_combined(options) {
  let packageIds = await templateUtil.ensureZclPackageIds(this)
  return queryEndpointType
    .selectUsedEndpointTypeIds(this.global.db, this.global.sessionId)
    .then((endpointTypes) =>
      queryCommand.selectAllClustersWithIncomingCommandsCombined(
        this.global.db,
        endpointTypes,
        packageIds
      )
    )
    .then((clustersWithIncomingCommands) =>
      templateUtil.collectBlocks(clustersWithIncomingCommands, options, this)
    )
}

/**
 * All commands that need to be parsed for a given cluster. This takes in booleans
 * for if the client and or server are included.
 * @param clusterName
 * @param clientSide
 * @param serverSide
 * @param options
 * @returns all commands that need to be parsed for a given cluster
 */
async function all_incoming_commands_for_cluster_combined(
  clusterName,
  clientSide,
  serverSide,
  options
) {
  let packageIds = await templateUtil.ensureZclPackageIds(this)
  let isMfgSpec =
    'isMfgSpecific' in options.hash
      ? options.hash.isMfgSpecific.toLowerCase() === 'true'
      : undefined
  let endpointTypes = await queryEndpointType.selectUsedEndpointTypeIds(
    this.global.db,
    this.global.sessionId
  )

  let clustersWithIncomingCommands =
    await queryCommand.selectAllIncomingCommandsForClusterCombined(
      this.global.db,
      endpointTypes,
      clusterName,
      clientSide,
      serverSide,
      isMfgSpec,
      packageIds
    )

  return templateUtil.collectBlocks(clustersWithIncomingCommands, options, this)
}

async function all_user_incoming_commands_for_all_clusters(options) {
  let packageIds = await templateUtil.ensureZclPackageIds(this)
  let isMfgSpec =
    'isMfgSpecific' in options.hash
      ? options.hash.isMfgSpecific.toLowerCase() === 'true'
      : undefined

  let endpointTypes = await queryEndpointType.selectUsedEndpointTypeIds(
    this.global.db,
    this.global.sessionId
  )

  let clustersWithIncomingCommands =
    await queryCommand.selectAllIncomingCommands(
      this.global.db,
      endpointTypes,
      isMfgSpec,
      packageIds
    )

  return templateUtil.collectBlocks(clustersWithIncomingCommands, options, this)
}

/**
  * A util function for all incoming or outgoing commands that need to be parsed
  * for a given cluster
  * @param clusterName
  * @param clusterSide
  * @param isIncoming
  * @param options
  * @returns All incoming or outgoing commands that need to be parsed for a given
 cluster
  */
async function all_incoming_or_outgoing_commands_for_cluster(
  clusterName,
  clusterSide,
  isIncoming,
  options,
  currentContext
) {
  let packageIds = await templateUtil.ensureZclPackageIds(currentContext)
  let isMfgSpec =
    'isMfgSpecific' in options.hash
      ? options.hash.isMfgSpecific.toLowerCase() === 'true'
      : undefined

  let endpointTypes = await queryEndpointType.selectUsedEndpointTypeIds(
    currentContext.global.db,
    currentContext.global.sessionId
  )

  let clustersWithIncomingOrOutgoingCommands = isIncoming
    ? await queryCommand.selectAllIncomingCommandsForCluster(
        currentContext.global.db,
        endpointTypes,
        clusterName,
        clusterSide,
        isMfgSpec,
        packageIds
      )
    : await queryCommand.selectAllOutgoingCommandsForCluster(
        currentContext.global.db,
        endpointTypes,
        clusterName,
        clusterSide,
        isMfgSpec,
        packageIds
      )

  return templateUtil.collectBlocks(
    clustersWithIncomingOrOutgoingCommands,
    options,
    currentContext
  )
}

/**
 * All incoming commands that need to be parsed for a given cluster
 * @param clusterName
 * @param options
 * @returns all incoming commands that need to be parsed for a given cluster
 */
async function all_incoming_commands_for_cluster(
  clusterName,
  clusterSide,
  options
) {
  return all_incoming_or_outgoing_commands_for_cluster(
    clusterName,
    clusterSide,
    true,
    options,
    this
  )
}

/**
 * All outgoing commands that need to be parsed for a given cluster
 * @param clusterName
 * @param options
 * @returns all outgoing commands that need to be parsed for a given cluster
 */
async function all_outgoing_commands_for_cluster(
  clusterName,
  clusterSide,
  options
) {
  return all_incoming_or_outgoing_commands_for_cluster(
    clusterName,
    clusterSide,
    false,
    options,
    this
  )
}

/**
 * Entails the Cluster details per endpoint
 * @param {*} options
 * @returns Cluster Details per endpoint with attribute summaries within the clusters
 */
async function generated_clustes_details(options) {
  let endpointTypes = await templateUtil.ensureEndpointTypeIds(this)
  let packageIds = await templateUtil.ensureZclPackageIds(this)
  let endpointsAndClusters =
    await queryEndpointType.selectClustersAndEndpointDetailsFromEndpointTypes(
      this.global.db,
      endpointTypes
    )
  let clusterDetails =
    await queryCluster.selectClusterDetailsFromEnabledClusters(
      this.global.db,
      endpointsAndClusters,
      packageIds
    )
  return templateUtil.collectBlocks(clusterDetails, options, this)
}

/**
 * Entails Endpoint type details along with their cluster summaries
 * @param options
 * @returns Endpoint type details along with their cluster summaries
 */
async function generated_endpoint_type_details(options) {
  let endpointTypes = await templateUtil.ensureEndpointTypeIds(this)
  let endpointsAndClusters =
    await queryEndpointType.selectClustersAndEndpointDetailsFromEndpointTypes(
      this.global.db,
      endpointTypes
    )
  let endpointDetails =
    await queryEndpointType.selectEndpointDetailsFromAddedEndpoints(
      this.global.db,
      endpointsAndClusters
    )
  return templateUtil.collectBlocks(endpointDetails, options, this)
}

/**
 * Returns attributes inside an endpoint type that either have a default or a
 * bounded attribute.
 *
 * @param name
 * @param side
 * @param options
 * @returns endpoints with bounds or defaults
 */
async function all_user_cluster_attributes_min_max_defaults(options) {
  let packageIds = await templateUtil.ensureZclPackageIds(this)
  let endpointTypes = await templateUtil.ensureEndpointTypeIds(this)
  let endpointsAndClusters =
    await queryEndpointType.selectClustersAndEndpointDetailsFromEndpointTypes(
      this.global.db,
      endpointTypes
    )
  let endpointAttributes =
    await queryAttribute.selectAttributeDetailsWithABoundFromEnabledClusters(
      this.global.db,
      endpointsAndClusters,
      packageIds
    )
  return templateUtil.collectBlocks(endpointAttributes, options, this)
}

/**
 *
 * @param clusterName
 * @param attributeName
 * @param attributeSide
 * @param attributeValue
 * @param attributeValueType
 * @param endpointAttributes
 * @returns arrayIndex
 */
async function checkAttributeMatch(
  clusterName,
  attributeName,
  attributeSide,
  attributeValue,
  attributeValueType,
  endpointAttributes
) {
  let dataPtr
  for (const ea of endpointAttributes) {
    if (
      ea.clusterName === clusterName &&
      ea.name === attributeName &&
      ea.side === attributeSide &&
      ea.attributeValueType === attributeValueType
    ) {
      dataPtr = ea.arrayIndex ? ea.arrayIndex : 0
      return dataPtr
    }
  }
  return attributeValue
}

/**
 * Extracts the index of generated defaults array which come from
 * all_user_cluster_attributes_for_generated_defaults
 * @param clusterName
 * @param attributeName
 * @param attributeValueType
 * @param attributeValue
 * @param prefixReturn
 * @param postFixReturn
 * @returns index of the generated default array
 */
async function generated_defaults_index(
  clusterName,
  attributeName,
  attributeValueType,
  attributeValue,
  prefixReturn,
  postFixReturn
) {
  let packageIds = await templateUtil.ensureZclPackageIds(this)
  let endpointTypes = await templateUtil.ensureEndpointTypeIds(this)

  let endpointsAndClusters =
    await queryEndpointType.selectClustersAndEndpointDetailsFromEndpointTypes(
      this.global.db,
      endpointTypes
    )

  let endpointAttributes = await queryAttribute.selectAttributeBoundDetails(
    this.global.db,
    endpointsAndClusters,
    packageIds
  )

  let dataPtr = attributeValue
  for (const ea of endpointAttributes) {
    if (
      ea.clusterName === clusterName &&
      ea.name === attributeName &&
      ea.attributeValueType === attributeValueType
    ) {
      dataPtr = ea.arrayIndex ? ea.arrayIndex : 0
    }
  }
  if (dataPtr === attributeValue) {
    dataPtr = dataPtr ? '(uint8_t*)' + dataPtr : 'NULL'
  } else {
    dataPtr = prefixReturn + dataPtr + postFixReturn
  }
  return dataPtr
}

/**
 * Extracts the index of generated defaults array which come from
 * all_user_cluster_attributes_for_generated_defaults
 * @param clusterName
 * @param attributeName
 * @param attributeSide
 * @param attributeValueType
 * @param attributeValue
 * @param prefixReturn
 * @param postFixReturn
 * @returns deafult value's index in the generated default array
 */
async function generated_default_index(
  clusterName,
  attributeName,
  attributeSide,
  attributeValueType,
  attributeValue,
  prefixReturn,
  postFixReturn
) {
  let packageIds = await templateUtil.ensureZclPackageIds(this)
  let endpointTypes = await templateUtil.ensureEndpointTypeIds(this)

  let endpointsAndClusters =
    await queryEndpointType.selectClustersAndEndpointDetailsFromEndpointTypes(
      this.global.db,
      endpointTypes
    )

  let endpointAttributes = await queryAttribute.selectAttributeBoundDetails(
    this.global.db,
    endpointsAndClusters,
    packageIds
  )

  let dataPtr = await checkAttributeMatch(
    clusterName,
    attributeName,
    attributeSide,
    attributeValue,
    attributeValueType,
    endpointAttributes
  )
  if (dataPtr === attributeValue) {
    dataPtr = dataPtr ? '(uint8_t*)' + dataPtr : 'NULL'
  } else {
    dataPtr = prefixReturn + dataPtr + postFixReturn
  }
  return dataPtr
}

/**
 *
 * Extracts the index of generated min max defaults array which come from
 * all_user_cluster_attributes_min_max_defaults
 * @param name
 * @param side
 * @param options
 * @returns index of the generated min max default array
 */
async function generated_attributes_min_max_index(clusterName, attributeName) {
  let packageIds = await templateUtil.ensureZclPackageIds(this)
  let endpointTypes = await templateUtil.ensureEndpointTypeIds(this)
  let endpointsAndClusters =
    await queryEndpointType.selectClustersAndEndpointDetailsFromEndpointTypes(
      this.global.db,
      endpointTypes
    )

  let endpointAttributes =
    await queryAttribute.selectAttributeDetailsWithABoundFromEnabledClusters(
      this.global.db,
      endpointsAndClusters,
      packageIds
    )
  let dataPtr = 0
  for (let i = 0; i < endpointAttributes.length; i++) {
    if (
      endpointAttributes[i].clusterName === clusterName &&
      endpointAttributes[i].name === attributeName
    ) {
      dataPtr = i
    }
  }
  return dataPtr
}

/**
 *
 * Extracts the index of generated min max defaults array which come from
 * all_user_cluster_attributes_min_max_defaults
 * @param clusterName
 * @param attributeName
 * @param attributeSide
 * @param options
 * @returns index of the generated min max default in the array
 */
async function generated_attribute_min_max_index(
  clusterName,
  attributeName,
  attributeSide
) {
  let packageIds = await templateUtil.ensureZclPackageIds(this)
  let endpointTypes = await templateUtil.ensureEndpointTypeIds(this)
  let endpointsAndClusters =
    await queryEndpointType.selectClustersAndEndpointDetailsFromEndpointTypes(
      this.global.db,
      endpointTypes
    )

  let endpointAttributes =
    await queryAttribute.selectAttributeDetailsWithABoundFromEnabledClusters(
      this.global.db,
      endpointsAndClusters,
      packageIds
    )
  let dataPtr = 0
  for (let i = 0; i < endpointAttributes.length; i++) {
    if (
      endpointAttributes[i].clusterName === clusterName &&
      endpointAttributes[i].name === attributeName &&
      endpointAttributes[i].side === attributeSide
    ) {
      dataPtr = i
    }
  }
  return dataPtr
}

const dep = templateUtil.deprecatedHelper

// WARNING! WARNING! WARNING! WARNING! WARNING! WARNING!
//
// Note: these exports are public API. Templates that might have been created in the past and are
// available in the wild might depend on these names.
// If you rename the functions, you need to still maintain old exports list.
exports.user_endpoint_types = user_endpoint_types
exports.user_endpoints = user_endpoints
exports.user_clusters = user_clusters
exports.user_cluster_attributes = user_cluster_attributes
exports.user_cluster_commands = user_cluster_commands
exports.user_cluster_events = user_cluster_events
exports.user_endpoint_type_count = user_endpoint_type_count
exports.user_endpoint_count_by_cluster = user_endpoint_count_by_cluster
exports.user_all_attributes = user_all_attributes
exports.all_user_cluster_commands = all_user_cluster_commands
exports.all_user_clusters = all_user_clusters
exports.all_user_clusters_names = all_user_clusters_names
exports.user_cluster_command_count_with_cli =
  user_cluster_command_count_with_cli
exports.user_cluster_commands_all_endpoints =
  user_cluster_commands_all_endpoints
exports.user_cluster_has_enabled_command = user_cluster_has_enabled_command
exports.user_session_key = user_session_key
exports.user_manufacturer_code = user_manufacturer_code
exports.user_default_response_policy = user_default_response_policy
exports.endpoint_type_identifier = endpoint_type_identifier
exports.endpoint_type_index = endpoint_type_index
exports.all_commands_for_user_enabled_clusters =
  all_commands_for_user_enabled_clusters
exports.all_user_clusters_irrespective_of_side =
  all_user_clusters_irrespective_of_side
exports.all_user_cluster_manufacturer_specific_commands =
  all_user_cluster_manufacturer_specific_commands
exports.all_user_cluster_non_manufacturer_specific_commands =
  all_user_cluster_non_manufacturer_specific_commands
exports.user_cluster_commands_with_cli = user_cluster_commands_with_cli
exports.all_cli_commands_for_user_enabled_clusters =
  all_cli_commands_for_user_enabled_clusters
exports.all_user_cluster_commands_irrespective_of_manufaturing_specification =
  all_user_cluster_commands_irrespective_of_manufaturing_specification
exports.all_user_cluster_manufacturer_specific_attributes =
  all_user_cluster_manufacturer_specific_attributes
exports.all_user_cluster_non_manufacturer_specific_attributes =
  all_user_cluster_non_manufacturer_specific_attributes
exports.all_user_cluster_attributes_irrespective_of_manufatucuring_specification =
  all_user_cluster_attributes_irrespective_of_manufatucuring_specification
exports.all_user_cluster_attributes_for_generated_defaults =
  all_user_cluster_attributes_for_generated_defaults
exports.all_user_cluster_generated_attributes =
  all_user_cluster_generated_attributes
exports.all_user_reportable_attributes = all_user_reportable_attributes
exports.all_user_cluster_generated_commands =
  all_user_cluster_generated_commands
exports.generated_clustes_details = generated_clustes_details
exports.generated_endpoint_type_details = generated_endpoint_type_details
exports.all_user_cluster_attributes_min_max_defaults =
  all_user_cluster_attributes_min_max_defaults
exports.generated_defaults_index = dep(generated_defaults_index, {
  to: 'generated_default_index',
})
exports.generated_default_index = generated_default_index
exports.generated_attributes_min_max_index = dep(
  generated_attributes_min_max_index,
  { to: 'generated_attribute_min_max_index' }
)
exports.generated_attribute_min_max_index = generated_attribute_min_max_index
exports.all_user_clusters_with_incoming_commands =
  all_user_clusters_with_incoming_commands
exports.all_user_incoming_commands_for_all_clusters =
  all_user_incoming_commands_for_all_clusters
exports.all_user_clusters_with_incoming_commands_combined =
  all_user_clusters_with_incoming_commands_combined
exports.all_user_clusters_with_incoming_commands_combined = dep(
  all_user_clusters_with_incoming_commands_combined,
  { to: 'all_user_incoming_commands_for_all_clusters' }
)
exports.all_incoming_commands_for_cluster = all_incoming_commands_for_cluster
exports.all_incoming_commands_for_cluster_combined =
  all_incoming_commands_for_cluster_combined
exports.all_incoming_commands_for_cluster_combined = dep(
  all_incoming_commands_for_cluster_combined,
  { to: 'all_user_incoming_commands_for_all_clusters' }
)
exports.if_command_discovery_enabled = if_command_discovery_enabled
exports.manufacturing_clusters_with_incoming_commands =
  manufacturing_clusters_with_incoming_commands
exports.all_user_clusters_with_outgoing_commands =
  all_user_clusters_with_outgoing_commands
exports.all_outgoing_commands_for_cluster = all_outgoing_commands_for_cluster
exports.is_command_default_response_enabled =
  is_command_default_response_enabled
exports.is_command_default_response_disabled =
  is_command_default_response_disabled
