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

const cHelper = require('../src-electron/generator/helper-c')
const env = require('../src-electron/util/env.ts')
const dbApi = require('../src-electron/db/db-api.js')
const genEngine = require('../src-electron/generator/generation-engine.js')
const testUtil = require('./test-util.js')
const zclLoader = require('../src-electron/zcl/zcl-loader.js')
const zclHelper = require('../src-electron/generator/helper-zcl.js')
const dbEnum = require('../src-shared/db-enum.js')
const zapHelper = require('../src-electron/generator/helper-zap.js')

let db
let zclContext

let ctx

beforeAll(async () => {
  env.setDevelopmentEnv()
  let file = env.sqliteTestFile('helpers')
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
    let context = await genEngine.loadTemplates(
      db,
      testUtil.testTemplate.zigbee
    )

    expect(context.crc).not.toBeNull()
    expect(context.templateData).not.toBeNull()
    expect(context.templateData.name).toEqual('Test templates')
    expect(context.templateData.version).toEqual('test-v1')
    expect(context.templateData.templates.length).toEqual(
      testUtil.testTemplate.zigbeeCount
    )
    expect(context.packageId).not.toBeNull()
  },
  testUtil.timeout.medium()
)

test(
  'Load ZCL stuff',
  async () => {
    let context = await zclLoader.loadZcl(db, env.builtinSilabsZclMetafile())
    zclContext = context

    let globalCtx = {
      db: zclContext.db,
      zclPackageId: zclContext.packageId,
      zclPackageIds: [zclContext.packageId],
    }
    ctx = {
      global: globalCtx,
    }
  },
  testUtil.timeout.medium()
)

/*
 * Helper-c.js
 */

test(
  'Add one',
  () => {
    expect(cHelper.add_one(52)).toEqual(53)
  },
  testUtil.timeout.short()
)

test(
  'Number greater than 2',
  () => {
    expect(cHelper.is_number_greater_than(3, 2)).toBeTruthy
    expect(cHelper.is_number_greater_than(1, 2)).toBeFalsy
  },
  testUtil.timeout.short()
)

test(
  'dataTypeForEnum',
  async () => {
    let result = await cHelper.data_type_for_enum(
      db,
      'patate',
      zclContext.packageId
    )
    expect(result).toBe('!!Invalid enum: patate')

    result = await cHelper.data_type_for_enum(
      db,
      'Status',
      zclContext.packageId
    )

    expect(result).toBe('SL_CLI_ARG_UINT8')
  },
  testUtil.timeout.short()
)

test(
  'dataTypeForEnum',
  async () => {
    let result = await cHelper.dataTypeForBitmap(
      db,
      'patate',
      zclContext.packageId
    )
    expect(result).toBe('!!Invalid bitmap: patate')

    result = await cHelper.dataTypeForBitmap(
      db,
      'LocationType',
      zclContext.packageId
    )
    expect(result).toBe('SL_CLI_ARG_UINT8')
  },
  testUtil.timeout.short()
)

test(
  'Various String helper',
  () => {
    expect(cHelper.cleanseLabelAsKebabCase('Very Simple:Label')).toEqual(
      'very-simple-label'
    )
    // Might want to add CamelCased string
    expect(cHelper.asUnderscoreLowercase('testString')).toBe('test_string')
    expect(cHelper.asSpacedLowercase('testString')).toBe('test string')
    expect(cHelper.asUnderscoreUppercase('bigTestString')).toBe(
      'BIG_TEST_STRING'
    )
    expect(cHelper.asUnderscoreUppercase('BigTestString')).toBe(
      'BIG_TEST_STRING'
    )
  },
  testUtil.timeout.short()
)

