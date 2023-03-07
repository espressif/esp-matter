/**
 *
 *    Copyright (c) 2021 Silicon Labs
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
const dbEnum = require('../src-shared/db-enum.js')
const dbApi = require('../src-electron/db/db-api.js')
const env = require('../src-electron/util/env.ts')
const zclLoader = require('../src-electron/zcl/zcl-loader.js')
const querySession = require('../src-electron/db/query-session.js')
const testUtil = require('./test-util.js')
const queryPackage = require('../src-electron/db/query-package.js')
const util = require('../src-electron/util/util.js')

let db
let haLightIsc = path.join(__dirname, 'resource/isc/ha-light.isc')
let haCombinedIsc = path.join(
  __dirname,
  'resource/isc/ha-combined-interface.isc'
)

beforeAll(async () => {
  env.setDevelopmentEnv()
  let file = env.sqliteTestFile('conversion')
  db = await dbApi.initDatabaseAndLoadSchema(
    file,
    env.schemaFile(),
    env.zapVersion()
  )
  await zclLoader.loadZcl(db, env.builtinSilabsZclMetafile())
  await zclLoader.loadZcl(db, env.builtinDotdotZclMetafile())
}, testUtil.timeout.long())

afterAll(async () => dbApi.closeDatabase(db), testUtil.timeout.short())

test.skip(
  path.basename(haLightIsc) + ' - conversion',
  async () => {
    let sid = await querySession.createBlankSession(db)
    await importJs.importDataFromFile(db, haLightIsc, { sessionId: sid })
    expect(sid).not.toBeUndefined()

    // validate packageId
    let pkgs = await queryPackage.getSessionPackagesByType(
      db,
      sid,
      dbEnum.packageType.zclProperties
    )
    expect(pkgs.length).toBe(1)
    expect(pkgs[0].description).toBe('ZigbeePro test data')
    expect(pkgs[0].category).toBe('zigbee')

    let dump = await util.sessionDump(db, sid)

    expect(dump.endpointTypes.length).toBe(2)
    expect(dump.endpoints.length).toBe(2)
    expect(dump.endpoints[0].networkId).toBe(0)
    expect(dump.endpoints[1].networkId).toBe(0)

    // Now make sure we have attributes ONLY from one package.
    expect(dump.usedPackages.length).toBe(1)

    let attributeCounts = dump.endpointTypes.map((ept) => ept.attributes.length)
    expect(attributeCounts).toStrictEqual([26, 11])

    let reportableCounts = dump.endpointTypes.map((ept) =>
      ept.attributes.reduce((ac, at) => ac + (at.includedReportable ? 1 : 0), 0)
    )
    expect(reportableCounts).toStrictEqual([2, 0])

    let boundedCounts = dump.endpointTypes.map((ept) =>
      ept.attributes.reduce((ac, at) => ac + (at.isBound ? 1 : 0), 0)
    )
    expect(boundedCounts).toStrictEqual([11, 2])

    let singletonCounts = dump.endpointTypes.map((ept) =>
      ept.attributes.reduce((ac, at) => ac + (at.isSingleton ? 1 : 0), 0)
    )
    expect(singletonCounts).toStrictEqual([7, 11])

    let serverAttributesCount = dump.attributes.reduce(
      (ac, at) => (ac += at.side == dbEnum.side.server ? 1 : 0),
      0
    )
    expect(serverAttributesCount).toBe(35)
    let clientAttributesCount = dump.attributes.reduce(
      (ac, at) => (ac += at.side == dbEnum.side.client ? 1 : 0),
      0
    )
    expect(clientAttributesCount).toBe(2)
  },
  testUtil.timeout.long()
)

test(
  path.basename(haCombinedIsc) + ' - conversion',
  async () => {
    sid = await querySession.createBlankSession(db)
    await importJs.importDataFromFile(db, haCombinedIsc, { sessionId: sid })
    expect(sid).not.toBeUndefined()

    // validate packageId
    let pkgs = await queryPackage.getSessionPackagesByType(
      db,
      sid,
      dbEnum.packageType.zclProperties
    )
    expect(pkgs.length).toBe(1)
    expect(pkgs[0].description).toBe('ZigbeePro test data')
    expect(pkgs[0].category).toBe('zigbee')

    let dump = await util.sessionDump(db, sid)

    expect(dump.endpointTypes.length).toBe(1)
    expect(dump.endpoints.length).toBe(1)
  },
  testUtil.timeout.long()
)
