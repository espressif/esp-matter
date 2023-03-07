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
    <q-card>
      <q-card-section>
        <div class="text-h6 text-align:left">
          {{ this.endpointReference ? 'Edit Endpoint' : 'Create New Endpoint' }}
        </div>
        <q-form>
          <q-input
            label="Endpoint"
            type="number"
            v-model="shownEndpoint.endpointIdentifier"
            ref="endpoint"
            filled
            class="col"
            :rules="[reqInteger, reqPosInt, reqUniqueEndpoint]"
            min="0"
          />
          <q-input
            v-if="$store.state.zap.isProfileIdShown"
            label="Profile ID"
            v-model="computedProfileId"
            ref="profile"
            outlined
            filled
            class="col"
            :rules="[reqInteger, reqPosInt]"
            @input="setProfileId"
          />
          <q-select
            label="Device"
            ref="device"
            outlined
            filled
            class="col"
            use-input
            hide-selected
            fill-input
            :options="deviceTypeOptions"
            v-model="computedDeviceTypeRefAndDeviceIdPair"
            :rules="[(val) => val != null || '* Required']"
            :option-label="getDeviceOptionLabel"
            @filter="filterDeviceTypes"
            @input="setDeviceTypeCallback"
            data-test="select-endpoint-input"  
          />
          <div class="q-gutter-md row">
            <q-input
              label="Network"
              type="number"
              v-model="shownEndpoint.networkIdentifier"
              ref="network"
              outlined
              filled
              stack-label
              :rules="[reqInteger, reqPosInt]"
              min="0"
            >
              <q-tooltip>
                An endpoint can be assigned a network id that corresponds to
                which network it is on.
              </q-tooltip>
            </q-input>

            <q-input
              label="Version"
              type="number"
              v-model="shownEndpoint.deviceVersion"
              ref="version"
              outlined
              filled
              stack-label
              :rules="[reqInteger, reqPosInt]"
              min="0"
            />
          </div>
        </q-form>
      </q-card-section>
      <q-card-actions>
        <q-btn label="Cancel" v-close-popup class="col" />
        <q-btn
          :label="endpointReference ? 'Save' : 'Create'"
          color="primary"
          class="col"
          @click="saveOrCreateHandler()"
        />
      </q-card-actions>
    </q-card>
  </div>
</template>

<script>
import * as RestApi from '../../src-shared/rest-api'
import * as DbEnum from '../../src-shared/db-enum'
import CommonMixin from '../util/common-mixin'
const _ = require('lodash')

