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
  <div id="q-app">
    <q-ajax-bar color="grey" />
    <router-view />
    <q-btn
      @click="viewExceptions"
      class="fixed-bottom-right q-ma-lg"
      flat
      v-if="showExceptionIcon"
    >
      <q-icon name="warning" style="font-size: 2.5em; color: red" />
    </q-btn>
  </div>
</template>

<script>
import { QSpinnerGears } from 'quasar'
const rendApi = require(`../src-shared/rend-api.js`)
const restApi = require(`../src-shared/rest-api.js`)
const observable = require('./util/observable.js')
const dbEnum = require(`../src-shared/db-enum.js`)
const storage = require('./util/storage.js')
const _ = require('lodash')

function initLoad(store) {
  store.dispatch('zap/loadInitialData')
  store.dispatch('zap/loadOptions', {
    key: 'defaultResponsePolicy',
    type: 'string',
  })
  store.dispatch('zap/loadOptions', {
    key: 'manufacturerCodes',
    type: 'object',
  })
  store.dispatch('zap/loadOptions', {
    key: 'profileCodes',
    type: 'object',
  })
  store.dispatch('zap/loadOptions', {
    key: 'generator',
    type: 'object',
  })
  store.dispatch('zap/loadSessionKeyValues')
  if (
    localStorage.getItem('showDevTools') &&
    localStorage.getItem('showDevTools') == 'true'
  ) {
    store.dispatch('zap/updateShowDevTools')
  }

  let promises = []
  promises.push(store.dispatch('zap/updateClusters'))
  promises.push(store.dispatch('zap/updateAtomics'))
  promises.push(store.dispatch('zap/updateZclDeviceTypes'))
  promises.push(store.dispatch(`zap/getProjectPackages`))
  promises.push(store.dispatch(`zap/loadZclClusterToUcComponentDependencyMap`))
  return Promise.all(promises)
}

export default {
  name: 'App',
  computed: {
    showExceptionIcon() {
      return this.$store.state.zap.showExceptionIcon
    },
  },
  methods: {
    setGenerationInProgress(progressMessage) {
      if (progressMessage != null && progressMessage.length > 0) {
        this.$q.loading.show({
          spinner: QSpinnerGears,
          messageColor: 'white',
          message: progressMessage,
          spinnerSize: 300,
        })
      } else {
        this.$q.loading.hide()
      }
    },
    viewExceptions() {
      this.$router.push('/')
      if (!this.$store.state.zap.showDevTools)
        this.$store.dispatch('zap/updateShowDevTools')
      if (!this.$store.state.zap.isExceptionsExpanded)
        this.$store.commit('zap/expandedExceptionsToggle')
      this.$store.dispatch('zap/setDefaultUiMode', 'general')
      this.$store.commit('zap/toggleShowExceptionIcon', false)
    },
  },
  mounted() {
    if (this.$serverGet != null) {
      this.$serverGet(restApi.uri.uiOptions).then((res) => {
        this.$store.commit('zap/updateIsProfileIdShown', res.data.showProfileId)
        console.log(res.data);
      })
    }
    window[rendApi.GLOBAL_SYMBOL_EXECUTE](
      rendApi.id.setDarkTheme,
      storage.getItem(rendApi.storageKey.isDarkThemeActive)
    )

    this.$q.loading.show({
      spinner: QSpinnerGears,
      messageColor: 'white',
      message: 'Please wait while zap is loading...',
      spinnerSize: 300,
    })

    // Parse the query string into the front end.
    const querystring = require('querystring')
    let search = global.location.search

    if (search[0] === '?') {
      search = search.substring(1)
    }

    let query = querystring.parse(search)
    if (query[`uiMode`]) {
      this.$store.dispatch('zap/setDefaultUiMode', query[`uiMode`])
    }

    if (`debugNavBar` in query) {
      this.$store.dispatch(
        'zap/setDebugNavBar',
        query[`debugNavBar`] === 'true'
      )
    } else {
      // If we don't specify it, default is on.
      this.$store.dispatch('zap/setDebugNavBar', true)
    }

    if ('standalone' in query) {
      this.$store.dispatch('zap/setStandalone', query['standalone'])
    }

    this.zclDialogTitle = 'ZCL tab!'
    this.zclDialogText = 'Welcome to ZCL tab. This is just a test of a dialog.'
    this.zclDialogFlag = false

    observable.observeAttribute(
      rendApi.observable.progress_attribute,
      (message) => {
        this.setGenerationInProgress(message)
      }
    )

    initLoad(this.$store).then(() => {
      this.$q.loading.hide()
    })

    this.$onWebSocket(dbEnum.wsCategory.ucComponentStateReport, (resp) => {
      this.$store.dispatch('zap/updateUcComponentState', resp)
    })
  },
}
</script>
