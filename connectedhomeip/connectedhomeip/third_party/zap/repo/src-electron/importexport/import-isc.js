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

const path = require('path')
const queryConfig = require('../db/query-config')
const queryEndpoint = require('../db/query-endpoint')
const queryZcl = require('../db/query-zcl')
const queryDeviceType = require('../db/query-device-type')
const queryAttribute = require('../db/query-attribute')
const queryCommand = require('../db/query-command')
const queryPackage = require('../db/query-package')
const querySession = require('../db/query-session')
const util = require('../util/util')
const dbEnum = require('../../src-shared/db-enum')
const restApi = require('../../src-shared/rest-api')
const env = require('../util/env')

/**
 * Locates or adds an attribute, and returns it.
 * at contains clusterId, attributeId, isClient, mfgCode and possibly value
 * @param {*} state
 */
function locateAttribute(state, at) {
  let match = null
  state.attributeType.forEach((a) => {
    if (
      at.clusterCode == a.clusterCode &&
      at.attributeCode == a.attributeCode &&
      at.isClient == a.isClient
    ) {
      match = a
    }
  })
  if (match == null) {
    state.attributeType.push(at)
    return at
  } else {
    return match
  }
}

/**
 * Parses attribute string in a form:
 *    cl:0xABCD, at:0xABCD, di: [client|server], mf:0xABCD
 *
 * @param {*} attributeString
 * @param {*} [value=null]
 */
function parseAttribute(attributeString, value = null) {
  let at = {}
  attributeString
    .split(',')
    .map((x) => x.trim())
    .forEach((el) => {
      if (el.startsWith('cl:')) {
        at.clusterCode = parseInt(el.substring(3))
      } else if (el.startsWith('at:')) {
        at.attributeCode = parseInt(el.substring(3))
      } else if (el.startsWith('di:')) {
        at.side =
          el.substring(3).trim() == 'client'
            ? dbEnum.side.client
            : dbEnum.side.server
      } else if (el.startsWith('mf:')) {
        at.mfgCode = parseInt(el.substring(3))
      }
    })
  at.storageOption = dbEnum.storageOption.ram
  if (value != null) {
    at.value = value
  }
  return at
}

/**
 * Logic that parses data out of an ISC file into a java object
 *
 * @param {*} state
 * @param {*} line
 */
