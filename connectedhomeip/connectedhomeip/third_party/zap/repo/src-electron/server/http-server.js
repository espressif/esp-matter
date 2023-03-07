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
 * This module provides the HTTP server functionality.
 *
 * @module JS API: http server
 */

const express = require('express')
const session = require('express-session')
const env = require('../util/env')
const querySession = require('../db/query-session.js')
const util = require('../util/util.js')
const webSocket = require('./ws-server.js')
const studio = require('../ide-integration/studio-rest-api')
const restApi = require('../../src-shared/rest-api.js')
const dbEnum = require('../../src-shared/db-enum.js')
const watchdog = require('../main-process/watchdog')
const dirtyFlag = require('../util/async-reporting')

const restApiModules = [
  require('../rest/admin.js'),
  require('../rest/static-zcl.js'),
  require('../rest/generation.js'),
  require('../rest/file-ops.js'),
  require('../rest/ide-api-handler.js'),
  require('../rest/endpoint.js'),
  require('../rest/user-data.js'),
]
let httpServer = null

/**
 * This function is used to register a rest module, which exports
 * get/post/etc. arrays.
 *
 * @param {*} filename
 * @param {*} db
 * @param {*} app
 */
function registerRestApi(module, db, app) {
  if (module.post != null)
    module.post.forEach((singlePost) => {
      let uri = singlePost.uri
      let callback = singlePost.callback
      app.post(uri, callback(db))
    })

  if (module.put != null)
    module.put.forEach((singlePut) => {
      let uri = singlePut.uri
      let callback = singlePut.callback
      app.put(uri, callback(db))
    })

  if (module.patch != null)
    module.patch.forEach((singlePatch) => {
      let uri = singlePatch.uri
      let callback = singlePatch.callback
      app.patch(uri, callback(db))
    })

  if (module.get != null)
    module.get.forEach((singleGet) => {
      let uri = singleGet.uri
      let callback = singleGet.callback
      app.get(uri, callback(db))
    })

  if (module.delete != null)
    module.delete.forEach((singleDelete) => {
      let uri = singleDelete.uri
      let callback = singleDelete.callback
      app.delete(uri, callback(db))
    })
}

function registerAllRestModules(db, app) {
  restApiModules.forEach((module) => registerRestApi(module, db, app))
}

/**
 * Promises to initialize the http server on a given port
 * using a given database.
 *
 * @export
 * @param {*} db Database object to use.
 * @param {*} port Port for the HTTP server.
 * @returns A promise that resolves with an express app.
 */
async function initHttpServer(
  db,
  port,
  studioPort,
  options = {
    allowCors: false,
    zcl: env.builtinSilabsZclMetafile(),
    template: env.builtinTemplateMetafile(),
  }
) {
  return new Promise((resolve, reject) => {
    const app = express()

    if (options.allowCors) {
      env.logWarning('CORS is enabled. Please be careful.')
      app.use(function (req, res, next) {
        res.setHeader('Access-Control-Allow-Origin', '*')
        res.setHeader(
          'Access-Control-Allow-Methods',
          'GET,POST,PUT,PATCH,DELETE,OPTIONS'
        )
        res.setHeader(
          'Access-Control-Allow-Headers',
          'Content-Type, Access-Control-Allow-Headers, Authorization, X-Requested-With'
        )
        next()
      })
    }

    //app.use(express.urlencoded({ extended: true }))
    app.use(express.json())
    app.use(
      session({
        secret: 'Zap@Watt@SiliconLabs',
        resave: true,
        saveUninitialized: true,
      })
    )

    app.use(userSessionHandler(db, options))

    // REST modules
    registerAllRestModules(db, app)

    // Static content
    env.logDebug(`HTTP static content location: ${env.httpStaticContent()}`)
    app.use(express.static(env.httpStaticContent()))
    httpServer = app.listen(port, () => {
      resolve(app)
    })

    process.on('uncaughtException', function (err) {
      env.logWarning(`HTTP server exception.`, err)
      if (err.errno === 'EADDRINUSE') {
        httpServer = app.listen(0, () => {
          resolve(app)
        })
      } else {
        env.logError(err)
      }
    })

    webSocket.initializeWebSocket(httpServer)
    webSocket.onWebSocket(dbEnum.wsCategory.tick, () => {
      watchdog.reset()
    })
    studio.initIdeIntegration(db, studioPort)
    dirtyFlag.startAsyncReporting(db)
  })
}

function userSessionHandler(db, options) {
  return (req, res, next) => {
    let sessionUuid = req.query[restApi.param.sessionId]
    let userKey = req.session.id

    if (sessionUuid == null || userKey == null) {
      // This request does not carry a session along. Do nothing.
      next()
    } else {
      let zapUserId = req.session.zapUserId
      let zapSessionId
      if (`zapSessionId` in req.session) {
        zapSessionId = req.session.zapSessionId[sessionUuid]
      } else {
        req.session.zapSessionId = {}
        zapSessionId = null
      }
      querySession
        .ensureZapUserAndSession(db, userKey, sessionUuid, {
          sessionId: zapSessionId,
          userId: zapUserId,
        })
        .then((result) => {
          req.session.zapUserId = result.userId
          req.session.zapSessionId[sessionUuid] = result.sessionId
          req.zapSessionId = result.sessionId
          return result
        })
        .then((result) => {
          if (result.newSession) {
            return util.initializeSessionPackage(db, result.sessionId, options)
          }
        })
        .then(() => {
          next()
        })
        .catch((err) => {
          let resp = {
            error: 'Could not create session: ' + err.message,
            errorMessage: err,
          }
          studio.sendSessionCreationErrorStatus(db, resp)
          env.logError(resp)
        })
    }
  }
}

/**
 * Promises to shut down the http server.
 *
 * @export
 * @returns Promise that resolves when server is shut down.
 */
function shutdownHttpServer() {
  return new Promise((resolve, reject) => {
    if (httpServer != null) {
      shutdownHttpServerSync(() => resolve(null))
    } else {
      resolve(null)
    }
  })
}

/**
 * Promises to shut down the http server.
 *
 * @export
 * @returns Promise that resolves when server is shut down.
 */
function shutdownHttpServerSync(fn = null) {
  if (httpServer != null) {
    dirtyFlag.stopAsyncReporting()
    studio.deinitIdeIntegration()
    httpServer.close(() => {
      env.logDebug('HTTP server shut down.')
      httpServer = null
      if (fn != null) fn()
    })
  }
}

/**
 * Port http server is listening on.
 *
 * @export
 * @returns port
 */
function httpServerPort() {
  if (httpServer) {
    return httpServer.address().port
  } else {
    return 0
  }
}

/**
 * Returns the URL of the server.
 * @returns the server URL
 */
function httpServerUrl() {
  return `http://localhost:${httpServerPort()}`
}

/**
 * Returns the startup message that needs to be printed out.
 */
function httpServerStartupMessage() {
  let ver = env.zapVersion()
  return {
    url: httpServerUrl(),
    ...ver,
  }
}

// exports
exports.initHttpServer = initHttpServer
exports.shutdownHttpServer = shutdownHttpServer
exports.shutdownHttpServerSync = shutdownHttpServerSync
exports.httpServerPort = httpServerPort
exports.httpServerStartupMessage = httpServerStartupMessage
