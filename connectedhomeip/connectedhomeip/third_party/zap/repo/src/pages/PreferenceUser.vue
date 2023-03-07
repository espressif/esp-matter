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
    <div class="text-h4 q-mb-md">User interface settings</div>
    <q-separator spaced="md" />
    <q-toggle
      class="q-mr-sm q-mt-lg q-mb-lg"
      label="Enable dark theme"
      dense
      left-label
      v-model="localtheme"
      id="darkTheme"
    >
      <q-tooltip> Enable Dark theme </q-tooltip>
    </q-toggle>
    <br />
    <q-toggle
      class="q-mr-sm q-mt-lg q-mb-lg"
      label="Enable development tools"
      dense
      left-label
      v-model="devtab"
    >
      <q-tooltip> Enable Dev Tools tab </q-tooltip>
    </q-toggle>
    <q-input @input="setPath" v-model="localPath" label="Last file location" />
  </div>
</template>
<script>
import * as storage from '../util/storage.js'
import rendApi from '../../src-shared/rend-api.js'
const observable = require('../util/observable.js')
export default {
  name: 'PreferenceUser',
  data() {
    return {
      localtheme: this.$q.dark.isActive,
    }
  },
  methods: {
    setPath(path) {
      storage.setItem(rendApi.storageKey.fileSave, path)
    },
    gettheme() {
      return new Promise((r) => {
        setTimeout(async () => {
          this.localtheme = this.$q.dark.isActive
        }, 1000)
      })
    },
  },
  watch: {
    localtheme(val) {
      window[rendApi.GLOBAL_SYMBOL_EXECUTE](rendApi.id.setDarkTheme, val)
    },
  },
  computed: {
    localPath: {
      get() {
        return storage.getItem(rendApi.storageKey.fileSave)
      },
    },
    devtab: {
      get() {
        return this.$store.state.zap.showDevTools
      },
      set() {
        localStorage.setItem('showDevTools', !this.devtab)
        return this.$store.dispatch('zap/updateShowDevTools')
      },
    },
  },
}
</script>
