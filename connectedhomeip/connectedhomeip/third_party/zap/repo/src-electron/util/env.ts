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
const os = require('os')
import fs from 'fs'
const pino = require('pino')
const zapBaseUrl = 'http://localhost:'
const zapUrlLog = 'zap.url'
import { VersionType, ErrorType } from '../types/env-types'

export function builtinSilabsZclMetafile() {
  return locateProjectResource('./zcl-builtin/silabs/zcl.json')
}

export function builtinMatterZclMetafile() {
  return locateProjectResource('./zcl-builtin/matter/zcl.json')
}

export function builtinDotdotZclMetafile() {
  return locateProjectResource('./zcl-builtin/dotdot/library.xml')
}

export function builtinTemplateMetafile() {
  return null // No default.
}

export const environmentVariable = {
  logLevel: {
    name: 'ZAP_LOGLEVEL',
    description: 'Sets the log level. If unset, then default is: warn.',
  },
  uniqueStateDir: {
    name: 'ZAP_TEMPSTATE',
    description:
      'If set to 1, then instead of .zap, a unique temporary state directory will be created.',
  },
  stateDir: {
    name: 'ZAP_DIR',
    description:
      'Sets a state directory. Can be overriden by --stateDirectory option. If unset, default is: ~/.zap',
  },
  skipPostGen: {
    name: 'ZAP_SKIP_POST_GENERATION',
    description:
      'If there is a defined post-generation action for zap, you can set this to variable to 1 to skip it.',
  },
  reuseZapInstance: {
    name: 'ZAP_REUSE_INSTANCE',
    description:
      'If set to 1, default behavior of zap will be to reuse existing instance.',
  },
  cleanupDelay: {
    name: 'ZAP_CLEANUP_DELAY',
    description:
      'Amount of millisecons zap will wait for cleanups to perform. This is workaround for some SQLite bug. If unset, default is: 1500',
  },
  jenkinsHome: {
    name: 'JENKINS_HOME',
    description:
      'When this env variable is present, zap will assume Jenkins environment. That will assume ZAP_TEMPSTATE and ZAP_SKIP_POST_GENERATION to be 1 by default.',
  },
}

// builtin pino levels: trace=10, debug=20, info=30, warn=40
const pinoOptions = {
  name: 'zap',
  level: process.env[environmentVariable.logLevel.name] || 'info', // This sets the default log level. If you set this, to say `sql`, then you will get SQL queries.
  customLevels: {
    fatal: 60,
    error: 50,
    warn: 40,
    info: 30,
    ipc: 27,
    browser: 25,
    sql: 22,
    debug: 20,
    trace: 10,
    all: 1,
  },
}

// Basic environment tie-ins
let pino_logger = pino(pinoOptions)

let explicit_logger_set = false
let httpStaticContentPath = path.join(__dirname, '../../../spa')
let versionObject: VersionType | null = null
let applicationStateDirectory: string | null = null

export function setDevelopmentEnv() {
  // @ts-ignore
  process.env.DEV = true
  // @ts-ignore
  global.__statics = path.join('src', 'statics').replace(/\\/g, '\\\\')
  httpStaticContentPath = path.join(__dirname, '../../spa')
  // @ts-ignore
  global.__backend = path.join(__dirname, '../').replace(/\\/g, '\\\\')
}

export function setProductionEnv() {
  // @ts-ignore
  global.__statics = path.join(__dirname, 'statics').replace(/\\/g, '\\\\')
  // @ts-ignore
  global.__backend = path.join(__dirname, '../').replace(/\\/g, '\\\\')
  httpStaticContentPath = path
    .join(__dirname, '../../../spa')
    .replace(/\\/g, '\\\\')
}

export function logInitStdout() {
  if (!explicit_logger_set) {
    pino_logger = pino(pinoOptions, pino.destination(1))
    explicit_logger_set = true
  }
}

export function logInitLogFile() {
  if (!explicit_logger_set) {
    pino_logger = pino(
      pinoOptions,
      pino.destination(path.join(appDirectory(), 'zap.log'))
    )
    explicit_logger_set = true
  }
}

