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
 * This module provides queries for user configuration.
 *
 * @module DB API: user configuration queries against the database.
 */
const dbApi = require('./db-api.js')
const dbMapping = require('./db-mapping.js')
const queryPackage = require('./query-package.js')
const dbEnum = require('../../src-shared/db-enum.js')
const queryZcl = require('./query-zcl.js')
const queryDeviceType = require('./query-device-type')
const queryCommand = require('./query-command.js')
const restApi = require('../../src-shared/rest-api.js')
const _ = require('lodash')

/**
 * Promises to update the cluster include/exclude state.
 * If the entry [as defined uniquely by endpointTypeId, clusterId, side] is not there, then insert
 * Else update the entry in place.
 *
 * @export
 * @param {*} db
 * @param {*} endpointTypeId
 * @param {*} clusterRef
 * @param {*} side
 * @param {*} enabled
 * @returns Promise to update the cluster exclude/include state.
 */
async function insertOrReplaceClusterState(
  db,
  endpointTypeId,
  clusterRef,
  side,
  enabled
) {
  return dbApi.dbInsert(
    db,
    `
INSERT
INTO ENDPOINT_TYPE_CLUSTER
  ( ENDPOINT_TYPE_REF, CLUSTER_REF, SIDE, ENABLED )
VALUES
  ( ?, ?, ?, ?)
ON CONFLICT
  (ENDPOINT_TYPE_REF, CLUSTER_REF, SIDE)
DO UPDATE SET ENABLED = ?`,
    [endpointTypeId, clusterRef, side, enabled, enabled]
  )
}

/**
 * Promise to get a endpoint type's clusters.
 * @param {*} db
 * @param {*} endpointTypeId
 */
 async function selectEndpointClusters(db, endpointTypeId) {
  let rows = await dbApi
    .dbAll(
      db,
      `
    SELECT * FROM ENDPOINT_TYPE_CLUSTER
    WHERE
      ENDPOINT_TYPE_REF = ?
    `,
      [endpointTypeId]
    )
    
    return rows.map(dbMapping.map.endpointTypeCluster)
}

/**
 * Promise to get a cluster's state.
 * This must return undefined/null for if the cluster state has not been used before for the endpointType
 * @param {*} db
 * @param {*} endpointTypeId
 * @param {*} clusterRef
 * @param {*} side
 */
async function selectClusterState(db, endpointTypeId, clusterRef, side) {
  return dbApi
    .dbGet(
      db,
      `
    SELECT
      ENDPOINT_TYPE_CLUSTER_ID,
      ENDPOINT_TYPE_REF,
      CLUSTER_REF,
      SIDE,
      ENABLED
    FROM ENDPOINT_TYPE_CLUSTER
    WHERE
      ENDPOINT_TYPE_REF = ? AND
      CLUSTER_REF = ? AND
      SIDE = ?
    `,
      [endpointTypeId, clusterRef, side]
    )
    .then(dbMapping.map.endpointTypeCluster)
}

/**
 * Promise to get clusters' states for all endpoints
 * This must return undefined/null for if the cluster state has not been used before for the endpointType
 * @param {*} db
 * @param {*} clusterRef
 * @param {*} side
 */
async function selectClusterStatesForAllEndpoints(db, clusterRef, side) {
  return dbApi
    .dbAll(
      db,
      `
    SELECT
      ENDPOINT_TYPE_CLUSTER_ID,
      ENDPOINT_TYPE_REF,
      CLUSTER_REF,
      SIDE,
      ENABLED
    FROM ENDPOINT_TYPE_CLUSTER
    WHERE
      CLUSTER_REF = ? AND
      SIDE = ?
    `,
      [clusterRef, side]
    )
    .then((rows) => rows.map(dbMapping.map.endpointTypeCluster))
}

/**
 * Promise that resolves after inserting the defaults associated with the clusterside to the database.
 * @param {*} db
 * @param {*} endpointTypeId
 * @param {*} clusterRef
 * @param {*} side
 */
async function insertClusterDefaults(db, endpointTypeId, packageId, cluster) {
  let promises = []
  promises.push(
    resolveDefaultAttributes(db, endpointTypeId, packageId, [cluster])
  )
  promises.push(
    resolveNonOptionalCommands(db, endpointTypeId, [cluster], packageId)
  )
  return Promise.all(promises)
}

