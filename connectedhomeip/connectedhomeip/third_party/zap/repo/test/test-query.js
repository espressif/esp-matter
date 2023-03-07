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
 * This module provides queries for user configuration.
 *
 * @module DB API: user configuration queries against the database.
 */
const dbApi = require('../src-electron/db/db-api')
const dbMapping = require('../src-electron/db/db-mapping')
const querySession = require('../src-electron/db/query-session')
const util = require('../src-electron/util/util')

/**
 * Resolves into all the cluster states.
 *
 * @export
 * @param {*} db
 * @param {*} endpointTypeId
 * @returns Promise that resolves with cluster states.
 */
async function getAllEndpointTypeClusterState(db, endpointTypeId) {
  let rows = await dbApi.dbAll(
    db,
    `
SELECT
  CLUSTER.NAME,
  CLUSTER.CODE,
  CLUSTER.MANUFACTURER_CODE,
  ENDPOINT_TYPE_CLUSTER.ENDPOINT_TYPE_CLUSTER_ID,
  ENDPOINT_TYPE_CLUSTER.SIDE,
  ENDPOINT_TYPE_CLUSTER.ENABLED
FROM
  ENDPOINT_TYPE_CLUSTER
INNER JOIN CLUSTER
ON ENDPOINT_TYPE_CLUSTER.CLUSTER_REF = CLUSTER.CLUSTER_ID
WHERE ENDPOINT_TYPE_CLUSTER.ENDPOINT_TYPE_REF = ?`,
    [endpointTypeId]
  )
  if (rows == null) return []

  let result = rows.map((row) => {
    let obj = {
      endpointTypeClusterId: row.ENDPOINT_TYPE_CLUSTER_ID,
      clusterName: row.NAME,
      clusterCode: row.CODE,
      side: row.SIDE,
      enabled: row.STATE == '1',
    }
    if (row.MANUFACTURER_CODE != null)
      obj.manufacturerCode = row.MANUFACTURER_CODE
    return obj
  })
  return result
}

/**
 * Simple query that returns number of rows in a given table.
 *
 * @export
 * @param {*} db
 * @param {*} table
 * @returns a promise that resolves into the count of the rows in the table.
 */
async function selectCountFrom(db, table) {
  return dbApi
    .dbGet(db, `SELECT COUNT(1) AS COUNT FROM ${table}`)
    .then((x) => x.COUNT)
}

/**
 * Extracts commands from the endpoint_type_command table.
 *
 * @export
 * @param {*} db
 * @param {*} endpointTypeId
 * @returns A promise that resolves into the rows.
 */
async function getEndpointTypeCommands(db, endpointTypeId) {
  return dbApi
    .dbAll(
      db,
      'SELECT COMMAND_REF, INCOMING, OUTGOING FROM ENDPOINT_TYPE_COMMAND WHERE ENDPOINT_TYPE_REF = ?',
      [endpointTypeId]
    )
    .then((rows) =>
      rows.map((row) => {
        return {
          commandID: row.COMMAND_REF,
          isIncoming: row.INCOMING,
          isOutgoing: row.OUTGOING,
        }
      })
    )
}

/**
 * Extracts attributes from the endpoint_type_attribute table, no joins.
 *
 * @export
 * @param {*} db
 * @param {*} endpointTypeId
 * @returns A promise that resolves into the rows.
 */
async function getEndpointTypeAttributes(db, endpointTypeId) {
  return dbApi
    .dbAll(
      db,
      `
SELECT
  ETA.ATTRIBUTE_REF,
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
  ENDPOINT_TYPE_ATTRIBUTE AS ETA
WHERE
  ETA.ENDPOINT_TYPE_REF = ?`,
      [endpointTypeId]
    )
    .then((rows) => rows.map(dbMapping.map.endpointTypeAttribute))
}

/**
 * Creates a user session and initializes it with the given packages.
 *
 * @param {*} db
 * @param {*} user
 * @param {*} sessionUuid
 * @param {*} zclFile
 * @param {*} genTemplatesFile
 * @returns
 */
async function createSession(db, user, sessionUuid, zclFile, genTemplatesFile) {
  let userSession = await querySession.ensureZapUserAndSession(
    db,
    user,
    sessionUuid
  )
  await util.initializeSessionPackage(db, userSession.sessionId, {
    zcl: zclFile,
    template: genTemplatesFile,
  })
  return userSession.sessionId
}

exports.getAllEndpointTypeClusterState = getAllEndpointTypeClusterState
exports.selectCountFrom = selectCountFrom
exports.getEndpointTypeAttributes = getEndpointTypeAttributes
exports.getEndpointTypeCommands = getEndpointTypeCommands
exports.createSession = createSession
