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
 * This module provides queries for discriminators
 */

const dbApi = require('./db-api')
const dbMapping = require('./db-mapping')

/**
 *
 * @param {*} db
 * @param {*} packageIds
 * @returns all the data type discriminator information
 */
async function selectAllDiscriminators(db, packageIds) {
  return dbApi
    .dbAll(
      db,
      `
    SELECT
      DISCRIMINATOR_ID,
      NAME
    FROM
      DISCRIMINATOR
    WHERE
      PACKAGE_REF IN (${dbApi.toInClause(packageIds)})
      `
    )
    .then((rows) => rows.map(dbMapping.map.discriminator))
}

exports.selectAllDiscriminators = selectAllDiscriminators
