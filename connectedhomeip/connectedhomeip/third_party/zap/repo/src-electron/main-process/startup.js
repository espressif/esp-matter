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

const fs = require('fs')
const fsp = fs.promises
const path = require('path')
const _ = require('lodash')
const YAML = require('yaml')

const dbApi = require('../db/db-api.js')
const dbEnum = require('../../src-shared/db-enum.js')
const env = require('../util/env')
const zclLoader = require('../zcl/zcl-loader.js')
const httpServer = require('../server/http-server.js')
const ipcServer = require('../server/ipc-server')
const ipcClient = require('../client/ipc-client')
const generatorEngine = require('../generator/generation-engine.js')
const querySession = require('../db/query-session.js')
const util = require('../util/util.js')
const importJs = require('../importexport/import.js')
const exportJs = require('../importexport/export.js')
const watchdog = require('./watchdog')
const sdkUtil = require('../util/sdk-util')

// This file contains various startup modes.

let mainDatabase = null

/**
 * Start up application in a normal mode.
 *
 * @param {*} uiEnabled
 * @param {*} showUrl
 * @param {*} uiMode
 * @param {*} zapFiles An array of .zap files to open, can be empty.
 */
async function startNormal(quitFunction, argv) {
  let showUrl = argv.showUrl
  let db = await dbApi.initDatabaseAndLoadSchema(
    env.sqliteFile(),
    env.schemaFile(),
    env.zapVersion()
  )

  watchdog.start(argv.watchdogTimer, () => {
    if (quitFunction != null) {
      quitFunction()
    } else {
      process.exit(0)
    }
  })
  mainDatabase = db

  try {
    await zclLoader.loadZclMetafiles(db, argv.zclProperties)
    let ctx = await generatorEngine.loadTemplates(db, argv.generationTemplate)

    if (ctx.error) {
      env.logWarning(ctx.error)
    }

    if (!argv.noServer) {
      await httpServer.initHttpServer(
        ctx.db,
        argv.httpPort,
        argv.studioHttpPort,
        {
          zcl: argv.zclProperties,
          template: argv.generationTemplate,
          allowCors: argv.allowCors,
        }
      )
      await ipcServer.initServer(ctx.db, argv.httpPort)
    }
    let port = httpServer.httpServerPort()

    if (showUrl) {
      // NOTE: this is parsed/used by Studio as the default landing page.
      console.log(httpServer.httpServerStartupMessage())
    }

    return port
  } catch (err) {
    env.logError(err)
    throw err
  }
}

/**
 * Returns the output file out of input file and a pattern
 *
 * @param {*} inputFile
 * @param {*} outputPattern
 * @returns the path to the output file.
 */
function outputFile(inputFile, outputPattern, index = 0) {
  let output = outputPattern
  let hadDir = false

  if (output.startsWith('{dir}/')) {
    let dir = path.dirname(inputFile)
    output = path.join(dir, output.substring(6))
    hadDir = true
  }

  if (output.includes('{')) {
    let dir = path.dirname(inputFile)
    let name = path.basename(inputFile)
    let basename
    let i = name.indexOf('.')
    if (i == -1) {
      basename = name
    } else {
      basename = name.substring(0, i)
    }
    output = output.replace('{name}', name)
    output = output.replace('{basename}', basename)
    output = output.replace('{index}', index)
    if (!hadDir) {
      if (!output.startsWith('/')) output = path.join(dir, output)
    }
  }
  return output
}

const BLANK_SESSION = '-- blank session --'
/**
 * This method gathers all the files to process.
 *
 * @param {*} filesArg array of files arguments
 * @param {*} options
 */
