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
  <div class="q-pa-md text-white bg-black">
    <q-file
      v-model="files"
      label="Included XMLs"
      multiple
      clearable
      @input="fileUpload(files)"
    />
  </div>
</template>

<script>
export default {
  name: 'ZclSettings',
  components: {},
  methods: {
    fileUpload(test) {
      console.log(test)
      if (test) {
        test.map((a) => console.log(a))
      }
    },
  },
  data() {
    return {
      files: [],
    }
  },
}
</script>