test(
  'asBytes helper',
  () => {
    return (
      cHelper.as_bytes
        .call(ctx, 'Garbage', null)
        .then((value) => expect(value).toBe('Garbage'))
        .then(() => cHelper.asBytes.call(ctx, '6', 'int8u'))
        .then((value) => expect(value).toBe('0x06'))
        .then(() => cHelper.asBytes.call(ctx, null, 'garbage'))
        .then((result) => expect(result).toBe('0x00'))
        // asBytes will return a list of character value in hex
        // if the type is invalid.
        .then(() => cHelper.asBytes.call(ctx, '9', 'garbage'))
        .then((result) => expect(result).toBe('0x39, 0x00'))
    )
  },
  testUtil.timeout.short()
)

test(
  'FormatValue helper',
  () => {
    expect(cHelper.formatValue('0xAA', 1)).toBe('0xAA')
    expect(cHelper.formatValue('0xAA', 2)).toBe('0x00, 0xAA')
    expect(cHelper.formatValue('0xAA', 4)).toBe('0x00, 0x00, 0x00, 0xAA')
    expect(cHelper.formatValue('g', -1)).toBe("1,'g'")
    expect(cHelper.formatValue('42', 1)).toBe('0x2A')
    expect(cHelper.formatValue('g', 1)).toBe('0x00')
  },
  testUtil.timeout.short()
)

/*
 * Helper-zcl.js
 */
test(
  'Various small Helper',
  () => {
    expect(zclHelper.isLastElement(99, 100)).toBeTruthy()

    zclHelper
      .isBitmap(ctx.global.db, 'patate', ctx.global.zclPackageId)
      .then((result) => {
        expect(result).toBe(dbEnum.zclType.unknown)
      })

    zclHelper
      .isBitmap(db, 'LocationType', zclContext.packageId)
      .then((result) => {
        expect(result).toBe(dbEnum.zclType.bitmap)
      })

    zclHelper.isEnum(db, 'patate', zclContext.packageId).then((result) => {
      expect(result).toBe(dbEnum.zclType.unknown)
    })
    zclHelper.isEnum(db, 'Status', zclContext.packageId).then((result) => {
      expect(result).toBe(dbEnum.zclType.enum)
    })

    zclHelper.isStruct(db, 'patate', zclContext.packageId).then((result) => {
      expect(result).toBe(dbEnum.zclType.unknown)
    })
    zclHelper.isStruct(db, 'Protocol', zclContext.packageId).then((result) => {
      expect(result).toBe(dbEnum.zclType.struct)
    })
  },
  testUtil.timeout.short()
)

test(
  'Generated Macro little endian for attribute of size 3 bytes',
  () => {
    let options = { hash: { endian: 'little' } }
    return zclHelper
      .as_generated_default_macro('0x003840', 3, options)
      .then((res) => {
        return expect(res).toBe('0x40, 0x38, 0x00, ')
      })
  },
  testUtil.timeout.short()
)

test(
  'Generated Macro big endian for attribute of size 3 bytes',
  () => {
    let options = { hash: { endian: 'big' } }
    return zclHelper
      .as_generated_default_macro('0x003840', 3, options)
      .then((res) => {
        return expect(res).toBe(' 0x00, 0x38, 0x40,')
      })
  },
  testUtil.timeout.short()
)

test(
  'Generated Macro little endian',
  () => {
    let options = { hash: { endian: 'little' } }
    return zclHelper
      .as_generated_default_macro('0x00003840', 4, options)
      .then((res) => expect(res).toBe('0x40, 0x38, 0x00, 0x00, '))
  },
  testUtil.timeout.short()
)

test(
  'Generated Macro big endian',
  () => {
    let options = { hash: { endian: 'big' } }
    return zclHelper
      .as_generated_default_macro('0x00003840', 4, options)
      .then((res) => expect(res).toBe(' 0x00, 0x00, 0x38, 0x40,'))
  },
  testUtil.timeout.short()
)

test(
  'Generated Macro Negative Value with attribute size of 8 bytes big endian',
  () => {
    let options = { hash: { endian: 'big' } }
    return zclHelper
      .as_generated_default_macro('-5', 8, options)
      .then((res) =>
        expect(res).toBe(' 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFB,')
      )
  },
  testUtil.timeout.short()
)

