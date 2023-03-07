/**
 *
 *    Copyright (c) 2020 Silicon Labs
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
import Vue from 'vue'
import CommonMixin from '../util/common-mixin'

/**
 * This module provides common properties used across various vue components related to attribute editting.
 */
export default {
  mixins: [CommonMixin],
  computed: {
    relevantAttributeData: {
      get() {
        return this.$store.state.zap.attributes
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
    selection: {
      get() {
        return this.$store.state.zap.attributeView.selectedAttributes
      },
    },
    nullValues: {
      get() {
        return this.$store.state.zap.attributeView.nullValues
      },
    },
    selectionSingleton: {
      get() {
        return this.$store.state.zap.attributeView.selectedSingleton
      },
    },
    selectionBounded: {
      get() {
        return this.$store.state.zap.attributeView.selectedBounded
      },
    },
    selectionStorageOption: {
      get() {
        return this.$store.state.zap.attributeView.storageOption
      },
    },
    selectionDefault: {
      get() {
        return this.$store.state.zap.attributeView.defaultValue
      },
    },
    selectedReporting: {
      get() {
        return this.$store.state.zap.attributeView.selectedReporting
      },
    },
    selectionMin: {
      get() {
        return this.$store.state.zap.attributeView.reportingMin
      },
    },
    selectionMax: {
      get() {
        return this.$store.state.zap.attributeView.reportingMax
      },
    },
    selectionReportableChange: {
      get() {
        return this.$store.state.zap.attributeView.reportableChange
      },
    },
    defaultValueValidation: {
      get() {
        return this.$store.state.zap.attributeView.defaultValueValidationIssues
      },
    },
    individualClusterFilterString: {
      get() {
        return this.$store.state.zap.clusterManager
          .individualClusterFilterString
      },
    },
  },
  watch: {},
  methods: {
    handleLocalChange(localChanges, listType, attributeData, clusterId) {
      this.handleAttributeDefaultChange(
        localChanges,
        listType,
        attributeData,
        clusterId
      )
    },
    handleLocalSelection(selectedList, listType, attributeData, clusterId) {
      let hash = this.hashAttributeIdClusterId(attributeData.id, clusterId)

      this.setAttributeSelection(
        selectedList.includes(hash),
        listType,
        attributeData,
        clusterId
      )
    },
    setAttributeSelection(enable, listType, attributeData, clusterId) {
      let editContext = {
        action: 'boolean',
        endpointTypeIdList: this.endpointTypeIdList,
        id: attributeData.id,
        value: enable,
        listType: listType,
        clusterRef: clusterId,
        attributeSide: attributeData.side,
        reportMinInterval: attributeData.reportMinInterval,
        reportMaxInterval: attributeData.reportMaxInterval,
        reportableChange: attributeData.reportableChange,
      }
      this.$store.dispatch('zap/updateSelectedAttribute', editContext)
    },
    isDefaultValueValid(id) {
      return this.defaultValueValidation[id] != null
        ? this.defaultValueValidation[id].length === 0
        : true
    },
    getDefaultValueErrorMessage(id) {
      return this.defaultValueValidation[id] != null
        ? this.defaultValueValidation[id].reduce(
            (validationIssueString, currentVal) => {
              return validationIssueString + '\n' + currentVal
            },
            ''
          )
        : ''
    },
    initializeBooleanEditableList(
      originatingList,
      editableList,
      attrClusterHash
    ) {
      if (originatingList.includes(attrClusterHash)) {
        if (!editableList.includes(attrClusterHash)) {
          editableList.push(attrClusterHash)
        }
      } else {
        if (editableList.includes(attrClusterHash)) {
          let index = editableList.indexOf(attrClusterHash)
          editableList.splice(index, 1)
        }
      }
    },
    handleAttributeDefaultChange(newValue, listType, attributeData, clusterId) {
      let editContext = {
        action: 'text',
        endpointTypeIdList: this.endpointTypeIdList,
        id: attributeData.id,
        value: newValue,
        listType: listType,
        clusterRef: clusterId,
        attributeSide: attributeData.side,
        reportMinInterval: attributeData.reportMinInterval,
        reportMaxInterval: attributeData.reportMaxInterval,
      }
      this.$store.dispatch('zap/updateSelectedAttribute', editContext)
    },
    toggleAttributeSelection(list, listType, attributeData, clusterId) {
      // We determine the ID that we need to toggle within the list.
      // This ID comes from hashing the base ZCL attribute and cluster data.
      let indexOfValue = list.indexOf(
        this.hashAttributeIdClusterId(attributeData.id, clusterId)
      )
      let addedValue
      if (indexOfValue === -1) {
        addedValue = true
      } else {
        addedValue = false
      }

      let editContext = {
        action: 'boolean',
        endpointTypeIdList: this.endpointTypeIdList,
        id: attributeData.id,
        value: addedValue,
        listType: listType,
        clusterRef: clusterId,
        attributeSide: attributeData.side,
        reportMinInterval: attributeData.reportMinInterval,
        reportMaxInterval: attributeData.reportMaxInterval,
      }
      this.$store.dispatch('zap/updateSelectedAttribute', editContext)
      if (
        addedValue &&
        listType === 'selectedAttributes' &&
        attributeData.isReportable
      ) {
        editContext.listType = 'selectedReporting'
        this.$store.dispatch('zap/updateSelectedAttribute', editContext)
      }
    },

    initializeTextEditableList(originatingList, editableList, attrClusterHash) {
      let data = originatingList[attrClusterHash]
      editableList[attrClusterHash] = data
    },

    setEditableAttribute(attributeData, selectedClusterId) {
      let initContext = {
        action: 'init',
        endpointTypeId: this.selectedEndpointTypeId,
        id: attributeData.id,
        value: 'init',
        listType: 'init',
        clusterRef: selectedClusterId,
        attributeSide: attributeData.side,
        reportMinInterval: attributeData.reportMinInterval,
        reportMaxInterval: attributeData.reportMaxInterval,
      }
      this.$store
        .dispatch('zap/initSelectedAttribute', initContext)
        .then(() => {
          this.$store.dispatch('zap/setAttributeEditting', {
            attributeId: attributeData.id,
            editState: true,
          })
        })
    },
    setEditableAttributeReporting(attributeId, selectedClusterId) {
      this.$store.dispatch('zap/setAttributeReportingEditting', {
        attributeId: attributeId,
        editState: true,
      })
    },

    resetAttributeReporting(attributeId) {
      this.$store.dispatch('zap/setAttributeReportingEditting', {
        attributeId: attributeId,
        editState: false,
      })
    },
    resetAttribute(attributeId) {
      this.$store.dispatch('zap/setAttributeEditting', {
        attributeId: attributeId,
        editState: false,
      })
    },

    sortByText(x, y, ascendingA, ascendingB, callback = (i, j) => 0) {
      return x.toLowerCase() > y.toLowerCase()
        ? 1
        : x.toLowerCase() < y.toLowerCase()
        ? -1
        : callback(ascendingA, ascendingB)
    },
    sortByBoolean(
      x,
      y,
      ascendingA,
      ascendingB,
      singletonList,
      callback = (i, j) => 0
    ) {
      let i = this.hashAttributeIdClusterId(x.id, this.selectedCluster.id)
      let j = this.hashAttributeIdClusterId(y.id, this.selectedCluster.id)
      if (singletonList.indexOf(i) != -1 && singletonList.indexOf(j) != -1) {
        return callback(ascendingA, ascendingB)
      } else {
        if (singletonList.indexOf(i) != -1) return 1
        else if (singletonList.indexOf(j) != -1) return -1
        else return callback(ascendingA, ascendingB)
      }
    },

    sortByClusterAndManufacturerCode(x, y) {
      if (x['manufacturerCode'] == y['manufacturerCode']) {
        return x['code'] > y['code'] ? 1 : x['code'] < y['code'] ? -1 : 0
      } else {
        return x['manufacturerCode'] > y['manufacturerCode']
          ? 1
          : x['manufacturerCode'] < y['manufacturerCode']
          ? -1
          : 0
      }
    },
  },
}
