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
 * This module provides the REST API to the static zcl queries.
 *
 * @module REST API: static zcl functions
 */

const queryZcl = require('../db/query-zcl')
const queryDeviceType = require('../db/query-device-type')
const queryCommand = require('../db/query-command')
const queryEvent = require('../db/query-event')
const queryPackage = require('../db/query-package')
const dbEnum = require('../../src-shared/db-enum')
const restApi = require('../../src-shared/rest-api')
const util = require('../util/util')
const env = require('../util/env')
const { StatusCodes } = require('http-status-codes')

// This function builds a function that has the following skeleton.
// This is used to simplify all the logic where we have selectAll and selectById for
// each of the different ZCL entities.
function zclEntityQuery(selectAllFunction, selectByIdFunction) {
  return (db, id, packageId = null) => {
    if (id == 'all') {
      return selectAllFunction(db, packageId)
    } else {
      return selectByIdFunction(db, id, packageId)
    }
  }
}

// For the CLUSTER path, we have special handling to also sideload attributes and commands relevant to that cluster.
async function returnZclEntitiesForClusterId(db, clusterId, packageId) {
  return zclEntityQuery(queryZcl.selectAllClusters, queryZcl.selectClusterById)(
    db,
    clusterId,
    packageId
  ).then((x) =>
    zclEntityQuery(
      queryZcl.selectAllAttributes,
      queryZcl.selectAttributesByClusterIdIncludingGlobal
    )(db, clusterId, [packageId]).then((y) =>
      zclEntityQuery(
        queryCommand.selectAllCommands,
        queryCommand.selectCommandsByClusterId
      )(db, clusterId).then((z) =>
        zclEntityQuery(
          queryEvent.selectAllEvents,
          queryEvent.selectEventsByClusterId
        )(db, clusterId, packageId).then((g) => {
          return {
            clusterData: x,
            attributeData: y,
            commandData: z,
            eventData: g,
          }
        })
      )
    )
  )
}

// This is the special merge function used for the CLUSTER path
function mergeZclClusterAttributeCommandEventData(accumulated, currentValue) {
  return {
    clusterData: [accumulated.clusterData, currentValue.clusterData].flat(1),
    commandData: [accumulated.commandData, currentValue.commandData].flat(1),
    attributeData: [accumulated.attributeData, currentValue.attributeData].flat(
      1
    ),
    eventData: [accumulated.eventData, currentValue.eventData].flat(1),
  }
}
//This maps over each packageId, and runs the query callback.
function reduceAndConcatenateZclEntity(
  db,
  id,
  packageIdArray,
  zclQueryCallback,
  mergeFunction = (accumulated, currentValue) => {
    return [accumulated, currentValue].flat(1)
  },
  defaultValue = []
) {
  let dataArray = packageIdArray.map((packageId) =>
    zclQueryCallback(db, id, packageId)
  )
  return Promise.all(dataArray).then((x) =>
    x.reduce(mergeFunction, defaultValue)
  )
}

