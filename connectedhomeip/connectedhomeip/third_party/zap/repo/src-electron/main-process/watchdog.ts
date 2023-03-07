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

let watchDogId: NodeJS.Timeout

/**
 * Starts a zap watchdog.
 *
 * @param {*} expirationInterval
 * @param {*} triggerFunction
 */
function start(expirationInterval: number, triggerFunction: () => void) {
  watchDogId = setTimeout(triggerFunction, expirationInterval)
  watchDogId.unref()
}

/**
 * Resets a zap watchdog.
 */
function reset() {
  if (watchDogId != null) watchDogId.refresh()
}

exports.start = start
exports.reset = reset
