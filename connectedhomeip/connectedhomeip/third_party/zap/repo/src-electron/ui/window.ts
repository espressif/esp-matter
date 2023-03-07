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

const { session, BrowserWindow, dialog } = require('electron')
const path = require('path')
const env = require('../util/env')
const menu = require('./menu.js')
const tray = require('./tray.js')
const browserApi = require('./browser-api.js')
const querystringUtil = require('querystring')
const httpServer = require('../server/http-server.js')
import { WindowCreateArgs } from '../types/window-types'

let windowCounter = 0

/**
 * Electron UI initialization.
 *
 * Note: You might be tempted to pass `db` to this function. Don't.
 * That was done before and it's just a lazy way to cut through the
 * layers between UI and back-end. Should not be done. Any information
 * UI needs from the database should be retrieved via renderer API.
 *
 * @param {*} port
 */
export function initializeElectronUi(port: number) {
  menu.initMenu(port)
  tray.initTray(port)
}

export function windowCreateIfNotThere(port: number) {
  if (BrowserWindow.getAllWindows().length == 0) {
    windowCreate(port)
  }
}

export function createQueryString(
  uiMode?: string | undefined,
  standalone?: boolean | undefined,
  restPort?: number
) {
  var params = new Map()

  if (!arguments.length) {
    return ''
  }

  if (uiMode !== undefined) {
    params.set('uiMode', uiMode)
  }

  if (standalone !== undefined) {
    params.set('standalone', standalone)
  }

  // Electron/Development mode
  if (
    process.env.DEV &&
    process.env.MODE === 'electron' &&
    restPort !== undefined
  ) {
    params.set('restPort', restPort)
  }

  return '?' + querystringUtil.stringify(Object.fromEntries(params))
}

/**
 * Create a window, possibly with a given file path.
 *
 * @export
 * @param {*} port
 * @param {*} [filePath=null]
 * @param {*} [uiMode=null]
 * @returns BrowserWindow that got created
 */
export function windowCreate(port: number, args?: WindowCreateArgs) {
  let webPreferences = {
    nodeIntegration: false,
    worldSafeExecuteJavaScript: true,
  }
  windowCounter++
  let w = new BrowserWindow({
    width: 1600,
    height: 800,
    x: 50 + windowCounter * 20,
    y: 50 + windowCounter * 20,
    resizable: true,
    center: true,
    icon: path.join(env.iconsDirectory(), 'zap_32x32.png'),
    title: args?.filePath == null ? menu.newConfiguration : args?.filePath,
    useContentSize: true,
    webPreferences: webPreferences,
  })

  let queryString = createQueryString(
    args?.uiMode,
    args?.standalone,
    httpServer.httpServerPort()
  )

  // @ts-ignore
  w.isDirty = false
  w.loadURL(`http://localhost:${port}/` + queryString).then(async () => {
    if (args?.filePath != null) {
      browserApi.execFileOpen(w, args?.filePath)
    }
  })

  w.on('page-title-updated', (e) => {
    e.preventDefault()
  }) // EO page-title-updated

  w.on('close', (e) => {
    e.preventDefault()
    // @ts-ignore
    if (w.isDirty) {
      const result = dialog.showMessageBoxSync(w, {
        type: 'warning',
        title: 'Unsaved changes?',
        message:
          'Your changes will be lost if you do not save them into the file.',
        buttons: ['Quit Anyway', 'Cancel'],
        defaultId: 0,
        cancelId: 1,
      })

      if (result === 0) w.destroy()
    } else {
      w.destroy()
    }
  }) // EO close

  w.webContents.on(
    'console-message',
    (event, level, message, line, sourceId) => {
      if (!browserApi.processRendererNotify(w, message)) {
        env.logBrowser(message)
      }
    }
  )
  w.webContents.on('before-input-event', (e, input) => {
    if (input.type === 'keyUp' && input.key.toLowerCase() === 'alt') {
      menu.toggleMenu(port)
    }
  })
  return w
}
