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

/*
 * This file provides the functionality that reads the ZAP data from a database
 * and exports it into a file.
 */
const os = require('os')
const fs = require('fs')
const fsp = fs.promises
const path = require('path')
const env = require('../util/env')
const querySession = require('../db/query-session.js')
const queryImpExp = require('../db/query-impexp.js')
const dbEnum = require('../../src-shared/db-enum.js')

async function exportEndpointType(db, endpointType) {
  let data = await queryImpExp.exportClustersFromEndpointType(
    db,
    endpointType.endpointTypeId
  )

  endpointType.clusters = data

  let ps = data.map(async (endpointCluster) => {
    let endpointClusterId = endpointCluster.endpointClusterId
    delete endpointCluster.endpointClusterId
    endpointCluster.commands =
      await queryImpExp.exportCommandsFromEndpointTypeCluster(
        db,
        endpointType.endpointTypeId,
        endpointClusterId
      )
    if (endpointCluster.commands.length == 0) {
      delete endpointCluster.commands
    }
    endpointCluster.attributes =
      await queryImpExp.exportAttributesFromEndpointTypeCluster(
        db,
        endpointType.endpointTypeId,
        endpointClusterId
      )
    if (endpointCluster.attributes.length == 0) {
      delete endpointCluster.attributes
    }
    endpointCluster.events =
      await queryImpExp.exportEventsFromEndpointTypeCluster(
        db,
        endpointType.endpointTypeId,
        endpointClusterId
      )
    if (endpointCluster.events.length == 0) {
      delete endpointCluster.events
    }
  })
  return Promise.all(ps)
}

/**
 * Resolves to an array of endpoint types.
 *
 * @export
 * @param {*} db
 * @param {*} sessionId
 * @returns Promise to retrieve all endpoint types.
 */
async function exportEndpointTypes(db, sessionId) {
  let endpointTypes = await queryImpExp.exportEndpointTypes(db, sessionId)

  let promises = endpointTypes.map((endpointType) =>
    exportEndpointType(db, endpointType)
  )

  await Promise.all(promises)

  let endpoints = await queryImpExp.exportEndpoints(
    db,
    sessionId,
    endpointTypes
  )

  endpointTypes.forEach((ept) => {
    delete ept.endpointTypeId
  })
  endpoints.forEach((ep) => {
    delete ep.endpointTypeRef
  })
  return { endpointTypes: endpointTypes, endpoints: endpoints }
}

/**
 * Resolves with data for packages.
 *
 * @param {*} db
 * @param {*} sessionId
 */
async function exportSessionPackages(db, sessionId, zapProjectFileLocation) {
  let packages = await queryImpExp.exportPackagesFromSession(db, sessionId)

  return packages.map((p) => {
    let pathRelativity = dbEnum.pathRelativity.relativeToUserHome
    let relativePath = path.relative(os.homedir(), p.path)
    if (zapProjectFileLocation != null) {
      let rel = path.relative(path.dirname(zapProjectFileLocation), p.path)
      if (rel.length > 0) {
        relativePath = rel
        pathRelativity = dbEnum.pathRelativity.relativeToZap
      }
    }
    let ret = {
      pathRelativity: pathRelativity,
      path: relativePath,
      type: p.type,
    }
    if (p.category != null) {
      ret.category = p.category
    }
    if (p.version != null) {
      ret.version = p.version
    }
    if (p.description != null) {
      ret.description = p.description
    }
    return ret
  })
}

/**
 * Toplevel file that takes a given session ID and exports the data into the file
 *
 * @export
 * @param {*} db
 * @param {*} sessionId
 * @param {*} filePath
 * @returns A promise that resolves with the path of the file written.
 */
async function exportDataIntoFile(
  db,
  sessionId,
  filePath,
  options = {
    removeLog: false,
    createBackup: false,
  }
) {
  env.logDebug(`Writing state from session ${sessionId} into file ${filePath}`)
  let state = await createStateFromDatabase(db, sessionId)
  if (options.removeLog) delete state.log

  if (fs.existsSync(filePath)) {
    fs.copyFileSync(filePath, filePath + '~')
  }

  await fsp.writeFile(filePath, JSON.stringify(state, null, 2))
  await querySession.setSessionClean(db, sessionId)
  return filePath
}

async function getSessionKeyValues(db, sessionId, excludedKeys) {
  let keyValues = await querySession.getAllSessionKeyValues(db, sessionId)

  env.logDebug(`Retrieved session keys: ${keyValues.length}`)
  let zapFilePath = null
  let storedKeyValuePairs = keyValues.filter(
    (datum) => !excludedKeys.includes(datum.key)
  )
  let x = keyValues.filter((datum) => datum.key == dbEnum.sessionKey.filePath)
  if (x.length > 0) zapFilePath = x[0].value

  let exportedKeyValues = {
    key: 'keyValuePairs',
    data: storedKeyValuePairs,
    zapFilePath: zapFilePath,
  }

  let d = await exportSessionPackages(
    db,
    sessionId,
    exportedKeyValues.zapFilePath
  )

  return [exportedKeyValues, { key: 'package', data: d }]
}

/**
 * Given a database and a session id, this method returns a promise that
 * resolves with a state object that needs to be saved into a file.
 *
 * @export
 * @param {*} db
 * @param {*} sessionId
 * @returns state object that needs to be saved into a file.
 */
async function createStateFromDatabase(db, sessionId) {
  let state = {
    featureLevel: env.zapVersion().featureLevel,
    creator: 'zap',
  }
  let promises = []
  let excludedKeys = [dbEnum.sessionKey.filePath]

  env.logInfo(`Exporting data for session: ${sessionId}`)

  promises.push(getSessionKeyValues(db, sessionId, excludedKeys))

  let allEndpointTypes = await exportEndpointTypes(db, sessionId)

  let parseEndpointTypes = Promise.resolve({
    key: 'endpointTypes',
    data: allEndpointTypes.endpointTypes,
  })

  let parseEndpoints = Promise.resolve({
    key: 'endpoints',
    data: allEndpointTypes.endpoints,
  })

  let appendLogPromise = querySession.readLog(db, sessionId).then((log) => {
    return { key: 'log', data: log }
  })

  promises.push(parseEndpointTypes)
  promises.push(parseEndpoints)
  promises.push(appendLogPromise)

  let data = await Promise.all(promises)
  data.flat().forEach((keyDataPair) => {
    state[keyDataPair.key] = keyDataPair.data
  })
  return state
}
// exports
exports.exportDataIntoFile = exportDataIntoFile
exports.createStateFromDatabase = createStateFromDatabase
