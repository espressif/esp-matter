<!--
Copyright (c) 2008,2020 Silicon Labs.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
-->

<template>
  <div>
    <q-btn
      v-if="this.$store.state.zap.showDevTools"
      outline
      @click="showEnableAllClustersDialog = true"
      label="Enable All Clusters"
    />
    <q-table
      :data="clusters"
      :columns="columns"
      :visible-columns="visibleColumns"
      :rows-per-page-options="[0]"
      hide-pagination
      row-key="id"
      flat
      square
      bordered
      table-header-class="section-header"
    >
      <template v-slot:body="props">
        <q-tr
          :props="props"
          :class="
            isClusterEnabled(props.row.id)
              ? 'text-weight-bolder'
              : 'text-weight-regular'
          "
        >
          <q-td key="status" :props="props" class="q-px-none">
            <q-icon
              v-show="doesClusterHaveAnyWarnings(props.row)"
              name="warning"
              class="text-amber"
              style="font-size: 1.5rem"
              @click="selectCluster(props.row)"
            >
            </q-icon>
            <q-popup-edit
              :disable="!doesClusterHaveAnyWarnings(props.row)"
              :cover="false"
              :offset="[0, -54]"
              v-model="uc_label"
              content-class="bg-white text-black"
              style="overflow-wrap: break-word; padding: 0px"
            >
              <div v-show="missingRequiredUcComponents(props.row).length">
                <div class="row items-center" items-center style="padding: 0px">
                  <q-icon
                    name="warning"
                    class="text-amber q-mr-sm"
                    style="font-size: 1.5rem"
                  ></q-icon>
                  <div class="vertical-middle text-subtitle2">
                    Required SLC Component not installed
                  </div>
                </div>

                <div class="row no-wrap">
                  Install following components to continue endpoint
                  configuration.
                </div>

                <div class="row no-wrap">
                  <div class="col justify-start">
                    <ul style="list-style-type: none; padding-left: 0px">
                      <li
                        v-for="id in missingRequiredUcComponents(props.row)"
                        :key="id"
                      >
                        {{ ucLabel(id) }}
                      </li>
                    </ul>
                  </div>

                  <div class="justify-end">
                    <q-btn
                      unelevated
                      text-color="primary"
                      @click="enableRequiredComponents(props.row.id)"
                      >Install</q-btn
                    >
                  </div>
                </div>
              </div>

              <div
                class="row no-wrap"
                v-show="isRequiredClusterMissingForId(props.row.id)"
              >
                <q-icon
                  name="warning"
                  class="text-amber q-mr-sm"
                  style="font-size: 1.5rem"
                ></q-icon>
                The configuration is missing the
                {{ missingClusterMessage(props.row) }}
              </div>
            </q-popup-edit>
          </q-td>
          <q-td key="label" :props="props" auto-width>
            {{ props.row.label }}
          </q-td>
          <q-td key="requiredCluster" :props="props">
            {{ isClusterRequired(props.row.id) }}
          </q-td>
          <q-td key="clusterId" :props="props">
            {{ asHex(props.row.code, 4) }}
          </q-td>
          <q-td key="manufacturerId" :props="props">
            {{
              props.row.manufacturerCode
                ? asHex(props.row.manufacturerCode, 4)
                : '---'
            }}
          </q-td>
          <q-td key="enable" :props="props">
            <q-select
              :v-model="getClusterEnabledStatus(props.row.id)"
              :value="getClusterEnabledStatus(props.row.id)"
              :display-value="`${getClusterEnabledStatus(props.row.id)}`"
              :options="clusterSelectionOptions"
              dense
              outlined
              @input="handleClusterSelection(props.row.id, $event)"
            />
          </q-td>
          <q-td key="configure" :props="props">
            <q-btn
              flat
              :color="isClusterEnabled(props.row.id) ? 'primary' : 'grey'"
              dense
              :disable="!isClusterEnabled(props.row.id)"
              icon="settings"
              @click="selectCluster(props.row)"
              to="/cluster"
            />
          </q-td>
        </q-tr>
      </template>
    </q-table>
    <q-dialog
      v-model="showEnableAllClustersDialog"
      class="background-color:transparent"
    >
      <q-card>
        <q-card-section>
          <div class="text-h6">Enable All Clusters</div>
          Enabling all clusters may cause the ZCL configuration to go into an
          invalid state. Are you sure want to enable all clusters?
        </q-card-section>
        <q-card-actions>
          <q-btn label="Cancel" v-close-popup class="col" />
          <q-btn
            :label="'Enable All Clusters'"
            color="primary"
            class="col"
            v-close-popup="showEnableAllClustersDialog"
            @click="enableAllClusters()"
            id="enable_all_clusters"
          />
        </q-card-actions>
      </q-card>
    </q-dialog>
  </div>
