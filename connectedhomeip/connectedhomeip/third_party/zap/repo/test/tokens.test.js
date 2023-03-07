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
const utilJs = require('../src-electron/util/util.js')

let db
const testFile = path.join(__dirname, 'resource/tokens-test.zap')
// let sessionId
let templateContext

beforeAll(async () => {
  env.setDevelopmentEnv()
  let file = env.sqliteTestFile('tokens')
  db = await dbApi.initDatabaseAndLoadSchema(
    file,
    env.schemaFile(),
    env.zapVersion()
  )
  await zclLoader.loadZcl(db, env.builtinSilabsZclMetafile())
}, testUtil.timeout.medium())

afterAll(() => dbApi.closeDatabase(db), testUtil.timeout.short())

test(
  'Basic gen template parsing and generation',
  async () => {
    let context = await genEngine.loadTemplates(
      db,
      testUtil.testTemplate.zigbee
    )
    expect(context.crc).not.toBeNull()
    expect(context.templateData).not.toBeNull()
    expect(context.templateData.name).toEqual('Test templates')
    expect(context.templateData.version).toEqual('test-v1')
    expect(context.packageId).not.toBeNull()
    templateContext = context
  },
  testUtil.timeout.medium()
)

test(
  'Test file import',
  async () => {
    let importResult = await importJs.importDataFromFile(db, testFile)
    templateContext.sessionId = importResult.sessionId
    expect(importResult.sessionId).not.toBeNull()
  },
  testUtil.timeout.medium()
)

