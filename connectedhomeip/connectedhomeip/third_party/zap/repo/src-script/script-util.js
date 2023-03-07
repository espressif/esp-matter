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
const { spawn } = require('cross-spawn')
const folderHash = require('folder-hash')
const fs = require('fs')
const fsp = fs.promises
const path = require('path')
const scriptUtil = require('./script-util.js')
const readline = require('readline')

const spaDir = path.join(__dirname, '../spa')
const backendDir = path.join(__dirname, '../dist')
const spaHashFileName = path.join(spaDir, 'hash.json')
const backendHashFileName = path.join(backendDir, 'hash.json')
process.env.PATH = process.env.PATH + ':./node_modules/.bin/'

const hashOptions = {}

// Utilities shared by scripts.

/**
 * Execute a command and resolve with the context.
 *
 * @param {*} ctx
 * @param {*} cmd
 * @param {*} args
 */
async function executeCmd(ctx, cmd, args) {
  return new Promise((resolve, reject) => {
    console.log(`🚀 Executing: ${cmd} ${args.join(' ')}`)
    let c = spawn(cmd, args)
    c.on('exit', (code) => {
      if (code == 0) resolve(ctx)
      else {
        if (code) {
          console.log(`👎 Program ${cmd} exited with error code: ${code}`)
          reject(code)
        } else {
          console.log(
            `👎 Program ${cmd} exited with signal code: ${c.signalCode}`
          )
          reject(c.signalCode)
        }
      }
    })
    c.stdout.on('data', (data) => {
      process.stdout.write(data)
    })
    c.stderr.on('data', (data) => {
      process.stderr.write('⇝ ' + data)
    })
    c.on('error', (err) => {
      reject(err)
    })
  })
}

/**
 * Executes a command with arguments and resolves with the stdout.
 *
 * @param {*} onError If there is an error with executable, resolve to this.
 * @param {*} cmd Command to run.
 * @param {*} args Arguments to pass.
 */
async function getStdout(onError, cmd, args) {
  return new Promise((resolve, reject) => {
    console.log(`🚀 Executing: ${cmd} ${args.join(' ')}`)
    let c = spawn(cmd, args)
    let str = ''
    c.on('exit', (code) => {
      if (code == 0) resolve(str)
      else {
        console.log(`👎 Program ${cmd} exited with error code: ${code}`)
        reject(code)
      }
    })
    c.stdout.on('data', (data) => {
      str = str.concat(data)
    })
    c.on('error', (err) => {
      resolve(onError)
    })
  })
}

/**
 * Resolves into a context object.
 * Check for context.needsRebuild
 *
 * @returns
 */
async function rebuildSpaIfNeeded() {
  let srcHash = await folderHash.hashElement(
    path.join(__dirname, '../src'),
    hashOptions
  )
  console.log(`🔍 Current src hash: ${srcHash.hash}`)
  let srcSharedHash = await folderHash.hashElement(
    path.join(__dirname, '../src-shared'),
    hashOptions
  )
  console.log(`🔍 Current src-shared hash: ${srcSharedHash.hash}`)
  let ctx = {
    hash: {
      srcHash: srcHash.hash,
      srcSharedHash: srcSharedHash.hash,
    },
  }
  return Promise.resolve(ctx)
    .then(
      (ctx) =>
        new Promise((resolve, reject) => {
          fs.readFile(spaHashFileName, (err, data) => {
            let oldHash = null
            if (err) {
              console.log(`👎 Error reading old hash file: ${spaHashFileName}`)
              ctx.needsRebuild = true
            } else {
              oldHash = JSON.parse(data)
              console.log(`🔍 Previous src hash: ${oldHash.srcHash}`)
              console.log(
                `🔍 Previous src-shared hash: ${oldHash.srcSharedHash}`
              )
              ctx.needsRebuild =
                oldHash.srcSharedHash != ctx.hash.srcSharedHash ||
                oldHash.srcHash != ctx.hash.srcHash
            }
            if (ctx.needsRebuild) {
              console.log(
                `🐝 Front-end code changed, so we need to rebuild SPA.`
              )
            } else {
              console.log(
                `👍 There were no changes to front-end code, so we don't have to rebuild the SPA.`
              )
            }
            resolve(ctx)
          })
        })
    )
    .then((ctx) => {
      if (ctx.needsRebuild)
        return scriptUtil.executeCmd(ctx, 'npx', ['quasar', 'build'])
      else return Promise.resolve(ctx)
    })
    .then(
      (ctx) =>
        new Promise((resolve, reject) => {
          if (ctx.needsRebuild) {
            console.log('✍ Writing out new hash file.')
            fs.writeFile(spaHashFileName, JSON.stringify(ctx.hash), (err) => {
              if (err) reject(err)
              else resolve(ctx)
            })
          } else {
            resolve(ctx)
          }
        })
    )
}

