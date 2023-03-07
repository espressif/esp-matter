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
  <div style="width: 800px; max-width: 800px; height: 500px; max-height: 500px">
    <q-card>
      <q-card-section
        ><q-img src="~assets/zap_splash.png">
          <div class="absolute-bottom text-subtitle1 text-center">
            Version {{ version }} (feature level: {{ featureLevel }}, commit #{{
              hash
            }}
            from {{ date }})
            <br />
            &copy; 2020 by the authors. Released as open-source, under terms of
            Apache 2.0 license. {{ version }}
          </div>
        </q-img>
      </q-card-section>
    </q-card>
  </div>
</template>
<script>
const restApi = require(`../../src-shared/rest-api.js`)

export default {
  name: 'About',
  mounted() {
    if (this.$serverGet != null) {
      this.$serverGet(restApi.uri.version).then((result) => {
        this.version = result.data.version
        this.featureLevel = result.data.featureLevel
        if ('hash' in result.data) {
          this.hash = result.data.hash.substring(0, 7)
        } else {
          this.hash = 'Unknown hash'
        }
        if ('date' in result.data) {
          let d = new Date(result.data.date)
          this.date = `${d.getMonth() + 1}/${d.getDate()}/${d.getFullYear()}`
        } else {
          this.date = 'Unknown date'
        }
      })
    }
  },
  data() {
    return {
      version: '',
      featureLevel: '',
      hash: '',
      date: '',
    }
  },
}
</script>