/**
 * Promise to update the attribute state.
 * If the attribute entry [as defined uniquely by endpointTypeId and id], is not there, then create a default entry
 * Afterwards, update entry.
 * @param {*} db
 * @param {*} endpointTypeId
 * @param {*} clusterRef // We want this to be explicitly passed in, rather than derived from that static attribute due to this possibly being a global attribute.
 * @param {*} side // We want this to be explicitly passed in, rather than derived from that static attribute due to this possibly being a global attribute. Note we handle bidirectional attributes with two instances
 * @param {*} attributeId
 * @param {*} paramValuePairArray An array of objects whose keys are [key, value]. Key is name of the column to be editted. Value is what the column should be set to. This does not handle empty arrays.
 */
async function insertOrUpdateAttributeState(
  db,
  endpointTypeId,
  clusterRef,
  side,
  attributeId,
  paramValuePairArray,
  reportMinInterval,
  reportMaxInterval,
  reportableChange
) {
  if (reportMinInterval === undefined || reportMinInterval === null) {
    reportMinInterval = 1
  }

  if (reportMaxInterval === undefined || reportMaxInterval === null) {
    reportMaxInterval = 0xffff - 1
  }

  if (reportableChange === undefined || reportableChange === null) {
    reportableChange = 0
  }

  let cluster = await insertOrSelectDefaultEndpointTypeCluster(
    db,
    endpointTypeId,
    clusterRef,
    side
  )
  if (cluster == null) {
    throw new Error(`Could not locate cluster: ${clusterRef}`)
  }

  let staticAttribute =
    await queryZcl.selectAttributeByAttributeIdAndClusterRef(
      db,
      attributeId,
      clusterRef
    )

  if (staticAttribute == null) {
    throw new Error(`COULD NOT LOCATE ATTRIBUTE: ${attributeId} `)
  }
  let etaId = await dbApi.dbInsert(
    db,
    `
INSERT OR IGNORE
INTO ENDPOINT_TYPE_ATTRIBUTE (
    ENDPOINT_TYPE_REF,
    ENDPOINT_TYPE_CLUSTER_REF,
    ATTRIBUTE_REF,
    DEFAULT_VALUE,
    STORAGE_OPTION,
    SINGLETON,
    MIN_INTERVAL,
    MAX_INTERVAL,
    REPORTABLE_CHANGE
) VALUES (
  ?,
  ?,
  ?,
  ?,
  ?,
  ( SELECT IS_SINGLETON FROM CLUSTER WHERE CLUSTER_ID = ? ),
  ?,
  ?,
  ?
)`,
    [
      endpointTypeId,
      cluster.endpointTypeClusterId,
      attributeId,
      staticAttribute.defaultValue ? staticAttribute.defaultValue : '',
      dbEnum.storageOption.ram,
      clusterRef,
      reportMinInterval,
      reportMaxInterval,
      reportableChange,
    ]
  )

  if (paramValuePairArray == null || paramValuePairArray.length == 0) {
    return etaId
  } else {
    let query =
      'UPDATE ENDPOINT_TYPE_ATTRIBUTE SET ' +
      getAllParamValuePairArrayClauses(paramValuePairArray) +
      'WHERE ENDPOINT_TYPE_REF = ? AND ENDPOINT_TYPE_CLUSTER_REF = ? AND ATTRIBUTE_REF = ?'

    await dbApi.dbUpdate(db, query, [
      endpointTypeId,
      cluster.endpointTypeClusterId,
      attributeId,
    ])

    let row = await dbApi.dbGet(
      db,
      `
SELECT
  ETA.ENDPOINT_TYPE_ATTRIBUTE_ID
FROM
  ENDPOINT_TYPE_ATTRIBUTE AS ETA
WHERE
  ETA.ENDPOINT_TYPE_REF = ? AND ENDPOINT_TYPE_CLUSTER_REF = ? AND ATTRIBUTE_REF = ?`,
      [endpointTypeId, cluster.endpointTypeClusterId, attributeId]
    )
    return row.ENDPOINT_TYPE_ATTRIBUTE_ID
  }
}

async function updateEndpointTypeAttribute(db, id, keyValuePairs) {
  if (keyValuePairs == null || keyValuePairs.length == 0) return

  let columns = keyValuePairs
    .map((kv) => `${convertRestKeyToDbColumn(kv[0])} = ?`)
    .join(', ')

  let args = []
  keyValuePairs.forEach((kv) => {
    args.push(kv[1])
  })
  args.push(id)

  let query = `UPDATE ENDPOINT_TYPE_ATTRIBUTE SET 
  ${columns}
WHERE ENDPOINT_TYPE_ATTRIBUTE_ID = ?`
  return dbApi.dbUpdate(db, query, args)
}

