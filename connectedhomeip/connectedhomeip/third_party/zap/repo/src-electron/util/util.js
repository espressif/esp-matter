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

/**
 * @module JS API: random utilities
 */

const os = require('os')
const fs = require('fs')
const fsp = fs.promises
const env = require('./env')
const crc = require('crc')
const path = require('path')
const childProcess = require('child_process')
const queryPackage = require('../db/query-package.js')
const queryEndpoint = require('../db/query-endpoint.js')
const queryEndpointType = require('../db/query-endpoint-type.js')
const queryConfig = require('../db/query-config.js')
const queryZcl = require('../db/query-zcl.js')
const queryCommand = require('../db/query-command.js')
const querySession = require('../db/query-session.js')
const dbEnum = require('../../src-shared/db-enum.js')
const { v4: uuidv4 } = require('uuid')
const xml2js = require('xml2js')
const singleInstance = require('single-instance')

/**
 * Returns the CRC of the data that is passed.
 * @param {*} data
 * @returns Calculated CRC of a data.
 */
function checksum(data) {
  return crc.crc32(data)
}

/**
 * This function assigns a proper package ID to the session.
 *
 * @param {*} db
 * @param {*} sessionId
 * @param {*} options: object containing 'zcl' and 'template'
 * @returns Promise that resolves with the packages array.
 */
async function initializeSessionPackage(db, sessionId, options) {
  let promises = []

  // This is the desired ZCL properties file. Because it is possible
  // that an array is passed from the command line, we are simply taking
  // the first one, if we pass multiple ones.
  let zclFile = options.zcl
  if (Array.isArray(zclFile)) zclFile = options.zcl[0]

  // 0. Read current packages.
  let currentPackages =
    await queryPackage.getPackageSessionPackagePairBySessionId(db, sessionId)
  let hasZclPackage = false
  let hasGenTemplate = false
  currentPackages.forEach((pair) => {
    if (pair.pkg.type == dbEnum.packageType.zclProperties) {
      hasZclPackage = true
    }
    if (pair.pkg.type == dbEnum.packageType.genTemplatesJson) {
      hasGenTemplate = true
    }
  })

  // 1. Associate a zclProperties file.
  if (!hasZclPackage) {
    let zclPropertiesPromise = queryPackage
      .getPackagesByType(db, dbEnum.packageType.zclProperties)
      .then((rows) => {
        let packageId
        if (rows.length == 1) {
          packageId = rows[0].id
          env.logDebug(
            `Single zcl.properties found, using it for the session: ${packageId}`
          )
        } else if (rows.length == 0) {
          env.logError(`No zcl.properties found for session.`)
          packageId = null
        } else {
          rows.forEach((p) => {
            if (path.resolve(zclFile) === p.path) {
              packageId = p.id
            }
          })
          env.logWarning(
            `${sessionId}, ${zclFile}: Multiple toplevel zcl.properties found. Using the first one from args: ${packageId}`
          )
        }
        if (packageId != null) {
          return queryPackage.insertSessionPackage(
            db,
            sessionId,
            packageId,
            true
          )
        }
      })
    promises.push(zclPropertiesPromise)
  }

  // 2. Associate a gen template file
  if (!hasGenTemplate) {
    let genTemplateJsonPromise = queryPackage
      .getPackagesByType(db, dbEnum.packageType.genTemplatesJson)
      .then((rows) => {
        let packageId
        if (rows.length == 1) {
          packageId = rows[0].id
          env.logDebug(
            `Single generation template metafile found, using it for the session: ${packageId}`
          )
        } else if (rows.length == 0) {
          env.logInfo(`No generation template metafile found for session.`)
          packageId = null
        } else {
          rows.forEach((p) => {
            if (
              options.template != null &&
              path.resolve(options.template) === p.path
            ) {
              packageId = p.id
            }
          })
          if (packageId != null) {
            env.logWarning(
              `Multiple toplevel generation template metafiles found. Using the one from args: ${packageId}`
            )
          } else {
            packageId = rows[0].id
            env.logWarning(
              `Multiple toplevel generation template metafiles found. Using the first one.`
            )
          }
        }
        if (packageId != null) {
          return queryPackage.insertSessionPackage(
            db,
            sessionId,
            packageId,
            true
          )
        }
      })
    promises.push(genTemplateJsonPromise)
  }

  if (promises.length > 0) await Promise.all(promises)

  // We have to set the default session key values.
  let packages = await queryPackage.insertSessionKeyValuesFromPackageDefaults(
    db,
    sessionId
  )
  await querySession.setSessionClean(db, sessionId)

  return packages
}

