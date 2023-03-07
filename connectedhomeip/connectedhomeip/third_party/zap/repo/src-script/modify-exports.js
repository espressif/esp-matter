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

// Simple script to modify ES6 exports into CommonJS.
// Babel can do that, but setting up babel is a total overkill for our simple need.

const fs = require('fs')
const path = require('path')
const readline = require('readline')

const filename = process.argv[2]

const readInterface = readline.createInterface({
  input: fs.createReadStream(filename),
  //output: process.stdout,
  console: false,
})

let exports = []

readInterface.on('line', (line) => {
  if (!line.includes('export function ')) console.log(line)
  else {
    // we have an export, let's convert it
    let bracketIndex = line.indexOf('(')
    if (bracketIndex == -1) {
      console.log(line)
    } else {
      let postBracket = line.substring(bracketIndex)
      let functionName = line.substring(0, bracketIndex).split(' ')[2]
      console.log(`function ${functionName}${postBracket}`)
      exports.push(functionName)
    }
  }
})

readInterface.on('close', () => {
  console.log('// exports')
  exports.forEach((exp) => {
    console.log(`exports.${exp} = ${exp}`)
  })
})
