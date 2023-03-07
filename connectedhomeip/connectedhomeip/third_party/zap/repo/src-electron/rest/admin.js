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

const dbApi = require('../db/db-api.js')
const restApi = require('../../src-shared/rest-api.js')
const env = require('../util/env')
const { StatusCodes } = require('http-status-codes')

/**
 * This module provides the REST API to the admin functions.
 *
 * @module REST API: admin functions
 */

/**
 * API: /sql
 * Request JSON:
 * <pre>
 *   {
 *     sql: SQL Query
 *   }
 * </pre>
 *
 * Response JSON:
 * <pre>
 *   {
 *     result: Array of rows.
 *   }
 * </pre>
 *
 * @export
 * @param {*} db
 * @param {*} app
 * @returns callback for the express uri registration
 */
function httpPostSql(db) {
  return async (request, response) => {
    let sql = request.body.sql
    if (sql) {
      try {
        let rows = await dbApi.dbAll(db, sql, [])
        response.status(StatusCodes.OK).json({ result: rows })
      } catch (err) {
        response.status(StatusCodes.OK).json({ error: err })
      }
    }
  }
}

/**
 * API: /version
 * Response JSON:
 * <pre>
 *   {
 *     version: full version.
 *     featureLevel: feature level.
 *     hash: git hash code
 *     timestamp: Unix time from the last commit in the repo.
 *     date: Date of the last commit in the repo.
 *   }
 * </pre>
 *
 * @param {*} db
 * @returns callback for the express uri registration.
 */
function httpGetVersion(db) {
  return (request, response) => {
    let version = env.zapVersion()
    response.status(StatusCodes.OK).json(version)
  }
}

exports.post = [
  {
    uri: restApi.uri.sql,
    callback: httpPostSql,
  },
]

exports.get = [
  {
    uri: restApi.uri.version,
    callback: httpGetVersion,
  },
]
