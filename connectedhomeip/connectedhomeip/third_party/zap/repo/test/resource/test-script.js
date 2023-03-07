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
// Example file for the post load functionality.

async function postLoad(api, context) {
  api.print('Test post-load function.')
  for (fn of api.functions()) {
    api.print(`Function: ${fn}`)
  }
  api.print(`Session Id: ${api.sessionId(context)}`)
  api.print('Endpoints:')
  let endpoints = await api.endpoints(context)
  for (ep of endpoints) {
    api.print(`  - endpoint: ${ep.endpointIdentifier}`)
    let clusters = await api.clusters(context, ep)
    for (cl of clusters) {
      api.print(`    - cluster ${cl.code}: ${cl.name} [${cl.side}]`)
      let attributes = await api.attributes(context, ep, cl)
      for (at of attributes) {
        api.print(`      - attribute ${at.code}: ${at.name}`)
      }
      let commands = await api.commands(context, ep, cl)
      for (co of commands) {
        api.print(`      - command ${co.code}: ${co.name}`)
      }
    }
  }
  api.deleteEndpoint(context, endpoints[0])
}

exports.postLoad = postLoad
