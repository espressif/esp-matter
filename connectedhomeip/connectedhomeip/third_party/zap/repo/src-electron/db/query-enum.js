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
 * This module provides queries for enums.
 *
 * @module DB API: zcl database access
 */
const dbApi = require('./db-api')
const dbMapping = require('./db-mapping')

/**
 * Retrieves all the enums in the database.
 *
 * @export
 * @param {*} db
 * @param {*} packageId
 * @returns Promise that resolves with the rows of enums.
 */
async function selectAllEnums(db, packageId) {
  return dbApi
    .dbAll(
      db,
      `
SELECT
  ENUM.ENUM_ID,
  DATA_TYPE.NAME,
  DATA_TYPE.DISCRIMINATOR_REF,
  ENUM.SIZE
FROM
  ENUM
INNER JOIN DATA_TYPE ON
  ENUM.ENUM_ID = DATA_TYPE.DATA_TYPE_ID
WHERE
  DATA_TYPE.PACKAGE_REF = ?
ORDER BY DATA_TYPE.NAME`,
      [packageId]
    )
    .then((rows) => rows.map(dbMapping.map.enum))
}

/**
 * Retrieves all the enums with cluster references in the database.
 *
 * @export
 * @param {*} db
 * @param {*} packageId
 * @param {*} clusterId
 * @returns Promise that resolves with the rows of enums.
 */
async function selectClusterEnums(db, packageIds, clusterId) {
  return dbApi
    .dbAll(
      db,
      `
SELECT
  E.ENUM_ID,
  DT.NAME,
  E.SIZE
FROM
  ENUM AS E
INNER JOIN
  DATA_TYPE AS DT
ON
  DT.DATA_TYPE_ID = E.ENUM_ID
INNER JOIN
  DATA_TYPE_CLUSTER
ON
  DT.DATA_TYPE_ID = DATA_TYPE_CLUSTER.DATA_TYPE_REF
WHERE
  DT.PACKAGE_REF IN (${dbApi.toInClause(packageIds)})
  AND DATA_TYPE_CLUSTER.CLUSTER_REF = ?
ORDER BY DT.NAME`,
      [clusterId]
    )
    .then((rows) => rows.map(dbMapping.map.enum))
}

/**
 * Returns an enum by ID.
 * @param {*} db
 * @param {*} id
 * @returns enum
 */
async function selectAllEnumItemsById(db, id) {
  return dbApi
    .dbAll(
      db,
      `
SELECT
  NAME,
  VALUE
FROM
  ENUM_ITEM
WHERE
  ENUM_REF = ?
ORDER BY FIELD_IDENTIFIER`,
      [id]
    )
    .then((rows) => rows.map(dbMapping.map.enumItem))
}

/**
 * Select all enum items in a package.
 *
 * @param {*} db
 * @param {*} packageId
 * @returns list of enum items
 */
async function selectAllEnumItems(db, packageId) {
  return dbApi
    .dbAll(
      db,
      `
SELECT
  EI.NAME,
  EI.VALUE,
  EI.ENUM_REF
FROM
  ENUM_ITEM AS EI
INNER JOIN
  ENUM AS E
ON
  E.ENUM_ID = EI.ENUM_REF
INNER JOIN
  DATA_TYPE AS DT
ON
  DT.DATA_TYPE_ID = E.ENUM_ID
WHERE
  DT.PACKAGE_REF = ?
ORDER BY EI.ENUM_REF, EI.FIELD_IDENTIFIER`,
      [packageId]
    )
    .then((rows) => rows.map(dbMapping.map.enumItem))
}

/**
 * Select an enum matched by its primary key.
 * @param {*} db
 * @param {*} id
 * @returns an enum or underfined if not found
 */
async function selectEnumById(db, id) {
  return dbApi
    .dbGet(
      db,
      `
SELECT
  ENUM.ENUM_ID,
  DATA_TYPE.NAME,
  ENUM.SIZE
FROM
  ENUM
INNER JOIN
  DATA_TYPE
ON
  ENUM.ENUM_ID = DATA_TYPE.DATA_TYPE_ID
WHERE
  ENUM_ID = ?`,
      [id]
    )
    .then(dbMapping.map.enum)
}

/**
 * Select an enum matched by name.
 *
 * @param {*} db
 * @param {*} name
 * @param {*} packageIds
 * @returns enum or undefined
 */
async function selectEnumByName(db, name, packageIds) {
  return dbApi
    .dbGet(
      db,
      `
SELECT
  ENUM.ENUM_ID,
  DATA_TYPE.NAME AS NAME,
  ENUM.SIZE AS SIZE 
FROM
  ENUM
INNER JOIN
  DATA_TYPE
ON
  ENUM.ENUM_ID = DATA_TYPE.DATA_TYPE_ID
WHERE
  (DATA_TYPE.NAME = ? OR DATA_TYPE.NAME = ?)AND PACKAGE_REF IN (${dbApi.toInClause(
    packageIds
  )})
ORDER BY NAME`,
      [name, name.toLowerCase()]
    )
    .then(dbMapping.map.enum)
}

// exports
exports.selectAllEnums = selectAllEnums
exports.selectEnumByName = selectEnumByName

exports.selectEnumById = selectEnumById
exports.selectClusterEnums = selectClusterEnums
exports.selectAllEnumItemsById = selectAllEnumItemsById
exports.selectAllEnumItems = selectAllEnumItems
