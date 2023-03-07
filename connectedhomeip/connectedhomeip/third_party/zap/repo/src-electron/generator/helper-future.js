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
 */
const futureKey = 'futures'
const valueKey = 'futureValues'
const timeoutMessage = '!---timeout---!'
/**
 * Block helper resolving the block if the
 * value of the specified future matches.
 *
 * @param {*} options
 */
function ifFuture(options) {
  let name = options.hash.name
  let value = options.hash.value
  if (!(futureKey in this.global)) {
    return
  }
  let futurePromise = this.global[futureKey][name]
  if (futurePromise == null) return

  return futurePromise.then((result) => {
    if (result == timeoutMessage) {
      return ''
    } else if (value == result) {
      return options.fn(this)
    } else {
      return ''
    }
  })
}

/**
 * This method sets the value of the future.
 * Use it as:
 *   {{set_future name="NAME" value="VALUE"}}
 * @param {*} options
 */
function setFuture(options) {
  let name = options.hash.name
  let value = options.hash.value
  this.global[valueKey][name] = value
}

/**
 * This method defines the future with a given name.
 * Use it as: {{future name="NAME"}}
 *
 * @param {*} options
 */
function future(options) {
  let name = options.hash.name
  let toS = options.hash.timeout

  let timeout
  if (toS == null) {
    timeout = 5000 // 5 second default timeout
  } else {
    timeout = parseInt(toS)
  }

  if (!(futureKey in this.global)) {
    this.global[futureKey] = {}
  }
  if (!(valueKey in this.global)) {
    this.global[valueKey] = {}
  }
  let valueObject = this.global[valueKey]

  let futurePromise = new Promise((resolve, reject) => {
    let x = 0
    let interval = setInterval(() => {
      if (valueObject[name] != undefined) {
        resolve(valueObject[name])
        clearInterval(interval)
      }
      x++
      if (10 * x > timeout) {
        resolve(timeoutMessage)
        clearInterval(interval)
      }
    }, 10)
  })
  this.global[futureKey][name] = futurePromise
}

exports.if_future = ifFuture
exports.set_future = setFuture
exports.future = future
