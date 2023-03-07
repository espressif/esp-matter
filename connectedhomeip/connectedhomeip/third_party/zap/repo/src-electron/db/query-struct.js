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

async function selectAllStructs(db, packageId) {
  let rows = await dbApi.dbAll(
    db,
    `
  SELECT
    STRUCT.STRUCT_ID,
    DATA_TYPE.NAME,
    DATA_TYPE.DISCRIMINATOR_REF
  FROM
    STRUCT
  INNER JOIN
    DATA_TYPE
  ON
    STRUCT.STRUCT_ID = DATA_TYPE.DATA_TYPE_ID
  WHERE
    PACKAGE_REF = ?
  ORDER BY
    NAME`,
    [packageId]
  )
  return rows.map(dbMapping.map.struct)
}

async function selectStructById(db, id) {
  return dbApi
    .dbGet(
      db,
      `
SELECT 
  STRUCT.STRUCT_ID,
  STRUCT.IS_FABRIC_SCOPED,
  DATA_TYPE.NAME,
  DATA_TYPE.DISCRIMINATOR_REF
FROM
  STRUCT
INNER JOIN
    DATA_TYPE
ON
  STRUCT.STRUCT_ID = DATA_TYPE.DATA_TYPE_ID
WHERE
  STRUCT_ID = ?`,
      [id]
    )
    .then(dbMapping.map.struct)
}

async function selectStructByName(db, name, packageIds) {
  return dbApi
    .dbGet(
      db,
      `
SELECT
  STRUCT.STRUCT_ID,
  STRUCT.IS_FABRIC_SCOPED,
  DATA_TYPE.NAME,
  DATA_TYPE.DISCRIMINATOR_REF
FROM
  STRUCT
INNER JOIN
  DATA_TYPE ON STRUCT.STRUCT_ID = DATA_TYPE.DATA_TYPE_ID
WHERE
  NAME = ?
  AND PACKAGE_REF IN (${dbApi.toInClause(packageIds)})
ORDER BY
  NAME`,
      [name]
    )
    .then(dbMapping.map.struct)
}

exports.selectStructById = selectStructById
exports.selectAllStructs = selectAllStructs
exports.selectStructByName = selectStructByName
