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
const queryEndpoint = require('../src-electron/db/query-endpoint.js')
const queryEndpointType = require('../src-electron/db/query-endpoint-type.js')
const queryConfig = require('../src-electron/db/query-config.js')
const queryPackage = require('../src-electron/db/query-package.js')
const types = require('../src-electron/util/types.js')
const bin = require('../src-electron/util/bin.ts')

let db
const templateCount = testUtil.testTemplate.zigbeeCount
const testFile = path.join(__dirname, 'resource/three-endpoint-device.zap')
let sessionId
let templateContext
let zclContext

beforeAll(async () => {
  env.setDevelopmentEnv()
  let file = env.sqliteTestFile('endpointconfig')
  db = await dbApi.initDatabaseAndLoadSchema(
    file,
    env.schemaFile(),
    env.zapVersion()
  )
}, testUtil.timeout.medium())

afterAll(() => dbApi.closeDatabase(db), testUtil.timeout.short())

test(
  'Basic gen template parsing and generation',
  async () => {
    templateContext = await genEngine.loadTemplates(
      db,
      testUtil.testTemplate.zigbee
    )
    expect(templateContext.crc).not.toBeNull()
    expect(templateContext.templateData).not.toBeNull()
    expect(templateContext.templateData.name).toEqual('Test templates')
    expect(templateContext.templateData.version).toEqual('test-v1')
    expect(templateContext.templateData.templates.length).toEqual(templateCount)
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
  'Test file import',
  async () => {
    let importResult = await importJs.importDataFromFile(db, testFile)
    sessionId = importResult.sessionId
    expect(sessionId).not.toBeNull()
    await queryPackage.insertSessionPackage(db, sessionId, zclContext.packageId)
  },
  testUtil.timeout.medium()
)

test(
  'Test endpoint config queries',
  async () => {
    let epts = await queryEndpointType.selectAllEndpointTypes(db, sessionId)

    expect(epts.length).toBe(3)
    let ps = []
    epts.forEach((ept) => {
      ps.push(queryEndpoint.selectEndpointClusters(db, ept.id))
    })
    let clusterArray = await Promise.all(ps)

    expect(clusterArray.length).toBe(3)
    expect(clusterArray[0].length).toBe(28)
    expect(clusterArray[1].length).toBe(5)
    expect(clusterArray[2].length).toBe(7)
    let promiseAttributes = []
    let promiseCommands = []
    clusterArray.forEach((clusters) => {
      clusters.forEach((cluster) => {
        promiseAttributes.push(
          queryEndpoint.selectEndpointClusterAttributes(
            db,
            cluster.clusterId,
            cluster.side,
            cluster.endpointTypeId
          )
        )
        promiseCommands.push(
          queryEndpoint.selectEndpointClusterCommands(
            db,
            cluster.clusterId,
            cluster.endpointTypeId
          )
        )
      })
    })
    let twoLists = await Promise.all([
      Promise.all(promiseAttributes),
      Promise.all(promiseCommands),
    ])

    let attributeLists = twoLists[0]
    let commandLists = twoLists[1]
    expect(attributeLists.length).toBe(40)
    expect(commandLists.length).toBe(40)

    let atSums = {}
    attributeLists.forEach((al) => {
      let l = al.length
      if (atSums[l]) {
        atSums[l]++
      } else {
        atSums[l] = 1
      }
    })
    expect(atSums[0]).toBe(18)

    let cmdSums = {}
    commandLists.forEach((cl) => {
      let l = cl.length
      if (cmdSums[l]) {
        cmdSums[l]++
      } else {
        cmdSums[l] = 1
      }
    })
    expect(cmdSums[0]).toBe(15)
  },
  testUtil.timeout.medium()
)

test(
  'Some intermediate queries',
  async () => {
    let size = await types.typeSize(db, zclContext.packageId, 'bitmap8')
    expect(size).toBe(1)
  },
  testUtil.timeout.medium()
)

test(
  'Test endpoint config generation',
  async () => {
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
    if (genResult.hasErrors) {
      console.log(genResult.errors)
    }
    expect(genResult.hasErrors).toBeFalsy()

    let epc = genResult.content['zap-config.h']
    let epcLines = epc.split(/\r?\n/)
    expect(epc).toContain(
      '#define FIXED_ENDPOINT_ARRAY { 0x0029, 0x002A, 0x002B }'
    )
    expect(epc).toContain(
      "17, 'V', 'e', 'r', 'y', ' ', 'l', 'o', 'n', 'g', ' ', 'u', 's', 'e', 'r', ' ', 'i', 'd',"
    )
    expect(epc).toContain(
      '{ ZAP_REPORT_DIRECTION(REPORTED), 0x0029, 0x00000101, 0x00000000, ZAP_CLUSTER_MASK(SERVER), 0x0000, {{ 0, 65534, 0 }} }, /* lock state */'
    )
    expect(epc).toContain(
      '{ 0x00000004, ZAP_TYPE(CHAR_STRING), 33, ZAP_ATTRIBUTE_MASK(TOKENIZE), ZAP_LONG_DEFAULTS_INDEX(0) }'
    )
    expect(epc.includes(bin.hexToCBytes(bin.stringToHex('Very long user id'))))
    expect(epc).toContain('#define FIXED_NETWORKS { 1, 1, 2 }')
    expect(epc).toContain(
      '#define FIXED_PROFILE_IDS { 0x0107, 0x0104, 0x0104 }'
    )
    expect(epc).toContain('#define FIXED_ENDPOINT_TYPES { 0, 1, 2 }')
    expect(epc).toContain('#define GENERATED_DEFAULTS_COUNT (12)')
    expect(epc).toContain(
      `17, 'T', 'e', 's', 't', ' ', 'm', 'a', 'n', 'u', 'f', 'a', 'c', 't', 'u', 'r', 'e', 'r',`
    )
    expect(epcLines.length).toBeGreaterThan(100)
    let cnt = 0
    epcLines.forEach((line) => {
      if (line.includes('ZAP_TYPE(')) {
        expect(line.includes('undefined')).toBeFalsy()
        cnt++
      }
    })
    expect(cnt).toBe(72)
    expect(epc).toContain('#define EMBER_AF_MANUFACTURER_CODE 0x1002')
    expect(epc).toContain('#define EMBER_AF_DEFAULT_RESPONSE_POLICY_ALWAYS')
  },
  testUtil.timeout.long()
)