</template>
<script>
import CommonMixin from '../util/common-mixin'

export default {
  name: 'ZclDomainClusterView',
  props: ['domainName', 'clusters'],
  mixins: [CommonMixin],
  computed: {
    showStatus: {
      get() {
        return !this.$store.state.zap.standalone
      },
    },
    recommendedClients: {
      get() {
        return this.$store.state.zap.clustersView.recommendedClients
      },
    },
    recommendedServers: {
      get() {
        return this.$store.state.zap.clustersView.recommendedServers
      },
    },
    visibleColumns: function () {
      let names = this.columns.map((x) => x.name)

      // show/hide 'status' column depending on this.showStatus
      let statusColumn = 'status'
      let statusShown = names.indexOf(statusColumn) > -1
      if (this.hasWarning() && !statusShown) {
        names.push(statusColumn)
      } else if (!this.hasWarning() && statusShown) {
        let i = names.indexOf(statusColumn)
        names.splice(i, 1)
      }
      return names
    },
  },
  methods: {
    enableAllClusters() {
      this.clusters.forEach((singleCluster) => {
        this.handleClusterSelection(singleCluster.id, {
          label: 'Client & Server',
          client: true,
          server: true,
        })
      })
    },
    toggleStatus: function () {
      this.showStatus = !this.showStatus
    },
    hasWarning: function () {
      return this.showStatus || this.isAnyRequiredClusterNotEnabled()
    },
    missingClusterMessage(clusterData) {
      let missingRequiredClusterPair = this.getMissingRequiredClusterPair(
        clusterData.id
      )
      let msg = ''
      if (
        missingRequiredClusterPair.missingClient &&
        missingRequiredClusterPair.missingServer
      ) {
        msg = 'server and client clusters, which are'
      } else {
        msg = missingRequiredClusterPair.missingClient ? 'client' : 'server'
        msg = msg + ' cluster which is'
      }
      return msg + " required for this endpoint's device type."
    },
    isClusterRequired(id) {
      let clientRequired = this.recommendedClients.includes(id)
      let serverRequired = this.recommendedServers.includes(id)
      if (clientRequired && serverRequired) return 'Client & Server'
      if (clientRequired) return 'Client'
      if (serverRequired) return 'Server'
      return ''
    },
    getClusterEnabledStatus(id) {
      let hasClient = this.selectionClients.includes(id)
      let hasServer = this.selectionServers.includes(id)
      if (hasClient && hasServer) return 'Client & Server'
      if (hasClient) return 'Client'
      if (hasServer) return 'Server'
      return 'Not Enabled'
    },
    isAnyRequiredClusterNotEnabled() {
      let lackingRequiredCluster = false
      this.recommendedClients.forEach((id) => {
        if (!this.isClientEnabled(id)) {
          lackingRequiredCluster = true
        }
      })
      this.recommendedServers.forEach((id) => {
        if (!this.isServerEnabled(id)) {
          lackingRequiredCluster = true
        }
      })
      return lackingRequiredCluster
    },
    getMissingRequiredClusterPair(id) {
      return {
        missingClient:
          this.recommendedClients.includes(id) && !this.isClientEnabled(id),
        missingServer:
          this.recommendedServers.includes(id) && !this.isServerEnabled(id),
      }
    },
    isRequiredClusterMissingForId(id) {
      let missingRequiredClusterPair = this.getMissingRequiredClusterPair(id)
      if (
        missingRequiredClusterPair.missingClient ||
        missingRequiredClusterPair.missingServer
      )
        return true
      return false
    },
    doesClusterHaveAnyWarnings(clusterData) {
      let id = clusterData.id
      if (this.isRequiredClusterMissingForId(id)) return true
      if (this.missingRequiredUcComponents(clusterData).length) return true
      return false
    },
    isClusterEnabled(id) {
      return (
        this.selectionClients.includes(id) || this.selectionServers.includes(id)
      )
    },
    isClientEnabled(id) {
      return this.selectionClients.includes(id)
    },
    isServerEnabled(id) {
      return this.selectionServers.includes(id)
    },
    handleClusterSelection(id, event) {
      let clientSelected = event.client
      let serverSelected = event.server

      this.$store
        .dispatch('zap/updateSelectedClients', {
          endpointTypeId: this.selectedEndpointTypeId,
          id: id,
          added: clientSelected,
          listType: 'selectedClients',
          view: 'clustersView',
        })
        .then(() =>
          this.$store.dispatch('zap/updateSelectedServers', {
            endpointTypeId: this.selectedEndpointTypeId,
            id: id,
            added: serverSelected,
            listType: 'selectedServers',
            view: 'clustersView',
          })
        )
        .then(() => {
          if (this.shareClusterStatesAcrossEndpoints()) {
            this.$store.dispatch('zap/shareClusterStatesAcrossEndpoints', {
              endpointTypeIdList: this.endpointTypeIdList,
            })
          }

          this.enableRequiredComponents(id)
        })
        .then(() => {
          this.$store.commit('zap/updateIsClusterOptionChanged', true)
        })
    },
    enableRequiredComponents(id) {
      let hasClient = this.selectionClients.includes(id)
      let hasServer = this.selectionServers.includes(id)

      let side = []
      if (hasClient) {
        side.push('client')
      }
      if (hasServer) {
        side.push('server')
      }

      this.updateSelectedComponentRequest({
        clusterId: id,
        side: side,
        added: true,
      })
    },
    selectCluster(cluster) {
      this.$store.dispatch('zap/updateSelectedCluster', cluster).then(() => {
        this.$store.dispatch(
          'zap/refreshEndpointTypeCluster',
          this.selectedEndpointTypeId
        )
        this.$store.dispatch('zap/setLastSelectedDomain', this.domainName)
      })
    },
    ucLabel(id) {
      let list = this.$store.state.zap.studio.ucComponents.filter(
        (x) => x.name === id
      )
      return list && list.length ? list[0].label : ''
    },
    missingRequiredUcComponents(cluster) {
      return this.missingUcComponentDependencies(cluster)
    },
  },
  data() {
    return {
      showEnableAllClustersDialog: false,
      uc_label: 'uc label',
      clusterSelectionOptions: [
        { label: 'Not Enabled', client: false, server: false },
        { label: 'Client', client: true, server: false },
        { label: 'Server', client: false, server: true },
        { label: 'Client & Server', client: true, server: true },
      ],
      columns: [
        {
          name: 'status',
          required: false,
          label: '',
          align: 'left',
          field: (row) => row.code,
          style: 'width: 100px;padding-left: 10px;padding-right: 0px;',
        },
        {
          name: 'label',
          required: true,
          label: 'Cluster',
          align: 'left',
          field: (row) => row.label,
          style: 'width:28%',
        },
        {
          name: 'requiredCluster',
          required: true,
          label: 'Required Cluster',
          align: 'center',
          field: (row) => this.isClusterRequired(row.id),
          style: 'width:20%',
        },
        {
          name: 'clusterId',
          required: false,
          label: 'Cluster ID',
          align: 'left',
          field: (row) => row.code,
          style: 'width:10%',
        },
        {
          name: 'manufacturerId',
          required: false,
          label: 'Manufacturer Code',
          align: 'left',
          field: (row) => (row.manufacturerCode ? row.manufacturerCode : '---'),
          style: 'width:10%',
        },
        {
          name: 'enable',
          required: false,
          label: 'Enable',
          align: 'left',
          field: (row) => 'test',
          style: 'width:20%',
        },
        {
          name: 'configure',
          required: true,
          label: 'Configure',
          align: 'center',
          style: 'width: 10%',
        },
      ],
    }
  },
}
</script>

<!-- Notice lang="scss" -->
<style lang="scss">
.bar {
  background-color: $grey-4;
  padding: 15px 15px 15px 15px;
}
</style>
