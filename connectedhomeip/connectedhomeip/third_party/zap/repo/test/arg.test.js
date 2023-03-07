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

const yargs = require('yargs')
const args = require('../src-electron/util/args')
const { timeout } = require('./test-util.js')
const env = require('../src-electron/util/env.ts')

function x(arg = 'blah') {
  return arg
}

function y(
  arg = {
    a: 1,
    b: 2,
  }
) {
  return arg.a + arg.b
}

beforeAll(async () => {
  env.setDevelopmentEnv()
})


test(
  'Test basic command line processing',
  () => {
    let a = args.processCommandLineArguments([
      'node',
      'test.js',
      '--noUI',
      '--httpPort',
      '123',
      '--arglessArg',
      '--xmlRoot',
      'XmlRoot',
    ])

    expect(a.noUI).toBeTruthy()
    expect(a.httpPort).toBeTruthy()
    expect(a.httpPort).toEqual(123)
    expect(a.arglessArg).toBeTruthy()
    expect(a.xmlRoot).toBe('XmlRoot')
  },
  timeout.short()
)

test(
  'Verify how yargs works',
  () => {
    let argv = yargs(['a', '--x', 1, 'b', '--y', 2, '--tst', 42]).parse()
    expect(argv._).toContain('a')
    expect(argv._).toContain('b')
    expect(argv.x).toBe(1)
    expect(argv.y).toBe(2)
    expect(argv.tst).toBe(42)
  },
  timeout.short()
)

test(
  'Verify how unpassed arguments work',
  () => {
    expect(x()).toBe('blah')
    expect(x(null)).toBe(null)
    expect(x(undefined)).toBe('blah')
    expect(x('funny')).toBe('funny')

    expect(y()).toBe(3)
    expect(y({ a: 22, b: 34 })).toBe(56)
    expect(y({ a: 22 })).toBe(NaN) // A missing options key is just missing
  },
  timeout.short()
)
