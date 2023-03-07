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
// No console.log() / setTimeout
// console.log = jest.fn(() => { throw new Error('Do not use console.log() in production') })
jest.setTimeout(2000)

// jest speedup when errors are part of the game
// Error.stackTraceLimit = 0

global.Promise = require('promise')

/*
import chai from 'chai'
// Make sure chai and jasmine ".not" play nice together
// https://medium.com/@RubenOostinga/combining-chai-and-jest-matchers-d12d1ffd0303
// updated here: https://www.andrewsouthpaw.com/jest-chai/
const originalNot = Object.getOwnPropertyDescriptor(chai.Assertion.prototype, 'not').get
Object.defineProperty(chai.Assertion.prototype, 'not', {
  get() {
    Object.assign(this, this.assignedNot)
    return originalNot.apply(this)
  },
  set(newNot) {
    this.assignedNot = newNot
    return newNot
  }
})

// Combine both jest and chai matchers on expect
const originalExpect = global.expect

global.expect = (actual) => {
  const originalMatchers = originalExpect(actual)
  const chaiMatchers = chai.expect(actual)

  // Add middleware to Chai matchers to increment Jest assertions made
  const { assertionsMade } = originalExpect.getState()
  Object.defineProperty(chaiMatchers, 'to', {
    get() {
      originalExpect.setState({ assertionsMade: assertionsMade + 1 })
      return chai.expect(actual)
    },
  })

  const combinedMatchers = Object.assign(chaiMatchers, originalMatchers)
  return combinedMatchers
}
Object.keys(originalExpect).forEach(key => (global.expect[key] = originalExpect[key]))
*/

// do this to make sure we don't get multiple hits from both webpacks when running SSR
setTimeout(() => {
  // do nothing
}, 1)
