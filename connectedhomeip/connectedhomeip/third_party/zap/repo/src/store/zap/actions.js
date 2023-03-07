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
import * as Util from '../../util/util.js'
import restApi from '../../../src-shared/rest-api.js'
import dbEnum from '../../../src-shared/db-enum.js'

const http = require('http-status-codes')

export function updateShowDevTools(context) {
  context.commit('updateShowDevTools')
}

export function updateExceptions(context, data) {
  context.commit('updateExceptions', data)
  context.commit('toggleShowExceptionIcon', true)
}

export function updateInformationText(context, text) {
  Vue.prototype
    .$serverPost(restApi.uri.saveSessionKeyValue, {
      key: dbEnum.sessionKey.informationText,
      value: text,
    })
    .then((response) => {
      context.commit('updateInformationText', text)
    })
}

export function updateClusters(context) {
  Vue.prototype.$serverGet(restApi.uri.zclCluster + 'all').then((response) => {
    context.commit('updateClusters', response.data.clusterData)
  })
}

export function updateAtomics(context) {
  Vue.prototype.$serverGet(restApi.uri.zclAtomics + 'all').then((response) => {
    context.commit('updateAtomics', response.data)
  })
}

export async function updateSelectedCluster(context, cluster) {
  let res = await Vue.prototype.$serverGet(
    restApi.uri.zclCluster + `${cluster.id}`
  )
  context.commit('updateSelectedCluster', [cluster])
  updateAttributes(context, res.data.attributeData || [])
  updateCommands(context, res.data.commandData || [])
  updateEvents(context, res.data.eventData || [])
}

export function updateAttributes(context, attributes) {
  context.commit('updateAttributes', attributes)
}

export function updateCommands(context, commands) {
  context.commit('updateCommands', commands)
}

export function updateEvents(context, events) {
  context.commit('updateEvents', events)
}

export function updateZclDeviceTypes(context) {
  Vue.prototype
    .$serverGet(restApi.uri.zclDeviceType + 'all')
    .then((response) => {
      let deviceTypes = response.data || []
      let deviceTypeObjects = {}
      deviceTypes.forEach((deviceType) => {
        deviceTypeObjects[deviceType.id] = {
          code: deviceType.code,
          profileId: deviceType.profileId,
          label: deviceType.label,
          description: deviceType.caption,
          domain: deviceType.domain,
        }
      })
      context.commit('updateZclDeviceTypes', deviceTypeObjects)
    })
}

export function updateEndpointConfigs(context, endpointConfigs) {
  context.commit('updateEndpointConfigs', endpointConfigs)
}

export function selectConfiguration(context, configurationName) {
  context.commit('selectConfiguration', configurationName)
}

export function initSelectedAttribute(context, selectionContext) {
  return Vue.prototype
    .$serverPost(restApi.uri.attributeUpdate, selectionContext)
    .then((res) => {
      let arg = res.data
      setAttributeState(context, arg.endpointTypeAttributeData)
    })
}

export function updateSelectedAttribute(context, selectionContext) {
  Vue.prototype
    .$serverPost(restApi.uri.attributeUpdate, selectionContext)
    .then((res) => {
      let arg = res.data
      setAttributeState(context, arg.endpointTypeAttributeData)
      if (arg.action === 'boolean') {
        context.commit('updateInclusionList', {
          id: Util.cantorPair(arg.id, arg.clusterRef),
          added: arg.added,
          listType: arg.listType,
          view: 'attributeView',
        })
      } else if (arg.action === 'text') {
        let isNull = false
        if (arg.added == null) {
          arg.added = 'NULL'
          isNull = true
        }
        context.commit('updateAttributeDefaults', {
          id: Util.cantorPair(arg.id, arg.clusterRef),
          newDefaultValue: arg.added,
          listType: arg.listType,
          defaultValueValidationIssues: arg.validationIssues.defaultValue,
          isNull,
        })
      }
    })
}

