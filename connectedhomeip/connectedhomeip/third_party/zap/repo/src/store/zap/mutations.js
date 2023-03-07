/**
 *
 *    Copyright (c) 2020 Silicon Labs
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

const reportingMinDefault = 1

export const togglePreviewTab = (state) => {
  state.showPreviewTab = !state.showPreviewTab
}

export const updateShowDevTools = (state) => {
  state.showDevTools = !state.showDevTools
}

export function updateInformationText(state, text) {
  state.informationText = text
}

export function setAllPackages(state, packages) {
  state.allPackages = packages
}

export function updateClusters(state, clusters) {
  state.clusters = clusters
  state.domains = [...new Set(state.clusters.map((a) => a.domainName))]
}

export function updateAtomics(state, atomics) {
  state.atomics = atomics
}

export function updateSelectedCluster(state, cluster) {
  state.clustersView.selected = cluster
}

export function updateSelectedEndpoint(state, endpoint) {
  state.endpointView.selectedEndpoint = endpoint
}

export function updateAttributes(state, attributes) {
  attributes.forEach((attribute) => {
    if (state.attributeView.defaultValue[attribute.id] === undefined) {
      Vue.set(
        state.attributeView.defaultValue,
        attribute.id,
        attribute.defaultValue
      )
    }
    if (state.attributeView.reportingMin[attribute.id] === undefined) {
      Vue.set(
        state.attributeView.reportingMin,
        attribute.id,
        reportingMinDefault
      )
    }
    if (state.attributeView.reportingMax[attribute.id] === undefined) {
      Vue.set(state.attributeView.reportingMax, attribute.id, 65534)
    }
    if (state.attributeView.reportableChange[attribute.id] === undefined) {
      Vue.set(state.attributeView.reportableChange, attribute.id, 0)
    }
  })
  state.attributes = attributes
}

export function setEndpointTypeAttribute(state, endpointTypeAttribute) {
  let attribute = endpointTypeAttribute

  updateInclusionList(state, {
    id: attribute.id,
    added: attribute.included,
    listType: 'selectedAttributes',
    view: 'attributeView',
  })
  updateInclusionList(state, {
    id: attribute.id,
    added: attribute.singleton,
    listType: 'selectedSingleton',
    view: 'attributeView',
  })
  updateInclusionList(state, {
    id: attribute.id,
    added: attribute.bounded,
    listType: 'selectedBounded',
    view: 'attributeView',
  })
  updateInclusionList(state, {
    id: attribute.id,
    added: attribute.includedReportable,
    listType: 'selectedReporting',
    view: 'attributeView',
  })

  Vue.set(
    state.attributeView.defaultValue,
    attribute.id,
    attribute.defaultValue
  )
  Vue.set(
    state.attributeView.storageOption,
    attribute.id,
    attribute.storageOption
  )
  Vue.set(state.attributeView.reportingMin, attribute.id, attribute.minInterval)
  Vue.set(state.attributeView.reportingMax, attribute.id, attribute.maxInterval)
  Vue.set(
    state.attributeView.reportableChange,
    attribute.id,
    attribute.reportableChange
  )
}

export function initializeDefaultEndpoints(state, defaultEndpoints) {
  defaultEndpoints.forEach((endpoint) => {
    if (state.endpointView.endpointId[endpoint.id] === undefined) {
      Vue.set(state.endpointView.endpointId, endpoint.id, endpoint.endpointId)
    }
    if (state.endpointView.endpointType[endpoint.id] === undefined) {
      Vue.set(
        state.endpointView.endpointType,
        endpoint.id,
        endpoint.endpointTypeRef
      )
    }
    if (state.endpointView.networkId[endpoint.id] === undefined) {
      Vue.set(state.endpointView.networkId, endpoint.id, endpoint.networkId)
    }

    if (state.endpointView.profileId[endpoint.id] === undefined) {
      Vue.set(state.endpointView.profileId, endpoint.id, endpoint.profileId)
    }

    if (state.endpointView.endpointVersion[endpoint.id] === undefined) {
      Vue.set(
        state.endpointView.endpointVersion,
        endpoint.id,
        endpoint.endpointVersion
      )
    }
  })
}

export function addEndpoint(state, endpoint) {
  Vue.set(state.endpointView.endpointId, endpoint.id, endpoint.endpointId)
  Vue.set(
    state.endpointView.endpointType,
    endpoint.id,
    endpoint.endpointTypeRef
  )
  Vue.set(state.endpointView.networkId, endpoint.id, endpoint.networkId)
  Vue.set(state.endpointView.profileId, endpoint.id, endpoint.profileId)
  Vue.set(state.endpointView.deviceId, endpoint.id, endpoint.deviceIdentifier)
  Vue.set(
    state.endpointView.endpointVersion,
    endpoint.id,
    endpoint.endpointVersion
  )
  Vue.set(
    state.endpointView.endpointIdValidationIssues,
    endpoint.id,
    endpoint.endpointIdValidationIssues
  )
  Vue.set(
    state.endpointView.networkIdValidationIssues,
    endpoint.id,
    endpoint.networkIdValidationIssues
  )
}

export function updateEndpoint(state, context) {
  context.changes.forEach((data) => {
    Vue.set(state.endpointView[data.updatedKey], context.id, data.value)
  })
  Vue.set(
    state.endpointView.endpointIdValidationIssues,
    context.id,
    context.endpointIdValidationIssues
  )
  Vue.set(
    state.endpointView.networkIdValidationIssues,
    context.id,
    context.networkIdValidationIssues
  )
}

export function initializeDefaultEndpointsTypes(state, defaultEndpointsTypes) {
  defaultEndpointsTypes.forEach((endpointType) => {
    if (state.endpointTypeView.name[endpointType.id] === undefined) {
      Vue.set(state.endpointTypeView.name, endpointType.id, endpointType.name)
    }
    if (state.endpointTypeView.deviceTypeRef[endpointType.id] === undefined) {
      Vue.set(
        state.endpointTypeView.deviceTypeRef,
        endpointType.id,
        endpointType.deviceTypeRef
      )
    }
  })
}

export function addEndpointType(state, endpointType) {
  Vue.set(state.endpointTypeView.name, endpointType.id, endpointType.name)
  Vue.set(
    state.endpointTypeView.deviceTypeRef,
    endpointType.id,
    endpointType.deviceTypeRef
  )
}

export function updateAttributeDefaults(state, selectionContext) {
  Vue.set(
    state.attributeView[selectionContext.listType],
    selectionContext.id,
    selectionContext.newDefaultValue
  )
  Vue.set(
    state.attributeView.defaultValueValidationIssues,
    selectionContext.id,
    selectionContext.defaultValueValidationIssues
  )
  Vue.set(
    state.attributeView.nullValues,
    selectionContext.id,
    selectionContext.isNull
  )
}

export function updateCommands(state, commands) {
  state.commands = commands
}

export function updateEvents(state, events) {
  state.events = events
}

export function updateZclDeviceTypes(state, zclDeviceTypes) {
  state.zclDeviceTypes = zclDeviceTypes
}

export function updateEndpointConfigs(state, endpoints) {
  state.endpoints = endpoints
}

export function selectConfiguration(state, configurationName) {
  state.configurationView.selected = configurationName
}

export function updateInclusionList(state, selectionContext) {
  let inclusionList = state[selectionContext.view][selectionContext.listType]
  if (selectionContext.added && !inclusionList.includes(selectionContext.id)) {
    inclusionList.push(selectionContext.id)
  } else if (
    !selectionContext.added &&
    inclusionList.includes(selectionContext.id)
  ) {
    let elementIndex = inclusionList.indexOf(selectionContext.id)
    inclusionList.splice(elementIndex, 1)
  }
  state[selectionContext.view][selectionContext.listType] = inclusionList
}

export function setDeviceTypeReference(state, endpointIdDeviceTypeRefPair) {
  Vue.set(
    state.endpointTypeView.deviceTypeRef,
    endpointIdDeviceTypeRefPair.endpointId,
    endpointIdDeviceTypeRefPair.deviceTypeRef
  )
}

export function updateSelectedEndpointType(state, endpointType) {
  state.endpointTypeView.selectedEndpointType = endpointType
}

export function removeEndpointType(state, endpointType) {
  state.endpointTypeView.selectedEndpointType = []
  Vue.delete(state.endpointTypeView.name, endpointType.id)
  Vue.delete(state.endpointTypeView.deviceTypeRef, endpointType.id)
}

export function deleteEndpoint(state, endpoint) {
  state.endpointView.selectedEndpoint = null
  Vue.delete(state.endpointView.endpointId, endpoint.id)
  Vue.delete(state.endpointView.endpointType, endpoint.id)
  Vue.delete(state.endpointView.networkId, endpoint.id)
  Vue.delete(state.endpointView.endpointVersion, endpoint.id)
}

export function setClusterList(state, data) {
  state.clustersView.selectedClients = data.clients
  state.clustersView.selectedServers = data.servers
}

export function resetAttributeDefaults(state) {
  state.attributeView.defaultValue = {}
  state.attributeView.reportingMin = {}
  state.attributeView.reportingMin = {}
  state.attributeView.reportableChange = {}
  state.attributeView.storageOption = {}

  state.attributes.forEach((attribute) => {
    Vue.set(
      state.attributeView.defaultValue,
      attribute.id,
      attribute.defaultValue
    )
    Vue.set(state.attributeView.storageOption, attribute.id, 'ram')
    Vue.set(state.attributeView.reportingMin, attribute.id, reportingMinDefault)
    Vue.set(state.attributeView.reportingMax, attribute.id, 65534)
    Vue.set(state.attributeView.reportableChange, attribute.id, 0)
  })
}

export function setAttributeLists(state, data) {
  state.attributeView.selectedAttributes = data.included
  state.attributeView.selectedSingleton = data.singleton
  state.attributeView.selectedBounded = data.bounded
  state.attributeView.selectedReporting = data.includedReportable

  resetAttributeDefaults(state)
  Object.entries(data.defaultValue).forEach(([attributeRef, defaultVal]) => {
    Vue.set(state.attributeView.defaultValue, attributeRef, defaultVal)
  })
  Object.entries(data.storageOption).forEach(
    ([attributeRef, storageOption]) => {
      Vue.set(state.attributeView.storageOption, attributeRef, storageOption)
    }
  )

  Object.entries(data.minInterval).forEach(([attributeRef, defaultVal]) => {
    Vue.set(state.attributeView.reportingMin, attributeRef, defaultVal)
  })

  Object.entries(data.maxInterval).forEach(([attributeRef, defaultVal]) => {
    Vue.set(state.attributeView.reportingMax, attributeRef, defaultVal)
  })
  Object.entries(data.reportableChange).forEach(
    ([attributeRef, defaultVal]) => {
      Vue.set(state.attributeView.reportableChange, attributeRef, defaultVal)
    }
  )
}

export function setEventLists(state, selected) {
  Vue.set(state.eventView, 'selectedEvents', selected)
}

export function setCommandLists(state, data) {
  Vue.set(state.commandView, 'selectedIn', data.incoming)
  Vue.set(state.commandView, 'selectedOut', data.outgoing)
}

export function setRecommendedClusterList(state, data) {
  Vue.set(state.clustersView, 'recommendedClients', data.recommendedClients)
  Vue.set(state.clustersView, 'recommendedServers', data.recommendedServers)
}

export function setRequiredAttributesList(state, data) {
  Vue.set(state.attributeView, 'requiredAttributes', data.requiredAttributes)
}

export function setRequiredCommandsList(state, data) {
  Vue.set(state.commandView, 'requiredCommands', data.requiredCommands)
}

export function setLeftDrawerState(state, data) {
  state.leftDrawerOpenState = data
}

export function setMiniState(state, data) {
  state.miniState = data
}

export function initializeEndpoints(state, endpoints) {
  endpoints.forEach((e) => {
    addEndpoint(state, e)
  })
}

export function initializeEndpointTypes(state, endpointTypes) {
  endpointTypes.forEach((et) => {
    addEndpointType(state, et)
  })
}

export function initializeSessionKeyValues(state, sessionKeyValues) {
  sessionKeyValues.forEach((skv) => {
    setSelectedGenericOption(state, skv)
  })
}

export function setOptions(state, data) {
  Vue.set(state.genericOptions, data.option, [
    ...new Set(
      data.data
        .filter((d) => d.optionCategory === data.option)
        .map((d) => {
          return { optionCode: d.optionCode, optionLabel: d.optionLabel }
        })
    ),
  ])
}

export function setSelectedGenericOption(state, keyValue) {
  Vue.set(state.selectedGenericOptions, keyValue.key, keyValue.value)
}

export function loadSessionKeyValues(state, sessionKeyValues) {
  sessionKeyValues.map((keyValue) => {
    Vue.set(state.selectedGenericOptions, keyValue.key, keyValue.value)
  })
}

export function setDefaultUiMode(state, uiMode) {
  Vue.set(state.calledArgs, `defaultUiMode`, uiMode)
}

export function setDebugNavBar(state, debugNavBar) {
  state.debugNavBar = debugNavBar
}

export function setStandalone(state, standalone) {
  state.standalone = standalone
}

export function setOpenDomain(state, context) {
  Vue.set(state.clusterManager.openDomains, context.domainName, context.value)
}

export function setDomainFilter(state, filterEnabledClusterPair) {
  let filter = filterEnabledClusterPair.filter
  state.clusterManager.filter = filter
  state.domains.map((domainName) => {
    setOpenDomain(state, {
      domainName: domainName,
      value: filter.domainFilterFn(
        domainName,
        state.clusterManager.openDomains,
        {
          enabledClusters: filterEnabledClusterPair.enabledClusters,
        }
      ),
    })
  })
}

export function doActionFilter(state, filterEnabledClusterPair) {
  let filter = filterEnabledClusterPair.filter
  // When we close all, we also clear all filters.
  resetFilters(state)
  state.domains.map((domainName) => {
    setOpenDomain(state, {
      domainName: domainName,
      value: filter.domainFilterFn(
        domainName,
        state.clusterManager.openDomains,
        {
          enabledClusters: filterEnabledClusterPair.enabledClusters,
        }
      ),
    })
  })
}

export function setFilterString(state, filterString) {
  state.clusterManager.filterString = filterString
}

export function setIndividualClusterFilterString(state, filterString) {
  state.clusterManager.individualClusterFilterString = filterString
}

export function setLastSelectedDomain(state, domainNameString) {
  state.clusterManager.lastSelectedDomain = domainNameString
}

export function clearLastSelectedDomain(state) {
  state.clusterManager.lastSelectedDomain = null
}

export function resetFilters(state) {
  state.clusterManager.filter = {
    label: 'No Filter',
    domainFilterFn: (domain, currentOpenDomains, context) =>
      currentOpenDomains[domain],
  }
  state.clusterManager.openDomains = {}
}

export function expandedExceptionsToggle(state) {
  state.isExceptionsExpanded = !state.isExceptionsExpanded
}

export function updateExceptions(state, value) {
  state.exceptions.push(value)
}

export function toggleShowExceptionIcon(state, value) {
  state.showExceptionIcon = value
}

export function updateProjectPackages(state, packages) {
  Vue.set(state, 'packages', packages)
}

export function updateUcComponentState(state, data) {
  if (data != null) {
    Vue.set(state.studio, 'projectInfoJson', data.projectInfoJson)
    Vue.set(state.studio, 'ucComponents', data.ucComponents)
    Vue.set(state.studio, 'selectedUcComponents', data.selectedUcComponents)
    Vue.set(state.studio, 'selectedUcComponentIds', data.selectedUcComponentIds)
  }
}

export function loadZclClusterToUcComponentDependencyMap(state, map) {
  if (map != null)
    Vue.set(state.studio, 'zclSdkExtClusterToUcComponentMap', map)
}

export function setAllEndpointsData(state, value) {
  Vue.set(state.allEndpointsData, value.endpointId,
    { 'selectedservers': value.servers,
      'selectedReporting': value.report,
      'selectedAttributes': value.attr,
      id: value.endpointId })
}

export function updateIsProfileIdShown (state, value) {
  value == 0 ? state.isProfileIdShown  = false : state.isProfileIdShown  = true
}

// This function will update the cluster stage if cluster changed it will update the endpoint data
export function updateIsClusterOptionChanged(state, value) {
  state.isClusterOptionChanged = value
}

// This function will toggle showEndpointData state and save that state
export function toggleShowEndpoint(state, item) {
  Vue.set(state.showEndpointData, item.id, item.value)
}