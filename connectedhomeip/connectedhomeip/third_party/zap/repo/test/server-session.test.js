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
 *
 *
 * @jest-environment node
 */

const axios = require('axios')
const dbApi = require('../src-electron/db/db-api.js')
const httpServer = require('../src-electron/server/http-server.js')
const env = require('../src-electron/util/env.ts')
const zclLoader = require('../src-electron/zcl/zcl-loader.js')
const testUtil = require('./test-util.js')
const restApi = require('../src-shared/rest-api.js')
const queryZcl = require('../src-electron/db/query-zcl.js')
const util = require('../src-electron/util/util.js')

let db
let axiosInstance = null
let sessionUuid = util.createUuid()

beforeAll(async () => {
  const { port, baseUrl } = testUtil.testServer(__filename)
  env.setDevelopmentEnv()
  let file = env.sqliteTestFile('server-session')
  axiosInstance = axios.create({ baseURL: baseUrl })
  db = await dbApi.initDatabaseAndLoadSchema(
    file,
    env.schemaFile(),
    env.zapVersion()
  )
  await zclLoader.loadZcl(db, env.builtinSilabsZclMetafile())
  await httpServer.initHttpServer(db, port)
}, testUtil.timeout.medium())

afterAll(
  () => httpServer.shutdownHttpServer().then(() => dbApi.closeDatabase(db)),
  testUtil.timeout.medium()
)

test(
  'get index.html',
  () =>
    axiosInstance.get('/index.html').then((response) => {
      let sessionCookie = response.headers['set-cookie'][0]
      axiosInstance.defaults.headers.Cookie = sessionCookie
      expect(
        response.data.includes(
          'Configuration tool for the Zigbee Cluster Library'
        )
      ).toBeTruthy()
    }),
  testUtil.timeout.medium()
)
