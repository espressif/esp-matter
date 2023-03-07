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

const http = require('http-status-codes')
import { Notify } from 'quasar'

// Implements the pairing function here as a perfect hash.
// https://en.wikipedia.org/wiki/Pairing_function#Cantor_pairing_function
// We don't implement the inverse at this time.
// This function takes in 2 non-negative natural numbers, and returns a natural number
export function cantorPair(x, y) {
  return ((x + y) * (x + y + 1)) / 2 + y
}

export function asHex(input, padding) {
  if (input == null) return ''
  if (input < 0) return input.toString()
  let output = input

  if (typeof input === 'string' || input instanceof String) {
    output = parseInt(input)
  }

  return '0x' + output.toString(16).padStart(padding, '0').toUpperCase()
}

/**
 * Update UI to reflect required components are NOT enabled!
 *
 * @param {*} actionSuccessful - true/false
 * @param {*} componentIds - list of strings
 */
export function notifyComponentUpdateStatus(componentIdStates, added) {
  let components = []
  let updated = false
  console.log(JSON.stringify(componentIdStates))
  if (componentIdStates.length) {
    let success = componentIdStates.filter(
      (x) => x.status == http.StatusCodes.OK
    )
    let failure = componentIdStates.filter(
      (x) => x.status != http.StatusCodes.OK
    )

    if (failure.length) {
      components = failure.map((x) => x.id)
      // updated stays false
    } else {
      components = success.map((x) => x.id)
      updated = true
    }

    if (Array.isArray(components) && components.length) {
      let color = updated ? 'positive' : 'negative'
      let verb = updated ? 'were' : "couldn't be"
      let action = added ? 'added' : 'removed'

      let msg = `<div><strong>The following components ${verb} ${action}.</strong></div>`
      msg += `<div><span style="text-transform: capitalize"><ul>`
      msg += components
        .map((id) => `<li>${id.replace(/_/g, ' ')}</li>`)
        .join(' ')
      msg += `</ul></span></div>`

      // notify ui
      Notify.create({
        message: msg,
        color,
        position: 'top',
        html: true,
      })
    }
  }
}

export function getSelectedUcComponents(ucComponentList) {
  return ucComponentList.filter((x) => x.isSelected)
}

export function getUcComponents(ucComponentTreeResponse) {
  // computed selected Nodes
  let selected = []
  if (ucComponentTreeResponse) {
    ucComponentTreeResponse.filter(function f(e) {
      if (e.children) {
        e.children.filter(f, this)
      }

      if (e.id && e.id.includes('zigbee_')) {
        this.push(e)
      }
    }, selected)
  }
  return selected
}

/**
 * Extract cluster id string "$cluster" from the internal Uc Component Id
 *
 * e.g. "zigbee_basic" from "studiocomproot-Zigbee-Cluster_Library-Common-zigbee_basic"
 * @param {*} ucComponentIds - an array of ids
 */
export function getClusterIdsByUcComponents(ucComponents) {
  return ucComponents
    .map((x) => x.id)
    .map((x) => x.substr(x.lastIndexOf('-') + 1))
}

/**
 * This method should return null if rest server is on the same port
 * as the main HTTP server.
 *
 * But in development envioronment, the ports are separated, so this method
 * should return the port that the rest server API is on, while
 * quasar development server can serve the raw UI content.
 */
export function getServerRestPort() {
  const searchParams = new URLSearchParams(window.location.search)
  if (searchParams.has('restPort')) {
    return searchParams.get('restPort')
  } else {
    return null
  }
}
