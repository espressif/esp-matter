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

const axios = require('axios')
const dbApi = require('../src-electron/db/db-api.js')
const queryPackage = require('../src-electron/db/query-package.js')
const dbEnum = require('../src-shared/db-enum.js')
const env = require('../src-electron/util/env.ts')
const zclLoader = require('../src-electron/zcl/zcl-loader.js')
const httpServer = require('../src-electron/server/http-server.js')
const generationEngine = require('../src-electron/generator/generation-engine.js')
const testUtil = require('./test-util.js')
const testQuery = require('./test-query.js')
const util = require('../src-electron/util/util.js')

let db
const { port, baseUrl } = testUtil.testServer(__filename)
let uuid = util.createUuid()

beforeAll(async () => {
  env.setDevelopmentEnv()
  let file = env.sqliteTestFile('generation')
  db = await dbApi.initDatabaseAndLoadSchema(
    file,
    env.schemaFile(),
    env.zapVersion()
  )
}, testUtil.timeout.medium())

afterAll(async () => {
  await httpServer.shutdownHttpServer()
  await dbApi.closeDatabase(db)
}, testUtil.timeout.medium())

describe('Session specific tests', () => {
  test(
    'make sure there is no session at the beginning',
    async () => {
      let cnt = await testQuery.selectCountFrom(db, 'SESSION')
      expect(cnt).toBe(0)
    },
    testUtil.timeout.short()
  )

  test(
    'Now actually load the static data.',
    () => zclLoader.loadZcl(db, env.builtinSilabsZclMetafile()),
    testUtil.timeout.medium()
  )

  test(
    'And load the templates.',
    async () => {
      let context = await generationEngine.loadTemplates(
        db,
        testUtil.testTemplate.zigbee
      )
      let packageId = context.packageId
      expect(packageId).not.toBe(null)
      expect(db).not.toBe(null)

      // Test for cluster package extensions
      let extensions = await queryPackage.selectPackageExtension(
        db,
        packageId,
        dbEnum.packageExtensionEntity.cluster
      )

      expect(extensions.length).toBe(2)
      expect(extensions[0].entity).toBe(dbEnum.packageExtensionEntity.cluster)
      expect(extensions[0].property).toBe('testClusterExtension')
      expect(extensions[0].type).toBe('text')
      expect(extensions[0].configurability).toBe('hidden')
      expect(extensions[0].label).toBe('Test cluster extension')
      expect(extensions[0].globalDefault).toBe(null)
      expect(extensions[0].defaults.length).toBe(3)
      expect(extensions[1].label).toBe(
        'Test cluster extension with external defaults values'
      )
      expect(extensions[1].globalDefault).toBe(null)
      expect(extensions[1].defaults.length).toBe(2)
      expect(extensions[1].defaults[0].value).toBe(
        'Extension value loaded via external default JSON file.'
      )

      // Test for command package extensions
      extensions = await queryPackage.selectPackageExtension(
        db,
        packageId,
        dbEnum.packageExtensionEntity.command
      )

      expect(extensions.length).toBe(2)
      let tcIndex = 1
      let icIndex = 0
      expect(extensions[tcIndex].entity).toBe(
        dbEnum.packageExtensionEntity.command
      )
      expect(extensions[tcIndex].property).toBe('testCommandExtension')
      expect(extensions[tcIndex].type).toBe('boolean')
      expect(extensions[tcIndex].configurability).toBe('hidden')
      expect(extensions[tcIndex].label).toBe('Test command extension')
      expect(extensions[tcIndex].globalDefault).toBe('0')
      expect(extensions[tcIndex].defaults.length).toBe(1)

      // Test for attribute package extensions
      extensions = await queryPackage.selectPackageExtension(
        db,
        packageId,
        dbEnum.packageExtensionEntity.attribute
      )

      expect(extensions.length).toBe(2)
      expect(extensions[0].entity).toBe(dbEnum.packageExtensionEntity.attribute)
      expect(extensions[0].property).toBe('testAttributeExtension1')
      expect(extensions[1].property).toBe('testAttributeExtension2')
      expect(extensions[0].type).toBe('integer')
      expect(extensions[0].configurability).toBe('hidden')
      expect(extensions[0].label).toBe('Test attribute extension 1')
      expect(extensions[1].label).toBe('Test attribute extension 2')
      expect(extensions[0].globalDefault).toBe('0')
      expect(extensions[1].globalDefault).toBe('1')
      expect(extensions[0].defaults.length).toBe(2)
      expect(extensions[1].defaults.length).toBe(1)
      expect(extensions[0].defaults[0].value).toBe('42')
      expect(extensions[0].defaults[0].parentCode).toBe(0)
      expect(extensions[0].defaults[0].entityCode).toBe(0)
      expect(extensions[0].defaults[1].entityCode).toBe(1)

      // Test for attribute type package extensions
      extensions = await queryPackage.selectPackageExtension(
        db,
        packageId,
        dbEnum.packageExtensionEntity.attributeType
      )
      expect(extensions.length).toBe(1)

      // Test for command package extensions
      extensions = await queryPackage.selectPackageExtensionByPropertyAndEntity(
        db,
        packageId,
        'implementedCommands',
        dbEnum.packageExtensionEntity.command
      )
      expect(extensions.type).toBe('boolean')
      expect(extensions.globalDefault).toBe('0')
      expect(extensions.defaults.length).toBe(5)
    },
    testUtil.timeout.medium()
  )

  test(
    'http server initialization',
    () => httpServer.initHttpServer(db, port),
    testUtil.timeout.medium()
  )

  let templateCount = 0
  test(
    'test retrieval of all preview template files',
    async () => {
      let response = await axios.get(`${baseUrl}/preview/?sessionId=${uuid}`)
      templateCount = response.data['length']
      for (let i = 0; i < response.data['length']; i++) {
        expect(response.data[i]['version']).toBeDefined()
      }
    },
    testUtil.timeout.short()
  )

  test(
    'test that zcl extension is loaded and exists',
    async () => {
      let response = await axios.get(
        `${baseUrl}/zclExtension/cluster/testClusterExtension1?sessionId=${uuid}`
      )
      expect(response.data.entity).toBe('cluster')
      expect(response.data.property).toBe('testClusterExtension1')
      expect(response.data.label).toBe(
        'Test cluster extension with external defaults values'
      )
      expect(response.data.defaults[0].entityCode).toBe('0x0003')
      expect(response.data.defaults[0].value).toBe(
        'Extension value loaded via external default JSON file.'
      )
      expect(response.data.defaults[1].entityCode).toBe(
        'clusterCode mixed with strings'
      )
    },
    testUtil.timeout.medium()
  )

  test(
    'Load a second set of templates.',
    () => generationEngine.loadTemplates(db, testUtil.testTemplate.matter),
    testUtil.timeout.medium()
  )

  // Make sure all templates are loaded
  test(
    'Make sure second set of templates are loaded.',
    async () => {
      let pkgs = await queryPackage.getPackagesByType(
        db,
        dbEnum.packageType.genSingleTemplate
      )
      expect(templateCount).toBeLessThan(pkgs.length)
    },
    testUtil.timeout.medium()
  )

  test(
    'test retrieval of all preview template files make sure they are session aware',
    async () => {
      let response = await axios.get(`${baseUrl}/preview/?sessionId=${uuid}`)
      expect(templateCount).toEqual(response.data['length'])
    },
    testUtil.timeout.short()
  )

  test(
    'test that there is generation data in the simple-test.out preview file. Index 1',
    async () => {
      let response = await axios.get(
        `${baseUrl}/preview/simple-test.out/1?sessionId=${uuid}`
      )
      expect(response.data['result']).toMatch('Test template file.')
    },
    testUtil.timeout.short()
  )

  test(
    'No generation test, incorrect file name',
    async () => {
      let response = await axios.get(
        `${baseUrl}/preview/no-file?sessionId=${uuid}`
      )
      expect(response.data['result']).toBeUndefined()
    },
    testUtil.timeout.short()
  )

  test(
    'No generation test, incorrect file name and incorrect index',
    async () => {
      let response = await axios.get(
        `${baseUrl}/preview/no-file/1?sessionId=${uuid}`
      )
      expect(response.data['result']).toBeUndefined()
    },
    testUtil.timeout.short()
  )
})
