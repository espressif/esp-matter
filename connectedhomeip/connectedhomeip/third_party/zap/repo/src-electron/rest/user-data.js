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
 * This module provides the REST API to the user specific data.
 *
 * @module REST API: user data
 */
const fs = require('fs')
const fsPromise = fs.promises
const env = require('../util/env')
const queryZcl = require('../db/query-zcl.js')
const dbApi = require('../db/db-api.js')
const queryAttribute = require('../db/query-attribute.js')
const queryCommand = require('../db/query-command.js')
const queryConfig = require('../db/query-config.js')
const queryEndpointType = require('../db/query-endpoint-type.js')
const queryEndpoint = require('../db/query-endpoint.js')
const querySession = require('../db/query-session.js')
const queryPackage = require('../db/query-package.js')
const asyncValidation = require('../validation/async-validation.js')
const validation = require('../validation/validation.js')
const restApi = require('../../src-shared/rest-api.js')
const zclLoader = require('../zcl/zcl-loader.js')
const dbEnum = require('../../src-shared/db-enum.js')
const { StatusCodes } = require('http-status-codes')

/**
 * HTTP GET: session key values
 *
 * @param {*} db
 * @returns callback for the express uri registration
 */
function httpGetSessionKeyValues(db) {
  return async (request, response) => {
    let sessionId = request.zapSessionId
    let sessionKeyValues = await querySession.getAllSessionKeyValues(
      db,
      sessionId
    )
    response.status(StatusCodes.OK).json(sessionKeyValues)
  }
}

/**
 * HTTP POST: save session key value
 *
 * @param {*} db
 * @returns callback for the express uri registration
 */
function httpPostSaveSessionKeyValue(db) {
  return async (request, response) => {
    let { key, value } = request.body
    let sessionId = request.zapSessionId
    env.logDebug(`[${sessionId}]: Saving: ${key} => ${value}`)
    await querySession.updateSessionKeyValue(db, sessionId, key, value)
    response.status(StatusCodes.OK).json({
      key: key,
      value: value,
    })
  }
}

/**
 * HTTP POST: cluster
 *
 * @param {*} db
 * @returns callback for the express uri registration
 */
function httpPostCluster(db) {
  return async (request, response) => {
    let { id, side, flag, endpointTypeId } = request.body
    let sessionId = request.zapSessionId

    try {
      let packageId = await queryPackage
        .getSessionPackagesByType(
          db,
          sessionId,
          dbEnum.packageType.zclProperties
        )
        .then((pkgs) => pkgs?.shift()?.id) // default to always picking first package

      if (packageId == null) {
        throw new Error('Unable to find packageId')
      }

      let insertDefault = await queryConfig
        .selectClusterState(db, endpointTypeId, id, side)
        .then((state) => state == null)

      await queryConfig.insertOrReplaceClusterState(
        db,
        endpointTypeId,
        id,
        side,
        flag
      )

      if (insertDefault) {
        await queryConfig.insertClusterDefaults(db, endpointTypeId, packageId, {
          clusterRef: id,
          side: side,
        })
      }

      response
        .status(StatusCodes.OK)
        .json({
          endpointTypeId,
          id,
          side,
          flag,
        })
        .send()
    } catch (err) {
      response
        .status(StatusCodes.INTERNAL_SERVER_ERROR)
        .json({ message: err.message, stack: err.stack })
    }
  }
}
/**
 * HTTP POST attribute update
 *
 * @param {*} db
 * @returns callback for the express uri registration */
function httpPostAttributeUpdate(db) {
  return async (request, response) => {
    let {
      action,
      endpointTypeIdList,
      id,
      value,
      listType,
      clusterRef,
      attributeSide,
      reportMinInterval,
      reportMaxInterval,
      reportableChange,
    } = request.body

    if (!Array.isArray(endpointTypeIdList) || !endpointTypeIdList.length) {
      return response.status(StatusCodes.BAD_REQUEST).json()
    }

    let paramType
    switch (listType) {
      case restApi.updateKey.attributeStorage:
      case restApi.updateKey.attributeDefault:
        paramType = 'text'
        break
      default:
        paramType = ''
        break
    }
    let paramArray =
      listType == restApi.updateKey.init
        ? null
        : [{ key: listType, value: value, type: paramType }]

    // all endpoints
    await Promise.all(
      endpointTypeIdList.map((endpointTypeId) =>
        queryConfig.insertOrUpdateAttributeState(
          db,
          endpointTypeId,
          clusterRef,
          attributeSide,
          id,
          paramArray,
          reportMinInterval,
          reportMaxInterval,
          reportableChange
        )
      )
    )

    // send latest value to frontend to update UI
    let eptAttr = await queryZcl.selectEndpointTypeAttribute(
      db,
      endpointTypeIdList[0],
      id,
      clusterRef
    )

    // only return 1 validation result.
    // error isn't endpoint specific.
    // endpointTypeId doesn't matter since all attributes are the seame.
    let validationData = await validation.validateAttribute(
      db,
      endpointTypeIdList[0],
      id,
      clusterRef
    )

    response.status(StatusCodes.OK).json({
      action: action,
      endpointTypeIdList: endpointTypeIdList,
      clusterRef: clusterRef,
      id: id,
      added: value,
      listType: listType,
      validationIssues: validationData,
      endpointTypeAttributeData: eptAttr,
    })
  }
}

