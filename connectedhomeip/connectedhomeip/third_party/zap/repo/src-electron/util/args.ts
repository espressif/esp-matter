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

export {}

const yargs = require('yargs')
const path = require('path')
const os = require('os')
const fs = require('fs')
const restApi = require('../../src-shared/rest-api.js')
const commonUrl = require('../../src-shared/common-url.js')
const env = require('./env')

function environmentVariablesDescription() {
  let vars = env.environmentVariable
  let desc = ''
  Object.keys(vars).forEach((key) => {
    desc = desc.concat(`  ${vars[key].name}: ${vars[key].description}\n`)
  })
  return desc
}

export interface Arguments {
  [x: string]: unknown
  httpPort: number
  studioHttpPort: number
  zapFile: string | undefined
  zclProperties: string | undefined
  generationTemplate: string | undefined
  uiMode: string | undefined
  debugNavBar: boolean
  noUi: boolean
  noServer: boolean
  genResultFile: boolean
  showUrl: boolean
  output: string | undefined
  clearDb: string | undefined
  stateDirectory: string | undefined
  tempState: boolean
  skipPostGeneration: boolean
  noZapFileLog: boolean
  reuseZapInstance: boolean
  watchdogTimer: number
  allowCors: boolean
  postImportScript: string | undefined
  _: string[]
  $0: string
}

/**
 * Process the command line arguments and resets the state in this file
 * to the specified values.
 *
 * @export
 * @param {*} argv
 * @returns parsed argv object
 */
