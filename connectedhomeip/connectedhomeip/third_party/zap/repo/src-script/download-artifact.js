#!/usr/bin/env node
'use strict'
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
var __awaiter =
  (this && this.__awaiter) ||
  function (thisArg, _arguments, P, generator) {
    function adopt(value) {
      return value instanceof P
        ? value
        : new P(function (resolve) {
            resolve(value)
          })
    }
    return new (P || (P = Promise))(function (resolve, reject) {
      function fulfilled(value) {
        try {
          step(generator.next(value))
        } catch (e) {
          reject(e)
        }
      }
      function rejected(value) {
        try {
          step(generator['throw'](value))
        } catch (e) {
          reject(e)
        }
      }
      function step(result) {
        result.done
          ? resolve(result.value)
          : adopt(result.value).then(fulfilled, rejected)
      }
      step((generator = generator.apply(thisArg, _arguments || [])).next())
    })
  }
var __generator =
  (this && this.__generator) ||
  function (thisArg, body) {
    var _ = {
        label: 0,
        sent: function () {
          if (t[0] & 1) throw t[1]
          return t[1]
        },
        trys: [],
        ops: [],
      },
      f,
      y,
      t,
      g
    return (
      (g = { next: verb(0), throw: verb(1), return: verb(2) }),
      typeof Symbol === 'function' &&
        (g[Symbol.iterator] = function () {
          return this
        }),
      g
    )
    function verb(n) {
      return function (v) {
        return step([n, v])
      }
    }
    function step(op) {
      if (f) throw new TypeError('Generator is already executing.')
      while (_)
        try {
          if (
            ((f = 1),
            y &&
              (t =
                op[0] & 2
                  ? y['return']
                  : op[0]
                  ? y['throw'] || ((t = y['return']) && t.call(y), 0)
                  : y.next) &&
              !(t = t.call(y, op[1])).done)
          )
            return t
          if (((y = 0), t)) op = [op[0] & 2, t.value]
          switch (op[0]) {
            case 0:
            case 1:
              t = op
              break
            case 4:
              _.label++
              return { value: op[1], done: false }
            case 5:
              _.label++
              y = op[1]
              op = [0]
              continue
            case 7:
              op = _.ops.pop()
              _.trys.pop()
              continue
            default:
              if (
                !((t = _.trys), (t = t.length > 0 && t[t.length - 1])) &&
                (op[0] === 6 || op[0] === 2)
              ) {
                _ = 0
                continue
              }
              if (op[0] === 3 && (!t || (op[1] > t[0] && op[1] < t[3]))) {
                _.label = op[1]
                break
              }
              if (op[0] === 6 && _.label < t[1]) {
                _.label = t[1]
                t = op
                break
              }
              if (t && _.label < t[2]) {
                _.label = t[2]
                _.ops.push(op)
                break
              }
              if (t[2]) _.ops.pop()
              _.trys.pop()
              continue
          }
          op = body.call(thisArg, _)
        } catch (e) {
          op = [6, e]
          y = 0
        } finally {
          f = t = 0
        }
      if (op[0] & 5) throw op[1]
      return { value: op[0] ? op[1] : void 0, done: true }
    }
  }
var __spreadArray =
  (this && this.__spreadArray) ||
  function (to, from, pack) {
    if (pack || arguments.length === 2)
      for (var i = 0, l = from.length, ar; i < l; i++) {
        if (ar || !(i in from)) {
          if (!ar) ar = Array.prototype.slice.call(from, 0, i)
          ar[i] = from[i]
        }
      }
    return to.concat(ar || Array.prototype.slice.call(from))
  }
var __importDefault =
  (this && this.__importDefault) ||
  function (mod) {
    return mod && mod.__esModule ? mod : { default: mod }
  }