/**
 * HTTP POST: command update
 *
 * @param {*} db
 * @returns callback for the express uri registration
 */
function httpPostCommandUpdate(db) {
  return async (request, response) => {
    let {
      action,
      endpointTypeIdList,
      id,
      value,
      listType,
      clusterRef,
      commandSide,
    } = request.body
    let isIncoming = null

    switch (listType) {
      case 'selectedIn':
        isIncoming = true
        break
      case 'selectedOut':
        isIncoming = false
        break
      default:
        break
    }

    await Promise.all(
      endpointTypeIdList.map((endpointTypeId) =>
        queryConfig.insertOrUpdateCommandState(
          db,
          endpointTypeId,
          clusterRef,
          commandSide,
          id,
          value,
          isIncoming
        )
      )
    )

    response.status(StatusCodes.OK).json({
      action: action,
      endpointTypeIdList: endpointTypeIdList,
      id: id,
      added: value,
      listType: listType,
      side: commandSide,
      clusterRef: clusterRef,
    })
  }
}

/**
 * HTTP POST: command update
 *
 * @param {*} db
 * @returns callback for the express uri registration
 */
function httpPostEventUpdate(db) {
  return async (request, response) => {
    let { action, endpointTypeId, id, value, listType, clusterRef, eventSide } =
      request.body
    await queryConfig.insertOrUpdateEventState(
      db,
      endpointTypeId,
      clusterRef,
      eventSide,
      id,
      value
    )

    response.status(StatusCodes.OK).json({
      action: action,
      endpointTypeId: endpointTypeId,
      id: id,
      added: value,
      listType: listType,
      side: eventSide,
      clusterRef: clusterRef,
    })
  }
}

/**
 * HTTP GET: initial state
 *
 * @param {*} db
 * @returns callback for the express uri registration
 */
function httpGetInitialState(db) {
  return async (request, response) => {
    let sessionId = request.zapSessionId
    let state = {}

    let session = await querySession.getSessionFromSessionId(db, sessionId)
    asyncValidation.initAsyncValidation(db, session)

    let results = await Promise.all([
      queryEndpointType.selectAllEndpointTypes(db, sessionId),
      queryEndpoint.selectAllEndpoints(db, sessionId),
      querySession.getAllSessionKeyValues(db, sessionId),
    ])

    state.endpointTypes = results[0]
    state.endpoints = results[1]
    state.sessionKeyValues = results[2]

    response.status(StatusCodes.OK).json(state)
  }
}

/**
 * HTTP GET: option
 *
 * @param {*} db
 * @returns callback for the express uri registration
 */
function httpGetOption(db) {
  return async (request, response) => {
    const { category } = request.params
    let sessionId = request.zapSessionId
    let packages = await queryPackage.getSessionPackages(db, sessionId)
    let p = packages.map((pkg) =>
      queryPackage.selectAllOptionsValues(db, pkg.packageRef, category)
    )
    let data = await Promise.all(p)
    data = data.flat(1)
    response.status(StatusCodes.OK).json(data)
  }
}

/**
 * HTTP GET: ui_options
 * @param {*} db
 * @returns  UI options from all packages.
 */
function httpGetUiOptions(db) {
  return async (request, response) => {
    let sessionId = request.zapSessionId
    let packages = await queryPackage.getSessionPackages(db, sessionId)
    let zclPackage = await queryPackage.getZclPropertiesPackage(db, packages)
    let data = await queryPackage.selectAllUiOptions(db, zclPackage[0].id)
    response.status(StatusCodes.OK).json(data)
  }
}

/**
 * HTTP GET: Project packages
 */