/**
 * Move database file out of the way into the backup location.
 *
 * @param {*} filePath
 */
function createBackupFile(filePath) {
  let pathBak = filePath + '~'
  if (fs.existsSync(filePath)) {
    if (fs.existsSync(pathBak)) {
      env.logDebug(`Deleting old backup file: ${pathBak}`)
      fs.unlinkSync(pathBak)
    }
    env.logDebug(`Creating backup file: ${filePath} to ${pathBak}`)
    fs.renameSync(filePath, pathBak)
  }
}

/**
 * Returns an object that contains:
 *    match: true or false if featureLevel is matched or not.
 *    message: in case of missmatch, the message shown to user.
 * @param {*} featureLevel
 */
function matchFeatureLevel(featureLevel, requirementSource = null) {
  if (featureLevel > env.zapVersion().featureLevel) {
    return {
      match: false,
      message: `${
        requirementSource == null ? 'File' : requirementSource
      } requires feature level ${featureLevel}, we only have ${
        env.zapVersion().featureLevel
      }. Please upgrade your zap!`,
    }
  } else {
    return { match: true }
  }
}

/**
 * Produces a text dump of a session data for human consumption.
 *
 * @param {*} db
 * @param {*} sessionId
 * @returns promise that resolves into a text report for the session.
 */
async function sessionReport(db, sessionId) {
  return queryEndpointType
    .selectAllEndpointTypes(db, sessionId)
    .then((epts) => {
      let ps = []
      epts.forEach((ept) => {
        ps.push(
          queryEndpoint.selectEndpointClusters(db, ept.id).then((clusters) => {
            let s = `Endpoint: ${ept.name} \n`
            let ps2 = []
            for (let c of clusters) {
              let rpt = `  - ${c.hexCode}: cluster: ${c.name} (${c.side})\n`
              ps2.push(
                queryEndpoint
                  .selectEndpointClusterAttributes(
                    db,
                    c.clusterId,
                    c.side,
                    ept.id
                  )
                  .then((attrs) => {
                    for (let at of attrs) {
                      rpt = rpt.concat(
                        `    - ${at.hexCode}: attribute: ${at.name} [${at.type}] [bound: ${at.isBound}]\n`
                      )
                    }
                  })
                  .then(() =>
                    queryEndpoint.selectEndpointClusterCommands(
                      db,
                      c.clusterId,
                      ept.id
                    )
                  )
                  .then((cmds) => {
                    for (let cmd of cmds) {
                      rpt = rpt.concat(
                        `    - ${cmd.hexCode}: command: ${cmd.name}\n`
                      )
                    }
                    return rpt
                  })
              )
            }
            return Promise.all(ps2)
              .then((rpts) => rpts.join(''))
              .then((r) => s.concat(r))
          })
        )
      })
      return Promise.all(ps).then((results) => results.join('\n'))
    })
}

/**
 * Produces a text dump of a session data for human consumption.
 *
 * @param {*} db
 * @param {*} sessionId
 * @returns promise that resolves into a text report for the session.
 */
async function sessionDump(db, sessionId) {
  let dump = {
    endpointTypes: [],
    attributes: [],
    commands: [],
    clusters: [],
    usedPackages: [],
    packageReport: '',
  }
  let endpoints = await queryEndpoint.selectAllEndpoints(db, sessionId)
  dump.endpoints = endpoints

  let epts = await queryEndpointType.selectAllEndpointTypes(db, sessionId)
  let ps = []

  epts.forEach((ept) => {
    ept.clusters = []
    ept.attributes = []
    ept.commands = []
    dump.endpointTypes.push(ept)
    ps.push(
      queryEndpoint.selectEndpointClusters(db, ept.id).then((clusters) => {
        let ps2 = []
        for (let c of clusters) {
          ept.clusters.push(c)
          dump.clusters.push(c)
          ps2.push(
            queryEndpoint
              .selectEndpointClusterAttributes(db, c.clusterId, c.side, ept.id)
              .then((attrs) => {
                c.attributes = attrs
                ept.attributes.push(...attrs)
                dump.attributes.push(...attrs)
              })
              .then(() =>
                queryEndpoint.selectEndpointClusterCommands(
                  db,
                  c.clusterId,
                  ept.id
                )
              )
              .then((cmds) => {
                c.commands = cmds
                ept.commands.push(...cmds)
                dump.commands.push(...cmds)
              })
          )
        }
        return Promise.all(ps2)
      })
    )
  })
  await Promise.all(ps)

  // Here we are testing that we have entities only from ONE
  // package present. There was a bug, where global attributes from
  // other packages got referenced under the session, because
  // some query wasn't taking packageId into consideration.
  for (const at of dump.attributes) {
    let attributeId = at.id
    let attribute = await queryZcl.selectAttributeById(db, attributeId)
    if (dump.usedPackages.indexOf(attribute.packageRef) == -1) {
      dump.usedPackages.push(attribute.packageRef)
    }
  }

  for (const cm of dump.commands) {
    let commandId = cm.id
    let cmd = await queryCommand.selectCommandById(db, commandId)
    if (dump.usedPackages.indexOf(cmd.packageRef) == -1) {
      dump.usedPackages.push(cmd.packageRef)
    }
  }

  for (const cl of dump.clusters) {
    let clusterId = cl.clusterId
    let cluster = await queryZcl.selectClusterById(db, clusterId)
    if (dump.usedPackages.indexOf(cluster.packageRef) == -1) {
      dump.usedPackages.push(cluster.packageRef)
    }
  }
  return dump
}

