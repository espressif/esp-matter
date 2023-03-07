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

const { dialog, Menu, shell } = require('electron')
const uiJs = require('./ui-util')
const about = require('./about')
const commonUrl = require('../../src-shared/common-url.js')
const browserApi = require('./browser-api.js')
const rendApi = require('../../src-shared/rend-api.js')

const newConfiguration = 'New Configuration'
let menuIsShown = true

const template = (httpPort) => [
  {
    role: 'fileMenu',
    submenu: [
      {
        label: newConfiguration + '...',
        accelerator: 'CmdOrCtrl+N',
        httpPort: httpPort,
        click(menuItem, browserWindow, event) {
          uiJs.openNewConfiguration(menuItem.httpPort)
        },
      },
      {
        label: 'Open File...',
        accelerator: 'CmdOrCtrl+O',
        httpPort: httpPort,
        click(menuItem, browserWindow, event) {
          doOpen(browserWindow, menuItem.httpPort)
        },
      },
      {
        label: 'Save',
        accelerator: 'CmdOrCtrl+S',
        click(menuItem, browserWindow, event) {
          doSave(browserWindow)
        },
      },
      {
        label: 'Save As...',
        click(menuItem, browserWindow, event) {
          doSaveAs(browserWindow)
        },
      },
      {
        type: 'separator',
      },
      {
        role: 'close',
      },
      {
        type: 'separator',
      },
      {
        role: 'quit',
      },
    ],
  },
  {
    role: 'editMenu',
  },
  {
    role: 'viewMenu',
    submenu: [
      {
        label: 'Dark theme',
        click(menuItem, browserWindow, event) {
          browserApi.execRendererApi(
            browserWindow,
            rendApi.id.setDarkTheme,
            true
          )
        },
      },
      {
        label: 'Light theme',
        click(menuItem, browserWindow, event) {
          browserApi.execRendererApi(
            browserWindow,
            rendApi.id.setDarkTheme,
            false
          )
        },
      },
      { type: 'separator' },
      {
        label: 'Navigate back ...',
        click(menuItem, browserWindow, event) {
          browserWindow.webContents.goBack()
        },
      },
      {
        label: 'Navigate forward ...',
        click(menuItem, browserWindow, event) {
          browserWindow.webContents.goForward()
        },
      },
      { role: 'reload' },
      { role: 'forceReload' },
      { role: 'toggleDevTools' },
      { type: 'separator' },
      { role: 'resetZoom' },
      { role: 'zoomIn' },
      { role: 'zoomOut' },
      { type: 'separator' },
      { role: 'togglefullscreen' },
    ],
  },
  {
    role: 'windowMenu',
  },
  {
    label: 'Help',
    submenu: [
      {
        label: 'Documentation',
        click(menuItem, browserWindow, event) {
          shell.openExternal(commonUrl.documentationUrl)
        },
      },
      {
        label: 'User and session information',
        click(menuItem, browserWindow, event) {
          getUserSessionInfoMessage(browserWindow)
            .then((msg) => {
              dialog.showMessageBox(browserWindow, {
                title: 'User and session information',
                message: msg,
                buttons: ['Dismiss'],
              })
            })
            .catch((err) => uiJs.showErrorMessage('Session info', err))
        },
      },
      {
        label: 'Renderer API information',
        click(menuItem, browserWindow, event) {
          browserApi.getRendererApiInformation(browserWindow).then((msg) => {
            dialog.showMessageBox(browserWindow, {
              title: 'Renderer API information',
              message: msg,
              buttons: ['Dismiss'],
            })
          })
        },
      },
      {
        type: 'separator',
      },
      {
        label: 'Start progress',
        click(menuItem, browserWindow) {
          browserApi.execRendererApi(
            browserWindow,
            rendApi.id.progressStart,
            'Test progress indication.'
          )
        },
      },
      {
        label: 'End progress',
        click(menuItem, browserWindow) {
          browserApi.execRendererApi(browserWindow, rendApi.id.progressEnd)
        },
      },
      {
        label: 'Show debug navigation bar',
        click(menuItem, browserWindow) {
          browserApi.execRendererApi(browserWindow, rendApi.id.debugNavBarOn)
        },
      },
      {
        label: 'Hide debug navigation bar',
        click(menuItem, browserWindow) {
          browserApi.execRendererApi(browserWindow, rendApi.id.debugNavBarOff)
        },
      },
      {
        label: 'About',
        httpPort: httpPort,
        click(menuItem, browserWindow, event) {
          about.createOrShowAboutWindow(browserWindow, menuItem.httpPort)
        },
      },
    ],
  },
]