function httpGetPackages(db) {
  return async (request, response) => {
    let sessionId = request.zapSessionId
    let packageSessionPackagePairs =
      await queryPackage.getPackageSessionPackagePairBySessionId(db, sessionId)
    response.status(StatusCodes.OK).json(packageSessionPackagePairs)
  }
}

/**
 * HTTP GET: All Packages
 */
function httpGetAllPackages(db) {
  return async (request, response) => {
    let packages = await queryPackage.getAllPackages(db)
    response.status(StatusCodes.OK).json({ packages })
  }
}

/**
 * HTTP POST: Add new project package
 */
function httpPostAddNewPackage(db) {
  return async (req, res) => {
    let sessionId = req.zapSessionId
    let filePath = req.body.path
    try {
      let data = await zclLoader.loadIndividualFile(db, filePath, sessionId)
      let status
      if (data.err) {
        status = {
          isValid: false,
          err: data.err.message,
        }
      } else {
        await queryPackage.insertSessionPackage(
          db,
          sessionId,
          data.packageId,
          false
        )
        status = {
          isValid: true,
          sessionId: sessionId,
        }
      }
      res.status(StatusCodes.OK).json(status)
    } catch (err) {
      response.status(StatusCodes.INTERNAL_SERVER_ERROR).json(err)
    }
  }
}

/**
 * HTTP POST: Unify all Attributes / Command states if a certain cluster is enabled
 *            on more than one endpoint.
 *
 * 1) In Zigbee world, the Attribute / Command configurations is a global singleton entity.
 *    If one cluster is enabled by more than 1 endpoint, the attribute states (on/off) should be
 *    identical across each endpoint.
 *    To emulate the global singleton entity, this function ensures Attribute changes
 *    are applied to all endpoint specific attribute fields.
 *    When unify event is triggered, this function will align all shared Attribute/Command states
 *    to the first matching entry from beginning of the endpoint list.
 * 2) (native case in ZAP) In Matter, the Attribute configuration are endpoint specific.
 *
 */
function httpPostShareClusterStatesAcrossEndpoints(db) {
  return async (request, response) => {
    let { endpointTypeIdList } = request.body
    let sessionId = request.zapSessionId
    let packageIds = await queryPackage.getSessionPackagesByType(
      db,
      sessionId,
      dbEnum.packageType.zclProperties
    )
    packageIds = packageIds.map(function getId(item) {
      return item.id
    })
    if (!Array.isArray(endpointTypeIdList) || endpointTypeIdList.length < 1) {
      return response.status(StatusCodes.BAD_REQUEST).send()
    }

    // Get a list of clusters enabled by multiple (>1) endpoints
    let sharedClusterList = await queryEndpointType
      .selectAllClustersDetailsFromEndpointTypes(
        db,
        endpointTypeIdList.map((id) => {
          return { endpointTypeId: id }
        })
      )
      .then((list) => list.filter((entry) => entry.endpointCount > 1))

    let attrDefaults = await attributeDefaults(
      db,
      endpointTypeIdList,
      sharedClusterList,
      packageIds
    )
    await writeAttributeDefaults(db, attrDefaults)

    let cmdDefaults = await commandDefaults(
      db,
      endpointTypeIdList,
      sharedClusterList,
      packageIds
    )
    await writeCommandDefaults(db, cmdDefaults)

    return response.status(StatusCodes.OK).json({
      sharedClusterList,
      sharedAttributeDefaults: attrDefaults,
    })
  }
}

async function commandDefaults(
  db,
  endpointTypeIdList,
  sharedClusterList,
  packageIds
) {
  let sharedCmdDefaults = {}
  let clusCmdToCmdObj = {}
  let sharedCommandList =
    await queryCommand.selectAllCommandDetailsFromEnabledClusters(
      db,
      sharedClusterList.map((c) => {
        return { endpointTypeClusterRef: c.endpointClusterId }
      }),
      packageIds
    )

  for (const endpointTypeId of endpointTypeIdList) {
    for (const sharedCmd of sharedCommandList) {
      let clusCmdKey = JSON.stringify({
        clusterId: sharedCmd.clusterId,
        clusterSide: sharedCmd.clusterSide,
        id: sharedCmd.id, // command id
        code: sharedCmd.code,
        mfgCode: sharedCmd.mfgCode,
      })

      if (!(endpointTypeId in sharedCmdDefaults)) {
        sharedCmdDefaults[endpointTypeId] = []
      }

      if (clusCmdKey in clusCmdToCmdObj) {
        sharedCmdDefaults[endpointTypeId].push(clusCmdToCmdObj[clusCmdKey])
      } else {
        let cmds = await queryEndpoint.selectEndpointClusterCommands(
          db,
          sharedCmd.clusterId,
          endpointTypeId
        )

        // find attr
        let matched = cmds.filter((cmd) => commandEquals(cmd, sharedCmd))
        if (matched.length) {
          let m = matched.shift()

          sharedCmdDefaults[endpointTypeId].push(m)
          clusCmdToCmdObj[clusCmdKey] = m
        }
      }
    }
  }
  return sharedCmdDefaults
}

