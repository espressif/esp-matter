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
 * This module provides the APIs for initializing timers specifically
 * for sessions.
 * 
 */
 import * as env from '../util/env'
 import * as SessionType from '../types/db-mapping-types'
 import * as SessionTimersTypes from '../types/session-timers-types'

 let ASYNC_DEFAULT_TIMER_INTERVAL_MS = 6000
 
 let sessionTimerIdArrayMap = {}
 
/**
 * Start session specific validation.
 *
 */
function initSessionTimers(db, session, sessionTimersSetupArray) {
    if (!sessionTimerIdArrayMap[session.sessionId]) {
        sessionTimerIdArrayMap[session.sessionId] = []
    }
    sessionTimersSetupArray.forEach((setup) => {
        sessionTimerIdArrayMap[session.sessionId]
            .push(setInterval(setup.func,
                              setup.timerInterval || ASYNC_DEFAULT_TIMER_INTERVAL_MS))
    })
}

/**
 * Deinitalize all validation timers associated with a specific session. 
 * @param db 
 * @param session 
 */
function deinitSessionTimers(session) {
    let asyncIdArray = sessionTimerIdArrayMap[session.sessionId]
    Object.keys(asyncIdArray).forEach(timer => clearInterval(asyncIdArray[timer]))
}

exports.initSessionTimers = initSessionTimers
exports.deinitSessionTimers = deinitSessionTimers