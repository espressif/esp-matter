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
 * This module provides queries related to imports and exports of files.
 *
 * @module DB API: package-based queries.
 */
const dbApi = require('./db-api')
const dbEnums = require('../../src-shared/db-enum')
/**
 * Imports a single endpoint
 * @param {} db
 * @param {*} sessionId
 * @param {*} endpoint
 * @param {*} endpointTypeRef
 */
async function importEndpoint(db, sessionId, endpoint, endpointTypeRef) {
  return dbApi.dbInsert(
    db,
    `
INSERT INTO ENDPOINT (
  SESSION_REF,
  ENDPOINT_TYPE_REF,
  PROFILE,
  ENDPOINT_IDENTIFIER,
  NETWORK_IDENTIFIER,
  DEVICE_VERSION,
  DEVICE_IDENTIFIER
) VALUES (
  ?,
  ?,
  ?,
  ?,
  ?,
  ?,
  ?
)
  `,
    [
      sessionId,
      endpointTypeRef,
      endpoint.profileId,
      endpoint.endpointId,
      endpoint.networkId,
      endpoint.endpointVersion,
      endpoint.deviceIdentifier,
    ]
  )
}

/**
 * Extracts endpoints.
 *
 * @param {*} db
 * @param {*} sessionId
 */
async function exportEndpoints(db, sessionId, endpointTypes) {
  let endpointTypeIndexFunction = (epts, endpointTypeRef) => {
    return epts.findIndex((value) => value.endpointTypeId == endpointTypeRef)
  }

  let mapFunction = (x) => {
    return {
      endpointTypeName: x.NAME,
      endpointTypeIndex: endpointTypeIndexFunction(
        endpointTypes,
        x.ENDPOINT_TYPE_REF
      ),
      endpointTypeRef: x.ENDPOINT_TYPE_REF,
      profileId: x.PROFILE,
      endpointId: x.ENDPOINT_IDENTIFIER,
      networkId: x.NETWORK_IDENTIFIER,
      endpointVersion: x.DEVICE_VERSION,
      deviceIdentifier: x.DEVICE_IDENTIFIER,
    }
  }
  return dbApi
    .dbAll(
      db,
      `
SELECT
  ET.NAME,
  E.ENDPOINT_TYPE_REF,
  E.PROFILE,
  E.ENDPOINT_IDENTIFIER,
  E.NETWORK_IDENTIFIER,
  E.DEVICE_VERSION,
  E.DEVICE_IDENTIFIER
FROM
  ENDPOINT AS E
LEFT JOIN
  ENDPOINT_TYPE AS ET
ON
  E.ENDPOINT_TYPE_REF = ET.ENDPOINT_TYPE_ID
WHERE
  E.SESSION_REF = ?
ORDER BY E.ENDPOINT_IDENTIFIER
    `,
      [sessionId]
    )
    .then((rows) => rows.map(mapFunction))
}

/**
 * Extracts raw endpoint types rows.
 *
 * @export
 * @param {*} db
 * @param {*} sessionId
 * @returns promise that resolves into rows in the database table.
 */
async function exportEndpointTypes(db, sessionId) {
  let mapFunction = (x) => {
    return {
      endpointTypeId: x.ENDPOINT_TYPE_ID,
      name: x.NAME,
      deviceTypeName: x.DEVICE_TYPE_NAME,
      deviceTypeCode: x.DEVICE_TYPE_CODE,
      deviceTypeProfileId: x.DEVICE_TYPE_PROFILE_ID,
    }
  }
  return dbApi
    .dbAll(
      db,
      `
SELECT DISTINCT
  ENDPOINT_TYPE.ENDPOINT_TYPE_ID,
  ENDPOINT_TYPE.NAME,
  ENDPOINT_TYPE.DEVICE_TYPE_REF,
  DEVICE_TYPE.CODE AS DEVICE_TYPE_CODE,
  DEVICE_TYPE.PROFILE_ID as DEVICE_TYPE_PROFILE_ID,
  DEVICE_TYPE.NAME AS DEVICE_TYPE_NAME
FROM
  ENDPOINT_TYPE
LEFT JOIN
  ENDPOINT
ON
  ENDPOINT.ENDPOINT_TYPE_REF = ENDPOINT_TYPE.ENDPOINT_TYPE_ID
LEFT JOIN
  DEVICE_TYPE
ON
  ENDPOINT_TYPE.DEVICE_TYPE_REF = DEVICE_TYPE.DEVICE_TYPE_ID
WHERE
  ENDPOINT_TYPE.SESSION_REF = ?
ORDER BY
  ENDPOINT.ENDPOINT_IDENTIFIER,
  ENDPOINT_TYPE.NAME,
  DEVICE_TYPE_CODE,
  DEVICE_TYPE_PROFILE_ID`,
      [sessionId]
    )
    .then((rows) => rows.map(mapFunction))
}

