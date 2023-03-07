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
const fs = require('fs')
const startup = require('../src-electron/main-process/startup.js')
const env = require('../src-electron/util/env.ts')
const testUtil = require('./test-util.js')

beforeAll(async () => {
  env.setDevelopmentEnv()
})

test(
  'startup: start generation',
  () => {
    let testGenDir = path.join(path.join(__dirname, '.zap/'), 'test-gen')
    if (!fs.existsSync(testGenDir))
      fs.mkdirSync(testGenDir, { recursive: true })
    return startup.startGeneration(
      {
        skipPostGeneration: true,
        output: testGenDir,
        generationTemplate: testUtil.testTemplate.zigbee,
        zclProperties: env.builtinSilabsZclMetafile(),
        zapFiles: null,
      },
      {
        quitFunction: null,
        logger: (msg) => {},
      }
    )
  },
  testUtil.timeout.long()
)

test(
  'startup: self-check',
  () => {
    return startup.startSelfCheck(
      {
        zclProperties: env.builtinSilabsZclMetafile(),
      },
      { logger: (msg) => {}, quit: false }
    )
  },
  testUtil.timeout.long()
)

test(
  'startup: convert',
  () => {
    let files = []
    files.push(path.join(__dirname, 'resource/isc/test-light.isc'))
    let output = '{basename}.conversion'
    let testOutputFile = path.join(
      __dirname,
      'resource/isc/test-light.conversion'
    )
    let testConversionResults = path.join(
      __dirname,
      'resource/isc/test-light.conversion.results.yaml'
    )

    return startup
      .startConvert(
        {
          zapFiles: files,
          output: output,
          zclProperties: env.builtinSilabsZclMetafile(),
          noZapFileLog: true,
          results: testConversionResults,
        },
        {
          quitFunction: null,
          logger: (msg) => {},
        }
      )
      .then(() => {
        expect(fs.existsSync(testOutputFile)).toBeTruthy()
        fs.unlinkSync(testOutputFile)

        expect(fs.existsSync(testConversionResults)).toBeTruthy()
        fs.unlinkSync(testConversionResults)
      })
  },
  testUtil.timeout.long()
)

test(
  'startup: analyze',
  () => {
    let files = []
    files.push(path.join(__dirname, 'resource/isc/test-light.isc'))
    return startup.startAnalyze(
      {
        zapFiles: files,
        zclProperties: env.builtinSilabsZclMetafile(),
      },
      {
        quitFunction: null,
        cleanDb: false,
        logger: (msg) => {},
      }
    )
  },
  testUtil.timeout.long()
)
