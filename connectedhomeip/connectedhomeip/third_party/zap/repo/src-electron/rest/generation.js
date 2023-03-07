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
 * This module provides the REST API to the generation.
 *
 * @module REST API: generation functions
 */

const generationEngine = require('../generator/generation-engine.js')
const queryPackage = require('../db/query-package.js')
const restApi = require('../../src-shared/rest-api.js')
const dbEnum = require('../../src-shared/db-enum.js')
const { StatusCodes } = require('http-status-codes')

/**
 * HTTP GET: preview single file with index.
 *
 * @param {*} db
 * @returns callback for the express uri registration
 */
function httpGetPreviewNameIndex(db) {
  return async (request, response) => {
    let sessionId = request.zapSessionId
    let previewObject = await generationEngine.generateSingleFileForPreview(
      db,
      sessionId,
      request.params.name
    )

    if (request.params.index in previewObject) {
      response.status(StatusCodes.OK).json({
        result: previewObject[request.params.index],
        size: Object.keys(previewObject).length,
      })
    } else {
      response.status(StatusCodes.OK).json({})
    }
  }
}

/**
 * HTTP GET: Preview a single file.
 *
 * @param {*} db
 * @returns callback for the express uri registration
 */
function httpGetPreviewName(db) {
  return async (request, response) => {
    let sessionId = request.zapSessionId
    let previewObject = await generationEngine.generateSingleFileForPreview(
      db,
      sessionId,
      request.params.name
    )
    response.status(StatusCodes.OK).json(previewObject)
  }
}

/**
 * HTTP GET: total preview object.
 *
 * @param {*} db
 * @returns callback for the express uri registration
 */
function httpGetPreview(db) {
  return async (request, response) => {
    let sessionId = request.zapSessionId
    let previewObject = await queryPackage.getSessionGenTemplates(db, sessionId)
    response.status(StatusCodes.OK).json(previewObject)
  }
}

/**
 * HTTP PUT: performs local generation into a specified directory.
 *
 * @param {*} db
 * @returns callback for the express uri registration
 */
function httpPutGenerate(db) {
  return async (request, response) => {
    let sessionId = request.zapSessionId
    let generationDirectory = request.body.generationDirectory
    let pkgs = await queryPackage.getSessionPackagesByType(
      db,
      sessionId,
      dbEnum.packageType.genTemplatesJson
    )

    let promises = []
    pkgs.forEach((pkg) => {
      promises.push(
        generationEngine.generateAndWriteFiles(
          db,
          sessionId,
          pkg.id,
          generationDirectory
        )
      )
    })
    await Promise.all(promises)
    response.status(StatusCodes.OK).send()
  }
}

exports.get = [
  {
    uri: restApi.uri.previewNameIndex,
    callback: httpGetPreviewNameIndex,
  },
  {
    uri: restApi.uri.previewName,
    callback: httpGetPreviewName,
  },
  {
    uri: restApi.uri.preview,
    callback: httpGetPreview,
  },
]
exports.put = [
  {
    uri: restApi.uri.generate,
    callback: httpPutGenerate,
  },
]