/**
 * Imports an endpoint type, resolving other data along the way.
 *
 * @param {*} db
 * @param {*} sessionId
 * @param {*} packageId
 * @param {*} endpointType
 * @returns Promise of endpoint insertion.
 */
async function importEndpointType(db, sessionId, packageIds, endpointType) {
  let multipleDeviceIds = await dbApi.dbAll(
    db,
    `SELECT DEVICE_TYPE_ID FROM DEVICE_TYPE WHERE CODE = "${parseInt(
      endpointType.deviceTypeCode
    )}" AND PROFILE_ID = "${parseInt(
      endpointType.deviceTypeProfileId
    )}" AND PACKAGE_REF IN ("${packageIds}")`
  )
  if (multipleDeviceIds != null && multipleDeviceIds.length > 1) {
    // Each endpoint has: 'name', 'deviceTypeName', 'deviceTypeCode', `deviceTypeProfileId`, 'clusters', 'commands', 'attributes'
    let deviceTypeId = await dbApi
      .dbAll(
        db,
        `SELECT DEVICE_TYPE_ID,PACKAGE_REF FROM DEVICE_TYPE WHERE CODE = ? AND PROFILE_ID = ? AND NAME = ? AND PACKAGE_REF IN (${dbApi.toInClause(
          packageIds
        )})`,
        [
          parseInt(endpointType.deviceTypeCode),
          parseInt(endpointType.deviceTypeProfileId),
          endpointType.deviceTypeName,
        ]
      )
      .then((matchedPackageIds) => matchedPackageIds.shift()?.DEVICE_TYPE_ID)

    return dbApi.dbInsert(
      db,
      `
  INSERT INTO ENDPOINT_TYPE (
    SESSION_REF,
    NAME,
    DEVICE_TYPE_REF
  ) VALUES(?, ?, ?)`,
      [sessionId, endpointType.name, deviceTypeId]
    )
  } else {
    // Each endpoint has: 'name', 'deviceTypeName', 'deviceTypeCode', `deviceTypeProfileId`, 'clusters', 'commands', 'attributes'
    let deviceTypeId = await dbApi
      .dbAll(
        db,
        `SELECT DEVICE_TYPE_ID,PACKAGE_REF FROM DEVICE_TYPE WHERE CODE = ? AND PROFILE_ID = ? AND PACKAGE_REF IN (${dbApi.toInClause(
          packageIds
        )})`,
        [
          parseInt(endpointType.deviceTypeCode),
          parseInt(endpointType.deviceTypeProfileId),
        ]
      )
      .then((matchedPackageIds) => matchedPackageIds.shift()?.DEVICE_TYPE_ID)

    return dbApi.dbInsert(
      db,
      `
  INSERT INTO ENDPOINT_TYPE (
    SESSION_REF,
    NAME,
    DEVICE_TYPE_REF
  ) VALUES( ?, ?, ?)`,
      [sessionId, endpointType.name, deviceTypeId]
    )
  }
}

/**
 * Exports packages for externalized form.
 *
 * @param {*} db
 * @param {*} sessionId
 * @returns Promise of a data that is listing all the packages in the session.
 */
