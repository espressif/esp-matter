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
 * This module provides queries for endpoint type.
 *
 * @module DB API: endpoint type queries against the database.
 */
const dbApi = require('./db-api.js')
const dbMapping = require('./db-mapping.js')

/**
 * Promise to delete an endpoint type.
 * @param {*} db
 * @param {*} sessionId
 * @param {*} id
 */

async function deleteEndpointType(db, id) {
  return dbApi.dbRemove(
    db,
    'DELETE FROM ENDPOINT_TYPE WHERE ENDPOINT_TYPE_ID = ?',
    [id]
  )
}

/**
 * Extracts raw endpoint types rows.
 *
 * @export
 * @param {*} db
 * @param {*} sessionId
 * @returns promise that resolves into rows in the database table.
 */
async function selectAllEndpointTypes(db, sessionId) {
  let rows = await dbApi.dbAll(
    db,
    `
SELECT
  ENDPOINT_TYPE_ID,
  NAME,
  DEVICE_TYPE_REF,
  SESSION_REF
FROM
  ENDPOINT_TYPE
WHERE SESSION_REF = ? ORDER BY NAME`,
    [sessionId]
  )
  return rows.map(dbMapping.map.endpointType)
}

/**
 * Extracts endpoint type ids.
 *
 * @export
 * @param {*} db
 * @param {*} sessionId
 * @returns promise that resolves into rows in the database table.
 */
async function selectEndpointTypeIds(db, sessionId) {
  let mapFunction = (x) => {
    return {
      endpointTypeId: x.ENDPOINT_TYPE_ID,
    }
  }
  return dbApi
    .dbAll(
      db,
      `
SELECT
  ENDPOINT_TYPE.ENDPOINT_TYPE_ID
FROM
  ENDPOINT_TYPE
LEFT JOIN
  DEVICE_TYPE
ON
  ENDPOINT_TYPE.DEVICE_TYPE_REF = DEVICE_TYPE.DEVICE_TYPE_ID
WHERE
  ENDPOINT_TYPE.SESSION_REF = ?
ORDER BY ENDPOINT_TYPE.NAME`,
      [sessionId]
    )
    .then((rows) => rows.map(mapFunction))
}

/**
 * Extracts endpoint type ids which belong to user endpoints.
 * There have been occasions when the endpoint types are present
 * but they do not belong to any endpoints.
 *
 * @export
 * @param {*} db
 * @param {*} sessionId
 * @returns promise that resolves into rows in the database table.
 */
async function selectUsedEndpointTypeIds(db, sessionId) {
  let mapFunction = (x) => {
    return {
      endpointTypeId: x.ENDPOINT_TYPE_ID,
    }
  }
  return dbApi
    .dbAll(
      db,
      `
SELECT
  ENDPOINT_TYPE.ENDPOINT_TYPE_ID
FROM
  ENDPOINT_TYPE
INNER JOIN
  ENDPOINT
ON
  ENDPOINT_TYPE.ENDPOINT_TYPE_ID = ENDPOINT.ENDPOINT_TYPE_REF
LEFT JOIN
  DEVICE_TYPE
ON
  ENDPOINT_TYPE.DEVICE_TYPE_REF = DEVICE_TYPE.DEVICE_TYPE_ID
WHERE
  ENDPOINT_TYPE.SESSION_REF = ?
ORDER BY ENDPOINT_TYPE.NAME`,
      [sessionId]
    )
    .then((rows) => rows.map(mapFunction))
}

/**
 * Returns promise of a details of an endpoint type.
 *
 * @param {*} db
 * @param {*} id
 * @returns endpoint type
 */
async function selectEndpointType(db, id) {
  return dbApi
    .dbGet(
      db,
      `SELECT ENDPOINT_TYPE_ID, SESSION_REF, NAME, DEVICE_TYPE_REF FROM ENDPOINT_TYPE WHERE ENDPOINT_TYPE_ID = ?`,
      [id]
    )
    .then(dbMapping.map.endpointType)
}

/**
 * Retrieve clusters from multiple endpoint types, calculating along the way the
 * number of how many times a cluster is present in the given endpoint types.
 *
 * @param {*} db
 * @param {*} endpointTypes
 * @returns Promise that resolves with the data that should go into the external form.
 */