function parseZclAfv2Line(state, line) {
  if (line.startsWith('configuredEndpoint:')) {
    if (!('endpoint' in state)) {
      state.endpoint = []
    }
    // configuredEndpoint:*ep:1,pi: -1,di:-1,dv:1,ept:Centralized,nwk:Primary
    let tokens = line.substring('configuredEndpoint:'.length).split(',')
    let endpoint = {}
    tokens.forEach((tok) => {
      if (tok.startsWith('ep:')) {
        endpoint.endpoint = parseInt(tok.substring('ep:'.length))
      } else if (tok.startsWith('*ep:')) {
        endpoint.endpoint = parseInt(tok.substring('*ep:'.length))
      } else if (tok.startsWith('pi:')) {
        // This might be -1 and should be overriden from the actual device from the endpoint type
        endpoint.profileId = parseInt(tok.substring('pi:'.length))
      } else if (tok.startsWith('di:')) {
        // This might be -1 and should be overriden from the actual device from the endpoint type
        endpoint.deviceId = parseInt(tok.substring('di:'.length))
      } else if (tok.startsWith('dv:')) {
        endpoint.deviceVersion = parseInt(tok.substring('dv:'.length))
      } else if (tok.startsWith('ept:')) {
        endpoint.endpointType = tok.substring('ept:'.length)
      } else if (tok.startsWith('nwk:')) {
        let network = tok.substring('nwk:'.length)
        let networkId = state.networks.indexOf(network)
        if (networkId == -1) {
          state.networks.push(network)
          networkId = state.networks.indexOf(network)
        }
        endpoint.network = networkId
      }
    })
    state.endpoint.push(endpoint)
  } else if (line.startsWith('beginEndpointType:')) {
    // Create a temporary state.endpointType
    state.endpointType = {
      typeName: line.substring('beginEndpointType:'.length),
      clusterOverride: [],
    }
  } else if (line.startsWith('endEndpointType')) {
    // Stick the endpoint into `state.endpointTypes[endpointType.typeName]'
    if (!('endpointTypes' in state)) {
      state.endpointTypes = {}
    }
    state.endpointTypes[state.endpointType.typeName] = state.endpointType
    delete state.endpointType
  } else if (line.startsWith('device:')) {
    state.endpointType.device = line.substring('device:'.length)
  } else if (line.startsWith('deviceId:')) {
    state.endpointType.deviceId = parseInt(line.substring('deviceId:'.length))
  } else if (line.startsWith('profileId:')) {
    state.endpointType.profileId = parseInt(line.substring('profileId:'.length))
  } else if (line.startsWith('overrideClientCluster:')) {
    let idOnOff = line.substring('overrideClientCluster:'.length).split(',')
    let override = {
      clusterId: parseInt(idOnOff[0]),
      isIncluded: idOnOff[1] == 'yes',
      side: dbEnum.side.client,
    }
    state.endpointType.clusterOverride.push(override)
  } else if (line.startsWith('overrideServerCluster:')) {
    let idOnOff = line.substring('overrideServerCluster:'.length).split(',')
    let override = {
      clusterId: parseInt(idOnOff[0]),
      isIncluded: idOnOff[1] == 'yes',
      side: dbEnum.side.server,
    }
    state.endpointType.clusterOverride.push(override)
  } else if (line.startsWith('intMap:DefaultResponsePolicy = ')) {
    let drp = parseInt(line.slice('intMap:DefaultResponsePolicy = '.length))
    switch (drp) {
      case 0:
        state.sessionKey.defaultResponsePolicy = 'always'
        break
      case 1:
        state.sessionKey.defaultResponsePolicy = 'conditional'
        break
      case 2:
        state.sessionKey.defaultResponsePolicy = 'never'
        break
    }
  } else if (line == 'beginAttributeDefaults') {
    state.parseState = line
  } else if (line == 'endAttributeDefaults') {
    state.parseState = 'zclAfv2'
  } else if (line == 'beginAttributeDefaultReportingConfig') {
    state.parseState = line
  } else if (line == 'endAttributeDefaultReportingConfig') {
    state.parseState = 'zclAfv2'
  } else if (line == 'beginAttrList:EXTERNALLY_SAVED') {
    state.parseState = line.substring('beginAttrList:'.length)
  } else if (line == 'endAttrList:EXTERNALLY_SAVED') {
    state.parseState = 'zclAfv2'
  } else if (line == 'beginAttrList:OPTIONAL') {
    state.parseState = line.substring('beginAttrList:'.length)
  } else if (line == 'endAttrList:OPTIONAL') {
    state.parseState = 'zclAfv2'
  } else if (line == 'beginAttrList:SINGLETON') {
    state.parseState = line.substring('beginAttrList:'.length)
  } else if (line == 'endAttrList:SINGLETON') {
    state.parseState = 'zclAfv2'
  } else if (line == 'beginAttrList:BOUNDED') {
    state.parseState = line.substring('beginAttrList:'.length)
  } else if (line == 'endAttrList:BOUNDED') {
    state.parseState = 'zclAfv2'
  } else if (line == 'beginAttrList:SAVED_TO_FLASH') {
    state.parseState = line.substring('beginAttrList:'.length)
  } else if (line == 'endAttrList:SAVED_TO_FLASH') {
    state.parseState = 'zclAfv2'
  } else if (line == 'beginAttrList:REPORTABLE') {
    state.parseState = line.substring('beginAttrList:'.length)
  } else if (line == 'endAttrList:REPORTABLE') {
    state.parseState = 'zclAfv2'
  } else if (state.parseState == 'beginAttributeDefaults') {
    let arr = line.split('=>').map((x) => x.trim())
    let at = parseAttribute(arr[0], arr[1])
    locateAttribute(state, at).defaultValue = at.value
  } else if (state.parseState == 'beginAttributeDefaultReportingConfig') {
    let arr = line.split('=>').map((x) => x.trim())
    // Now parse arr[1], which is min,max:change
    let rpt = {}
    let splits = arr[1].split(':')
    let splits2 = splits[0].split(',')
    rpt.reportableChange = splits[1]
    rpt.minInterval = splits2[0]
    rpt.maxInterval = splits2[1]
    let at = parseAttribute(arr[0], rpt)
    at = locateAttribute(state, at)
    at.minInterval = parseInt(rpt.minInterval)
    at.maxInterval = parseInt(rpt.maxInterval)
    at.reportableChange = parseInt(rpt.reportableChange)
  } else if (state.parseState == 'EXTERNALLY_SAVED') {
    let at = parseAttribute(line.trim())
    locateAttribute(state, at).storageOption = dbEnum.storageOption.external
  } else if (state.parseState == 'OPTIONAL') {
    let at = parseAttribute(line.trim())
    locateAttribute(state, at).isOptional = true
  } else if (state.parseState == 'SINGLETON') {
    let at = parseAttribute(line.trim())
    locateAttribute(state, at).isSingleton = true
  } else if (state.parseState == 'BOUNDED') {
    let at = parseAttribute(line.trim())
    locateAttribute(state, at).bounded = true
  } else if (state.parseState == 'SAVED_TO_FLASH') {
    let at = parseAttribute(line.trim())
    locateAttribute(state, at).storageOption = dbEnum.storageOption.nvm
  } else if (state.parseState == 'REPORTABLE') {
    let at = parseAttribute(line.trim())
    locateAttribute(state, at).reportable = true
  }
}

