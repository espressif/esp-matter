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
    <!-- Add onClick handler for new endpoint-->
    <div class="row">
      <q-btn
        class="vertical-align:middle q-pa-md q-mini-drawer-hide row-8"
        text-color="primary"
        @click="newEndpointDialog = true"
        icon="add"
        label="Add New Endpoint"
        flat
        :ripple="false"
        :unelevated="false"
        id="new_endpoint_button"
        data-test="add-new-endpoint"
      />
      <q-space />
      <q-btn
        text-color="primary"
        :icon="miniState ? 'fast_forward' : 'fast_rewind'"
        flat
        :ripple="false"
        :unelevated="false"
        @click="miniState = !miniState"
        class="col"
      />
    </div>
    <q-separator class="q-mini-drawer-hide" />
    <template v-for="(child, index) in endpoints">
      <zcl-endpoint-card
        v-bind:key="index"
        v-bind:endpointReference="child.id"
        class="q-mini-drawer-hide"
      >
      </zcl-endpoint-card>
    </template>

    <q-dialog v-model="newEndpointDialog" class="background-color:transparent">
      <zcl-create-modify-endpoint
        v-bind:endpointReference="null"
        v-on:saveOrCreateValidated="newEndpointDialog = false"
      />
    </q-dialog>
  </div>
</template>

<script>
import ZclEndpointCard from './ZclEndpointCard.vue'
import ZclCreateModifyEndpoint from './ZclCreateModifyEndpoint.vue'
import CommonMixin from '../util/common-mixin'

export default {
  name: 'ZclEndpointManager',
  components: { ZclEndpointCard, ZclCreateModifyEndpoint },
  mixins: [CommonMixin],
  mounted() {
    // initialize ZclClusterManager with first endpoint info.
    if (this.endpointIdListSorted.size && !this.selectedEndpointId) {
      this.setSelectedEndpointType(
        this.endpointIdListSorted.keys().next().value
      )
    }
  },
  computed: {
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
      set(newMiniState) {
        this.$store.dispatch('zap/setMiniState', newMiniState)
      },
    },
    endpoints: {
      get() {
        return Array.from(this.endpointIdListSorted.keys()).map((id) => ({
          id: id,
        }))
      },
    },
  },
  data() {
    return {
      newEndpointDialog: false,
    }
  },
}
</script>