/**
 * If you have an array of arguments, and a function that creates
 * a promise out of each of those arguments, this function
 * executes them sequentially, one by one.
 *
 * @param {*} arrayOfData
 * @param {*} promiseCreator
 */
function executePromisesSequentially(arrayOfData, promiseCreator) {
  return arrayOfData.reduce((prev, nextData, currentIndex) => {
    return prev.then(() => promiseCreator(nextData, currentIndex))
  }, Promise.resolve())
}

/**
 * This function creates absolute path out of relative path and its relativity
 * @param {*} relativePath
 * @param {*} relativity
 * @param {*} zapFilePath
 */
function createAbsolutePath(relativePath, relativity, zapFilePath) {
  switch (relativity) {
    case dbEnum.pathRelativity.absolute:
      return relativePath
    case dbEnum.pathRelativity.relativeToUserHome:
      return path.join(os.homedir(), relativePath)
    case dbEnum.pathRelativity.relativeToZap:
      return path.join(path.dirname(zapFilePath), relativePath)
  }
  return relativePath
}

/**
 * This method takes an array of root locations and a relative path.
 * It will attempt to locate an absolute file at the path, combining
 * the root location and a relative path, until a file is found and returned.
 *
 * If none of the combined root locations and relative paths results
 * in an actual file, null is returned.
 *
 * @param {*} rootFileLocations Array of root file locations, typically directories
 * @param {*} relativeFilePath Relative path
 * @returns A fully resolved path that exists, or null if none is available.
 */
function locateRelativeFilePath(rootFileLocations, relativeFilePath) {
  if (relativeFilePath) {
    for (let i = 0; i < rootFileLocations.length; i++) {
      let resolvedFile = path.resolve(
        rootFileLocations[i],
        relativeFilePath.trim()
      )
      if (fs.existsSync(resolvedFile)) {
        return resolvedFile
      }
    }
  }
  return null
}

/**
 * Returns a promise of an execution of an external program.
 *
 * @param {*} cmd
 */
function executeExternalProgram(
  cmd,
  workingDirectory,
  options = {
    rejectOnFail: true,
    routeErrToOut: false,
  }
) {
  return new Promise((resolve, reject) => {
    childProcess.exec(
      cmd,
      {
        cwd: workingDirectory,
        windowsHide: true,
        timeout: 10000,
      },
      (error, stdout, stderr) => {
        console.log(`    ✍  ${cmd}`)
        if (error) {
          if (options.rejectOnFail) {
            reject(error)
          } else {
            console.log(error)
            resolve()
          }
        } else {
          console.log(stdout)
          if (options.routeErrToOut) {
            console.log(stderr)
          } else {
            console.error(stderr)
          }
          resolve()
        }
      }
    )
  })
}

/**
 * Retrieve specific entry from extensions defaults(array) via 'clusterCode' key fields
 *
 * @param {*} extensions
 * @param {*} extensionId field name under specific extension
 * @param {*} clusterCode search key
 * @parem {*} clusterRole: one of server/client enums, or null for either.
 * @returns Value of the cluster extension property.
 */
function getClusterExtensionDefault(
  extensions,
  extensionId,
  clusterCode,
  clusterRole = null
) {
  let f = getClusterExtension(extensions, extensionId)
  if (f.length == 0) {
    return ''
  } else {
    let val = null
    f[0].defaults.forEach((d) => {
      if (d.entityCode == clusterCode) {
        if (clusterRole == null) {
          val = d.value
        } else if (clusterRole == d.entityQualifier) {
          val = d.value
        }
      }
    })
    if (val == null) val = f[0].globalDefault
    if (val == null) val = ''
    return val
  }
}