function gatherFiles(filesArg, options = { suffix: '.zap', doBlank: true }) {
  let list = []
  if (filesArg == null || filesArg.length == 0) {
    if (options.doBlank) list.push(BLANK_SESSION)
  } else {
    filesArg.forEach((f) => {
      let stat = fs.statSync(f)
      if (stat.isDirectory()) {
        let dirents = fs.readdirSync(f, { withFileTypes: true })
        dirents.forEach((element) => {
          if (
            element.name.endsWith(options.suffix.toLowerCase()) ||
            element.name.endsWith(options.suffix.toUpperCase())
          ) {
            list.push(path.join(f, element.name))
          }
        })
      } else {
        list.push(f)
      }
    })
  }
  return list
}

/**
 * Perform file conversion.
 *
 * @param {*} files
 * @param {*} output
 */
async function startConvert(argv, options) {
  let files = argv.zapFiles
  let output = argv.output
  let conversion_results = argv.results
  options.logger(`🤖 Conversion started
    🔍 input files: ${files}
    🔍 output pattern: ${output}`)

  let dbFile = env.sqliteFile('convert')
  let db = await dbApi.initDatabaseAndLoadSchema(
    dbFile,
    env.schemaFile(),
    env.zapVersion()
  )
  options.logger('    🐝 database and schema initialized')
  await zclLoader.loadZclMetafiles(db, argv.zclProperties)
  options.logger(`    🐝 zcl package loaded: ${argv.zclProperties}`)
  if (argv.generationTemplate != null) {
    await generatorEngine.loadTemplates(db, argv.generationTemplate)
    options.logger(`    🐝 templates loaded: ${argv.generationTemplate}`)
  }

  await util.executePromisesSequentially(files, async (singlePath, index) => {
    let importResult = await importJs.importDataFromFile(db, singlePath, {
      defaultZclMetafile: argv.zclProperties,
      postImportScript: argv.postImportScript,
    })

    let sessionId = importResult.sessionId

    await util.initializeSessionPackage(db, sessionId, {
      zcl: argv.zclProperties,
      template: argv.generationTemplate,
    })

    if (argv.postImportScript) {
      await importJs.executePostImportScript(
        db,
        importResult.sessionId,
        argv.postImportScript
      )
    }

    options.logger(`    👈 read in: ${singlePath}`)
    let of = outputFile(singlePath, output, index)
    let parent = path.dirname(of)
    if (!fs.existsSync(parent)) {
      fs.mkdirSync(parent, { recursive: true })
    }
    // Now we need to write the sessionKey for the file path
    await querySession.updateSessionKeyValue(
      db,
      sessionId,
      dbEnum.sessionKey.filePath,
      of
    )
    let outputPath = await exportJs.exportDataIntoFile(db, sessionId, of, {
      removeLog: argv.noZapFileLog,
      createBackup: true,
    })

    options.logger(`    👉 write out: ${outputPath}`)
  })

  try {
    if (conversion_results != null)
      await fsp.writeFile(
        conversion_results,
        YAML.stringify({
          upgrade_results: [
            {
              message:
                'Zigbee Cluster Configurator configuration has been successfully upgraded.',
              status: 'automatic',
            },
          ],
        })
      )
    options.logger(`    👉 write out: ${conversion_results}`)
  } catch (error) {
    options.logger(`    ⚠️  failed to write out: ${conversion_results}`)
  }

  options.logger('😎 Conversion done!')
  if (options.quitFunction != null) {
    options.quitFunction()
  }
}

/**
 * Performs a full SDK regeneration.
 *
 * @param {*} argv
 * @param {*} options
 */