/**
 * Set the state directory. This method is intended to be called
 * only at the application startup, when CLI args are being parsed.
 * This method honors '~/' being the first characters in its argument.
 *
 * @param {*} path Absolute path. Typically '~/.zap'.
 */
export function setAppDirectory(directoryPath: string) {
  let appDir
  if (directoryPath.startsWith('~/')) {
    appDir = path.join(os.homedir(), directoryPath.substring(2))
  } else {
    appDir = directoryPath
  }
  if (!fs.existsSync(appDir)) {
    fs.mkdirSync(appDir, { recursive: true })
  }
  applicationStateDirectory = appDir
}

/**
 * Returns an app directory. It creates it, if it doesn't exist
 *
 * @returns state directory, which is guaranteed to be already existing
 */
export function appDirectory() {
  if (applicationStateDirectory == null) {
    let appDir = path.join(os.homedir(), '.zap')
    if (!fs.existsSync(appDir)) {
      fs.mkdirSync(appDir, { recursive: true })
    }
    applicationStateDirectory = appDir
    return appDir
  }
  return applicationStateDirectory
}

export function iconsDirectory() {
  // @ts-ignore
  return path.join(global.__backend, '/icons')
}

export function schemaFile() {
  // @ts-ignore
  return path.join(global.__backend, '/db/zap-schema.sql')
}

export function sqliteFile(filename = 'zap') {
  return path.join(appDirectory(), `${filename}.sqlite`)
}

export function sqliteTestFile(id: string, deleteExistingFile = true) {
  let dir = path.join(__dirname, '../../test/.zap')
  if (!fs.existsSync(dir)) {
    fs.mkdirSync(dir)
  }
  let fileName = path.join(dir, `test-${id}.sqlite`)
  if (deleteExistingFile && fs.existsSync(fileName)) fs.unlinkSync(fileName)
  return fileName
}
/**
 * Returns a version as a single on-line string.
 *
 */
export function zapVersionAsString() {
  let vo = zapVersion()
  return `ver. ${vo.version}, featureLevel ${vo.featureLevel}, commit: ${
    vo.hash
  } from ${vo.date}${vo.source ? ', mode: source' : ', mode: binary'}`
}

/**
 * This function locates a resource in the project, such as various
 * JSON files and zcl-builtin stuff.
 *
 * It needs to adapt to a change in path that can occur when
 * things are copied into the dist/ directory.
 *
 * @param filePath
 * @returns
 */
export function locateProjectResource(filePath: string): string {
  if (fs.existsSync(path.join(__dirname, '../../zcl-builtin/'))) {
    return path.join(__dirname, '../../', filePath)
  } else if (fs.existsSync(path.join(__dirname, '../../../zcl-builtin/'))) {
    return path.join(__dirname, '../../../', filePath)
  } else {
    throw new Error(
      `Zap integrity error: can not locate resource: ${filePath}.`
    )
  }
}
/**
 * Returns the zap version.
 *
 * @returns zap version, which is an object that
 * contains 'version', 'featureLevel', 'hash', 'timestamp' and 'date'
 */
export function zapVersion() {
  if (versionObject == null) {
    versionObject = {
      version: '',
      featureLevel: 0,
      hash: 0,
      timestamp: 0,
      date: '',
    }
    try {
      let p = require(locateProjectResource('./package.json'))
      versionObject.version = p.version
    } catch (err) {
      logError('Could not retrieve version from package.json')
      versionObject.version = '0.0.0'
    }

    try {
      let p = require(locateProjectResource('./apack.json'))
      versionObject.featureLevel = p.featureLevel
    } catch (err) {
      logError('Could not retrieve featureLevel from apack.json')
      versionObject.featureLevel = 0
    }

    try {
      let ver = require(locateProjectResource('./.version.json'))
      versionObject.hash = ver.hash
      versionObject.timestamp = ver.timestamp
      versionObject.date = ver.date
    } catch {
      logError('Could not retrieve version from .version.json')
    }

    try {
      let readme = locateProjectResource('./README.md')
      if (fs.existsSync(readme)) {
        versionObject.source = true
      }
    } catch (err) {
      console.log(err)
      logError('Could not identify source vs binary mode.')
    }
  }
  return versionObject
}