async function rebuildBackendIfNeeded() {
  return scriptUtil
    .executeCmd({}, 'npx', ['tsc', '--build', './tsconfig.json'])
    .then(() =>
      scriptUtil.executeCmd({}, 'npx', [
        'copyfiles',
        '-V',
        './src-electron/**/*.sql',
        './dist/',
      ])
    )
}

/**
 * Executes:
 *   git log -1 --format="{\"hash\": \"%H\",\"date\": \"%cI\"}"
 * ads the timestamp and saves it into .version.json
 */
async function stampVersion() {
  try {
    let out = await getStdout('{"hash": null,"date": null}', 'git', [
      'log',
      '-1',
      '--format={"hash": "%H","timestamp": %ct}',
    ])
    let version = JSON.parse(out)
    let d = new Date(version.timestamp * 1000) // git gives seconds, Date needs milliseconds
    version.date = d
    let versionFile = path.join(__dirname, '../.version.json')
    console.log(`🔍 Git commit: ${version.hash} from ${version.date}`)
    await fsp.writeFile(versionFile, JSON.stringify(version))

    await setPackageJsonVersion(d, 'real')
  } catch (err) {
    console.log(`Error retrieving version: ${err}`)
  }
}

/**
 * Sets the version in package.json
 * @param {*} mode 'fake', 'real' or 'print'
 */
async function setPackageJsonVersion(date, mode) {
  let promise = new Promise((resolve, reject) => {
    let packageJson = path.join(__dirname, '../package.json')
    let output = ''
    let cnt = 0
    let wasChanged = false

    const stream = fs.createReadStream(packageJson)
    const rl = readline.createInterface({
      input: stream,
      crlfDelay: Infinity,
    })

    rl.on('line', (line) => {
      if (cnt < 10 && line.includes('"version":')) {
        let output
        if (mode == 'real') {
          output = `  "version": "${date.getFullYear()}.${
            date.getMonth() + 1
          }.${date.getDate()}",`
        } else if (mode == 'fake') {
          output = `  "version": "0.0.0",`
        } else {
          output = line
        }

        if (output == line) {
          wasChanged = false
        } else {
          line = output
          wasChanged = true
        }
        versionPrinted = line
      }
      output = output.concat(line + '\n')
      cnt++
    })

    rl.on('close', () => {
      if (wasChanged) {
        fs.writeFileSync(packageJson, output)
      }
      resolve(wasChanged)
    })
  })
  return promise
}

/**
 * This method takes a nanosecond duration and prints out
 * decently human readable time out of it.
 *
 * @param {*} nsDifference
 * @returns
 */
function duration(nsDifference) {
  let diff = Number(nsDifference)
  let out = ''
  if (diff > 1000000000) {
    out += `${Math.floor(diff / 1000000000)}s `
  }
  out += `${Math.round((diff % 1000000000) / 1000000)}ms`
  return out
}

/**
 * Printout of timings at the end of a script.
 * This function also cleans up the package.json
 *
 * @param {*} startTime
 */
async function doneStamp(startTime) {
  let nsDuration = process.hrtime.bigint() - startTime
  console.log(`😎 All done: ${duration(nsDuration)}.`)
  return setPackageJsonVersion(null, 'fake')
}

/**
 * Main entry of the program.
 *
 * @param {*} isNode
 * @returns
 */
function mainPath(isElectron) {
  if (isElectron) {
    return path.join(__dirname, '../dist/src-electron/ui/main-ui.js')
  } else {
    return path.join(__dirname, '../dist/src-electron/main-process/main.js')
  }
}

/**
 * Simple function that reads a JSON file representing an array,
 * and adds an object to it. If file doesn't exist it will create it
 * with an array containing the passed object.
 *
 * @param {*} file
 * @param {*} object
 */
async function addToJsonFile(file, object) {
  let json
  if (fs.existsSync(file)) {
    let data = await fsp.readFile(file)
    json = JSON.parse(data)
  } else {
    json = []
  }
  json.push(object)
  await fsp.writeFile(file, JSON.stringify(json, null, 2))
}

exports.executeCmd = executeCmd
exports.rebuildSpaIfNeeded = rebuildSpaIfNeeded
exports.rebuildBackendIfNeeded = rebuildBackendIfNeeded
exports.stampVersion = stampVersion
exports.duration = duration
exports.doneStamp = doneStamp
exports.mainPath = mainPath
exports.setPackageJsonVersion = setPackageJsonVersion
exports.addToJsonFile = addToJsonFile
