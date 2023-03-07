#!/usr/bin/env node
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

const yargs = require('yargs')
const scriptUtil = require('./script-util.js')

let startTime = process.hrtime.bigint()

let arg = yargs
  .option('zcl', {
    desc: 'Specifies zcl metafile file to be used.',
    alias: 'z',
    type: 'string',
    demandOption: true,
  })
  .option('generationTemplate', {
    desc: 'Specifies gen-template.json file to be used.',
    alias: 'g',
    type: 'string',
    demandOption: true,
  })
  .option('out', {
    desc: 'Output filename where the converted file goes.',
    alias: 'o',
    type: 'string',
    demandOption: true,
  })
  .demandOption(
    ['zcl', 'out', 'generationTemplate'],
    'Please provide required options!'
  )
  .help()
  .wrap(null).argv

let ctx = {}

let cli = [
  scriptUtil.mainPath(false),
  'convert',
  '--noUi',
  '--noServer',
  '--noZapFileLog',
  '--zcl',
  arg.zcl,
  '--out',
  arg.out,
  '--generationTemplate',
  arg.generationTemplate,
  '--results',
  arg.results,
]
arg._.forEach((x) => cli.push(x))

scriptUtil
  .stampVersion()
  .then(() => scriptUtil.rebuildBackendIfNeeded())
  .then(() => scriptUtil.executeCmd(ctx, 'node', cli))
  .then(() => scriptUtil.doneStamp(startTime))
  .then(() => {
    process.exit(0)
  })
  .catch((code) => {
    process.exit(code)
  })
