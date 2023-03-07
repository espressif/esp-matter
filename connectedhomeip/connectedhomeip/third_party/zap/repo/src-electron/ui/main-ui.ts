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

// enable stack trace to be mapped back to the correct line number in TypeScript source files.
require('source-map-support').install()

const { app } = require('electron')

import * as args from '../util/args'
const env = require('../util/env')
const windowJs = require('./window')
const startup = require('../main-process/startup')
const uiUtil = require('./ui-util')
const util = require('../util/util')

env.versionsCheck()
env.setProductionEnv()

function hookSecondInstanceEvents(argv: args.Arguments) {
  app
    .whenReady()
    .then(() =>
      startup.startUpSecondaryInstance(argv, { quitFunction: app.quit })
    )
}

/**
 * Hook up all the events for the electron app object.
 */
function hookMainInstanceEvents(argv: args.Arguments) {
  app
    .whenReady()
    .then(() =>
      startup.startUpMainInstance(argv, {
        quitFunction: app.quit,
        uiEnableFunction: uiUtil.enableUi,
      })
    )
    .catch((err) => {
      console.log(err)
      app.quit()
    })

  if (!argv._.includes('server') && !argv.noServer) {
    app.on('window-all-closed', () => {
      if (process.platform !== 'darwin') {
        app.quit()
      }
    })
    app.on('activate', () => {
      env.logInfo('Activate...')
      windowJs.windowCreateIfNotThere(argv.httpPort)
    })
  }

  app.on('will-quit', () => {
    startup.shutdown()
  })

  app.on(
    'second-instance',
    (event: Event, commandLine: string[], workingDirectory: string) => {
      env.logInfo(`Zap instance started with command line: ${commandLine}`)
    }
  )
}

let argv = args.processCommandLineArguments(process.argv)

util.mainOrSecondaryInstance(
  argv.reuseZapInstance,
  () => hookMainInstanceEvents(argv),
  () => hookSecondInstanceEvents(argv)
)