function convertRestKeyToDbColumn(key) {
  switch (key) {
    case restApi.updateKey.endpointId:
      return 'ENDPOINT_IDENTIFIER'
    case restApi.updateKey.endpointType:
      return 'ENDPOINT_TYPE_REF'
    case restApi.updateKey.networkId:
      return 'NETWORK_IDENTIFIER'
    case restApi.updateKey.profileId:
      return 'PROFILE'
    case restApi.updateKey.deviceId:
      return 'DEVICE_IDENTIFIER'
    case restApi.updateKey.endpointVersion:
      return 'DEVICE_VERSION'
    case restApi.updateKey.deviceTypeRef:
      return 'DEVICE_TYPE_REF'
    case restApi.updateKey.name:
      return 'NAME'
    case restApi.updateKey.attributeSelected:
      return 'INCLUDED'
    case restApi.updateKey.attributeSingleton:
      return 'SINGLETON'
    case restApi.updateKey.attributeBounded:
      return 'BOUNDED'
    case restApi.updateKey.attributeDefault:
      return 'DEFAULT_VALUE'
    case restApi.updateKey.attributeReporting:
      return 'INCLUDED_REPORTABLE'
    case restApi.updateKey.attributeReportMin:
      return 'MIN_INTERVAL'
    case restApi.updateKey.attributeReportMax:
      return 'MAX_INTERVAL'
    case restApi.updateKey.attributeReportChange:
      return 'REPORTABLE_CHANGE'
    case restApi.updateKey.attributeStorage:
      return 'STORAGE_OPTION'
  }
  throw new Error(`Invalid rest update key: ${key}`)
}

function getAllParamValuePairArrayClauses(paramValuePairArray) {
  return paramValuePairArray.reduce((currentString, paramValuePair, index) => {
    if (index > 0) currentString += ','
    currentString += convertRestKeyToDbColumn(paramValuePair.key)
    currentString += ' = '
    if (_.isBoolean(paramValuePair.value)) {
      currentString += paramValuePair.value ? '1' : '0'
    } else if (paramValuePair.value == '') {
      currentString += false
    } else if (paramValuePair.value == null) {
      currentString += 'null'
    } else {
      if (paramValuePair.type == 'text') {
        currentString += "'" + paramValuePair.value + "'"
      } else {
        currentString += paramValuePair.value
      }
    }
    currentString += ' '
    return currentString
  }, '')
}

/**
 * Promise to update the command state.
 * If the attribute entry [as defined uniquely by endpointTypeId and id], is not there, then create a default entry
 * Afterwards, update entry.
 *
 * @param {*} db
 * @param {*} endpointTypeId
 * @param {*} clusterRef // Note that this is the clusterRef from CLUSTER and not the ENDPOINT_TYPE_CLUSTER
 * @param {*} side // client or server
 * @param {*} id
 * @param {*} value
 * @param {*} booleanParam
 */
async function insertOrUpdateCommandState(
  db,
  endpointTypeId,
  clusterRef,
  side,
  commandId,
  value,
  isIncoming
) {
  let cluster = await insertOrSelectDefaultEndpointTypeCluster(
    db,
    endpointTypeId,
    clusterRef,
    side
  )

  await dbApi.dbInsert(
    db,
    `
INSERT OR IGNORE
INTO ENDPOINT_TYPE_COMMAND (
  ENDPOINT_TYPE_REF,
  ENDPOINT_TYPE_CLUSTER_REF,
  COMMAND_REF
) VALUES( ?, ?, ? )
`,
    [endpointTypeId, cluster.endpointTypeClusterId, commandId]
  )
  return dbApi.dbUpdate(
    db,
    `
UPDATE ENDPOINT_TYPE_COMMAND
SET ${isIncoming ? 'INCOMING' : 'OUTGOING'} = ? 
WHERE ENDPOINT_TYPE_REF = ?
  AND ENDPOINT_TYPE_CLUSTER_REF = ?
  AND COMMAND_REF = ? `,
    [value, endpointTypeId, cluster.endpointTypeClusterId, commandId]
  )
}

/**
 * Promise to update the event state.
 * If the attribute entry [as defined uniquely by endpointTypeId and id], is not there, then create a default entry
 * Afterwards, update entry.
 *
 * @param {*} db
 * @param {*} endpointTypeId
 * @param {*} clusterRef // Note that this is the clusterRef from CLUSTER and not the ENDPOINT_TYPE_CLUSTER
 * @param {*} side // client or server
 * @param {*} eventId
 * @param {*} value
 */