export async function updateSelectedCommands(context, selectionContext) {
  let res = await Vue.prototype.$serverPost(
    restApi.uri.commandUpdate,
    selectionContext
  )
  let arg = res.data
  if (arg.action === 'boolean') {
    context.commit('updateInclusionList', {
      id: Util.cantorPair(arg.id, arg.clusterRef),
      added: arg.added,
      listType: arg.listType,
      view: 'commandView',
    })
  }
}

export function updateSelectedEvents(context, selectionContext) {
  Vue.prototype
    .$serverPost(restApi.uri.eventUpdate, selectionContext)
    .then((res) => {
      let arg = res.data
      if (arg.action === 'boolean') {
        context.commit('updateInclusionList', {
          id: Util.cantorPair(arg.id, arg.clusterRef),
          added: arg.added,
          listType: arg.listType,
          view: 'eventView',
        })
      }
    })
}

export function updateSelectedComponent(context, payload) {
  let op = payload.added ? restApi.uc.componentAdd : restApi.uc.componentRemove
  return Vue.prototype.$serverPost(op, payload)
}

export function updateSelectedServers(context, selectionContext) {
  return Vue.prototype
    .$serverPost(restApi.uri.cluster, {
      endpointTypeId: selectionContext.endpointTypeId,
      id: selectionContext.id,
      side: 'server',
      flag: selectionContext.added,
    })
    .then(() => {
      context.commit('updateInclusionList', selectionContext)
    })
}

export function updateSelectedClients(context, selectionContext) {
  return Vue.prototype
    .$serverPost(restApi.uri.cluster, {
      endpointTypeId: selectionContext.endpointTypeId,
      id: selectionContext.id,
      side: 'client',
      flag: selectionContext.added,
    })
    .then(() => {
      context.commit('updateInclusionList', selectionContext)
    })
}

export function getProjectPackages(context) {
  return Vue.prototype.$serverGet(restApi.uri.packages).then((res) => {
    let data = res.data
    context.commit('updateProjectPackages', data)
  })
}

export function initializeDefaultEndpoints(context, defaultEndpoints) {
  context.commit('initializeDefaultEndpoints', defaultEndpoints)
}
export function initializeDefaultEndpointsTypes(
  context,
  defaultEndpointsTypes
) {
  context.commit('initializeDefaultEndpointsTypes', defaultEndpointsTypes)
}

export function updateSelectedEndpoint(context, endpoint) {
  context.commit('updateSelectedEndpoint', endpoint)
}

export function updateEndpointType(context, endpointType) {
  Vue.prototype
    .$serverPatch(restApi.uri.endpointType, endpointType)
    .then((res) => {
      let arg = res.data
      if (arg.updatedKey === 'deviceTypeRef') {
        setDeviceTypeReference(context, {
          endpointId: arg.endpointTypeId,
          deviceTypeRef: arg.updatedValue,
        })
      }
    })
}

export function setDeviceTypeReference(context, endpointIdDeviceTypeRefPair) {
  Vue.prototype
    .$serverGet(
      `${restApi.uri.deviceTypeClusters}${endpointIdDeviceTypeRefPair.deviceTypeRef}`
    )
    .then((res) => {
      setRecommendedClusterList(context, res.data)
    })
  Vue.prototype
    .$serverGet(
      `${restApi.uri.deviceTypeAttributes}${endpointIdDeviceTypeRefPair.deviceTypeRef}`
    )
    .then((res) => {
      setRequiredAttributes(context, res.data)
    })
  Vue.prototype
    .$serverGet(
      `${restApi.uri.deviceTypeCommands}${endpointIdDeviceTypeRefPair.deviceTypeRef}`
    )
    .then((res) => {
      setRequiredCommands(context, res.data)
    })

  Vue.prototype
    .$serverGet(
      `${restApi.uri.endpointTypeClusters}${endpointIdDeviceTypeRefPair.endpointId}`
    )
    .then((res) => {
      setClusterList(context, res.data)
    })
  Vue.prototype
    .$serverGet(
      `${restApi.uri.endpointTypeAttributes}${endpointIdDeviceTypeRefPair.endpointId}`
    )
    .then((res) => {
      setAttributeStateLists(context, res.data || [])
    })
  Vue.prototype
    .$serverGet(
      `${restApi.uri.endpointTypeCommands}${endpointIdDeviceTypeRefPair.endpointId}`
    )
    .then((res) => {
      setCommandStateLists(context, res.data || [])
    })
  Vue.prototype
    .$serverGet(
      `${restApi.uri.endpointTypeEvents}${endpointIdDeviceTypeRefPair.endpointId}`
    )
    .then((res) => {
      setEventStateLists(context, res.data || [])
    })

  context.commit('setDeviceTypeReference', endpointIdDeviceTypeRefPair)
}

