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

const templateEngine = require('../src-electron/generator/template-engine.js')
const fs = require('fs')
const path = require('path')
const { timeout } = require('./test-util.js')

test(
  'Helper functions need to be snake_case without uppercase characters unless they are deprecated.',
  () => {
    let api = templateEngine.allBuiltInHelpers()
    expect(Object.keys(api.helpers).length).toBeGreaterThan(50)
    for (const x of api.helpers) {
      let n = x.name
      if (!x.isDeprecated && x.category == null) {
        expect(n.toLowerCase()).toEqual(n)
      }
    }
  },
  timeout.short()
)

test(
  'Check that there is no overlapping duplicates.',
  () => {
    let api = templateEngine.allBuiltInHelpers()
    expect(api.hasDuplicates).toBe(false)
    let dups = api.duplicates.join(', ')
    expect(dups).toBe('')
  },
  timeout.short()
)

test(
  'Compare APIs against the baseline.',
  () => {
    let apiFromFile = JSON.parse(
      fs.readFileSync(path.join(__dirname, 'helper-api-baseline.json'))
    )
    let api = templateEngine.allBuiltInHelpers()

    let errorMessage = ''

    apiFromFile.forEach((x) => {
      let apiFn = api.helpers.find((a) => a.name === x.name)
      if (apiFn == undefined) {
        errorMessage += `Helper ${x.name} has been removed, breaking the API.\n`
      }
      if (x.isDeprecated) {
        if (!apiFn.isDeprecated)
          errorMessage += `Helper ${x.name} has been deprecated, but now it's not any more.\n`
      }
    })
    expect(errorMessage).toEqual('')
  },
  timeout.short()
)
