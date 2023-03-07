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
const importJs = require('../src-electron/importexport/import.js')
const testUtil = require('./test-util.js')

let db
const templateCount = testUtil.testTemplate.zigbeeCount
const testFile2 = path.join(__dirname, 'resource/three-endpoint-device.zap')
const testFile3 = path.join(__dirname, 'resource/zll-on-off-switch-test.zap')
const testFile4 = path.join(
  __dirname,
  'resource/mfg-specific-clusters-commands.zap'
)

beforeAll(async () => {
  env.setDevelopmentEnv()
  let file = env.sqliteTestFile('genzigbee2')
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
  'Test file 2 generation',
  async () => {
    let { sessionId, errors, warnings } = await importJs.importDataFromFile(
      db,
      testFile2
    )

    expect(errors.length).toBe(0)
    expect(warnings.length).toBe(0)
    let genResult = await genEngine.generate(
      db,
      sessionId,
      templateContext.packageId,
      {},
      {
        disableDeprecationWarnings: true,
      }
    )

    expect(genResult.hasErrors).toBeFalsy()
    expect(genResult).not.toBeNull()
    expect(genResult.partial).toBeFalsy()
    expect(genResult.content).not.toBeNull()
    let sdkExtension = genResult.content['sdk-extension.out']
    expect(sdkExtension).not.toBeNull()
    expect(
      sdkExtension.includes(
        'IMPLEMENTED_COMMANDS2>IdentifyQueryResponse,IdentifyQuery,<END2'
      )
    ).toBeTruthy()
  },
  testUtil.timeout.long()
)

test(
  'Testing zap command parser generation',
  async () => {
    let sid = await querySession.createBlankSession(db)
    await importJs.importDataFromFile(db, testFile3, { sessionId: sid })

    let genResult = await genEngine.generate(
      db,
      sid,
      templateContext.packageId,
      {},
      {
        disableDeprecationWarnings: true,
      }
    )

    let pv3 = genResult.content['zap-command-parser-ver-3.c']
    // Test Cluster command parsers that should be defined
    expect(pv3).toContain(
      'EmberAfStatus emberAfGroupsClusterClientCommandParse(EmberAfClusterCommand * cmd);'
    )
    expect(pv3).toContain(
      'EmberAfStatus emberAfGroupsClusterServerCommandParse(EmberAfClusterCommand * cmd);'
    )
    expect(pv3).toContain(
      'EmberAfStatus emberAfIdentifyClusterClientCommandParse(EmberAfClusterCommand * cmd);'
    )
    expect(pv3).toContain(
      'EmberAfStatus emberAfIdentifyClusterServerCommandParse(EmberAfClusterCommand * cmd);'
    )
    expect(pv3).toContain(
      'EmberAfStatus emberAfLevelControlClusterServerCommandParse(EmberAfClusterCommand * cmd);'
    )
    expect(pv3).toContain(
      'EmberAfStatus emberAfOnOffClusterServerCommandParse(EmberAfClusterCommand * cmd);'
    )
    expect(pv3).toContain(
      'EmberAfStatus emberAfScenesClusterServerCommandParse(EmberAfClusterCommand * cmd);'
    )
    expect(pv3).toContain(
      'EmberAfStatus emberAfZllCommissioningClusterClientCommandParse(EmberAfClusterCommand * cmd);'
    )
    // Test Command callback
    expect(pv3).toContain(
      'wasHandled = emberAfIdentifyClusterIdentifyCallback(identifyTime);'
    )
    expect(pv3).toContain(
      'wasHandled = emberAfLevelControlClusterMoveToLevelWithOnOffCallback(level, transitionTime);'
    )
    // Test command argument declarations for data types greater than 4
    // bytes and make sure they are declared as pointers
    expect(pv3).toContain('case ZCL_GP_PROXY_TABLE_REQUEST_COMMAND_ID:')
    expect(pv3).not.toContain('uint64_t gpdIeee;')
    expect(pv3).toContain('uint8_t * gpdIeee;')

    //********* Test the new helpers for the same content as above******************
    // Test Cluster command parsers that should be defined
    let pv5 = genResult.content['zap-command-parser-ver-5.c']
    expect(pv5).toContain(
      'EmberAfStatus emberAfGroupsClusterClientCommandParse(EmberAfClusterCommand * cmd);'
    )
    expect(pv5).toContain(
      'EmberAfStatus emberAfGroupsClusterServerCommandParse(EmberAfClusterCommand * cmd);'
    )
    expect(pv5).toContain(
      'EmberAfStatus emberAfIdentifyClusterClientCommandParse(EmberAfClusterCommand * cmd);'
    )
    expect(pv5).toContain(
      'EmberAfStatus emberAfIdentifyClusterServerCommandParse(EmberAfClusterCommand * cmd);'
    )
    expect(pv5).toContain(
      'EmberAfStatus emberAfLevelControlClusterServerCommandParse(EmberAfClusterCommand * cmd);'
    )
    expect(pv5).toContain(
      'EmberAfStatus emberAfOnOffClusterServerCommandParse(EmberAfClusterCommand * cmd);'
    )
    expect(pv5).toContain(
      'EmberAfStatus emberAfScenesClusterServerCommandParse(EmberAfClusterCommand * cmd);'
    )
    expect(pv5).toContain(
      'EmberAfStatus emberAfZllCommissioningClusterClientCommandParse(EmberAfClusterCommand * cmd);'
    )
    // Test Command callback
    expect(pv5).toContain(
      'wasHandled = emberAfIdentifyClusterIdentifyCallback(identifyTime);'
    )
    expect(pv5).toContain(
      'wasHandled = emberAfLevelControlClusterMoveToLevelWithOnOffCallback(level, transitionTime);'
    )
    // Test command argument declarations for data types greater than 4
    // bytes and make sure they are declared as pointers
    expect(pv5).toContain('case ZCL_GP_PROXY_TABLE_REQUEST_COMMAND_ID:')
    expect(pv5).not.toContain('uint64_t gpdIeee;')
    expect(pv5).toContain('uint8_t * gpdIeee;')

    //********* Test the helpers related to outgoing commands******************
    // Test Cluster outgoing commands that should be generated
    let zapOutgoingCommands = genResult.content['zap-outgoing-command.out']
    expect(zapOutgoingCommands).not.toBeNull()
    expect(zapOutgoingCommands).toContain(
      'Groups client Cluster Outgoing commands\nOutgoing Command: AddGroup\nOutgoing Command: ViewGroup\nOutgoing Command: GetGroupMembership\nOutgoing Command: RemoveGroup\nOutgoing Command: RemoveAllGroups\nOutgoing Command: AddGroupIfIdentifying\nGroups server Cluster Outgoing commands\nOutgoing Command: AddGroupResponse\nOutgoing Command: ViewGroupResponse\nOutgoing Command: GetGroupMembershipResponse\nOutgoing Command: RemoveGroupResponse\nIdentify client Cluster Outgoing commands\nOutgoing Command: Identify\nOutgoing Command: IdentifyQuery\nIdentify server Cluster Outgoing commands\nOutgoing Command: IdentifyQueryResponse\nLevel Control client Cluster Outgoing commands\nOutgoing Command: MoveToLevel\nOutgoing Command: Move\nOutgoing Command: Step\nOutgoing Command: Stop\nOutgoing Command: MoveToLevelWithOnOff\nOutgoing Command: MoveWithOnOff\nOutgoing Command: StepWithOnOff\nOutgoing Command: StopWithOnOff\nOn/off client Cluster Outgoing commands\nOutgoing Command: Off\nOutgoing Command: On\nOutgoing Command: Toggle\nScenes server Cluster Outgoing commands\nOutgoing Command: AddSceneResponse\nOutgoing Command: ViewSceneResponse\nOutgoing Command: RemoveSceneResponse\nOutgoing Command: RemoveAllScenesResponse\nOutgoing Command: StoreSceneResponse\nOutgoing Command: GetSceneMembershipResponse\nZLL Commissioning client Cluster Outgoing commands\nOutgoing Command: ScanRequest\nOutgoing Command: DeviceInformationRequest\nOutgoing Command: IdentifyRequest\nOutgoing Command: ResetToFactoryNewRequest\nOutgoing Command: NetworkStartRequest\nOutgoing Command: NetworkJoinRouterRequest\nOutgoing Command: NetworkJoinEndDeviceRequest\nOutgoing Command: NetworkUpdateRequest\nZLL Commissioning server Cluster Outgoing commands\nOutgoing Command: ScanResponse\nOutgoing Command: DeviceInformationResponse\nOutgoing Command: NetworkStartResponse\nOutgoing Command: NetworkJoinRouterResponse\nOutgoing Command: NetworkJoinEndDeviceResponse\nOutgoing Command: EndpointInformation\nOutgoing Command: GetGroupIdentifiersResponse\nOutgoing Command: GetEndpointListResponse'
    )
  },
  testUtil.timeout.long()
)

test(
  'Testing zap command parser generation for manufacturing specific clusters',
  async () => {
    let sid = await querySession.createBlankSession(db)
    await importJs.importDataFromFile(db, testFile4, { sessionId: sid })

    let genResult = await genEngine.generate(
      db,
      sid,
      templateContext.packageId,
      {},
      {
        disableDeprecationWarnings: true,
      }
    )

    let pv4 = genResult.content['zap-command-parser-ver-4.c']
    // Test Cluster command parsers for manufacturing specific clusters
    expect(pv4).toContain('case 0xFC57: //Manufacturing Specific cluster')
    expect(pv4).toContain('case 0x1217: // Cluster: SL Works With All Hubs')
    expect(pv4).toContain(
      'result = emberAfSlWorksWithAllHubsClusterClientCommandParse(cmd);'
    )
    expect(pv4).toContain('case 0xFC02: //Manufacturing Specific cluster')
    expect(pv4).toContain('case 0x1002: // Cluster: MFGLIB Cluster')
    expect(pv4).toContain(
      'result = emberAfMfglibClusterClusterServerCommandParse(cmd);'
    )
  },
  testUtil.timeout.long()
)

test(
  'Validate custom xml package loading',
  async () => {
    // Import a zap file
    let { sessionId, errors, warnings } = await importJs.importDataFromFile(
      db,
      testFile2
    )

    // Load a custom xml file
    let result = await zclLoader.loadIndividualFile(
      db,
      testUtil.testCustomXml2,
      sessionId
    )

    if (!result.succeeded) {
      console.log(`Test failure: ${result.err}`)
    }
    expect(result.succeeded).toBeTruthy()

    // Add the packageId from above into the session
    await queryPackage.insertSessionPackage(
      db,
      sessionId,
      result.packageId,
      false
    )

    // Generate code using templates
    let genResult = await genEngine.generate(
      db,
      sessionId,
      templateContext.packageId,
      {},
      {
        disableDeprecationWarnings: true,
      }
    )
    // Check if the types are generated correctly
    // Test custom enum generation
    let types = genResult.content['zap-type.h']
    expect(types).toContain('EMBER_ZCL_CUSTOM_STATUS_A = 0,')
    expect(types).toContain('EmberAfCustomStatus;')
    // Test custom struct generation
    expect(types).toContain('typedef struct _CustomStruct')
    expect(types).toContain('uint32_t S1;')
    expect(types).toContain('EmberAfCustomType2 S2;')
    expect(types).toContain('EmberAfCustomLevel S3;')
    expect(types).toContain('EmberAfCustomArea S4;')

    // Test custom outgoing commands are generated correctly
    let commands = genResult.content['zap-command-ver-2.h']
    expect(commands).toContain('#define emberAfFillCommandCustomClusterC1')
    expect(commands).toContain('ZCL_C1_COMMAND_ID')
    expect(commands).toContain('#define emberAfFillCommandCustomClusterC13')
    expect(commands).toContain('ZCL_C13_COMMAND_ID')

    // Test custom command coming from standard cluster extensions(identify cluster extension)
    expect(commands).toContain(
      '#define emberAfFillCommandIdentifyClusterSampleMfgSpecificIdentifyCommand1'
    )

    // Test command structs genereted
    let structs = genResult.content['zap-command-structs.h']
    expect(structs).toContain(
      'typedef struct __zcl_custom_cluster_cluster_c14_command'
    )
    expect(structs).toContain('sl_zcl_custom_cluster_cluster_c5_command_t;')

    // Test Custom attributes and command ids
    let ids = genResult.content['zap-id.h']
    expect(ids).toContain('#define ZCL_C15_COMMAND_ID (0x03)')
    expect(ids).toContain('#define ZCL_A8_ATTRIBUTE_ID (0x0301)')
    expect(ids).toContain('#define ZCL_C11_COMMAND_ID (0x0A)')

    // Test custom attributes coming from standard cluster extensions(identify cluster extension)
    expect(ids).toContain(
      '#define ZCL_SAMPLE_MFG_SPECIFIC_IDENTIFY_1_ATTRIBUTE_ID (0x0000)'
    )

    // Delete the custom xml packageId from the existing session and test generation again
    await queryPackage.deleteSessionPackage(db, sessionId, result.packageId)

    // Generate again after removing a custom xml file
    genResult = await genEngine.generate(
      db,
      sessionId,
      templateContext.packageId,
      {},
      {
        disableDeprecationWarnings: true,
      }
    )
    ids = genResult.content['zap-id.h']
    commands = genResult.content['zap-command-ver-2.h']

    // Test custom attributes removal coming from standard cluster extensions(identify cluster extension)
    expect(ids).not.toContain(
      '#define ZCL_SAMPLE_MFG_SPECIFIC_IDENTIFY_1_ATTRIBUTE_ID (0x0000)'
    )

    // Test custom command removal coming from standard cluster extensions(identify cluster extension)
    expect(commands).not.toContain(
      '#define emberAfFillCommandIdentifyClusterSampleMfgSpecificIdentifyCommand1'
    )
  },
  testUtil.timeout.long()
)