test(
  'Generated Macro Negative Value with attribute size of 8 bytes little endian',
  () => {
    let options = { hash: { endian: 'little' } }
    return zclHelper
      .as_generated_default_macro('-5', 8, options)
      .then((res) =>
        expect(res).toBe('0xFB, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, ')
      )
  },
  testUtil.timeout.short()
)

test(
  'Generated Macro Negative Value with attribute size of 5 bytes big endian',
  () => {
    let options = { hash: { endian: 'big' } }
    return zclHelper
      .as_generated_default_macro('-5', 5, options)
      .then((res) => expect(res).toBe(' 0xFF, 0xFF, 0xFF, 0xFF, 0xFB,'))
  },
  testUtil.timeout.short()
)

test(
  'Generated Macro Negative Value with attribute size of 5 bytes little endian',
  () => {
    let options = { hash: { endian: 'little' } }
    return zclHelper
      .as_generated_default_macro('-5', 5, options)
      .then((res) => expect(res).toBe('0xFB, 0xFF, 0xFF, 0xFF, 0xFF, '))
  },
  testUtil.timeout.short()
)

test(
  'Generated Macro Float Value little endian',
  () => {
    let options = { hash: { endian: 'little' } }
    return zclHelper
      .as_generated_default_macro('17.0', 8, options)
      .then((res) =>
        expect(res).toBe('0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x31, 0x40, ')
      )
  },
  testUtil.timeout.short()
)

test(
  'Generated Macro Float Value big endian',
  () => {
    let options = { hash: { endian: 'big' } }
    return zclHelper
      .as_generated_default_macro('17.0', 8, options)
      .then((res) =>
        expect(res).toBe(' 0x40, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,')
      )
  },
  testUtil.timeout.short()
)

test(
  'Generated Negative Macro Float Value little endian',
  () => {
    let options = { hash: { endian: 'little' } }
    return zclHelper
      .as_generated_default_macro('-17.0', 8, options)
      .then((res) =>
        expect(res).toBe('0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x31, 0xC0, ')
      )
  },
  testUtil.timeout.short()
)

test(
  'Generated Negative Macro Float Value big endian',
  () => {
    let options = { hash: { endian: 'big' } }
    return zclHelper
      .as_generated_default_macro('-17.0', 8, options)
      .then((res) =>
        expect(res).toBe(' 0xC0, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,')
      )
  },
  testUtil.timeout.short()
)

test(
  'Generated Macro for 5 byte integer big endian',
  () => {
    let options = { hash: { endian: 'big' } }
    return zclHelper
      .as_generated_default_macro('549755813887', 5, options)
      .then((res) => expect(res).toBe(' 0x7F, 0xFF, 0xFF, 0xFF, 0xFF,'))
  },
  testUtil.timeout.short()
)

test(
  'Generated Macro for 5 byte integer little endian',
  () => {
    let options = { hash: { endian: 'little' } }
    return zclHelper
      .as_generated_default_macro('549755813887', 5, options)
      .then((res) => expect(res).toBe('0xFF, 0xFF, 0xFF, 0xFF, 0x7F, '))
  },
  testUtil.timeout.short()
)

test(
  'Generated Macro for 6 byte integer big endian',
  () => {
    let options = { hash: { endian: 'big' } }
    return zclHelper
      .as_generated_default_macro('140737488355327', 6, options)
      .then((res) => expect(res).toBe(' 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,'))
  },
  testUtil.timeout.short()
)

test(
  'Generated Macro for 6 byte integer little endian',
  () => {
    let options = { hash: { endian: 'little' } }
    return zclHelper
      .as_generated_default_macro('140737488355327', 6, options)
      .then((res) => expect(res).toBe('0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, '))
  },
  testUtil.timeout.short()
)

test(
  'Generated Macro for 7 byte integer big endian',
  () => {
    let options = { hash: { endian: 'big' } }
    return zclHelper
      .as_generated_default_macro('140737488355327', 7, options)
      .then((res) =>
        expect(res).toBe('0x00,  0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,')
      )
  },
  testUtil.timeout.short()
)

