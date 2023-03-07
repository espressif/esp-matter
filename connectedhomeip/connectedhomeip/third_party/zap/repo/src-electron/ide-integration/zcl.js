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
 * This module provides the API to access zcl specific information.
 *
 * @module REST API: user data
 */

const queryPackage = require('../db/query-package.js')
const queryZcl = require('../db/query-zcl.js')
const dbEnum = require('../../src-shared/db-enum.js')
const util = require('../util/util.js')

/**
 * Promise that return a list of component Ids required by a specific cluster
 * @param {*} db
 * @param {*} sessionId
 * @param {*} clusterId
 * @param {*} side
 * @returns {*} - {componentIds, clusterId, clusterLabel, side}
 */
function getComponentIdsByCluster(db, sessionId, clusterId, side) {
  // enable components
  let extensions = undefined
  return queryPackage
    .getSessionPackagesByType(
      db,
      sessionId,
      dbEnum.packageType.genTemplatesJson
    )
    .then((pkgs) => (pkgs.length == 0 ? null : pkgs[0].id))
    .then((id) => {
      return queryPackage.selectPackageExtension(
        db,
        id,
        dbEnum.packageExtensionEntity.cluster
      )
    })
    .then((ext) => {
      extensions = ext
      return queryZcl.selectClusterById(db, clusterId)
    })
    .then((cluster) => {
      let componentIds = []
      if (cluster) {
        side.forEach((zclRole) => {
          let clusterKey = `${cluster.label.toLowerCase()}-${zclRole}`
          let ids = util.getClusterExtensionDefault(
            extensions,
            'component',
            clusterKey
          )
          if (ids) {
            ids = ids.split(',').map((x) => x.trim())
            componentIds = componentIds.concat(ids)
          }
        })
      } else {
        console.log(`Failed to retrieve cluster via clusterId(${clusterId}).`)
      }

      return Promise.resolve({
        componentIds,
        clusterId,
        clusterLabel: cluster.label.toLowerCase(),
        side,
      })
    })
    .catch((err) => {
      console.log(
        `Failed to retrieve component ids required by clusterId(${clusterId}) from cluster extension mapping.`,
        err
      )

      return Promise.resolve({
        componentIds: [],
        clusterId,
        side,
      })
    })
}

exports.getComponentIdsByCluster = getComponentIdsByCluster