async function exportPackagesFromSession(db, sessionId) {
  let mapFunction = (x) => {
    return {
      path: x.PATH,
      category: x.CATEGORY,
      version: x.VERSION,
      description: x.DESCRIPTION,
      type: x.TYPE,
      required: x.REQUIRED,
    }
  }
  return dbApi
    .dbAll(
      db,
      `
SELECT
  PACKAGE.PATH,
  PACKAGE.CATEGORY,
  PACKAGE.VERSION,
  PACKAGE.DESCRIPTION,
  PACKAGE.TYPE,
  SESSION_PACKAGE.REQUIRED
FROM PACKAGE
INNER JOIN SESSION_PACKAGE
ON PACKAGE.PACKAGE_ID = SESSION_PACKAGE.PACKAGE_REF
WHERE SESSION_PACKAGE.SESSION_REF = ? AND SESSION_PACKAGE.ENABLED = 1`,
      [sessionId]
    )
    .then((rows) => rows.map(mapFunction))
}

/**
 * Exports clusters to an externalized form.
 *
 * @param {*} db
 * @param {*} endpointTypeId
 * @returns Promise that resolves with the data that should go into the external form.
 */
async function exportClustersFromEndpointType(db, endpointTypeId) {
  let mapFunction = (x) => {
    return {
      name: x.NAME,
      code: x.CODE,
      mfgCode: x.MANUFACTURER_CODE,
      define: x.DEFINE,
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
WHERE ENDPOINT_TYPE_CLUSTER.ENDPOINT_TYPE_REF = ?
ORDER BY CLUSTER.CODE, CLUSTER.NAME`,
      [endpointTypeId]
    )
    .then((rows) => rows.map(mapFunction))
}

/**
 * Imports a single cluster to endpoint type.
 *
 * @param {*} db
 * @param {*} endpointTypeId
 * @param {*} cluster Object populated same way as export method leaves it.
 * @returns Promise of an imported cluster.
 */
async function importClusterForEndpointType(
  db,
  packageIds,
  endpointTypeId,
  cluster
) {
  let matchedPackageId = await dbApi
    .dbAll(
      db,
      `SELECT CLUSTER_ID, PACKAGE_REF FROM CLUSTER WHERE PACKAGE_REF IN (${dbApi.toInClause(
        packageIds
      )}) AND CODE = ? AND ${
        cluster.mfgCode == null
          ? 'MANUFACTURER_CODE IS NULL'
          : 'MANUFACTURER_CODE = ?'
      }`,
      cluster.mfgCode == null ? [cluster.code] : [cluster.code, cluster.mfgCode]
    )
    .then((matchedPackageIds) => matchedPackageIds.shift()?.PACKAGE_REF)

  return dbApi.dbInsert(
    db,
    `
INSERT INTO ENDPOINT_TYPE_CLUSTER
  (ENDPOINT_TYPE_REF, CLUSTER_REF, SIDE, ENABLED)
VALUES
  (?,
   (SELECT CLUSTER_ID FROM CLUSTER WHERE PACKAGE_REF = ? AND CODE = ? AND ${
     cluster.mfgCode == null
       ? 'MANUFACTURER_CODE IS NULL'
       : 'MANUFACTURER_CODE = ?'
   }),
   ?,
   ?)`,
    cluster.mfgCode == null
      ? [
          endpointTypeId,
          matchedPackageId,
          cluster.code,
          cluster.side,
          cluster.enabled,
        ]
      : [
          endpointTypeId,
          matchedPackageId,
          cluster.code,
          cluster.mfgCode,
          cluster.side,
          cluster.enabled,
        ]
  )
}

/**
 * Returns a promise of data for events inside an endpoint type.
 *
 * @param {*} db
 * @param {*} endpointTypeId
 * @param {*} endpointClusterId
 * @returns Promise that resolves with the events data.
 */
async function exportEventsFromEndpointTypeCluster(
  db,
  endpointTypeId,
  endpointClusterId
) {
  let mapFunction = (x) => {
    return {
      name: x.NAME,
      code: x.CODE,
      mfgCode: x.MANUFACTURER_CODE,
      side: x.SIDE,
      included: x.INCLUDED,
    }
  }
  return dbApi
    .dbAll(
      db,
      `
SELECT
  E.NAME,
  E.CODE,
  E.MANUFACTURER_CODE,
  E.SIDE,
  ETE.INCLUDED
FROM
  EVENT AS E
INNER JOIN
  ENDPOINT_TYPE_EVENT AS ETE
ON
  E.EVENT_ID = ETE.EVENT_REF
WHERE
  ETE.ENDPOINT_TYPE_REF = ?
  AND ETE.ENDPOINT_TYPE_CLUSTER_REF = ?
ORDER BY
  E.CODE, E.MANUFACTURER_CODE
  `,
      [endpointTypeId, endpointClusterId]
    )
    .then((rows) => rows.map(mapFunction))
}

/**
 * Imports an event information of an endpoint type.
 *
 * @param {*} db
 * @param {*} packageId
 * @param {*} endpointTypeId
 * @param {*} endpointClusterId
 * @param {*} event
 * @returns Promise of an event insertion.
 */
async function importEventForEndpointType(
  db,
  packageIds,
  endpointTypeId,
  endpointClusterId,
  event
) {
  let selectEventQuery = `
SELECT
  E.EVENT_ID
FROM
  EVENT AS E
INNER JOIN
  ENDPOINT_TYPE_CLUSTER AS ETC
ON
  E.CLUSTER_REF = ETC.CLUSTER_REF
WHERE
  E.CODE = ?
  AND E.PACKAGE_REF IN (${dbApi.toInClause(packageIds)})
  AND E.SIDE = ETC.SIDE
  AND ETC.ENDPOINT_TYPE_CLUSTER_ID = ?
  AND ${
    event.mfgCode == null
      ? 'E.MANUFACTURER_CODE IS NULL'
      : 'E.MANUFACTURER_CODE = ?'
  }`

  let selectArgs = [event.code, endpointClusterId]
  if (event.mfgCode != null) selectArgs.push(event.mfgCode)

  let eRow = await dbApi.dbAll(db, selectEventQuery, selectArgs)
  let eventId
  if (eRow.length == 0) {
    eventId = null
  } else {
    eventId = eRow[0].EVENT_ID
  }

  // We got the ids, now we update ENDPOINT_TYPE_EVENT
  let arg = [endpointTypeId, endpointClusterId, eventId, event.included]
  return dbApi.dbInsert(
    db,
    `
INSERT INTO ENDPOINT_TYPE_EVENT (
  ENDPOINT_TYPE_REF,
  ENDPOINT_TYPE_CLUSTER_REF,
  EVENT_REF,
  INCLUDED
) VALUES (
  ?,?,?,?
)
`,
    arg
  )
}

/**
 * Returns a promise of data for attributes inside an endpoint type.
 *
 * @param {*} db
 * @param {*} endpointTypeId
 * @param {*} endpointClusterId
 * @returns Promise that resolves with the attribute data.
 */
async function exportAttributesFromEndpointTypeCluster(
  db,
  endpointTypeId,
  endpointClusterId
) {
  let mapFunction = (x) => {
    return {
      name: x.NAME,
      code: x.CODE,
      mfgCode: x.MANUFACTURER_CODE,
      side: x.SIDE,
      type: x.TYPE,
      included: x.INCLUDED,
      storageOption: x.STORAGE_OPTION,
      singleton: x.SINGLETON,
      bounded: x.BOUNDED,
      defaultValue: x.DEFAULT_VALUE,
      reportable: x.INCLUDED_REPORTABLE,
      minInterval: x.MIN_INTERVAL,
      maxInterval: x.MAX_INTERVAL,
      reportableChange: x.REPORTABLE_CHANGE,
    }
  }
  return dbApi
    .dbAll(
      db,
      `
SELECT
  A.NAME,
  A.CODE,
  A.MANUFACTURER_CODE,
  A.SIDE,
  A.TYPE,
  ETA.INCLUDED,
  ETA.STORAGE_OPTION,
  ETA.SINGLETON,
  ETA.BOUNDED,
  ETA.DEFAULT_VALUE,
  ETA.INCLUDED_REPORTABLE,
  ETA.MIN_INTERVAL,
  ETA.MAX_INTERVAL,
  ETA.REPORTABLE_CHANGE
FROM
  ATTRIBUTE AS A
INNER JOIN
  ENDPOINT_TYPE_ATTRIBUTE AS ETA
ON
  A.ATTRIBUTE_ID = ETA.ATTRIBUTE_REF
WHERE
  ETA.ENDPOINT_TYPE_REF = ?
  AND ETA.ENDPOINT_TYPE_CLUSTER_REF = ?
ORDER BY
  A.CODE, A.MANUFACTURER_CODE
    `,
      [endpointTypeId, endpointClusterId]
    )
    .then((rows) => rows.map(mapFunction))
}

/**
 * Imports an attribute information of an endpoint type.
 *
 * @param {*} db
 * @param {*} packageId
 * @param {*} endpointTypeId
 * @param {*} endpointClusterId may be null if global attribute
 * @param {*} attribute
 * @returns Promise of an attribute insertion.
 */
async function importAttributeForEndpointType(
  db,
  packageIds,
  endpointTypeId,
  endpointClusterId,
  attribute
) {
  let selectAttributeQuery = `
SELECT
  A.ATTRIBUTE_ID,
  A.REPORTING_POLICY,
  A.STORAGE_POLICY
FROM
  ATTRIBUTE AS A
INNER JOIN
  ENDPOINT_TYPE_CLUSTER AS ETC
ON
  ETC.CLUSTER_REF = A.CLUSTER_REF OR A.CLUSTER_REF IS NULL
WHERE
  A.CODE = ?
  AND A.PACKAGE_REF IN (${dbApi.toInClause(packageIds)})
  AND A.SIDE = ETC.SIDE
  AND ETC.ENDPOINT_TYPE_CLUSTER_ID = ?
  AND ${
    attribute.mfgCode == null
      ? 'A.MANUFACTURER_CODE IS NULL'
      : 'A.MANUFACTURER_CODE = ?'
  }`

  let selectArgs = [attribute.code, endpointClusterId]
  if (attribute.mfgCode != null) selectArgs.push(attribute.mfgCode)

  let atRow = await dbApi.dbAll(db, selectAttributeQuery, selectArgs)
  let attributeId
  let reportingPolicy
  let storagePolicy
  if (atRow.length == 0) {
    attributeId = null
    reportingPolicy = null
    storagePolicy = null
  } else {
    attributeId = atRow[0].ATTRIBUTE_ID
    reportingPolicy = atRow[0].REPORTING_POLICY
    storagePolicy = atRow[0].STORAGE_POLICY
  }

  // If the spec has meanwhile changed the policies to mandatory or prohibited,
  // we update the flags in the file to the requirements.
  if (reportingPolicy == dbEnums.reportingPolicy.mandatory) {
    attribute.reportable = true
  } else if (reportingPolicy == dbEnums.reportingPolicy.prohibited) {
    attribute.reportable = false
  }

  if (storagePolicy == dbEnums.storagePolicy.attributeAccessInterface) {
    attribute.storageOption = dbEnums.storageOption.external
  }

  let arg = [
    endpointTypeId,
    endpointClusterId,
    attributeId,
    attribute.included,
    attribute.storageOption,
    attribute.singleton,
    attribute.bounded,
    attribute.defaultValue,
    attribute.reportable,
    attribute.minInterval,
    attribute.maxInterval,
    attribute.reportableChange,
  ]

  return dbApi.dbInsert(
    db,
    `
INSERT INTO ENDPOINT_TYPE_ATTRIBUTE ( 
  ENDPOINT_TYPE_REF,
  ENDPOINT_TYPE_CLUSTER_REF,
  ATTRIBUTE_REF,
  INCLUDED,
  STORAGE_OPTION,
  SINGLETON,
  BOUNDED,
  DEFAULT_VALUE,
  INCLUDED_REPORTABLE,
  MIN_INTERVAL,
  MAX_INTERVAL,
  REPORTABLE_CHANGE 
) VALUES ( 
  ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?
)
  `,
    arg
  )
}

/**
 * Returns a promise of data for commands inside an endpoint type.
 *
 * @param {*} db
 * @param {*} endpointTypeId
 * @returns Promise that resolves with the command data.
 */
async function exportCommandsFromEndpointTypeCluster(
  db,
  endpointTypeId,
  endpointClusterId
) {
  let mapFunction = (x) => {
    return {
      name: x.NAME,
      code: x.CODE,
      mfgCode: x.MANUFACTURER_CODE,
      source: x.SOURCE,
      incoming: x.INCOMING,
      outgoing: x.OUTGOING,
    }
  }
  return dbApi
    .dbAll(
      db,
      `
SELECT
  C.NAME,
  C.CODE,
  C.MANUFACTURER_CODE,
  C.SOURCE,
  ETC.INCOMING,
  ETC.OUTGOING
FROM
  COMMAND AS C
INNER JOIN
  ENDPOINT_TYPE_COMMAND AS ETC
ON
  C.COMMAND_ID = ETC.COMMAND_REF
WHERE
  ETC.ENDPOINT_TYPE_REF = ?
  AND ETC.ENDPOINT_TYPE_CLUSTER_REF = ?
ORDER BY
  C.MANUFACTURER_CODE, C.CODE
        `,
      [endpointTypeId, endpointClusterId]
    )
    .then((rows) => rows.map(mapFunction))
}

/**
 * Imports a command information of an endpoint type.
 *
 * @param {*} db
 * @param {*} packageId
 * @param {*} endpointTypeId
 * @param {*} endpointClusterId may be null if global command
 * @param {*} command
 * @returns Promise of a command insertion.
 */
async function importCommandForEndpointType(
  db,
  packageIds,
  endpointTypeId,
  endpointClusterId,
  command
) {
  let matchedCmdId = await dbApi
    .dbAll(
      db,
      `SELECT COMMAND_ID
      FROM COMMAND, ENDPOINT_TYPE_CLUSTER WHERE
        COMMAND.CODE = ?
        AND COMMAND.SOURCE = ?
        AND COMMAND.PACKAGE_REF IN (${dbApi.toInClause(packageIds)})
        AND ENDPOINT_TYPE_CLUSTER.ENDPOINT_TYPE_CLUSTER_ID = ?
        AND COMMAND.CLUSTER_REF = ENDPOINT_TYPE_CLUSTER.CLUSTER_REF
        AND ${
          command.mfgCode == null
            ? 'MANUFACTURER_CODE IS NULL'
            : 'MANUFACTURER_CODE = ?'
        }`,
      command.mfgCode == null
        ? [command.code, command.source, endpointClusterId]
        : [command.code, command.source, endpointClusterId, command.mfgCode]
    )
    .then((matchedCmdIds) => matchedCmdIds.shift()?.COMMAND_ID)

  let arg = [
    endpointTypeId,
    endpointClusterId,
    matchedCmdId,
    command.incoming,
    command.outgoing,
  ]
  return dbApi.dbInsert(
    db,
    `
INSERT INTO ENDPOINT_TYPE_COMMAND
( ENDPOINT_TYPE_REF,
  ENDPOINT_TYPE_CLUSTER_REF,
  COMMAND_REF,
  INCOMING,
  OUTGOING )
VALUES
  (?, ?, ?, ?, ?)
  `,
    arg
  )
}

exports.exportEndpointTypes = exportEndpointTypes
exports.importEndpointType = importEndpointType

exports.exportClustersFromEndpointType = exportClustersFromEndpointType
exports.importClusterForEndpointType = importClusterForEndpointType

exports.exportPackagesFromSession = exportPackagesFromSession

exports.exportEndpoints = exportEndpoints
exports.importEndpoint = importEndpoint

exports.exportAttributesFromEndpointTypeCluster =
  exportAttributesFromEndpointTypeCluster
exports.importAttributeForEndpointType = importAttributeForEndpointType

exports.exportCommandsFromEndpointTypeCluster =
  exportCommandsFromEndpointTypeCluster
exports.importCommandForEndpointType = importCommandForEndpointType

exports.exportEventsFromEndpointTypeCluster =
  exportEventsFromEndpointTypeCluster
exports.importEventForEndpointType = importEventForEndpointType
