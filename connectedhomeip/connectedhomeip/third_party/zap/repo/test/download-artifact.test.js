/**
 *
 *    Copyright (c) 2021 Silicon Labs
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

const { exec, execSync, execFile } = require('child_process')
const path = require('node:path')
const download_artifact_script = path.join(
  __dirname,
  '../src-script/download-artifact.js'
)

beforeAll(() => {
  execSync(`npm run compile:download-artifact`)
  execSync(`chmod a+x ${download_artifact_script}`)
})

test('Downloading from Nexus (default)', async () => {
  let output = execSync(download_artifact_script)
  console.log(output.toString())
  expect(output.toString()).toMatch(/.*nexus.silabs.*/)
  expect(output.toString()).toMatch(/.*zap-...-zip.zip[.]*done.*/)
})

test('Downloading from Github', async () => {
  let output = execSync(`${download_artifact_script} --src github`)
  console.log(output.toString())
  expect(output.toString()).toMatch(/.*github.com.*/)
  expect(output.toString()).toMatch(/.*zap-...-zip.zip[.]*done.*/)
})

test('Default back to master branch if unknown branch is specified', async () => {
  let output = execSync(
    `${download_artifact_script} -b random_unknown_branch_name`
  )
  console.log(output.toString())
  expect(output.toString()).toMatch(/.Defaulting to master branch instead.*/)
})

test('Download from Github if specified branch is available on Github but not Nexus', async () => {
  let output = execSync(`${download_artifact_script} -b unit_test_branch`)
  console.log(output.toString())
  expect(output.toString()).toMatch(
    /.*Defaulting to master branch on Github instead.*/
  )
})
