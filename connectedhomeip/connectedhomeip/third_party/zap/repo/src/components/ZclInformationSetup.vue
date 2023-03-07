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
    <q-input
      debounce="500"
      type="textarea"
      rounded
      filled
      v-model="informationText"
      label="Information about the application"
      :min-rows="3"
    />
  </div>
</template>

<script>
export default {
  name: 'ZclInformationSetup',
  computed: {
    informationText: {
      get() {
        return this.$store.state.zap.informationText
      },
      set(val) {
        this.$store.dispatch('zap/updateInformationText', val)
      },
    },
    state: {
      get() {
        return this.$store.state
      },
    },
  },
}
</script>
