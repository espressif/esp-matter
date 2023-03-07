#!/usr/bin/env node
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
 */

const scriptUtil = require('./script-util.js')

let startTime = process.hrtime.bigint()

//workaround: executeCmd()/spawn() fails silently without complaining about missing path to electron
process.env.PATH = process.env.PATH + ':/usr/local/bin/'

scriptUtil
  .executeCmd({}, 'mkdir', ['-p', 'reports'])
  .then(() =>
    scriptUtil.executeCmd(
      {},
      'cp',
      'cypress-coverage/coverage-final.json reports/from-cypress.json'.split(
        ' '
      )
    )
  )

  .then(() =>
    scriptUtil.executeCmd({}, 'cp', [
      'jest-coverage/coverage-final.json',
      'reports/from-jest.json',
    ])
  )

  .then(() => scriptUtil.executeCmd({}, 'npx', ['nyc', 'merge', 'reports']))
  .then(() =>
    scriptUtil.executeCmd({}, 'mv', ['coverage.json', '.nyc_output/out.json'])
  )
  .then(() =>
    scriptUtil.executeCmd(
      {},
      'npx',
      'nyc report --reporter lcov --reporter text --report-dir coverage'.split(
        ' '
      )
    )
  )
  .then(() =>
    console.log(
      `✅ Please find the combined report (Jest & Cypress) at ./coverage/lcov-report/index.html`
    )
  )

  .catch((err) => {
    console.log(err)
  })