async function insertOrUpdateEventState(
  db,
  endpointTypeId,
  clusterRef,
  side,
  eventId,
  value
) {
  let cluster = await insertOrSelectDefaultEndpointTypeCluster(
    db,
    endpointTypeId,
    clusterRef,
    side
  )

  await dbApi.dbInsert(
    db,
    `
INSERT OR IGNORE
INTO ENDPOINT_TYPE_EVENT (
  ENDPOINT_TYPE_REF,
  ENDPOINT_TYPE_CLUSTER_REF,
  EVENT_REF
) VALUES( ?, ?, ? )
`,
    [endpointTypeId, cluster.endpointTypeClusterId, eventId]
  )
  return dbApi.dbUpdate(
    db,
    `
UPDATE ENDPOINT_TYPE_EVENT
SET INCLUDED = ? 
WHERE ENDPOINT_TYPE_REF = ?
  AND ENDPOINT_TYPE_CLUSTER_REF = ?
  AND EVENT_REF = ? `,
    [value, endpointTypeId, cluster.endpointTypeClusterId, eventId]
  )
}

/**
 * Returns a promise to update the endpoint
 *
 * @param {*} db
 * @param {*} sessionId
 * @param {*} endpointId
 * @param {*} changesArray
 * @returns Promise to update the endpoint
 */
async function updateEndpoint(db, sessionId, endpointId, changesArray) {
  return dbApi.dbUpdate(
    db,
    `UPDATE ENDPOINT SET ` +
      getAllParamValuePairArrayClauses(changesArray) +
      `WHERE ENDPOINT_ID = ? AND SESSION_REF = ?`,
    [endpointId, sessionId]
  )
}

/**
 * Returns the number of endpoints with a given endpoint_identifier and sessionid.
 * Used for validation
 *
 * @param {*} endpointIdentifier
 * @param {*} sessionId
 * @returns Promise that resolves into a count.
 */
async function selectCountOfEndpointsWithGivenEndpointIdentifier(
  db,
  endpointIdentifier,
  sessionId
) {
  return dbApi
    .dbGet(
      db,
      'SELECT COUNT(ENDPOINT_IDENTIFIER) FROM ENDPOINT WHERE ENDPOINT_IDENTIFIER = ? AND SESSION_REF = ?',
      [endpointIdentifier, sessionId]
    )
    .then((x) => x['COUNT(ENDPOINT_IDENTIFIER)'])
}

/**
 * Promises to add an endpoint type.
 *
 * @export
 * @param {*} db
 * @param {*} sessionId
 * @param {*} name
 * @param {*} deviceTypeRef
 * @returns Promise to update endpoints.
 */
async function insertEndpointType(
  db,
  sessionId,
  name,
  deviceTypeRef,
  doTransaction = true
) {
  let newEndpointTypeId = await dbApi.dbInsert(
    db,
    'INSERT OR REPLACE INTO ENDPOINT_TYPE ( SESSION_REF, NAME, DEVICE_TYPE_REF ) VALUES ( ?, ?, ?)',
    [sessionId, name, deviceTypeRef]
  )
  await setEndpointDefaults(
    db,
    sessionId,
    newEndpointTypeId,
    deviceTypeRef,
    doTransaction
  )
  return newEndpointTypeId
}

/**
* Promises to duplicate an endpoint type.
*
* @export
* @param {*} db
* @param {*} endpointTypeId
* @returns Promise to duplicate endpoint type.
*/
async function duplicateEndpointType(
  db, 
  endpointTypeId
  ) {
  let newEndpointTypeId = await dbApi.dbInsert(
    db,
    `INSERT INTO ENDPOINT_TYPE (SESSION_REF, NAME, DEVICE_TYPE_REF)
    select SESSION_REF, NAME, DEVICE_TYPE_REF
    from ENDPOINT_TYPE
    where ENDPOINT_TYPE_ID = ?`,
    [endpointTypeId]
  )

  return newEndpointTypeId
}

/**
 * Promise to update a an endpoint type.
 * @param {*} db
 * @param {*} sessionId
 * @param {*} endpointTypeId
 * @param {*} param
 * @param {*} updatedValue
 */
async function updateEndpointType(
  db,
  sessionId,
  endpointTypeId,
  updateKey,
  updatedValue
) {
  let param = convertRestKeyToDbColumn(updateKey)
  let wasPresent = await dbApi.dbGet(
    db,
    'SELECT DEVICE_TYPE_REF FROM ENDPOINT_TYPE WHERE ENDPOINT_TYPE_ID = ? AND SESSION_REF = ?',
    [endpointTypeId, sessionId]
  )

  let newEndpointId = await dbApi.dbUpdate(
    db,
    `UPDATE ENDPOINT_TYPE SET ${param} = ? WHERE ENDPOINT_TYPE_ID = ? AND SESSION_REF = ?`,
    [updatedValue, endpointTypeId, sessionId]
  )

  if (param === 'DEVICE_TYPE_REF' && wasPresent[param] != updatedValue) {
    await setEndpointDefaults(db, sessionId, endpointTypeId, updatedValue)
  }
  return newEndpointId
}

