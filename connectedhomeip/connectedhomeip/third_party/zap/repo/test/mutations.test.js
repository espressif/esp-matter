/**
 *
 *    Copyright (c) 2021 Silicon Labs
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
 *
 *
 * @jest-environment node
 */

import ZapState from '../src/store/zap/state.js'
import { timeout } from './test-util.js'
const mutations = require('../src/store/zap/mutations.js')
const restApi = require('../src-shared/rest-api')

test(
  'updateInformationText',
  () => {
    let text = 'foobar'
    let state = ZapState()
    mutations.updateInformationText(state, text)
    expect(state.informationText).toEqual(text)
  },
  timeout.short()
)

test(
  'updateClusters',
  () => {
    let clusters = [
      { name: 'foo', domainName: 'bar' },
      { name: 'bar', domainName: 'bar' },
    ]
    let domains = ['bar']
    let state = ZapState()
    mutations.updateClusters(state, clusters)
    expect(state.clusters).toEqual(clusters)
    expect(state.domains).toEqual(domains)
  },
  timeout.short()
)

test(
  'updateSelectedCluster',
  () => {
    let cluster = 'foobar'
    let state = ZapState()
    mutations.updateSelectedCluster(state, cluster)
    expect(state.clustersView.selected).toEqual(cluster)
  },
  timeout.short()
)

test(
  'updateAttributeDefaults',
  () => {
    let selectionContext = {
      id: 0,
      listType: 'defaultValue',
      newDefaultValue: 'foo',
      defaultValueValidationIssues: [],
    }
    let state = ZapState()
    mutations.updateAttributeDefaults(state, selectionContext)
    expect(state.attributeView.defaultValue[selectionContext.id]).toEqual(
      selectionContext.newDefaultValue
    )
    expect(
      state.attributeView.defaultValueValidationIssues[selectionContext.id]
    ).toEqual(selectionContext.defaultValueValidationIssues)
  },
  timeout.short()
)

test(
  'updateSelectedEndpoint',
  () => {
    let endpoint = 'foobar'
    let state = ZapState()
    mutations.updateSelectedEndpoint(state, endpoint)
    expect(state.endpointView.selectedEndpoint).toEqual(endpoint)
  },
  timeout.short()
)

test(
  'updateAttributes',
  () => {
    let attributes = ['foo', 'bar']
    let state = ZapState()
    mutations.updateAttributes(state, attributes)
    expect(state.attributes).toEqual(attributes)
  },
  timeout.short()
)

test(
  'updateCommands',
  () => {
    let commands = ['foo', 'bar']
    let state = ZapState()
    mutations.updateCommands(state, commands)
    expect(state.commands).toEqual(commands)
  },
  timeout.short()
)

test(
  'setCommandLists',
  () => {
    let data = { incoming: [1, 2, 3], outgoing: [4, 5, 6] }
    let state = ZapState()
    mutations.setCommandLists(state, data)
    expect(state.commandView.selectedIn).toEqual(data.incoming)
    expect(state.commandView.selectedOut).toEqual(data.outgoing)
  },
  timeout.short()
)

test(
  'initializeDefaultEndpoints',
  () => {
    let endpoints = [{ id: 0, endpointId: 1 }]
    let state = ZapState()
    mutations.initializeDefaultEndpoints(state, endpoints)
    expect(state.endpointView.endpointId[0]).toEqual(1)
  },
  timeout.short()
)

test(
  'addEndpoint',
  () => {
    let endpoints = { id: 0, endpointId: 1, endpointTypeRef: 'foo' }
    let state = ZapState()
    mutations.addEndpoint(state, endpoints)
    expect(state.endpointView.endpointType[0]).toEqual('foo')
  },
  timeout.short()
)

test(
  'updateEndpoint',
  () => {
    let endpoints = { id: 0, endpointId: 1, endpointTypeRef: 'foo' }
    let state = ZapState()
    mutations.addEndpoint(state, endpoints)
    let context = {
      id: 0,
      changes: [{ updatedKey: 'endpointType', value: 'bar' }],
    }
    mutations.updateEndpoint(state, context)
    expect(state.endpointView['endpointType'][0]).toEqual('bar')
  },
  timeout.short()
)

test(
  'initializeDefaultEndpointTypes',
  () => {
    let endpointTypes = [
      { id: 0, endpointId: 1, name: 'foo', deviceTypeRef: 'bar' },
    ]
    let state = ZapState()
    mutations.initializeDefaultEndpointsTypes(state, endpointTypes)
    expect(state.endpointTypeView.name[0]).toEqual(endpointTypes[0].name)
  },
  timeout.short()
)

test(
  'addEndpointType',
  () => {
    let endpointType = { id: 0, endpointId: 1, name: 'foo' }
    let state = ZapState()
    mutations.addEndpointType(state, endpointType)
    expect(state.endpointTypeView.name[0]).toEqual('foo')
  },
  timeout.short()
)

test(
  'Reset domain filter',
  () => {
    let state = ZapState()
    state.clusterManager.filter.label = 'blah'
    state.clusterManager.openDomains = { foo: 1, bar: 2, tuna: 3 }
    expect(Object.keys(state.clusterManager.openDomains).length).toEqual(3)
    expect(state.clusterManager.filter.label).toEqual('blah')
    mutations.resetFilters(state)
    expect(state.clusterManager.filter.label).toEqual(restApi.noFilter)
    expect(Object.keys(state.clusterManager.openDomains).length).toBe(0)
  },
  timeout.short()
)

test(
  'Set endpoint type attribute',
  () => {
    let state = ZapState()
    let eptAttr = {
      id: 5,
      included: true,
      singleton: true,
      bounded: false,
      includedReportable: true,
      defaultValue: '423',
      storageOption: 'ram',
      minInterval: 0,
      maxInterval: 532,
      reportableChange: 40,
    }
    mutations.setEndpointTypeAttribute(state, eptAttr)
    expect(
      state.attributeView.selectedAttributes.find((a) => a == eptAttr.id)
    ).toBe(5)
    expect(
      state.attributeView.selectedSingleton.find((a) => a == eptAttr.id)
    ).toBe(5)
    expect(
      state.attributeView.selectedBounded.find((a) => a == eptAttr.id)
    ).toBeUndefined()
    expect(
      state.attributeView.selectedReporting.find((a) => a == eptAttr.id)
    ).toBe(5)
    expect(state.attributeView.defaultValue[eptAttr.id]).toBe('423')
    expect(state.attributeView.storageOption[eptAttr.id]).toBe('ram')
    expect(state.attributeView.reportingMin[eptAttr.id]).toBe(0)
    expect(state.attributeView.reportingMax[eptAttr.id]).toBe(532)
    expect(state.attributeView.reportableChange[eptAttr.id]).toBe(40)
  },
  timeout.short()
)