test(
  'Generated Macro for 7 byte integer little endian',
  () => {
    let options = { hash: { endian: 'little' } }
    return zclHelper
      .as_generated_default_macro('140737488355327', 7, options)
      .then((res) =>
        expect(res).toBe('0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F,  0x00,')
      )
  },
  testUtil.timeout.short()
)

test(
  'Generated Macro for 8 byte integer big endian',
  () => {
    let options = { hash: { endian: 'big' } }
    return zclHelper
      .as_generated_default_macro('140737488355327', 8, options)
      .then((res) =>
        expect(res).toBe('0x00, 0x00,  0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,')
      )
  },
  testUtil.timeout.short()
)

test(
  'Generated Macro for 8 byte integer little endian',
  () => {
    let options = { hash: { endian: 'little' } }
    return zclHelper
      .as_generated_default_macro('140737488355327', 8, options)
      .then((res) =>
        expect(res).toBe('0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F,  0x00, 0x00,')
      )
  },
  testUtil.timeout.short()
)

test(
  'Generated Macro for string',
  () => {
    let options = { hash: { endian: 'big' } }
    return zclHelper
      .as_generated_default_macro('abc', 5, options)
      .then((res) => expect(res).toBe("0x03, 'a', 'b', 'c', 0, "))
  },
  testUtil.timeout.short()
)

test(
  'Generated Macro for long string',
  () => {
    // Currently string length prefix is always little endian based
    // on the rest of our implementation for string attribute handling.
    let options = { hash: { endian: 'big' } }
    return zclHelper
      .as_generated_default_macro('abc', 259, options)
      .then((res) => expect(res).toContain("0x03, 0x00, 'a', 'b', 'c', 0, "))
  },
  testUtil.timeout.short()
)

test(
  'Attribute Mask',
  () =>
    zclHelper
      .attribute_mask(0, 'RAM', 0, 0, 0, 'server', 1, 'ATTRIBUTE_MASK_', '')
      .then((res) => expect(res).toBe('ATTRIBUTE_MASK_SINGLETON'))
      .then(() =>
        zclHelper.attribute_mask(
          1,
          'RAM',
          1,
          0,
          32,
          'server',
          0,
          'ATTRIBUTE_MASK_',
          ''
        )
      )
      .then((res) =>
        expect(res).toBe('ATTRIBUTE_MASK_WRITABLE| ATTRIBUTE_MASK_MIN_MAX')
      ),
  testUtil.timeout.short()
)

test(
  'Command Mask',
  () =>
    zclHelper
      .command_mask('client', 'either', 1, 1, 0, 'COMMAND_MASK_')
      .then((res) =>
        expect(res).toBe(
          'COMMAND_MASK_INCOMING_SERVER | COMMAND_MASK_OUTGOING_CLIENT'
        )
      ),
  testUtil.timeout.short()
)

/*
helper-zap.js
*/
test(
  'String comparison',
  () => {
    expect(zapHelper.is_lowercase_equal('A', 'a')).toBeTruthy()
  },
  testUtil.timeout.short()
)

test(
  'Number comparison',
  () => {
    expect(zapHelper.is_num_equal(1, 1)).toBeTruthy()
    expect(zapHelper.is_num_equal(2, 1)).toBeFalsy()
    expect(zapHelper.is_num_equal('1', 1)).toBeTruthy()
  },
  testUtil.timeout.short()
)

test(
  'is argument undefined',
  () => {
    expect(zapHelper.is_defined('abc')).toBeTruthy()
    expect(zapHelper.is_defined('')).toBeFalsy()
    expect(zapHelper.is_defined(null)).toBeFalsy()
    expect(zapHelper.is_defined(undefined)).toBeFalsy()
  },
  testUtil.timeout.short()
)

test(
  'replace string',
  () => {
    expect(zapHelper.replace_string('testString', 'test', '')).toBe('String')
  },
  testUtil.timeout.short()
)
