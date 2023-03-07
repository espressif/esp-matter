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
const genEngine = require('../src-electron/generator/generation-engine.js')
const env = require('../src-electron/util/env.ts')
const dbApi = require('../src-electron/db/db-api.js')
const queryPackage = require('../src-electron/db/query-package.js')
const queryAttribute = require('../src-electron/db/query-attribute.js')
const querySession = require('../src-electron/db/query-session.js')
const queryZcl = require('../src-electron/db/query-zcl.js')
const utilJs = require('../src-electron/util/util.js')
const zclLoader = require('../src-electron/zcl/zcl-loader.js')
const helperZap = require('../src-electron/generator/helper-zap.js')
const importJs = require('../src-electron/importexport/import.js')
const testUtil = require('./test-util.js')
const testQuery = require('./test-query.js')

let db
let templateContext
let zclPackageId

const testFile = path.join(__dirname, 'resource/matter-test.zap')
const testMatterSwitch = path.join(__dirname, 'resource/matter-switch.zap')
const templateCount = testUtil.testTemplate.matterCount

beforeAll(async () => {
  env.setDevelopmentEnv()
  let file = env.sqliteTestFile('gen-matter')
  db = await dbApi.initDatabaseAndLoadSchema(
    file,
    env.schemaFile(),
    env.zapVersion()
  )
  let ctx = await zclLoader.loadZcl(db, env.builtinMatterZclMetafile())
  zclPackageId = ctx.packageId
}, testUtil.timeout.medium())

afterAll(() => dbApi.closeDatabase(db), testUtil.timeout.short())

test('Validate loading', async () => {
  let c = await testQuery.selectCountFrom(db, 'TAG')
  expect(c).toBe(15)
  c = await testQuery.selectCountFrom(db, 'GLOBAL_ATTRIBUTE_BIT')
  expect(c).toBe(10)

  let attr = await queryAttribute.selectAttributeByCode(
    db,
    zclPackageId,
    null,
    0xfffc,
    null
  )
  expect(attr).not.toBe(null)

  let cluster = await queryZcl.selectClusterByCode(
    db,
    zclPackageId,
    0x0029,
    null
  )
  expect(cluster).not.toBe(null)
  expect(cluster.name).toBe('OTA Software Update Provider')
})

test(
  'Basic gen template parsing and generation',
  async () => {
    templateContext = await genEngine.loadTemplates(
      db,
      testUtil.testTemplate.matter
    )

    expect(templateContext.crc).not.toBeNull()
    expect(templateContext.templateData).not.toBeNull()
    expect(templateContext.templateData.name).toEqual('Matter test template')
    expect(templateContext.templateData.version).toEqual('test-matter')
    expect(templateContext.templateData.templates.length).toEqual(templateCount)
    expect(templateContext.packageId).not.toBeNull()
  },
  testUtil.timeout.medium()
)

test(
  path.basename(testFile) + ' - load and generate',
  async () => {
    let sessionId = await querySession.createBlankSession(db)

    await importJs.importDataFromFile(db, testFile, {
      sessionId: sessionId,
    })

    let genResult = await genEngine.generate(
      db,
      sessionId,
      templateContext.packageId,
      {},
      { disableDeprecationWarnings: true }
    )

    expect(genResult).not.toBeNull()
    expect(genResult.partial).toBeFalsy()
    expect(genResult.content).not.toBeNull()
    let sdkExt = genResult.content['sdk-ext.txt']
    expect(sdkExt).not.toBeNull()
    expect(
      sdkExt.includes(
        "// cluster: 0x0029 OTA Software Update Provider, text extension: ''"
      )
    ).toBeTruthy()

    let simpleTest = genResult.content['simple-test.h']
    expect(simpleTest).toContain(
      'Cluster Name : OnOff+Command Name : OnWithRecallGlobalScene'
    )

    let deviceType = genResult.content['device-types.txt']
    expect(deviceType).toContain(
      '// device type: CHIP / 0x000E => MA-bridge // extension: '
    )
    expect(deviceType).toContain('>> Attribute: identify time [0]')
    expect(deviceType).toContain('>> Command: TriggerEffect [64]')

    let events = genResult.content['events.out']
    expect(events).toContain('Field: arg4 [BITMAP]')
    expect(events).toContain('Field: OperationSource [ENUM]')
    expect(events).toContain('Field: SourceNode')
  },
  testUtil.timeout.long()
)

test(
  path.basename(testMatterSwitch) + ' - load and generate',
  async () => {
    let sessionId = await querySession.createBlankSession(db)

    await importJs.importDataFromFile(db, testMatterSwitch, {
      sessionId: sessionId,
    })

    let genResult = await genEngine.generate(
      db,
      sessionId,
      templateContext.packageId,
      {},
      { disableDeprecationWarnings: true }
    )

    expect(genResult).not.toBeNull()
    expect(genResult.partial).toBeFalsy()
    expect(genResult.content).not.toBeNull()

    let endpoints = genResult.content['endpoints.out']
    expect(endpoints).toContain('>> device: MA-genericswitch [15]')
    expect(endpoints).toContain('> Switch [59] - server: 1')
    expect(endpoints).toContain('- InitialPress: 1')
    expect(endpoints).toContain('- ShortRelease: 1')
    expect(endpoints).toContain('- MultiPressOngoing: 1')
  },
  testUtil.timeout.long()
)
