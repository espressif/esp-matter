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

/**
 * Script that updates the version in package.json
 */

const fs = require('fs')
const path = require('path')
const readline = require('readline')
const scriptUtil = require('./script-util')

if (
  process.argv[2] == '-?' ||
  process.argv[2] == '--?' ||
  process.argv[2] == '--help'
) {
  console.log('Usage: zap-update-package-version.js [-fake|-real]\n')
  console.log('  fake: adds the fake version to package.json for committing')
  console.log('  real: adds the real version to package.json from current date')
  console.log('\nIf no command is passed, script just prints out the version.')
  process.exit(0)
}

let mode = 'print'

if (process.argv[2] == '-fake') mode = 'fake'
if (process.argv[2] == '-real') mode = 'real'

scriptUtil
  .setPackageJsonVersion(new Date(), mode)
  .then((wasChanged) => {
    if (wasChanged) {
      console.log(
        '⛔ Version in package.json was not set correctly. It was automatically updated. Review and commit again, please.'
      )
    } else {
      console.log('😎 Version in package.json was not changed.')
    }
    process.exit(wasChanged ? 1 : 0)
  })
  .catch((err) => {
    console.log(`⛔ Error: ${err}`)
    process.exit(1)
  })
