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
 * This file is used specifically and only for development. It installs
 * `electron-debug` & `vue-devtools`. There shouldn't be any need to
 *  modify this file, but it can be used to extend your development
 *  environment.
 */

const env = require('../util/env')

// Install `electron-debug` with `devtron`
require('electron-debug')({ showDevTools: false })

// Install `vue-devtools`
require('electron')
  .app.whenReady()
  .then(() => {
    let installExtension = require('electron-devtools-installer')
    installExtension
      .default(installExtension.VUEJS_DEVTOOLS)
      .then(() => {
        env.logInfo('Installation of `vue-tools` succesful.')
      })
      .catch((err) => {
        env.logError('Unable to install `vue-devtools`: \n', err)
      })
  })

// Development via Electron.
if (process.env.MODE === 'electron') {
  process.argv.push('--allowCors')
}

// Require `main` process to boot app
require('./main-ui.js')