export function updateEndpoint(context, endpoint) {
  Vue.prototype.$serverPatch(restApi.uri.endpoint, endpoint).then((res) => {
    let arg = res.data
    context.commit('updateEndpoint', {
      id: arg.endpointId,
      changes: arg.changes,
      endpointIdValidationIssues: arg.validationIssues.endpointId,
      networkIdValidationIssues: arg.validationIssues.networkId,
    })
  })
}

export function addEndpoint(context, newEndpointContext) {
  return Vue.prototype
    .$serverPost(restApi.uri.endpoint, newEndpointContext)
    .then((res) => {
      let arg = res.data
      context.commit('addEndpoint', {
        id: arg.id,
        endpointId: arg.endpointId,
        endpointTypeRef: arg.endpointType,
        networkId: arg.networkId,
        profileId: arg.profileId,
        deviceIdentifier: arg.deviceId,
        endpointVersion: arg.endpointVersion,
        endpointIdValidationIssues: arg.validationIssues.endpointId,
        networkIdValidationIssues: arg.validationIssues.networkId,
      })
      return arg
    })
}

export function addEndpointType(context, endpointTypeData) {
  return Vue.prototype
    .$serverPost(restApi.uri.endpointType, endpointTypeData)
    .then((res) => {
      context.commit('addEndpointType', {
        id: res.data.id,
        name: res.data.name,
        deviceTypeRef: res.data.deviceTypeRef,
      })
      return res.data
    })
}

export function duplicateEndpointType(context, { endpointTypeId }) {
  return Vue.prototype
    .$serverPost(restApi.uri.duplicateEndpointType, { endpointTypeId: endpointTypeId })
    .then((res) => {
      return res.data
    })
}

export function deleteEndpoint(context, endpointId) {
  Vue.prototype
    .$serverDelete(restApi.uri.endpoint, { params: { id: endpointId } })
    .then((response) => {
      context.commit('deleteEndpoint', { id: response.data.id })
    })
}

export function duplicateEndpoint(context, {endpointId, endpointIdentifier, endpointTypeId}) {
  return Vue.prototype
    .$serverPost(restApi.uri.duplicateEndpoint, { id: endpointId, endpointIdentifier: endpointIdentifier, endpointTypeId: endpointTypeId })
    .then((response) => {
    return response
    }) 
}

export function deleteEndpointType(context, endpointTypeId) {
  Vue.prototype
    .$serverDelete(restApi.uri.endpointType, { params: { id: endpointTypeId } })
    .then((response) => {
      if (response.data.successful) {
        context.commit('removeEndpointType', {
          id: response.data.id,
        })
      }
    })
}

export function refreshEndpointTypeCluster(context, endpointType) {
  Vue.prototype
    .$serverGet(`${restApi.uri.endpointTypeAttributes}${endpointType}`)
    .then((res) => {
      setAttributeStateLists(context, res.data || [])
    })
  Vue.prototype
    .$serverGet(`${restApi.uri.endpointTypeCommands}${endpointType}`)
    .then((res) => {
      setCommandStateLists(context, res.data || [])
    })
  Vue.prototype
    .$serverGet(`${restApi.uri.endpointTypeEvents}${endpointType}`)
    .then((res) => {
      setEventStateLists(context, res.data || [])
    })
}

