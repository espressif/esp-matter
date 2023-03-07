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

exports.renderer_api = {
  featureLevel: 1,
  prefix: 'zap',
  description: 'Zap Renderer API',
  notifications: {
    dirtyFlag: {
      arg: 'dirtyState',
    },
    fileBrowse: {
      arg: 'browseObject',
    },
  },
  functions: {
    debugNavBarOff: {
      description: 'Hide debug navigation bar...',
    },
    debugNavBarOn: {
      description: 'Show debug navigation bar...',
    },
    getFileLocation: {
      description: 'Get file location.',
    },
    getItem: {
      description: 'Get item...',
    },
    getStorageItem: {
      description: 'Get storage item',
    },
    open: {
      description: 'Open file...',
    },
    progressEnd: {
      description: 'End progress indicator.',
    },
    progressStart: {
      description: 'Start progress indicator.',
    },
    removeItem: {
      description: 'Remove item...',
    },
    removeStorageItem: {
      description: 'Remove storage item',
    },
    reportFiles: {
      description: 'Reports files selected by the renderer.',
    },
    save: {
      description: 'Save file...',
    },
    saveFileLocation: {
      description: 'Save file location',
    },
    setItem: {
      description: 'Set item...',
    },
    setStorageItem: {
      description: 'Set storage item...',
    },
    setDarkTheme: {
      description: 'Set dark theme...',
      arg: 'darkTheme',
    },
  },
}

exports.renderer_api_info = [
  { id: 'debugNavBarOff', description: 'Hide debug navigation bar...' },
  { id: 'debugNavBarOn', description: 'Show debug navigation bar...' },
  { id: 'getFileLocation' },
  { id: 'getItem', description: 'Get item...' },
  { id: 'getStorageItem' },
  { id: 'open', description: 'Open file...' },
  { id: 'progressEnd', description: 'End progress indicator.' },
  { id: 'progressStart', description: 'Start progress indicator.' },
  { id: 'removeItem', description: 'Remove item...' },
  { id: 'removeStorageItem' },
  { id: 'reportFiles', description: 'Reports files selected by the renderer.' },
  { id: 'save', description: 'Save file...' },
  { id: 'saveFileLocation' },
  { id: 'setItem', description: 'Set item...' },
  { id: 'setStorageItem' },
  { id: 'setDarkTheme', description: 'Set dark theme...', arg: 'darkTheme' },
  { id: 'isDirty', description: 'Query for session dirty flag' },
]

exports.id = {
  debugNavBarOff: 'debugNavBarOff',
  debugNavBarOn: 'debugNavBarOn',
  getFileLocation: 'getFileLocation',
  getItem: 'getItem',
  getStorageItem: 'getStorageItem',
  open: 'open',
  progressEnd: 'progressEnd',
  progressStart: 'progressStart',
  removeItem: 'removeItem',
  removeStorageItem: 'removeStorageItem',
  reportFiles: 'reportFiles',
  save: 'save',
  saveFileLocation: 'saveFileLocation',
  setItem: 'setItem',
  setStorageItem: 'setStorageItem',
  setDarkTheme: 'setDarkTheme',
  isDirty: 'isDirty',
}

exports.notifyKey = { dirtyFlag: 'dirtyFlag', fileBrowse: 'fileBrowse' }

exports.jsonPrefix = 'rendererApiJson:'

exports.observable = {
  debugNavBar: 'debugNavBar',
  progress_attribute: 'progress-message',
  reported_files: 'reported-files',
}

exports.storageKey = {
  fileSave: 'lastFileLocation_openFileSave',
  theme: 'ui_theme',
  isDarkThemeActive: 'isDarkThemeActive',
}

/** Global symbol that carries the API info metadata */
exports.GLOBAL_SYMBOL_INFO = 'global_renderer_api_info'

/** Global function that can execute the APIs. */
exports.GLOBAL_SYMBOL_EXECUTE = 'global_renderer_api_execute'

/** Global function that can be overloaded by jxbrowser for notifications */
exports.GLOBAL_SYMBOL_NOTIFY = 'global_renderer_api_notify'
