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
  <q-card>
    <q-card-section>
      <q-tree
        :nodes="ucComponentTree"
        node-key="id"
        tick-strategy="leaf"
        :ticked.sync="ucComponentTicked"
        :expanded.sync="uc.expanded"
        @update:ticked="handleClick"
      ></q-tree>
    </q-card-section>
  </q-card>
</template>

<script>
const restApi = require('../../src-shared/rest-api.js')
import CommonMixin from '../util/common-mixin'
const util = require('../util/util.js')

export default {
  name: 'UcComponentSetup',
  mixins: [CommonMixin],

  data() {
    return {
      value: false,
      checked: false,
      input: [],

      // q-tree attri
      uc: {
        ticked: [],
        expanded: [],
      },
    }
  },

  computed: {
    ucComponentTree: function () {
      return this.$store.state.zap.studio.projectInfoJson
    },
    ucComponentTicked: function () {
      return this.$store.state.zap.studio.selectedUcComponents.map((x) => x.id)
    },
  },

  methods: {
    handleClick: function (target) {
      let enabledItems = target.filter(
        (x) => !this.ucComponentTicked.includes(x)
      )
      let disabledItems = this.ucComponentTicked.filter(
        (x) => !target.includes(x)
      )

      enabledItems = util.getClustersByUcComponentIds(enabledItems)
      if (enabledItems.length) {
        this.updateSelectedComponentRequest({
          componentIds: enabledItems,
          added: true,
        })
      }

      disabledItems = util.getClustersByUcComponentIds(disabledItems)
      if (disabledItems.length) {
        this.updateSelectedComponentRequest({
          componentIds: disabledItems,
          added: false,
        })
      }
    },
  },
}
</script>