/**
 * Function that deals with the zcl customizer data inside the ISC file
 *
 * @param {*} state
 * @param {*} line
 */
function parseZclCustomizer(state, line) {
  //console.log(`zclCustomizer:${line}`)
}

/**
 * Toplevel parser that ignore anything except the two setups that are
 * ZCL relevant.
 *
 * @param {*} filePath
 * @param {*} data
 * @returns promise of read ISC data
 */
async function readIscData(filePath, data, zclMetafile) {
  const lines = data.toString().split(/\r?\n/)
  const errorLines = []

  let parser = null
  let state = {
    log: [],
    filePath: filePath,
    featureLevel: 0,
    keyValuePairs: [],
    loader: iscDataLoader,
    parseState: 'init',
    // These are not the same as with zap files
    attributeType: [],
    zclMetafile: zclMetafile,
    sessionKey: {},
    networks: [],
  }

  state.log.push({
    timestamp: new Date().toISOString(),
    log: `Imported from ${path.basename(filePath)}`,
  })
  lines.forEach((line) => {
    if (line == '{setupId:zclAfv2') {
      parser = parseZclAfv2Line
      state.parseState = 'zclAfv2'
      return
    }
    if (line == '{setupId:zclCustomizer') {
      parser = parseZclCustomizer
      state.parseState = 'zclCustomizer'
      return
    }

    if (state.parseState != 'init' && line == '}') {
      parser = null
      state.parseState = 'nonSetup'
      return
    }

    if (parser != null) {
      try {
        parser(state, line)
      } catch (msg) {
        errorLines.push(msg)
      }
    }
  })

  if (state.parseState == 'init') {
    const S =
      'Error importing the file: there is no usable ZCL content in this file.'
    state.log.push(S)
    throw new Error(S)
  }
  delete state.parseState
  if (errorLines.length > 0) {
    throw new Error(
      'Error while importing the file:\n  - ' + errorLines.join('\n  - ')
    )
  } else {
    return state
  }
}

/**
 * Load individual endpoint types.
 *
 * @param {*} db
 * @param {*} sessionId
 * @param {*} zclPackages Array of package IDs for zcl queries.
 * @param {*} endpointType
 */
