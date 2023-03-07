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
const path = require('path')
const fs = require('fs')
const importJs = require('../src-electron/importexport/import')
const dbApi = require('../src-electron/db/db-api')
const env = require('../src-electron/util/env.ts')
const zclLoader = require('../src-electron/zcl/zcl-loader')
const querySession = require('../src-electron/db/query-session')
const queryPackage = require('../src-electron/db/query-package')
const querySessionZcl = require('../src-electron/db/query-session-zcl')
const testUtil = require('./test-util')
const queryEndpoint = require('../src-electron/db/query-endpoint')
const dbEnum = require('../src-shared/db-enum')
const utilJs = require('../src-electron/util/util')

let testFile = path.join(__dirname, 'resource/three-endpoint-device.zap')
let testScript2 = path.join(__dirname, 'resource/test-script-2.js')
let testScript3 = path.join(__dirname, 'resource/test-script-3.js')
let testScript4 = path.join(__dirname, 'resource/test-script-4.js')

beforeAll(() => {
  env.setDevelopmentEnv()
  let file = env.sqliteTestFile('script-api')
  return dbApi
    .initDatabaseAndLoadSchema(file, env.schemaFile(), env.zapVersion())
    .then((d) => {
      db = d
      env.logInfo(`Test database initialized: ${file}.`)
    })
    .then(() => zclLoader.loadZcl(db, env.builtinSilabsZclMetafile()))
    .catch((err) => env.logError(`Error: ${err}`))
}, testUtil.timeout.medium())

afterAll(() => dbApi.closeDatabase(db), testUtil.timeout.short())

test(
  path.basename(testScript2),
  async () => {
    let sid = await querySession.createBlankSession(db)
    await importJs.importDataFromFile(db, testFile, {
      sessionId: sid,
      postImportScript: testScript2,
    })
    let endpoints = await queryEndpoint.selectAllEndpoints(db, sid)
    expect(endpoints.length).toBe(2)
    expect(endpoints[0].endpointIdentifier).toBe(42)
  },
  testUtil.timeout.medium()
)

test(
  path.basename(testScript3),
  async () => {
    let sid = await querySession.createBlankSession(db)
    await importJs.importDataFromFile(db, testFile, {
      sessionId: sid,
      postImportScript: testScript3,
    })
    let endpoints = await queryEndpoint.selectAllEndpoints(db, sid)
    expect(endpoints.length).toBe(3)
    // get clusters on first endpoint
    let clusters = await queryEndpoint.selectEndpointClusters(
      db,
      endpoints[0].endpointTypeRef
    )
    // Script 3 is supposed to remove that cluster client with code
    // 2 so verify that this in fact happened.
    let deviceTemps = clusters.filter(
      (cl) => cl.code == 2 && cl.side == dbEnum.side.client
    )
    expect(deviceTemps.length).toBe(0)

    // Script 3 is also to set the manufacturer name
    // attributed to not be included any more, so
    // we check that it in fact gets turned off.
    let basicCluster = clusters.filter((cl) => cl.code == 0)[0]
    let basicAttributes = await queryEndpoint.selectEndpointClusterAttributes(
      db,
      basicCluster.clusterId,
      dbEnum.side.server,
      endpoints[0].endpointTypeRef
    )
    expect(basicAttributes.length).toBe(5)
    let manufName = basicAttributes.filter(
      (at) => at.code == 4 && at.isIncluded == 1
    )
    expect(manufName.length).toBe(0)

    let groupsCluster = clusters.filter((cl) => cl.code == 4)[0]
    let groupsCommands = await queryEndpoint.selectEndpointClusterCommands(
      db,
      groupsCluster.clusterId,
      endpoints[0].endpointTypeRef
    )
    expect(groupsCommands.length).toBe(10)
  },
  testUtil.timeout.medium()
)

test(
  path.basename(testScript4),
  async () => {
    let sid = await querySession.createBlankSession(db)
    await importJs.importDataFromFile(db, testFile, {
      sessionId: sid,
      postImportScript: testScript4,
    })
    let endpoints = await queryEndpoint.selectAllEndpoints(db, sid)
    let cl = await querySessionZcl.selectSessionClusterByCode(db, sid, 0x0101)
    expect(cl).not.toBeNull()

    expect(endpoints.length).toBe(3)
    expect(endpoints[0].endpointIdentifier).toBe(41)
  },
  testUtil.timeout.medium()
)