async function selectAllClustersDetailsFromEndpointTypes(db, endpointTypes) {
  let endpointTypeIds = endpointTypes.map((ep) => ep.endpointTypeId).toString()
  let mapFunction = (x) => {
    return {
      id: x.CLUSTER_ID,
      name: x.NAME,
      code: x.CODE,
      description: x.DESCRIPTION,
      define: x.DEFINE,
      mfgCode: x.MANUFACTURER_CODE,
      side: x.SIDE,
      enabled: x.ENABLED,
      endpointClusterId: x.ENDPOINT_TYPE_CLUSTER_ID,
      endpointCount: x['COUNT(*)'],
    }
  }

  let doOrderBy = true
  return dbApi
    .dbAll(
      db,
      `
SELECT
  CLUSTER.CLUSTER_ID,
  CLUSTER.CODE,
  CLUSTER.DESCRIPTION,
  CLUSTER.MANUFACTURER_CODE,
  CLUSTER.NAME,
  CLUSTER.DEFINE,
  ENDPOINT_TYPE_CLUSTER.SIDE,
  ENDPOINT_TYPE_CLUSTER.ENABLED,
  ENDPOINT_TYPE_CLUSTER.ENDPOINT_TYPE_CLUSTER_ID,
  COUNT(*)
FROM
  CLUSTER
INNER JOIN
  ENDPOINT_TYPE_CLUSTER
ON
  CLUSTER.CLUSTER_ID = ENDPOINT_TYPE_CLUSTER.CLUSTER_REF
WHERE
  ENDPOINT_TYPE_CLUSTER.ENDPOINT_TYPE_REF IN (${endpointTypeIds})
AND
  ENDPOINT_TYPE_CLUSTER.SIDE IS NOT "" AND ENDPOINT_TYPE_CLUSTER.ENABLED = 1
GROUP BY
  NAME, SIDE
${
  doOrderBy
    ? 'ORDER BY CLUSTER.MANUFACTURER_CODE, CLUSTER.CODE, CLUSTER.DEFINE'
    : ''
}`
    )
    .then((rows) => rows.map(mapFunction))
}

/**
 * Endpoint type details along with their cluster and attribute details
 *
 * @param db
 * @param endpointsAndClusters
 * @returns Endpoint type details
 */
