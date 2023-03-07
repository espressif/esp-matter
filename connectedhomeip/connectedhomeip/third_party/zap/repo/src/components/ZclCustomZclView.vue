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
  <div style="width: 800px; max-width: 80vw">
    <q-card>
      <q-card-section>
        <div class="text-h5">Add Custom ZCL</div>
        <div class="row items-center">
          <div>
            You can use this functionality to add custom ZCL clusters or
            commands to the Zigbee Clusters Configurator
          </div>
          <q-space />
          <q-btn
            color="primary"
            icon="add"
            @click="browseForFile()"
          />
        </div>
      </q-card-section>
      <q-card-section>
        <q-list bordered separator>
          <template v-for="(sessionPackage, index) in packages">
            <q-item v-bind:key="index">
              <q-item-section>
                <q-expansion-item>
                  <template slot="header">
                    <q-toolbar>
                      <div>
                        <strong>{{
                          getFileName(sessionPackage.pkg.path)
                        }}</strong>
                      </div>
                      <q-space />
                      <q-btn
                        label="Delete"
                        icon="delete"
                        flat
                        @click.stop="deletePackage(sessionPackage)"
                        :disable="sessionPackage.sessionPackage.required == 1"
                      />
                      <q-btn
                        label="Relative to..."
                        outlined
                        v-show="false"
                        @click.stop
                      />
                    </q-toolbar>
                  </template>
                  Full File path: {{ sessionPackage.pkg.path }} <br />
                  Package Type: {{ sessionPackage.pkg.type }} <br />
                  Version: {{ sessionPackage.pkg.version }} <br />
                  Required:
                  {{
                    sessionPackage.sessionPackage.required ? 'True' : 'False'
                  }}
                  <br />
                </q-expansion-item>
              </q-item-section>
            </q-item>
          </template>
        </q-list>
      </q-card-section>
    </q-card>
  </div>
</template>

<script>
import CommonMixin from '../util/common-mixin'
import rendApi from '../../src-shared/rend-api.js'
const observable = require('../util/observable.js')
import { Notify } from 'quasar'

export default {
  mixins: [CommonMixin],
  methods: {
    getFileName(path) {
      let fileName = path.match(/[^/]+$/)
      return fileName.length > 0 ? fileName[0] : path
    },
    browseForFile() {
      window[rendApi.GLOBAL_SYMBOL_NOTIFY](rendApi.notifyKey.fileBrowse, {
        context: 'customXml',
        title: 'Select an XML file containing custom ZCL objects',
        mode: 'file',
        defaultPath: this.packageToLoad,
        buttonLabel: 'Open'
      })
    },
    loadNewPackage() {
      return this.$store
        .dispatch('zap/addNewPackage', this.packageToLoad)
        .then((packageStatus) => {
          if (packageStatus.isValid) {
            this.error = null
            this.$store.dispatch('zap/updateClusters')
            this.$store.dispatch('zap/updateAtomics')
          } else {
            this.error = packageStatus.err
            Notify.create({
              message: this.error,
              color: 'negative',
              position: 'top',
              html: true,
            })
          }
        })
    },
    async deletePackage(packageToDelete) {
      await this.$store.dispatch(
        'zap/deleteSessionPackage',
        packageToDelete.sessionPackage
      )
      await this.$store.dispatch('zap/updateClusters')
      await this.$store.dispatch('zap/updateAtomics')
    },
  },
  mounted() {
    if (this.$serverGet != null) {
      this.$store.dispatch('zap/getProjectPackages')
      observable.observeAttribute(
        rendApi.observable.reported_files,
        (value) => {
          if (value.context == 'customXml') {
            this.packageToLoad = value.filePaths[0]
            this.loadNewPackage().then(() => {
              this.$store.dispatch('zap/updateZclDeviceTypes')
            })
          }
        }
      )
    }
  },
  data() {
    return {
      packageToLoad: '',
      error: null,
    }
  },
}
</script>