exports.__esModule = true
// This util script downloads ZAP artifact from Github.com
// Usage: node ./download-artifact.js $branch [ $commit | latest ]
var octokit_1 = require('octokit')
var http_status_codes_1 = require('http-status-codes')
var nodejs_file_downloader_1 = __importDefault(
  require('nodejs-file-downloader')
)
var yargs_1 = __importDefault(require('yargs'))
var node_os_1 = __importDefault(require('node:os'))
var is_reachable_1 = __importDefault(require('is-reachable'))
var axios_1 = __importDefault(require('axios'))
var path_1 = __importDefault(require('path'))
var fs_1 = __importDefault(require('fs'))
var date_fns_1 = require('date-fns')
// const
var DEBUG = false
var DEFAULT_COMMIT_LATEST = 'commit_latest'
var DEFAULT_BRANCH = 'master'
var DEFAULT_OWNER = 'SiliconLabs'
var DEFAULT_REPO = 'zap'
var NEXUS_SERVER = 'https://nexus.silabs.net'
var NEXUS_REPO_NAME = 'zap-release-package'
var nexusCachedBranches = ['master', 'rel']
// cheap and secure
process.env['NODE_TLS_REJECT_UNAUTHORIZED'] = '0'
function nexusRestApiUrl(repository, nameFilter) {
  return ''
    .concat(NEXUS_SERVER, '/service/rest/v1/search/assets?repository=')
    .concat(repository, '&name=')
    .concat(nameFilter)
}
function urlContent(url) {
  return __awaiter(this, void 0, void 0, function () {
    var resp
    return __generator(this, function (_a) {
      switch (_a.label) {
        case 0:
          return [4 /*yield*/, axios_1['default'].get(url)]
        case 1:
          resp = _a.sent()
          return [2 /*return*/, resp.data]
      }
    })
  })
}
function verifyPlatformAndFormat(name, platforms, formats) {
  // verify platform
  var verifyPlatform = platforms.reduce(function (prev, cur) {
    return prev || name.includes(cur)
  }, false)
  var verifyFormat = formats.reduce(function (prev, cur) {
    return prev || name.endsWith(cur)
  }, false)
  if (!verifyPlatform || !verifyFormat) {
    return false
  }
  return true
}
function githubDownloadArtifacts(
  artifacts,
  dlOptions,
  verifyPlatformAndFormat
) {
  return __awaiter(this, void 0, void 0, function () {
    var outputDir,
      githubToken,
      owner,
      repo,
      branch,
      commit,
      platforms,
      formats,
      _i,
      artifacts_1,
      artifact,
      archive_download_url,
      name_1,
      created_at,
      size_in_bytes
    return __generator(this, function (_a) {
      switch (_a.label) {
        case 0:
          ;(outputDir = dlOptions.outputDir),
            (githubToken = dlOptions.githubToken),
            (owner = dlOptions.owner),
            (repo = dlOptions.repo),
            (branch = dlOptions.branch),
            (commit = dlOptions.commit),
            (platforms = dlOptions.platforms),
            (formats = dlOptions.formats)
          if (artifacts.length == 0) {
            return [2 /*return*/]
          }
          console.log(
            'Repo: https://github.com/'
              .concat(owner, '/')
              .concat(repo, '/tree/')
              .concat(branch)
          )
          console.log(
            'Commit: https://github.com/'
              .concat(owner, '/')
              .concat(repo, '/commit/')
              .concat(
                commit === null || commit === void 0
                  ? void 0
                  : commit.substring(0, 7)
              )
          )
          if (!artifacts.length) return [3 /*break*/, 4]
          if (dlOptions.mirror) {
            outputDir = path_1['default'].join(
              outputDir,
              'artifacts',
              dlOptions.branch,
              artifacts[0].created_at
            )
          }
          console.log('Output directory: '.concat(outputDir))
          ;(_i = 0), (artifacts_1 = artifacts)
          _a.label = 1
        case 1:
          if (!(_i < artifacts_1.length)) return [3 /*break*/, 4]
          artifact = artifacts_1[_i]
          ;(archive_download_url = artifact.archive_download_url),
            (name_1 = artifact.name),
            (created_at = artifact.created_at),
            (size_in_bytes = artifact.size_in_bytes)
          if (!verifyPlatformAndFormat.call(null, name_1, platforms, formats)) {
            return [3 /*break*/, 3]
          }
          return [
            4 /*yield*/,
            download(
              archive_download_url,
              outputDir,
              githubToken,
              ''.concat(name_1, '.zip')
            ),
            // download metadata file.
          ]
        case 2:
          _a.sent()
          // download metadata file.
          try {
            fs_1['default'].writeFileSync(
              path_1['default'].join(outputDir, ''.concat(name_1, '.json')),
              JSON.stringify(artifact, null, 4)
            )
          } catch (err) {
            console.error(err)
          }
          _a.label = 3
        case 3:
          _i++
          return [3 /*break*/, 1]
        case 4:
          return [2 /*return*/]
      }
    })
  })
}
function githubListArtifacts(artifacts, dlOptions, verifyPlatformAndFormat) {
  return __awaiter(this, void 0, void 0, function () {
    var branch,
      outputDir,
      platforms,
      formats,
      artifactsList,
      _i,
      artifacts_2,
      artifact,
      name_2,
      artifactPath
    return __generator(this, function (_a) {
      ;(branch = dlOptions.branch),
        (outputDir = dlOptions.outputDir),
        (platforms = dlOptions.platforms),
        (formats = dlOptions.formats)
      if (artifacts.length == 0) {
        return [2 /*return*/]
      }
      if (artifacts.length) {
        if (dlOptions.mirror) {
          outputDir = path_1['default'].join(outputDir, 'artifacts')
        }
        artifactsList = []
        for (_i = 0, artifacts_2 = artifacts; _i < artifacts_2.length; _i++) {
          artifact = artifacts_2[_i]
          name_2 = artifact.name
          if (!verifyPlatformAndFormat.call(null, name_2, platforms, formats)) {
            continue
          }
          artifactPath = path_1['default'].join(
            branch,
            artifacts[0].created_at,
            ''.concat(name_2, '.zip')
          )
          if (DEBUG) console.log(''.concat(artifactPath))
          artifactsList.push(artifactPath)
        }
        try {
          fs_1['default'].mkdirSync(outputDir, { recursive: true })
          fs_1['default'].writeFileSync(
            path_1['default'].join(outputDir, ''.concat(branch, '.txt')),
            artifactsList.join('\n')
          )
        } catch (err) {
          console.error(err)
        }
      }
      return [2 /*return*/]
    })
  })
}
function download(archive_download_url, outDir, githubToken, name) {
  return __awaiter(this, void 0, void 0, function () {
    var chunkCount, chunkSize, downloader, error_1
    return __generator(this, function (_a) {
      switch (_a.label) {
        case 0:
          chunkCount = 0
          chunkSize = 200
          downloader = new nodejs_file_downloader_1['default']({
            url: archive_download_url,
            directory: outDir,
            cloneFiles: false,
            maxAttempts: 3,
            headers: {
              'User-Agent': 'Silabs Download Script',
              Authorization: 'token '.concat(githubToken),
            },
            onProgress: function (percentage, chunk, remainingSize) {
              chunkCount++
              if (chunkCount > chunkSize) {
                process.stdout.write('.')
                chunkCount = 0
              }
            },
          })
          _a.label = 1
        case 1:
          _a.trys.push([1, 3, , 4])
          process.stdout.write('Downloading '.concat(name, '...'))
          return [4 /*yield*/, downloader.download()] //Downloader.download() returns a promise.
        case 2:
          _a.sent() //Downloader.download() returns a promise.
          process.stdout.write('done\n')
          return [3 /*break*/, 4]
        case 3:
          error_1 = _a.sent()
          process.stdout.write('failed\n')
          console.error(error_1)
          return [3 /*break*/, 4]
        case 4:
          return [2 /*return*/]
      }
    })
  })
}
function platforms(argv) {
  var platformMap = {
    linux: 'linux',
    win32: 'win',
    darwin: 'mac',
  }
  var list = []
  if (argv.linux) {
    list.push('linux')
  }
  if (argv.win) {
    list.push('win')
  }
  if (argv.mac) {
    list.push('mac')
  }
  if (!argv.linux && !argv.win && !argv.mac) {
    list.push(platformMap[node_os_1['default'].platform()])
  }
  return list
}
function getExistingGithubBranches(options) {
  var _a
  return __awaiter(this, void 0, void 0, function () {
    var url, branches, resp, error_2
    return __generator(this, function (_b) {
      switch (_b.label) {
        case 0:
          url = 'https://api.github.com/repos/'
            .concat(options.owner, '/')
            .concat(options.repo, '/branches')
          branches = []
          _b.label = 1
        case 1:
          _b.trys.push([1, 3, , 4])
          if (DEBUG) console.log('GET: '.concat(url))
          return [4 /*yield*/, axios_1['default'].get(url)]
        case 2:
          resp = _b.sent()
          branches =
            (_a = resp === null || resp === void 0 ? void 0 : resp.data) ===
              null || _a === void 0
              ? void 0
              : _a.map(function (x) {
                  return x.name
                })
          return [3 /*break*/, 4]
        case 3:
          error_2 = _b.sent()
          return [3 /*break*/, 4]
        case 4:
          return [2 /*return*/, branches]
      }
    })
  })
}
function nexusGetArtifacts(baseUrl, options) {
  var _a, _b, _c, _d
  return __awaiter(this, void 0, void 0, function () {
    var accumulatedItems, continuationToken, resp, url, error_3
    return __generator(this, function (_e) {
      switch (_e.label) {
        case 0:
          accumulatedItems = []
          continuationToken = ''
          _e.label = 1
        case 1:
          _e.trys.push([1, 7, , 8])
          if (DEBUG) console.log('GET: '.concat(baseUrl))
          return [4 /*yield*/, axios_1['default'].get(baseUrl)]
        case 2:
          resp = _e.sent()
          accumulatedItems = accumulatedItems.concat(
            (_a = resp === null || resp === void 0 ? void 0 : resp.data) ===
              null || _a === void 0
              ? void 0
              : _a.items
          )
          continuationToken =
            (_b = resp === null || resp === void 0 ? void 0 : resp.data) ===
              null || _b === void 0
              ? void 0
              : _b.continuationToken
          url = ''
          _e.label = 3
        case 3:
          if (continuationToken) {
            url = baseUrl + '&continuationToken='.concat(continuationToken)
          } else {
            return [3 /*break*/, 6]
          }
          if (DEBUG) console.log('GET: '.concat(url))
          return [4 /*yield*/, axios_1['default'].get(url)]
        case 4:
          resp = _e.sent()
          accumulatedItems = accumulatedItems.concat(
            (_c = resp === null || resp === void 0 ? void 0 : resp.data) ===
              null || _c === void 0
              ? void 0
              : _c.items
          )
          continuationToken =
            (_d = resp === null || resp === void 0 ? void 0 : resp.data) ===
              null || _d === void 0
              ? void 0
              : _d.continuationToken
          _e.label = 5
        case 5:
          if (continuationToken) return [3 /*break*/, 3]
          _e.label = 6
        case 6:
          return [3 /*break*/, 8]
        case 7:
          error_3 = _e.sent()
          console.error(error_3)
          return [3 /*break*/, 8]
        case 8:
          return [2 /*return*/, accumulatedItems]
      }
    })
  })
}
function nexusDownloadArtifacts(items, dlOptions, verifyPlatformAndFormat) {
  return __awaiter(this, void 0, void 0, function () {
    var owner,
      repo,
      branch,
      outputDir,
      platforms,
      formats,
      dateRegex,
      date,
      artifacts,
      jsonFiles,
      jsonContent,
      _i,
      artifacts_3,
      artifact,
      downloadUrl,
      name_3
    return __generator(this, function (_a) {
      switch (_a.label) {
        case 0:
          ;(owner = dlOptions.owner),
            (repo = dlOptions.repo),
            (branch = dlOptions.branch),
            (outputDir = dlOptions.outputDir),
            (platforms = dlOptions.platforms),
            (formats = dlOptions.formats)
          dateRegex = new RegExp(
            ''.concat(owner, '/').concat(repo, '/').concat(branch, '/([^/]*)')
          )
          items.sort(function (a, b) {
            var _a, _b, _c
            // sort entries via 'path' key
            // e.g. path: 'SiliconLabs/zap/master/2022-07-22T14:55:43Z/zap-win-zip.json'
            var res =
              (_a = a === null || a === void 0 ? void 0 : a.path) === null ||
              _a === void 0
                ? void 0
                : _a.match(dateRegex)
            var dateA = new Date(
              (_b = a === null || a === void 0 ? void 0 : a.path) === null ||
              _b === void 0
                ? void 0
                : _b.match(dateRegex)[1]
            )
            var dateB = new Date(
              (_c = b === null || b === void 0 ? void 0 : b.path) === null ||
              _c === void 0
                ? void 0
                : _c.match(dateRegex)[1]
            )
            if ((0, date_fns_1.isEqual)(dateA, dateB)) {
              return b.path.localeCompare(a.path)
            } else {
              return (0, date_fns_1.compareDesc)(dateA, dateB)
            }
          })
          if (!(items.length > 1)) return [3 /*break*/, 6]
          date = items[0].path.match(dateRegex)[1]
          console.log(
            'Repo: '
              .concat(NEXUS_SERVER, '/#browse/browse:')
              .concat(NEXUS_REPO_NAME, ':')
              .concat(dlOptions.owner, '/')
              .concat(dlOptions.repo, '/')
              .concat(dlOptions.branch, '/')
              .concat(date)
          )
          return [
            4 /*yield*/,
            nexusGetArtifacts(
              nexusRestApiUrl(
                NEXUS_REPO_NAME,
                ''
                  .concat(dlOptions.owner, '/')
                  .concat(dlOptions.repo, '/')
                  .concat(dlOptions.branch, '/')
                  .concat(date, '/*')
              ),
              dlOptions
            ),
            // find commit
          ]
        case 1:
          artifacts = _a.sent()
          jsonFiles = artifacts.filter(function (x) {
            return x.contentType.toLowerCase().includes('json')
          })
          return [4 /*yield*/, urlContent(jsonFiles.shift().downloadUrl)]
        case 2:
          jsonContent = _a.sent()
          console.log(
            'Commit: '.concat(jsonContent.workflow_run.head_sha.substring(0, 7))
          )
          console.log('Output directory: '.concat(outputDir))
          ;(_i = 0), (artifacts_3 = artifacts)
          _a.label = 3
        case 3:
          if (!(_i < artifacts_3.length)) return [3 /*break*/, 6]
          artifact = artifacts_3[_i]
          downloadUrl = artifact.downloadUrl
          name_3 = downloadUrl.substring(downloadUrl.lastIndexOf('/') + 1)
          // download .json from Nexus as well.
          // This is needed by internal CI to track zap version across builds.
          formats.push('.json')
          if (!verifyPlatformAndFormat.call(null, name_3, platforms, formats)) {
            return [3 /*break*/, 5]
          }
          return [
            4 /*yield*/,
            download(artifact.downloadUrl, outputDir, undefined, name_3),
          ]
        case 4:
          _a.sent()
          _a.label = 5
        case 5:
          _i++
          return [3 /*break*/, 3]
        case 6:
          return [2 /*return*/]
      }
    })
  })
}
/**
 * Get list of artifacts from Github:
 * @param {*} owner
 * @param {*} repo
 * @param {*} branch
 * @param {*} commit ["latest" | commit_hash_id]
 * @returns list of artifact entries following Github Artifacts schema
 *          https://docs.github.com/en/rest/actions/artifacts
 */
