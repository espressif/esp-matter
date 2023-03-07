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
 * @jest-environment node
 */

import { renderer_api_info } from '../src/api/renderer_api'
import { timeout } from './test-util'

test(
  'Test structure of renderer API',
  () => {
    let api = renderer_api_info()
    expect(api).not.toBeNull()
    expect(api.prefix).toEqual('zap')
    expect(api.functions.length).toBeGreaterThanOrEqual(2)
  },
  timeout.short()
)