async function startRegenerateSdk(argv, options) {
  options.logger('🤖 Regenerating whole SDK.')
  let sdkPath = argv.sdk
  if (!sdkPath) {
    options.logger(`⛔ regenerateSdk requires the --sdk <sdkFile> argument`)
  } else {
    let dbFile = env.sqliteFile('regenerateSdk')
    let db = await dbApi.initDatabaseAndLoadSchema(
      dbFile,
      env.schemaFile(),
      env.zapVersion()
    )

    let sdk = await sdkUtil.readSdkJson(sdkPath, options)

    options.logger('🐝 Loading ZCL information')
    sdk.zclPackageId = {}
    for (let key of Object.keys(sdk.rt.zclMetafiles)) {
      let p = sdk.rt.zclMetafiles[key]
      options.logger(`    👈 ${p}`)
      let loadData = await zclLoader.loadZcl(db, p)
      sdk.zclPackageId[key] = loadData.packageId
    }
    options.logger('🐝 Loading generation templates')
    sdk.templatePackageId = {}
    for (let key of Object.keys(sdk.rt.genTemplates)) {
      let p = sdk.rt.genTemplates[key]
      options.logger(`    👈 ${p}`)
      let loadData = await generatorEngine.loadTemplates(db, p)
      sdk.templatePackageId[key] = loadData.packageId
    }
    options.logger('🐝 Performing generation')
    for (let gen of sdk.rt.generateCommands) {
      let inputFile = gen.inputFile
      let outputDirectory = gen.outputDirectory
      options.logger(`    👈 loading: ${inputFile} `)
      let loaderResult = await importJs.importDataFromFile(db, inputFile)
      let sessionId = loaderResult.sessionId
      let templateKeys = []
      if (gen.template != null) {
        if (Array.isArray(gen.template)) {
          templateKeys.push(...gen.template)
        } else {
          templateKeys.push(gen.template)
        }
      }
      for (let tK of templateKeys) {
        options.logger(`    👉 generating: ${tK} => ${outputDirectory}`)
        await generatorEngine.generateAndWriteFiles(
          db,
          sessionId,
          sdk.templatePackageId[tK],
          outputDirectory,
          {
            logger: (msg) => {
              console.log(`    ${msg}`)
            },
            backup: false,
            genResultFile: false,
            skipPostGeneration: false,
          }
        )
      }
    }
    options.logger('😎 Regeneration done!')
  }
  if (options.quitFunction != null) options.quitFunction()
}

/**
 * Perform file analysis.
 *
 * @param {*} paths List of paths to analyze
 * @param {boolean} [options={ log: true, quit: true }]
 */
async function startAnalyze(argv, options) {
  let paths = argv.zapFiles
  let dbFile = env.sqliteFile('analysis')
  options.logger(`🤖 Starting analysis: ${paths}`)
  if (options.cleanDb && fs.existsSync(dbFile)) {
    options.logger('    👉 remove old database file')
    fs.unlinkSync(dbFile)
  }
  let db = await dbApi.initDatabaseAndLoadSchema(
    dbFile,
    env.schemaFile(),
    env.zapVersion()
  )
  options.logger('    👉 database and schema initialized')
  await zclLoader.loadZclMetafiles(db, argv.zclProperties)
  await util.executePromisesSequentially(paths, (singlePath) =>
    importJs
      .importDataFromFile(db, singlePath, {
        defaultZclMetafile: argv.zclProperties,
        postImportScript: argv.postImportScript,
      })
      .then((importResult) => util.sessionReport(db, importResult.sessionId))
      .then((report) => {
        options.logger(`🤖 File: ${singlePath}\n`)
        options.logger(report)
      })
  )
  options.logger('😎 Analysis done!')
  if (options.quitFunction != null) options.quitFunction()
}

/**
 * Starts zap in a server mode.
 *
 * @param {*} options
 * @returns promise of a startup
 */
async function startServer(argv, quitFunction) {
  let db = await dbApi.initDatabaseAndLoadSchema(
    env.sqliteFile(),
    env.schemaFile(),
    env.zapVersion()
  )

  watchdog.start(argv.watchdogTimer, () => {
    if (quitFunction != null) {
      quitFunction()
    } else {
      process.exit(0)
    }
  })
  mainDatabase = db
  try {
    await zclLoader.loadZclMetafiles(db, argv.zclProperties)
    let ctx = await generatorEngine.loadTemplates(db, argv.generationTemplate)
    if (ctx.error) {
      env.logWarning(ctx.error)
    }
    await httpServer.initHttpServer(
      ctx.db,
      argv.httpPort,
      argv.studioHttpPort,
      {
        zcl: argv.zclProperties,
        template: argv.generationTemplate,
        allowCors: argv.allowCors,
      }
    )
    await ipcServer.initServer(ctx.db, argv.httpPort)
    logRemoteData(httpServer.httpServerStartupMessage())
  } catch (err) {
    env.logError(err)
    throw err
  }
}