async function selectEndpointDetailsFromAddedEndpoints(
  db,
  endpointsAndClusters
) {
  let endpointClusterIds = endpointsAndClusters
    .map((ep) => ep.endpointClusterId)
    .toString()
  let mapFunction = (x) => {
    return {
      id: x.ATTRIBUTE_ID,
      attributeSize: x.ATOMIC_SIZE,
      endpointIndex: x.ENDPOINT_INDEX,
      clusterCount: x.CLUSTER_COUNT,
      attributesSize: x.ATTRIBUTES_SIZE,
      endpointTypeId: x.ENDPOINT_TYPE_ID,
      endpointIdentifier: x.ENDPOINT_IDENTIFIER,
      totalAttributeSizeAcrossEndpoints: x.ALL_ATTRIBUTES_SIZE_ACROSS_ENDPOINTS,
      profileId: x.PROFILE_ID,
      deviceId: x.DEVICE_ID,
      deviceVersion: x.DEVICE_VERSION,
      networkId: x.NETWORK_ID,
    }
  }
  return dbApi
    .dbAll(
      db,
      `
SELECT * FROM (
  SELECT
    ATTRIBUTE.ATTRIBUTE_ID AS ATTRIBUTE_ID,
    CASE
      WHEN ATOMIC.IS_STRING=1 THEN 
        CASE WHEN ATOMIC.IS_LONG=0 THEN ATTRIBUTE.MAX_LENGTH+1
             WHEN ATOMIC.IS_LONG=1 THEN ATTRIBUTE.MAX_LENGTH+2
             ELSE ATOMIC.ATOMIC_SIZE
             END
        WHEN ATOMIC.ATOMIC_SIZE IS NULL THEN ATTRIBUTE.MAX_LENGTH
        ELSE ATOMIC.ATOMIC_SIZE
    END AS ATOMIC_SIZE,
    ROW_NUMBER() OVER (PARTITION BY ENDPOINT.ENDPOINT_IDENTIFIER) ENDPOINT_INDEX,
    (DENSE_RANK() over (PARTITION BY ENDPOINT.ENDPOINT_IDENTIFIER ORDER BY CLUSTER.MANUFACTURER_CODE, CLUSTER.NAME, ENDPOINT_TYPE_CLUSTER.SIDE) + DENSE_RANK() OVER (PARTITION BY ENDPOINT_TYPE.ENDPOINT_TYPE_ID ORDER BY CLUSTER.MANUFACTURER_CODE DESC, CLUSTER.NAME DESC, ENDPOINT_TYPE_CLUSTER.SIDE DESC) - 1) AS CLUSTER_COUNT,
    SUM(CASE
          WHEN
            (ENDPOINT_TYPE_ATTRIBUTE.SINGLETON != 1 AND ENDPOINT_TYPE_ATTRIBUTE.STORAGE_OPTION != "External")
          THEN
            (CASE
              WHEN
                ATOMIC.IS_STRING=1
              THEN 
                CASE
                  WHEN
                    ATOMIC.IS_LONG=0 THEN ATTRIBUTE.MAX_LENGTH+1
                  WHEN
                    ATOMIC.IS_LONG=1 THEN ATTRIBUTE.MAX_LENGTH+2
                  ELSE
                    ATOMIC.ATOMIC_SIZE
                END
              WHEN
                ATOMIC.ATOMIC_SIZE IS NULL
              THEN
                ATTRIBUTE.MAX_LENGTH
              ELSE
                ATOMIC.ATOMIC_SIZE
            END)
          ELSE
              0
        END
      ) OVER (PARTITION BY ENDPOINT.ENDPOINT_IDENTIFIER) ATTRIBUTES_SIZE,
    ENDPOINT_TYPE.ENDPOINT_TYPE_ID AS ENDPOINT_TYPE_ID,
    ENDPOINT.ENDPOINT_IDENTIFIER AS ENDPOINT_IDENTIFIER,
    SUM(CASE
          WHEN
            (ENDPOINT_TYPE_ATTRIBUTE.SINGLETON != 1 AND ENDPOINT_TYPE_ATTRIBUTE.STORAGE_OPTION != "External")
          THEN
            (CASE
              WHEN
                ATOMIC.IS_STRING=1
              THEN 
                CASE
                  WHEN
                    ATOMIC.IS_LONG=0
                  THEN
                    ATTRIBUTE.MAX_LENGTH+1
                  WHEN
                    ATOMIC.IS_LONG=1
                  THEN
                    ATTRIBUTE.MAX_LENGTH+2
                  ELSE
                    ATOMIC.ATOMIC_SIZE
                END
              WHEN
                ATOMIC.ATOMIC_SIZE IS NULL
              THEN
                ATTRIBUTE.MAX_LENGTH
              ELSE
                ATOMIC.ATOMIC_SIZE
            END)
          ELSE
            0
        END
        ) OVER () ALL_ATTRIBUTES_SIZE_ACROSS_ENDPOINTS,
    ENDPOINT.PROFILE AS PROFILE_ID,
    ENDPOINT.DEVICE_IDENTIFIER AS DEVICE_ID,
    ENDPOINT.DEVICE_VERSION AS DEVICE_VERSION,
    ENDPOINT.NETWORK_IDENTIFIER AS NETWORK_ID
  FROM ATTRIBUTE
  INNER JOIN ENDPOINT_TYPE_ATTRIBUTE
  ON ATTRIBUTE.ATTRIBUTE_ID = ENDPOINT_TYPE_ATTRIBUTE.ATTRIBUTE_REF
  INNER JOIN ENDPOINT_TYPE_CLUSTER
  ON ENDPOINT_TYPE_ATTRIBUTE.ENDPOINT_TYPE_CLUSTER_REF = ENDPOINT_TYPE_CLUSTER.ENDPOINT_TYPE_CLUSTER_ID
  INNER JOIN CLUSTER
  ON ENDPOINT_TYPE_CLUSTER.CLUSTER_REF = CLUSTER.CLUSTER_ID
  INNER JOIN ATOMIC
  ON ATOMIC.NAME = ATTRIBUTE.TYPE
  INNER JOIN ENDPOINT_TYPE
  ON ENDPOINT_TYPE.ENDPOINT_TYPE_ID = ENDPOINT_TYPE_CLUSTER.ENDPOINT_TYPE_REF
  INNER JOIN ENDPOINT
  ON ENDPOINT_TYPE.ENDPOINT_TYPE_ID = ENDPOINT.ENDPOINT_TYPE_REF
  WHERE ENDPOINT_TYPE_ATTRIBUTE.ENDPOINT_TYPE_CLUSTER_REF IN (${endpointClusterIds}) AND ENDPOINT_TYPE_CLUSTER.ENABLED=1
  AND ENDPOINT_TYPE_ATTRIBUTE.INCLUDED = 1 AND ENDPOINT_TYPE_CLUSTER.SIDE=ATTRIBUTE.SIDE
  GROUP BY ENDPOINT.ENDPOINT_IDENTIFIER, CLUSTER.MANUFACTURER_CODE, CLUSTER.NAME, ENDPOINT_TYPE_CLUSTER.SIDE, ATTRIBUTE.NAME) WHERE ENDPOINT_INDEX=1 ORDER BY ENDPOINT_IDENTIFIER
        `
    )
    .then((rows) => rows.map(mapFunction))
}

