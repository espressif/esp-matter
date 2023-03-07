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

import * as sqlite from 'sqlite3'
export type DbType = sqlite.Database

// This file is generated via ./src-script/regen-ts-type.js.  Do not hand edit this file!

export interface DbPackageType {
  crc: number
  id: number
  parentId: number
  path: string
  type: string
  version: string
}

export interface DbPackageOptionType {
  optionCategory: string
  optionCode: string
  id: number
  optionLabel: string
  packageRef: number
}

export interface DbPackageOptionDefaultType {
  optionCategory: string
  id: number
  optionRef: number
  packageRef: number
}

export interface DbPackageExtensionType {
  configurability: string
  entity: string
  globalDefault: string
  label: string
  id: number
  packageRef: number
  property: string
  type: string
}

export interface DbPackageExtensionDefaultType {
  entityCode: number
  entityQualifier: string
  manufacturerCode: number
  packageExtensionRef: number
  parentCode: number
  value: string
}

export interface DbSpecType {
  certifiable: number
  code: string
  description: string
  packageRef: number
  id: number
}

export interface DbDomainType {
  id: number
  latestSpecRef: number
  name: string
  packageRef: number
}

export interface DbClusterType {
  id: number
  code: number
  define: string
  caption: string
  domainName: string
  introducedInRef: number
  isSingleton: number
  manufacturerCode: number
  label: string
  packageRef: number
  removedInRef: number
  revision: number
}

export interface DbCommandType {
  clusterRef: number
  code: number
  id: number
  description: string
  introducedInRef: number
  isOptional: number
  manufacturerCode: number
  name: string
  packageRef: number
  removedInRef: number
  responseName: number
  responseRef: number
  source: string
}

export interface DbCommandArgType {
  commandRef: number
  countArg: string
  fieldIdentifier: number
  introducedInRef: number
  isArray: number
  name: string
  presentIf: string
  removedInRef: number
  type: string
}

export interface DbEventType {
  clusterRef: number
  code: number
  description: string
  id: number
  introducedInRef: number
  isOptional: string
  manufacturerCode: number
  name: string
  packageRef: number
  priority: string
  removedInRef: number
  side: string
}

export interface DbEventFieldType {
  eventRef: number
  fieldIdentifier: number
  introducedInRef: number
  name: string
  removedInRef: number
  type: string
}

export interface DbAttributeType {
  arrayType: string
  id: number
  clusterRef: number
  code: number
  defaultValue: string
  define: string
  introducedInRef: number
  isOptional: number
  reportingPolicy: string
  isSceneRequired: number
  isWritable: number
  manufacturerCode: number
  max: string
  maxLength: number
  min: string
  minLength: number
  name: string
  packageRef: number
  removedInRef: number
  reportableChange: string
  reportableChangeLength: number
  reportMaxInterval: number
  reportMinInterval: number
  side: string
  type: string
}

export interface DbGlobalAttributeDefaultType {
  attributeRef: number
  clusterRef: number
  defaultValue: string
  id: number
}

export interface DbGlobalAttributeBitType {
  bit: number
  globalAttributeDefaultRef: number
  tagRef: number
  value: number
}

export interface DbDeviceTypeType {
  code: number
  description: string
  id: number
  domain: string
  name: string
  packageRef: number
  profileId: number
}

export interface DbDeviceTypeClusterType {
  clusterName: string
  clusterRef: number
  id: number
  deviceTypeRef: number
  includeClient: number
  includeServer: number
  lockClient: number
  lockServer: number
}

export interface DbDeviceTypeAttributeType {
  attributeName: string
  attributeRef: number
  deviceTypeClusterRef: number
}

export interface DbDeviceTypeCommandType {
  commandName: string
  commandRef: number
  deviceTypeClusterRef: number
}

export interface DbTagType {
  clusterRef: number
  description: string
  name: string
  packageRef: number
  id: number
}

export interface DbAtomicType {
  id: number
  atomicIdentifier: number
  atomicSize: number
  description: string
  isChar: number
  isDiscrete: number
  isLong: number
  isSigned: number
  isString: number
  name: string
  packageRef: number
}

export interface DbBitmapType {
  id: number
  name: string
  packageRef: number
  type: string
}

export interface DbBitmapFieldType {
  bitmapRef: number
  fieldIdentifier: number
  mask: number
  name: string
  type: string
}

export interface DbEnumType {
  id: number
  name: string
  packageRef: number
  type: string
}

export interface DbEnumItemType {
  enumRef: number
  fieldIdentifier: number
  name: string
  value: number
}

export interface DbStructType {
  name: string
  packageRef: number
  id: number
}

export interface DbStructItemType {
  isArray: boolean
  fieldIdentifier: number
  isWritable: number
  maxLength: number
  minLength: number
  name: string
  structRef: number
  type: string
}

export interface DbUserType {
  creationTime: number
  id: number
  userKey: string
}

export interface DbSessionType {
  creationTime: number
  dirty: number
  id: number
  sessionKey: string
  userRef: number
}

export interface DbSessionKeyValueType {
  key: string
  sessionRef: number
  value: string
}

export interface DbSessionLogType {
  log: string
  sessionRef: number
  timestamp: string
}

export interface DbSessionPackageType {
  enabled: number
  packageRef: number
  required: number
  sessionRef: number
}

export interface DbEndpointTypeType {
  deviceTypeRef: number
  id: number
  name: string
  sessionRef: number
}

export interface DbEndpointType {
  deviceIdentifier: number
  deviceVersion: number
  id: number
  endpointIdentifier: number
  endpointTypeRef: number
  networkIdentifier: number
  profile: number
  sessionRef: number
}

export interface DbEndpointTypeClusterType {
  clusterRef: number
  enabled: number
  id: number
  endpointTypeRef: number
  side: string
}

export interface DbEndpointTypeAttributeType {
  attributeRef: number
  bounded: number
  defaultValue: string
  id: number
  endpointTypeClusterRef: number
  endpointTypeRef: number
  included: number
  includedReportable: number
  maxInterval: number
  minInterval: number
  reportableChange: number
  singleton: number
  storageOption: string
}

export interface DbEndpointTypeCommandType {
  commandRef: number
  endpointTypeClusterRef: number
  id: number
  endpointTypeRef: number
  incoming: number
  outgoing: number
}

export interface DbEndpointTypeEventType {
  endpointTypeClusterRef: number
  id: number
  endpointTypeRef: number
  eventRef: number
  included: number
}

export interface DbPackageExtensionValueType {
  entityCode: number
  packageExtensionRef: number
  id: number
  parentCode: number
  sessionRef: number
  value: string
}

export interface DbSettingType {
  category: string
  key: string
  value: string
}
