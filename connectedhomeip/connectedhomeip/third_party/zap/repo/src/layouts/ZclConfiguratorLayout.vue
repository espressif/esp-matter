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
    <q-layout view="hHh Lpr lFf" :dense="$q.screen.lt.md">
      <q-header
        elevated
        bordered
        height-hint="500"
        class="shadow-2 zclConfiguratorLayoutHeader"
      >
        <q-toolbar bordered class="shadow-2 zclConfiguratorLayoutHeader row">
          <q-toolbar-title
            v-on:click.ctrl="showVersion"
            v-if="showPreviewTab && this.endpointId[this.selectedEndpointId]"
          >
            <q-select
              filled
              :options="endpoints"
              :value="selectedEndpointId"
              label="Endpoint"
              emit-value
              map-options
              @input="setSelectedEndpoint($event)"
              style="width: 250px"
            />
          </q-toolbar-title>
          <q-toolbar-title v-on:click.ctrl="showVersion" v-else>
            Cluster Configurator:
            {{ zclProperties != undefined ? zclProperties.description : '' }}
          </q-toolbar-title>
          <q-btn
            class="hidden"
            outline
            color="primary"
            label="View Manual"
            v-on:click="openDocumentation()"
          />
          <q-btn
            flat
            icon="public"
            @click="globalOptionsDialog = !globalOptionsDialog"
            id="global_options"
          >
            <Transition name="bounce">
              <div v-if="displayButton" class="q-ml-xs">
                ZCL Global Options…
              </div>
            </Transition>
          </q-btn>
          <q-btn
            icon="list"
            align="center"
            flat
            :ripple="false"
            :unelevated="false"
            :outline="false"
            @click="zclExtensionDialog = true"
          >
            <Transition name="bounce">
              <div v-if="displayButton" class="text-align q-ml-xs">
                ZCL Extensions…
              </div></Transition
            >
          </q-btn>
        </q-toolbar>
        <q-dialog
          v-model="globalOptionsDialog"
          class="background-color:transparent"
        >
          <ZclGeneralOptionsBar />
        </q-dialog>
      </q-header>
      <!-- Not using mobile mode, so breakpoint is set at 0 -->
      <q-drawer
        v-if="!showPreviewTab"
        v-model="leftDrawerOpen"
        bordered
        :breakpoint="0"
        :mini="!leftDrawerOpen || miniState"
      >
        <zcl-endpoint-manager />
      </q-drawer>
      <q-page-container>
        <q-scroll-area style="height: 75vh; max-width: 200vh">
          <initial-content v-if="isSelectedEndpoint" />
          <zcl-cluster-manager />
        </q-scroll-area>
      </q-page-container>
    </q-layout>
    <q-dialog v-model="zclExtensionDialog" style="width: 800px">
      <ZclExtensionDialog />
    </q-dialog>
  </div>
</template>

<script>
import ZclGeneralOptionsBar from '../components/ZclGeneralOptionsBar.vue'
import ZclEndpointManager from '../components/ZclEndpointManager.vue'
import ZclClusterManager from '../components/ZclClusterManager.vue'
import InitialContent from '../components/InitialContent.vue'
import ZclExtensionDialog from '../components/ZclCustomZclView.vue'

const restApi = require('../../src-shared/rest-api.js')
const commonUrl = require('../../src-shared/common-url.js')

export default {
  name: 'ZclConfiguratorLayout',
  methods: {
    collapseOnResize(e) {
      if (e.currentTarget.innerWidth < 750) {
        this.miniState = true
      }
    },
    setSelectedEndpoint(value) {
      this.$store.dispatch('zap/updateSelectedEndpointType', {
        endpointType: this.endpointType[value],
        deviceTypeRef: this.endpointDeviceTypeRef[this.endpointType[value]],
      })
      this.$store.dispatch('zap/updateSelectedEndpoint', value)
    },
    openDocumentation() {
      window.open(commonUrl.documentationUrl, '_blank')
    },
    showVersion() {
      this.$serverGet(restApi.uri.version).then((result) => {
        let msg = `ZAP Version Information

 - version: ${result.data.version}
 - feature level: ${result.data.featureLevel}
 - date of relese commit: ${result.data.date}
 - hash of release commit: ${result.data.hash}`
        alert(msg)
      })
    },
  },
  mounted() {
    this.$store.dispatch('zap/clearLastSelectedDomain')
    window.addEventListener('resize', this.collapseOnResize)
  },
  computed: {
    displayButton() {
      return !this.$q.screen.lt.md
    },
    endpointDeviceTypeRef: {
      get() {
        return this.$store.state.zap.endpointTypeView.deviceTypeRef
      },
    },
    endpointType: {
      get() {
        return this.$store.state.zap.endpointView.endpointType
      },
    },
    selectedEndpointId: {
      get() {
        return this.$store.state.zap.endpointView.selectedEndpoint
      },
    },
    endpointId: {
      get() {
        return this.$store.state.zap.endpointView.endpointId
      },
    },
    showPreviewTab: {
      get() {
        return this.$store.state.zap.showPreviewTab
      },
      set() {
        return this.$store.dispatch('zap/togglePreviewTab')
      },
    },
    endpoints: {
      get() {
        const endpoints = []
        for (let id in this.endpointId) {
          if (this.endpointId[id]) {
            endpoints.push({
              label: `Endpoint - ${this.endpointId[id]}`,
              value: id,
            })
          }
        }
        return endpoints
      },
    },
    leftDrawerOpen: {
      get() {
        return this.$store.state.zap.leftDrawerOpenState
      },
      set(newLeftDrawerOpenState) {
        this.$store.dispatch('zap/setLeftDrawerState', newLeftDrawerOpenState)
      },
    },
    miniState: {
      get() {
        return this.$store.state.zap.miniState
      },
      set(miniState) {
        this.$store.dispatch('zap/setMiniState', miniState)
      },
    },
    isSelectedEndpoint: {
      get() {
        return this.$store.state.zap.endpointView.selectedEndpoint == null
      },
    },
    zclProperties: {
      get() {
        return this.$store.state.zap.allPackages.find(
          (single) => single.type === 'zcl-properties'
        )
      },
    },
  },
  data() {
    return {
      isExpanded: false,
      globalOptionsDialog: false,
      zclExtensionDialog: false,
    }
  },
  components: {
    ZclGeneralOptionsBar,
    ZclEndpointManager,
    ZclClusterManager,
    InitialContent,
    ZclExtensionDialog,
  },
}
</script>

<style lang="scss">
.zclConfiguratorLayoutHeader {
  border: 1px white;
  background: white;
  color: black;
  vertical-align: middle;
  margin-bottom: 5px;
}

.body--dark .zclConfiguratorLayoutHeader {
  background: $dark-header-bar-bg;
  color: white;
}

body.body--dark {
  background: #272821;
}

.bounce-enter-active {
  animation: bounce-in 0.5s;
}
.bounce-leave-active {
  animation: bounce-in 0.5s reverse;
}

@keyframes bounce-in {
  0% {
    opacity: 0;
  }
  25% {
    opacity: 0.25;
  }
  50% {
    opacity: 0.5;
  }
  75% {
    opacity: 0.75;
  }
  100% {
    opacity: 1;
  }
}
</style>
