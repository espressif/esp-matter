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

//Usage 'node ./license-add.js

let fs = require('fs')
let path = require('path')

const licenseJs = (year, who = 'Silicon Labs') => `/**
 *
 *    Copyright (c) ${year} ${who}
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
`

const licenseTestJs = (year, jestEnv = 'node', who = 'Silicon Labs') => `/**
 *
 *    Copyright (c) ${year} ${who}
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
 * @jest-environment ${jestEnv}
 */
`

const licenseXml = (year, who = 'Silicon Labs') => `<!--
Copyright (c) 2008,${year} ${who}.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
-->
`

// Directories that we will scan
const directories = ['src', 'src-electron', 'test']
const startYear = 2020
const whos = ['Silicon Labs', 'Project CHIP Authors']

async function processSingleFile(path) {
  let isVue = path.endsWith('.vue')
  let isJs = path.endsWith('.js')
  let isTestJs = path.endsWith('.test.js')
  let currentYear = new Date().getFullYear()

  if (!(isVue || isJs)) return

  fs.readFile(path, 'utf8', (err, data) => {
    if (err) throw err

    for (const who of whos) {
      for (let year = startYear; year <= currentYear; year++) {
        if (isTestJs && data.startsWith(licenseTestJs(year, 'node', who))) {
          console.log(`    -  valid: ${path}: `)
          return
        }

        if (isTestJs && data.startsWith(licenseTestJs(year, 'jsdom', who))) {
          console.log(`    -  valid: ${path}: `)
          return
        }

        if (isJs && data.startsWith(licenseJs(year, who))) {
          console.log(`    -  valid: ${path}: `)
          return
        }

        if (isVue && data.startsWith(licenseXml(year, who))) {
          console.log(`    -  valid: ${path}: `)
          return
        }
      }
    }

    // Now check if we need to remove old license
    if (data.startsWith('// Copyright') || data.startsWith('<!-- Copyright')) {
      // We remove the firstline
      let pos = data.indexOf('\n')
      data = data.substr(pos + 1)
      console.log(`    - update: ${path}`)
    } else {
      console.log(`    - add: ${path}: `)
    }

    // Now we write license and then data
    let output = isJs
      ? isTestJs
        ? licenseTestJs(currentYear)
        : licenseJs(currentYear)
      : licenseXml(currentYear)
    output = output.concat(data)
    fs.writeFile(path, output, (err) => {
      if (err) throw err
    })
  })
}

async function readDirent(level, currentDir, dirent) {
  if (dirent == null) return
  let fullName = path.join(currentDir.path, dirent.name)
  if (dirent.isFile()) {
    processSingleFile(fullName)
  } else if (dirent.isDirectory()) {
    fs.promises.opendir(fullName).then((dir) => recursiveScan(level + 1, dir))
  }
  return currentDir
    .read()
    .then((dirent) => readDirent(level, currentDir, dirent))
}

async function recursiveScan(level, currentDir) {
  currentDir.read().then((dirent) => readDirent(level, currentDir, dirent))
  return currentDir
}

directories.forEach((path) => {
  fs.promises.opendir(path).then((dir) => recursiveScan(0, dir))
})
