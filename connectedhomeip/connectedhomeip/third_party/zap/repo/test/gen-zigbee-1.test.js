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
const queryPackage = require('../src-electron/db/query-package.js')
const querySession = require('../src-electron/db/query-session.js')
const utilJs = require('../src-electron/util/util.js')
const zclLoader = require('../src-electron/zcl/zcl-loader.js')
const helperZap = require('../src-electron/generator/helper-zap.js')
const importJs = require('../src-electron/importexport/import.js')
const testUtil = require('./test-util.js')

let db
const templateCount = testUtil.testTemplate.zigbeeCount
const testFile = path.join(__dirname, 'resource/generation-test-file-1.zap')
const testFile5 = path.join(__dirname, 'resource/gp-combo-basic-test.zap')
const testFile6 = path.join(__dirname, 'resource/mfgSpecificConfig.zap')

beforeAll(async () => {
  env.setDevelopmentEnv()
  let file = env.sqliteTestFile('genzigbee1')
  db = await dbApi.initDatabaseAndLoadSchema(
    file,
    env.schemaFile(),
    env.zapVersion()
  )
  return zclLoader.loadZcl(db, env.builtinSilabsZclMetafile())
}, testUtil.timeout.medium())

afterAll(() => dbApi.closeDatabase(db), testUtil.timeout.short())

let templateContext

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
    expect(context.templateData.templates.length).toEqual(templateCount)
    expect(context.packageId).not.toBeNull()
    templateContext = context
  },
  testUtil.timeout.medium()
)

test(
  'Validate package loading',
  async () => {
    templateContext.packages = await queryPackage.getPackageByParent(
      templateContext.db,
      templateContext.packageId
    )
    expect(templateContext.packages.length).toBe(templateCount - 1 + 2) // -1 for ignored one, one for helper and one for overridable
  },
  testUtil.timeout.short()
)

test(
  'Create session',
  async () => {
    let sessionId = await querySession.createBlankSession(db)
    expect(sessionId).not.toBeNull()
    templateContext.sessionId = sessionId
  },
  testUtil.timeout.short()
)

test(
  'Initialize session packages',
  async () => {
    let packages = await utilJs.initializeSessionPackage(
      templateContext.db,
      templateContext.sessionId,
      {
        zcl: env.builtinSilabsZclMetafile(),
        template: env.builtinTemplateMetafile(),
      }
    )

    expect(packages.length).toBe(2)
  },
  testUtil.timeout.short()
)

test(
  'Validate basic generation',
  async () => {
    let genResult = await genEngine.generate(
      templateContext.db,
      templateContext.sessionId,
      templateContext.packageId,
      {},
      { disableDeprecationWarnings: true }
    )
    expect(genResult).not.toBeNull()
    expect(genResult.partial).toBeFalsy()
    expect(genResult.content).not.toBeNull()
    let simpleTest = genResult.content['simple-test.out']
    expect(simpleTest.startsWith('Test template file.')).toBeTruthy()
    expect(simpleTest).toContain('Strange type: bacnet_type_t')
  },
  testUtil.timeout.long()
)

