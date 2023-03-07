/**
 *
 *    Copyright (c) 2021 Project CHIP Authors
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
 * This module provides API to access various iterator utilities that can then
 * be used to build iterator helpers.
 */

const templateUtil = require('../generator/template-util.js')
const queryEndpointType = require('../db/query-endpoint-type.js')
const queryCommand = require('../db/query-command.js')

/**
 * Helper for add_user_cluster_commands that does all the work except the
 * collectBlocks.  This allows other iterators to further filter the list
 * before doing collectBlocks.
 */
async function all_user_cluster_commands_helper(options) {
  let endpointTypes = await templateUtil.ensureEndpointTypeIds(this)
  let packageIds = await templateUtil.ensureZclPackageIds(this)
  let endpointsAndClusters =
    await queryEndpointType.selectClustersAndEndpointDetailsFromEndpointTypes(
      this.global.db,
      endpointTypes
    )

  return queryCommand.selectCommandDetailsFromAllEndpointTypesAndClusters(
    this.global.db,
    endpointsAndClusters,
    true,
    packageIds
  )
}

exports.all_user_cluster_commands_helper = all_user_cluster_commands_helper
