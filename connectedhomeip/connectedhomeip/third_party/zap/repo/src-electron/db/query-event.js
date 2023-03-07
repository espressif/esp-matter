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
 * This module provides queries related to events.
 *
 * @module DB API: event queries.
 */
const dbApi = require('./db-api.js')
const dbMapping = require('./db-mapping.js')

/**
 * Retrieves events for a given cluster Id.
 *
 * @param {*} db
 * @param {*} clusterId
 * @returns promise of an array of event rows, which represent per-cluster events.
 */
async function selectEventsByClusterId(db, clusterId) {
  return dbApi
    .dbAll(
      db,
      `
SELECT
  EVENT_ID,
  CLUSTER_REF,
  CODE,
  MANUFACTURER_CODE,
  NAME,
  DESCRIPTION,
  SIDE,
  IS_OPTIONAL,
  IS_FABRIC_SENSITIVE,
  PRIORITY
FROM
  EVENT
WHERE
  CLUSTER_REF = ?
ORDER BY
  CODE`,
      [clusterId]
    )
    .then((rows) => rows.map(dbMapping.map.event))
}

/**
 * Retrieves all events under a given package
 *
 * @param {*} db
 * @param {*} packageIds
 * @returns promise of an array of events
 */
async function selectAllEvents(db, packageIds) {
  return dbApi
    .dbAll(
      db,
      `
SELECT
  E.EVENT_ID,
  E.CLUSTER_REF,
  E.CODE,
  C.CODE AS CLUSTER_CODE,
  E.MANUFACTURER_CODE,
  E.NAME,
  E.DESCRIPTION,
  E.SIDE,
  E.IS_OPTIONAL,
  E.IS_FABRIC_SENSITIVE,
  E.PRIORITY
FROM
  EVENT AS E
INNER JOIN
  CLUSTER AS C
ON
  E.CLUSTER_REF = C.CLUSTER_ID
WHERE
  E.PACKAGE_REF in (${dbApi.toInClause(packageIds)})
ORDER BY
  C.CODE, E.CODE`,
      []
    )
    .then((rows) => rows.map(dbMapping.map.event))
}

async function selectAllEventFields(db, packageIds) {
  return dbApi
    .dbAll(
      db,
      `
SELECT
  EF.FIELD_IDENTIFIER,
  EF.NAME,
  EF.TYPE,
  EF.IS_ARRAY,
  EF.IS_NULLABLE,
  EF.IS_OPTIONAL
FROM
  EVENT_FIELD AS EF
INNER JOIN
  EVENT
ON
  EVENT_FIELD.EVENT_REF = EVENT.EVENT_ID
WHERE
  EVENT.PACKAGE_REF IN (${dbApi.toInClause(packageIds)})
ORDER BY
  EF.FIELD_IDENTIFIER
`
    )
    .then((rows) => rows.map(dbMapping.map.eventField))
}

async function selectEventFieldsByEventId(db, eventId) {
  return dbApi
    .dbAll(
      db,
      `
SELECT
  FIELD_IDENTIFIER,
  NAME,
  TYPE,
  IS_ARRAY,
  IS_NULLABLE,
  IS_OPTIONAL
FROM
  EVENT_FIELD
WHERE
  EVENT_REF = ?
ORDER BY
  FIELD_IDENTIFIER
  `,
      [eventId]
    )
    .then((rows) => rows.map(dbMapping.map.eventField))
}

exports.selectEventsByClusterId = selectEventsByClusterId
exports.selectAllEvents = selectAllEvents
exports.selectAllEventFields = selectAllEventFields
exports.selectEventFieldsByEventId = selectEventFieldsByEventId