export function baseUrl() {
  return zapBaseUrl
}

/**
 * Prints the data to stderr, without much fuss.
 * @param msg
 */
export function printToStderr(msg: string): void {
  console.error(msg)
}

/**
 * Base level common logger.
 *
 * @param {*} level
 * @param {*} msg
 * @param {*} err
 */
export function log(level: string, msg: string, err = null) {
  let objectToLog: ErrorType = {
    msg: msg,
  }
  if (err != null) {
    objectToLog.err = err
    // @ts-ignore
    objectToLog.err.alert = '⛔'
  }
  pino_logger[level](objectToLog)
}

/**
 * Info level message.
 *
 * @param {*} msg
 * @param {*} err
 */
export function logInfo(msg: string, err = null) {
  log('info', msg, err)
}

/**
 * Error level message.
 *
 * @param {*} msg
 * @param {*} err
 */
export function logError(msg: string, err = null) {
  log('error', msg, err)
}

/**
 * Warning level message.
 *
 * @param {*} msg
 * @param {*} err
 */
export function logWarning(msg: string, err = null) {
  log('warn', msg, err)
}

/**
 * Sql level message.
 *
 * @param {*} msg
 * @param {*} err
 */
export function logSql(msg: string, query: string | null, args: string | null) {
  if (query == null) {
    log('sql', msg)
  } else {
    // If you want a printout of all the queries for someting,
    // you can uncomment this next line. This is sometimes useful
    // in development environment to build a map of which queries
    // are bottlenecks, so they should be cached.
    //console.log(`SQL: ${query.replace(/\s/g, ' ')}`)
    log('sql', `${msg} => ${query}: ${args}`)
  }
}

/**
 * Browser level message.
 *
 * @param {*} msg
 * @param {*} err
 */
export function logBrowser(msg: string, err = null) {
  log('browser', msg, err)
}

/**
 * IPC level message.
 *
 * @param {*} msg
 * @param {*} err
 */
export function logIpc(msg: string, err = null) {
  log('ipc', msg, err)
}

/**
 * Debug level message.
 *
 * @param {*} msg
 * @param {*} err
 */
export function logDebug(msg: string, err = null) {
  log('debug', msg, err)
}

// Returns true if major or minor component of versions is different.
export function isMatchingVersion(
  versionsArray: string[],
  providedVersion: string
) {
  let ret = false
  let v2 = providedVersion.split('.')
  versionsArray.forEach((element) => {
    let v1 = element.split('.')
    if (v1.length != 3 || v2.length != 3) return

    if (v1[0] != 'x' && v1[0] != v2[0]) return
    if (v1[1] != 'x' && v1[1] != v2[1]) return
    if (v1[2] != 'x' && v1[2] != v2[2]) return

    ret = true
  })

  return ret
}

/**
 * Returns true if versions of node and electron are matching.
 * If versions are not matching, it  prints out a warhing
 * and returns false.
 *
 * @returns true or false, depending on match
 */
export function versionsCheck() {
  let expectedNodeVersion = ['v14.x.x', 'v16.x.x']
  let expectedElectronVersion = ['17.4.x', '18.x.x']
  let nodeVersion = process.version
  let electronVersion = process.versions.electron
  let ret = true
  if (!isMatchingVersion(expectedNodeVersion, nodeVersion)) {
    ret = false
    console.log(`Expected node versions: ${expectedNodeVersion}`)
    console.log(`Provided node version: ${nodeVersion}`)
    console.log(
      'WARNING: you are using different node version than recommended.'
    )
  }
  if (
    electronVersion != null &&
    !isMatchingVersion(expectedElectronVersion, electronVersion)
  ) {
    ret = false
    console.log(`Expected electron version: ${expectedElectronVersion}`)
    console.log(`Provided electron version: ${electronVersion}`)
    console.log(
      'WARNING: you are using different electron version that recommended.'
    )
  }
  return ret
}

/**
 * Returns path to HTTP static content while taking into account DEV / PROD modes.
 *
 * @returns full path to HTTP static content
 */
export function httpStaticContent() {
  return httpStaticContentPath
}
