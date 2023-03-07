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

/**
 * Setup MutationObserver for specific attribute on the ZAP front end
 * and invoke callback functions on the Window Object
 *
 * user case: front-end container (jxBrowser/Electron/etc) intending to 'watch' data inside front-end.
 *
 * @param {*} attributeName
 * @param {*} callback -
 *                      if type "string", it's used as a function name and will be invoked upon the Window Object
 *                      if type "function", it's invoked as a callback.
 */

export function observeAttribute(attributeName, callbackObj) {
  // HTML attribute names are not guaranteed to be case sensitive
  attributeName = attributeName.toLowerCase()
  let html = document.documentElement

  new MutationObserver((mutations) => {
    mutations.forEach((mutation) => {
      if (
        mutation.type === 'attributes' &&
        mutation.attributeName === attributeName
      ) {
        let attributeValue = mutation.target.getAttribute(attributeName)
        callbackObj(JSON.parse(attributeValue))
      }
    })
  }).observe(html, {
    attributeFilter: [attributeName],
  })
}

/**
 * Sets a current value of the observable attribute, triggering
 * all observers to fire.
 *
 * @param {*} attributeName
 * @param {*} value
 */
export function setObservableAttribute(attributeName, value) {
  document.documentElement.setAttribute(attributeName, JSON.stringify(value))
}

/**
 * Returns a value of observable attribute.
 *
 * @param {*} attributeName
 * @returns value of observable attribute.
 */
export function getObservableAttribute(attributeName) {
  return JSON.parse(document.documentElement.getAttribute(attributeName))
}
