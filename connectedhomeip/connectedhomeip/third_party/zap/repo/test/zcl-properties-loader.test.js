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

const dbApi = require('../src-electron/db/db-api')
const dbEnum = require('../src-shared/db-enum')
const queryZcl = require('../src-electron/db/query-zcl')
const queryDeviceType = require('../src-electron/db/query-device-type')
const queryPackage = require('../src-electron/db/query-package')
const zclLoader = require('../src-electron/zcl/zcl-loader')
const env = require('../src-electron/util/env')
const path = require('path')
const testQuery = require('./test-query')
const { timeout } = require('./test-util')

const zclTestPropertiesFile = path.join(
  __dirname,
  '../zcl-builtin/silabs/zcl-test.properties'
)

beforeAll(() => {
  env.setDevelopmentEnv()
})

test(
  'test Silabs zcl data loading in memory',
  async () => {
    let db = await dbApi.initRamDatabase()
    try {
      await dbApi.loadSchema(db, env.schemaFile(), env.zapVersion())
      let ctx = await zclLoader.loadZcl(db, zclTestPropertiesFile)
      let packageId = ctx.packageId
      let p = await queryPackage.getPackageByPackageId(ctx.db, ctx.packageId)
      expect(p.version).toEqual('ZCL Test Data')
      let x = await queryPackage.getPackagesByType(
        db,
        dbEnum.packageType.zclProperties
      )
      expect(x.length).toEqual(1)

      x = await queryZcl.selectAllClusters(db, packageId)
      expect(x.length).toEqual(104)
      x = await queryZcl.selectAllDomains(db, packageId)
      expect(x.length).toEqual(17)
      x = await queryZcl.selectAllEnums(db, packageId)
      expect(x.length).toEqual(208)
      x = await queryZcl.selectAllStructsWithItemCount(db, [packageId])
      expect(x.length).toEqual(50)
      x = await queryZcl.selectAllBitmaps(db, packageId)
      expect(x.length).toEqual(128)
      x = await queryDeviceType.selectAllDeviceTypes(db, packageId)
      expect(x.length).toEqual(152)
      x = await testQuery.selectCountFrom(db, 'COMMAND_ARG')
      expect(x).toEqual(1663)
      x = await testQuery.selectCountFrom(db, 'COMMAND')
      expect(x).toEqual(553)
      x = await testQuery.selectCountFrom(db, 'ENUM_ITEM')
      expect(x).toEqual(1560)
      x = await testQuery.selectCountFrom(db, 'ATTRIBUTE')
      expect(x).toEqual(3408)
      x = await testQuery.selectCountFrom(db, 'BITMAP_FIELD')
      expect(x).toEqual(721)
      x = await testQuery.selectCountFrom(db, 'STRUCT_ITEM')
      expect(x).toEqual(154)
      x = await dbApi.dbAll(
        db,
        'SELECT MANUFACTURER_CODE FROM CLUSTER WHERE MANUFACTURER_CODE NOT NULL',
        []
      )
      expect(x.length).toEqual(0)
      x = await dbApi.dbAll(
        db,
        'SELECT MANUFACTURER_CODE FROM COMMAND WHERE MANUFACTURER_CODE NOT NULL',
        []
      )
      expect(x.length).toEqual(0)
      x = await dbApi.dbAll(
        db,
        'SELECT MANUFACTURER_CODE FROM ATTRIBUTE WHERE MANUFACTURER_CODE NOT NULL',
        []
      )
      expect(x.length).toEqual(0)
      let rows = await dbApi.dbMultiSelect(
        db,
        'SELECT CLUSTER_ID FROM CLUSTER WHERE CODE = ?',
        [[0], [6]]
      )
      expect(rows.length).toBe(2)
      expect(rows[0]).not.toBeUndefined()
      expect(rows[1]).not.toBeUndefined()
      expect(rows[0].CLUSTER_ID).not.toBeUndefined()
      expect(rows[1].CLUSTER_ID).not.toBeUndefined()

      rows = await queryPackage.selectAllOptionsValues(
        db,
        packageId,
        dbEnum.sessionOption.defaultResponsePolicy
      )

      expect(rows.length).toBe(3)
    } finally {
      await dbApi.closeDatabase(db)
    }
  },
  timeout.medium()
)