/**
 * Start up applicationa in self-check mode.
 */
async function startSelfCheck(
  argv,
  options = {
    quitFunction: null,
    cleanDb: true,
    logger: console.log,
  }
) {
  env.logInitStdout()
  options.logger('🤖 Starting self-check')
  let dbFile = env.sqliteFile('self-check')
  if (options.cleanDb && fs.existsSync(dbFile)) {
    options.logger('    👉 remove old database file')
    fs.unlinkSync(dbFile)
  }
  let mainDb = await dbApi.initDatabaseAndLoadSchema(
    dbFile,
    env.schemaFile(),
    env.zapVersion()
  )
  options.logger('    👉 database and schema initialized')
  await zclLoader.loadZclMetafiles(mainDb, argv.zclProperties)
  options.logger('    👉 zcl data loaded')
  let ctx = await generatorEngine.loadTemplates(mainDb, argv.generationTemplate)
  if (ctx.error) {
    options.logger(`    ⚠️  ${ctx.error}`)
  } else {
    options.logger('    👉 generation templates loaded')
  }

  // This is a hack to prevent too quick shutdown that causes core dumps.
  dbApi.closeDatabaseSync(mainDb)
  options.logger('    👉 database closed')
  await util.waitFor(2000)
  options.logger('😎 Self-check done!')
  if (options.quitFunction != null) {
    options.quitFunction()
  }
}

async function generateSingleFile(
  db,
  f,
  templatePackageId,
  outputPattern,
  index,
  options = {
    logger: console.log,
    zcl: env.builtinSilabsZclMetafile(),
    template: env.builtinTemplateMetafile(),
    postImportScript: null,
  }
) {
  let hrstart = process.hrtime.bigint()
  let sessionId
  let output
  if (f === BLANK_SESSION) {
    options.logger(`👉 using empty configuration`)
    sessionId = await querySession.createBlankSession(db)
    output = outputPattern
  } else {
    options.logger(`👉 using input file: ${f}`)
    let importResult = await importJs.importDataFromFile(db, f, {
      defaultZclMetafile: options.zcl,
      postImportScript: options.postImportScript,
    })
    sessionId = importResult.sessionId
    output = outputFile(f, outputPattern, index)
  }
  options.logger(`👉 using output destination: ${output}`)

  await util.initializeSessionPackage(db, sessionId, options)

  let nsDuration = process.hrtime.bigint() - hrstart
  options.logger(`🕐 File loading time: ${util.duration(nsDuration)}`)

  options.fileLoadTime = nsDuration
  let genResult = await generatorEngine.generateAndWriteFiles(
    db,
    sessionId,
    templatePackageId,
    output,
    options
  )

  if (genResult.hasErrors) {
    console.log(JSON.stringify(genResult.errors))
    throw new Error(`Generation failed: ${f}`)
  }

  return genResult
}

/**
 * Performs headless regeneration for given parameters.
 *
 * @returns Nothing, triggers quit function
 */
