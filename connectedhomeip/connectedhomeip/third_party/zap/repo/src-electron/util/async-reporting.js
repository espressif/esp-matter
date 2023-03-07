/**
 *
 *    Copyright (c) 2022 Silicon Labs
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
 * This module provides the mechanism for dealing with the async reporting
 * from backend to the UI.
 *
 * This mechanism takes care of:
 *   - dirty flag
 *
 * @module JS API: async reporting
 */

const env = require('./env')
const wsServer = require('../server/ws-server')
const querySession = require('../db/query-session')
const dbEnum = require('../../src-shared/db-enum')

// This object contains all the async reports.
let asyncReports = {
  dirtyFlag: {
    fn: sendDirtyFlagStatus,
    intervalMs: 1000,
    sessionBased: true,
  },
}

/**
 * Sends a dirty flag status for a single session.
 * @param {*} db
 * @param {*} session
 */
async function sendDirtyFlagStatus(db, session) {
  let socket = wsServer.clientSocket(session.sessionKey)
  if (socket) {
    try {
      let flag = await querySession.getSessionDirtyFlag(db, session.sessionId)
      if (flag != undefined) {
        wsServer.sendWebSocketMessage(socket, {
          category: dbEnum.wsCategory.dirtyFlag,
          payload: flag,
        })
      }
    } catch (err) {
      // If we close the database before this is executed from a timer, then
      // we don't log that.
      let msg = err.toString()
      if (!msg.includes('SQLITE_MISUSE')) {
        env.logWarning(
          `Error reading dirty flag status: ${session.sessionKey} => ${err}`
        )
      }
    }
  }
}

/**
 * Start the interval that will check and report dirty flags.
 * @param {*} db
 * @param {*} intervalMs
 */
function startAsyncReporting(db) {
  for (let key of Object.keys(asyncReports)) {
    let report = asyncReports[key]
    if (report.sessionBased) {
      // Session based reports get iterated over all sessions
      // and called with appropriate session.
      report.id = setInterval(async () => {
        let sessions = await querySession.getAllSessions(db)
        let allPromises = sessions.map((session) => report.fn(db, session))
        return Promise.all(allPromises)
      }, report.intervalMs)
    } else {
      // Non session based reports get called once with the db as the argument.
      report.id = setInterval(() => {
        report.fn(db)
      }, report.intervalMs)
    }
  }
}

/**
 * Stop the interval that will check and report dirty flags
 */
function stopAsyncReporting() {
  for (let key of Object.keys(asyncReports)) {
    let report = asyncReports[key]
    if (report.id) clearInterval(report.id)
  }
}

exports.startAsyncReporting = startAsyncReporting
exports.stopAsyncReporting = stopAsyncReporting
