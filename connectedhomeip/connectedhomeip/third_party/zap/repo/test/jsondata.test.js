/**
 *
 *    Copyright (c) 2022 Silicon Labs
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

const path = require('path')
const util = require('../src-electron/util/util')

const jsonDir = path.join(__dirname, 'resource/json/')

test('Test json data', async () => {
  let jsonFile = path.join(jsonDir, 'data1.json')

  let data = await util.collectJsonData(jsonFile)

  expect(data[0]).toEqual('3')
  expect(data[6]).toEqual('9')
})