/**
 * Promise to set the default attributes and clusters for a endpoint type.
 * @param {*} db
 * @param {*} endpointTypeId
 */
async function setEndpointDefaults(
  db,
  sessionId,
  endpointTypeId,
  deviceTypeRef,
  doTransaction = true
) {
  if (doTransaction) {
    await dbApi.dbBeginTransaction(db)
  }
  let pkgs = await queryPackage.getSessionPackagesByType(
    db,
    sessionId,
    dbEnum.packageType.zclProperties
  )
  if (pkgs == null || pkgs.length < 1)
    throw new Error('Could not locate package id for a given session.')

  let packageId = pkgs[0].id
  let clusters = await queryDeviceType.selectDeviceTypeClustersByDeviceTypeRef(
    db,
    deviceTypeRef
  )
  let defaultClusters = await resolveDefaultClusters(
    db,
    endpointTypeId,
    clusters
  )
  let promises = []

  promises.push(
    resolveDefaultDeviceTypeAttributes(db, endpointTypeId, deviceTypeRef),
    resolveDefaultDeviceTypeCommands(db, endpointTypeId, deviceTypeRef),
    resolveDefaultAttributes(db, endpointTypeId, packageId, defaultClusters),
    resolveNonOptionalCommands(db, endpointTypeId, defaultClusters, packageId)
  )

  return Promise.all(promises).finally(() => {
    if (doTransaction) return dbApi.dbCommit(db)
    else return Promise.resolve({ defaultClusters })
  })
}

/**
 * Returns a promise of resolving default clusters.
 *
 * @param {*} db
 * @param {*} endpointTypeId
 * @param {*} clusters
 * @returns Promise of resolved default clusters.
 */
async function resolveDefaultClusters(db, endpointTypeId, clusters) {
  let promises = clusters.map((cluster) => {
    let clientServerPromise = []
    if (cluster.includeClient) {
      clientServerPromise.push(
        insertOrReplaceClusterState(
          db,
          endpointTypeId,
          cluster.clusterRef,
          dbEnum.side.client,
          true
        ).then(() => {
          return {
            clusterRef: cluster.clusterRef,
            side: dbEnum.side.client,
          }
        })
      )
    }
    if (cluster.includeServer) {
      clientServerPromise.push(
        insertOrReplaceClusterState(
          db,
          endpointTypeId,
          cluster.clusterRef,
          dbEnum.side.server,
          true
        ).then(() => {
          return {
            clusterRef: cluster.clusterRef,
            side: dbEnum.side.server,
          }
        })
      )
    }
    return Promise.all(clientServerPromise)
  })
  let allClustersResult = await Promise.all(promises)
  return allClustersResult.flat()
}

/**
 * Returns promise of default device type attributes.
 *
 * @param {*} db
 * @param {*} endpointTypeId
 * @param {*} deviceTypeRef
 * @returns promise of default device type attributes.
 */
async function resolveDefaultDeviceTypeAttributes(
  db,
  endpointTypeId,
  deviceTypeRef
) {
  let deviceTypeAttributes =
    await queryDeviceType.selectDeviceTypeAttributesByDeviceTypeRef(
      db,
      deviceTypeRef
    )

  let promises = deviceTypeAttributes.map(async (deviceAttribute) => {
    if (deviceAttribute.attributeRef != null) {
      let attribute = await queryZcl.selectAttributeById(
        db,
        deviceAttribute.attributeRef
      )

      let clusterRef = attribute?.clusterRef

      return insertOrUpdateAttributeState(
        db,
        endpointTypeId,
        clusterRef,
        attribute.side,
        deviceAttribute.attributeRef,
        [
          {
            key: restApi.updateKey.attributeSelected,
            value: true,
          },
          {
            key: restApi.updateKey.attributeReporting,
            value:
              deviceAttribute.reportingPolicy ==
                dbEnum.reportingPolicy.mandatory ||
              deviceAttribute.reportingPolicy ==
                dbEnum.reportingPolicy.suggested,
          },
        ],
        attribute.reportMinInterval,
        attribute.reportMaxInterval,
        attribute.reportableChange
      )
    }
  })
  return Promise.all(promises)
}

