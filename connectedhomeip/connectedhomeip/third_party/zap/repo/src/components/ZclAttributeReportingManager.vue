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
  <div v-show="attributeData.length > 0">
    <q-table
      class="my-sticky-header-table"
      :data.sync="attributeData"
      :columns="columns"
      row-key="<b>name</b>"
      dense
      flat
      virtual-scroll
      binary-state-sort
      :pagination.sync="pagination"
      :sort-method="customAttributeSort"
      data-cy="Attributes Reporting"
    >
      <template v-slot:body="props">
        <q-tr :props="props">
          <q-td key="status" :props="props" class="q-px-none">
            <q-icon
              v-show="displayAttrWarning(props.row)"
              name="warning"
              class="text-amber"
              style="font-size: 1.5rem"
            />
            <q-popup-edit
              :disable="!displayAttrWarning(props.row)"
              :cover="false"
              :offset="[0, -54]"
              content-class="bg-white text-black"
              style="overflow-wrap: break-word; padding: 0px"
            >
              <div class="row items-center" items-center style="padding: 0px">
                <q-icon
                  name="warning"
                  class="text-amber q-mr-sm"
                  style="font-size: 1.5rem"
                ></q-icon>
                <div class="vertical-middle text-subtitle2">
                  Reporatble change should be in
                  {{ props.row.type.toUpperCase() }} Range
                </div>
              </div>
            </q-popup-edit>
          </q-td>
          <q-td key="enabled" :props="props" auto-width>
            <q-toggle
              :disable="checkReportingPolicy(props.row)"
              class="q-mt-xs"
              v-model="selectedReporting"
              :val="hashAttributeIdClusterId(props.row.id, selectedCluster.id)"
              indeterminate-value="false"
              keep-color
              @input="
                toggleAttributeSelection(
                  selectedReporting,
                  'selectedReporting',
                  props.row,
                  selectedCluster.id
                )
              "
            />
          </q-td>
          <q-td key="attrID" :props="props" auto-width>{{
            asHex(props.row.code, 4)
          }}</q-td>
          <q-td key="attrName" :props="props" auto-width>{{
            props.row.label
          }}</q-td>
          <q-td key="required" :props="props" auto-width>
            {{ props.row.reportingPolicy }}
          </q-td>
          <q-td key="mfgID" :props="props" auto-width>{{
            selectedCluster.manufacturerCode
              ? asHex(selectedCluster.manufacturerCode, 4)
              : props.row.manufacturerCode
              ? asHex(props.row.manufacturerCode, 4)
              : ''
          }}</q-td>
          <q-td key="clientServer" :props="props" auto-width>{{
            props.row.side === 'client' ? 'Client' : 'Server'
          }}</q-td>
          <q-td key="min" :props="props" auto-width>
            <q-input
              dense
              type="number"
              outlined
              :value="
                selectionMin[
                  hashAttributeIdClusterId(props.row.id, selectedCluster.id)
                ]
              "
              @input="
                handleLocalChange(
                  $event,
                  'reportingMin',
                  props.row,
                  selectedCluster.id
                )
              "
            />
          </q-td>
          <q-td key="max" :props="props" auto-width>
            <q-input
              dense
              type="number"
              outlined
              :value="
                selectionMax[
                  hashAttributeIdClusterId(props.row.id, selectedCluster.id)
                ]
              "
              @input="
                handleLocalChange(
                  $event,
                  'reportingMax',
                  props.row,
                  selectedCluster.id
                )
              "
            />
          </q-td>
          <q-td key="type" :props="props" auto-width>{{
            props.row.type ? props.row.type.toUpperCase() : 'UNKNOWN'
          }}</q-td>
          <q-td key="reportable" :props="props" auto-width>
            <q-input
              v-show="isAttributeAnalog(props.row)"
              dense
              bottom-slots
              hide-bottom-space
              outlined
              min="0"
              v-model="
                selectionReportableChange[
                  hashAttributeIdClusterId(props.row.id, selectedCluster.id)
                ]
              "
              :error="
                !isDefaultValueValid(
                  hashAttributeIdClusterId(props.row.id, selectedCluster.id)
                )
              "
              :error-message="
                getDefaultValueErrorMessage(
                  hashAttributeIdClusterId(props.row.id, selectedCluster.id)
                )
              "
              @input="
                handleAttributeDefaultChange(
                  $event,
                  'reportableChange',
                  props.row,
                  selectedCluster.id
                )
              "
              type="number"
            />
            <q-input
              v-show="!isAttributeAnalog(props.row)"
              label=" not analog "
              disable
              borderless
            />
          </q-td>
        </q-tr>
      </template>
    </q-table>
  </div>
</template>

<script>
//This mixin derives from common-mixin.
import EditableAttributeMixin from '../util/editable-attributes-mixin'

