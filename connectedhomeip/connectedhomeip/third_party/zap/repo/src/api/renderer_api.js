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

import { Dark } from 'quasar'
const _ = require('lodash')

const observable = require('../util/observable.js')
const restApi = require('../../src-shared/rest-api.js')
const rendApi = require('../../src-shared/rend-api.js')
const storage = require('../util/storage.js')

// This file provide glue logic to enable function calls & HTML attribute data change listener logic
// between front-end containers (jxBrowser, Electron, etc) and the node.js
//
// If a callback applies (e.g. when a function returns a value),
// the javascript code will invoke the Java function name "${id}Callback"
// e.g. for function "open", "openCallback" is invoked.

/**
 * Each declared 'function' entry offers such features:
 * - ability to invoke front-end functions within container via function 'id' with callback.
 * - ability to observe specific HTML target (a DOM Node) for data change.
 *
 * Per entry, 'type' is 'observer', it is dedicated as a data cahgne listener. The
 * e.g. The 'open' function is invoked by the container when opening a new configuration.
 * The front-end is informed and proceed to init UI elements.
 */
export function renderer_api_info() {
  return {
    prefix: 'zap',
    description: 'Zap Renderer API',
    functions: rendApi.renderer_api_info,
  }
}

function fnOpen(zapFilePath, ideProjectPath) {
  // Make a request for a user with a given ID
  window
    .serverPost(`${restApi.ide.open}`, {
      zapFilePath: zapFilePath,
      ideProjectPath: ideProjectPath,
    })
    .then((res) => window.location.reload())
    .catch((err) => console.log(err))
}

function fnSave(zap_file) {
  let data = {}
  if (zap_file != null) data.path = zap_file
  window
    .serverPost(`${restApi.ide.save}`, data)
    .catch((err) => console.log(err))
}

function fnIsDirty() {
  return window
    .serverGet(`${restApi.ide.isDirty}`)
    .then((res) => {
      return res?.data?.DIRTY
    })
    .catch((err) => console.log(err))
}

export function renderer_api_execute(id, ...args) {
  let ret = null
  switch (id) {
    case rendApi.id.open:
      ret = fnOpen.apply(null, args)
      break
    case rendApi.id.save:
      ret = fnSave.apply(null, args)
      break
    case rendApi.id.progressStart:
      observable.setObservableAttribute(
        rendApi.observable.progress_attribute,
        args[0]
      )
      break
    case rendApi.id.progressEnd:
      observable.setObservableAttribute(
        rendApi.observable.progress_attribute,
        ''
      )
      break
    case rendApi.id.reportFiles:
      observable.setObservableAttribute(
        rendApi.observable.reported_files,
        JSON.parse(args[0])
      )
      break
    case rendApi.id.debugNavBarOn:
      observable.setObservableAttribute(rendApi.observable.debugNavBar, true)
      break
    case rendApi.id.debugNavBarOff:
      observable.setObservableAttribute(rendApi.observable.debugNavBar, false)
      break
    case rendApi.id.setDarkTheme:
      if (_.isBoolean(args[0])) {
        Dark.set(args[0])
        storage.setItem(rendApi.storageKey.isDarkThemeActive, args[0])
        renderer_api_notify(rendApi.id.setDarkTheme, args[0])
      }

      if (_.isString(args[0])) {
        Dark.set(args[0] === 'true')
        storage.setItem(
          rendApi.storageKey.isDarkThemeActive,
          args[0] === 'true'
        )
        renderer_api_notify(rendApi.id.setDarkTheme, args[0] === 'true')
      }

      break

    case rendApi.id.isDirty:
      ret = fnIsDirty.apply(null)
      break
  }
  return ret
}

/**
 * Default implementation of the notification function simply
 * prints the notification to the console log. In case of electron
 * renderer container, this is all that's needed. In case of others,
 * they can register their own notifier functions.
 *
 * @param {*} key
 * @param {*} value
 */
export function renderer_api_notify(key, value) {
  console.log(
    `${rendApi.jsonPrefix}${JSON.stringify({ key: key, value: value })}`
  )
}
