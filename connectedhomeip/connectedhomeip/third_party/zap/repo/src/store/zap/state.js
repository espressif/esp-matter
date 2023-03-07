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
const restApi = require('../../../src-shared/rest-api.js')

export default function () {
  return {
    isProfileIdShown: null,
    showPreviewTab: false,
    isExceptionsExpanded: false,
    exceptions: [],
    showExceptionIcon: false,
    leftDrawerOpenState: true,
    miniState: false,
    informationText: '',
    clusters: [],
    atomics: [],
    domains: [],
    attributes: [],
    commands: [],
    events: [],
    zclDeviceTypes: {},
    endpoints: [],
    genericOptions: {},
    selectedGenericOptions: {},
    projectPackages: [],
    allPackages: [],
    clusterManager: {
      openDomains: {},
      lastSelectedDomain: null,
      filter: {
        label: restApi.noFilter,
        domainFilterFn: (domain, currentOpenDomains, context) =>
          currentOpenDomains[domain],
      },
      filterOptions: [
        {
          label: restApi.noFilter,
          domainFilterFn: (domain, currentOpenDomains, context) =>
            currentOpenDomains[domain],
        },
        {
          label: 'All Clusters',
          domainFilterFn: (domain, currentOpenDomains, context) => true,
        },
        {
          label: 'Enabled Clusters',
          domainFilterFn: (domain, currentOpenDomains, context) =>
            context.enabledClusters.map((a) => a.domainName).includes(domain),
          clusterFilterFn: (cluster, context) =>
            context.enabledClusters.find((a) => cluster.id == a.id) !=
            undefined,
        },
        {
          label: 'Legal Clusters',
          domainFilterFn: (domain, currentOpenDomains, context) =>
            context.enabledClusters.map((a) => a.domainName).includes(domain),
          clusterFilterFn: (cluster, context) =>
            context.enabledClusters.find((a) => cluster.id == a.id) !=
            undefined,
        },
      ],
      actionOptions: [
        {
          label: 'Close All',
          domainFilterFn: (domain, currentOpenDomains, context) => false,
        },
      ],
      filterString: '',
      individualClusterFilterString: '',
    },
    endpointView: {
      selectedEndpoint: null,
      endpointId: {},
      endpointType: {},
      networkId: {},
      profileId: {},
      deviceId: {},
      endpointVersion: {},
      endpointIdValidationIssues: {},
      networkIdValidationIssues: {},
    },
    endpointTypeView: {
      selectedEndpointType: [],
      name: {},
      deviceTypeRef: {},
    },
    clustersView: {
      selected: [],
      selectedServers: [],
      selectedClients: [],
      // These are based off of the selected ZCL Endpoints Device Type
      recommendedClients: [],
      recommendedServers: [],
    },
    attributeView: {
      selectedAttributes: [],
      selectedSingleton: [],
      selectedBounded: [],
      defaultValue: {},
      storageOption: {},
      // These are based off of the selected ZCL Endpoint Device Type
      recommendedAttributes: [],
      defaultValueValidationIssues: {},
      selectedReporting: [],
      reportingMin: {},
      reportingMax: {},
      reportableChange: {},
      nullValues: {},
    },
    commandView: {
      selectedIn: [],
      selectedOut: [],
      // These are based off of the selected ZCL Endpoint Device Type
      requiredCommands: [],
    },
    eventView: {
      selectedEvents: [],
    },
    calledArgs: {
      defaultUiMode: restApi.uiMode.ZIGBEE,
    },
    debugNavBar: false, // default visibility mode for debug navigation bar
    standalone: false, // flag indicating if ZAP frontend is running outside of Studio.
    showDevTools: false,
    studio: {
      projectInfoJson: [], // HTTP response from Studio jetty server
      ucComponents: [],
      selectedUcComponents: [], // [] of 'studio..' prefixed Studio internal component ids
      selectedUcComponentIds: [], // [] of 'zigbee_' prefixed component ids
      // a list of dict: { "clusterCode": "$zcl_cluster-$zcl_role", "value": ["$uc_component_id"] }
      zclSdkExtClusterToUcComponentMap: [],
    },
    allEndpointsData: {},
    isClusterOptionChanged: false,
    showEndpointData: {}
  }
}
