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
 * @module REST API: endpoint
 */
const queryEndpointType = require('../db/query-endpoint-type.js')
const queryEndpoint = require('../db/query-endpoint.js')
const queryConfig = require('../db/query-config.js')
const validation = require('../validation/validation.js')
const restApi = require('../../src-shared/rest-api.js')
const { StatusCodes } = require('http-status-codes')

/**
 * HTTP DELETE: endpoint
 *
 * @param {*} db
 * @returns callback for the express uri registration
 */
function httpDeleteEndpoint(db) {
  return async (request, response) => {
    let id = request.query.id
    let removed = await queryEndpoint.deleteEndpoint(db, id)

    response.status(StatusCodes.OK).json({
      successful: removed > 0,
      id: id,
    })
  }
}

/**
 * HTTP DELETE: endpoint type
 *
 * @param {*} db
 * @returns callback for the express uri registration
 */
function httpDeleteEndpointType(db) {
  return async (request, response) => {
    let id = request.query.id
    let removed = await queryEndpointType.deleteEndpointType(db, id)
    response.status(StatusCodes.OK).json({
      successful: removed > 0,
      id: id,
    })
  }
}

/**
 * HTTP POST: endpoint
 *
 * @param {*} db
 * @returns callback for the express uri registration
 */
function httpPostEndpoint(db) {
  return async (request, response) => {
    let {
      endpointId,
      networkId,
      profileId,
      endpointType,
      endpointVersion,
      deviceIdentifier,
    } = request.body
    let sessionId = request.zapSessionId
    let newId = await queryEndpoint.insertEndpoint(
      db,
      sessionId,
      endpointId,
      endpointType,
      networkId,
      profileId,
      endpointVersion,
      deviceIdentifier
    )
    try {
      let validationData = await validation.validateEndpoint(db, newId)

      response.status(StatusCodes.OK).json({
        id: newId,
        endpointId: endpointId,
        endpointType: endpointType,
        networkId: networkId,
        deviceId: deviceIdentifier,
        profileId: profileId,
        endpointVersion: endpointVersion,
        validationIssues: validationData,
      })
    } catch (err) {
      response.status(StatusCodes.INTERNAL_SERVER_ERROR).json(err)
    }
  }
}

/**
 * HTTP POST: endpoint
 *
 * @param {*} db Main database to use for the operation.
 * @returns callback for the express uri registration
 */
function httpPatchEndpoint(db) {
  return async (request, response) => {
    let context = request.body
    let sessionIdexport = request.zapSessionId
    let changes = context.changes.map((data) => {
      let paramType = ''
      return {
        key: data.updatedKey,
        value: data.value,
        type: paramType,
      }
    })
    await queryConfig.updateEndpoint(db, sessionIdexport, context.id, changes)
    let validationData = await validation.validateEndpoint(db, context.id)
    response.status(StatusCodes.OK).json({
      endpointId: context.id,
      changes: context.changes,
      validationIssues: validationData,
    })
  }
}

/**
 * HTTP POST endpoint type
 *
 * @param {*} db
 * @returns callback for the express uri registration
 */
function httpPostEndpointType(db) {
  return async (request, response) => {
    let { name, deviceTypeRef } = request.body
    let sessionId = request.zapSessionId
    try {
      let newId = await queryConfig.insertEndpointType(
        db,
        sessionId,
        name,
        deviceTypeRef
      )

      response.status(StatusCodes.OK).json({
        id: newId,
        name: name,
        deviceTypeRef: deviceTypeRef,
      })
    } catch (err) {
      response.status(StatusCodes.INTERNAL_SERVER_ERROR).json(err)
    }
  }
}

/**
 * HTTP POST: endpoint type update
 *
 * @param {*} db
 * @returns callback for the express uri registration
 */
function httpPatchEndpointType(db) {
  return async (request, response) => {
    let { endpointTypeId, updatedKey, updatedValue } = request.body
    let sessionId = request.zapSessionId

    await queryConfig.updateEndpointType(
      db,
      sessionId,
      endpointTypeId,
      updatedKey,
      updatedValue
    )

    response.status(StatusCodes.OK).json({
      endpointTypeId: endpointTypeId,
      updatedKey: updatedKey,
      updatedValue: updatedValue,
    })
  }
}

exports.post = [
  {
    uri: restApi.uri.endpoint,
    callback: httpPostEndpoint,
  },
  {
    uri: restApi.uri.endpointType,
    callback: httpPostEndpointType,
  },
]

exports.patch = [
  {
    uri: restApi.uri.endpoint,
    callback: httpPatchEndpoint,
  },
  {
    uri: restApi.uri.endpointType,
    callback: httpPatchEndpointType,
  },
]

exports.delete = [
  {
    uri: restApi.uri.endpoint,
    callback: httpDeleteEndpoint,
  },
  {
    uri: restApi.uri.endpointType,
    callback: httpDeleteEndpointType,
  },
]
