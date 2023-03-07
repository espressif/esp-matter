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
 * @jest-environment jsdom
 */

const axios = require('../src/boot/axios.js')
const ws = require('../src/boot/ws.js')
const actions = require('../src/store/zap/actions.js')
const mutations = require('../src/store/zap/mutations.js')
const rendApi = require('../src-shared/rend-api.js')
import routes from '../src/router/routes.js'
import index from '../src/router/index.js'
import { timeout } from './test-util.js'

const Vue = require('vue')

test(
  'Global renderer API',
  () => {
    expect(window[rendApi.GLOBAL_SYMBOL_EXECUTE]).not.toBeNull()
    expect(window[rendApi.GLOBAL_SYMBOL_INFO]).not.toBeNull()
    expect(window[rendApi.GLOBAL_SYMBOL_NOTIFY]).not.toBeNull()
  },
  timeout.short()
)

test(
  'Session ID existence',
  () => {
    expect(window.sessionStorage.getItem('session_uuid')).not.toBeNull()
  },
  timeout.short()
)

test(
  'WebSockets',
  () => {
    expect(Vue.prototype.$onWebSocket).not.toBeNull()
  },
  timeout.short()
)

test(
  'Routes',
  () => {
    expect(routes.length).toBeGreaterThan(0)
    let router = index()
    expect(router.mode).toBe('hash')
  },
  timeout.short()
)
