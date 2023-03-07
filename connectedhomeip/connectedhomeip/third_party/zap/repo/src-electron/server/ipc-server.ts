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
export {}

import ipc from 'node-ipc'
import * as env from '../util/env'
import * as ipcTypes from '../../src-shared/types/ipc-types'
const path = require('path')
const os = require('os')
const util = require('../util/util.js')
const watchdog = require('../main-process/watchdog')
const httpServer = require('../server/http-server.js')
const startup = require('../main-process/startup.js')
const queryPackage = require('../db/query-package.js')
const dbEnum = require('../../src-shared/db-enum.js')

const eventType: { [key: string]: string } = {
  ping: 'ping', // Receiver responds with pong, returning the object.
  pong: 'pong', // Return of the ping data, no response required.
  over: 'over', // Sent from server to client as an intermediate printout.
  overAndOut: 'overAndOut', // Sent from server to client as a final answer.
  convert: 'convert', // Sent from client to server when requesting to convert files
  generate: 'generate', // Sent from client to server when requesting generation.
  serverStatus: 'serverStatus', // Sent from client to ask for server URL
  stop: 'stop', // Sent from client to ask for server to shut down
}

const server: ipcTypes.Server = {
  ipc: new ipc.IPC(),
  serverStarted: false,
}

/**
 * Returns the socket path for the IPC.
 */
function socketPath() {
  var defaultSocketPath =
    process.platform == 'win32'
      ? '\\\\.\\pipe\\' + 'zap-ipc' + '-sock'
      : path.join(os.tmpdir(), 'zap-ipc' + '.sock')
  return defaultSocketPath
}

function log(msg: string) {
  env.logIpc(`Ipc server: ${msg}`)
}

function handlerPing(context: ipcTypes.IpcEventHandlerContext, data: any) {
  server.ipc.server.emit(context.socket, eventType.pong, data)
}

function handlerServerStatus(context: ipcTypes.IpcEventHandlerContext) {
  let svr = httpServer.httpServerStartupMessage()
  svr.zapServerStatus = 'running'
  server.ipc.server.emit(context.socket, eventType.overAndOut, svr)
}

function handlerConvert(context: ipcTypes.IpcEventHandlerContext, data: any) {
  let zapFiles = data.files

  server.ipc.server.emit(context.socket, eventType.over, 'Convert')
  zapFiles.forEach((element: string) => {
    server.ipc.server.emit(context.socket, eventType.over, `File: ${element}`)
  })
  server.ipc.server.emit(context.socket, eventType.overAndOut, 'Done.')
}

function handlerStop(context: ipcTypes.IpcEventHandlerContext, data: any) {
  console.log('Shutting down because of remote client request.')
  server.ipc.server.emit(
    context.socket,
    eventType.overAndOut,
    'Shutting down server.'
  )
  startup.shutdown()
  util.waitFor(1000).then(() => startup.quit())
}

// Data contains: zapFileArray, outputPattern, zcl, template
async function handlerGenerate(
  context: ipcTypes.IpcEventHandlerContext,
  data: { zapFileArray: string[]; outputPattern: string }
) {
  let ps: Promise<void>[] = []
  let packages = await queryPackage.getPackagesByType(
    context.db,
    dbEnum.packageType.genTemplatesJson
  )
  let templatePackageId = packages[0].id

  data.zapFileArray.forEach((zapFile, index) => {
    ps.push(
      startup.generateSingleFile(
        context.db,
        zapFile,
        templatePackageId,
        data.outputPattern,
        index,
        {
          logger: (x: any) =>
            server.ipc.server.emit(context.socket, eventType.over, x),
          zcl: env.builtinSilabsZclMetafile(),
          template: env.builtinTemplateMetafile(),
        }
      )
    )
  })
  return Promise.all(ps).then(() => {
    server.ipc.server.emit(
      context.socket,
      eventType.overAndOut,
      'Generation done.'
    )
  })
}

const handlers = [
  {
    eventType: eventType.ping,
    handler: handlerPing,
  },
  {
    eventType: eventType.serverStatus,
    handler: handlerServerStatus,
  },
  {
    eventType: eventType.convert,
    handler: handlerConvert,
  },
  {
    eventType: eventType.generate,
    handler: handlerGenerate,
  },
  {
    eventType: eventType.stop,
    handler: handlerStop,
  },
]

/**
 * Runs just before every time IPC request is processed.
 */
function preHandler() {
  watchdog.reset()
}

/**
 * IPC initialization.
 *
 * @parem {*} isServer 'true' if this is a server, 'false' for client.
 * @param {*} options
 */
async function initServer(db = null, httpPort: number = 0) {
  return new Promise((resolve, reject) => {
    server.ipc.config.logger = log
    server.ipc.config.id = 'main'

    server.ipc.serve(socketPath(), () => {
      env.logIpc('IPC server started.')
      server.serverStarted = true

      // Register top-level handlers
      server.ipc.server.on('error', (err) => {
        env.logIpc('IPC error', err)
      })
      server.ipc.server.on('connect', () => {
        env.logIpc('New connection.')
        watchdog.reset()
      })
      server.ipc.server.on('destroy', () => {
        env.logIpc('IPC server destroyed.')
      })

      // Register individual type handlers
      handlers.forEach((handlerRecord) => {
        server.ipc.server.on(handlerRecord.eventType, (data, socket) => {
          preHandler()
          handlerRecord.handler(
            {
              db: db,
              socket: socket,
              httpPort: httpPort,
            },
            data
          )
        })
      })
      resolve('')
    })
    server.ipc.server.start()
  })
}

/**
 * Returns true if server is running.
 *
 * @returns true if server is running.
 */
function isServerRunning() {
  return server.serverStarted === true
}

/**
 * Shuts down the IPC server.
 *
 * @param {*} isServer
 */
function shutdownServerSync() {
  env.logIpc('Shutting down the server.')
  if (server.ipc.server) {
    server.ipc.server.stop()
    server.serverStarted = false
  } else {
    env.logIpc('There is no server.')
  }
}

exports.socketPath = socketPath
exports.initServer = initServer
exports.shutdownServerSync = shutdownServerSync
exports.isServerRunning = isServerRunning
exports.eventType = eventType
