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
  <div class="q-pa-md">
    <q-list
      bordered
      separator
      v-for="(item, index) in exceptions"
      v-bind:key="index"
    >
      <q-item class="column">
        <div><spam class="q-mr-sm text-bold">URL:</spam> {{ item.url }}</div>
        <div>
          <spam class="q-mr-sm text-bold">Method:</spam> {{ item.method }}
        </div>
        <div v-if="item.payload !== null">
          <spam class="q-mr-sm text-bold">Payload:</spam> {{ item.payload }}
        </div>
        <div>
          <spam class="q-mr-sm text-bold">Status Code:</spam>
          {{ item.statusCode }}
        </div>
        <div>
          <spam class="q-mr-sm text-bold">Message:</spam> {{ item.message }}
        </div>
      </q-item>
    </q-list>
  </div>
</template>

<script>
export default {
  name: 'Exceptions',
  computed: {
    exceptions() {
      return this.$store.state.zap.exceptions
    },
  },
}
</script>
