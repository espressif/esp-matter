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
// this is mapped in jest.config.js to resolve @vue/test-utils
import { createLocalVue, shallowMount } from 'test-utils'

import Vuex from 'vuex'
import VueRouter from 'vue-router'
import Quasar, { Cookies } from 'quasar'

const mockSsrContext = () => {
  return {
    req: {
      headers: {},
    },
    res: {
      setHeader: () => undefined,
    },
  }
}

// https://eddyerburgh.me/mock-vuex-in-vue-unit-tests
export const mountQuasar = (component, options = {}) => {
  const localVue = createLocalVue()
  const app = {}

  localVue.use(Vuex)
  localVue.use(VueRouter)
  localVue.use(Quasar)
  const store = new Vuex.Store({})
  const router = new VueRouter()

  if (options) {
    const ssrContext = options.ssr ? mockSsrContext() : null

    if (options.cookies) {
      const cookieStorage = ssrContext ? Cookies.parseSSR(ssrContext) : Cookies
      const cookies = options.cookies
      Object.keys(cookies).forEach((key) => {
        cookieStorage.set(key, cookies[key])
      })
    }

    if (options.plugins) {
      options.plugins.forEach((plugin) => {
        plugin({ app, store, router, Vue: localVue, ssrContext })
      })
    }
  }

  // mock vue-i18n
  const $t = () => {}
  const $tc = () => {}
  const $n = () => {}
  const $d = () => {}

  return shallowMount(component, {
    localVue: localVue,
    store,
    router,
    mocks: { $t, $tc, $n, $d },
    // Injections for Components with a QPage root Element
    provide: {
      pageContainer: true,
      layout: {
        header: {},
        right: {},
        footer: {},
        left: {},
      },
    },
  })
}