async function startGeneration(argv, options) {
  let templateMetafile = argv.generationTemplate
  let zapFiles = argv.zapFiles
  let output = argv.output
  let zclProperties = argv.zclProperties
  let genResultFile = argv.genResultFile
  let skipPostGeneration = argv.skipPostGeneration

  let hrstart = process.hrtime.bigint()
  options.logger(
    `🤖 ZAP generation started: 
    🔍 input files: ${zapFiles}
    🔍 output pattern: ${output}
    🔍 using templates: ${templateMetafile}
    🔍 using zcl data: ${zclProperties}
    🔍 zap version: ${env.zapVersionAsString()}`
  )

  let dbFile = env.sqliteFile('generate')
  if (options.cleanDb && fs.existsSync(dbFile)) fs.unlinkSync(dbFile)
  let mainDb = await dbApi.initDatabaseAndLoadSchema(
    dbFile,
    env.schemaFile(),
    env.zapVersion()
  )

  await zclLoader.loadZclMetafiles(mainDb, zclProperties)
  let ctx = await generatorEngine.loadTemplates(mainDb, templateMetafile)
  if (ctx.error) {
    throw ctx.error
  }

  let files = gatherFiles(zapFiles, { suffix: '.zap', doBlank: true })
  if (files.length == 0) {
    options.logger(`    👎 no zap files found in: ${zapFiles}`)
    throw `👎 no zap files found in: ${zapFiles}`
  }

  options.zcl = zclProperties
  options.template = templateMetafile
  options.backup = false
  options.genResultFile = genResultFile
  options.skipPostGeneration = skipPostGeneration
  options.postImportScript = argv.postImportScript

  let nsDuration = process.hrtime.bigint() - hrstart
  options.logger(`🕐 Setup time: ${util.duration(nsDuration)} `)

  await util.executePromisesSequentially(files, (f, index) =>
    generateSingleFile(mainDb, f, ctx.packageId, output, index, options)
  )

  if (options.quitFunction != null) options.quitFunction()
}
/**
 * Move database file out of the way into the backup location.
 *
 * @param {*} path
 */
function clearDatabaseFile(dbPath) {
  util.createBackupFile(dbPath)
}

/**
 * Shuts down any servers that might be running.
 */
function shutdown() {
  env.logInfo('Shutting down HTTP and IPC servers...')
  ipcServer.shutdownServerSync()
  httpServer.shutdownHttpServerSync()

  if (mainDatabase != null) {
    // Use a sync call, because you can't have promises in the 'quit' event.
    try {
      dbApi.closeDatabaseSync(mainDatabase)
      mainDatabase = null
      env.logInfo('Database closed, shutting down.')
    } catch (err) {
      env.logError('Failed to close database.')
    }
  }
}

function logRemoteData(data) {
  if (data != null) {
    if (_.isString(data)) {
      console.log(data)
    } else {
      console.log('-- JSON START --')
      console.log(JSON.stringify(data))
      console.log('-- JSON END --')
    }
  }
}
/**
 * Startup method for the secondary instance.
 *
 * @param {*} argv
 */
function startUpSecondaryInstance(argv, callbacks) {
  console.log('🧐 Existing instance of zap will service this request.')
  ipcClient.initAndConnectClient().then(() => {
    ipcClient.on(ipcServer.eventType.overAndOut, (data) => {
      logRemoteData(data)
      if (callbacks.quitFunction != null) callbacks.quitFunction()
      else process.exit(0)
    })

    ipcClient.on(ipcServer.eventType.over, (data) => {
      logRemoteData(data)
    })
  })
  if (argv._.includes('status') || argv._.includes('server')) {
    ipcClient.emit(ipcServer.eventType.serverStatus)
  } else if (argv._.includes('convert') && argv.zapFiles != null) {
    ipcClient.emit(ipcServer.eventType.convert, {
      output: argv.output,
      files: argv.zapFiles,
    })
  } else if (argv._.includes('stop')) {
    ipcClient.emit(ipcServer.eventType.stop)
  } else if (argv._.includes('regenerateSdk')) {
    console.log('⛔ SDK regeneration from client process is not yet supported.')
    process.exit(0)
  } else if (argv._.includes('generate') && argv.zapFiles != null) {
    let data = {
      zapFileArray: argv.zapFiles,
      outputPattern: argv.output,
      zcl: argv.zclProperties,
      template: argv.generationTemplate,
    }
    ipcClient.emit(ipcServer.eventType.generate, data)
  }
}