async function resolveCommandState(db, endpointTypeId, deviceCommand) {
  let deviceTypeCluster =
    await queryDeviceType.selectDeviceTypeClusterByDeviceTypeClusterId(
      db,
      deviceCommand.deviceTypeClusterRef
    )
  if (deviceCommand.commandRef == null) return null

  let command = await queryCommand.selectCommandById(
    db,
    deviceCommand.commandRef
  )
  if (command == null) return null

  let promises = []
  if (deviceTypeCluster.includeClient) {
    promises.push(
      insertOrUpdateCommandState(
        db,
        endpointTypeId,
        command.clusterRef,
        command.source,
        deviceCommand.commandRef,
        true,
        command.source != dbEnum.source.client
      )
    )
  }
  if (deviceTypeCluster.includeServer) {
    promises.push(
      insertOrUpdateCommandState(
        db,
        endpointTypeId,
        command.clusterRef,
        command.source,
        deviceCommand.commandRef,
        true,
        command.source != dbEnum.source.server
      )
    )
  }
  return Promise.all(promises)
}

/**
 * Returns promise of default device type commands.
 *
 * @param {*} db
 * @param {*} endpointTypeId
 * @param {*} deviceTypeRef
 * @returns promise of default device type commands.
 */
async function resolveDefaultDeviceTypeCommands(
  db,
  endpointTypeId,
  deviceTypeRef
) {
  let commands = await queryDeviceType.selectDeviceTypeCommandsByDeviceTypeRef(
    db,
    deviceTypeRef
  )
  return Promise.all(
    commands.map((cmd) => resolveCommandState(db, endpointTypeId, cmd))
  )
}

async function resolveNonOptionalCommands(
  db,
  endpointTypeId,
  clusters,
  packageIds
) {
  let clustersPromises = clusters.map((cluster) =>
    queryCommand
      .selectCommandsByClusterId(db, cluster.clusterRef, packageIds)
      .then((commands) =>
        Promise.all(
          commands.map((command) => {
            if (!command.isOptional) {
              let isOutgoing =
                (cluster.side == dbEnum.side.client &&
                  command.source == dbEnum.source.client) ||
                (cluster.side == dbEnum.side.server &&
                  command.source == dbEnum.source.server)
              return insertOrUpdateCommandState(
                db,
                endpointTypeId,
                command.clusterRef,
                command.source,
                command.id,
                true,
                !isOutgoing
              )
            } else {
              return Promise.resolve()
            }
          })
        )
      )
  )
  return Promise.all(clustersPromises)
}

async function resolveDefaultAttributes(
  db,
  endpointTypeId,
  packageId,
  endpointClusters
) {
  let endpointClustersPromises = endpointClusters.map((cluster) =>
    queryZcl
      .selectAttributesByClusterIdIncludingGlobal(db, cluster.clusterRef, [
        packageId,
      ])
      .then((attributes) => {
        let promiseArray = []
        promiseArray.push(
          resolveNonOptionalAndReportableAttributes(
            db,
            endpointTypeId,
            attributes,
            cluster
          )
        )
        return Promise.all(promiseArray)
      })
  )
  return Promise.all(endpointClustersPromises)
}

async function resolveNonOptionalAndReportableAttributes(
  db,
  endpointTypeId,
  attributes,
  cluster
) {
  let promises = attributes.map((attribute) => {
    let settings = []
    if (
      attribute.reportingPolicy == dbEnum.reportingPolicy.mandatory ||
      attribute.reportingPolicy == dbEnum.reportingPolicy.suggested
    )
      settings.push({
        key: restApi.updateKey.attributeReporting,
        value: true,
      })
    if (!attribute.isOptional) {
      settings.push({
        key: restApi.updateKey.attributeSelected,
        value: true,
      })
    }
    if (cluster.isSingleton) {
      settings.push({
        key: restApi.updateKey.attributeSingleton,
        value: true,
      })
    }
    let clusterRef = cluster.clusterRef

    if (settings.length > 0 && clusterRef != null) {
      return insertOrUpdateAttributeState(
        db,
        endpointTypeId,
        clusterRef,
        attribute.side,
        attribute.id,
        settings,
        attribute.reportMinInterval,
        attribute.reportMaxInterval,
        attribute.reportableChange
      )
    } else {
      return Promise.resolve()
    }
  })
  return Promise.all(promises)
}

/**
 * Resolves into the number of endpoint types for session.
 *
 * @param {*} db
 * @param {*} sessionId
 * @returns Promise that resolves into a count.
 */
async function selectEndpointTypeCount(db, sessionId) {
  let x = await dbApi.dbGet(
    db,
    'SELECT COUNT(ENDPOINT_TYPE_ID) AS CNT FROM ENDPOINT_TYPE WHERE SESSION_REF = ?',
    [sessionId]
  )
  return x['CNT']
}

