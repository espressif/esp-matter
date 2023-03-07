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
const fs = require('fs')

const dbApi = require('../src-electron/db/db-api')
const queryZcl = require('../src-electron/db/query-zcl')
const queryDeviceType = require('../src-electron/db/query-device-type')
const queryCommand = require('../src-electron/db/query-command')
const queryLoader = require('../src-electron/db/query-loader')
const queryConfig = require('../src-electron/db/query-config')
const queryEndpointType = require('../src-electron/db/query-endpoint-type')
const queryEndpoint = require('../src-electron/db/query-endpoint')
const queryPackage = require('../src-electron/db/query-package')
const querySession = require('../src-electron/db/query-session')
const querySessionZcl = require('../src-electron/db/query-session-zcl')

const env = require('../src-electron/util/env')
const util = require('../src-electron/util/util')
const zclUtil = require('../src-electron/util/zcl-util')

const zclLoader = require('../src-electron/zcl/zcl-loader')
const exportJs = require('../src-electron/importexport/export')
const dbEnum = require('../src-shared/db-enum')
const generationEngine = require('../src-electron/generator/generation-engine')
const testUtil = require('./test-util')
const testQuery = require('./test-query')
const restApi = require('../src-shared/rest-api')

/*
 * Created Date: Friday, March 13th 2020, 7:44:12 pm
 * Author: Timotej Ecimovic
 *
 * Copyright (c) 2020 Silicon Labs
 */

let db
let sid
let pkgId

beforeAll(async () => {
  env.setDevelopmentEnv()
  let file = env.sqliteTestFile('query')
  db = await dbApi.initDatabaseAndLoadSchema(
    file,
    env.schemaFile(),
    env.zapVersion()
  )
}, testUtil.timeout.medium())

afterAll(() => dbApi.closeDatabase(db), testUtil.timeout.short())

test(
  'Dirty Flag Validation',
  async () => {
    let result = await querySession.getSessionDirtyFlag(db, sid)
    expect(result).toBeFalsy()
  },
  testUtil.timeout.short()
)

test(
  'Path CRC queries.',
  async () => {
    let path = '/some/random/path'
    let crc = 42
    await queryPackage.insertPathCrc(db, path, crc)
    let c = await queryPackage.getPathCrc(db, path)
    expect(c).toBe(crc)
  },
  testUtil.timeout.short()
)

test(
  'Replace query',
  async () => {
    let rowId = await dbApi.dbInsert(
      db,
      'REPLACE INTO SETTING (CATEGORY, KEY, VALUE) VALUES (?,?,?)',
      ['cat', 'key', 12]
    )
    expect(rowId).toBeGreaterThan(0)

    let result = await dbApi.dbGet(
      db,
      'SELECT VALUE FROM SETTING WHERE CATEGORY = ? AND KEY = ?',
      ['cat', 'key']
    )

    expect(result.VALUE).toBe('12')

    rowId = await dbApi.dbInsert(
      db,
      'REPLACE INTO SETTING (CATEGORY, KEY, VALUE) VALUES (?,?,?)',
      ['cat', 'key', 13]
    )

    expect(rowId).toBeGreaterThan(0)

    result = await dbApi.dbGet(
      db,
      'SELECT VALUE FROM SETTING WHERE CATEGORY = ? AND KEY = ?',
      ['cat', 'key']
    )

    expect(result.VALUE).toBe('13')
  },
  testUtil.timeout.short()
)

test(
  'Simple cluster addition.',
  async () => {
    let pkgId = null
    let rowid = await queryPackage.insertPathCrc(db, 'test', 1)

    pkgId = rowid
    await queryLoader.insertClusters(db, rowid, [
      {
        code: 0x1234,
        name: 'Test',
        description: 'Test cluster',
        define: 'TEST',
      },
    ])

    let rows = await queryZcl.selectAllClusters(db, pkgId)
    expect(rows.length).toBe(1)
    rowid = rows[0].id
    expect(rows[0].code).toBe(4660)
    expect(rows[0].label).toBe('Test')
    row = await queryZcl.selectClusterById(db, rowid)
    expect(row.code).toBe(4660)
    expect(row.label).toBe('Test')
    rows = await queryZcl.selectAttributesByClusterIdIncludingGlobal(
      db,
      rowid,
      pkgId
    )
    expect(rows.length).toBe(0)
    rows = await queryCommand.selectCommandsByClusterId(db, rowid, pkgId)
    expect(rows.length).toBe(0)
  },
  testUtil.timeout.short()
)

test(
  'Now actually load the static data.',
  () => zclLoader.loadZcl(db, env.builtinSilabsZclMetafile()),
  testUtil.timeout.medium()
)

