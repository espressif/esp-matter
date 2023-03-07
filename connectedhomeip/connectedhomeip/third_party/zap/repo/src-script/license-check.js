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

//Usage 'node ./license-check.js --production'

let fs = require('fs')
let path = require('path')
let checker = require('../node_modules/license-checker/lib/index')
let args = require('../node_modules/license-checker/lib/args').parse()
let whiteList = fs
  .readFileSync(path.join(__dirname, 'license-whitelist.txt'))
  .toString()
  .split('\n')
let fail = false
checker.init(args, (err, json) => {
  for (let x of Object.keys(json)) {
    let license = json[x].licenses
    if (!x.includes('zap@') && !whiteList.includes(license.toString())) {
      console.log(
        'New License Found for module: ' +
          x +
          ' license:"' +
          json[x].licenses +
          '"'
      )
      fail = true
    }
  }
  if (fail) {
    console.log('⛔ License check FAILED')
    process.exit(1)
  } else {
    console.log('😎 License check SUCCESS')
  }
})