function parseForZclData(db, entity, id, packageIdArray) {
  switch (entity) {
    case 'atomics':
      return reduceAndConcatenateZclEntity(
        db,
        id,
        packageIdArray,
        zclEntityQuery(queryZcl.selectAllAtomics, queryZcl.selectAtomicById)
      )
    case 'cluster':
      return reduceAndConcatenateZclEntity(
        db,
        id,
        packageIdArray,
        returnZclEntitiesForClusterId,
        mergeZclClusterAttributeCommandEventData,
        { clusterData: [], attributeData: [], commandData: [], eventData: [] }
      ).then((data) => {
        return {
          clusterData: data.clusterData,
          attributeData: data.attributeData,
          commandData: data.commandData,
          eventData: data.eventData,
        }
      })
    case 'domain':
      return reduceAndConcatenateZclEntity(
        db,
        id,
        packageIdArray,
        zclEntityQuery(queryZcl.selectAllDomains, queryZcl.selectDomainById)
      )
    case 'bitmap':
      return reduceAndConcatenateZclEntity(
        db,
        id,
        packageIdArray,
        zclEntityQuery(queryZcl.selectAllBitmaps, queryZcl.selectBitmapById)
      )
    case 'enum':
      return reduceAndConcatenateZclEntity(
        db,
        id,
        packageIdArray,
        zclEntityQuery(queryZcl.selectAllEnums, queryZcl.selectEnumById)
      )
    case 'struct':
      return reduceAndConcatenateZclEntity(
        db,
        id,
        packageIdArray,
        zclEntityQuery(queryZcl.selectAllStructsWithItemCount, [
          queryZcl.selectStructById,
        ])
      )
    case 'deviceType':
      return reduceAndConcatenateZclEntity(
        db,
        id,
        packageIdArray,
        zclEntityQuery(
          queryDeviceType.selectAllDeviceTypes,
          queryDeviceType.selectDeviceTypeById
        )
      )
    case 'endpointTypeClusters':
      return queryZcl.selectEndpointTypeClustersByEndpointTypeId(db, id)
    case 'endpointTypeAttributes':
      return queryZcl.selectEndpointTypeAttributesByEndpointId(db, id)
    case 'endpointTypeCommands':
      return queryZcl.selectEndpointTypeCommandsByEndpointId(db, id)
    case 'endpointTypeEvents':
      return queryZcl.selectEndpointTypeEventsByEndpointId(db, id)
    case `deviceTypeClusters`:
      return queryDeviceType.selectDeviceTypeClustersByDeviceTypeRef(db, id)
    case `deviceTypeAttributes`:
      return queryDeviceType.selectDeviceTypeAttributesByDeviceTypeRef(db, id)
    case `deviceTypeCommands`:
      return queryDeviceType.selectDeviceTypeCommandsByDeviceTypeRef(db, id)
    default:
      return { type: 'Unknown' }
  }
}

/**
 * API: /zcl/:entity/:id
 *
 * @export
 * @param {*} app Express instance.
 */

function httpGetZclEntity(db) {
  return async (request, response) => {
    const { id, entity } = request.params
    let sessionId = request.zapSessionId

    let packageIdArray = await queryPackage.getSessionZclPackageIds(
      db,
      sessionId
    )
    let resultData = await parseForZclData(db, entity, id, packageIdArray)
    response.status(StatusCodes.OK).json(resultData)
  }
}
/**
 * API: /zclExtension/:entity/:extension
 *
 * @param {*} db
 * @returns zcl extension handler
 */
function httpGetZclExtension(db) {
  return (request, response) => {
    const { entity, extensionId } = request.params
    let sessionId = request.zapSessionId
    if (!sessionId) {
      let err = 'Unable to retrieve zcl extension. Invalid sessionId!'
      env.logError(err)
      return response.status(StatusCodes.NOT_FOUND).send(err)
    }

    // enable components
    queryPackage
      .getSessionPackagesByType(
        db,
        sessionId,
        dbEnum.packageType.genTemplatesJson
      )
      .then((pkgs) => (pkgs.length == 0 ? null : pkgs[0].id))
      .then((packageId) => {
        if (!packageId) {
          throw new Error('Unable to retrieve valid packageId!')
        }
        return queryPackage.selectPackageExtension(db, packageId, entity)
      })
      .then((exts) => {
        let clusterExt = util.getClusterExtension(exts, extensionId)
        if (clusterExt.length) {
          return response.status(StatusCodes.OK).json(clusterExt[0])
        } else {
          throw new Error(`Unable to find cluster extension by ${extensionId}.`)
        }
      })
      .catch((err) => {
        env.logError(err)
        return response.status(StatusCodes.NOT_FOUND).send(err)
      })
  }
}

exports.get = [
  {
    uri: restApi.uri.zclEntity,
    callback: httpGetZclEntity,
  },
  {
    uri: restApi.uri.zclExtension,
    callback: httpGetZclExtension,
  },
]