test(
  'Test tokens header generation',
  async () => {
    let genResult = await genEngine.generate(
      db,
      templateContext.sessionId,
      templateContext.packageId,
      {},
      { disableDeprecationWarnings: true }
    )

    expect(genResult).not.toBeNull()
    expect(genResult.partial).toBeFalsy()
    expect(genResult.content).not.toBeNull()

    let header = genResult.content['zap-tokens.h']

    // Singletons
    expect(header).toContain('#define CREATOR_STACK_VERSION_SINGLETON')

    expect(header).toContain(
      '#define NVM3KEY_STACK_VERSION_SINGLETON (NVM3KEY_DOMAIN_ZIGBEE'
    )

    expect(header).not.toContain('#define CREATOR_STACK_VERSION_1')
    expect(header).not.toContain(
      '#define NVM3KEY_STACK_VERSION_1 (NVM3KEY_DOMAIN_ZIGBEE'
    )

    expect(header).toContain('#define CREATOR_HW_VERSION_SINGLETON')
    expect(header).toContain(
      '#define NVM3KEY_HW_VERSION_SINGLETON (NVM3KEY_DOMAIN_ZIGBEE'
    )

    expect(header).not.toContain('#define CREATOR_HW_VERSION_1')
    expect(header).not.toContain(
      '#define NVM3KEY_HW_VERSION_1 (NVM3KEY_DOMAIN_ZIGBEE'
    )

    // Non-singletons

    expect(header).toContain('#define CREATOR_APPLICATION_VERSION_1')

    expect(header).toContain(
      '#define NVM3KEY_APPLICATION_VERSION_1 (NVM3KEY_DOMAIN_ZIGBEE'
    )

    expect(header).toContain('#define CREATOR_APPLICATION_VERSION_7')

    expect(header).toContain(
      '#define NVM3KEY_APPLICATION_VERSION_7 (NVM3KEY_DOMAIN_ZIGBEE'
    )

    expect(header).not.toContain(
      '#define CREATOR_APPLICATION_VERSION_SINGLETON'
    )
    expect(header).not.toContain(
      '#define NVM3KEY_APPLICATION_VERSION_SINGLETON (NVM3KEY_DOMAIN_ZIGBEE'
    )

    expect(header).toContain('#define CREATOR_PRODUCT_CODE_1')
    expect(header).toContain(
      '#define NVM3KEY_PRODUCT_CODE_1 (NVM3KEY_DOMAIN_ZIGBEE'
    )

    expect(header).not.toContain('#define CREATOR_PRODUCT_CODE_2')
    expect(header).not.toContain(
      '#define NVM3KEY_PRODUCT_CODE_2 (NVM3KEY_DOMAIN_ZIGBEE'
    )

    expect(header).not.toContain('#define CREATOR_PRODUCT_CODE_7')
    expect(header).not.toContain(
      '#define NVM3KEY_PRODUCT_CODE_7 (NVM3KEY_DOMAIN_ZIGBEE'
    )

    expect(header).not.toContain('#define CREATOR_COLOR_CONTROL_COLOR_MODE_1')

    expect(header).not.toContain(
      '#define NVM3KEY_COLOR_CONTROL_COLOR_MODE_1 (NVM3KEY_DOMAIN_ZIGBEE'
    )

    expect(header).not.toContain('#define CREATOR_COLOR_CONTROL_COLOR_MODE_2')

    expect(header).not.toContain(
      '#define NVM3KEY_COLOR_CONTROL_COLOR_MODE_2 (NVM3KEY_DOMAIN_ZIGBEE'
    )

    expect(header).toContain('#define CREATOR_COLOR_CONTROL_COLOR_MODE_7')

    expect(header).toContain(
      '#define NVM3KEY_COLOR_CONTROL_COLOR_MODE_7 (NVM3KEY_DOMAIN_ZIGBEE'
    )

    expect(header).toContain('#define CREATOR_LEVEL_CONTROL_REMAINING_TIME_7')

    expect(header).toContain(
      '#define NVM3KEY_LEVEL_CONTROL_REMAINING_TIME_7 (NVM3KEY_DOMAIN_ZIGBEE'
    )

    expect(header).toContain('#define CREATOR_COLOR_CONTROL_REMAINING_TIME_7')

    expect(header).toContain(
      '#define NVM3KEY_COLOR_CONTROL_REMAINING_TIME_7 (NVM3KEY_DOMAIN_ZIGBEE'
    )

    expect(header).not.toContain('#define CREATOR_REMAINING_TIME_')

    // Check token IDs
    expect(header).toContain('(NVM3KEY_DOMAIN_ZIGBEE | 0xB000)')
    expect(header).not.toContain('(NVM3KEY_DOMAIN_ZIGBEE | 0xB009)')

    // DEFINETYPES

    expect(header).toContain('typedef uint8_t tokType_stack_version;')

    expect(header).toContain('typedef uint8_t tokType_hw_version;')
    expect(header).toContain('typedef uint8_t tokType_product_code[16];')

    expect(header).toContain(
      'typedef uint16_t tokType_level_control_remaining_time;'
    )

    expect(header).toContain('uint16_t tokType_color_control_remaining_time;')

    // DEFINETOKENS

    expect(header).toContain(
      'DEFINE_BASIC_TOKEN(STACK_VERSION_SINGLETON, tokType_stack_version, 12)'
    )

    expect(header).toContain(
      'DEFINE_BASIC_TOKEN(HW_VERSION_SINGLETON, tokType_hw_version, 13)'
    )

    expect(header).toContain(
      'DEFINE_BASIC_TOKEN(APPLICATION_VERSION_1, tokType_application_version, 11)'
    )

    expect(header).toContain(
      'DEFINE_BASIC_TOKEN(APPLICATION_VERSION_7, tokType_application_version, 11)'
    )

    expect(header).toContain(
      "DEFINE_BASIC_TOKEN(PRODUCT_CODE_1, tokType_product_code, { 3, 'A', 'B', 'C', 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  })"
    )

    expect(header).toContain(
      'DEFINE_BASIC_TOKEN(LEVEL_CONTROL_REMAINING_TIME_7, tokType_level_control_remaining_time, 10)'
    )

    expect(header).toContain(
      'DEFINE_BASIC_TOKEN(COLOR_CONTROL_REMAINING_TIME_7, tokType_color_control_remaining_time, 0xA1B2)'
    )

    expect(header).toContain(
      'DEFINE_BASIC_TOKEN(COLOR_CONTROL_COLOR_MODE_7, tokType_color_control_color_mode, 1)'
    )

    // GENERATED_TOKEN_LOADER

    expect(header).toContain(
      'halCommonGetToken((tokType_stack_version *)ptr, TOKEN_STACK_VERSION_SINGLETON);'
    )

    expect(header).toContain(
      'emberAfWriteServerAttribute(1, ZCL_BASIC_CLUSTER_ID, ZCL_STACK_VERSION_ATTRIBUTE_ID, (uint8_t*)ptr, ZCL_INT8U_ATTRIBUTE_TYPE);'
    )

    expect(header).toContain(
      'halCommonGetToken((tokType_hw_version *)ptr, TOKEN_HW_VERSION_SINGLETON);'
    )

    expect(header).toContain(
      'emberAfWriteServerAttribute(1, ZCL_BASIC_CLUSTER_ID, ZCL_HW_VERSION_ATTRIBUTE_ID, (uint8_t*)ptr, ZCL_INT8U_ATTRIBUTE_TYPE);'
    )

    expect(header).not.toContain(
      'halCommonGetToken((tokType_stack_version *)ptr, TOKEN_STACK_VERSION_1);'
    )
    expect(header).not.toContain(
      'halCommonGetToken((tokType_hw_version *)ptr, TOKEN_HW_VERSION_1);'
    )

    expect(header).toContain(
      'if(1 == (endpoint) || (EMBER_BROADCAST_ENDPOINT == (endpoint) && epNetwork == curNetwork))'
    )

    expect(header).not.toContain(
      'if(2 == (endpoint) || (EMBER_BROADCAST_ENDPOINT == (endpoint) && epNetwork == curNetwork))'
    )
    expect(header).toContain(
      'if(7 == (endpoint) || (EMBER_BROADCAST_ENDPOINT == (endpoint) && epNetwork == curNetwork))'
    )

    expect(header).toContain(
      'halCommonGetToken((tokType_application_version *)ptr, TOKEN_APPLICATION_VERSION_1);'
    )

    expect(header).toContain(
      'halCommonGetToken((tokType_application_version *)ptr, TOKEN_APPLICATION_VERSION_7);'
    )

    expect(header).toContain(
      'emberAfWriteServerAttribute(1, ZCL_BASIC_CLUSTER_ID, ZCL_APPLICATION_VERSION_ATTRIBUTE_ID, (uint8_t*)ptr, ZCL_INT8U_ATTRIBUTE_TYPE);'
    )

    // GENERATED_TOKEN_SAVER

    expect(header).toContain('if ( 0x0000 == clusterId )')
    expect(header).not.toContain('if ( 0x0001 == clusterId )')
    expect(header).toContain(
      'if ( 0x0002 == metadata->attributeId && 0x0000 == emberAfGetMfgCode(metadata) && !emberAfAttributeIsClient(metadata) ) {'
    )

    expect(header).toContain(
      'halCommonSetToken(TOKEN_STACK_VERSION_SINGLETON, data); }'
    )

    expect(header).toContain(
      'if ( 0x0003 == metadata->attributeId && 0x0000 == emberAfGetMfgCode(metadata) && !emberAfAttributeIsClient(metadata) )'
    )

    expect(header).toContain(
      'halCommonSetToken(TOKEN_HW_VERSION_SINGLETON, data); }'
    )

    expect(header).toContain('if ( 1 == endpoint )')
    expect(header).not.toContain('if ( 2 == endpoint )')
    expect(header).toContain('if ( 7 == endpoint )')
    expect(header).toContain(
      'if ( 0x0001 == metadata->attributeId && 0x0000 == emberAfGetMfgCode(metadata) && !emberAfAttributeIsClient(metadata) )'
    )

    expect(header).toContain(
      'halCommonSetToken(TOKEN_APPLICATION_VERSION_1, data);'
    )

    expect(header).toContain(
      'halCommonSetToken(TOKEN_APPLICATION_VERSION_7, data);'
    )
  },
  testUtil.timeout.long()
)
