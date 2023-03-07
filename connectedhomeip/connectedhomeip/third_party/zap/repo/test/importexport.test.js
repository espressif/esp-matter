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
const importJs = require('../src-electron/importexport/import.js')
const exportJs = require('../src-electron/importexport/export.js')
const dbEnum = require('../src-shared/db-enum.js')
const dbApi = require('../src-electron/db/db-api.js')
const env = require('../src-electron/util/env.ts')
const zclLoader = require('../src-electron/zcl/zcl-loader.js')
const generationEngine = require('../src-electron/generator/generation-engine.js')
const querySession = require('../src-electron/db/query-session.js')
const queryPackage = require('../src-electron/db/query-package.js')
const testUtil = require('./test-util.js')
const testQuery = require('./test-query.js')
const queryEndpointType = require('../src-electron/db/query-endpoint-type.js')
const queryEndpoint = require('../src-electron/db/query-endpoint.js')
const util = require('../src-electron/util/util.js')

let db
let sleepyGenericZap = path.join(__dirname, 'resource/isc/sleepy-generic.zap')
let sleepyGenericIsc = path.join(__dirname, 'resource/isc/sleepy-generic.isc')
let testFile1 = path.join(__dirname, 'resource/save-file-1.zap')
let testFile2 = path.join(__dirname, 'resource/save-file-2.zap')
let matterSwitch = path.join(__dirname, 'resource/matter-switch.zap')
let testLightIsc = path.join(__dirname, 'resource/isc/test-light.isc')
let testDoorLockIsc = path.join(__dirname, 'resource/isc/ha-door-lock.isc')
let haLightIsc = path.join(__dirname, 'resource/isc/ha-light.isc')
let haCombinedInterfaceIsc = path.join(
  __dirname,
  'resource/isc/ha-combined-interface.isc'
)

// Due to future plans to rework how we handle global attributes,
// we introduce this flag to bypass those attributes when testing import/export.
let bypassGlobalAttributes = false

