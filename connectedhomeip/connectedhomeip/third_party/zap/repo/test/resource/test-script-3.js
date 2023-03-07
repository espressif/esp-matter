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
  let endpoints = await api.endpoints(context)
  let ep = endpoints[0]
  // Here we disable cluster with code 2
  await api.disableClientCluster(context, ep, 2)

  // Here we turn off attribute 4 on cluster 0, the manufacturer name
  await api.disableServerAttribute(context, ep, 0, 4)

  await api.enableOutgoingCommand(context, ep, 0x00, 0x00, api.client)
  await api.disableOutgoingCommand(context, ep, 0x00, 0x00, api.client)
}

exports.postLoad = postLoad
