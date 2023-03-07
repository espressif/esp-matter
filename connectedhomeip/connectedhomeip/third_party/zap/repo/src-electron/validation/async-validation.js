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
 * This module provides the APIs for validating inputs to the database, and returning flags indicating if
 * things were successful or not.
 *
 */

import * as env from '../util/env'
import * as SessionType from '../types/db-mapping-types'
import * as SessionTimersTypes from '../types/session-timers-types'
const queryPackage = require('../db/query-package.js')
const sessionTimers = require('../main-process/session-timers.js')
const wsServer = require('../server/ws-server.js')
const dbEnum = require('../../src-shared/db-enum.js')

let ASYNC_ENFORCE_COMMON_CLUSTER_SPEC_INTERVAL_MS = 3000

const TimerTypeFunctionMap = {
  zigbeeEnforceCommonClusterSpec: zigbeeEnforceCommonClusterSpecInit
}

 /**
 * Start session specific validation.
 *
 */
export async function initAsyncValidation(db, session) {
  let validationTimers = await queryPackage.getSessionPackages(db, session.sessionId)
                  .then((packages) => {
                    let p = packages.map((pkg) =>
                          queryPackage.selectAllOptionsValues(db, pkg.packageRef, dbEnum.packageOptionCategory.validationTimersFlags))
                    return Promise.all(p).then((data) => data.flat(1))
                  })

  let timers = []
  let validationTimersFlag = {}
  validationTimers.forEach(data => {
    validationTimersFlag[data.optionCode] = true
  })

  if (validationTimersFlag.zigbeeEnforceCommonCluster) {
    timers.push(zigbeeEnforceCommonClusterSpecInit(session))
  }
  // init session timers
  sessionTimers.initSessionTimers(db, session, timers)
}

function zigbeeEnforceCommonClusterSpecInit(session) {
  let timerSetup = {
    func: function () {zigbeeEnforceCommonClusterSpecCallback(session)},
    timerInterval : ASYNC_ENFORCE_COMMON_CLUSTER_SPEC_INTERVAL_MS
  }
  return timerSetup
}

function zigbeeEnforceCommonClusterSpecCallback(session) {
  let socket = wsServer.clientSocket(session.sessionKey)
  if (socket) {
        wsServer.sendWebSocketMessage(socket, {
          category: dbEnum.wsCategory.validation,
          payload: "green",
        })
  }
}

exports.initAsyncValidation = initAsyncValidation