beforeAll(() => {
  env.setDevelopmentEnv()
  let file = env.sqliteTestFile('importexport')
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
  'Basic gen template parsing and generation',
  async () => {
    let context = await generationEngine.loadTemplates(
      db,
      testUtil.testTemplate.zigbee
    )
    expect(context.crc).not.toBeNull()
    expect(context.templateData).not.toBeNull()
  },
  testUtil.timeout.short()
)

test(
  path.basename(testFile1) + ' - import',
  async () => {
    let importResult = await importJs.importDataFromFile(db, testFile1)
    let sid = importResult.sessionId

    let x = await testQuery.selectCountFrom(db, 'ENDPOINT_TYPE')
    expect(x).toBe(1)
    x = await testQuery.selectCountFrom(db, 'ENDPOINT_TYPE_CLUSTER')
    expect(x).toBe(11)
    x = await testQuery.selectCountFrom(db, 'ENDPOINT_TYPE_COMMAND')
    expect(x).toBe(7)
    x = await testQuery.selectCountFrom(db, 'ENDPOINT_TYPE_ATTRIBUTE')
    expect(x).toBe(21)

    let state = await exportJs.createStateFromDatabase(db, sid)
    let commandCount = 0
    let attributeCount = 0
    expect(state.featureLevel).toBe(env.zapVersion().featureLevel)
    expect(state.endpointTypes.length).toBe(1)
    expect(state.endpointTypes[0].clusters.length).toBe(11)
    state.endpointTypes[0].clusters.forEach((c) => {
      commandCount += c.commands ? c.commands.length : 0
      attributeCount += c.attributes ? c.attributes.length : 0
    })
    expect(commandCount).toBe(7)
    // This flag exists for this test due to planned global attribute rework.
    expect(attributeCount).toBe(bypassGlobalAttributes ? 15 : 21)

    await querySession.deleteSession(db, sid)
  },
  testUtil.timeout.medium()
)

test(
  path.basename(testFile2) + ' - import',
  async () => {
    let sid = await querySession.createBlankSession(db)
    await importJs.importDataFromFile(db, testFile2, { sessionId: sid })

    let x = await testQuery.selectCountFrom(db, 'ENDPOINT_TYPE')
    expect(x).toBe(1)

    x = await testQuery.selectCountFrom(db, 'ENDPOINT_TYPE_CLUSTER')
    expect(x).toBe(19)

    x = await testQuery.selectCountFrom(db, 'ENDPOINT_TYPE_COMMAND')
    expect(x).toBe(24)

    x = await testQuery.selectCountFrom(db, 'ENDPOINT_TYPE_ATTRIBUTE')
    expect(x).toBe(28)

    let state = await exportJs.createStateFromDatabase(db, sid)
    let commandCount = 0
    let attributeCount = 0
    expect(state.endpointTypes.length).toBe(1)
    expect(state.endpointTypes[0].clusters.length).toBe(19)
    state.endpointTypes[0].clusters.forEach((c) => {
      commandCount += c.commands ? c.commands.length : 0
      attributeCount += c.attributes ? c.attributes.length : 0
    })
    expect(commandCount).toBe(24)
    // This flag exists for this test due to planned global attribute rework.
    expect(attributeCount).toBe(bypassGlobalAttributes ? 16 : 28)
  },
  testUtil.timeout.medium()
)

test(
  path.basename(matterSwitch) + ' - import',
  async () => {
    let sid = await querySession.createBlankSession(db)
    await importJs.importDataFromFile(db, matterSwitch, { sessionId: sid })

    let x = await testQuery.selectCountFrom(db, 'ENDPOINT_TYPE')
    expect(x).toBe(2)
    x = await testQuery.selectCountFrom(db, 'ENDPOINT_TYPE_CLUSTER')
    expect(x).toBe(27)
    x = await testQuery.selectCountFrom(db, 'ENDPOINT_TYPE_COMMAND')
    expect(x).toBe(28)
    x = await testQuery.selectCountFrom(db, 'ENDPOINT_TYPE_ATTRIBUTE')
    expect(x).toBe(66)
    x = await testQuery.selectCountFrom(db, 'ENDPOINT_TYPE_EVENT')
    expect(x).toBe(3)
  },
  testUtil.timeout.medium()
)

test(
  path.basename(sleepyGenericZap) + ' - import',
  async () => {
    let sid = await querySession.createBlankSession(db)
    await importJs.importDataFromFile(db, sleepyGenericZap, { sessionId: sid })
    let endpoints = await queryEndpoint.selectAllEndpoints(db, sid)
    expect(endpoints.length).toBe(1)
    expect(endpoints[0].deviceIdentifier).toBe(1281)
  },
  testUtil.timeout.medium()
)

test(
  path.basename(sleepyGenericIsc) + ' - import',
  async () => {
    let sid = await querySession.createBlankSession(db)
    await importJs.importDataFromFile(db, sleepyGenericIsc, { sessionId: sid })
    let endpoints = await queryEndpoint.selectAllEndpoints(db, sid)
    expect(endpoints.length).toBe(1)
    expect(endpoints[0].deviceIdentifier).toBe(1281)
  },
  testUtil.timeout.medium()
)

test(
  path.basename(testLightIsc) + ' - read state',
  async () => {
    let state = await importJs.readDataFromFile(
      testLightIsc,
      env.builtinSilabsZclMetafile()
    )
    expect(Object.keys(state.endpointTypes).length).toBe(4)
    expect(Object.keys(state.endpoint).length).toBe(3)
    expect(state.endpoint[2].endpoint).toBe(242)
    expect(state).not.toHaveProperty('parseState')
    expect(state.attributeType.length).toBe(6)
  },
  testUtil.timeout.medium()
)

test(
  path.basename(testLightIsc) + ' - import',
  async () => {
    let sid = await querySession.createBlankSession(db)
    await importJs.importDataFromFile(db, testLightIsc, { sessionId: sid })
    expect(sid).not.toBeUndefined()
    let endpointTypes = await queryEndpointType.selectAllEndpointTypes(db, sid)
    expect(endpointTypes.length).toBe(3)
    expect(endpointTypes[0].name).toBe('Centralized')
    expect(endpointTypes[1].name).toBe('GreenPower')
    expect(endpointTypes[2].name).toBe('Touchlink')
    let endpoints = await queryEndpoint.selectAllEndpoints(db, sid)
    expect(endpoints.length).toBe(3)
    let drp = await querySession.getSessionKeyValue(
      db,
      sid,
      dbEnum.sessionOption.defaultResponsePolicy
    )
    expect(drp).toBe('always')
  },
  testUtil.timeout.medium()
)

test(
  path.basename(testDoorLockIsc) + ' - import',
  async () => {
    sid = await querySession.createBlankSession(db)
    await importJs.importDataFromFile(db, testDoorLockIsc, { sessionId: sid })
    expect(sid).not.toBeUndefined()
    let endpointTypes = await queryEndpointType.selectAllEndpointTypes(db, sid)
    expect(endpointTypes.length).toBe(1)
    let endpoints = await queryEndpoint.selectAllEndpoints(db, sid)
    expect(endpoints.length).toBe(1)
    expect(endpoints[0].deviceIdentifier).toBe(10)
    let clusterState = await testQuery.getAllEndpointTypeClusterState(
      db,
      endpointTypes[0].id
    )
    expect(clusterState.length).toBe(107)

    let drp = await querySession.getSessionKeyValue(
      db,
      sid,
      dbEnum.sessionOption.defaultResponsePolicy
    )
    expect(drp).toBe('conditional')
  },
  testUtil.timeout.medium()
)

test(
  path.basename(haLightIsc) + ' - import',
  async () => {
    sid = await querySession.createBlankSession(db)
    await importJs.importDataFromFile(db, haLightIsc, { sessionId: sid })
    expect(sid).not.toBeUndefined()
    let endpointTypes = await queryEndpointType.selectAllEndpointTypes(db, sid)
    expect(endpointTypes.length).toBe(2)
    let endpoints = await queryEndpoint.selectAllEndpoints(db, sid)
    expect(endpoints.length).toBe(2)
    expect(endpoints[0].networkId).toBe(0)
    expect(endpoints[1].networkId).toBe(0)
    let ps = []
    endpointTypes.forEach((ept) => {
      ps.push(testQuery.getEndpointTypeAttributes(db, ept.id))
    })
    let attributes = await Promise.all(ps)

    let attributeCounts = attributes.map((atArray) => atArray.length)
    expect(attributeCounts).toStrictEqual([39, 16])

    let reportableCounts = attributes.map((atArray) =>
      atArray.reduce((ac, at) => ac + (at.includedReportable ? 1 : 0), 0)
    )
    expect(reportableCounts).toStrictEqual([2, 0])

    let boundedCounts = attributes.map((atArray) =>
      atArray.reduce((ac, at) => ac + (at.bounded ? 1 : 0), 0)
    )
    expect(boundedCounts).toStrictEqual([13, 4])
    let singletonCounts = attributes.map((atArray) =>
      atArray.reduce((ac, at) => ac + (at.singleton ? 1 : 0), 0)
    )
    expect(singletonCounts).toStrictEqual([9, 13])
  },
  testUtil.timeout.medium()
)

test(
  path.basename(haCombinedInterfaceIsc) + ' - import',
  async () => {
    sid = await querySession.createBlankSession(db)
    await importJs.importDataFromFile(db, haCombinedInterfaceIsc, {
      sessionId: sid,
    })
    expect(sid).not.toBeUndefined()
    let endpointTypes = await queryEndpointType.selectAllEndpointTypes(db, sid)
    expect(endpointTypes.length).toBe(1)
    let endpoints = await queryEndpoint.selectAllEndpoints(db, sid)
    expect(endpoints.length).toBe(1)
    expect(endpoints[0].networkId).toBe(0)
    let ps = []
    endpointTypes.forEach((ept) => {
      ps.push(testQuery.getEndpointTypeAttributes(db, ept.id))
    })
    let attributes = await Promise.all(ps)

    let attributeCounts = attributes.map((atArray) => atArray.length)
    expect(attributeCounts).toStrictEqual([12])

    let reportableCounts = attributes.map((atArray) =>
      atArray.reduce((ac, at) => ac + (at.includedReportable ? 1 : 0), 0)
    )
    expect(reportableCounts).toStrictEqual([1])

    let boundedCounts = attributes.map((atArray) =>
      atArray.reduce((ac, at) => ac + (at.bounded ? 1 : 0), 0)
    )
    expect(boundedCounts).toStrictEqual([3])
    let singletonCounts = attributes.map((atArray) =>
      atArray.reduce((ac, at) => ac + (at.singleton ? 1 : 0), 0)
    )
    expect(singletonCounts).toStrictEqual([6])

    let sessionDump = await util.sessionDump(db, sid)
    expect(sessionDump.usedPackages.length).toBe(1)
  },
  testUtil.timeout.medium()
)
