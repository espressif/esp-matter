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

// The purpose of this file is to populate the api for the entry points from jxbrowser.

import {
  renderer_api_info,
  renderer_api_execute,
  renderer_api_notify,
} from '../api/renderer_api.js'
import rendApi from '../../src-shared/rend-api.js'

/**
 * This symbol contains the actual Renderer API meta object.
 * This is the symbol that is discovered by the renderer container code.
 */
window[rendApi.GLOBAL_SYMBOL_INFO] = renderer_api_info()

/**
 * This symbol contains the function that executes a renderer api.
 * The first argument is the renderer API function ID, remaining
 * arguments are arguments for the function itself.
 */
window[rendApi.GLOBAL_SYMBOL_EXECUTE] = renderer_api_execute

/**
 * This symbol contains the notification function. It allows the
 * renderer container to register a function or action to happen
 * when the notification is about to happen.
 */
window[rendApi.GLOBAL_SYMBOL_NOTIFY] = renderer_api_notify