async function getUserSessionInfoMessage(browserWindow) {
  let userKey = await browserApi.getUserKeyFromBrowserWindow(browserWindow)
  let sessionUuid = await browserApi.getSessionUuidFromBrowserWindow(
    browserWindow
  )
  return `
  Browser session UUID: ${sessionUuid}
  Browser user key: ${userKey}
  `
}

/**
 * Perform a file->open operation.
 *
 * @param {*} menuItem
 * @param {*} browserWindow
 * @param {*} event
 */
function doOpen(browserWindow, httpPort) {
  browserApi
    .execRendererApi(
      browserWindow,
      rendApi.id.getStorageItem,
      rendApi.storageKey.fileSave
    )
    .then((filePath) => {
      let opts = {
        title: 'Select ZAP or ISC file to load.',
        properties: ['openFile', 'multiSelections'],
      }
      if (filePath != null) {
        opts.defaultPath = filePath
      }
      return dialog.showOpenDialog(browserWindow, opts)
    })
    .then((result) => {
      if (!result.canceled) {
        fileOpen(result.filePaths, httpPort)
        browserApi.execRendererApi(
          browserWindow,
          rendApi.id.setStorageItem,
          rendApi.storageKey.fileSave,
          result.filePaths[0]
        )
      }
    })
    .catch((err) => uiJs.showErrorMessage('Open file', err))
}

/**
 * Perform a save, defering to save as if file is not yet selected.
 *
 * @param {*} browserWindow
 */
function doSave(browserWindow) {
  if (browserWindow.getTitle().includes(newConfiguration)) {
    doSaveAs(browserWindow)
  } else {
    fileSave(browserWindow, null)
  }
}

/**
 * Perform save as.
 *
 * @param {*} menuItem
 * @param {*} browserWindow
 * @param {*} event
 */
function doSaveAs(browserWindow) {
  browserApi
    .execRendererApi(
      browserWindow,
      rendApi.id.getStorageItem,
      rendApi.storageKey.fileSave
    )
    .then((filePath) => {
      let opts = {
        filters: [
          { name: 'ZAP Config', extensions: ['zap'] },
          { name: 'All Files', extensions: ['*'] },
        ],
      }
      if (filePath != null) {
        opts.defaultPath = filePath
      }
      return dialog.showSaveDialog(opts)
    })
    .then((result) => {
      if (!result.canceled) {
        fileSave(browserWindow, result.filePath)
        return result.filePath
      } else {
        return null
      }
    })
    .then((filePath) => {
      if (filePath != null) {
        browserWindow.setTitle(filePath)
        browserApi.execRendererApi(
          browserWindow,
          rendApi.id.setStorageItem,
          rendApi.storageKey.fileSave,
          filePath
        )
      }
    })
    .catch((err) => uiJs.showErrorMessage('Save file', err))
}

/**
 * perform the save.
 *
 * @param {*} db
 * @param {*} browserWindow
 * @param {*} filePath
 * @returns Promise of saving.
 */
function fileSave(browserWindow, filePath) {
  browserApi.execRendererApi(browserWindow, rendApi.id.save, filePath)
}

/**
 * Perform the do open action, possibly reading in multiple files.
 *
 * @param {*} db
 * @param {*} filePaths
 */
function fileOpen(filePaths, httpPort) {
  filePaths.forEach((filePath) => {
    uiJs.openFileConfiguration(filePath, httpPort)
  })
}

function showMenu(httpPort) {
  const menu = Menu.buildFromTemplate(template(httpPort))
  Menu.setApplicationMenu(menu)
}

function hideMenu(httpPort) {
  Menu.setApplicationMenu(null)
}
/**
 * Toggling of menu
 *
 * @param {*} port
 */
function toggleMenu(httpPort) {
  menuIsShown = !menuIsShown
  if (!menuIsShown) {
    hideMenu(httpPort)
  } else {
    showMenu(httpPort)
  }
}

/**
 * Initial menu show.
 *
 * @param {*} httpPort
 */
function initMenu(httpPort) {
  menuIsShown = true
  showMenu(httpPort)
}

exports.toggleMenu = toggleMenu
exports.initMenu = initMenu
exports.newConfiguration = newConfiguration
