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

import * as ipcTypes from '../../src-shared/types/ipc-types'
const env = require('../util/env')
const ipcServer = require('../server/ipc-server')
const util = require('../util/util.js')
import ipc from 'node-ipc'

const client: ipcTypes.Client = {
  ipc: new ipc.IPC(),
  uuid: util.createUuid(),
  connected: false,
}

let lastPong: string = ''

function log(msg: string) {
  env.logIpc(`Ipc client: ${msg}`)
}

/**
 * Initializes and connects a client.
 *
 * @returns a promise which resolves when client connects
 */
function initAndConnectClient() {
  client.ipc.config.logger = log
  client.ipc.config.id = 'main'

  return new Promise((resolve, reject) => {
    client.ipc.connectTo(client.uuid, ipcServer.socketPath(), () => {
      env.logIpc('Started the IPC client.')
      client.connected = true
      let socket = client.ipc.of[client.uuid]
      socket.on('disconnect', () => {
        env.logIpc('Client disconnected.')
        client.connected = false
      })

      // Serve pings
      socket.on(ipcServer.eventType.ping, (data) => {
        env.logIpc(`Client received a ping: ${JSON.stringify(data)}`)
        emit(ipcServer.eventType.pong, data)
      })

      socket.on(ipcServer.eventType.pong, (data) => {
        env.logIpc(`Client received a pong: ${JSON.stringify(data)}`)
        lastPong = data
      })

      resolve('')
    })
  })
}

/**
 * Register a handler for the event type.
 *
 * @param {*} eventType
 * @param {*} handler
 */
function on(eventType: string, handler: any) {
  client.ipc.of[client.uuid].on(eventType, handler)
}

/**
 * Get the last pong data.
 *
 * @returns last pong data or null if none is available
 */
function lastPongData() {
  return lastPong
}

/**
 * Returns true if client is connected.
 *
 * @returns true if client is connected
 */
function isClientConnected() {
  return client.connected === true
}

/**
 * Disconnects a client asynchronously.
 */
function disconnectClient() {
  env.logIpc('Disconnecting the IPC client.')
  client.ipc.disconnect(client.uuid)
}

/**
 * Sends a message to server.
 *
 * @param {*} key
 * @param {*} object
 */
async function emit(key: string, object?: any) {
  client.ipc.of[client.uuid].emit(key, object)
}

exports.initAndConnectClient = initAndConnectClient
exports.isClientConnected = isClientConnected
exports.disconnectClient = disconnectClient
exports.emit = emit
exports.lastPongData = lastPongData
exports.on = on
