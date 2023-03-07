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

/**
 * This module provides mappings between database columns and JS keys.
 *
 * @module DB API: DB types and enums.
 */
exports.packageType = {
  zclProperties: 'zcl-properties',
  zclXml: 'zcl-xml-child',
  zclXmlStandalone: 'zcl-xml-standalone',
  sqlSchema: 'sql-schema',
  zclSchema: 'zcl-schema',
  zclValidation: 'zcl-validation-script',
  genTemplatesJson: 'gen-templates-json',
  genSingleTemplate: 'gen-template',
  genHelper: 'gen-helper',
  genOverride: 'gen-override',
  genPartial: 'gen-partial',
}

exports.packageOptionCategory = {
  manufacturerCodes: 'manufacturerCodes',
  typeMap: 'typeMap',
  generator: 'generator',
  profileCodes: 'profileCodes',
  validationTimersFlags: 'validationTimersFlags',
  ui: 'ui',
  helperCategories: 'helperCategories',
  helperAliases: 'helperAliases',
  resources: 'resources',
}

exports.side = {
  client: 'client',
  server: 'server',
  either: 'either',
  both: 'both',
}
exports.source = {
  client: 'client',
  server: 'server',
}

exports.storageOption = {
  ram: 'RAM',
  nvm: 'NVM',
  external: 'External',
}

exports.zclType = {
  struct: 'struct',
  enum: 'enum',
  bitmap: 'bitmap',
  atomic: 'atomic',
  unknown: 'unknown',
  array: 'array',
  zclCharFormatter: 'zclCharFormatter',
  string: 'string',
  number: 'number',
}

exports.sessionKey = {
  filePath: 'filePath',
  ideProjectPath: 'ideProjectPath',
  informationText: 'informationText',
}

exports.pathRelativity = {
  relativeToZap: 'relativeToZap',
  relativeToUserHome: 'relativeToHome',
  absolute: 'absolute',
}

exports.wsCategory = {
  generic: 'generic',
  dirtyFlag: 'dirtyFlag',
  validation: 'validation',
  sessionCreationError: 'sessionCreationError',
  componentUpdateStatus: 'componentUpdateStatus',
  ucComponentStateReport: 'ucComponentStateReport',
  init: 'init',
  tick: 'tick',
}

exports.packageExtensionEntity = {
  cluster: 'cluster',
  command: 'command',
  attribute: 'attribute',
  attributeType: 'attributeType',
  deviceType: 'deviceType',
  event: 'event',
}

exports.generatorOptions = {
  postProcessMulti: 'postProcessMulti',
  postProcessSingle: 'postProcessSingle',
  routeErrToOut: 'routeErrToOut',
  postProcessConditionalFile: 'postProcessConditionalFile',
  enabled: 'enabled',
  shareClusterStatesAcrossEndpoints: 'shareClusterStatesAcrossEndpoints',
}

exports.sessionOption = {
  defaultResponsePolicy: 'defaultResponsePolicy',
  manufacturerCodes: 'manufacturerCodes',
  profileCodes: 'profileCodes',
}

const reportingPolicy = {
  mandatory: 'mandatory',
  suggested: 'suggested',
  optional: 'optional',
  prohibited: 'prohibited',
  defaultReportingPolicy: 'optional',
  resolve: (txt) => {
    switch (txt) {
      case reportingPolicy.mandatory:
      case reportingPolicy.optional:
      case reportingPolicy.suggested:
      case reportingPolicy.prohibited:
        return txt
      default:
        // Default
        return reportingPolicy.defaultReportingPolicy
    }
  },
}

exports.reportingPolicy = reportingPolicy

const storagePolicy = {
  // Allow any storage policy to be used in the UI
  any: 'any',
  // Must use external storage and bypasses attribute store altogether.
  attributeAccessInterface: 'attributeAccessInterface',
  resolve: (txt) => {
    switch (txt) {
      case storagePolicy.any:
      case storagePolicy.attributeAccess:
        return txt
      default:
        return storagePolicy.any
    }
  },
}

exports.storagePolicy = storagePolicy

// When SDK supports a custom device, these are the default values for it.
exports.customDevice = {
  domain: 'Custom',
  name: 'Custom ZCL Device Type',
  description: 'Custom ZCL device type supports any combination of clusters.',
  code: 0xffff,
  profileId: 0xffff,
}

exports.helperCategory = {
  zigbee: 'zigbee',
  matter: 'matter',
  meta: 'meta',
}
