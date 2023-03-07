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

import { BrowserWindow } from 'electron'

const path = require('path')
const env = require('../util/env')

let window: BrowserWindow | null

function createAboutWindow(parentWindow: BrowserWindow, port: number) {
  let webPreferences: Electron.WebPreferences = {
    nodeIntegration: false,
  }
  if (parentWindow != null && 'webContents' in parentWindow) {
    webPreferences.session = parentWindow.webContents.session
  }
  window = new BrowserWindow({
    width: 880,
    height: 525,
    resizable: true,
    center: true,
    title: 'About',
    icon: path.join(env.iconsDirectory(), 'zap_32x32.png'),
    useContentSize: true,
    webPreferences: webPreferences,
  })
  window.setMenu(null)
  window.loadURL(`http://localhost:${port}/#/about`)
  window.on('close', (e) => {
    window = null
  })
  window.on('page-title-updated', (e) => {
    e.preventDefault()
  }) // EO page-title-updated
}

/**
 * Call this function to create a new or show an existing preference window.
 *
 * @export
 * @param {*} port
 */
function createOrShowAboutWindow(parentWindow: BrowserWindow, port: number) {
  if (window == null) {
    createAboutWindow(parentWindow, port)
  } else {
    window.show()
  }
}

exports.createOrShowAboutWindow = createOrShowAboutWindow