export function updateSelectedEndpointType(
  context,
  endpointTypeDeviceTypeRefPair
) {
  if (endpointTypeDeviceTypeRefPair != null) {
    Vue.prototype
      .$serverGet(
        `${restApi.uri.endpointTypeClusters}${endpointTypeDeviceTypeRefPair.endpointType}`
      )
      .then((res) => {
        setClusterList(context, res.data)
      })
    Vue.prototype
      .$serverGet(
        `${restApi.uri.endpointTypeAttributes}${endpointTypeDeviceTypeRefPair.endpointType}`
      )
      .then((res) => {
        setAttributeStateLists(context, res.data || [])
      })
    Vue.prototype
      .$serverGet(
        `${restApi.uri.endpointTypeCommands}${endpointTypeDeviceTypeRefPair.endpointType}`
      )
      .then((res) => {
        setCommandStateLists(context, res.data || [])
      })
    Vue.prototype
      .$serverGet(
        `${restApi.uri.endpointTypeEvents}${endpointTypeDeviceTypeRefPair.endpointType}`
      )
      .then((res) => {
        setEventStateLists(context, res.data || [])
      })

    Vue.prototype
      .$serverGet(
        `${restApi.uri.deviceTypeClusters}${endpointTypeDeviceTypeRefPair.deviceTypeRef}`
      )
      .then((res) => {
        setRecommendedClusterList(context, res.data)
      })
    Vue.prototype
      .$serverGet(
        `${restApi.uri.deviceTypeAttributes}${endpointTypeDeviceTypeRefPair.deviceTypeRef}`
      )
      .then((res) => {
        setRequiredAttributes(context, res.data)
      })
    Vue.prototype
      .$serverGet(
        `${restApi.uri.deviceTypeCommands}${endpointTypeDeviceTypeRefPair.deviceTypeRef}`
      )
      .then((res) => {
        setRequiredCommands(context, res.data)
      })
    context.commit(
      'updateSelectedEndpointType',
      endpointTypeDeviceTypeRefPair.endpointType
    )
  }
}

export function setClusterList(context, selectionContext) {
  let enabledClients = []
  let enabledServers = []
  selectionContext.forEach((record) => {
    if (record.enabled) {
      if (record.side === 'client') {
        enabledClients.push(record.clusterRef)
      } else {
        enabledServers.push(record.clusterRef)
      }
    }
  })
  context.commit(`setClusterList`, {
    clients: enabledClients,
    servers: enabledServers,
  })
}
export function setAttributeState(context, selectionContext) {
  let resolvedReference = Util.cantorPair(
    selectionContext.attributeRef,
    selectionContext.clusterRef
  )
  context.commit('setEndpointTypeAttribute', {
    id: resolvedReference,
    included: selectionContext.included,
    singleton: selectionContext.singleton,
    bounded: selectionContext.bounded,
    includedReportable: selectionContext.includedReportable,
    defaultValue: selectionContext.defaultValue,
    storageOption: selectionContext.storageOption,
    minInterval: selectionContext.minInterval,
    maxInterval: selectionContext.maxInterval,
    reportableChange: selectionContext.reportableChange,
  })
}

export function setAttributeStateLists(context, selectionContext) {
  let includedAttributes = []
  let singletonAttributes = []
  let boundedAttributes = []
  let defaultValue = {}
  let storageOption = {}

  let includedReportableAttributes = []
  let min = {}
  let max = {}
  let change = {}

  selectionContext.forEach((record) => {
    let resolvedReference = Util.cantorPair(
      record.attributeRef,
      record.clusterRef
    )
    if (record.included) includedAttributes.push(resolvedReference)
    if (record.singleton) singletonAttributes.push(resolvedReference)
    if (record.bounded) boundedAttributes.push(resolvedReference)
    if (record.includedReportable)
      includedReportableAttributes.push(resolvedReference)
    defaultValue[resolvedReference] = record.defaultValue
    storageOption[resolvedReference] = record.storageOption
    min[resolvedReference] = record.minInterval
    max[resolvedReference] = record.maxInterval
    change[resolvedReference] = record.reportableChange
  })
  context.commit(`setAttributeLists`, {
    included: includedAttributes,
    singleton: singletonAttributes,
    bounded: boundedAttributes,
    defaultValue: defaultValue,
    storageOption: storageOption,
    includedReportable: includedReportableAttributes,
    minInterval: min,
    maxInterval: max,
    reportableChange: change,
  })
}