/**
 * Retrieve specific entry from extensions defaults(array) via 'clusterCode' key fields
 *
 * @param {*} extensions
 * @param {*} property field name under specific extension
 * @param {*} clusterCode search key
 * @returns Object containing all attribuetes specific to the extension
 */
function getClusterExtension(extensions, extensionId) {
  return extensions.filter((x) => x.property == extensionId)
}

/**
 * Global way how to get an UUID.
 */
function createUuid() {
  return uuidv4()
}

/**
 * Returns a promise that resolves after time milliseconds
 * @param {} time
 */
function waitFor(time) {
  return new Promise((r) => setTimeout(r, time))
}

/**
 * Returns a promise that resolve into a parsed XML object.
 * @param {*} fileContent
 * @returns promise that resolves into parsed object.
 */
async function parseXml(fileContent) {
  return xml2js.parseStringPromise(fileContent)
}

/**
 * Reads the properties file and returns object containing
 * 'data', 'filePath' and 'crc'
 *
 * @param {*} metadata file
 * @returns Promise to populate data, filePath and crc into the context.
 */
async function readFileContentAndCrc(metadataFile) {
  let content = await fsp.readFile(metadataFile, { encoding: 'utf-8' })
  return {
    data: content,
    filePath: metadataFile,
    crc: checksum(content),
  }
}

/**
 * This method takes a nanosecond duration and prints out
 * decently human readable time out of it.
 *
 * @param {*} nsDifference
 * @returns String with human readable time duration.
 */
function duration(nsDifference) {
  let diff = Number(nsDifference)
  let out = ''
  if (diff > 1000000000) {
    out += `${Math.floor(diff / 1000000000)}s `
  }
  out += `${Math.round((diff % 1000000000) / 1000000)}ms`
  return out
}

/**
 * This method returns true if the running instance is the first
 * and main instance of the zap, and false if zap instance is already
 * running.
 *
 */
function mainOrSecondaryInstance(
  allowSecondary,
  mainInstanceCallback,
  secondaryInstanceCallback
) {
  if (allowSecondary) {
    let lock = new singleInstance('zap')
    lock.lock().then(mainInstanceCallback).catch(secondaryInstanceCallback)
  } else {
    mainInstanceCallback()
  }
}

function disable(testName) {
  const index = this.indexOf(testName)
  if (index == -1) {
    const errStr = `Test ${testName}  does not exists.`
    throw new Error(errStr)
  }

  this.splice(index, 1)
}

/**
 * Utility method that collects data from a JSON file.
 *
 * JSON file is formatted as a bunch of keyed strings:
 *    "someKey": [ "a", "b", "c"]
 * Then it supports following special keys:
 *    "include": "path/to/json/file"  - includes the said JSON file
 *    "disable": [ "x", "y" ...] - disables the specified data points
 *    "collection": ["key", "key2", ...] - collects final list of data points
 *
 * @param {*} jsonFile
 */
async function collectJsonData(jsonFile, recursiveLevel = 0) {
  if (recursiveLevel > 20) {
    // Prevent infinite recursion
    throw new Error(`Recursion too deep in JSON file inclusion.`)
  }
  let rawData = await fsp.readFile(jsonFile)
  let jsonData = JSON.parse(rawData)
  let collectedData = []
  if ('include' in jsonData) {
    let f = path.join(path.dirname(jsonFile), jsonData.include)
    let includedData = await collectJsonData(f, recursiveLevel++)
    collectedData.push(...includedData)
  }
  if ('collection' in jsonData) {
    collectedData.push(...jsonData.collection.map((c) => jsonData[c]).flat(1))
  }
  if ('disable' in jsonData) {
    collectedData = collectedData.filter(
      (test) => !jsonData.disable.includes(test)
    )
  }
  collectedData.disable = disable.bind(collectedData)
  return collectedData
}

exports.createBackupFile = createBackupFile
exports.checksum = checksum
exports.initializeSessionPackage = initializeSessionPackage
exports.matchFeatureLevel = matchFeatureLevel
exports.sessionReport = sessionReport
exports.sessionDump = sessionDump
exports.executePromisesSequentially = executePromisesSequentially
exports.createAbsolutePath = createAbsolutePath
exports.executeExternalProgram = executeExternalProgram
exports.locateRelativeFilePath = locateRelativeFilePath
exports.createUuid = createUuid
exports.waitFor = waitFor
exports.getClusterExtension = getClusterExtension
exports.getClusterExtensionDefault = getClusterExtensionDefault
exports.parseXml = parseXml
exports.readFileContentAndCrc = readFileContentAndCrc
exports.duration = duration
exports.mainOrSecondaryInstance = mainOrSecondaryInstance
exports.collectJsonData = collectJsonData