async function loadEndpointType(db, sessionId, packageId, endpointType) {
  let deviceName = endpointType.device
  let deviceCode = endpointType.deviceId

  let dev
  if (isCustomDevice(deviceName, deviceCode)) {
    dev = await queryDeviceType.selectDeviceTypeByCodeAndName(
      db,
      packageId,
      dbEnum.customDevice.code,
      dbEnum.customDevice.name
    )
  } else {
    dev = await queryDeviceType.selectDeviceTypeByCodeAndName(
      db,
      packageId,
      deviceCode,
      deviceName
    )
  }

  if (dev == null)
    throw new Error(`Unknown device type: ${deviceName} / ${deviceCode}`)
  return queryConfig.insertEndpointType(
    db,
    sessionId,
    endpointType.typeName,
    dev.id,
    false
  )
}

function isCustomDevice(deviceName, deviceCode) {
  return deviceName == 'zcustom'
}

async function loadSingleAttribute(db, endpointTypeId, packageId, at) {
  let id = await queryConfig.selectEndpointTypeAttributeId(
    db,
    endpointTypeId,
    packageId,
    at.clusterCode,
    at.attributeCode,
    at.side,
    at.mfgCode
  )

  if (id == null) {
    if (at.isOptional) {
      // We need to load this thing.
      let cluster = await queryZcl.selectClusterByCode(
        db,
        packageId,
        at.clusterCode,
        at.mfgCode
      )
      let attribute = await queryAttribute.selectAttributeByCode(
        db,
        packageId,
        at.clusterCode,
        at.attributeCode,
        at.mfgCode
      )
      if (cluster == null || attribute == null) {
        env.logWarning(
          `Could not resolve attribute ${at.clusterCode} / ${at.attributeCode}`
        )
        return
      }
      let clusterRef = cluster.id
      let attributeRef = attribute.id
      id = await queryConfig.insertOrUpdateAttributeState(
        db,
        endpointTypeId,
        clusterRef,
        at.side,
        attributeRef,
        [{ key: restApi.updateKey.attributeSelected, value: 1 }],
        attribute.reportMinInterval,
        attribute.reportMaxInterval,
        attribute.reportableChange
      )
    } else {
      // This is ok: we are iterating over all endpoint type ids,
      // since ISC file doesn't really specifically override attribute
      // for every given endpoint type. So if we are looking at
      // the endpoint type which simply doesn't have this
      // attribute, so be it. Move on.
      return
    }
  }

  let keyValuePairs = []
  if ('storageOption' in at) {
    keyValuePairs.push([restApi.updateKey.attributeStorage, at.storageOption])
  }
  if ('defaultValue' in at) {
    keyValuePairs.push([restApi.updateKey.attributeDefault, at.defaultValue])
  }
  let reportable = false
  if ('minInterval' in at) {
    keyValuePairs.push([restApi.updateKey.attributeReportMin, at.minInterval])
    reportable = true
  }
  if ('maxInterval' in at) {
    keyValuePairs.push([restApi.updateKey.attributeReportMax, at.maxInterval])
    reportable = true
  }
  if ('reportableChange' in at) {
    keyValuePairs.push([
      restApi.updateKey.attributeReportChange,
      at.reportableChange,
    ])
    reportable = true
  }
  if ('isSingleton' in at) {
    keyValuePairs.push([restApi.updateKey.attributeSingleton, at.isSingleton])
  }
  if ('bounded' in at) {
    keyValuePairs.push([restApi.updateKey.attributeBounded, at.bounded])
  }
  if (reportable) {
    keyValuePairs.push([restApi.updateKey.attributeReporting, 1])
  }
  return queryConfig.updateEndpointTypeAttribute(db, id, keyValuePairs)
}

/**
 * Loads all implemented commands for a single endpoint.
 *
 * @param {*} db
 * @param {*} zclPackageId
 * @param {*} state
 * @param {*} commandExtensions
 * @param {*} endpointTypeId
 */