export function setEventStateLists(context, selectionContext) {
  let selected = []
  selectionContext.forEach((record) => {
    if (record.included == 1) {
      let ref = Util.cantorPair(record.eventRef, record.clusterRef)
      selected.push(ref)
    }
  })
  context.commit('setEventLists', selected)
}

export function setCommandStateLists(context, selectionContext) {
  let incoming = []
  let outgoing = []
  selectionContext.forEach((record) => {
    let resolvedReference = Util.cantorPair(
      record.commandRef,
      record.clusterRef
    )
    if (record.incoming) incoming.push(resolvedReference)
    if (record.outgoing) outgoing.push(resolvedReference)
  })
  context.commit(`setCommandLists`, {
    incoming: incoming,
    outgoing: outgoing,
  })
}

// TODO (?) This does not handle/highlight prohibited clusters. For now we just keep it in here
export function setRecommendedClusterList(context, data) {
  let recommendedClients = []
  let recommendedServers = []

  data.forEach((record) => {
    if (record.includeClient) recommendedClients.push(record.clusterRef)
    if (record.includeServer) recommendedServers.push(record.clusterRef)
  })
  context.commit(`setRecommendedClusterList`, {
    recommendedClients: recommendedClients,
    recommendedServers: recommendedServers,
  })
}

export function setRequiredAttributes(context, data) {
  let requiredAttributes = []
  data.forEach((record) => {
    if (record.attributeRef) requiredAttributes.push(record.attributeRef)
  })
  context.commit(`setRequiredAttributesList`, {
    requiredAttributes: requiredAttributes,
  })
}

export function setRequiredCommands(context, data) {
  let requiredCommands = []
  data.forEach((record) => {
    if (record.commandRef) requiredCommands.push(record.commandRef)
  })
  context.commit(`setRequiredCommandsList`, {
    requiredCommands: requiredCommands,
  })
}

export function setLeftDrawerState(context, data) {
  context.commit('setLeftDrawerState', data)
}

export function setMiniState(context, data) {
  context.commit('setMiniState', data)
}

/**
 * This action loads the initial data from the database.
 *
 * @export
 * @param {*} context
 * @param {*} data
 */
export function loadInitialData(context, data) {
  Vue.prototype.$serverGet(restApi.uri.initialState).then((response) => {
    let initialState = response.data
    if ('endpoints' in initialState) {
      context.commit('initializeEndpoints', initialState.endpoints)
    }

    if ('endpointTypes' in initialState) {
      context.commit('initializeEndpointTypes', initialState.endpointTypes)
    }

    if ('sessionKeyValues' in initialState) {
      context.commit(
        'initializeSessionKeyValues',
        initialState.sessionKeyValues
      )
    }
  })
  Vue.prototype.$serverGet(restApi.uri.getAllPackages).then((response) => {
    context.commit('setAllPackages', response.data.packages)
  })
}

/**
 * This action loads the option from the backend, including any defaults that may exist.
 */
export function loadOptions(context, option) {
  Vue.prototype
    .$serverGet(`${restApi.uri.option}/${option.key}`)
    .then((response) => {
      let optionsData = {
        data: response.data,
        option: option.key,
        type: option.type,
      }
      context.commit('setOptions', optionsData)
    })
}

/**
 * Posts a key/value pair
 *
 * @export
 * @param {*} context
 * @param {*} data Object containing 'key' and 'value'
 */
export async function setSelectedGenericKey(context, data) {
  let response = await Vue.prototype.$serverPost(
    restApi.uri.saveSessionKeyValue,
    data
  )
  context.commit('setSelectedGenericOption', response.data)
}

export async function loadSessionKeyValues(context) {
  let response = await Vue.prototype.$serverGet(
    restApi.uri.getAllSessionKeyValues
  )
  context.commit('loadSessionKeyValues', response.data)
}

/**
 * Adds a new custom package.
 *
 * @param {*} context
 * @param {*} filePath
 * @returns validity object
 */
export async function addNewPackage(context, filePath) {
  let response = await Vue.prototype.$serverPost(restApi.uri.addNewPackage, {
    path: filePath,
  })

  if (response.data.isValid) {
    let packages = await getProjectPackages(context)
    return { packages: packages, isValid: response.data.isValid }
  } else {
    return { isValid: false, err: response.data.err }
  }
}

