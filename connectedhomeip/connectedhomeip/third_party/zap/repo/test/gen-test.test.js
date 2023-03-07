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
const genEngine = require('../src-electron/generator/generation-engine.js')
const env = require('../src-electron/util/env.ts')
const dbApi = require('../src-electron/db/db-api.js')
const zclLoader = require('../src-electron/zcl/zcl-loader.js')
const importJs = require('../src-electron/importexport/import.js')
const testUtil = require('./test-util.js')
const queryPackage = require('../src-electron/db/query-package.js')

let db
const testFile = path.join(__dirname, 'resource/three-endpoint-device.zap')
let sessionId
let templateContext
let zclContext

beforeAll(async () => {
  env.setDevelopmentEnv()
  let file = env.sqliteTestFile('testgen')
  db = await dbApi.initDatabaseAndLoadSchema(
    file,
    env.schemaFile(),
    env.zapVersion()
  )
}, testUtil.timeout.medium())

afterAll(() => dbApi.closeDatabase(db), testUtil.timeout.short())

test(
  'Basic test template parsing and generation',
  async () => {
    templateContext = await genEngine.loadTemplates(
      db,
      testUtil.testTemplate.unittest
    )
    expect(templateContext.crc).not.toBeNull()
    expect(templateContext.templateData).not.toBeNull()
    expect(templateContext.templateData.name).toEqual('Unit test templates')
    expect(templateContext.templateData.version).toEqual('unit-test')
    expect(templateContext.templateData.templates.length).toEqual(
      testUtil.testTemplate.testCount
    )
    expect(templateContext.packageId).not.toBeNull()
  },
  testUtil.timeout.medium()
)

test(
  'Load ZCL stuff',
  async () => {
    zclContext = await zclLoader.loadZcl(db, env.builtinSilabsZclMetafile())
  },
  testUtil.timeout.medium()
)

test(
  'File import and zcl package insertion',
  async () => {
    let importResult = await importJs.importDataFromFile(db, testFile)
    sessionId = importResult.sessionId
    expect(sessionId).not.toBeNull()
    await queryPackage.insertSessionPackage(db, sessionId, zclContext.packageId)
  },
  testUtil.timeout.medium()
)

test(
  'Test template generation',
  async () => {
    let genResult = await genEngine.generate(
      db,
      sessionId,
      templateContext.packageId,
      {},
      {
        disableDeprecationWarnings: true,
      }
    )

    expect(genResult).not.toBeNull()
    expect(genResult.partial).toBeFalsy()
    expect(genResult.content).not.toBeNull()

    let epc = genResult.content['test-fail.out']
    expect(epc).not.toBeNull()

    expect(genResult.hasErrors).toBeTruthy()

    let err = genResult.errors['test-fail.out']
    expect(err.message).toContain('this is where the failure lies')
    expect(err.message).toContain('line: 3, column: 0')
    expect(err.message).toContain('test-fail.zapt')

    err = genResult.errors['test-missing.out']
    // This is weird. If helpers are not defined, they are simply
    // ignored. Hence there is no error here.
    expect(err).toBeUndefined()
    //  expect(err.message).toContain('"non_existent_helper" not defined')).toBeTruthy()

    let testFutures = genResult.content['test-future.out']
    expect(testFutures).toContain('x=1')
    expect(testFutures).not.toContain('y=1')
  },
  testUtil.timeout.medium()
)
