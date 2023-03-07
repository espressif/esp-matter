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
const genEngine = require('../src-electron/generator/generation-engine')
const env = require('../src-electron/util/env')
const dbApi = require('../src-electron/db/db-api')
const zclLoader = require('../src-electron/zcl/zcl-loader')
const importJs = require('../src-electron/importexport/import')
const testUtil = require('./test-util')
const queryPackage = require('../src-electron/db/query-package')
const queryZcl = require('../src-electron/db/query-zcl')
const queryAccess = require('../src-electron/db/query-access')

let db
const testFile = path.join(__dirname, 'resource/test-meta.zap')
let sessionId
let templateContext
let zclContext

beforeAll(async () => {
  env.setDevelopmentEnv()
  let file = env.sqliteTestFile('testmeta')
  db = await dbApi.initDatabaseAndLoadSchema(
    file,
    env.schemaFile(),
    env.zapVersion()
  )
}, testUtil.timeout.medium())

afterAll(() => dbApi.closeDatabase(db), testUtil.timeout.short())

test(
  'Meta test - template loading',
  async () => {
    templateContext = await genEngine.loadTemplates(
      db,
      testUtil.testTemplate.meta
    )
    expect(templateContext.crc).not.toBeNull()
    expect(templateContext.templateData).not.toBeNull()
    expect(templateContext.templateData.name).toEqual('Meta test templates')
    expect(templateContext.templateData.version).toEqual('meta-test')
    expect(templateContext.packageId).not.toBeNull()
  },
  testUtil.timeout.medium()
)

test(
  'Meta test - zcl loading',
  async () => {
    zclContext = await zclLoader.loadZcl(db, testUtil.testZclMetafile)

    const attributes = await queryZcl.selectAllAttributes(db, [
      zclContext.packageId,
    ])
    expect(attributes.length).toBe(4)
    expect(attributes[0].name).toBe('at1')
    expect(attributes[1].name).toBe('at2')

    let access
    access = await queryAccess.selectAttributeAccess(db, attributes[0].id)
    expect(access.length).toBe(1)
    expect(access[0].operation).toBe('write')
    expect(access[0].role).toBe('manage')
    expect(access[0].accessModifier).toBe('fabric-scoped')
    access = await queryAccess.selectAttributeAccess(db, attributes[1].id)
    expect(access.length).toBe(1)
    expect(access[0].operation).toBeNull()
    expect(access[0].role).toBeNull()
    expect(access[0].accessModifier).toBe('fabric-sensitive')

    const structs = await queryZcl.selectAllStructsWithItemCount(db, [
      zclContext.packageId,
    ])
    for (const s of structs) {
      let clusters = await queryZcl.selectStructClusters(db, s.id)
      if (s.name == 'SimpleStruct') {
        expect(clusters.length).toBe(2)
        expect(clusters[0].code).toBe(0xabcd)
        expect(clusters[1].code).toBe(0xabce)
      } else if (s.name == 'StructWithArray') {
        expect(clusters.length).toBe(1)
        expect(clusters[0].code).toBe(0xabcd)
      } else {
        expect(clusters.length).toBe(0)
      }
    }

    const enums = await queryZcl.selectAllEnums(db, zclContext.packageId)
    for (const e of enums) {
      let clusters = await queryZcl.selectEnumClusters(db, e.id)
      if (
        e.name == 'TestEnum' ||
        e.name == 'SparseEnum' ||
        e.name == 'LongerEnum'
      ) {
        expect(clusters.length).toBe(1)
        expect(clusters[0].code).toBe(0xabcd)
      } else {
        expect(clusters.length).toBe(0)
      }
    }

    const bitmaps = await queryZcl.selectAllBitmaps(db, zclContext.packageId)
    expect(bitmaps.length).toBe(10)
    for (const b of bitmaps) {
      let clusters = await queryZcl.selectBitmapClusters(db, b.id)
      if (b.name == 'ClusterBitmap') {
        expect(clusters.length).toBe(1)
        expect(clusters[0].code).toBe(0xabcd)
      } else {
        expect(clusters.length).toBe(0)
      }
    }

    const ops = await queryAccess.selectAccessOperations(
      db,
      zclContext.packageId
    )
    expect(ops.length).toBe(3)
    const roles = await queryAccess.selectAccessRoles(db, zclContext.packageId)
    expect(roles.length).toBe(4)
    const mods = await queryAccess.selectAccessModifiers(
      db,
      zclContext.packageId
    )
    expect(mods.length).toBe(2)

    let d

    d = await queryAccess.selectDefaultAccess(
      db,
      zclContext.packageId,
      'command'
    )
    expect(d[0].operation).toBe('invoke')
    expect(d[0].role).toBeNull()
    expect(d[0].accessModifier).toBeNull()
    expect(d.length).toBe(1)

    d = await queryAccess.selectDefaultAccess(
      db,
      zclContext.packageId,
      'cluster'
    )
    expect(d.length).toBe(2)
    expect(d[0].operation).toBe('read')
    expect(d[1].operation).toBe('write')

    d = await queryAccess.selectDefaultAccess(
      db,
      zclContext.packageId,
      'attribute'
    )
    expect(d.length).toBe(2)
    expect(d[0].operation).toBe('read')
    expect(d[1].operation).toBe('write')
    expect(d[0].role).toBe('view')
    expect(d[1].role).toBe('operate')
  },
  testUtil.timeout.medium()
)

test(
  'Meta test - file import',
  async () => {
    let importResult = await importJs.importDataFromFile(db, testFile)
    sessionId = importResult.sessionId
    expect(sessionId).not.toBeNull()
  },
  testUtil.timeout.medium()
)

test(
  'Meta test - generation',
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

    let epc = genResult.content['test1.out']
    expect(epc).not.toBeNull()
    expect(epc).toContain('Test1 template.')
    expect(epc).toContain('- attribute: at2 => true => mandatory')
    expect(epc).toContain('Stamp: Meta stamp.')
    expect(epc).toContain('Category: Meta category.')

    epc = genResult.content['out/test1.out']
    expect(epc).not.toBeNull()
    expect(epc).toContain('validating')

    epc = genResult.content['type-by-cluster.h']
    expect(epc).toContain('enum item: c')
    expect(epc).toContain('Bitmap: ClusterBitmap')
    expect(epc).toContain('* First unused enum value for SparseEnum: 2')
    expect(epc).toContain('* First next larger enum value for SparseEnum: 4')

    epc = genResult.content['struct.h']
    expect(epc).toContain('Nest complex;// <- has nested array')
    expect(epc).toContain('// DoubleNest <- contains nested array')
    expect(epc).toContain('array;  // FABRIC SENSITIVE')
    expect(epc).toContain('// Struct is fabric-scoped')
    expect(epc).toContain('uint8_t IndexForFabricField')
    epc = genResult.content['access.out']
    expect(epc).not.toBeNull()
    expect(epc).toContain(
      '* Op: write / Role: manage / Modifier: fabric-scoped'
    )
    expect(epc).toContain('* Op:  / Role:  / Modifier: fabric-sensitive')
    expect(epc).toContain(
      '* Aggregates [3]: fScope=true/fSensitive=false/read=view/write=[operate - manage]/invoke=NONE'
    )
    expect(epc).toContain('HelloEvent is a fabric-sensitive event')
  },
  testUtil.timeout.medium()
)

test('Package options test', async () => {
  let uiOpts = await queryPackage.selectAllUiOptions(db, zclContext.packageId)
  expect(uiOpts.events).toBe('1')
  expect(uiOpts.header).toBe('Header text')
})
