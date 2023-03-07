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
 * This module provides queries for endpoint configuration.
 *
 * @module DB API: endpoint configuration queries against the database.
 */
const dbApi = require('./db-api.js')
const bin = require('../util/bin')
const dbMapping = require('./db-mapping.js')

/**
 * Returns a promise resolving into all endpoints.
 *
 * @param {*} db
 * @param {*} sessionId
 * @returns Promise resolving into all endpoints.
 */
async function selectAllEndpoints(db, sessionId) {
  let rows = await dbApi.dbAll(
    db,
    `
SELECT
  ENDPOINT_ID,
  SESSION_REF,
  ENDPOINT_TYPE_REF,
  PROFILE,
  ENDPOINT_IDENTIFIER,
  NETWORK_IDENTIFIER,
  DEVICE_VERSION,
  DEVICE_IDENTIFIER
FROM ENDPOINT
WHERE SESSION_REF = ?
ORDER BY ENDPOINT_IDENTIFIER
    `,
    [sessionId]
  )
  return rows.map(dbMapping.map.endpoint)
}

/**
 * Retrieves clusters on an endpoint.
 *
 * @param {*} db
 * @param {*} endpointTypeId
 * @returns promise that resolves into endpoint clusters.
 */
async function selectEndpointClusters(db, endpointTypeId) {
  let rows = await dbApi.dbAll(
    db,
    `
SELECT
  C.CLUSTER_ID,
  EC.ENDPOINT_TYPE_CLUSTER_ID,
  EC.ENDPOINT_TYPE_REF,
  C.CODE,
  C.NAME,
  C.MANUFACTURER_CODE,
  EC.SIDE
FROM
  CLUSTER AS C
LEFT JOIN
  ENDPOINT_TYPE_CLUSTER AS EC
ON
  C.CLUSTER_ID = EC.CLUSTER_REF
WHERE
  EC.ENABLED = 1
  AND EC.ENDPOINT_TYPE_REF = ?
ORDER BY C.CODE
`,
    [endpointTypeId]
  )

  return rows.map((row) => {
    return {
      clusterId: row['CLUSTER_ID'],
      endpointTypeId: row['ENDPOINT_TYPE_REF'],
      endpointTypeClusterId: row['ENDPOINT_TYPE_CLUSTER_ID'],
      hexCode: '0x' + bin.int16ToHex(row['CODE']),
      manufacturerCode: row['MANUFACTURER_CODE'],
      code: row['CODE'],
      name: row['NAME'],
      side: row['SIDE'],
    }
  })
}

/**
 * Retrieves endpoint cluster attributes
 *
 * @param {*} db
 * @param {*} clusterId
 * @param {*} side
 * @param {*} endpointTypeId
 * @returns promise that resolves into endpoint cluster attributes
 */