async function loadImplementedCommandsForEndpoint(
  db,
  zclPackageId,
  state,
  commandExtensions,
  endpointTypeId
) {
  let codes = {}
  for (const ext of commandExtensions.defaults) {
    if (ext.value == 1) {
      if (!(ext.parentCode in codes)) {
        codes[ext.parentCode] = []
      }
      codes[ext.parentCode].push(ext.entityCode)
    }
  }
  let insertionPromises = []
  // We have an array of codes now that we have to load into the database.
  for (const c of Object.keys(codes)) {
    let clusterCode = parseInt(c)
    let commandIds = codes[c]
    let cluster = await queryZcl.selectClusterByCode(
      db,
      zclPackageId,
      clusterCode
    )
    for (const commandCode of Object.keys(commandIds)) {
      let command = await queryCommand.selectCommandByCode(
        db,
        zclPackageId,
        clusterCode,
        commandCode
      )
      if (cluster != null && command != null) {
        // Inject the corresponding cluster command combo into endpoint
        let p = queryConfig.insertOrUpdateCommandState(
          db,
          endpointTypeId,
          cluster.id,
          command.source,
          command.id,
          1,
          true
        )
        insertionPromises.push(p)
      }
    }
  }
  return Promise.all(insertionPromises)
}

/**
 * This method goes over the endpoint type and the state, and enables
 * commands that belong to enabled clusters and are listed in the
 * commandExtensions as "implemented".
 * @param {*} db
 * @param {*} zclPackageId
 * @param {*} state
 * @param {*} commandExtensions
 * @param {*} endpointTypeIdArray
 */
async function loadImplementedCommandsExtensions(
  db,
  zclPackageId,
  state,
  commandExtensions,
  endpointTypeIdArray
) {
  let promises = []
  for (let endpointTypeId of endpointTypeIdArray) {
    promises.push(
      loadImplementedCommandsForEndpoint(
        db,
        zclPackageId,
        state,
        commandExtensions,
        endpointTypeId
      )
    )
  }
  return Promise.all(promises)
}

/**
 * This method resolves promises that contain all the
 * queries that are needed to load the attribute state
 *
 * @param {*} db
 * @param {*} state
 * @param {*} sessionId
 */
async function loadCommands(
  db,
  state,
  zclPackageId,
  genPackageId,
  endpointTypeIdArray
) {
  if (genPackageId != null) {
    let commandExtensions =
      await queryPackage.selectPackageExtensionByPropertyAndEntity(
        db,
        genPackageId,
        'implementedCommands',
        dbEnum.packageExtensionEntity.command
      )
    if (
      commandExtensions != null &&
      commandExtensions.defaults != null &&
      commandExtensions.defaults.length > 0
    ) {
      await loadImplementedCommandsExtensions(
        db,
        zclPackageId,
        state,
        commandExtensions,
        endpointTypeIdArray
      )
    }
  }
}

/**
 * This method resolves promises that contain all the
 * queries that are needed to load the attribute state
 *
 * @param {*} db
 * @param {*} state
 * @param {*} sessionId
 */
async function loadAttributes(db, state, packageId, endpointTypeIdArray) {
  let promises = []
  if (state.attributeType.length > 0 && endpointTypeIdArray.length > 0) {
    endpointTypeIdArray.forEach((endpointTypeId) => {
      state.attributeType.forEach((at) => {
        promises.push(loadSingleAttribute(db, endpointTypeId, packageId, at))
      })
    })
  }
  if (promises.length > 0) {
    return Promise.all(promises)
  } else {
    return []
  }
}

/**
 * Loads the session key values from the keyValues object
 * @param {*} db
 * @param {*} sessionId
 * @param {*} keyValues
 */
async function loadSessionKeyValues(db, sessionId, keyValues) {
  return querySession.insertSessionKeyValues(db, sessionId, keyValues)
}

/**
 * Function that actually loads the data out of a state object.
 * Session at this point is blank, and has no packages.
 *
 * @param {*} db
 * @param {*} state
 * @param {*} sessionId
 */