test(
  'Now load the generation data.',
  () => generationEngine.loadTemplates(db, testUtil.testTemplate.zigbee),
  testUtil.timeout.medium()
)

describe('Session specific queries', () => {
  beforeAll(async () => {
    let userSession = await querySession.ensureZapUserAndSession(
      db,
      'USER',
      'SESSION'
    )
    sid = userSession.sessionId
    await util.initializeSessionPackage(db, sid, {
      zcl: env.builtinSilabsZclMetafile(),
      template: env.builtinTemplateMetafile(),
    })
  }, testUtil.timeout.medium())

  test(
    'Test that package id for session is present.',
    () =>
      queryPackage
        .getSessionPackages(db, sid)
        .then((ids) => expect(ids.length).toBe(2)),
    testUtil.timeout.short()
  ) // One for zclpropertie and one for gen template

  test(
    'Test that Zigbee specific generator setting for session is present.',
    () =>
      queryPackage
        .getPackagesByType(db, dbEnum.packageType.genTemplatesJson)
        .then((packages) => {
          expect(packages.length).toBe(1)
          let pkgId = packages.shift().id

          queryPackage
            .selectAllOptionsValues(db, pkgId, 'generator')
            .then((generatorConfigurations) => {
              expect(generatorConfigurations).toBe(1)
              expect(generatorConfigurations.shift().optionCode).toBe(
                'shareClusterStatesAcrossEndpoints'
              )
              expect(generatorConfigurations.shift().optionLabel).toBe('true')
            })
        }),
    testUtil.timeout.short()
  )

  test(
    'Test that ZCL package id for session is present.',
    () =>
      queryPackage
        .getSessionZclPackages(db, sid)
        .then((packages) => expect(packages.length).toBe(1)),
    testUtil.timeout.short()
  ) // One for zclpropertie

  test(
    'Random key value queries',
    async () => {
      await querySession.updateSessionKeyValue(db, sid, 'key1', 'value1')
      let value = await querySession.getSessionKeyValue(db, sid, 'key1')
      expect(value).toBe('value1')
      await querySession.updateSessionKeyValue(db, sid, 'key1', 'value2')
      value = await querySession.getSessionKeyValue(db, sid, 'key1')
      expect(value).toBe('value2')
      value = await querySession.getSessionKeyValue(db, sid, 'nonexistent')
      expect(value).toBeUndefined()
    },
    testUtil.timeout.short()
  )

  test(
    'Make sure session is dirty',
    async () => {
      let result = await querySession.getSessionDirtyFlag(db, sid)
      expect(result).toBeTruthy()
      await querySession.setSessionClean(db, sid)
      result = await querySession.getSessionDirtyFlag(db, sid)
      expect(result).toBeFalsy()
    },
    testUtil.timeout.short()
  )

  test(
    'Make sure triggers work',
    async () => {
      let result = await querySession.getSessionDirtyFlag(db, sid)
      expect(result).toBeFalsy()

      let endpointTypeId = await queryConfig.insertEndpointType(
        db,
        sid,
        'Test endpoint'
      )
      result = await querySession.getSessionDirtyFlag(db, sid)
      expect(result).toBeTruthy()
      let rows = await queryEndpointType.selectAllEndpointTypes(db, sid)
      expect(rows.length).toBe(1)
      await querySession.setSessionClean(db, sid)
      result = await querySession.getSessionDirtyFlag(db, sid)
      expect(result).toBeFalsy()
      await queryEndpointType.deleteEndpointType(db, endpointTypeId)
      result = await querySession.getSessionDirtyFlag(db, sid)
      expect(result).toBeTruthy()
    },
    testUtil.timeout.medium()
  )

  test(
    'Test key values',
    async () => {
      await querySession.updateSessionKeyValue(db, sid, 'testKey', 'testValue')
      let value = await querySession.getSessionKeyValue(db, sid, 'testKey')
      expect(value).toBe('testValue')
    },
    testUtil.timeout.short()
  )

  test(
    'Test state creation',
    () => {
      let endpointTypeId
      return queryConfig
        .insertEndpointType(db, sid, 'Test endpoint')
        .then((id) => {
          endpointTypeId = id
        })
        .then(() => exportJs.createStateFromDatabase(db, sid))
        .then((state) => {
          expect(state.creator).toBe('zap')
          expect(state.keyValuePairs.length).toBe(5)
          expect(state.keyValuePairs[0].key).toBe('commandDiscovery')
          expect(state.keyValuePairs[0].value).toBe('1')
          expect(state.keyValuePairs[1].key).toBe(
            dbEnum.sessionOption.defaultResponsePolicy
          )
          expect(state.keyValuePairs[1].value).toBe('always')
          expect(state.keyValuePairs[2].key).toBe('key1')
          expect(state.keyValuePairs[2].value).toBe('value2')
          expect(state.keyValuePairs[4].key).toBe('testKey')
          expect(state.keyValuePairs[4].value).toBe('testValue')
          expect(state.endpointTypes.length).toBe(1)
          expect(state.endpointTypes[0].name).toBe('Test endpoint')
          expect(state.endpointTypes[0].clusters.length).toBe(0)
          expect(state.package.length).toBe(2)
          let zclIndex
          let genIndex
          if (state.package[0].type === dbEnum.packageType.zclProperties) {
            zclIndex = 0
            genIndex = 1
          } else {
            zclIndex = 1
            genIndex = 0
          }
          expect(state.package[zclIndex].type).toBe(
            dbEnum.packageType.zclProperties
          )
          expect(state.package[zclIndex].version).toBe(1)
          expect(state.package[genIndex].type).toBe(
            dbEnum.packageType.genTemplatesJson
          )
          expect(state.package[genIndex].version).toBe('test-v1')
        })
    },
    testUtil.timeout.short()
  )

  test(
    'Empty delete',
    () =>
      queryEndpoint.deleteEndpoint(db, 123).then((data) => {
        expect(data).toBe(0)
      }),
    testUtil.timeout.short()
  )
})