test(
  'Validate more complex generation',
  async () => {
    let genResult = await genEngine.generate(
      templateContext.db,
      templateContext.sessionId,
      templateContext.packageId,
      {},
      { disableDeprecationWarnings: true }
    )

    expect(genResult).not.toBeNull()
    expect(genResult.partial).toBeFalsy()
    expect(genResult.content).not.toBeNull()
    let simpleTest = genResult.content['simple-test.out']
    expect(simpleTest.startsWith('Test template file.')).toBeTruthy()
    expect(simpleTest).toContain(helperZap.zap_header())
    expect(simpleTest).toContain(`SessionId: ${genResult.sessionId}`)
    expect(simpleTest).toContain('Addon: This is example of test addon helper')

    let zclId = genResult.content['zcl-test.out']
    //expect(zclId).toEqual('random placeholder')
    expect(zclId).toContain(
      `// ${testUtil.totalNonAtomicEnumCount - 1}/${
        testUtil.totalNonAtomicEnumCount
      }: label=>ZllStatus caption=>Enum of size 1 byte`
    )
    expect(zclId).toContain(`Label count: ${testUtil.totalNonAtomicEnumCount}`)
    expect(zclId).toContain(
      `// 129/${testUtil.totalNonAtomicEnumCount}: label=>MeteringBlockEnumerations caption=>Enum of size 1 byte`
    )
    expect(zclId).toContain(
      '// struct: ReadReportingConfigurationAttributeRecord'
    )
    expect(zclId).toContain('cluster: 0x0700 Price')
    expect(zclId).toContain('cmd: 0x0A GetUserStatusResponse')
    expect(zclId).toContain('att: 0x0002 gps communication mode')
    expect(zclId).toContain('First item\n// struct: BlockThreshold')
    expect(zclId).toContain('// struct: WwahClusterStatusToUseTC\nLast item')
    expect(zclId).toContain('// event: 0x0001 HelloEvent')
    expect(zclId).toContain('-> field: 0x0002 arg2 INT32U')

    let accumulator = genResult.content['accumulator.out']
    expect(accumulator).toContain('Iteration: 19 out of 20')
    expect(accumulator).toContain('Cumulative size: 16 / 206')
    expect(accumulator).toContain('Cumulative size: 8 / 109')
    expect(accumulator).toContain('Cumulative size: 0 / 206')

    let atomics = genResult.content['atomics.out']
    expect(atomics).toContain('C type: bacnet_type_t')
    // Now check for the override
    //expect(atomics).toContain('C type: security_key_type_override')

    let zapCommand = genResult.content['zap-command.h']
    expect(zapCommand).not.toBeNull()
    expect(zapCommand).toContain(
      '#define emberAfFillCommandGlobalReadAttributesResponse(clusterId,'
    )

    let zapPrint = genResult.content['zap-print.h']
    expect(zapPrint).toContain(
      '#define SILABS_PRINTCLUSTER_POWER_CONFIG_CLUSTER {ZCL_POWER_CONFIG_CLUSTER_ID, 0x0000, "Power Configuration" },'
    )

    let sdkExtension = genResult.content['sdk-extension.out']
    expect(sdkExtension).toContain(
      "// cluster: 0x0000 Basic, text extension: 'Extension to basic cluster'"
    )
    expect(sdkExtension).toContain(
      "// cluster: 0x0002 Device Temperature Configuration, text extension: 'Extension to temperature config cluster'"
    )
    expect(sdkExtension).toContain(
      "// server cluster: 0x0001 Power Configuration, text extension: 'Extension to power cluster'"
    )
    expect(sdkExtension).toContain(
      "// client cluster: 0x0001 Power Configuration, text extension: ''"
    )
    expect(sdkExtension).toContain(
      "// attribute: 0x0000 / 0x0000 => ZCL version, extensions: '42', '99'"
    )
    expect(sdkExtension).toContain(
      "attribute: 0x0015 / 0x0015 => network key sequence number, extensions: '0', '1', [int8u:666]"
    )
    expect(sdkExtension).toContain(
      "// cluster: 0x0003 Identify, text extension: ''"
    )
    expect(sdkExtension).toContain(
      "// command: 0x0000 / 0x00 => ResetToFactoryDefaults, test extension: '1'"
    )
    expect(sdkExtension).toContain(
      "// device type: HA / 0x0006 => HA-remote // extension: 'path/to/remote.c'"
    )
    expect(sdkExtension).toContain(
      'IMPLEMENTED_COMMANDS>ResetToFactoryDefaults,IdentifyQueryResponse,IdentifyQuery,EZModeInvoke,UpdateCommissionState,<END'
    )
    // Testing {{#if_is_struct}} helper
    expect(zclId).toContain(`attributeIds is not struct`)
    // Testing {{#if_command_discovery_enabled}} helper
    expect(zclId).toContain(`#define EMBER_AF_SUPPORT_COMMAND_DISCOVERY`)
    // Testing {{#zcl_struct_items_by_struct_name}} helper
    expect(zclId).toContain(`configureReportingRecords::direction struct item`)
    expect(zclId).toContain(`readAttributeStatusRecords is struct`)
  },
  testUtil.timeout.long()
)