async function writeCommandDefaults(db, defaults) {
  let promises = []
  for (const [endpointTypeId, commandList] of Object.entries(defaults)) {
    for (const cmd of commandList) {
      promises.push(
        queryConfig.insertOrUpdateCommandState(
          db,
          endpointTypeId,
          cmd.clusterId,
          cmd.source,
          cmd.id,
          cmd.isIncoming,
          true
        )
      )

      promises.push(
        queryConfig.insertOrUpdateCommandState(
          db,
          endpointTypeId,
          cmd.clusterId,
          cmd.source,
          cmd.id,
          cmd.isOutgoing,
          false
        )
      )
    }
  }
  await Promise.all(promises)
}

async function attributeDefaults(
  db,
  endpointTypeIdList,
  sharedClusterList,
  packageIds
) {
  let sharedAttributeDefaults = {}
  let clusterIdnSideToAttrCache = {}
  let sharedAttributeList =
    await queryAttribute.selectAttributeDetailsFromEnabledClusters(
      db,
      sharedClusterList,
      packageIds
    )

  for (const endpointTypeId of endpointTypeIdList) {
    for (const sharedAttr of sharedAttributeList) {
      let clusAttrCacheKey = JSON.stringify({
        clusterId: sharedAttr.clusterId,
        side: sharedAttr.side,
        id: sharedAttr.id, // attr id
        code: sharedAttr.code,
        name: sharedAttr.name,
        type: sharedAttr.type,
        mfgCode: sharedAttr.mfgCode,
        define: sharedAttr.define,
      })

      if (clusAttrCacheKey in clusterIdnSideToAttrCache) {
        !(endpointTypeId in sharedAttributeDefaults) &&
          (sharedAttributeDefaults[endpointTypeId] = [])
        sharedAttributeDefaults[endpointTypeId].push(
          clusterIdnSideToAttrCache[clusAttrCacheKey]
        )
      } else {
        let attributes = await queryEndpoint.selectEndpointClusterAttributes(
          db,
          sharedAttr.clusterId,
          sharedAttr.side,
          endpointTypeId
        )

        // find attr
        let matched = attributes.filter((attr) =>
          attributeEquals(attr, sharedAttr)
        )
        if (matched.length) {
          let m = matched.shift()

          !(endpointTypeId in sharedAttributeDefaults) &&
            (sharedAttributeDefaults[endpointTypeId] = [])
          sharedAttributeDefaults[endpointTypeId].push(m)
          clusterIdnSideToAttrCache[clusAttrCacheKey] = m
        }
      }
    }
  }
  return sharedAttributeDefaults
}

async function writeAttributeDefaults(db, defaults) {
  let promises = []
  for (const [endpointTypeId, attributeList] of Object.entries(defaults)) {
    for (const attr of attributeList) {
      promises.push(
        queryConfig.insertOrUpdateAttributeState(
          db,
          endpointTypeId,
          attr.clusterId,
          attr.side,
          attr.id,
          [
            { key: restApi.updateKey.attributeSelected, value: 1 },
            {
              key: restApi.updateKey.attributeStorage,
              value: `"${attr.storage}"`,
            },
            {
              key: restApi.updateKey.attributeSingleton,
              value: attr.isSingleton,
            },
            {
              key: restApi.updateKey.attributeBounded,
              value: attr.isBounded,
            },
            {
              key: restApi.updateKey.attributeDefault,
              value: attr.defaultValue,
            },
            {
              key: restApi.updateKey.attributeReporting,
              value: attr.includedReportable,
            },
          ],
          attr.min,
          attr.max,
          attr.reportableChange
        )
      )
    }
  }

  await Promise.all(promises)
}

function commandEquals(a, b) {
  return (
    a.id === b.id &&
    a.name === b.name &&
    a.code === b.code &&
    a.source === b.source &&
    a.manufacturerCode === b.mfgCode
  )
}

function attributeEquals(a, b) {
  return (
    a.id === b.id &&
    a.code === b.code &&
    a.name === b.name &&
    a.side === b.side &&
    a.type === b.type &&
    a.manufacturerCode === b.mfgCode &&
    a.define === b.define
  )
}