function quit() {
  // Empty function by default. Startup sequence is supposed
  // to declare this depending on whether this is node or electron.
}

/**
 * Default startup method.
 *
 * @param {*} quitFunction
 * @param {*} argv
 */
async function startUpMainInstance(argv, callbacks) {
  let quitFunction = callbacks.quitFunction
  let uiFunction = callbacks.uiEnableFunction
  if (quitFunction != null) {
    exports.quit = () => {
      quitFunction()
    }
  } else {
    exports.quit = () => {
      process.exit(0)
    }
  }

  if (argv.logToStdout) {
    env.logInitStdout()
  } else {
    env.logInitLogFile()
  }

  // For now delete the DB file. There is some weird constraint we run into.
  if (argv.clearDb != null) {
    clearDatabaseFile(env.sqliteFile())
  }

  if (argv._.includes('status')) {
    console.log('⛔ Server is not running.')
    logRemoteData({ zapServerStatus: 'missing' })
    cleanExit(argv.cleanupDelay, 0)
  } else if (argv._.includes('selfCheck')) {
    let options = {
      quitFunction: quitFunction,
      cleanDb: true,
      logger: console.log,
    }
    return startSelfCheck(argv, options)
  } else if (argv._.includes('analyze')) {
    if (argv.zapFiles.length < 1)
      throw 'You need to specify at least one zap file.'
    let options = {
      quitFunction: quitFunction,
      cleanDb: true,
      logger: console.log,
    }
    return startAnalyze(argv, options)
  } else if (argv._.includes('server')) {
    return startServer(argv, quitFunction)
  } else if (argv._.includes('convert')) {
    if (argv.zapFiles.length < 1)
      throw 'You need to specify at least one zap file.'
    if (argv.output == null) throw 'You need to specify output file.'
    return startConvert(argv, {
      logger: console.log,
      quitFunction: quitFunction,
    }).catch((code) => {
      console.log(code)
      cleanExit(argv.cleanupDelay, 0)
    })
  } else if (argv._.includes('stop')) {
    console.log('No server running, nothing to stop.')
    cleanExit(argv.cleanupDelay, 0)
  } else if (argv._.includes('regenerateSdk')) {
    let options = {
      quitFunction: quitFunction,
      logger: console.log,
    }
    return startRegenerateSdk(argv, options)
  } else if (argv._.includes('generate')) {
    let options = {
      quitFunction: quitFunction,
      cleanDb: true,
      logger: console.log,
    }
    return startGeneration(argv, options).catch((err) => {
      console.log(err)
      env.printToStderr(`Zap generation error: ${err}`)
      cleanExit(argv.cleanupDelay, 1)
    })
  } else {
    // If we run with node only, we force no UI as it won't work.
    if (callbacks.uiEnableFunction == null) {
      argv.noUi = true
      argv.showUrl = true
      argv.standalone = false
    } else {
      argv.standalone = true
    }
    let uiEnabled = !argv.noUi
    let zapFiles = argv.zapFiles
    let port = await startNormal(quitFunction, argv)
    if (uiEnabled && uiFunction != null) {
      uiFunction(port, zapFiles, argv.uiMode, argv.standalone)
    } else {
      if (argv.showUrl) {
        // NOTE: this is parsed/used by Studio as the default landing page.
        logRemoteData(httpServer.httpServerStartupMessage())
      }
    }
  }
}

function cleanExit(delay, code) {
  util.waitFor(delay).then(() => process.exit(code))
}

exports.startGeneration = startGeneration
exports.startSelfCheck = startSelfCheck
exports.clearDatabaseFile = clearDatabaseFile
exports.startConvert = startConvert
exports.startAnalyze = startAnalyze
exports.startUpMainInstance = startUpMainInstance
exports.startUpSecondaryInstance = startUpSecondaryInstance
exports.shutdown = shutdown
exports.quit = quit
exports.generateSingleFile = generateSingleFile
