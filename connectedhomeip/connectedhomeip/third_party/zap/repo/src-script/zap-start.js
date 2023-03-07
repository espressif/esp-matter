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

const path = require('path')

const scriptUtil = require('./script-util.js')

//workaround: executeCmd()/spawn() fails silently without complaining about missing path to electron
process.env.PATH = process.env.PATH + ':/usr/local/bin/'
let startTime = process.hrtime.bigint()
let args = process.argv.slice(2)

scriptUtil
  .stampVersion()
  .then(() => scriptUtil.rebuildSpaIfNeeded())
  .then(() => scriptUtil.rebuildBackendIfNeeded())
  .then(() => {
    let plat = process.platform
    let executable
    let main

    switch (args[0]) {
      case 'selfCheck':
      case 'generate':
      case 'server':
      case 'stop':
      case 'status':
      case 'convert':
      case 'analyze':
      case 'regenerateSdk':
        executable = 'node'
        main = scriptUtil.mainPath(false)
        break
      default:
        executable = 'electron'
        main = scriptUtil.mainPath(true)
        break
    }
    if (args.includes('--version')) {
      executable = 'node'
      main = scriptUtil.mainPath(false)
    }

    let cmdArgs = [executable]
    if (executable == 'node') {
      cmdArgs.push('--unhandled-rejections=strict')
    }
    if (plat == 'linux' && executable == 'electron') {
      // This makes it safer to run on Linux, and latest linux distros broke this.
      cmdArgs.push('--disable-seccomp-filter-sandbox')
    }
    cmdArgs.push(main)

    if (process.platform == 'linux' && executable == 'electron') {
      if (!process.env.DISPLAY) {
        console.log(`
⛔ You are on Linux and you are attempting to run zap in UI mode without DISPLAY set.
⛔ Please set your DISPLAY environment variable or run zap-start.js with a command that does not require DISPLAY.`)
        process.exit(1)
      }
    }
    cmdArgs.push(...args)
    return scriptUtil.executeCmd(null, 'npx', cmdArgs)
  })
  .then(() => scriptUtil.doneStamp(startTime))
  .then(() => {
    process.exit(0)
  })
  .catch((err) => {
    console.log(err)
    process.exit(1)
  })
