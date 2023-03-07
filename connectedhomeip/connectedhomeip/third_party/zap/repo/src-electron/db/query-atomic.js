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
 * This module provides queries for atomic type queries.
 *
 * @module DB API: zcl database access
 */
const dbApi = require('./db-api.js')
const dbMapping = require('./db-mapping.js')

const ATOMIC_QUERY = `
SELECT
  ATOMIC_ID,
  ATOMIC_IDENTIFIER,
  NAME,
  DESCRIPTION,
  ATOMIC_SIZE,
  IS_DISCRETE,
  IS_STRING,
  IS_LONG,
  IS_CHAR,
  IS_SIGNED
FROM ATOMIC
`

const cacheKey = 'atomic'

// Raw query versions without caching.

async function selectAllAtomics(db, packageId) {
  let rows = await dbApi.dbAll(
    db,
    `${ATOMIC_QUERY} WHERE PACKAGE_REF = ? ORDER BY ATOMIC_IDENTIFIER`,
    [packageId]
  )
  return rows.map(dbMapping.map.atomic)
}

/**
 * Locates atomic type based on a type name. Query is not case sensitive.
 *
 * @param {*} db
 * @param {*} packageId
 * @param {*} typeName
 */
async function selectAtomicType(db, packageIds, name) {
  return dbApi
    .dbGet(
      db,
      `${ATOMIC_QUERY} WHERE PACKAGE_REF IN (${dbApi.toInClause(
        packageIds
      )}) AND UPPER(NAME) = ?`,
      [name == null ? name : name.toUpperCase()]
    )
    .then(dbMapping.map.atomic)
}

/**
 * Retrieves atomic type by a given Id.
 * @param {*} db
 * @param {*} packageId
 */
async function selectAtomicById(db, id) {
  return dbApi
    .dbGet(db, `${ATOMIC_QUERY} WHERE ATOMIC_ID = ?`, [id])
    .then((rows) => rows.map(dbMapping.map.atomic))
}

exports.selectAllAtomics = selectAllAtomics
exports.selectAtomicType = selectAtomicType
exports.selectAtomicById = selectAtomicById