async function selectEndpointClusterAttributes(
  db,
  clusterId,
  side,
  endpointTypeId
) {
  let rows = await dbApi.dbAll(
    db,
    `
SELECT
  A.ATTRIBUTE_ID,
  A.CODE,
  A.NAME,
  A.SIDE,
  A.TYPE,
  A.ARRAY_TYPE,
  A.MIN_LENGTH,
  A.MAX_LENGTH,
  A.MIN,
  A.MAX,
  A.MANUFACTURER_CODE,
  A.IS_WRITABLE,
  A.IS_NULLABLE,
  A.MUST_USE_TIMED_WRITE,
  A.IS_SCENE_REQUIRED,
  A.IS_OPTIONAL,
  A.DEFINE,
  EA.STORAGE_OPTION,
  EA.SINGLETON,
  EA.BOUNDED,
  EA.INCLUDED,
  EA.DEFAULT_VALUE,
  EA.INCLUDED_REPORTABLE,
  EA.MIN_INTERVAL,
  EA.MAX_INTERVAL,
  EA.REPORTABLE_CHANGE
FROM
  ATTRIBUTE AS A
LEFT JOIN
  ENDPOINT_TYPE_ATTRIBUTE AS EA
ON
  A.ATTRIBUTE_ID = EA.ATTRIBUTE_REF
WHERE
  (A.CLUSTER_REF = ? OR A.CLUSTER_REF IS NULL)
  AND A.SIDE = ?
  AND (EA.ENDPOINT_TYPE_REF = ? AND (EA.ENDPOINT_TYPE_CLUSTER_REF =
    (SELECT ENDPOINT_TYPE_CLUSTER_ID
     FROM ENDPOINT_TYPE_CLUSTER
     WHERE CLUSTER_REF = ? AND SIDE = ? AND ENDPOINT_TYPE_REF = ?) ))
ORDER BY A.MANUFACTURER_CODE, A.CODE
    `,
    [clusterId, side, endpointTypeId, clusterId, side, endpointTypeId]
  )

  return rows.map((row) => {
    return {
      id: row.ATTRIBUTE_ID,
      clusterId: clusterId,
      code: row.CODE,
      manufacturerCode: row.MANUFACTURER_CODE,
      hexCode: '0x' + bin.int16ToHex(row['CODE']),
      name: row.NAME,
      side: row.SIDE,
      type: row.TYPE,
      entryType: row.ARRAY_TYPE,
      minLength: row.MIN_LENGTH,
      maxLength: row.MAX_LENGTH,
      min: row.MIN,
      max: row.MAX,
      storage: row.STORAGE_OPTION,
      isIncluded: dbApi.fromDbBool(row.INCLUDED),
      isSingleton: dbApi.fromDbBool(row.SINGLETON),
      isBound: dbApi.fromDbBool(row.BOUNDED),
      isWritable: dbApi.fromDbBool(row.IS_WRITABLE),
      isNullable: dbApi.fromDbBool(row.IS_NULLABLE),
      mustUseTimedWrite: dbApi.fromDbBool(row.MUST_USE_TIMED_WRITE),
      isSceneRequired: dbApi.fromDbBool(row.IS_SCENE_REQUIRED),
      isOptional: dbApi.fromDbBool(row.IS_OPTIONA),
      defaultValue: row.DEFAULT_VALUE,
      includedReportable: dbApi.fromDbBool(row.INCLUDED_REPORTABLE),
      minInterval: row.MIN_INTERVAL,
      maxInterval: row.MAX_INTERVAL,
      reportableChange: row.REPORTABLE_CHANGE,
      define: row.DEFINE,
    }
  })
}

/**
 * Retrieves endpoint cluster commands.
 *
 * @param {*} db
 * @param {*} clusterId
 * @param {*} endpointTypeId
 * @returns promise that resolves into endpoint cluster commands
 */
async function selectEndpointClusterCommands(db, clusterId, endpointTypeId) {
  let rows = await dbApi.dbAll(
    db,
    `
SELECT
  C.COMMAND_ID,
  C.NAME,
  C.CODE,
  C.CLUSTER_REF,
  C.SOURCE,
  C.MANUFACTURER_CODE,
  C.IS_OPTIONAL,
  C.MUST_USE_TIMED_INVOKE,
  C.IS_FABRIC_SCOPED,
  C.RESPONSE_NAME,
  RC.MANUFACTURER_CODE as RESPONSE_MANUFACTURER_CODE,
  RC.CODE AS RESPONSE_CODE,
  C.RESPONSE_REF,
  ETC.INCOMING,
  ETC.OUTGOING
FROM
  COMMAND AS C
LEFT JOIN
  ENDPOINT_TYPE_COMMAND AS ETC
ON
  C.COMMAND_ID = ETC.COMMAND_REF
LEFT JOIN
  COMMAND AS RC
ON
  RC.COMMAND_ID = C.RESPONSE_REF
WHERE
  C.CLUSTER_REF = ?
  AND ETC.ENDPOINT_TYPE_REF = ?
ORDER BY C.CODE
  `,
    [clusterId, endpointTypeId]
  )

  return rows.map((row) => {
    return {
      id: row['COMMAND_ID'],
      name: row['NAME'],
      code: row['CODE'],
      clusterId: row['CLUSTER_REF'],
      manufacturerCode: row['MANUFACTURER_CODE'],
      isOptional: dbApi.fromDbBool(row['IS_OPTIONAL']),
      mustUseTimedInvoke: dbApi.fromDbBool(row['MUST_USE_TIMED_INVOKE']),
      isFabricScoped: dbApi.fromDbBool(row['IS_FABRIC_SCOPED']),
      source: row['SOURCE'],
      isIncoming: dbApi.fromDbBool(row['INCOMING']),
      isOutgoing: dbApi.fromDbBool(row['OUTGOING']),
      responseName: row['RESPONSE_NAME'],
      responseManufacturerCode: row['RESPONSE_MANUFACTURER_CODE'],
      responseCode: row['RESPONSE_CODE'],
      responseRef: row['RESPONSE_REF'],
      hexCode: '0x' + bin.int8ToHex(row['CODE']),
    }
  })
}