/**
 * Resolves into the number of endpoint types for session.
 * by cluster ID
 *
 * @param {*} db
 * @param {*} sessionId
 * @returns Promise that resolves into a count.
 */
async function selectEndpointTypeCountByCluster(
  db,
  sessionId,
  endpointClusterId,
  side
) {
  let x = await dbApi.dbGet(
    db,
    `
SELECT
  COUNT(ENDPOINT_TYPE_ID)
FROM
  ENDPOINT_TYPE
WHERE SESSION_REF = ?
  AND ENDPOINT_TYPE_ID IN
      (SELECT ENDPOINT_TYPE_REF
       FROM ENDPOINT_TYPE_CLUSTER
       WHERE CLUSTER_REF = ? AND SIDE = ? AND ENABLED = 1) `,
    [sessionId, endpointClusterId, side]
  )
  return x['COUNT(ENDPOINT_TYPE_ID)']
}

/**
 * Get or inserts default endpoint type cluster given endpoint type, cluster ref, and side.
 * @param {*} db
 * @param {*} endpointTypeId
 * @param {*} clusterRef
 * @param {*} side
 */

async function insertOrSelectDefaultEndpointTypeCluster(
  db,
  endpointTypeId,
  clusterRef,
  side
) {
  await dbApi.dbInsert(
    db,
    `
INSERT OR IGNORE
INTO ENDPOINT_TYPE_CLUSTER (
  ENDPOINT_TYPE_REF, CLUSTER_REF, SIDE, ENABLED
) VALUES ( ?, ?, ?, ? )
`,
    [endpointTypeId, clusterRef, side, false]
  )

  let eptClusterData = await dbApi.dbGet(
    db,
    `
SELECT
  ENDPOINT_TYPE_CLUSTER_ID,
  ENDPOINT_TYPE_REF,
  CLUSTER_REF,
  SIDE,
  ENABLED
FROM ENDPOINT_TYPE_CLUSTER
WHERE ENDPOINT_TYPE_REF = ?
  AND CLUSTER_REF = ?
  AND SIDE = ?`,
    [endpointTypeId, clusterRef, side]
  )

  return dbMapping.map.endpointTypeCluster(eptClusterData)
}

/**
 * Returns a promise that resolve into an endpoint type attribute id.
 *
 * @param {*} db
 * @param {*} sessionId
 * @param {*} clusterCode
 * @param {*} attributeCode
 * @param {*} attributeSide
 * @param {*} mfgCode
 * @returns endpointType attribute id or null
 */
async function selectEndpointTypeAttributeId(
  db,
  endpointTypeId,
  packageId,
  clusterCode,
  attributeCode,
  attributeSide,
  mfgCode
) {
  let args = [
    endpointTypeId,
    packageId,
    clusterCode,
    attributeCode,
    attributeSide,
  ]
  if (!(mfgCode == 0 || mfgCode == null)) args.push(mfgCode)
  let rows = await dbApi.dbAll(
    db,
    `
SELECT 
  ENDPOINT_TYPE_ATTRIBUTE_ID
FROM 
  ENDPOINT_TYPE_ATTRIBUTE AS ETA
INNER JOIN
  ATTRIBUTE AS A
ON
  ETA.ATTRIBUTE_REF = A.ATTRIBUTE_ID 
INNER JOIN
  CLUSTER AS C
ON 
  C.CLUSTER_ID = A.CLUSTER_REF
WHERE
  ETA.ENDPOINT_TYPE_REF = ?
  AND C.PACKAGE_REF = ?
  AND C.CODE = ?
  AND A.CODE = ?
  AND A.SIDE = ?
  AND ${
    mfgCode == 0 || mfgCode == null
      ? 'A.MANUFACTURER_CODE IS NULL'
      : 'A.MANUFACTURER_CODE = ?'
  }
`,
    args
  )
  if (rows.length == 0) {
    return null
  } else if (rows.length == 1) {
    return rows[0].ENDPOINT_TYPE_ATTRIBUTE_ID
  } else {
    throw new Error(
      `Ambiguity: multiple attributes with same data loaded: ${endpointTypeId} / ${clusterCode} / ${attributeCode} / ${attributeSide}.`
    )
  }
}

/**
 * Retrieves all the attribute data for the session.
 *
 * @param {*} db
 * @param {*} sessionId
 */
