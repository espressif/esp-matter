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
 * This module provides queries for strings
 */

const dbApi = require('./db-api')
const dbMapping = require('./db-mapping')

/**
 * Select all Strings.
 *
 * @param db
 * @param packageId
 * @returns All Strings
 */
async function selectAllStrings(db, packageId) {
  return dbApi
    .dbAll(
      db,
      `
   SELECT
     STRING.STRING_ID,
     STRING.IS_LONG,
     STRING.IS_CHAR,
     STRING.SIZE AS SIZE,
     DATA_TYPE.NAME AS NAME
   FROM STRING
   INNER JOIN DATA_TYPE 
    ON STRING.STRING_ID = DATA_TYPE.DATA_TYPE_ID
   WHERE PACKAGE_REF = ?`,
      [packageId]
    )
    .then((rows) => rows.map(dbMapping.map.string))
}

/**
 * Select String by ID.
 *
 * @param db
 * @param packageId
 * @returns String
 */
async function selectStringById(db, id) {
  return dbApi
    .dbGet(
      db,
      `
   SELECT
     STRING.STRING_ID,
     STRING.IS_LONG,
     STRING.IS_CHAR,
     STRING.SIZE AS SIZE,
     DATA_TYPE.NAME AS NAME
   FROM STRING
   INNER JOIN DATA_TYPE 
    ON STRING.STRING_ID = DATA_TYPE.DATA_TYPE_ID
   WHERE STRING.STRING_ID = ?`,
      [id]
    )
    .then(dbMapping.map.string)
}

/**
 * Select String by name.
 *
 * @param db
 * @param name
 * @param packageIds
 * @returns String
 */
async function selectStringByName(db, name, packageIds) {
  return dbApi
    .dbGet(
      db,
      `
   SELECT
     STRING.STRING_ID,
     STRING.IS_LONG,
     STRING.IS_CHAR,
     STRING.SIZE AS SIZE,
     DATA_TYPE.NAME AS NAME
   FROM
     STRING
   INNER JOIN
     DATA_TYPE
   ON
     STRING.STRING_ID = DATA_TYPE.DATA_TYPE_ID
   WHERE
     DATA_TYPE.NAME = ?
     AND DATA_TYPE.PACKAGE_REF IN (${dbApi.toInClause(packageIds)})`,
      [name]
    )
    .then(dbMapping.map.string)
}

exports.selectAllStrings = selectAllStrings
exports.selectStringById = selectStringById
exports.selectStringByName = selectStringByName