function httpDeleteSessionPackage(db) {
  return async (request, response) => {
    let { sessionRef, packageRef } = request.query
    let removed = await queryPackage.deleteSessionPackage(
      db,
      sessionRef,
      packageRef
    )

    response.status(StatusCodes.OK).json({
      successful: removed > 0,
      sessionRef: sessionRef,
      packageRef: packageRef,
    })
  }
}

/**
 * Creating a duplicate for endpoint
 * 
 * @param {*} db
 * @returns newly created endpoint id
 */
function httpPostDuplicateEndpoint(db) {
  return async (req, res) => {
    let endpointId = req.body.id
    let endpointIdentifier = req.body.endpointIdentifier
    let endpointTypeId = req.body.endpointTypeId
    let id = await queryEndpoint.duplicateEndpoint(
      db,
      endpointId,
      endpointIdentifier,
      endpointTypeId
    )
    res.status(StatusCodes.OK).json({id:id})
  }
}

/**
 * Creating a duplicate for endpoint-type and endpoint-type-attributes
 * 
 * @param {*} db
 * @returns newly created endpoint-type id
 */
function httpPostDuplicateEndpointType(db) {
  return async (request, response) => {
    let { endpointTypeId } = request.body
    let newId = await queryConfig.duplicateEndpointType(
      db, endpointTypeId
    )

    duplicateEndpointTypeClusters(db, endpointTypeId, newId)

    response.status(StatusCodes.OK).json({
      id: newId
    })
  }
}

/**
 * duplicate all clusters and attributes of an old endpoint type, using oldEndpointType id and newly created endpointType id
 * 
 * @param {*} db
 * @param {*} oldEndpointTypeId
 * @param {*} newEndpointTypeId
 */
async function duplicateEndpointTypeClusters(db, oldEndpointTypeId, newEndpointTypeId) {
  let oldEndpointTypeClusters = await queryConfig.selectEndpointClusters(db, oldEndpointTypeId);
  oldEndpointTypeClusters.forEach(async (endpointTypeCluster) => {
    let newEndpointTypeClusterId = await queryConfig.insertOrReplaceClusterState(db,newEndpointTypeId,endpointTypeCluster.clusterRef,endpointTypeCluster.side, endpointTypeCluster.enabled)
    let oldAttributes = await queryAttribute.selectEndpointTypeAttributesByEndpointTypeRefAndClusterRef(db,oldEndpointTypeId,endpointTypeCluster.endpointTypeClusterId)
    oldAttributes.forEach(async (attrubute) => {
      await queryAttribute.duplicateEndpointTypeAttribute(db,
        newEndpointTypeId,
        newEndpointTypeClusterId,
        attrubute)
    })
  })
}

exports.post = [
  {
    uri: restApi.uri.cluster,
    callback: httpPostCluster,
  },
  {
    uri: restApi.uri.attributeUpdate,
    callback: httpPostAttributeUpdate,
  },
  {
    uri: restApi.uri.commandUpdate,
    callback: httpPostCommandUpdate,
  },
  {
    uri: restApi.uri.eventUpdate,
    callback: httpPostEventUpdate,
  },
  {
    uri: restApi.uri.saveSessionKeyValue,
    callback: httpPostSaveSessionKeyValue,
  },
  {
    uri: restApi.uri.addNewPackage,
    callback: httpPostAddNewPackage,
  },
  {
    uri: restApi.uri.shareClusterStatesAcrossEndpoints,
    callback: httpPostShareClusterStatesAcrossEndpoints,
  },
  {
    uri: restApi.uri.duplicateEndpoint,
    callback: httpPostDuplicateEndpoint,
  },
  {
    uri: restApi.uri.duplicateEndpointType,
    callback: httpPostDuplicateEndpointType,
  },
]

exports.get = [
  {
    uri: restApi.uri.getAllSessionKeyValues,
    callback: httpGetSessionKeyValues,
  },
  {
    uri: restApi.uri.initialState,
    callback: httpGetInitialState,
  },
  {
    uri: `${restApi.uri.option}/:category`,
    callback: httpGetOption,
  },
  {
    uri: restApi.uri.uiOptions,
    callback: httpGetUiOptions,
  },
  {
    uri: restApi.uri.packages,
    callback: httpGetPackages,
  },
  {
    uri: restApi.uri.getAllPackages,
    callback: httpGetAllPackages,
  },
]

exports.delete = [
  {
    uri: restApi.uri.sessionPackage,
    callback: httpDeleteSessionPackage,
  },
]