export default {
  name: 'ZclCreateModifyEndpoint',
  props: ['endpointReference'],
  emits: ['saveOrCreateValidated'],
  mixins: [CommonMixin],
  mounted() {
    if (this.endpointReference != null) {
      this.shownEndpoint.endpointIdentifier = parseInt(
        this.endpointId[this.endpointReference]
      )
      this.shownEndpoint.networkIdentifier = parseInt(
        this.networkId[this.endpointReference]
      )
      this.shownEndpoint.profileIdentifier = this.asHex(
        parseInt(this.profileId[this.endpointReference]),
        4
      )
      this.shownEndpoint.deviceVersion = parseInt(
        this.endpointVersion[this.endpointReference]
      )
      this.shownEndpoint.deviceTypeRefAndDeviceIdPair = {
        deviceTypeRef:
          this.endpointDeviceTypeRef[this.endpointType[this.endpointReference]],
        deviceIdentifier: this.endpointDeviceId[this.endpointReference],
      }
    } else {
      this.shownEndpoint.endpointIdentifier = this.getSmallestUnusedEndpointId()
    }
  },
  data() {
    return {
      deviceTypeOptions: this.zclDeviceTypeOptions,
      shownEndpoint: {
        endpointIdentifier: 1,
        profileIdentifier: null,
        networkIdentifier: 0,
        deviceVersion: 1,
        deviceTypeRefAndDeviceIdPair: {
          deviceTypeRef: null,
          deviceIdentifier: null,
        },
      },
      saveOrCreateCloseFlag: false,
    }
  },
  computed: {
    zclDeviceTypeOptions: {
      get() {
        let dt = this.$store.state.zap.zclDeviceTypes
        let keys = Object.keys(dt).sort((a, b) => {
          return dt[a].description.localeCompare(dt[b].description)
        })
        return keys.map((item) => {
          return { deviceTypeRef: item, deviceIdentifier: dt[item].code }
        })
      },
    },
    zclProfileIdString: {
      get() {
        return this.$store.state.zap.endpointView.profileId
      },
    },
    networkId: {
      get() {
        return this.$store.state.zap.endpointView.networkId
      },
    },
    profileId: {
      get() {
        return this.$store.state.zap.endpointView.profileId
      },
    },
    customDeviceIdReference: {
      get() {
        let dt = this.$store.state.zap.zclDeviceTypes
        let val = Object.keys(dt).find((a) => {
          return parseInt(dt[a].code) == parseInt(DbEnum.customDevice.code)
        })
        return val
      },
    },
    endpointVersion: {
      get() {
        return this.$store.state.zap.endpointView.endpointVersion
      },
    },
    endpointDeviceTypeRef: {
      get() {
        return this.$store.state.zap.endpointTypeView.deviceTypeRef
      },
    },
    endpointDeviceId: {
      get() {
        return this.$store.state.zap.endpointView.deviceId
      },
    },
    computedDeviceTypeRefAndDeviceIdPair: {
      get() {
        return this.shownEndpoint.deviceTypeRefAndDeviceIdPair
      },
    },
    computedProfileId: {
      get() {
        let profileOption =
          this.profileCodesOptions == null
            ? null
            : this.profileCodesOptions.find(
              (o) => o.optionCode === this.shownEndpoint.profileIdentifier
            )

        return profileOption
          ? profileOption.optionCode + ' (' + profileOption.optionLabel + ')'
          : this.shownEndpoint.profileIdentifier
      },
    },
    profileCodesOptions: {
      get() {
        return this.$store.state.zap.genericOptions[
          DbEnum.sessionOption.profileCodes
          ]
      },
    },
  },
  methods: {
    setProfileId(value) {
      this.shownEndpoint.profileIdentifier = value
    },
    setDeviceTypeCallback(value) {
      let deviceTypeRef = value.deviceTypeRef
      let profileId = this.shownEndpoint.profileIdentifier
      // On change of device type, reset the profileId to the current deviceType _unless_ the default profileId is custom
      if (this.shownEndpoint.profileIdentifier != null) {
        profileId =
          this.zclDeviceTypes[deviceTypeRef].profileId ==
          DbEnum.customDevice.profileId
            ? this.asHex(profileId, 4)
            : this.asHex(this.zclDeviceTypes[deviceTypeRef].profileId, 4)
      } else {
        profileId = this.asHex(this.zclDeviceTypes[deviceTypeRef].profileId, 4)
      }
      this.shownEndpoint.profileIdentifier = profileId
      this.shownEndpoint.deviceTypeRefAndDeviceIdPair.deviceTypeRef =
        value.deviceTypeRef
      this.shownEndpoint.deviceTypeRefAndDeviceIdPair.deviceIdentifier =
        value.deviceIdentifier
    },
    saveOrCreateHandler() {
      let profile = this.$store.state.zap.isProfileIdShown ? this.$refs.profile.validate() : true

      if (
        this.$refs.endpoint.validate() &&
        this.$refs.device.validate() &&
        this.$refs.network.validate() &&
        this.$refs.version.validate() &&
        profile
      ) {
        this.$emit('saveOrCreateValidated')
        if (this.endpointReference) {
          this.editEpt(this.shownEndpoint, this.endpointReference)
          this.$emit('updateData')
        } else {
          this.newEpt(this.shownEndpoint)
        }
      }
    },
    reqValue(value) {
      return !_.isEmpty(value) || '* Required'
    },
    reqInteger(value) {
      return Number.isInteger(parseFloat(value)) || '* Integer required'
    },
    reqPosInt(value) {
      return parseInt(value) >= 0 || '* Positive integer required'
    },
    reqUniqueEndpoint(value) {
      return (
        _.isNil(_.findKey(this.endpointId, (a) => a == value)) ||
        this.endpointReference ==
        _.findKey(this.endpointId, (a) => a == value) ||
        'Endpoint identifier must be unique'
      )
    },
    newEpt(shownEndpoint) {
      this.$store
        .dispatch(`zap/addEndpointType`, {
          name: 'Anonymous Endpoint Type',
          deviceTypeRef:
          shownEndpoint.deviceTypeRefAndDeviceIdPair.deviceTypeRef,
        })
        .then((response) => {
          this.$store
            .dispatch(`zap/addEndpoint`, {
              endpointId: parseInt(this.shownEndpoint.endpointIdentifier),
              networkId: this.shownEndpoint.networkIdentifier,
              profileId: parseInt(this.shownEndpoint.profileIdentifier),
              endpointType: response.id,
              endpointVersion: this.shownEndpoint.deviceVersion,
              deviceIdentifier:
              this.shownEndpoint.deviceTypeRefAndDeviceIdPair
                .deviceIdentifier,
            })
            .then((res) => {
              if (this.shareClusterStatesAcrossEndpoints()) {
                this.$store.dispatch('zap/shareClusterStatesAcrossEndpoints', {
                  endpointTypeIdList: this.endpointTypeIdList,
                })
              }

              this.$store.dispatch('zap/updateSelectedEndpointType', {
                endpointType: this.endpointType[res.id],
                deviceTypeRef:
                  this.endpointDeviceTypeRef[this.endpointType[res.id]],
              })

              // collect all cluster id from new endpoint
              this.selectionClients.forEach((id) => {
                this.updateSelectedComponentRequest({
                  clusterId: id,
                  side: ['client'],
                  added: true,
                })
              })

              this.selectionServers.forEach((id) => {
                this.updateSelectedComponentRequest({
                  clusterId: id,
                  side: ['server'],
                  added: true,
                })
              })

              this.$store.dispatch('zap/updateSelectedEndpoint', res.id)
            })
        })
    },
    editEpt(shownEndpoint, endpointReference) {
      let endpointTypeReference = this.endpointType[this.endpointReference]

      this.$store.dispatch('zap/updateEndpointType', {
        endpointTypeId: endpointTypeReference,
        updatedKey: RestApi.updateKey.deviceTypeRef,
        updatedValue: shownEndpoint.deviceTypeRefAndDeviceIdPair.deviceTypeRef,
      })

      this.$store.dispatch('zap/updateEndpoint', {
        id: endpointReference,
        changes: [
          {
            updatedKey: RestApi.updateKey.endpointId,
            value: parseInt(shownEndpoint.endpointIdentifier),
          },
          {
            updatedKey: RestApi.updateKey.networkId,
            value: shownEndpoint.networkIdentifier,
          },
          {
            updatedKey: RestApi.updateKey.profileId,
            value: parseInt(shownEndpoint.profileIdentifier),
          },
          {
            updatedKey: RestApi.updateKey.endpointVersion,
            value: shownEndpoint.deviceVersion,
          },
          {
            updatedKey: RestApi.updateKey.deviceId,
            value: parseInt(
              shownEndpoint.deviceTypeRefAndDeviceIdPair.deviceIdentifier
            ),
          },
        ],
      })

      // collect all cluster id from new endpoint
      this.selectionClients.forEach((id) => {
        this.updateSelectedComponentRequest({
          clusterId: id,
          side: ['client'],
          added: true,
        })
      })

      this.selectionServers.forEach((id) => {
        this.updateSelectedComponentRequest({
          clusterId: id,
          side: ['server'],
          added: true,
        })
      })

      this.$store.dispatch('zap/updateSelectedEndpointType', {
        endpointType: endpointReference,
        deviceTypeRef:
          this.endpointDeviceTypeRef[this.endpointType[this.endpointReference]],
      })
      this.$store.dispatch('zap/updateSelectedEndpoint', this.endpointReference)
    },
    getDeviceOptionLabel(item) {
      if (item == null || item.deviceTypeRef == null) return ''
      if (
        item.deviceIdentifier != this.zclDeviceTypes[item.deviceTypeRef].code
      ) {
        return this.asHex(item.deviceIdentifier, 4)
      } else {
        return (
          this.zclDeviceTypes[item.deviceTypeRef].description +
          ' (' +
          this.asHex(this.zclDeviceTypes[item.deviceTypeRef].code, 4) +
          ')'
        )
      }
    },
    createValue(val, done) {
      try {
        done(
          {
            deviceTypeRef: this.shownEndpoint.deviceTypeRefAndDeviceIdPair
              .deviceTypeRef
              ? this.shownEndpoint.deviceTypeRefAndDeviceIdPair.deviceTypeRef
              : this.customDeviceIdReference,
            deviceIdentifier: parseInt(val),
          },
          'add-unique'
        )
      } catch (err) {
        //Catch bad inputs.
        console.log(err)
      }
    },
    filterDeviceTypes(val, update) {
      if (val === '') {
        update(() => {
          this.deviceTypeOptions = this.zclDeviceTypeOptions
        })
      }
      update(() => {
        let dt = this.$store.state.zap.zclDeviceTypes
        const needle = val.toLowerCase()
        this.deviceTypeOptions = this.zclDeviceTypeOptions.filter((v) => {
          return (
            dt[v.deviceTypeRef].description.toLowerCase().indexOf(needle) > -1
          )
        })
      })
    },
  },
}
</script>
