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

const { Menu, Tray, nativeImage, app } = require('electron')
const path = require('path')
const fs = require('fs')
const env = require('../util/env')
const uiUtil = require('./ui-util')

let tray

function initTray(port) {
  let trayIconPath = path.join(env.iconsDirectory(), 'zap_32x32.png')
  let dockIconPath = path.join(env.iconsDirectory(), 'zap_128x128.png')

  if (!fs.existsSync(trayIconPath)) {
    env.logError(`Tray not created, icon does not exist: ${trayIconPath}`)
    return
  }

  let trayIcon = nativeImage.createFromPath(trayIconPath)
  if (trayIcon.isEmpty()) {
    env.logError('Image got created, but it is empty')
  }
  tray = new Tray(trayIcon.resize({ width: 16, height: 16 }))

  const contextMenu = Menu.buildFromTemplate([
    {
      label: 'New ZCL configuration',
      type: 'normal',
      httpPort: port,
      click: (item, window, event) => {
        uiUtil.openNewConfiguration(item.httpPort)
      },
    },
    {
      label: 'Quit ZAP',
      role: 'quit',
    },
  ])

  // This covers the case of the mac dock
  if (app.dock) {
    app.dock.setIcon(dockIconPath)
    app.dock.setMenu(contextMenu)
  }

  tray.setToolTip('ZCL Advanced Platform')
  tray.setContextMenu(contextMenu)
}

exports.initTray = initTray