export function processCommandLineArguments(argv: string[]) {
  let zapVersion = env.zapVersion()
  let commands = new Map([
    ['generate', 'Generate ZCL artifacts.'],
    ['selfCheck', 'Perform the self-check of the application.'],
    ['analyze', 'Analyze the zap file without doing anything.'],
    ['convert', 'Convert a zap or ISC file to latest zap file.'],
    ['status', 'Query the status of a zap server.'],
    ['server', 'Run zap in a server mode.'],
    ['stop', 'Stop zap server if one is running.'],
    ['new', 'If in client mode, start a new window on a main instance.'],
    ['regenerateSdk', 'Perform full SDK regeneration.'],
  ])
  let y = yargs
  for (let cmd of commands.entries()) {
    y.command(cmd[0], cmd[1])
  }
  let ret = y
    .option('httpPort', {
      desc: 'Port used for the HTTP server',
      alias: 'p',
      type: 'number',
      default: 9070,
    })
    .option('studioHttpPort', {
      desc: "Port used for integration with Silicon Labs Simplicity Studio's internal HTTP server",
      type: 'number',
      default: 9000,
    })
    .option('zapFile', {
      desc: 'input .zap file to read in. You can also specify them without an option, directly.',
      alias: ['zap', 'in', 'i'],
      type: 'string',
      default: null,
    })
    .option('zclProperties', {
      desc: 'zcl.properties file to read in.',
      alias: ['zcl', 'z'],
      type: 'array',
      default: env.builtinSilabsZclMetafile(),
    })
    .option('sdk', {
      desc: 'sdk.json file to read, for operations that act on whole SDK',
      type: 'string',
      default: null,
    })
    .option('generationTemplate', {
      desc: 'generation template metafile (gen-template.json) to read in.',
      alias: ['gen', 'g'],
      type: 'string',
      default: env.builtinTemplateMetafile(),
    })
    .option('uiMode', {
      desc: 'Mode of the UI to begin in. Options are: ZIGBEE',
      alias: 'ui',
      type: 'string',
      default: restApi.uiMode.ZIGBEE,
    })
    .option('debugNavBar', {
      desc: 'Boolean for when you want to embed purely the ZCL parts of the ZAP tool',
      alias: 'embed',
      type: 'boolean',
      default: exports.debugNavBar,
    })
    .option('noUi', {
      desc: "Don't show the main window when starting.",
    })
    .options('noServer', {
      desc: "Don't run the http or IPC server. You should probably also specify -noUi with this.",
      type: 'boolean',
      default: false,
    })
    .options('genResultFile', {
      desc: 'If this option is present, then generate the result file.',
      type: 'boolean',
      default: false,
    })
    .option('showUrl', {
      desc: 'Print out the URL that an external browser should use.',
    })
    .option('output', {
      desc: 'Specifying the output directory for generation or output file for conversion.',
      alias: ['out', 'o'],
      type: 'string',
    })
    .option('clearDb', {
      desc: 'Clear out the database and start with a new file.',
      type: 'string',
    })
    .option('stateDirectory', {
      desc: 'Sets the state directory.',
      default: process.env[env.environmentVariable.stateDir.name] || '~/.zap',
    })
    .option('jenkins', {
      desc: 'Assume jenkins environment, enables tempState and skipPostGeneration.',
      type: 'boolean',
      default: process.env[env.environmentVariable.jenkinsHome.name] != null,
    })
    .option('tempState', {
      desc: 'Use a unique temporary directory for state',
      type: 'boolean',
      default: process.env[env.environmentVariable.uniqueStateDir.name] == '1',
    })
    .option('skipPostGeneration', {
      desc: 'If there is a defined post-generation action for zap, you can set this to variable to 1 to skip it.',
      type: 'boolean',
      default: process.env[env.environmentVariable.skipPostGen.name] == '1',
    })
    .option('noZapFileLog', {
      desc: `When writing out the .zap files, don't include the log. Useful in unit testing, where timestamps otherwise cause diffs.`,
      type: 'boolean',
      default: false,
    })
    .option('cleanupDelay', {
      desc: 'When shutting down zap, this provides a number of millisecons to wait for SQLite to perform cleanup. Default is: 1500',
      type: 'number',
      default: process.env[env.environmentVariable.cleanupDelay.name] || '1500',
    })
    .option('reuseZapInstance', {
      desc: `When starting zap, should zap attempt to reuse an instance of previous zap already running.`,
      type: 'boolean',
      default:
        process.env[env.environmentVariable.reuseZapInstance.name] == '1',
    })
    .option('watchdogTimer', {
      desc: `In a server mode, how long of no-activity (in ms) shuts down the server.`,
      type: 'number',
      default: 300000, // = 5 minutes
    })
    .option('allowCors', {
      desc: `Sets the CORS policy to be enabled or disabled.`,
      type: 'boolean',
      default: false,
    })
    .option('postImportScript', {
      desc: 'Script to execute after data is loaded.',
      type: 'string',
      default: null,
    })
    .option('results', {
      desc: 'Specifying the output YAML file to capture convert results.',
      type: 'string',
    })
    .usage('Usage: $0 <command> [options] ... [file.zap] ...')
    .version(
      `Version: ${zapVersion.version}\nFeature level: ${
        zapVersion.featureLevel
      }\nHash: ${zapVersion.hash}\nDate: ${zapVersion.date}${
        zapVersion.source ? '\nMode: source' : '\nMode: binary'
      }`
    )
    .help()
    .alias({
      help: ['h', '?'],
    })
    .epilogue(
      `Environment variables:
${environmentVariablesDescription()}
For more information, see ${commonUrl.projectUrl}`
    )
    .wrap(null)
    .parse(argv)

  // Apply Jenkins logic.
  if (ret.jenkins) {
    console.log(
      '🔧 Detected Jenkins environment. Making necessary adjustments.'
    )
    if (process.env[env.environmentVariable.skipPostGen.name] == null) {
      ret.skipPostGen = true
    }
    if (process.env[env.environmentVariable.uniqueStateDir.name] == null) {
      ret.tempState = true
    }
  }

  // Collect files that are passed as loose arguments
  let allFiles = ret._.filter((arg: string | number, index: number) => {
    if (index == 0) return false
    if (typeof arg == 'number') return false
    if (arg.endsWith('.js')) return false
    if (commands.has(arg)) return false
    return true
  })
  if (ret.zapFile != null) allFiles.push(ret.zapFile)
  ret.zapFiles = allFiles

  if (ret.tempState) {
    let tempDir = fs.mkdtempSync(`${os.tmpdir()}${path.sep}zap.`)
    console.log(`🔧 Using temporary state directory: ${tempDir}`)
    env.setAppDirectory(tempDir)
  } else {
    console.log(`🔧 Using state directory: ${ret.stateDirectory}`)
    env.setAppDirectory(ret.stateDirectory)
  }

  return ret
}