test(
  'Validate test file 1 generation',
  async () => {
    let genResult = await genEngine.generate(
      templateContext.db,
      templateContext.sessionId,
      templateContext.packageId,
      {},
      { disableDeprecationWarnings: true }
    )

    expect(genResult).not.toBeNull()
    expect(genResult.partial).toBeFalsy()
    expect(genResult.content).not.toBeNull()

    let zapId = genResult.content['zap-id.h']
    //expect(zapId).toEqual('random placeholder')

    expect(zapId).toContain('// Definitions for cluster: Basic')
    expect(zapId).toContain(
      '#define ZCL_GET_PROFILE_RESPONSE_COMMAND_ID (0x00)'
    )
    // Testing {{#zcl_commands_source_client}} helper
    expect(zapId).toContain(
      '#define ZCL_IDENTIFY_C_TO_S_IDENTIFY_QUERY_COMMAND_ID (0x01)'
    )
    // Testing {{#zcl_commands_source_server}} helper
    expect(zapId).toContain(
      '#define ZCL_IDENTIFY_S_TO_C_IDENTIFY_QUERY_RESPONSE_COMMAND_ID (0x00)'
    )
    expect(zapId).toContain(
      '// Client attributes for cluster: Fluoride Concentration Measurement'
    )
    expect(zapId).toContain(
      '#define ZCL_NUMBER_OF_RESETS_ATTRIBUTE_ID (0x0000)'
    )
    let zapTypes = genResult.content['zap-type.h']
    expect(zapTypes).toContain(
      'ZCL_INT16U_ATTRIBUTE_TYPE = 0x21, // Unsigned 16-bit integer'
    )
    expect(zapTypes).toContain('uint32_t snapshotCause')
    expect(zapTypes).toContain('typedef uint8_t EphemeralData;')

    let zapCommandParser = genResult.content['zap-command-parser.c']
    expect(zapCommandParser).not.toBeNull()
    expect(zapCommandParser).toContain(
      'EmberAfStatus emberAfClusterSpecificCommandParse(EmberAfClusterCommand * cmd)'
    )
  },
  testUtil.timeout.long()
)

test(
  'Test file 1 generation',
  async () => {
    let sid = await querySession.createBlankSession(db)
    let loaderResult = await importJs.importDataFromFile(db, testFile, {
      sessionId: sid,
    })

    let genResult = await genEngine.generate(
      db,
      sid,
      loaderResult.templateIds[0],
      {},
      {
        disableDeprecationWarnings: true,
      }
    )
    expect(genResult).not.toBeNull()
    expect(genResult.partial).toBeFalsy()
    expect(genResult.content).not.toBeNull()

    let cfgVer2 = genResult.content['zap-config-version-2.h']
    // Test GENERATED_DEFAULTS
    expect(cfgVer2).toContain(
      '0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  /* 0,DEFAULT value for cluster: Over the Air Bootloading, attribute: OTA Upgrade Server ID, side: client*/'
    )
    // Test GENERATED_ATTRIBUTE_COUNT
    expect(cfgVer2).toContain('#define GENERATED_ATTRIBUTE_COUNT 81')
    // Test GENERATED_ATTRIBUTES
    expect(cfgVer2).toContain(
      '{ 0x000F, ZCL_BITMAP8_ATTRIBUTE_TYPE, 1, (ATTRIBUTE_MASK_WRITABLE), { (uint8_t*)0x00  } }, /* 46 Cluster: Color Control, Attribute: color control options, Side: server*/'
    )
    // Test is_number_greater_than within GENERATED_ATTRIBUTES
    expect(cfgVer2).toContain(
      '{ 0x0000, ZCL_IEEE_ADDRESS_ATTRIBUTE_TYPE, 8, (ATTRIBUTE_MASK_CLIENT), { (uint8_t*)&(generatedDefaults[0]) } }, /* 35 Cluster: Over the Air Bootloading, Attribute: OTA Upgrade Server ID, Side: client*/'
    )
    // Test GENERATED_CLUSTER_COUNT
    expect(cfgVer2).toContain('#define GENERATED_CLUSTER_COUNT 18')
    // Test GENERATED_CLUSTERS
    expect(cfgVer2).toContain(
      '0x0019, (EmberAfAttributeMetadata*)&(generatedAttributes[35]), 4, 15, CLUSTER_MASK_CLIENT, NULL }, /* 15, Endpoint Id: 2, Cluster: Over the Air Bootloading, Side: client*/'
    )
    // Test GENERATED_ENDPOINT_TYPE_COUNT
    expect(cfgVer2).toContain('#define GENERATED_ENDPOINT_TYPE_COUNT (2)')
    // Test GENERATED_ENDPOINT_TYPES
    expect(cfgVer2).toContain(
      '{ ((EmberAfCluster*)&(generatedClusters[0])), 9, 50 },'
    )
    // Test ATTRIBUTE_LARGEST
    expect(cfgVer2).toContain('#define ATTRIBUTE_LARGEST (65)')
    // Test ATTRIBUTE_SINGLETONS_SIZE
    expect(cfgVer2).toContain('#define ATTRIBUTE_SINGLETONS_SIZE (191)')
    // Test ATTRIBUTE_MAX_SIZE
    expect(cfgVer2).toContain('#define ATTRIBUTE_MAX_SIZE (164)')
    // Test FIXED_ENDPOINT_COUNT
    expect(cfgVer2).toContain('#define FIXED_ENDPOINT_COUNT (2)')
    // Test EMBER_AF_GENERATED_COMMAND_COUNT
    expect(cfgVer2).toContain('#define EMBER_AF_GENERATED_COMMAND_COUNT  (88)')
    // Test GENERATED_COMMANDS
    expect(cfgVer2).toContain(
      '{ 0x0004, 0x01, COMMAND_MASK_OUTGOING_SERVER }, /* 7, Cluster: Groups, Command: ViewGroupResponse*/'
    )
  },
  testUtil.timeout.long()
)