async function selectAllSessionAttributes(db, sessionId) {
  return dbApi
    .dbAll(
      db,
      `
SELECT
  A.NAME,
  A.CODE AS ATTRIBUTE_CODE,
  C.CODE AS CLUSTER_CODE,
  ETA.DEFAULT_VALUE,
  ETA.STORAGE_OPTION,
  ETA.SINGLETON,
  ETA.BOUNDED,
  A.TYPE,
  A.SIDE,
  A.MIN,
  A.MAX,
  A.IS_WRITABLE,
  A.ARRAY_TYPE,
  ETA.INCLUDED_REPORTABLE,
  ETA.MIN_INTERVAL,
  ETA.MAX_INTERVAL,
  ETA.REPORTABLE_CHANGE
FROM
  ENDPOINT_TYPE_ATTRIBUTE AS ETA
JOIN
  ENDPOINT_TYPE_CLUSTER AS ETC ON ETA.ENDPOINT_TYPE_CLUSTER_REF = ETC.ENDPOINT_TYPE_CLUSTER_ID
JOIN
  CLUSTER AS C ON ETC.CLUSTER_REF = C.CLUSTER_ID
JOIN
  ATTRIBUTE AS A ON ETA.ATTRIBUTE_REF = A.ATTRIBUTE_ID
JOIN
  ENDPOINT_TYPE AS ET ON ETA.ENDPOINT_TYPE_REF = ET.ENDPOINT_TYPE_ID
WHERE
  ET.SESSION_REF = ? AND ETA.INCLUDED = 1
ORDER BY
  CLUSTER_CODE, ATTRIBUTE_CODE
  `,
      [sessionId]
    )
    .then((rows) =>
      rows.map((row) => {
        return {
          name: row.NAME,
          attributeCode: row.ATTRIBUTE_CODE,
          clusterCode: row.CLUSTER_CODE,
          defaultValue: row.DEFAULT_VALUE,
          storageOption: row.STORAGE_OPTION,
          isSingleton: row.SINGLETON,
          isBounded: row.BOUNDED,
          type: row.TYPE,
          side: row.SIDE,
          min: row.MIN,
          max: row.MAX,
          writable: row.IS_WRITABLE,
          entryType: row.ARRAY_TYPE,
          reportable: {
            included: row.INCLUDED_REPORTABLE,
            minInterval: row.MIN_INTERVAL,
            maxInterval: row.MAX_INTERVAL,
            change: row.REPORTABLE_CHANGE,
          },
        }
      })
    )
}

/**
 * Sets a given cluster to be included on a given endpoint.
 *
 * @param {*} db
 * @param {*} packageId
 * @param {*} clusterCode
 * @param {*} isIncluded
 * @param {*} side
 */
async function setClusterIncluded(
  db,
  packageId,
  endpointTypeId,
  clusterCode,
  isIncluded,
  side
) {
  let cluster = await queryZcl.selectClusterByCode(db, packageId, clusterCode)
  let clusterState = await selectClusterState(
    db,
    endpointTypeId,
    cluster.id,
    side
  )
  let insertDefaults = clusterState == null
  await insertOrReplaceClusterState(
    db,
    endpointTypeId,
    cluster.id,
    side,
    isIncluded
  )
  if (insertDefaults) {
    await insertClusterDefaults(db, endpointTypeId, packageId, {
      clusterRef: cluster.id,
      side: side,
    })
  }
}

// exports
exports.insertOrReplaceClusterState = insertOrReplaceClusterState
exports.selectClusterState = selectClusterState
exports.selectClusterStatesForAllEndpoints = selectClusterStatesForAllEndpoints
exports.insertOrUpdateAttributeState = insertOrUpdateAttributeState
exports.insertOrUpdateCommandState = insertOrUpdateCommandState
exports.insertOrUpdateEventState = insertOrUpdateEventState
exports.convertRestKeyToDbColumn = convertRestKeyToDbColumn
exports.duplicateEndpointType = duplicateEndpointType
exports.selectEndpointClusters = selectEndpointClusters

exports.updateEndpoint = updateEndpoint

exports.insertEndpointType = insertEndpointType
exports.updateEndpointType = updateEndpointType

exports.selectCountOfEndpointsWithGivenEndpointIdentifier =
  selectCountOfEndpointsWithGivenEndpointIdentifier
exports.selectEndpointTypeCount = selectEndpointTypeCount
exports.selectEndpointTypeCountByCluster = selectEndpointTypeCountByCluster
exports.selectAllSessionAttributes = selectAllSessionAttributes
exports.insertClusterDefaults = insertClusterDefaults

exports.setClusterIncluded = setClusterIncluded
exports.selectEndpointTypeAttributeId = selectEndpointTypeAttributeId
exports.updateEndpointTypeAttribute = updateEndpointTypeAttribute
