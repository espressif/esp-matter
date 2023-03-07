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
 * This module provides cache for commonly used static database queries.
 *
 * @module DB API: zcl database access
 */
const dbApi = require('./db-api.js')
const dbMapping = require('./db-mapping.js')

const cacheEnabled = true
let cache = {}

/**
 * Clears the entire cache.
 */
function clear() {
  cache = {}
}

/**
 * Puts a data object into the cache under a given key/packageId
 * @param {*} key
 * @param {*} packageId
 * @param {*} data
 */
function put(key, packageId, data) {
  const keyCache = {}
  keyCache[packageId] = data
  cache[key] = keyCache
}

/**
 * Returns a data object under a given key/packageId.
 *
 * @param {*} key
 * @param {*} packageId
 * @returns cached object or null if none is present
 */
function get(key, packageId) {
  const keyCache = cache[key]
  if (keyCache != null) {
    return keyCache[packageId]
  } else {
    return null
  }
}

/**
 * Returns true if a given key/packageId cache exists.
 *
 * @param {*} key
 * @param {*} packageId
 * @returns true or false, depending on whether the cache is present.
 */
function isCached(key, packageId) {
  const keyCache = cache[key]
  return keyCache != null && keyCache[packageId] != null
}

exports.clear = clear
exports.put = put
exports.get = get
exports.isCached = isCached

exports.cacheEnabled = cacheEnabled