test(
  'Test generated defaults and cli',
  async () => {
    let sid = await querySession.createBlankSession(db)
    await importJs.importDataFromFile(db, testFile5, { sessionId: sid })

    let genResult = await genEngine.generate(
      db,
      sid,
      templateContext.packageId,
      {},
      {
        disableDeprecationWarnings: true,
      }
    )

    expect(genResult).not.toBeNull()
    expect(genResult.partial).toBeFalsy()
    expect(genResult.content).not.toBeNull()
    let cfgVer2 = genResult.content['zap-config-version-2.h']

    // Test GENERATED_DEFAULTS big endian
    expect(cfgVer2).toContain(
      '0x0F, 0xAE, 0x2F, /* 0,DEFAULT value for cluster: Green Power, attribute: gps functionality, side: server */'
    )

    // Test GENERATED_DEFAULTS little endian
    expect(cfgVer2).toContain(
      '0x2F, 0xAE, 0x0F,  /* 0,DEFAULT value for cluster: Green Power, attribute: gps functionality, side: server*/'
    )

    // Test GENERATED_DEFAULTS big endian for attribute of size > 8
    expect(cfgVer2).toContain(
      '0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0F, 0x0F, 0x0F, /* 6,DEFAULT value for cluster: Green Power, attribute: gp link key, side: server */'
    )

    // Test GENERATED_DEFAULTS for same attribute name but different side of cluster, compare it to above test
    expect(cfgVer2).toContain(
      '0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0F, 0x0F, 0x0F, /* 28,DEFAULT value for cluster: Green Power, attribute: gp link key, side: client */'
    )

    // Test GENERATED_DEFAULTS little endian for attribute of size > 8 is same as big endian. Bytes are not inverted
    expect(cfgVer2).not.toContain(
      `0x0F, 0x0F, 0x0F, 0x0D, 0x0C, 0x0B, 0x0A, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, /* 12,DEFAULT value for cluster: Green Power, attribute: gp link key, side: client*/`
    )

    // Testing GENERATED ATTRIBUTES to see that they are refering to the correct generation defaults
    expect(cfgVer2).toContain(
      `0x0022, ZCL_SECURITY_KEY_ATTRIBUTE_TYPE, 16, (ATTRIBUTE_MASK_WRITABLE| ATTRIBUTE_MASK_CLIENT), { (uint8_t*)&(generatedDefaults[28]) } }, /* 25 Cluster: Green Power, Attribute: gp link key, Side: client*/`
    )
    expect(cfgVer2).toContain(
      `0x0022, ZCL_SECURITY_KEY_ATTRIBUTE_TYPE, 16, (ATTRIBUTE_MASK_WRITABLE), { (uint8_t*)&(generatedDefaults[6]) } }, /* 37 Cluster: Green Power, Attribute: gp link key, Side: server*/`
    )

    // Test EMBER_AF_GENERATED_REPORTING_CONFIG_DEFAULTS to see that it generates reporting for singleton attributes correctly
    // This test makes sure the reporting default generates only once for a singleton attribute and not per endpoint.
    // In this case: Basic Server Cluster, ZCL version is enabled on enpoint 2 and 242
    expect(cfgVer2).toContain(`EMBER_AF_GENERATED_REPORTING_CONFIG_DEFAULTS`)
    expect(cfgVer2).toContain(
      `{ EMBER_ZCL_REPORTING_DIRECTION_REPORTED, 0x0002, 0x0000, 0x0000, CLUSTER_MASK_SERVER, 0x0000, 0, 65534, 0 }, /* Endpoint Id: 2, Cluster: Basic, Attribute: ZCL version */`
    )
    expect(cfgVer2).not.toContain(
      `{ EMBER_ZCL_REPORTING_DIRECTION_REPORTED, 0x0001, 0x0000, 0x0000, CLUSTER_MASK_SERVER, 0x0000, 0, 65534, 0 }, /* Endpoint Id: 1, Cluster: Basic, Attribute: ZCL version */`
    )
    expect(cfgVer2).not.toContain(
      `{ EMBER_ZCL_REPORTING_DIRECTION_REPORTED, 0x00F2, 0x0000, 0x0000, CLUSTER_MASK_SERVER, 0x0000, 0, 65534, 0 }, /* Endpoint Id: 242, Cluster: Basic, Attribute: ZCL version */`
    )

    // Testing zap cli helpers
    expect(genResult.content['zap-cli.c']).toContain(
      'static const sl_cli_command_entry_t zcl_identify_cluster_command_table[]'
    )

    expect(genResult.content['zap-cli.c']).toContain(
      'static const sl_cli_command_info_t cli_cmd_identify_group'
    )
    expect(genResult.content['zap-cli.c']).toContain(
      'SL_CLI_COMMAND_GROUP(zcl_identify_cluster_command_table, "ZCL identify cluster commands");'
    )
    expect(genResult.content['zap-cli.c']).toContain(
      '{ "identify", &cli_cmd_identify_group, false },'
    )
  },
  testUtil.timeout.long()
)

