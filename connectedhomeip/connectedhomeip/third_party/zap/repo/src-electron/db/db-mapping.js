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
 * @module DB API: DB mappings between columns and JS object keys.
 */

const dbApi = require('./db-api.js')
const dbEnums = require('../../src-shared/db-enum.js')

exports.map = {
  package: (x) => {
    if (x == null) return undefined
    return {
      id: x.PACKAGE_ID,
      path: x.PATH,
      crc: x.CRC,
      type: x.TYPE,
      category: x.CATEGORY,
      description: x.DESCRIPTION,
      version: x.VERSION,
      parentId: x.PARENT_PACKAGE_REF,
    }
  },
  options: (x) => {
    if (x == null) return undefined
    return {
      id: x.OPTION_ID,
      packageRef: x.PACKAGE_REF,
      optionCategory: x.OPTION_CATEGORY,
      optionCode: x.OPTION_CODE,
      optionLabel: x.OPTION_LABEL,
    }
  },
  optionDefaults: (x) => {
    if (x == null) return undefined
    return {
      id: x.OPTION_DEFAULT_ID,
      packageRef: x.PACKAGE_REF,
      optionCategory: x.OPTION_CATEGORY,
      optionRef: x.OPTION_REF,
    }
  },
  trackedFile: (x) => {
    if (x == null) return undefined
    return {
      path: x.PATH,
      crc: x.CRC,
    }
  },
  cluster: (x) => {
    if (x == null) return undefined
    return {
      id: x.CLUSTER_ID,
      packageRef: x.PACKAGE_REF,
      code: x.CODE,
      manufacturerCode: x.MANUFACTURER_CODE,
      label: x.NAME,
      name: x.NAME,
      caption: x.DESCRIPTION,
      define: x.DEFINE,
      domainName: x.DOMAIN_NAME,
      isSingleton: dbApi.fromDbBool(x.IS_SINGLETON),
      revision: x.REVISION,
    }
  },

  attribute: (x) => {
    if (x == null) return undefined
    return {
      id: x.ATTRIBUTE_ID,
      clusterRef: x.CLUSTER_REF,
      packageRef: x.PACKAGE_REF,
      code: x.CODE,
      clusterCode: x.CLUSTER_CODE,
      manufacturerCode: x.MANUFACTURER_CODE,
      name: x.NAME,
      label: x.NAME,
      type: x.TYPE,
      side: x.SIDE,
      define: x.DEFINE,
      min: x.MIN,
      max: x.MAX,
      minLength: x.MIN_LENGTH,
      maxLength: x.MAX_LENGTH,
      reportMinInterval: x.REPORT_MIN_INTERVAL,
      reportMaxInterval: x.REPORT_MAX_INTERVAL,
      reportableChange: x.REPORTABLE_CHANGE,
      reportableChangeLength: x.REPORTABLE_CHANGE_LENGTH,
      isWritable: dbApi.fromDbBool(x.IS_WRITABLE),
      isNullable: dbApi.fromDbBool(x.IS_NULLABLE),
      defaultValue: x.DEFAULT_VALUE,
      isOptional: dbApi.fromDbBool(x.IS_OPTIONAL),
      isReportable:
        x.REPORTING_POLICY == dbEnums.reportingPolicy.mandatory ||
        x.REPORTING_POLICY == dbEnums.reportingPolicy.suggested,
      reportingPolicy: x.REPORTING_POLICY,
      isSceneRequired: dbApi.fromDbBool(x.IS_SCENE_REQUIRED),
      entryType: x.ARRAY_TYPE,
      mustUseTimedWrite: dbApi.fromDbBool(x.MUST_USE_TIMED_WRITE),
    }
  },

  eventField: (x) => {
    if (x == null) return undefined
    return {
      fieldIdentifier: x.FIELD_IDENTIFIER,
      name: x.NAME,
      type: x.TYPE,
      isArray: dbApi.fromDbBool(x.IS_ARRAY),
      isNullable: dbApi.fromDbBool(x.IS_NULLABLE),
      isOptional: dbApi.fromDbBool(x.IS_OPTIONAL),
    }
  },

  event: (x) => {
    if (x == null) return undefined
    return {
      id: x.EVENT_ID,
      clusterRef: x.CLUSTER_REF,
      clusterCode: x.CLUSTER_CODE,
      packageRef: x.PACKAGE_REF,
      code: x.CODE,
      manufacturerCode: x.MANUFACTURER_CODE,
      name: x.NAME,
      description: x.DESCRIPTION,
      side: x.SIDE,
      isOptional: dbApi.fromDbBool(x.IS_OPTIONAL),
      isFabricSensitive: dbApi.fromDbBool(x.IS_FABRIC_SENSITIVE),
      priority: x.PRIORITY,
    }
  },

  command: (x) => {
    if (x == null) return undefined
    return {
      id: x.COMMAND_ID,
      clusterRef: x.CLUSTER_REF,
      packageRef: x.PACKAGE_REF,
      code: x.CODE,
      manufacturerCode: x.MANUFACTURER_CODE,
      label: x.NAME,
      name: x.NAME,
      description: x.DESCRIPTION,
      source: x.SOURCE,
      isOptional: dbApi.fromDbBool(x.IS_OPTIONAL),
      mustUseTimedInvoke: dbApi.fromDbBool(x.MUST_USE_TIMED_INVOKE),
      isFabricScoped: dbApi.fromDbBool(x.IS_FABRIC_SCOPED),
      clusterCode: x.CLUSTER_CODE,
      clusterName: x.CLUSTER_NAME,
      clusterDefineName: x.CLUSTER_DEFINE_NAME,
      argName: x.ARG_NAME,
      argType: x.ARG_TYPE,
      argIsArray: dbApi.fromDbBool(x.ARG_IS_ARRAY),
      argPresentIf: x.ARG_PRESENT_IF,
      argCountArg: x.ARG_COUNT_ARG,
      commandArgCount: x.COMMAND_ARGUMENT_COUNT,
      argIsNullable: x.ARG_IS_NULLABLE,
      responseRef: x.RESPONSE_REF,
      responseName: x.RESPONSE_NAME,
      isIncoming: x.INCOMING,
      isOutgoing: x.OUTGOING,
      isDefaultResponseEnabled: x.IS_DEFAULT_RESPONSE_ENABLED,
    }
  },

  commandArgument: (x) => {
    if (x == null) return undefined
    return {
      commandRef: x.COMMAND_REF,
      fieldIdentifier: x.FIELD_IDENTIFIER,
      label: x.NAME,
      name: x.NAME,
      type: x.TYPE,
      min: x.MIN,
      max: x.MAX,
      minLength: x.MIN_LENGTH,
      maxLength: x.MAX_LENGTH,
      code: x.CODE,
      isArray: dbApi.fromDbBool(x.IS_ARRAY),
      presentIf: x.PRESENT_IF,
      isNullable: dbApi.fromDbBool(x.IS_NULLABLE),
      isOptional: dbApi.fromDbBool(x.IS_OPTIONAL),
      introducedInRef: x.INTRODUCED_IN_REF,
      removedInRef: x.REMOVED_IN_REF,
      countArg: x.COUNT_ARG,
      caption: `Command argument of type ${x.TYPE}`,
    }
  },

  domain: (x) => {
    if (x == null) return undefined
    return {
      id: x.DOMAIN_ID,
      label: x.NAME,
      caption: `Domain, named ${x.NAME}`,
    }
  },

  dataType: (x) => {
    if (x == null) return undefined
    return {
      id: x.DATA_TYPE_ID,
      name: x.NAME,
      description: x.DESCRIPTION,
      descriminatorId: x.DISCRIMINATOR_REF,
      packageId: x.PACKAGE_REF,
      discriminatorName: x.DISCRIMINATOR_NAME,
      clusterCode: x.CLUSTER_CODE,
    }
  },

  discriminator: (x) => {
    if (x == null) return undefined
    return {
      id: x.DISCRIMINATOR_ID,
      name: x.NAME,
    }
  },

  string: (x) => {
    if (x == null) return undefined
    return {
      id: x.STRING_ID,
      label: x.NAME,
      name: x.NAME,
      isLong: x.IS_LONG,
      isChar: x.IS_CHAR,
      size: x.SIZE,
    }
  },

  number: (x) => {
    if (x == null) return undefined
    return {
      id: x.NUMBER_ID,
      label: x.NAME,
      name: x.NAME,
      isSigned: x.IS_SIGNED,
      size: x.SIZE,
    }
  },

  enum: (x) => {
    if (x == null) return undefined
    return {
      id: x.ENUM_ID,
      label: x.NAME,
      name: x.NAME,
      caption: `Enum of size ${x.SIZE} byte`,
      size: x.SIZE,
    }
  },

  enumItem: (x) => {
    if (x == null) return undefined
    return {
      name: x.NAME,
      label: x.NAME,
      value: x.VALUE,
      enumRef: x.ENUM_REF,
      caption: `EnumItem, named ${x.NAME}`,
    }
  },

  struct: (x) => {
    if (x == null) return undefined
    return {
      id: x.STRUCT_ID,
      label: x.NAME,
      name: x.NAME,
      itemCnt: x.ITEM_COUNT,
      isFabricScoped: dbApi.fromDbBool(x.IS_FABRIC_SCOPED),
      caption: `Struct, named ${x.NAME}`,
    }
  },

  structItem: (x) => {
    if (x == null) return undefined
    return {
      name: x.NAME,
      label: x.NAME,
      fieldIdentifier: x.FIELD_IDENTIFIER,
      structRef: x.STRUCT_REF,
      type: x.TYPE,
      minLength: x.MIN_LENGTH,
      maxLength: x.MAX_LENGTH,
      isArray: dbApi.fromDbBool(x.IS_ARRAY),
      isEnum: dbApi.fromDbBool(x.IS_ENUM),
      isWritable: dbApi.fromDbBool(x.IS_WRITABLE),
      isNullable: dbApi.fromDbBool(x.IS_NULLABLE),
      isOptional: dbApi.fromDbBool(x.IS_OPTIONAL),
      isFabricSensitive: dbApi.fromDbBool(x.IS_FABRIC_SENSITIVE),
      dataTypeReference: x.TYPE,
      dataTypeReferenceName: x.DATA_TYPE_REF_NAME,
      discriminatorName: x.DISCRIMINATOR_NAME,
    }
  },

  atomic: (x) => {
    if (x == null) return undefined
    return {
      id: x.ATOMIC_ID,
      atomicId: x.ATOMIC_IDENTIFIER,
      name: x.NAME,
      description: x.DESCRIPTION,
      size: x.ATOMIC_SIZE,
      isDiscrete: dbApi.fromDbBool(x.IS_DISCRETE),
      isString: dbApi.fromDbBool(x.IS_STRING),
      isLong: dbApi.fromDbBool(x.IS_LONG),
      isChar: dbApi.fromDbBool(x.IS_CHAR),
      isSigned: dbApi.fromDbBool(x.IS_SIGNED),
    }
  },

  bitmap: (x) => {
    if (x == null) return undefined
    return {
      id: x.BITMAP_ID,
      label: x.NAME,
      name: x.NAME,
      type: x.TYPE,
      size: x.SIZE,
    }
  },

  bitmapField: (x) => {
    if (x == null) return undefined
    return {
      label: x.NAME,
      mask: x.MASK,
      type: x.TYPE,
      bitmapRef: x.BITMAP_REF,
      caption: `BitmapField, named ${x.NAME}`,
    }
  },

  deviceType: (x) => {
    if (x == null) return undefined
    return {
      id: x.DEVICE_TYPE_ID,
      code: x.CODE,
      profileId: x.PROFILE_ID,
      domain: x.DOMAIN,
      label: x.NAME,
      caption: x.DESCRIPTION,
    }
  },

  deviceTypeCluster: (x) => {
    if (x == null) return undefined
    return {
      id: x.DEVICE_TYPE_CLUSTER_ID,
      deviceTypeRef: x.DEVICE_TYPE_REF,
      clusterRef: x.CLUSTER_REF,
      clusterName: x.CLUSTER_NAME,
      includeClient: dbApi.fromDbBool(x.INCLUDE_CLIENT),
      includeServer: dbApi.fromDbBool(x.INCLUDE_SERVER),
      lockClient: dbApi.fromDbBool(x.LOCK_CLIENT),
      lockServer: dbApi.fromDbBool(x.LOCK_SERVER),
    }
  },

  deviceTypeAttribute: (x) => {
    if (x == null) return undefined
    return {
      deviceTypeClusterRef: x.DEVICE_TYPE_CLUSTER_REF,
      attributeRef: x.ATTRIBUTE_REF,
      name: x.NAME,
      code: x.CODE,
      manufacturerCode: x.MANUFACTURER_CODE,
    }
  },

  deviceTypeCommand: (x) => {
    if (x == null) return undefined
    return {
      deviceTypeClusterRef: x.DEVICE_TYPE_CLUSTER_REF,
      commandRef: x.COMMAND_REF,
      name: x.NAME,
      code: x.CODE,
      manufacturerCode: x.MANUFACTURER_CODE,
    }
  },

  endpoint: (x) => {
    if (x == null) return undefined
    return {
      id: x.ENDPOINT_ID,
      endpointRef: x.ENDPOINT_ID,
      sessionRef: x.SESSION_REF,
      endpointIdentifier: x.ENDPOINT_IDENTIFIER,
      endpointId: x.ENDPOINT_IDENTIFIER,
      endpointTypeRef: x.ENDPOINT_TYPE_REF,
      profileId: x.PROFILE,
      networkId: x.NETWORK_IDENTIFIER,
      endpointVersion: x.DEVICE_VERSION,
      deviceIdentifier: x.DEVICE_IDENTIFIER,
    }
  },
  endpointType: (x) => {
    if (x == null) return undefined
    return {
      id: x.ENDPOINT_TYPE_ID,
      endpointTypeId: x.ENDPOINT_TYPE_ID,
      sessionRef: x.SESSION_REF,
      name: x.NAME,
      deviceTypeRef: x.DEVICE_TYPE_REF,
    }
  },
  endpointTypeCluster: (x) => {
    if (x == null) return undefined
    return {
      endpointTypeClusterId: x.ENDPOINT_TYPE_CLUSTER_ID,
      endpointTypeRef: x.ENDPOINT_TYPE_REF,
      clusterRef: x.CLUSTER_REF,
      side: x.SIDE,
      enabled: dbApi.fromDbBool(x.ENABLED),
    }
  },

  endpointTypeAttribute: (x) => {
    if (x == null) return undefined
    return {
      endpointTypeRef: x.ENDPOINT_TYPE_REF,
      clusterRef: x.CLUSTER_REF,
      attributeRef: x.ATTRIBUTE_REF,
      included: dbApi.fromDbBool(x.INCLUDED),
      storageOption: x.STORAGE_OPTION,
      singleton: dbApi.fromDbBool(x.SINGLETON),
      bounded: dbApi.fromDbBool(x.BOUNDED),
      defaultValue: x.DEFAULT_VALUE,
      includedReportable: dbApi.fromDbBool(x.INCLUDED_REPORTABLE),
      minInterval: x.MIN_INTERVAL,
      maxInterval: x.MAX_INTERVAL,
      reportableChange: x.REPORTABLE_CHANGE,
    }
  },

  endpointTypeCommand: (x) => {
    if (x == null) return undefined
    return {
      endpointTypeRef: x.ENDPOINT_TYPE_REF,
      clusterRef: x.CLUSTER_REF,
      commandRef: x.COMMAND_REF,
      incoming: dbApi.fromDbBool(x.INCOMING),
      outgoing: dbApi.fromDbBool(x.OUTGOING),
    }
  },

  endpointTypeEvent: (x) => {
    if (x == null) return undefined
    return {
      endpointTypeRef: x.ENDPOINT_TYPE_REF,
      clusterRef: x.CLUSTER_REF,
      eventRef: x.EVENT_REF,
      included: dbApi.fromDbBool(x.INCLUDED),
    }
  },

  packageExtension: (x) => {
    if (x == null) return undefined
    return {
      entity: x.ENTITY,
      property: x.PROPERTY,
      type: x.TYPE,
      configurability: x.CONFIGURABILITY,
      label: x.LABEL,
      globalDefault: x.GLOBAL_DEFAULT,
    }
  },
  packageExtensionDefault: (x) => {
    if (x == null) return undefined
    return {
      entityCode: x.ENTITY_CODE,
      entityQualifier: x.ENTITY_QUALIFIER,
      parentCode: x.PARENT_CODE,
      manufacturerCode: x.MANUFACTURER_CODE,
      value: x.VALUE,
    }
  },
  sessionPackage: (x) => {
    if (x == null) return undefined
    return {
      packageRef: x.PACKAGE_REF,
      sessionRef: x.SESSION_REF,
      required: x.REQUIRED,
    }
  },
  sessionLog: (x) => {
    if (x == null) return undefined
    return {
      timestamp: x.TIMESTAMP,
      log: x.LOG,
    }
  },
  session: (x) => {
    if (x == null) return undefined
    return {
      sessionId: x.SESSION_ID,
      sessionKey: x.SESSION_KEY,
      creationTime: x.CREATION_TIME,
      dirty: x.DIRTY == 1,
    }
  },
  user: (x) => {
    if (x == null) return undefined
    return {
      userId: x.USER_ID,
      userKey: x.USER_KEY,
      creationTime: x.CREATION_TIME,
    }
  },
  accessRole: (x) => {
    if (x == null) return undefined
    return {
      name: x.NAME,
      description: x.DESCRIPTION,
      level: x.LEVEL,
    }
  },
  accessOperation: (x) => {
    if (x == null) return undefined
    return {
      name: x.NAME,
      description: x.DESCRIPTION,
    }
  },
  accessModifier: (x) => {
    if (x == null) return undefined
    return {
      name: x.NAME,
      description: x.DESCRIPTION,
    }
  },
  access: (x) => {
    if (x == null) return undefined
    return {
      operation: x.OP_NAME,
      role: x.ROLE_NAME,
      accessModifier: x.MODIFIER_NAME,
    }
  },
  settings: (x) => {
    if (x == null) return undefined
    return {
      category: x.CATEGORY,
      key: x.KEY,
      value: x.VALUE,
    }
  },
}

exports.reverseMap = {
  endpoint: {
    endpointRef: 'ENDPOINT_ID',
    sessionRef: 'SESSION_REF',
    endpointId: 'ENDPOINT_IDENTIFIER',
    endpointTypeRef: 'ENDPOINT_TYPE_REF',
    profileId: 'PROFILE',
    networkId: 'NETWORK_IDENTIFIER',
    endpointVersion: 'DEVICE_VERSION',
  },
}