/**
 * Exports clusters to an externalized form without duplicates caused by side.
 *
 * @param {*} db
 * @param {*} endpointTypeId
 * @returns Promise that resolves with the data that should go into the external form.
 */
async function selectAllClustersNamesFromEndpointTypes(db, endpointTypes) {
  let endpointTypeIds = endpointTypes.map((ep) => ep.endpointTypeId).toString()
  let mapFunction = (x) => {
    return {
      id: x.CLUSTER_ID,
      name: x.NAME,
      code: x.CODE,
      define: x.DEFINE,
      mfgCode: x.MANUFACTURER_CODE,
      enabled: x.ENABLED,
      endpointClusterId: x.ENDPOINT_TYPE_CLUSTER_ID,
    }
  }
  return dbApi
    .dbAll(
      db,
      `
SELECT
  CLUSTER.CLUSTER_ID,
  CLUSTER.CODE,
  CLUSTER.MANUFACTURER_CODE,
  CLUSTER.NAME,
  CLUSTER.DEFINE,
  ENDPOINT_TYPE_CLUSTER.SIDE,
  ENDPOINT_TYPE_CLUSTER.ENABLED,
  ENDPOINT_TYPE_CLUSTER.ENDPOINT_TYPE_CLUSTER_ID
FROM
  CLUSTER
INNER JOIN
  ENDPOINT_TYPE_CLUSTER
ON
  CLUSTER.CLUSTER_ID = ENDPOINT_TYPE_CLUSTER.CLUSTER_REF
WHERE
  ENDPOINT_TYPE_CLUSTER.ENDPOINT_TYPE_REF IN (${endpointTypeIds})
  AND ENDPOINT_TYPE_CLUSTER.SIDE IS NOT "" AND ENDPOINT_TYPE_CLUSTER.ENABLED = 1
GROUP BY
  NAME`
    )
    .then((rows) => rows.map(mapFunction))
}

/**
 * Exports clusters to an externalized form irrespecive of side.
 *
 * @param {*} db
 * @param {*} endpointTypeId
 * @returns Promise that resolves with the data that should go into the external form.
 */
async function selectAllClustersDetailsIrrespectiveOfSideFromEndpointTypes(
  db,
  endpointTypes
) {
  let endpointTypeIds = endpointTypes.map((ep) => ep.endpointTypeId).toString()
  let mapFunction = (x) => {
    return {
      id: x.CLUSTER_ID,
      name: x.NAME,
      code: x.CODE,
      define: x.DEFINE,
      mfgCode: x.MANUFACTURER_CODE,
      side: x.SIDE,
      enabled: x.ENABLED,
      endpointClusterId: x.ENDPOINT_TYPE_CLUSTER_ID,
    }
  }

  return dbApi
    .dbAll(
      db,
      `
SELECT
  CLUSTER.CLUSTER_ID,
  CLUSTER.CODE,
  CLUSTER.MANUFACTURER_CODE,
  CLUSTER.NAME,
  CLUSTER.DEFINE,
  ENDPOINT_TYPE_CLUSTER.SIDE,
  ENDPOINT_TYPE_CLUSTER.ENABLED,
  ENDPOINT_TYPE_CLUSTER.ENDPOINT_TYPE_CLUSTER_ID
FROM CLUSTER
INNER JOIN ENDPOINT_TYPE_CLUSTER
ON CLUSTER.CLUSTER_ID = ENDPOINT_TYPE_CLUSTER.CLUSTER_REF
WHERE ENDPOINT_TYPE_CLUSTER.ENDPOINT_TYPE_REF IN (${endpointTypeIds})
AND ENDPOINT_TYPE_CLUSTER.SIDE IS NOT "" AND ENDPOINT_TYPE_CLUSTER.ENABLED=1
GROUP BY NAME`
    )
    .then((rows) => rows.map(mapFunction))
}