test(
  'Test zap config for mfg specific content',
  async () => {
    let sid = await querySession.createBlankSession(db)
    await importJs.importDataFromFile(db, testFile6, { sessionId: sid })

    let genResult = await genEngine.generate(
      db,
      sid,
      templateContext.packageId,
      {},
      {
        disableDeprecationWarnings: true,
      }
    )

    expect(genResult).not.toBeNull()
    expect(genResult.partial).toBeFalsy()
    expect(genResult.content).not.toBeNull()
    let cfgVer3 = genResult.content['zap-config-version-3.h']

    // Test GENERATED_ATTRIBUTES for the same attribute name but different attribute code
    expect(cfgVer3).toContain(
      '{ 0x0000, ZCL_INT16U_ATTRIBUTE_TYPE, 2, (ATTRIBUTE_MASK_WRITABLE), { (uint8_t*)0x0000  } }, /* 51 Cluster: Sample Mfg Specific Cluster 2, Attribute: ember sample attribute 2, Side: server*/'
    )

    expect(cfgVer3).toContain(
      '{ 0x0001, ZCL_INT16U_ATTRIBUTE_TYPE, 2, (ATTRIBUTE_MASK_WRITABLE), { (uint8_t*)0x0000  } }, /* 52 Cluster: Sample Mfg Specific Cluster 2, Attribute: ember sample attribute 2, Side: server*/'
    )

    // Test GENERATED_CLUSTERS for attribute index and size on endpoint 1 and endpoint 2
    expect(cfgVer3).toContain(
      '{ 0xFC00, (EmberAfAttributeMetadata*)&(generatedAttributes[43]), 2, 3, CLUSTER_MASK_CLIENT, NULL }, /* 9, Endpoint Id: 1, Cluster: Sample Mfg Specific Cluster, Side: client*/'
    )

    expect(cfgVer3).toContain(
      '{ 0xFC00, (EmberAfAttributeMetadata*)&(generatedAttributes[45]), 4, 5, CLUSTER_MASK_SERVER, NULL }, /* 10, Endpoint Id: 1, Cluster: Sample Mfg Specific Cluster, Side: server*/'
    )

    expect(cfgVer3).toContain(
      '{ 0xFC00, (EmberAfAttributeMetadata*)&(generatedAttributes[43]), 2, 3, CLUSTER_MASK_CLIENT, NULL }, /* 15, Endpoint Id: 2, Cluster: Sample Mfg Specific Cluster, Side: client*/'
    )

    expect(cfgVer3).toContain(
      '{ 0xFC00, (EmberAfAttributeMetadata*)&(generatedAttributes[49]), 2, 3, CLUSTER_MASK_CLIENT, NULL }, /* 16, Endpoint Id: 2, Cluster: Sample Mfg Specific Cluster 2, Side: client*/'
    )

    expect(cfgVer3).toContain(
      '{ 0xFC00, (EmberAfAttributeMetadata*)&(generatedAttributes[45]), 4, 5, CLUSTER_MASK_SERVER, NULL }, /* 17, Endpoint Id: 2, Cluster: Sample Mfg Specific Cluster, Side: server*/'
    )

    expect(cfgVer3).toContain(
      '{ 0xFC00, (EmberAfAttributeMetadata*)&(generatedAttributes[51]), 3, 5, CLUSTER_MASK_SERVER, NULL } /* 18, Endpoint Id: 2, Cluster: Sample Mfg Specific Cluster 2, Side: server*/'
    )

    // Test GENERATED_COMMANDS and its mask
    expect(cfgVer3).toContain(
      '{ 0xFC00, 0x00, COMMAND_MASK_INCOMING_SERVER | COMMAND_MASK_OUTGOING_CLIENT | COMMAND_MASK_MANUFACTURER_SPECIFIC }, /* 69, Cluster: Sample Mfg Specific Cluster, Command: CommandOne*/'
    )

    expect(cfgVer3).toContain(
      '{ 0xFC00, 0x00, COMMAND_MASK_INCOMING_SERVER | COMMAND_MASK_OUTGOING_CLIENT | COMMAND_MASK_MANUFACTURER_SPECIFIC }, /* 70, Cluster: Sample Mfg Specific Cluster 2, Command: CommandTwo*/'
    )

    // Test GENERATED_COMMAND_MANUFACTURER_CODES
    expect(cfgVer3).toContain('{ 69, 0x1002 },')

    expect(cfgVer3).toContain('{ 70, 0x1049 },')

    // Test GENERATED_CLUSTER_MANUFACTURER_CODES
    expect(cfgVer3).toContain('{ 9, 0x1002 },')
    expect(cfgVer3).toContain('{ 10, 0x1002 },')
    expect(cfgVer3).toContain('{ 15, 0x1002 },')
    expect(cfgVer3).toContain('{ 16, 0x1049 },')
    expect(cfgVer3).toContain('{ 17, 0x1002 },')
    expect(cfgVer3).toContain('{ 18, 0x1049 },')

    // Test GENERATED_ATTRIBUTE_MANUFACTURER_CODES, global attributes do not show up here
    expect(cfgVer3).toContain('{ 45, 0x1002 },')
    expect(cfgVer3).toContain('{ 46, 0x1002 },')
    expect(cfgVer3).toContain('{ 51, 0x1049 },')
    expect(cfgVer3).toContain('{ 52, 0x1049 },')
  },
  testUtil.timeout.long()
)

test(
  'Test content indexer - simple',
  async () => {
    let preview = await genEngine.contentIndexer('Short example')
    expect(preview['1']).toBe('Short example\n')
  },
  testUtil.timeout.short()
)

test(
  'Test content indexer - line by line',
  async () => {
    let preview = await genEngine.contentIndexer(
      'Short example\nwith three\nlines of text',
      1
    )
    expect(preview['1']).toBe('Short example\n')
    expect(preview['2']).toBe('with three\n')
    expect(preview['3']).toBe('lines of text\n')
  },
  testUtil.timeout.short()
)

test(
  'Test content indexer - blocks',
  async () => {
    let content = ''
    let i = 0
    for (i = 0; i < 1000; i++) {
      content = content.concat(`line ${i}\n`)
    }
    let preview = await genEngine.contentIndexer(content, 50)
    expect(preview['1'].startsWith('line 0')).toBeTruthy()
    expect(preview['2'].startsWith('line 50')).toBeTruthy()
    expect(preview['3'].startsWith('line 100')).toBeTruthy()
    expect(preview['20'].startsWith('line 950')).toBeTruthy()
  },
  testUtil.timeout.short()
)