export function deleteSessionPackage(context, sessionPackage) {
  return Vue.prototype
    .$serverDelete(restApi.uri.sessionPackage, { params: sessionPackage })
    .then((response) => {
      return getProjectPackages(context)
    })
}

export function setDefaultUiMode(context, uiMode) {
  context.commit(`setDefaultUiMode`, uiMode)
}

export function setDebugNavBar(context, debugNavBar) {
  context.commit('setDebugNavBar', debugNavBar)
}

export function setStandalone(context, standalone) {
  context.commit('setStandalone', standalone)
}

export function setAttributeEditting(context, editContext) {
  context.commit('setAttributeEditting', editContext)
}

export function setAttributeReportingEditting(context, editContext) {
  context.commit('setAttributeReportingEditting', editContext)
}

export function setOpenDomain(context, state) {
  context.commit('setOpenDomain', state)
}

export function setDomainFilter(context, filterEnabledClusterPair) {
  context.commit('setDomainFilter', filterEnabledClusterPair)
}

export function doActionFilter(context, filterEnabledClusterPair) {
  context.commit('doActionFilter', filterEnabledClusterPair)
}

export function setFilterString(context, filterString) {
  context.commit('setFilterString', filterString)
}

export function resetFilters(context) {
  context.commit('resetFilters')
}

export function setIndividualClusterFilterString(context, filterString) {
  context.commit('setIndividualClusterFilterString', filterString)
}

export function setLastSelectedDomain(context, domainNameString) {
  context.commit('setLastSelectedDomain', domainNameString)
}

export function clearLastSelectedDomain(context) {
  context.commit('clearLastSelectedDomain')
}

export function updateUcComponentState(context, projectInfoJson) {
  let ucComponents = Util.getUcComponents(projectInfoJson)
  let selectedUcComponents = Util.getSelectedUcComponents(ucComponents)
  let selectedUcComponentIds =
    Util.getClusterIdsByUcComponents(selectedUcComponents)
  context.commit('updateUcComponentState', {
    projectInfoJson,
    ucComponents,
    selectedUcComponents,
    selectedUcComponentIds,
  })
}

export function loadZclClusterToUcComponentDependencyMap(context) {
  Vue.prototype
    .$serverGet(`/zclExtension/cluster/component`)
    .then((response) => {
      context.commit(
        'loadZclClusterToUcComponentDependencyMap',
        response?.data?.defaults
      )
    })
}

export function shareClusterStatesAcrossEndpoints(context, data) {
  let { endpointTypeIdList } = data
  Vue.prototype
    .$serverPost(restApi.uri.shareClusterStatesAcrossEndpoints, {
      endpointTypeIdList,
    })
    .then((response) => {
      console.log(`${restApi.uri.shareClusterStatesAcrossEndpoints} finished.`)
    })
}

export function generateAllEndpointsData(context, endpointData) {
  let attr = []
  let report = []
  let server = []
  let promise1 = Vue.prototype.$serverGet(endpointData.clusterRequestUrl).then((res) => {
    let enabledClients = []
    let enabledServers = []
    res.data.forEach((record) => {
      if (record.enabled) {
        if (record.side === 'client') {
          enabledClients.push(record.clusterRef)
        } else {
          enabledServers.push(record.clusterRef)
        }
      }
    })
    server = [...enabledServers, ...enabledServers]
  })

  let promise2 = Vue.prototype.$serverGet(endpointData.attributesRequestUrl).then((res) => {

    res.data.forEach((record) => {
      let resolvedReference = Util.cantorPair(
        record.attributeRef,
        record.clusterRef
      )
      if (record.included) {
        attr.push(resolvedReference)
      }
      if (record.includedReportable) {
        report.push(resolvedReference)
      }
    })
  })

  Promise.all([promise1, promise2]).then(() => {
    context.commit('setAllEndpointsData', {
      endpointId: endpointData.endpointId,
      report: report,
      servers: server,
      attr: attr,
    })
  })
}