describe('Endpoint Type Config Queries', () => {
  beforeAll(
    () =>
      querySession
        .ensureZapUserAndSession(db, 'USER', 'SESSION', { sessionId: sid })
        .then((userSession) => {
          sid = userSession.sessionId
        })
        .then(() =>
          queryPackage.getSessionPackagesByType(
            db,
            sid,
            dbEnum.packageType.zclProperties
          )
        )
        .then((packages) => {
          pkgId = packages[0].id
        }),
    testUtil.timeout.medium()
  )

  let endpointTypeIdOnOff
  let levelControlCluster
  let haOnOffDeviceType, zllOnOffLightDevice

  test(
    'Insert EndpointType and test various states',
    () =>
      queryDeviceType.selectAllDeviceTypes(db, pkgId).then((rows) => {
        let haOnOffDeviceTypeArray = rows.filter(
          (data) => data.label === 'HA-onoff'
        )
        let zllOnOffLightDeviceTypeArray = rows.filter(
          (data) => data.label === 'ZLL-onofflight'
        )
        expect(haOnOffDeviceTypeArray.length > 0).toBeTruthy()
        expect(zllOnOffLightDeviceTypeArray.length > 0).toBeTruthy()
        haOnOffDeviceType = haOnOffDeviceTypeArray[0]
        zllOnOffLightDevice = zllOnOffLightDeviceTypeArray[0]
        expect(typeof haOnOffDeviceType).toBe('object')
        expect(typeof zllOnOffLightDevice).toBe('object')
      }),
    testUtil.timeout.medium()
  )
  test(
    'Insert Endpoint Type',
    () =>
      queryConfig
        .insertEndpointType(db, sid, 'testEndpointType', haOnOffDeviceType.id)
        .then((rowId) => {
          endpointTypeIdOnOff = rowId
          return queryEndpointType.selectEndpointType(db, rowId)
        })
        .then((endpointType) => {
          expect(endpointType.deviceTypeRef).toBe(haOnOffDeviceType.id)
          expect(endpointType.name).toBe('testEndpointType')
        }),
    testUtil.timeout.medium()
  )

  test(
    'Test get all cluster states',
    () =>
      testQuery
        .getAllEndpointTypeClusterState(db, endpointTypeIdOnOff)
        .then((clusters) => {
          expect(clusters.length).toBe(6)
        })
        .then(() => queryZcl.selectAllClusters(db, pkgId))
        .then((allClusters) => {
          levelControlCluster = allClusters.find((a) => {
            return a.code == 8
          })
          return queryConfig.insertOrReplaceClusterState(
            db,
            endpointTypeIdOnOff,
            levelControlCluster.id,
            'CLIENT',
            true
          )
        })
        .then((rowId) => {
          expect(typeof rowId).toBe('number')
        })
        .then(() =>
          testQuery.getAllEndpointTypeClusterState(db, endpointTypeIdOnOff)
        )
        .then((clusters) => {
          expect(clusters.length).toBe(7)
        }),
    testUtil.timeout.medium()
  )

  test(
    'Test get all attribute states',
    () =>
      testQuery
        .getEndpointTypeAttributes(db, endpointTypeIdOnOff)
        .then((attributes) => {
          expect(attributes.length).toBe(10)
        }),
    testUtil.timeout.medium()
  )

  test(
    'Get all cluster commands',
    () =>
      testQuery
        .getEndpointTypeCommands(db, endpointTypeIdOnOff)
        .then((commands) => {
          expect(commands.length).toBe(6)
        }),
    testUtil.timeout.medium()
  )
  test(
    'Test Enpoint ID related query',
    async () => {
      let clusterRef = 0
      let attributeRef = 0
      let attributeDefaultValue = 0
      let x = await queryZcl.selectEndpointTypeClustersByEndpointTypeId(
        db,
        endpointTypeIdOnOff
      )
      expect(x.length).toBe(7)
      x.forEach((element) => {
        if (element.side == 'server' && clusterRef == 0) {
          clusterRef = element.clusterRef
        }
      })
      expect(clusterRef == 0).toBeFalsy()

      x = await queryZcl.selectEndpointTypeAttributesByEndpointId(
        db,
        endpointTypeIdOnOff
      )

      expect(x.length).toBe(10)
      x.forEach((element) => {
        if (element.clusterRef == clusterRef && attributeRef == 0) {
          attributeRef = element.attributeRef
          attributeDefaultValue = element.defaultValue
        }
      })
      expect(attributeRef == 0).toBeFalsy()
      x = await queryZcl.selectEndpointTypeAttribute(
        db,
        endpointTypeIdOnOff,
        attributeRef,
        clusterRef
      )

      expect(x.defaultValue).toBe(attributeDefaultValue)

      x = await queryZcl.selectEndpointTypeCommandsByEndpointId(
        db,
        endpointTypeIdOnOff
      )

      expect(x.length).toBe(6)
    },
    testUtil.timeout.medium()
  )
  test(
    'Get all cluster names',
    () => {
      let expectedNames = ['Basic', 'Identify', 'Level Control', 'On/off']
      return queryEndpointType
        .selectEndpointTypeIds(db, sid)
        .then((endpointTypes) =>
          queryEndpointType
            .selectAllClustersNamesFromEndpointTypes(db, endpointTypes)
            .then((names) => {
              expect(names.length).toBe(4)
              names.forEach((element) => {
                expect(expectedNames.includes(element.name)).toBeTruthy()
              })
            })
        )
    },
    testUtil.timeout.medium()
  )

  test(
    'Set additional attributes and commands when cluster state is inserted',
    () => {
      return queryConfig
        .insertOrReplaceClusterState(
          db,
          endpointTypeIdOnOff,
          levelControlCluster.id,
          'CLIENT',
          true
        )
        .then(() =>
          queryConfig.insertClusterDefaults(db, endpointTypeIdOnOff, pkgId, {
            clusterRef: levelControlCluster.id,
            side: 'CLIENT',
          })
        )
        .then(() =>
          testQuery
            .getEndpointTypeAttributes(db, endpointTypeIdOnOff)
            .then((attributes) => {
              expect(attributes.length).toBe(13)
            })
        )
    },
    testUtil.timeout.medium()
  )

  test(
    'Insert Endpoint Test',
    () =>
      queryEndpoint
        .insertEndpoint(db, sid, 4, endpointTypeIdOnOff, 9, 260, 22, 43)
        .then((rowId) => {
          return queryEndpoint.selectEndpoint(db, rowId)
        })
        .then((endpoint) => {
          expect(endpoint.endpointId).toBe(4)
          expect(endpoint.profileId).toBe(260)
          expect(endpoint.networkId).toBe(9)
          expect(endpoint.endpointVersion).toBe(22)
          expect(endpoint.deviceIdentifier).toBe(43)
          expect(endpoint.endpointTypeRef).toBe(endpointTypeIdOnOff)
        }),
    testUtil.timeout.medium()
  )

  test(
    'Test session report',
    () =>
      util.sessionReport(db, sid).then((report) => {
        expect(report.includes('Endpoint: Test endpoint')).toBeTruthy()
        expect(report.includes('0x0000: cluster: Basic (server)')).toBeTruthy()
        expect(
          report.includes('0x0000: attribute: ZCL version [int8u]')
        ).toBeTruthy()
        expect(
          report.includes('0x0007: attribute: power source [enum8]')
        ).toBeTruthy()
        expect(report.includes('0x0006: cluster: On/off (client)')).toBeTruthy()
        expect(report.includes('0x0030: cluster: On/off')).toBeFalsy()
        expect(report.includes('0x00: command: MoveToLevel')).toBeTruthy()
      }),
    testUtil.timeout.medium()
  )

  test('Test session clusters', async () => {
    let clusters = await querySessionZcl.selectAllSessionClusters(db, sid)
    expect(clusters.length).toBeGreaterThan(10)
    let c = await querySessionZcl.selectSessionClusterByCode(db, sid, 2305)
    expect(c.label).toBe('Data Sharing')
  })

  test(
    'Delete Endpoint Type',
    () =>
      queryEndpointType
        .deleteEndpointType(db, endpointTypeIdOnOff)
        .then(testQuery.getAllEndpointTypeClusterState(db, endpointTypeIdOnOff))
        .then((clusters) => {
          expect(clusters.length).toBe(undefined)
          return Promise.resolve()
        }),
    testUtil.timeout.medium()
  )

  test(
    'Test inserting and retrieving options',
    () => {
      let pkgId = null
      return queryPackage
        .insertPathCrc(db, 'junk', 123)
        .then((p) => {
          pkgId = p
          return queryPackage.insertOptionsKeyValues(db, pkgId, 'test', [
            '1',
            '2',
            '3',
          ])
        })
        .then(() => queryPackage.selectAllOptionsValues(db, pkgId, 'test'))
        .then((data) => {
          expect(data.length).toBe(3)
        })
    },
    testUtil.timeout.medium()
  )
})