/**
 * Deletes an endpoint.
 *
 * @export
 * @param {*} db
 * @param {*} id
 * @returns Promise to delete an endpoint that resolves with the number of rows that were deleted.
 */
async function deleteEndpoint(db, id) {
  return dbApi.dbRemove(db, 'DELETE FROM ENDPOINT WHERE ENDPOINT_ID = ?', [id])
}

/**
 * Promises to add an endpoint.
 *
 * @export
 * @param {*} db
 * @param {*} sessionId
 * @param {*} endpointIdentifier
 * @param {*} endpointTypeRef
 * @param {*} networkIdentifier
 * @returns Promise to update endpoints.
 */
async function insertEndpoint(
  db,
  sessionId,
  endpointIdentifier,
  endpointTypeRef,
  networkIdentifier,
  profileIdentifier,
  endpointVersion,
  deviceIdentifier
) {
  return dbApi.dbInsert(
    db,
    `
INSERT OR REPLACE
INTO ENDPOINT (
  SESSION_REF,
  ENDPOINT_IDENTIFIER,
  ENDPOINT_TYPE_REF,
  NETWORK_IDENTIFIER,
  DEVICE_VERSION,
  DEVICE_IDENTIFIER,
  PROFILE
) VALUES ( ?, ?, ?, ?, ?, ?, ?)`,
    [
      sessionId,
      endpointIdentifier,
      endpointTypeRef,
      networkIdentifier,
      endpointVersion,
      deviceIdentifier,
      profileIdentifier,
    ]
  )
}

/**
* @export
* @param {*} db
* @param {*} id
* @param {*} endpointIdentifier
* @returns Promise to duplicate an endpoint.
*/
async function duplicateEndpoint( db, id, endpointIdentifier, endpointTypeId ) {
  return dbApi.dbInsert( db,
    `
    insert into ENDPOINT (SESSION_REF,ENDPOINT_IDENTIFIER,ENDPOINT_TYPE_REF,NETWORK_IDENTIFIER,DEVICE_VERSION,DEVICE_IDENTIFIER,PROFILE)
    select SESSION_REF, ? , ? ,NETWORK_IDENTIFIER,DEVICE_VERSION,DEVICE_IDENTIFIER,PROFILE
    from ENDPOINT
    where ENDPOINT_ID = ?`,
    [
      endpointIdentifier,
      endpointTypeId,
      id
    ]
  )
}

/**
 * Returns a promise of a single endpoint.
 * Mayb resolve into null if invalid reference.
 *
 * @param {*} db
 * @param {*} endpointId
 * @returns Promise of an endpoint.
 */
async function selectEndpoint(db, endpointId) {
  return dbApi
    .dbGet(
      db,
      `
SELECT
  ENDPOINT_ID,
  SESSION_REF,
  ENDPOINT_IDENTIFIER,
  ENDPOINT_TYPE_REF,
  PROFILE,
  NETWORK_IDENTIFIER,
  DEVICE_VERSION,
  DEVICE_IDENTIFIER
FROM
  ENDPOINT
WHERE
  ENDPOINT_ID = ?`,
      [endpointId]
    )
    .then(dbMapping.map.endpoint)
}

exports.selectEndpointClusters = selectEndpointClusters
exports.selectEndpointClusterAttributes = selectEndpointClusterAttributes
exports.selectEndpointClusterCommands = selectEndpointClusterCommands
exports.insertEndpoint = insertEndpoint
exports.deleteEndpoint = deleteEndpoint
exports.selectEndpoint = selectEndpoint
exports.duplicateEndpoint = duplicateEndpoint
exports.selectAllEndpoints = selectAllEndpoints
