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

import * as Util from './util'
import * as DbEnum from '../../src-shared/db-enum'
const http = require('http-status-codes')

/**
 * This module provides common computed properties used across various vue components
 */
export default {
  computed: {
    selectedEndpointTypeId: {
      get() {
        return this.$store.state.zap.endpointTypeView.selectedEndpointType
      },
    },
    endpointDeviceTypeRef: {
      get() {
        return this.$store.state.zap.endpointTypeView.deviceTypeRef
      },
    },
    selectedEndpointId: {
      get() {
        return this.$store.state.zap.endpointView.selectedEndpoint
      },
    },
    endpointIdListSorted: {
      get() {
        // return sorted endpoint (by endpoint id value, in ascending order) for display
        // parseInt is used as endpoint id value can be int or strings
        // NOTE: a Map is returned to maintain the order of the keys.
        //       coversion to an Object will reshuffle the entries.
        const endpointIds = new Map(
          Object.entries(this.$store.state.zap.endpointView.endpointId)
        )

        return new Map(
          [...endpointIds.entries()].sort((a, b) => {
            return parseInt(a[1], 16) - parseInt(b[1], 16)
          })
        )
      },
    },
    endpointId: {
      get() {
        return this.$store.state.zap.endpointView.endpointId
      },
    },
    endpointType: {
      get() {
        return this.$store.state.zap.endpointView.endpointType
      },
    },
    selectedCluster: {
      get() {
        return this.$store.state.zap.clustersView.selected[0] || {}
      },
    },
    selectedClusterId: {
      get() {
        return this.selectedCluster.id
      },
    },
    selectionClients: {
      get() {
        return this.$store.state.zap.clustersView.selectedClients
      },
    },
    selectionServers: {
      get() {
        return this.$store.state.zap.clustersView.selectedServers
      },
    },
    zclDeviceTypes: {
      get() {
        return this.$store.state.zap.zclDeviceTypes
      },
    },
    packages: {
      get() {
        return this.$store.state.zap.packages
      },
    },
    endpointTypeIdList: {
      get() {
        if (this.shareClusterStatesAcrossEndpoints()) {
          return Object.keys(this.endpointId)
        } else {
          return [this.selectedEndpointTypeId]
        }
      },
    },
  },
  methods: {
    getSmallestUnusedEndpointId() {
      let id = 1
      for (id; id < Object.values(this.endpointId).length + 1; id++) {
        if (
          _.isNil(
            _.find(
              Object.values(this.endpointId),
              (existingEndpointId) => id == existingEndpointId
            )
          )
        ) {
          return id
        }
      }
      return id
    },
    asHex(value, padding) {
      return Util.asHex(value, padding)
    },
    hashAttributeIdClusterId(attributeId, clusterId) {
      return Util.cantorPair(attributeId, clusterId)
    },
    getAttributeById(attributeId) {
      return this.$store.state.zap.attributes.find((a) => a.id == attributeId)
    },
    setSelectedEndpointType(endpointReference) {
      this.$store.dispatch('zap/updateSelectedEndpointType', {
        endpointType: this.endpointType[endpointReference],
        deviceTypeRef:
          this.endpointDeviceTypeRef[this.endpointType[endpointReference]],
      })
      this.$store.dispatch('zap/updateSelectedEndpoint', endpointReference)
    },
    sdkExtClusterCode(extEntry) {
      return extEntry ? extEntry.entityCode : ''
    },
    sdkExtUcComponentId(extEntry) {
      return extEntry ? extEntry.value : ''
    },

    /**
     * Whether ZAP is running in standalone / Electron mode or not.
     * @returns
     */
    standaloneMode() {
      return this.$store.state.zap.standalone
    },

    /**
     * Whether ZAP is running in Zigbee mode to enforce rules when Cluster
     * are shared between 2 or more endpoints.
     */
    shareClusterStatesAcrossEndpoints() {
      let res = this.$store.state.zap.genericOptions?.generator?.filter(
        (x) =>
          x.optionCode ==
          DbEnum.generatorOptions.shareClusterStatesAcrossEndpoints
      )

      if (res?.length) {
        return res[0].optionLabel === 'true'
      } else {
        return false
      }
    },

    /**
     * Enable components by pinging backend, which pings Studio jetty server.
     * @param {*} params
     */
    updateSelectedComponentRequest(params) {
      if (!this.standaloneMode()) {
        this.$store
          .dispatch('zap/updateSelectedComponent', params)
          .then((response) => {
            if (response.status != http.StatusCodes.OK) {
              console.log('Failed to update selected components!')
            }
          })
      }
    },

    /**
     * Enable components by pinging backend, which pings Studio jetty server.
     * @param {*} params
     */
    missingUcComponentDependencies(cluster) {
      let hasClient = this.selectionClients.includes(cluster.id)
      let hasServer = this.selectionServers.includes(cluster.id)

      let requiredList = []
      if (hasClient) {
        let compList = this.ucComponentRequiredByCluster(cluster, 'client')
        requiredList = requiredList.concat(
          compList.map((x) => this.sdkExtUcComponentId(x))
        )
      }

      if (hasServer) {
        let compList = this.ucComponentRequiredByCluster(cluster, 'server')
        requiredList = requiredList.concat(
          compList.map((x) => this.sdkExtUcComponentId(x))
        )
      }

      return requiredList.filter(
        (id) =>
          !this.$store.state.zap.studio.selectedUcComponentIds.includes(id)
      )
    },

    ucComponentRequiredByCluster(cluster, role) {
      let clusterRoleName = cluster.label.toLowerCase() + '-' + role
      return this.$store.state.zap.studio.zclSdkExtClusterToUcComponentMap.filter(
        (x) => this.sdkExtClusterCode(x) === clusterRoleName
      )
    },
  },
}