function githubGetArtifacts(options) {
  var _a, _b, _c, _d, _e, _f
  return __awaiter(this, void 0, void 0, function () {
    var owner,
      repo,
      branch,
      commit,
      githubToken,
      octokit,
      refCommit,
      refWorkflowRunId,
      res,
      artifacts
    return __generator(this, function (_g) {
      switch (_g.label) {
        case 0:
          ;(owner = options.owner),
            (repo = options.repo),
            (branch = options.branch),
            (commit = options.commit),
            (githubToken = options.githubToken)
          octokit = new octokit_1.Octokit({
            githubToken: githubToken,
          })
          refCommit = ''
          refWorkflowRunId = 0
          return [
            4 /*yield*/,
            octokit.request('GET /repos/{owner}/{repo}/actions/artifacts', {
              owner: owner,
              repo: repo,
            }),
          ]
        case 1:
          res = _g.sent()
          if (res.status != http_status_codes_1.StatusCodes.OK) {
            console.error(
              'Failed to query Github Artifacts API to download artifacts'
            )
            return [2 /*return*/, []]
          }
          artifacts = res.data.artifacts
          // filter all artifact with current branch
          artifacts =
            artifacts === null || artifacts === void 0
              ? void 0
              : artifacts.filter(function (e) {
                  return e.workflow_run.head_branch === branch
                })
          if (artifacts && artifacts.length) {
            if (commit === DEFAULT_COMMIT_LATEST) {
              refCommit =
                (_b =
                  (_a = artifacts[0]) === null || _a === void 0
                    ? void 0
                    : _a.workflow_run) === null || _b === void 0
                  ? void 0
                  : _b.head_sha
              options.commit = refCommit
              refWorkflowRunId =
                (_d =
                  (_c = artifacts[0]) === null || _c === void 0
                    ? void 0
                    : _c.workflow_run) === null || _d === void 0
                  ? void 0
                  : _d.id
              return [
                2 /*return*/,
                artifacts.filter(function (artifact) {
                  return (
                    artifact.workflow_run.head_sha === refCommit &&
                    artifact.workflow_run.id === refWorkflowRunId
                  )
                }),
              ]
            } else {
              refCommit = commit
              artifacts =
                artifacts === null || artifacts === void 0
                  ? void 0
                  : artifacts.filter(function (artifact) {
                      return artifact.workflow_run.head_sha.startsWith(
                        refCommit
                      )
                    })
              // multiple builds can correspond to the same commit id
              // always pick artifacts with the latest run (newest run "id")
              if (
                artifacts === null || artifacts === void 0
                  ? void 0
                  : artifacts.length
              ) {
                refWorkflowRunId =
                  (_f =
                    (_e = artifacts[0]) === null || _e === void 0
                      ? void 0
                      : _e.workflow_run) === null || _f === void 0
                    ? void 0
                    : _f.id
                artifacts = artifacts.filter(function (artifact) {
                  return artifact.workflow_run.id == refWorkflowRunId
                })
              }
              return [2 /*return*/, __spreadArray([], artifacts, true)]
            }
          } else {
            console.error('Unable to find any artifacts for download.')
            return [2 /*return*/, []]
          }
          return [2 /*return*/]
      }
    })
  })
}
function configureBuildCommand() {
  return yargs_1['default']
    .option('mac', {
      alias: 'm',
      description: 'Download macOS artifacts',
    })
    .option('linux', {
      alias: 'l',
      description: 'Download Linux artifacts',
    })
    .option('win', {
      alias: 'w',
      description: 'Download Windows artifacts',
    })
    .option('outputDir', {
      description: 'Output dir',
      default: process.cwd(),
      type: 'string',
    })
    .option('commit', {
      alias: 'c',
      description: 'Commit hash for Github artifact',
      default: DEFAULT_COMMIT_LATEST,
    })
    .option('branch', {
      alias: 'b',
      description: 'Commit hash for Github artifact',
      default: DEFAULT_BRANCH,
    })
    .option('repo', {
      alias: 'r',
      description: 'Name of Github repo: https://github.com/${owner}/${repo}',
      default: DEFAULT_REPO,
    })
    .option('owner', {
      alias: 'o',
      description: 'Owner of Github repo: https://github.com/${owner}/${repo}',
      default: DEFAULT_OWNER,
    })
    .option('githubToken', {
      description: 'Define GITHUB_TOKEN for downloading artifacts from Github',
      default: null,
      type: 'string',
    })
    .option('formats', {
      alias: 'f',
      description: 'Define a list of desired format for artifacts',
      default: ['zip'],
      type: 'array',
    })
    .option('mirror', {
      description:
        'Download Github artifacts into ./artifacts folder to simplify Nexus upload process.',
      type: 'boolean',
      default: false,
    })
    .option('nameOnly', {
      description:
        'Output list of latest artifacts to <branch_name>.txt. Used for verifying the presence on Nexus',
      type: 'boolean',
      default: false,
    })
    .option('src', {
      description: 'URL source for obtaining ZAP binaries',
      type: 'string',
      default: 'nexus',
      choices: ['github', 'nexus'],
    })
    .help('h')
    .alias('h', 'help')
}
function main() {
  return __awaiter(this, void 0, void 0, function () {
    var y, dlOptions, githubBranches, _a, nexusUrl, nexusItems, artifacts
    return __generator(this, function (_b) {
      switch (_b.label) {
        case 0:
          y = configureBuildCommand()
          dlOptions = {
            githubToken: y.argv.githubToken
              ? y.argv.githubToken
              : process.env.GITHUB_TOKEN,
            owner: y.argv.owner,
            repo: y.argv.repo,
            branch: y.argv.branch,
            commit: y.argv.commit,
            outputDir: y.argv.outputDir,
            platforms: platforms(y.argv),
            formats: y.argv.formats,
            src: y.argv.src,
            mirror: y.argv.mirror,
            nameOnly: y.argv.nameOnly,
          }
          return [
            4 /*yield*/,
            getExistingGithubBranches(dlOptions),
            // evaluate artifact source
          ]
        case 1:
          githubBranches = _b.sent()
          _a = dlOptions.src === 'nexus'
          if (!_a) return [3 /*break*/, 3]
          return [4 /*yield*/, (0, is_reachable_1['default'])(NEXUS_SERVER)]
        case 2:
          _a = _b.sent()
          _b.label = 3
        case 3:
          // evaluate artifact source
          if (_a) {
            if (
              githubBranches.includes(dlOptions.branch) &&
              !nexusCachedBranches.includes(dlOptions.branch)
            ) {
              console.log(
                'Branch '.concat(
                  dlOptions.branch,
                  ' is not cached on Nexus. Defaulting to master branch on Github instead.'
                )
              )
              dlOptions.src = 'github'
            } else if (!nexusCachedBranches.includes(dlOptions.branch)) {
              console.log(
                'Branch '.concat(
                  dlOptions.branch,
                  ' is not cached on Nexus. Defaulting to master branch instead.'
                )
              )
              dlOptions.branch = 'master'
            }
          }
          if (!(dlOptions.src === 'nexus')) return [3 /*break*/, 6]
          nexusUrl = nexusRestApiUrl(
            NEXUS_REPO_NAME,
            ''
              .concat(dlOptions.owner, '/')
              .concat(dlOptions.repo, '/')
              .concat(dlOptions.branch, '/*')
          )
          return [4 /*yield*/, nexusGetArtifacts(nexusUrl, dlOptions)]
        case 4:
          nexusItems = _b.sent()
          return [
            4 /*yield*/,
            nexusDownloadArtifacts(
              nexusItems,
              dlOptions,
              verifyPlatformAndFormat
            ),
          ]
        case 5:
          _b.sent()
          return [3 /*break*/, 11]
        case 6:
          if (!dlOptions.githubToken) {
            return [
              2 /*return*/,
              console.error(
                'Missing GITHUB_TOKEN env variable for Github.com access!\nFind more information at https://docs.github.com/en/actions/security-guides/automatic-token-authentication#about-the-github_token-secret'
              ),
            ]
          }
          return [4 /*yield*/, githubGetArtifacts(dlOptions)]
        case 7:
          artifacts = _b.sent()
          if (!dlOptions.nameOnly) return [3 /*break*/, 9]
          return [
            4 /*yield*/,
            githubListArtifacts(artifacts, dlOptions, verifyPlatformAndFormat),
          ]
        case 8:
          _b.sent()
          return [3 /*break*/, 11]
        case 9:
          return [
            4 /*yield*/,
            githubDownloadArtifacts(
              artifacts,
              dlOptions,
              verifyPlatformAndFormat
            ),
          ]
        case 10:
          _b.sent()
          _b.label = 11
        case 11:
          return [2 /*return*/]
      }
    })
  })
}
main()