test(
  'Test Rest Key to DB Column Test',
  () => {
    expect(
      queryConfig.convertRestKeyToDbColumn(restApi.updateKey.endpointId)
    ).toEqual('ENDPOINT_IDENTIFIER')
    expect(
      queryConfig.convertRestKeyToDbColumn(restApi.updateKey.endpointType)
    ).toEqual('ENDPOINT_TYPE_REF')
    expect(
      queryConfig.convertRestKeyToDbColumn(restApi.updateKey.networkId)
    ).toEqual('NETWORK_IDENTIFIER')
    expect(
      queryConfig.convertRestKeyToDbColumn(restApi.updateKey.profileId)
    ).toEqual('PROFILE')
    expect(
      queryConfig.convertRestKeyToDbColumn(restApi.updateKey.deviceTypeRef)
    ).toEqual('DEVICE_TYPE_REF')
    expect(
      queryConfig.convertRestKeyToDbColumn(restApi.updateKey.name)
    ).toEqual('NAME')
    expect(
      queryConfig.convertRestKeyToDbColumn(restApi.updateKey.attributeSelected)
    ).toEqual('INCLUDED')
    expect(
      queryConfig.convertRestKeyToDbColumn(restApi.updateKey.attributeSingleton)
    ).toEqual('SINGLETON')
    expect(
      queryConfig.convertRestKeyToDbColumn(restApi.updateKey.attributeBounded)
    ).toEqual('BOUNDED')
    expect(
      queryConfig.convertRestKeyToDbColumn(restApi.updateKey.attributeDefault)
    ).toEqual('DEFAULT_VALUE')
    expect(
      queryConfig.convertRestKeyToDbColumn(restApi.updateKey.attributeReporting)
    ).toEqual('INCLUDED_REPORTABLE')
    expect(
      queryConfig.convertRestKeyToDbColumn(restApi.updateKey.attributeReportMin)
    ).toEqual('MIN_INTERVAL')
    expect(
      queryConfig.convertRestKeyToDbColumn(restApi.updateKey.attributeReportMax)
    ).toEqual('MAX_INTERVAL')
    expect(
      queryConfig.convertRestKeyToDbColumn(
        restApi.updateKey.attributeReportChange
      )
    ).toEqual('REPORTABLE_CHANGE')
    expect(
      queryConfig.convertRestKeyToDbColumn(restApi.updateKey.attributeStorage)
    ).toEqual('STORAGE_OPTION')
  },
  testUtil.timeout.medium()
)

test(
  'Test determineType',
  async () => {
    let type

    type = await zclUtil.determineType(db, 'patate', pkgId)
    expect(type.type).toEqual(dbEnum.zclType.unknown)
    type = await zclUtil.determineType(db, 'Status', pkgId)
    expect(type.type).toEqual(dbEnum.zclType.enum)
    type = await zclUtil.determineType(db, 'Protocol', pkgId)
    expect(type.type).toEqual(dbEnum.zclType.struct)
    type = await zclUtil.determineType(db, 'CO2TrailingDigit', pkgId)
    expect(type.type).toEqual(dbEnum.zclType.bitmap)
    type = await zclUtil.determineType(db, 'int8u', pkgId)
    expect(type.type).toEqual(dbEnum.zclType.atomic)
  },
  testUtil.timeout.medium()
)