async function iscDataLoader(db, state, sessionId) {
  let endpointTypes = state.endpointTypes
  let promises = []

  // We don't have the package info inside ISC file, so we
  // do our best here.
  await util.initializeSessionPackage(db, sessionId, {
    zcl: state.zclMetafile,
    template: null,
  })
  let zclPackages = await queryPackage.getSessionPackagesByType(
    db,
    sessionId,
    dbEnum.packageType.zclProperties
  )

  let genPackages = await queryPackage.getSessionPackagesByType(
    db,
    sessionId,
    dbEnum.packageType.genTemplatesJson
  )

  if (zclPackages.length == 0) {
    throw new Error('No zcl packages found for ISC import.')
  }

  let genPackageId = null
  if (genPackages.length == 0) {
    env.logWarning('No gen packages, missing the extensions matching.')
  } else {
    genPackageId = genPackages[0].id
  }
  let zclPackageId = zclPackages[0].id

  // Remove endpoint types that are not used.
  let usedEndpointTypes = state.endpoint.map((ep) => ep.endpointType)
  for (let endpointTypeKey of Object.keys(endpointTypes)) {
    if (!usedEndpointTypes.includes(endpointTypeKey)) {
      delete endpointTypes[endpointTypeKey]
    }
  }

  for (let key of Object.keys(endpointTypes)) {
    promises.push(
      loadEndpointType(db, sessionId, zclPackageId, endpointTypes[key])
        .then((newEndpointTypeId) => {
          return {
            endpointTypeId: newEndpointTypeId,
            endpointType: endpointTypes[key],
          }
        })
        .then((endpointTypeIds) => {
          // Now load the cluster configs
          let clusterOverridePromises = []
          endpointTypes[key].clusterOverride.forEach((cl) => {
            let clusterCode = cl.clusterId
            let isIncluded = cl.isIncluded
            let side = cl.side
            clusterOverridePromises.push(
              queryConfig.setClusterIncluded(
                db,
                zclPackageId,
                endpointTypeIds.endpointTypeId,
                clusterCode,
                isIncluded,
                side
              )
            )
          })
          return Promise.all(clusterOverridePromises).then(
            () => endpointTypeIds
          )
        })
    )
  }

  let results = await Promise.all(promises)

  // results is an array of "endpointTypeId"/"endpointType" objects.

  let endpointInsertionPromises = []
  if (state.endpoint != null)
    state.endpoint.forEach((ep) => {
      // insert individual endpoint
      let endpointTypeId = undefined
      results.forEach((res) => {
        if (res.endpointType.typeName == ep.endpointType) {
          endpointTypeId = res.endpointTypeId

          // Now let's deal with the endpoint id and device id
          if (ep.profileId == -1) {
            ep.profileId = res.endpointType.profileId
          }
          if (ep.deviceId == -1) {
            ep.deviceId = res.endpointType.deviceId
          }
        }
      })

      if (endpointTypeId != undefined) {
        endpointInsertionPromises.push(
          queryEndpoint
            .insertEndpoint(
              db,
              sessionId,
              ep.endpoint,
              endpointTypeId,
              ep.network,
              ep.profileId,
              ep.deviceVersion,
              ep.deviceId
            )
            .then(() => endpointTypeId)
        )
      }
    })

  if (state.log != null) {
    querySession.writeLog(db, sessionId, state.log)
  }
  let endpointTypeIds = await Promise.all(endpointInsertionPromises)
  await loadAttributes(db, state, zclPackageId, endpointTypeIds)
  await loadCommands(db, state, zclPackageId, genPackageId, endpointTypeIds)
  await loadSessionKeyValues(db, sessionId, state.sessionKey)
  await querySession.setSessionClean(db, sessionId)
  return {
    sessionId: sessionId,
    zclPackageId: zclPackageId,
    templateIds: genPackages,
    errors: [],
    warnings: [],
  }
}

exports.readIscData = readIscData
