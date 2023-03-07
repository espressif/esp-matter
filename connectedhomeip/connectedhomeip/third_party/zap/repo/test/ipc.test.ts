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
 *
 *
 * @jest-environment node
 */

const ipcClient = require('../src-electron/client/ipc-client.ts')
const ipcServer = require('../src-electron/server/ipc-server')
const util = require('../src-electron/util/util.js')
const env = require('../src-electron/util/env.ts')
const { timeout } = require('./test-util.js')

const responseWaitPeriod = 500
/**
 * This test suite is testing the basic functionality of the
 * IPC between the secondary and primary zap processes.
 */

beforeAll(async () => {
  env.setDevelopmentEnv()
})

test('test no server', () => expect(ipcServer.isServerRunning()).toBeFalsy())

test(
  'start server',
  async () => {
    await ipcServer.initServer()
    expect(ipcServer.isServerRunning()).toBeTruthy()
  },
  timeout.medium()
)

test('test no client', () => expect(ipcClient.isClientConnected()).toBeFalsy())

test(
  'connect first client',
  async () => {
    await ipcClient.initAndConnectClient()
    expect(ipcClient.isClientConnected()).toBeTruthy()
  },
  timeout.medium()
)

test(
  'no pong data',
  () => {
    expect(ipcClient.lastPongData()).toBeFalsy()
  },
  timeout.medium()
)

test(
  'send ping from client, wait a second',
  async () => {
    await ipcClient.emit(ipcServer.eventType.ping, 'hello')
    await util.waitFor(responseWaitPeriod)
  },
  timeout.medium()
)
test(
  'pong data received',
  () => {
    expect(ipcClient.lastPongData()).toEqual('hello')
  },
  timeout.medium()
)

test(
  'server status',
  async () => {
    let response = null
    ipcClient.on(ipcServer.eventType.overAndOut, (data) => (response = data))
    ipcClient.emit(ipcServer.eventType.serverStatus)
    await util.waitFor(responseWaitPeriod)
    expect(response).not.toBeNull()
    expect(response.url).not.toBeNull()
    expect(response.url.includes('http://localhost')).toBeTruthy()
    let myVersion = env.zapVersion()
    expect(response.hash).toEqual(myVersion.hash)
  },
  timeout.medium()
)

test('disconnect client', () => ipcClient.disconnectClient(), timeout.short())

test(
  'shutdown server',
  () => {
    ipcServer.shutdownServerSync()
    expect(ipcServer.isServerRunning()).toBeFalsy()
  },
  timeout.medium()
)