/**
 * Returns a promise of data for commands inside all existing endpoint types.
 *
 * @param {*} db
 * @param {*} endpointTypes
 * @param {*} endpointClusterId
 * @param {*} packageIds
 * @returns Promise that resolves with the command data.
 */
async function selectCommandDetailsFromAllEndpointTypeCluster(
  db,
  endpointTypes,
  endpointClusterId,
  packageIds
) {
  let endpointTypeIds = endpointTypes.map((ep) => ep.endpointTypeId).toString()
  let mapFunction = (x) => {
    return {
      id: x.COMMAND_ID,
      name: x.NAME,
      code: x.CODE,
      mfgCode: x.MANUFACTURER_CODE,
      incoming: x.INCOMING,
      outgoing: x.OUTGOING,
      description: x.DESCRIPTION,
    }
  }
  return dbApi
    .dbAll(
      db,
      `
  SELECT
    COMMAND.COMMAND_ID,
    COMMAND.NAME,
    COMMAND.CODE,
    COMMAND.MANUFACTURER_CODE,
    ENDPOINT_TYPE_COMMAND.INCOMING,
    ENDPOINT_TYPE_COMMAND.OUTGOING,
    COMMAND.DESCRIPTION
  FROM COMMAND
  INNER JOIN ENDPOINT_TYPE_COMMAND
  ON COMMAND.COMMAND_ID = ENDPOINT_TYPE_COMMAND.COMMAND_REF
  WHERE ENDPOINT_TYPE_COMMAND.ENDPOINT_TYPE_REF IN (${endpointTypeIds}) AND ENDPOINT_TYPE_COMMAND.ENDPOINT_TYPE_CLUSTER_REF = ?
  AND COMMAND.PACKAGE_REF IN (${dbApi.toInClause(packageIds)})
        `,
      [endpointClusterId]
    )
    .then((rows) => rows.map(mapFunction))
}

/**
 * Exports clusters and endpoint ids
 *
 * This function outputs an array of objects. Each object contains:
 *    endpointId - the id of the endpoint
 *    endpointClusterId - the id of the endpoint_type_cluster table listing this.
 *    endpointTypeClusterRef - clusterID of the given cluster.
 *
 * @param {*} db
 * @param {*} endpointTypes
 * @returns Promise that resolves with the data that contains cluster
 * and endpoint id references
 */
async function selectClustersAndEndpointDetailsFromEndpointTypes(
  db,
  endpointTypes
) {
  let endpointTypeIds = endpointTypes.map((ep) => ep.endpointTypeId).toString()
  let mapFunction = (x) => {
    return {
      endpointId: x.ENDPOINT_TYPE_REF,
      endpointClusterId: x.ENDPOINT_TYPE_CLUSTER_ID,
      endpointTypeClusterRef: x.CLUSTER_REF,
    }
  }

  let rows = await dbApi.dbAll(
    db,
    `
SELECT
  ETC.ENDPOINT_TYPE_REF,
  ETC.ENDPOINT_TYPE_CLUSTER_ID,
  ETC.CLUSTER_REF
FROM 
  CLUSTER
INNER JOIN 
  ENDPOINT_TYPE_CLUSTER AS ETC
ON 
  CLUSTER.CLUSTER_ID = ETC.CLUSTER_REF
WHERE
  ETC.ENDPOINT_TYPE_REF IN (${endpointTypeIds})`
  )
  return rows.map(mapFunction)
}

exports.deleteEndpointType = deleteEndpointType
exports.selectAllEndpointTypes = selectAllEndpointTypes
exports.selectEndpointTypeIds = selectEndpointTypeIds
exports.selectUsedEndpointTypeIds = selectUsedEndpointTypeIds
exports.selectEndpointType = selectEndpointType
exports.selectAllClustersDetailsFromEndpointTypes =
  selectAllClustersDetailsFromEndpointTypes
exports.selectEndpointDetailsFromAddedEndpoints =
  selectEndpointDetailsFromAddedEndpoints

exports.selectAllClustersNamesFromEndpointTypes =
  selectAllClustersNamesFromEndpointTypes
exports.selectAllClustersDetailsIrrespectiveOfSideFromEndpointTypes =
  selectAllClustersDetailsIrrespectiveOfSideFromEndpointTypes
exports.selectCommandDetailsFromAllEndpointTypeCluster =
  selectCommandDetailsFromAllEndpointTypeCluster

exports.selectClustersAndEndpointDetailsFromEndpointTypes =
  selectClustersAndEndpointDetailsFromEndpointTypes
