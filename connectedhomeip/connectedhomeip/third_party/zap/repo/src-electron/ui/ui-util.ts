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
import { dialog } from 'electron'
import * as window from './window'
import browserApi from './browser-api.js'
import * as uiTypes from '../../src-shared/types/ui-types'
import { WindowCreateArgs } from 'types/window-types'
import * as util from '../util/util'
import * as args from '../util/args'

/**
 * Simple dialog to show error messages from electron renderer scope.
 *
 * @param {*} title
 * @param {*} err
 */
function showErrorMessage(title: string, err: Error | string) {
  let msg
  if (err instanceof Error) {
    msg = err.toString() + '\n\nStack trace:\n' + err.stack
  } else {
    msg = err
  }
  dialog.showErrorBox(title, msg)
}

/**
 * Process a single file, parsing it in as JSON and then possibly opening
 * a new window if all is good.
 *
 * @param {*} db
 * @param {*} filePath
 * @param {*} httpPort Server port for the URL that will be constructed.
 */
function openFileConfiguration(
  filePath: string,
  httpPort: number,
  standalone: boolean = false
) {
  window.windowCreate(httpPort, {
    filePath,
    standalone,
  })
}

/**
 * Creates a new window with a blank configuration.
 *
 * @param {*} httpPort
 * @param {*} options: uiMode, debugNavBar
 */
async function openNewConfiguration(
  httpPort: number,
  options?: WindowCreateArgs
) {
  window.windowCreate(httpPort, options)
}

/**
 * Toggles the dirty flag.
 *
 * @param {*} browserWindow window to affect
 * @param {*} dirty true if this windows is now dirty, false if otherwise
 */
function toggleDirtyFlag(
  browserWindow: Electron.BrowserWindow,
  dirty: boolean
) {
  let title = browserWindow.getTitle()
  // @ts-ignore TODO: type 'isDirty' somehow.
  browserWindow.isDirty = dirty
  if (title.startsWith('* ') && !dirty) {
    browserWindow.setTitle(title.slice(2))
  } else if (!title.startsWith('*') && dirty) {
    browserWindow.setTitle('* ' + title)
  }
}

/**
 * This function should be invoked as a result of the fileBrowse
 * notification via the renderer API. It pops the open dialog and
 * reports result back through the API.
 *
 * @param {*} browserWindow
 * @param {*} options 'key', 'title', 'mode', 'defaultPath'
 */
function openFileDialogAndReportResult(
  browserWindow: Electron.BrowserWindow,
  options: uiTypes.UiFileBrowseOptionsType
) {
  let p: Electron.OpenDialogOptions = {
    buttonLabel: 'Generate',
  }

  if (options.buttonLabel) {
    p.buttonLabel = options.buttonLabel
  }

  if (options.mode === 'file') {
    p.properties = ['openFile']
  } else if (options.mode == 'directory') {
    p.properties = ['openDirectory']
  }
  p.defaultPath = options.defaultPath
  dialog.showOpenDialog(browserWindow, p).then((result) => {
    if (!result.canceled) {
      let paths : string[] = [];
      // check if application is running on Windows standalone mode, then replace the backward slashs with forward slash
      // otherwise it will throw a JSON syntax error
      if(process.platform == "win32"){
        result.filePaths.forEach(path => {
          paths.push(
            path.replace(/\\/g, '/')
            );
        })
      }else{
        paths = result.filePaths
      }

      let output = {
        context: options.context,
        filePaths: paths,
      }
      browserApi.reportFiles(browserWindow, output)
    }
  })
}

function enableUi(
  port: number,
  zapFiles: string[],
  uiMode: string,
  standalone: boolean
) {
  window.initializeElectronUi(port)
  if (zapFiles.length == 0) {
    return openNewConfiguration(port, {
      uiMode: uiMode,
      standalone: standalone,
      filePath: null,
    })
  } else {
    return util.executePromisesSequentially(zapFiles, (f: string) =>
      openFileConfiguration(f, port)
    )
  }
}

exports.showErrorMessage = showErrorMessage
exports.openFileConfiguration = openFileConfiguration
exports.openNewConfiguration = openNewConfiguration
exports.toggleDirtyFlag = toggleDirtyFlag
exports.openFileDialogAndReportResult = openFileDialogAndReportResult
exports.enableUi = enableUi