export default {
  name: 'ZclAttributeReportingManager',
  mixins: [EditableAttributeMixin],
  computed: {
    atomics: {
      get() {
        return this.$store.state.zap.atomics
      },
    },
    attributeData: {
      get() {
        return this.$store.state.zap.attributes
          .filter((attribute) => {
            return this.$store.state.zap.attributeView.selectedAttributes.includes(
              this.hashAttributeIdClusterId(
                attribute.id,
                this.selectedCluster.id
              )
            )
          })
          .filter((a) => {
            let relevantList =
              a.side === 'client'
                ? this.selectionClients
                : this.selectionServers
            return relevantList.includes(this.selectedClusterId)
          })
          .filter((attribute) => {
            return this.individualClusterFilterString == ''
              ? true
              : attribute.label
                  .toLowerCase()
                  .includes(this.individualClusterFilterString.toLowerCase())
          })
      },
    },
    requiredDeviceTypeAttributes: {
      get() {
        return this.$store.state.zap.attributeView.requiredAttributes
      },
    },
    requiredAttributes: {
      get() {
        console.log(
          this.relevantAttributeData.filter(
            (attribute) =>
              !attribute.isOptional ||
              this.requiredDeviceTypeAttributes.includes(attribute.id)
          )
        )
        return this.relevantAttributeData
          .filter(
            (attribute) =>
              !attribute.isOptional ||
              this.requiredDeviceTypeAttributes.includes(attribute.id)
          )
          .map((attribute) => attribute.id)
      },
    },
  },
  data() {
    return {
      pagination: {
        rowsPerPage: 0,
        sortBy: 'clientServer',
      },
      columns: [
        {
          name: 'status',
          required: false,
          label: '',
          align: 'left',
          style: 'width:1%',
        },
        {
          name: 'enabled',
          label: 'Enabled',
          field: 'enabled',
          align: 'left',
          sortable: true,
        },
        {
          name: 'attrID',
          align: 'left',
          label: 'Attribute ID',
          field: 'attrID',
          sortable: true,
          style: 'max-width: 90px',
          headerStyle: 'max-width: 90px',
        },
        {
          name: 'attrName',
          label: 'Attribute',
          field: 'attrName',
          align: 'left',
          sortable: true,
        },
        {
          name: 'required',
          label: 'Reporting Policy',
          field: 'required',
          align: 'left',
          sortable: true,
        },
        {
          name: 'clientServer',
          label: 'Client/Server',
          field: 'clientServer',
          align: 'left',
          sortable: true,
        },
        {
          name: 'mfgID',
          label: 'Mfg Code',
          align: 'left',
          field: 'mfgID',
          sortable: true,
        },
        {
          name: 'min',
          align: 'left',
          label: 'Min Interval',
          field: 'min',
        },
        {
          name: 'max',
          align: 'left',
          label: 'Max Interval',
          field: 'max',
        },
        {
          name: 'type',
          align: 'left',
          label: 'Type',
          field: 'type',
          sortable: true,
        },
        {
          name: 'reportable',
          align: 'left',
          label: 'Reportable Change',
          field: 'reportable',
        },
      ],
    }
  },
  methods: {
    checkReportingPolicy(attr) {
      if (
        attr.reportingPolicy == 'mandatory' ||
        attr.reportingPolicy == 'prohibited'
      ) {
        return true
      } else {
        return false
      }
    },
    isRowDisabled(attributeId) {
      return !this.editableAttributesReporting[attributeId]
    },
    displayAttrWarning(row) {
      let indexOfValue = this.selectedReporting.indexOf(
        this.hashAttributeIdClusterId(row.id, this.selectedCluster.id)
      )
      let isDisabled
      if (indexOfValue === -1) {
        isDisabled = true
      } else {
        isDisabled = false
      }
      return isDisabled && row.isReportable
    },
    isAttributeRequired(attribute) {
      // TODO set by reporting required
      return this.requiredAttributes.includes(attribute.id)
    },
    isAttributeAnalog(props) {
      return this.isTypeAnalog(props.type)
    },
    isTypeAnalog(typeName) {
      let atomicType = this.atomics.filter((a) => {
        return a.name == typeName
      })
      if (atomicType.length > 0) {
        return !atomicType[0].isDiscrete
      } else {
        return true
      }
    },
    customAttributeSort(rows, sortBy, descending) {
      const data = [...rows]

      if (sortBy) {
        data.sort((a, b) => {
          const x = descending ? b : a
          const y = descending ? a : b
          if (sortBy === 'enabled') {
            return this.sortByBoolean(
              x,
              y,
              a,
              b,
              this.selectedReporting,
              this.sortByClusterAndManufacturerCode
            )
          } else if (sortBy === 'attrName') {
            return this.sortByText(x['label'], y['label'], a, b)
          } else if (sortBy === 'clientServer') {
            return this.sortByText(
              x['side'],
              y['side'],
              a,
              b,
              this.sortByClusterAndManufacturerCode
            )
          }
        })
      }
      return data
    },
  },
}
</script>
<style>
/** disableing numbber input arrows */
input::-webkit-inner-spin-button {
  -webkit-appearance: none;
  margin: 0;
}
/* Firefox */
input[type='number'] {
  -moz-appearance: textfield;
}
</style>
