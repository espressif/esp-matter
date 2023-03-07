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

let createDMG = require('electron-installer-dmg')
let opts = {
  //debug: true,
  appPath: './dist/electron/zap-darwin-x64/zap.app',
  out: './dist/electron/zap-darwin-x64/',
  name: 'zap-darwin-x64',
  overwrite: true,
}

createDMG(opts, function done(err) {
  console.log(err)
})
