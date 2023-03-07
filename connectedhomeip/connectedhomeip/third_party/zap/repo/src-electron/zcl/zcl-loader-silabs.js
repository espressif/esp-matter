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

const fs = require('fs')
const fsp = fs.promises
const path = require('path')
const properties = require('properties')
const dbApi = require('../db/db-api')
const queryPackage = require('../db/query-package')
const queryDeviceType = require('../db/query-device-type')
const queryLoader = require('../db/query-loader')
const queryZcl = require('../db/query-zcl')
const env = require('../util/env')
const bin = require('../util/bin')
const util = require('../util/util')
const dbEnum = require('../../src-shared/db-enum')
const zclLoader = require('./zcl-loader')
const _ = require('lodash')

/**
 * Promises to read the JSON file and resolve all the data.
 * @param {*} ctx  Context containing information about the file
 * @returns Promise of resolved file.
 */
async function collectDataFromJsonFile(metadataFile, data) {
  env.logDebug(`Collecting ZCL files from JSON file: ${metadataFile}`)
  let obj = JSON.parse(data)
  let f
  let returnObject = {}

  let fileLocations
  if (Array.isArray(obj.xmlRoot)) {
    fileLocations = obj.xmlRoot.map((p) =>
      path.join(path.dirname(metadataFile), p)
    )
  } else {
    fileLocations = [path.join(path.dirname(metadataFile), obj.xmlRoot)]
  }
  let zclFiles = []
  obj.xmlFile.forEach((xmlF) => {
    f = util.locateRelativeFilePath(fileLocations, xmlF)
    if (f != null) zclFiles.push(f)
  })

  returnObject.zclFiles = zclFiles

  // Manufacturers XML file.
  f = util.locateRelativeFilePath(fileLocations, obj.manufacturersXml)
  if (f != null) returnObject.manufacturersXml = f

  // Profiles XML File
  f = util.locateRelativeFilePath(fileLocations, obj.profilesXml)
  if (f != null) returnObject.profilesXml = f

  // Zcl XSD file
  f = util.locateRelativeFilePath(fileLocations, obj.zclSchema)
  if (f != null) returnObject.zclSchema = f

  // Zcl Validation Script
  f = util.locateRelativeFilePath(fileLocations, obj.zclValidation)
  if (f != null) returnObject.zclValidation = f

  // General options
  // Note that these values when put into OPTION_CODE will generally be converted to lowercase.
  if (obj.options) {
    returnObject.options = obj.options
  }
  // Defaults. Note that the keys should be the categories that are listed for PACKAGE_OPTION, and the value should be the OPTION_CODE
  if (obj.defaults) {
    returnObject.defaults = obj.defaults
  }

  // Feature Flags
  if (obj.featureFlags) {
    returnObject.featureFlags = obj.featureFlags
  }

  if (obj.uiOptions) {
    returnObject.uiOptions = obj.uiOptions
  }
  // Default reportability.
  // `defaultReportable` was old thing that could be true or false.
  // We still honor it.
  returnObject.defaultReportingPolicy =
    dbEnum.reportingPolicy.defaultReportingPolicy

  if ('defaultReportable' in obj) {
    returnObject.defaultReportingPolicy = obj.defaultReportable
      ? dbEnum.reportingPolicy.suggested
      : dbEnum.reportingPolicy.optional
  }
  // Default reporting policy is the new thing that can be mandatory/optional/suggested/prohibited.
  // If it's missing, 'optional' is a default reporting policy.
  if ('defaultReportingPolicy' in obj) {
    returnObject.defaultReportingPolicy = dbEnum.reportingPolicy.resolve(
      obj.defaultReportingPolicy
    )
  }
  returnObject.version = obj.version
  returnObject.category = obj.category
  returnObject.description = obj.description
  returnObject.supportCustomZclDevice = obj.supportCustomZclDevice

  if ('listsUseAttributeAccessInterface' in obj) {
    returnObject.listsUseAttributeAccessInterface =
      obj.listsUseAttributeAccessInterface
  }

  if ('attributeAccessInterfaceAttributes' in obj) {
    returnObject.attributeAccessInterfaceAttributes =
      obj.attributeAccessInterfaceAttributes
  }

  if ('ZCLDataTypes' in obj) {
    returnObject.ZCLDataTypes = obj.ZCLDataTypes
  } else {
    returnObject.ZCLDataTypes = [
      'ARRAY',
      'BITMAP',
      'ENUM',
      'NUMBER',
      'STRING',
      'STRUCT',
    ]
  }

  // zcl.json can contain 'fabricHandling' toplevel key. It is expected
  // to look like this:
  //  "fabricHandling": {
  //    "automaticallyCreateFields": true,
  //    "indexFieldId": 254,
  //    "indexFieldName": "FabricIndex",
  //    "indexType": "fabric_idx"
  // },
  //
  // If this configuration is present, then special logic to automatically
  // add fabric index field to fabric-sensitive and fabric-scoped
  // things will kick in.
  //
  // If this field is not present then the special logic will not
  // happen.
  if ('fabricHandling' in obj) {
    returnObject.fabricHandling = obj.fabricHandling
  } else {
    returnObject.fabricHandling = {
      automaticallyCreateFields: false,
    }
  }
  env.logDebug(
    `Resolving: ${returnObject.zclFiles}, version: ${returnObject.version}`
  )

  return returnObject
}

/**
 * Promises to read the properties file, extract all the actual xml files, and resolve with the array of files.
 *
 * @param {*} ctx Context which contains information about the propertiesFiles and data
 * @returns Promise of resolved files.
 */
async function collectDataFromPropertiesFile(metadataFile, data) {
  return new Promise((resolve, reject) => {
    env.logDebug(`Collecting ZCL files from properties file: ${metadataFile}`)

    let returnObject = {}

    properties.parse(data, { namespaces: true }, (err, zclProps) => {
      if (err) {
        env.logError(`Could not read file: ${metadataFile}`)
        reject(err)
      } else {
        let fileLocations = zclProps.xmlRoot
          .split(',')
          .map((p) => path.join(path.dirname(metadataFile), p))
        let zclFiles = []
        let f

        // Iterate over all XML files in the properties file, and check
        // if they exist in one or the other directory listed in xmlRoot
        zclProps.xmlFile.split(',').forEach((singleXmlFile) => {
          let fullPath = util.locateRelativeFilePath(
            fileLocations,
            singleXmlFile
          )
          if (fullPath != null) zclFiles.push(fullPath)
        })

        returnObject.zclFiles = zclFiles
        // Manufacturers XML file.
        f = util.locateRelativeFilePath(
          fileLocations,
          zclProps.manufacturersXml
        )
        if (f != null) returnObject.manufacturersXml = f

        // Profiles XML file.
        f = util.locateRelativeFilePath(fileLocations, zclProps.profilesXml)
        if (f != null) returnObject.profilesXml = f

        // Zcl XSD file
        f = util.locateRelativeFilePath(fileLocations, zclProps.zclSchema)
        if (f != null) returnObject.zclSchema = f

        // Zcl Validation Script
        f = util.locateRelativeFilePath(fileLocations, zclProps.zclValidation)
        if (f != null) returnObject.zclValidation = f

        // General options
        // Note that these values when put into OPTION_CODE will generally be converted to lowercase.
        if (zclProps.options) {
          returnObject.options = zclProps.options
        }
        // Defaults. Note that the keys should be the categories that are listed for PACKAGE_OPTION, and the value should be the OPTION_CODE
        if (zclProps.defaults) {
          returnObject.defaults = zclProps.defaults
        }

        // Feature Flags
        if (zclProps.featureFlags) {
          returnObject.featureFlags = zclProps.featureFlags
        }

        returnObject.supportCustomZclDevice = zclProps.supportCustomZclDevice
        returnObject.version = zclProps.version
        returnObject.description = zclProps.description
        returnObject.category = zclProps.category
        // Don't bother with allowing this in the properties file.
        // It's legacy only.
        returnObject.fabricHandling = {
          automaticallyCreateFields: false,
        }
        env.logDebug(
          `Resolving: ${returnObject.zclFiles}, version: ${returnObject.version}`
        )
        resolve(returnObject)
      }
    })
  })
}

/**
 * Silabs XML does not carry types with bitmap fields, but dotdot does, so they are in the schema.
 * Just to put some data in, we differentiate between "bool" and "enum" types here.
 *
 * @param {*} mask
 * @returns bool or corresponding enum
 */
function maskToType(mask) {
  let n = parseInt(mask)
  let bitCount = bin.bitCount(n)
  if (bitCount <= 1) {
    return 'bool'
  } else if (bitCount <= 8) {
    return 'enum8'
  } else if (bitCount <= 16) {
    return 'enum16'
  } else {
    return 'enum32'
  }
}

/**
 * Prepare atomic to db insertion.
 *
 * @param {*} a
 */
function prepareAtomic(a) {
  return {
    name: a.$.name,
    id: parseInt(a.$.id),
    size: a.$.size,
    description: a.$.description,
    isDiscrete: a.$.discrete == 'true',
    isSigned: a.$.signed == 'true',
    isString: a.$.string == 'true',
    isLong: a.$.long == 'true',
    isChar: a.$.char == 'true',
  }
}
/**
 * Processes atomic types for DB insertion.
 *
 * @param {*} db
 * @param {*} filePath
 * @param {*} packageId
 * @param {*} data
 * @returns Promise of inserted bitmaps
 */
async function processAtomics(db, filePath, packageId, data) {
  let types = data[0].type
  env.logDebug(`${filePath}, ${packageId}: ${types.length} atomic types.`)
  return queryLoader.insertAtomics(
    db,
    packageId,
    types.map((x) => prepareAtomic(x))
  )
}

/**
 * Prepares global attribute data.
 *
 * @param {*} cluster
 * @returns Object containing the data from XML.
 */
function prepareClusterGlobalAttribute(cluster) {
  if ('globalAttribute' in cluster) {
    let ret = {}

    ret.code = parseInt(cluster.code[0], 16)
    if ('$' in cluster) {
      let mfgCode = cluster['$'].manufacturerCode
      if (mfgCode != null) ret.manufacturerCode = mfgCode
    }

    ret.globalAttribute = []
    cluster.globalAttribute.forEach((ga) => {
      let at = {
        code: parseInt(ga.$.code),
        value: ga.$.value,
      }

      if ('featureBit' in ga) {
        at.featureBit = ga.featureBit.map((fb) => {
          let content = fb._ != null ? fb._.toLowerCase() : null
          return {
            tag: fb.$.tag,
            bit: parseInt(fb.$.bit),
            value: content == '1' || content == 'true',
          }
        })
      }

      if (ga.$.side == dbEnum.side.either) {
        ret.globalAttribute.push(
          Object.assign({ side: dbEnum.side.client }, at)
        )
        ret.globalAttribute.push(
          Object.assign({ side: dbEnum.side.server }, at)
        )
      } else {
        ret.globalAttribute.push(Object.assign({ side: ga.$.side }, at))
      }
    })
    return ret
  } else {
    return null
  }
}

function extractAccessTag(ac) {
  let e = {
    op: ac.$.op,
    role: ac.$.role,
    modifier: ac.$.modifier,
  }
  if ('privilege' in ac.$) {
    e.role = ac.$.privilege
  }
  return e
}

function extractAccessIntoArray(xmlElement) {
  let accessArray = []
  if ('access' in xmlElement) {
    for (const ac of xmlElement.access) {
      accessArray.push(extractAccessTag(ac))
    }
  }
  return accessArray
}

/**
 * Prepare XML cluster for insertion into the database.
 * This method can also prepare clusterExtensions.
 *
 * @param {*} cluster
 * @returns Object containing all data from XML.
 */
function prepareCluster(cluster, context, isExtension = false) {
  let ret = {
    isExtension: isExtension,
  }

  if (isExtension) {
    if ('$' in cluster && 'code' in cluster.$) {
      ret.code = parseInt(cluster.$.code)
    }
  } else {
    ret.code = parseInt(cluster.code[0])
    ret.name = cluster.name[0]
    ret.description = cluster.description[0].trim()
    ret.define = cluster.define[0]
    ret.domain = cluster.domain[0]
    ret.isSingleton = false
    if ('$' in cluster) {
      if (cluster.$.manufacturerCode == null) {
        ret.manufacturerCode = null
      } else {
        ret.manufacturerCode = parseInt(cluster.$.manufacturerCode)
      }
      if (cluster.$.singleton == 'true') {
        ret.isSingleton = true
      }
      ret.introducedIn = cluster.$.introducedIn
      ret.removedIn = cluster.$.removedIn
    }
  }

  if ('tag' in cluster) {
    ret.tags = cluster.tag.map((tag) => prepareTag(tag))
  }

  if ('command' in cluster) {
    ret.commands = []
    cluster.command.forEach((command) => {
      let cmd = {
        code: parseInt(command.$.code),
        manufacturerCode: command.$.manufacturerCode,
        name: command.$.name,
        description: command.description[0].trim(),
        source: command.$.source,
        isOptional: command.$.optional == 'true' ? true : false,
        mustUseTimedInvoke: command.$.mustUseTimedInvoke == 'true',
        introducedIn: command.$.introducedIn,
        removedIn: command.$.removedIn,
        responseName: command.$.response == null ? null : command.$.response,
        isDefaultResponseEnabled:
          command.$.disableDefaultResponse == 'true' ? false : true,
        isFabricScoped: command.$.isFabricScoped == 'true',
      }
      cmd.access = extractAccessIntoArray(command)
      if (cmd.manufacturerCode == null) {
        cmd.manufacturerCode = ret.manufacturerCode
      } else {
        cmd.manufacturerCode = parseInt(cmd.manufacturerCode)
      }
      if ('arg' in command) {
        cmd.args = []
        let lastFieldId = -1
        command.arg.forEach((arg) => {
          let defaultFieldId = lastFieldId + 1
          lastFieldId = arg.$.fieldId ? parseInt(arg.$.fieldId) : defaultFieldId
          // We are only including ones that are NOT removedIn
          if (arg.$.removedIn == null)
            cmd.args.push({
              name: arg.$.name,
              type: arg.$.type,
              min: arg.$.min,
              max: arg.$.max,
              minLength: 0,
              maxLength: arg.$.length ? arg.$.length : null,
              isArray: arg.$.array == 'true' ? 1 : 0,
              presentIf: arg.$.presentIf,
              isNullable: arg.$.isNullable == 'true' ? true : false,
              isOptional: arg.$.optional == 'true' ? true : false,
              countArg: arg.$.countArg,
              fieldIdentifier: lastFieldId,
              introducedIn: arg.$.introducedIn,
              removedIn: arg.$.removedIn,
            })
        })
      }
      ret.commands.push(cmd)
    })
  }
  if ('event' in cluster) {
    ret.events = []
    cluster.event.forEach((event) => {
      let ev = {
        code: parseInt(event.$.code),
        manufacturerCode: event.$.manufacturerCode,
        name: event.$.name,
        side: event.$.side,
        priority: event.$.priority,
        description: event.description[0].trim(),
        isOptional: event.$.optional == 'true',
        isFabricSensitive: event.$.isFabricSensitive == 'true',
      }
      ev.access = extractAccessIntoArray(event)
      if (ev.manufacturerCode == null) {
        ev.manufacturerCode = ret.manufacturerCode
      } else {
        ev.manufacturerCode = parseInt(ev.manufacturerCode)
      }
      if ('field' in event) {
        ev.fields = []
        let lastFieldId = -1
        event.field.forEach((field) => {
          let defaultFieldId = lastFieldId + 1
          lastFieldId = field.$.id ? parseInt(field.$.id) : defaultFieldId
          if (field.$.removedIn == null) {
            ev.fields.push({
              name: field.$.name,
              type: field.$.type,
              isArray: field.$.array == 'true' ? 1 : 0,
              isNullable: field.$.isNullable == 'true' ? true : false,
              isOptional: field.$.optional == 'true' ? true : false,
              fieldIdentifier: lastFieldId,
              introducedIn: field.$.introducedIn,
              removedIn: field.$.removedIn,
            })
          }
        })
      }
      if (
        context.fabricHandling &&
        context.fabricHandling.automaticallyCreateFields &&
        ev.isFabricSensitive
      ) {
        if (!ev.fields) {
          ev.fields = []
        }
        ev.fields.push({
          name: context.fabricHandling.indexFieldName,
          type: context.fabricHandling.indexType,
          isArray: false,
          isNullable: false,
          isOptional: false,
          fieldIdentifier: context.fabricHandling.indexFieldId,
          introducedIn: null,
          removedIn: null,
        })
      }

      // We only add event if it does not have removedIn
      if (ev.removedIn == null) ret.events.push(ev)
    })
  }

  if ('attribute' in cluster) {
    ret.attributes = []
    cluster.attribute.forEach((attribute) => {
      let name = attribute._
      if ('description' in attribute && name == null) {
        name = attribute.description.join('')
      }
      let reportingPolicy = context.defaultReportingPolicy
      if (attribute.$.reportable == 'true') {
        reportingPolicy = dbEnum.reportingPolicy.suggested
      } else if (attribute.$.reportable == 'false') {
        reportingPolicy = dbEnum.reportingPolicy.optional
      } else if (attribute.$.reportingPolicy != null) {
        reportingPolicy = dbEnum.reportingPolicy.resolve(
          attribute.$.reportingPolicy
        )
      }
      let storagePolicy = dbEnum.storagePolicy.any
      if (context.listsUseAttributeAccessInterface && attribute.$.entryType) {
        storagePolicy = dbEnum.storagePolicy.attributeAccessInterface
      } else if (
        context.attributeAccessInterfaceAttributes &&
        context.attributeAccessInterfaceAttributes[cluster.name] &&
        context.attributeAccessInterfaceAttributes[cluster.name].includes(name)
      ) {
        storagePolicy = dbEnum.storagePolicy.attributeAccessInterface
      }
      let att = {
        code: parseInt(attribute.$.code),
        manufacturerCode: attribute.$.manufacturerCode,
        name: name,
        type:
          attribute.$.type.toUpperCase() == attribute.$.type
            ? attribute.$.type.toLowerCase()
            : attribute.$.type,
        side: attribute.$.side,
        define: attribute.$.define,
        min: attribute.$.min,
        max: attribute.$.max,
        minLength: 0,
        maxLength: attribute.$.length ? attribute.$.length : null,
        reportMinInterval: attribute.$.reportMinInterval,
        reportMaxInterval: attribute.$.reportMaxInterval,
        reportableChange: attribute.$.reportableChange,
        reportableChangeLength: attribute.$.reportableChangeLength
          ? attribute.$.reportableChangeLength
          : null,
        isWritable: attribute.$.writable == 'true',
        defaultValue: attribute.$.default,
        isOptional: attribute.$.optional == 'true',
        reportingPolicy: reportingPolicy,
        storagePolicy: storagePolicy,
        isSceneRequired: attribute.$.sceneRequired == 'true',
        introducedIn: attribute.$.introducedIn,
        removedIn: attribute.$.removedIn,
        isNullable: attribute.$.isNullable == 'true' ? true : false,
        entryType: attribute.$.entryType,
        mustUseTimedWrite: attribute.$.mustUseTimedWrite == 'true',
      }
      att.access = extractAccessIntoArray(attribute)
      if (att.manufacturerCode == null) {
        att.manufacturerCode = ret.manufacturerCode
      } else {
        att.manufacturerCode = parseInt(att.manufacturerCode)
      }
      // Setting max length for string type attributes when not specified by
      // the xml.
      if (
        att.type &&
        (att.type.toLowerCase() == 'long_octet_string' ||
          att.type.toLowerCase() == 'long_char_string') &&
        (att.maxLength == 0 || !att.maxLength)
      ) {
        if (context.category == 'zigbee') {
          // Setting the max length for long strings to 253 instead of 65534
          // if not already set by xml.
          env.logWarning(
            'Long string max length not set for ' +
              att.name +
              ' in xml. \
          Currently defaulting to a max length of 253 for long strings instead of 65534 \
          for space conservation and no support available for long strings in zigbee pro.'
          )
          att.maxLength = 253
        } else {
          // Setting the max length for long strings to 1024 instead of 65534
          // if not already set by xml.
          env.logWarning(
            'Long string max length not set for ' +
              att.name +
              ' in xml. \
          Currently defaulting to a max length of 1024 for long strings instead of 65534 \
          for space conservation.'
          )
          att.maxLength = 1024
        }
      }
      if (
        att.type &&
        (att.type.toLowerCase() == 'octet_string' ||
          att.type.toLowerCase() == 'char_string') &&
        (att.maxLength == 0 || !att.maxLength)
      ) {
        att.maxLength = 254
      }
      // If attribute has removedIn, then it's not valid any more in LATEST spec.
      if (att.removedIn == null) ret.attributes.push(att)
    })
  }

  return ret
}

/**
 * Process clusters for insertion into the database.
 *
 * @param {*} db
 * @param {*} filePath
 * @param {*} packageId
 * @param {*} data
 * @returns Promise of cluster insertion.
 */
async function processClusters(db, filePath, packageId, data, context) {
  env.logDebug(`${filePath}, ${packageId}: ${data.length} clusters.`)
  return queryLoader.insertClusters(
    db,
    packageId,
    data.map((x) => prepareCluster(x, context))
  )
}

/**
 * Processes global attributes for insertion into the database.
 *
 * @param {*} db
 * @param {*} filePath
 * @param {*} packageId
 * @param {*} data
 * @returns Promise of inserted data.
 */
function processClusterGlobalAttributes(db, filePath, packageId, data) {
  let objs = []
  data.forEach((x) => {
    let p = prepareClusterGlobalAttribute(x)
    if (p != null) objs.push(p)
  })
  if (objs.length > 0) {
    return queryLoader.insertGlobalAttributeDefault(db, packageId, objs)
  } else {
    return null
  }
}

/**
 * Cluster Extension contains attributes and commands in a same way as regular cluster,
 * and it has an attribute code="0xXYZ" where code is a cluster code.
 *
 * @param {*} db
 * @param {*} filePath
 * @param {*} packageId
 * @param {*} data
 * @returns promise to resolve the clusterExtension tags
 */
async function processClusterExtensions(
  db,
  filePath,
  dataPackageId,
  knownPackages,
  data,
  context
) {
  env.logDebug(
    `${filePath}, ${dataPackageId}: ${data.length} cluster extensions.`
  )
  return queryLoader.insertClusterExtensions(
    db,
    dataPackageId,
    knownPackages,
    data.map((x) => prepareCluster(x, context, true))
  )
}

/**
 * Processes the globals in the XML files. The `global` tag contains
 * attributes and commands in a same way as cluster or clusterExtension
 *
 * @param {*} db
 * @param {*} filePath
 * @param {*} packageId
 * @param {*} data
 * @returns promise to resolve the globals
 */
async function processGlobals(db, filePath, packageId, data, context) {
  env.logDebug(`${filePath}, ${packageId}: ${data.length} globals.`)
  return queryLoader.insertGlobals(
    db,
    packageId,
    data.map((x) => prepareCluster(x, context, true))
  )
}

function prepareTag(tag) {
  return {
    name: tag.$.name,
    description: tag.$.description,
  }
}

/**
 * Process defaultAccess tag in the XML.
 * @param {*} db
 * @param {*} filePath
 * @param {*} packageId
 * @param {*} defaultAccessList
 */
async function processDefaultAccess(
  db,
  filePath,
  packageId,
  defaultAccessList
) {
  let p = []
  for (const da of defaultAccessList) {
    let type = {
      type: da.$.type,
      access: [],
    }
    for (const ac of da.access) {
      type.access.push(extractAccessTag(ac))
    }
    p.push(queryLoader.insertDefaultAccess(db, packageId, type))
  }
  return Promise.all(p)
}

/**
 * Process accessControl tag in the XML.
 * @param {*} db
 * @param {*} filePath
 * @param {*} packageId
 * @param {*} accessControlList
 */
async function processAccessControl(
  db,
  filePath,
  packageId,
  accessControlList
) {
  let operations = []
  let roles = []
  let accessModifiers = []

  for (const ac of accessControlList) {
    if ('operation' in ac) {
      for (const op of ac.operation) {
        operations.push({
          name: op.$.type,
          description: op.$.description,
        })
      }
    }
    if ('role' in ac) {
      for (const role of ac.role) {
        roles.push({
          name: role.$.type,
          description: role.$.description,
          level: roles.length,
        })
      }
    }
    if ('privilege' in ac) {
      for (const role of ac.privilege) {
        roles.push({
          name: role.$.type,
          description: role.$.description,
          level: roles.length,
        })
      }
    }
    if ('modifier' in ac) {
      for (const modifier of ac.modifier) {
        accessModifiers.push({
          name: modifier.$.type,
          description: modifier.$.description,
        })
      }
    }
  }

  let p = []
  p.push(queryLoader.insertAccessRoles(db, packageId, roles))
  p.push(queryLoader.insertAccessOperations(db, packageId, operations))
  p.push(queryLoader.insertAccessModifiers(db, packageId, accessModifiers))
  return Promise.all(p)
}

/**
 * Processes the tags in the XML.
 * @param {*} db
 * @param {*} filePath
 * @param {*} packageId
 * @param {*} tags
 */
async function processTags(db, filePath, packageId, tags) {
  // <tag name="AB" description="Description"/>
  env.logDebug(`${filePath}, ${packageId}: ${tags.length} tags.`)
  let preparedTags = tags.map((x) => prepareTag(x))
  return queryLoader.insertTags(db, packageId, preparedTags, null)
}

/**
 * Convert domain from XMl to domain for DB.
 *
 * @param {*} domain
 * @returns Domain object for DB.
 */
function prepareDomain(domain) {
  let d = {
    name: domain.$.name,
    specCode: domain.$.spec,
    specDescription: `Latest ${domain.$.name} spec: ${domain.$.spec}`,
    specCertifiable: domain.$.certifiable == 'true',
  }
  if ('older' in domain) {
    d.older = domain.older.map((old) => {
      return {
        specCode: old.$.spec,
        specDescription: `Older ${domain.$.name} spec ${old.$.spec}`,
        specCertifiable: old.$.certifiable == 'true',
      }
    })
  }
  return d
}

/**
 * Process domains for insertion.
 *
 * @param {*} db
 * @param {*} filePath
 * @param {*} packageId
 * @param {*} data
 * @returns Promise of database insertion of domains.
 */
async function processDomains(db, filePath, packageId, data) {
  // <domain name="ZLL" spec="zll-1.0-11-0037-10" dependsOn="zcl-1.0-07-5123-03">
  //    <older ....
  // </domain>
  env.logDebug(`${filePath}, ${packageId}: ${data.length} domains.`)
  let preparedDomains = data.map((x) => prepareDomain(x))
  let preparedSpecs = preparedDomains.filter((d) => d.specCode != null)
  let specIds = await queryLoader.insertSpecs(db, packageId, preparedSpecs)
  for (let i = 0; i < specIds.length; i++) {
    preparedDomains[i].specRef = specIds[i]
  }
  return queryLoader.insertDomains(db, packageId, preparedDomains)
}

/**
 * Prepare Data Type Discriminator for database table insertion.
 *
 * @param {*} a
 * @returns An Object
 */
function prepareDataTypeDiscriminator(a) {
  return {
    name: a.name,
    id: a.id,
  }
}

/**
 * Processes Data Type Discriminator.
 *
 * @param {*} db
 * @param {*} filePath
 * @param {*} zclDataTypes
 * @returns Promise of inserted Data Type Discriminators.
 */
async function processDataTypeDiscriminator(db, packageId, zclDataTypes) {
  // Loading the Data Types using ZCLDataTypes mentioned in zcl.json metadata
  // file
  let types = zclDataTypes.map((x, index) => {
    return { id: index + 1, name: x }
  })
  env.logDebug(`${packageId}: ${types.length} Data Type Discriminator.`)
  return queryLoader.insertDataTypeDiscriminator(
    db,
    packageId,
    types.map((x) => prepareDataTypeDiscriminator(x))
  )
}

/**
 * Prepare Data Types for database table insertion.
 *
 * @param {*} a
 * @param {*} dataType
 * @param {*} typeMap
 * @returns An Object
 */
function prepareDataType(a, dataType, typeMap) {
  let dataTypeRef = 0
  // The following is when the dataType is atomic
  if (!dataType && a.$.name.toLowerCase().includes(dbEnum.zclType.bitmap)) {
    dataTypeRef = typeMap.get(dbEnum.zclType.bitmap)
  } else if (
    !dataType &&
    a.$.name.toLowerCase().includes(dbEnum.zclType.enum)
  ) {
    dataTypeRef = typeMap.get(dbEnum.zclType.enum)
  } else if (
    !dataType &&
    a.$.name.toLowerCase().includes(dbEnum.zclType.string)
  ) {
    dataTypeRef = typeMap.get(dbEnum.zclType.string)
  } else if (
    !dataType &&
    a.$.name.toLowerCase().includes(dbEnum.zclType.struct)
  ) {
    dataTypeRef = typeMap.get(dbEnum.zclType.struct)
  } else if (!dataType) {
    dataTypeRef = typeMap.get(dbEnum.zclType.number)
  }
  return {
    name: a.$.name,
    id: parseInt(a.$.id),
    description: a.$.description ? a.$.description : a.$.name,
    discriminator_ref: dataType ? dataType : dataTypeRef,
    cluster_code: a.cluster ? a.cluster : null,
  }
}

/**
 * Processes Data Type.
 *
 * @param {*} db
 * @param {*} filePath
 * @param {*} packageId
 * @param {*} knownPackages
 * @param {*} data
 * @param {*} dataType
 * @returns Promise of inserted Data Types into the Data Type table.
 */
async function processDataType(
  db,
  filePath,
  packageId,
  knownPackages,
  data,
  dataType
) {
  let typeMap = await zclLoader.getDiscriminatorMap(db, knownPackages)

  if (dataType == dbEnum.zclType.atomic) {
    let types = data[0].type
    env.logDebug(`${filePath}, ${packageId}: ${data.length} Atomic Data Types.`)
    return queryLoader.insertDataType(
      db,
      packageId,
      types.map((x) => prepareDataType(x, 0, typeMap))
    )
  } else if (dataType == dbEnum.zclType.enum) {
    env.logDebug(`${filePath}, ${packageId}: ${data.length} Enum Data Types.`)
    return queryLoader.insertDataType(
      db,
      packageId,
      data.map((x) =>
        prepareDataType(x, typeMap.get(dbEnum.zclType.enum), typeMap)
      )
    )
  } else if (dataType == dbEnum.zclType.bitmap) {
    env.logDebug(`${filePath}, ${packageId}: ${data.length} Bitmap Data Types.`)
    return queryLoader.insertDataType(
      db,
      packageId,
      data.map((x) =>
        prepareDataType(x, typeMap.get(dbEnum.zclType.bitmap), typeMap)
      )
    )
  } else if (dataType == dbEnum.zclType.struct) {
    env.logDebug(`${filePath}, ${packageId}: ${data.length} Struct Data Types.`)
    return queryLoader.insertDataType(
      db,
      packageId,
      data.map((x) =>
        prepareDataType(x, typeMap.get(dbEnum.zclType.struct), typeMap)
      )
    )
  } else if (dataType == dbEnum.zclType.string) {
    env.logDebug(`${filePath}, ${packageId}: ${data.length} String Data Types.`)
    return queryLoader.insertDataType(
      db,
      packageId,
      data.map((x) =>
        prepareDataType(x, typeMap.get(dbEnum.zclType.string), typeMap)
      )
    )
  } else {
    env.logError(
      'Could not find the discriminator for the data type: ' + dataType
    )
  }
}

/**
 * Prepare numbers for database table insertion.
 *
 * @param {*} a
 * @param {*} dataType
 * @returns An Object
 */
function prepareNumber(a, dataType) {
  return {
    size: a.$.size,
    is_signed: a.$.name.endsWith('u') || !a.$.name.includes('int') ? 0 : 1,
    name: a.$.name,
    cluster_code: a.cluster ? a.cluster : null,
    discriminator_ref: dataType,
  }
}

/**
 * Processes Numbers.
 *
 * @param {*} db
 * @param {*} filePath
 * @param {*} packageId
 * @param {*} knownPackages
 * @param {*} data
 * @returns Promise of inserted numbers into the number table.
 */
async function processNumber(db, filePath, packageId, knownPackages, data) {
  let typeMap = await zclLoader.getDiscriminatorMap(db, knownPackages)
  let numbers = data[0].type.filter(function (item) {
    return (
      !item.$.name.toLowerCase().includes(dbEnum.zclType.bitmap) &&
      !item.$.name.toLowerCase().includes(dbEnum.zclType.enum) &&
      !item.$.name.toLowerCase().includes(dbEnum.zclType.string) &&
      !item.$.name.toLowerCase().includes(dbEnum.zclType.struct)
    )
  })
  env.logDebug(`${filePath}, ${packageId}: ${data.length} Number Types.`)
  return queryLoader.insertNumber(
    db,
    packageId,
    numbers.map((x) => prepareNumber(x, typeMap.get(dbEnum.zclType.number)))
  )
}

/**
 * Prepare strings for database table insertion.
 *
 * @param {*} a
 * @param {*} dataType
 * @returns An Object
 */
function prepareString(a, dataType) {
  return {
    is_long: a.$.long && a.$.long.toLowerCase() == 'true' ? 1 : 0,
    size: a.$.size,
    is_char: 0,
    name: a.$.name,
    cluster_code: a.cluster ? a.cluster : null,
    discriminator_ref: dataType,
  }
}

/**
 * Processes Strings.
 *
 * @param {*} db
 * @param {*} filePath
 * @param {*} packageId
 * @param {*} knownPackages
 * @param {*} data
 * @returns Promise of inserted strings into the String table.
 */
async function processString(db, filePath, packageId, knownPackages, data) {
  let typeMap = await zclLoader.getDiscriminatorMap(db, knownPackages)
  let strings = data[0].type.filter(function (item) {
    return (
      (item.$.string && item.$.string.toLowerCase() == 'true') ||
      (item.$.name && item.$.name.toLowerCase().includes('string'))
    )
  })
  env.logDebug(`${filePath}, ${packageId}: ${data.length} String Types.`)
  return queryLoader.insertString(
    db,
    packageId,
    strings.map((x) => prepareString(x, typeMap.get(dbEnum.zclType.string)))
  )
}

/**
 * Prepare enums or bitmaps for database table insertion.
 *
 * @param {*} a
 * @param {*} dataType
 * @returns An Object
 */
function prepareEnumOrBitmapAtomic(a, dataType) {
  return {
    size: a.$.size,
    name: a.$.name,
    cluster_code: a.cluster ? a.cluster : null,
    discriminator_ref: dataType,
  }
}

/**
 * Processes the enums.
 *
 * @param {*} db
 * @param {*} filePath
 * @param {*} packageId
 * @param {*} knownPackages
 * @param {*} data
 * @returns A promise of inserted enums.
 */
async function processEnumAtomic(db, filePath, packageId, knownPackages, data) {
  let typeMap = await zclLoader.getDiscriminatorMap(db, knownPackages)
  let enums = data[0].type.filter(function (item) {
    return item.$.name.toLowerCase().includes('enum')
  })
  env.logDebug(`${filePath}, ${packageId}: ${data.length} Baseline Enum Types.`)
  return queryLoader.insertEnumAtomic(
    db,
    packageId,
    enums.map((x) =>
      prepareEnumOrBitmapAtomic(x, typeMap.get(dbEnum.zclType.enum))
    )
  )
}

/**
 * Prepare enums or bitmaps for database table insertion.
 *
 * @param {*} a
 * @param {*} dataType
 * @returns An Object
 */
function prepareEnumOrBitmap(a, dataType, typeMap) {
  // Taking care of a typo for backwards compatibility
  // for eg <enum name="Status" type="INT8U" i.e. an enum defined as int8u
  let enumIndex = typeMap.get(dbEnum.zclType.enum)
  if (
    dataType == enumIndex &&
    (a.$.type.toLowerCase().includes('int') ||
      a.$.type.toLowerCase().includes(dbEnum.zclType.bitmap))
  ) {
    env.logWarning(
      'Check type contradiction in XML metadata for ' +
        a.$.name +
        ' with type ' +
        a.$.type
    )
    a.$.type = 'enum' + a.$.type.toLowerCase().match(/\d+/g).join('')
  }
  return {
    name: a.$.name,
    type: a.$.type.toLowerCase(),
    cluster_code: a.cluster ? a.cluster : null,
    discriminator_ref: dataType,
  }
}

/**
 * Processes the enums.
 *
 * @param {*} db
 * @param {*} filePath
 * @param {*} packageId
 * @param {*} knownPackages
 * @param {*} data
 * @returns A promise of inserted enums.
 */
async function processEnum(db, filePath, packageId, knownPackages, data) {
  env.logDebug(`${filePath}, ${packageId}: ${data.length} Enum Types.`)
  let typeMap = await zclLoader.getDiscriminatorMap(db, knownPackages)
  return queryLoader.insertEnum(
    db,
    knownPackages,
    data.map((x) =>
      prepareEnumOrBitmap(x, typeMap.get(dbEnum.zclType.enum), typeMap)
    )
  )
}

/**
 * Processes the enum Items.
 *
 * @param {*} db
 * @param {*} filePath
 * @param {*} packageId
 * @param {*} knownPackages
 * @param {*} data
 * @returns A promise of inserted enum items.
 */
async function processEnumItems(db, filePath, packageId, knownPackages, data) {
  env.logDebug(`${filePath}, ${packageId}: ${data.length} Enum Items.`)
  let enumItems = []
  let lastFieldId = -1
  data.forEach((e) => {
    if ('item' in e) {
      e.item.forEach((item) => {
        let defaultFieldId = lastFieldId + 1
        lastFieldId = item.$.fieldId ? parseInt(item.$.fieldId) : defaultFieldId
        enumItems.push({
          enumName: e.$.name,
          enumClusterCode: e.cluster ? parseInt(e.clusterCode) : null,
          name: item.$.name,
          value: parseInt(item.$.value),
          fieldIdentifier: lastFieldId,
        })
      })
    }
  })
  return queryLoader.insertEnumItems(db, packageId, knownPackages, enumItems)
}

/**
 * Processes the bitmaps.
 *
 * @param {*} db
 * @param {*} filePath
 * @param {*} packageId
 * @param {*} knownPackages
 * @param {*} data
 * @returns A promise of inserted bitmaps.
 */
async function processBitmapAtomic(
  db,
  filePath,
  packageId,
  knownPackages,
  data
) {
  let typeMap = await zclLoader.getDiscriminatorMap(db, knownPackages)
  let bitmaps = data[0].type.filter(function (item) {
    return item.$.name.toLowerCase().includes(dbEnum.zclType.bitmap)
  })
  env.logDebug(
    `${filePath}, ${packageId}: ${data.length} Baseline Bitmap Types.`
  )
  return queryLoader.insertBitmapAtomic(
    db,
    packageId,
    bitmaps.map((x) =>
      prepareEnumOrBitmapAtomic(x, typeMap.get(dbEnum.zclType.bitmap))
    )
  )
}

/**
 * Processes the bitmaps.
 *
 * @param {*} db
 * @param {*} filePath
 * @param {*} packageId
 * @param {*} knownPackages
 * @param {*} data
 * @returns A promise of inserted bitmaps.
 */
async function processBitmap(db, filePath, packageId, knownPackages, data) {
  env.logDebug(`${filePath}, ${packageId}: ${data.length} Bitmap Types.`)
  let typeMap = await zclLoader.getDiscriminatorMap(db, knownPackages)
  return queryLoader.insertBitmap(
    db,
    knownPackages,
    data.map((x) =>
      prepareEnumOrBitmap(x, typeMap.get(dbEnum.zclType.bitmap), typeMap)
    )
  )
}

/**
 * Processes the bitmap fields.
 *
 * @param {*} db
 * @param {*} filePath
 * @param {*} packageId
 * @param {*} knownPackages
 * @param {*} data
 * @returns A promise of inserted bitmap fields.
 */
async function processBitmapFields(
  db,
  filePath,
  packageId,
  knownPackages,
  data
) {
  env.logDebug(`${filePath}, ${packageId}: ${data.length} Bitmap Fields.`)
  let bitmapFields = []
  let lastFieldId = -1
  data.forEach((bm) => {
    if ('field' in bm) {
      bm.field.forEach((item) => {
        let defaultFieldId = lastFieldId + 1
        lastFieldId = item.$.fieldId ? parseInt(item.$.fieldId) : defaultFieldId
        bitmapFields.push({
          bitmapName: bm.$.name,
          bitmapClusterCode: bm.cluster ? parseInt(bm.clusterCode) : null,
          name: item.$.name,
          mask: parseInt(item.$.mask),
          fieldIdentifier: lastFieldId,
        })
      })
    }
  })
  return queryLoader.insertBitmapFields(
    db,
    packageId,
    knownPackages,
    bitmapFields
  )
}

/**
 * Prepare structs for database table insertion.
 *
 * @param {*} a
 * @param {*} dataType
 * @returns An Object
 */
function prepareStruct(a, dataType) {
  return {
    name: a.$.name,
    cluster_code: a.cluster ? a.cluster : null,
    discriminator_ref: dataType,
    isFabricScoped: a.$.isFabricScoped == 'true',
  }
}

/**
 * Processes the structs.
 *
 * @param {*} db
 * @param {*} filePath
 * @param {*} packageId
 * @param {*} knownPackages
 * @param {*} data
 * @returns A promise of inserted structs.
 */
async function processStruct(db, filePath, packageId, knownPackages, data) {
  env.logDebug(`${filePath}, ${packageId}: ${data.length} Struct Types.`)
  let typeMap = await zclLoader.getDiscriminatorMap(db, knownPackages)
  return queryLoader.insertStruct(
    db,
    knownPackages,
    data.map((x) => prepareStruct(x, typeMap.get(dbEnum.zclType.struct)))
  )
}

/**
 * Processes the struct Items.
 *
 * @param {*} db
 * @param {*} filePath
 * @param {*} packageIds
 * @param {*} data
 * @returns A promise of inserted struct items.
 */
async function processStructItems(db, filePath, packageIds, data, context) {
  env.logDebug(`${filePath}, ${packageIds}: ${data.length} Struct Items.`)
  let structItems = []
  data.forEach((si) => {
    let lastFieldId = -1
    if ('item' in si) {
      si.item.forEach((item) => {
        let defaultFieldId = lastFieldId + 1
        lastFieldId = item.$.fieldId ? parseInt(item.$.fieldId) : defaultFieldId
        structItems.push({
          structName: si.$.name,
          structClusterCode: si.cluster ? parseInt(si.clusterCode) : null,
          name: item.$.name,
          type:
            item.$.type == item.$.type.toUpperCase() && item.$.type.length > 1
              ? item.$.type.toLowerCase()
              : item.$.type,
          fieldIdentifier: lastFieldId,
          minLength: 0,
          maxLength: item.$.length ? item.$.length : null,
          isWritable: item.$.writable == 'true',
          isArray: item.$.array == 'true' ? true : false,
          isEnum: item.$.enum == 'true' ? true : false,
          isNullable: item.$.isNullable == 'true' ? true : false,
          isOptional: item.$.optional == 'true' ? true : false,
          isFabricSensitive: item.$.isFabricSensitive == 'true' ? true : false,
        })
      })
    }

    if (
      context.fabricHandling &&
      context.fabricHandling.automaticallyCreateFields &&
      si.$.isFabricScoped == 'true'
    ) {
      structItems.push({
        structName: si.$.name,
        structClusterCode: si.cluster ? parseInt(si.clusterCode) : null,
        name: context.fabricHandling.indexFieldName,
        type: context.fabricHandling.indexType,
        fieldIdentifier: context.fabricHandling.indexFieldId,
        minLength: 0,
        maxLength: null,
        isWritable: false,
        isArray: false,
        isEnum: false,
        isNullable: false,
        isOptional: false,
        isFabricSensitive: false,
      })
    }
  })
  return queryLoader.insertStructItems(db, packageIds, structItems)
}

/**
 * Preparation step for the device types.
 *
 * @param {*} deviceType
 * @returns an object containing the prepared device types.
 */
function prepareDeviceType(deviceType) {
  let ret = {
    code: parseInt(deviceType.deviceId[0]['_']),
    profileId: parseInt(deviceType.profileId[0]['_']),
    domain: deviceType.domain[0],
    name: deviceType.name[0],
    description: deviceType.typeName[0],
  }
  if ('clusters' in deviceType) {
    ret.clusters = []
    deviceType.clusters.forEach((cluster) => {
      if ('include' in cluster) {
        cluster.include.forEach((include) => {
          let attributes = []
          let commands = []
          if ('requireAttribute' in include) {
            attributes = include.requireAttribute
          }
          if ('requireCommand' in include) {
            commands = include.requireCommand
          }
          ret.clusters.push({
            client: 'true' == include.$.client,
            server: 'true' == include.$.server,
            clientLocked: 'true' == include.$.clientLocked,
            serverLocked: 'true' == include.$.serverLocked,
            clusterName:
              include.$.cluster != undefined ? include.$.cluster : include._,
            requiredAttributes: attributes,
            requiredCommands: commands,
          })
        })
      }
    })
  }
  return ret
}

/**
 * Process all device types.
 *
 * @param {*} db
 * @param {*} filePath
 * @param {*} packageId
 * @param {*} data
 * @returns Promise of a resolved device types.
 */
async function processDeviceTypes(db, filePath, packageId, data) {
  env.logDebug(`${filePath}, ${packageId}: ${data.length} deviceTypes.`)
  return queryLoader.insertDeviceTypes(
    db,
    packageId,
    data.map((x) => prepareDeviceType(x))
  )
}

/**
 * After XML parser is done with the barebones parsing, this function
 * branches the individual toplevel tags.
 *
 * @param {*} db
 * @param {*} argument
 * @returns promise that resolves when all the subtags are parsed.
 */
async function processParsedZclData(
  db,
  argument,
  previouslyKnownPackages,
  context
) {
  let filePath = argument.filePath
  let data = argument.result
  let packageId = argument.packageId
  previouslyKnownPackages.add(packageId)
  let knownPackages = Array.from(previouslyKnownPackages)

  if (!('result' in argument)) {
    return []
  } else {
    let toplevel = null

    if ('configurator' in data) {
      toplevel = data.configurator
    }
    if ('zap' in data) {
      toplevel = data.zap
    }

    if (toplevel == null) return []

    // We load in multiple batches, since each batch needs to resolve
    // before the next batch can be loaded, as later data depends on
    // previous data. Final batch is delayed, meaning that
    // the promises there can't start yet, until all files are loaded.

    // Batch 1: load accessControl, tag and domain
    let batch1 = []
    if ('accessControl' in toplevel) {
      batch1.push(
        processAccessControl(db, filePath, packageId, toplevel.accessControl)
      )
    }
    if ('tag' in toplevel) {
      batch1.push(processTags(db, filePath, packageId, toplevel.tag))
    }
    if ('domain' in toplevel) {
      batch1.push(processDomains(db, filePath, packageId, toplevel.domain))
    }
    await Promise.all(batch1)

    // Batch 2: device types, globals, clusters
    let batch2 = []
    if ('deviceType' in toplevel) {
      batch2.push(
        processDeviceTypes(db, filePath, packageId, toplevel.deviceType)
      )
    }
    if ('global' in toplevel) {
      batch2.push(
        processGlobals(db, filePath, packageId, toplevel.global, context)
      )
    }
    if ('cluster' in toplevel) {
      batch2.push(
        processClusters(db, filePath, packageId, toplevel.cluster, context)
      )
    }
    await Promise.all(batch2)
    // Batch 3: Load the data type table which lists all data types
    let batch3 = []
    if (dbEnum.zclType.atomic in toplevel) {
      batch3.push(
        processDataType(
          db,
          filePath,
          packageId,
          knownPackages,
          toplevel.atomic,
          dbEnum.zclType.atomic
        )
      )
    }

    if (dbEnum.zclType.bitmap in toplevel) {
      batch3.push(
        processDataType(
          db,
          filePath,
          packageId,
          knownPackages,
          toplevel.bitmap,
          dbEnum.zclType.bitmap
        )
      )
    }
    if (dbEnum.zclType.enum in toplevel) {
      batch3.push(
        processDataType(
          db,
          filePath,
          packageId,
          knownPackages,
          toplevel.enum,
          dbEnum.zclType.enum
        )
      )
    }
    if (dbEnum.zclType.struct in toplevel) {
      batch3.push(
        processDataType(
          db,
          filePath,
          packageId,
          knownPackages,
          toplevel.struct,
          dbEnum.zclType.struct
        )
      )
    }
    await Promise.all(batch3)

    // Batch4 and Batch5: Loads the inidividual tables per data type from
    // atomics/baseline types to inherited types
    let Batch4 = []
    if (dbEnum.zclType.atomic in toplevel) {
      Batch4.push(
        processNumber(db, filePath, packageId, knownPackages, toplevel.atomic)
      )
      Batch4.push(
        processString(db, filePath, packageId, knownPackages, toplevel.atomic)
      )
      Batch4.push(
        processEnumAtomic(
          db,
          filePath,
          packageId,
          knownPackages,
          toplevel.atomic
        )
      )
      Batch4.push(
        processBitmapAtomic(
          db,
          filePath,
          packageId,
          knownPackages,
          toplevel.atomic
        )
      )
    }
    await Promise.all(Batch4)

    let Batch5 = []
    if (dbEnum.zclType.enum in toplevel) {
      Batch5.push(
        processEnum(db, filePath, packageId, knownPackages, toplevel.enum)
      )
    }
    if (dbEnum.zclType.bitmap in toplevel) {
      Batch5.push(
        processBitmap(db, filePath, packageId, knownPackages, toplevel.bitmap)
      )
    }
    if (dbEnum.zclType.struct in toplevel) {
      Batch5.push(
        processStruct(db, filePath, packageId, knownPackages, toplevel.struct)
      )
    }
    await Promise.all(Batch5)

    // Batch7: Loads the items within a bitmap, struct and enum data types
    let batch6 = []
    if (dbEnum.zclType.enum in toplevel) {
      batch6.push(
        processEnumItems(db, filePath, packageId, knownPackages, toplevel.enum)
      )
    }
    if (dbEnum.zclType.bitmap in toplevel) {
      batch6.push(
        processBitmapFields(
          db,
          filePath,
          packageId,
          knownPackages,
          toplevel.bitmap
        )
      )
    }
    if (dbEnum.zclType.struct in toplevel) {
      batch6.push(
        processStructItems(
          db,
          filePath,
          knownPackages,
          toplevel.struct,
          context
        )
      )
    }
    await Promise.all(batch6)

    // Batch7: Loads the defaultAccess
    let Batch7 = []
    if ('defaultAccess' in toplevel) {
      Batch7.push(
        processDefaultAccess(db, filePath, packageId, toplevel.defaultAccess)
      )
    }
    if ('atomic' in toplevel) {
      Batch7.push(processAtomics(db, filePath, packageId, toplevel.atomic))
    }
    await Promise.all(Batch7)
    //}

    // Batch 8: cluster extensions and global attributes
    //   These don't start right away, but are delayed. So we don't return
    //   promises that have already started, but functions that return promises.
    let delayedPromises = []

    if ('cluster' in toplevel) {
      delayedPromises.push(() =>
        processClusterGlobalAttributes(
          db,
          filePath,
          packageId,
          toplevel.cluster
        )
      )
    }
    if ('clusterExtension' in toplevel) {
      delayedPromises.push(() =>
        processClusterExtensions(
          db,
          filePath,
          packageId,
          knownPackages,
          toplevel.clusterExtension,
          context
        )
      )
    }
    return Promise.all(delayedPromises)
  }
}

/**
 * This function is used for parsing each individual ZCL file at a grouped zcl file package level.
 * This should _not_ be used for custom XML addition due to custom xmls potentially relying on existing packges.
 * @param {*} db
 * @param {*} packageId
 * @param {*} file
 * @returns A promise for when the last stage of the loading pipeline finishes.
 */
async function parseSingleZclFile(db, packageId, file, context) {
  try {
    let fileContent = await fsp.readFile(file)
    let data = {
      filePath: file,
      data: fileContent,
      crc: util.checksum(fileContent),
    }
    let result = await zclLoader.qualifyZclFile(
      db,
      data,
      packageId,
      dbEnum.packageType.zclXml,
      false
    )
    if (result.data) {
      result.result = await util.parseXml(fileContent)
      delete result.data
    }
    return processParsedZclData(db, result, new Set(), context)
  } catch (err) {
    err.message = `Error reading xml file: ${file}\n` + err.message
    throw err
  }
}

/**
 *
 * Promises to iterate over all the XML files and returns an aggregate promise
 * that will be resolved when all the XML files are done, or rejected if at least one fails.
 *
 * @param {*} db
 * @param {*} packageId
 * @param {*} zclFiles
 * @param {*} context
 * @returns Promise that resolves when all the individual promises of each file pass.
 */
async function parseZclFiles(db, packageId, zclFiles, context) {
  env.logDebug(`Starting to parse ZCL files: ${zclFiles}`)
  // Populate the Data Type Discriminator
  if (context.ZCLDataTypes)
    await processDataTypeDiscriminator(db, packageId, context.ZCLDataTypes)

  // Load the Types File first such the atomic types are loaded and can be
  //referenced by other types
  let typesFiles = zclFiles.filter((file) => file.includes('types.xml'))
  let typeFilePromise = typesFiles.map((file) =>
    parseSingleZclFile(db, packageId, file, context)
  )
  await Promise.all(typeFilePromise)

  // Load everything apart from atomic data types
  let nonTypesFiles = zclFiles.filter((file) => !file.includes('types.xml'))
  let individualFilePromise = nonTypesFiles.map((file) =>
    parseSingleZclFile(db, packageId, file, context)
  )
  let individualResults = await Promise.all(individualFilePromise)
  let laterPromises = individualResults.flat(2)
  await Promise.all(laterPromises.map((promise) => promise()))

  // Load some missing content which was not possible before the above was done
  return zclLoader.processZclPostLoading(db, packageId)
}

/**
 * Parses the manufacturers xml.
 *
 * @param {*} db
 * @param {*} ctx
 * @returns Promise of a parsed manufacturers file.
 */
async function parseManufacturerData(db, packageId, manufacturersXml) {
  let data = await fsp.readFile(manufacturersXml)

  let manufacturerMap = await util.parseXml(data)

  return queryPackage.insertOptionsKeyValues(
    db,
    packageId,
    dbEnum.packageOptionCategory.manufacturerCodes,
    manufacturerMap.map.mapping.map((datum) => {
      let mfgPair = datum['$']
      return { code: mfgPair['code'], label: mfgPair['translation'] }
    })
  )
}

/**
 * Parses the profiles xml.
 *
 * @param {*} db
 * @param {*} ctx
 * @returns Promise of a parsed profiles file.
 */
async function parseProfilesData(db, packageId, profilesXml) {
  let data = await fsp.readFile(profilesXml)

  let profilesMap = await util.parseXml(data)

  return queryPackage.insertOptionsKeyValues(
    db,
    packageId,
    dbEnum.packageOptionCategory.profileCodes,
    profilesMap.map.mapping.map((datum) => {
      let profilePair = datum['$']
      return { code: profilePair['code'], label: profilePair['translation'] }
    })
  )
}

/**
 * Parses the ZCL Schema
 * @param {*} db
 */
async function parseZclSchema(db, packageId, zclSchema, zclValidation) {
  let content = await fsp.readFile(zclSchema)
  let info = {
    filePath: zclSchema,
    data: content,
    crc: util.checksum(content),
  }
  await zclLoader.qualifyZclFile(
    db,
    info,
    packageId,
    dbEnum.packageType.zclSchema,
    false
  )
  content = await fsp.readFile(zclValidation)
  info = {
    filePath: zclValidation,
    data: content,
    crc: util.checksum(content),
  }

  return zclLoader.qualifyZclFile(
    db,
    info,
    packageId,
    dbEnum.packageType.zclValidation,
    false
  )
}

/**
 * Inside the `zcl.json` can be a `featureFlags` key, which is
 * a general purpose object. It contains keys, that map to objects.
 * Each key is a "package option category".
 * Key/velues of the object itself, end up in CODE/LABEL combinations.
 *
 * @param {*} db
 * @param {*} packageId
 * @param {*} featureFlags
 * @returns array of feature flags
 */
async function parseFeatureFlags(db, packageId, featureFlags) {
  return Promise.all(
    Object.keys(featureFlags).map((featureCategory) => {
      return queryPackage.insertOptionsKeyValues(
        db,
        packageId,
        featureCategory,
        Object.keys(featureFlags[featureCategory]).map((data) => {
          return {
            code: data,
            label: featureFlags[featureCategory][data] == '1' ? true : false,
          }
        })
      )
    })
  )
}

/**
 * Inside the `zcl.json` can be a `featureFlags` key, which is
 * a general purpose object. It contains keys, that map to objects.
 * Each key is a "package option category".
 * Key/velues of the object itself, end up in CODE/LABEL combinations.
 *
 * @param {*} db
 * @param {*} packageId
 * @param {*} featureFlags
 * @returns Promise that loads the uiOptions object into the database.
 */
async function parseUiOptions(db, packageId, uiOptions) {
  let data = []
  Object.keys(uiOptions).map((key) => {
    data.push({
      code: key,
      label: uiOptions[key],
    })
  })
  return queryPackage.insertOptionsKeyValues(
    db,
    packageId,
    dbEnum.packageOptionCategory.ui,
    data
  )
}
/**
 * Parses and loads the text and boolean options.
 *
 * @param {*} db
 * @returns promise of parsed options
 */
async function parseOptions(db, packageId, options) {
  let promises = []
  promises.push(parseTextOptions(db, packageId, options.text))
  promises.push(parseBoolOptions(db, packageId, options.bool))
  return Promise.all(promises)
}

/**
 * Parses the text options.
 *
 * @param {*} db
 * @param {*} pkgRef
 * @param {*} textOptions
 * @returns Promise of a parsed text options.
 */
async function parseTextOptions(db, pkgRef, textOptions) {
  if (!textOptions) return Promise.resolve()
  let promises = Object.keys(textOptions).map((optionKey) => {
    let val = textOptions[optionKey]
    let optionValues
    if (Array.isArray(val)) {
      optionValues = val
    } else {
      optionValues = val.split(',').map((opt) => opt.trim())
    }
    return queryPackage.insertOptionsKeyValues(
      db,
      pkgRef,
      optionKey,
      optionValues.map((optionValue) => {
        return { code: optionValue.toLowerCase(), label: optionValue }
      })
    )
  })
  return Promise.all(promises)
}

/**
 * Parses the boolean options.
 *
 * @param {*} db
 * @param {*} pkgRef
 * @param {*} booleanCategories
 * @returns Promise of a parsed boolean options.
 */
async function parseBoolOptions(db, pkgRef, booleanCategories) {
  if (!booleanCategories) return Promise.resolve()
  let options
  if (Array.isArray(booleanCategories)) {
    options = booleanCategories
  } else {
    options = booleanCategories
      .split(',')
      .map((optionValue) => optionValue.trim())
  }
  let promises = []
  options.forEach((optionCategory) => {
    promises.push(
      queryPackage.insertOptionsKeyValues(db, pkgRef, optionCategory, [
        { code: 1, label: 'True' },
        { code: 0, label: 'False' },
      ])
    )
  })
  return Promise.all(promises)
}

/**
 * Parses the default values inside the options.
 *
 * @param {*} db
 * @param {*} ctx
 * @returns Promised of parsed text and bool defaults.
 */
async function parseDefaults(db, packageId, defaults) {
  let promises = []
  promises.push(parseTextDefaults(db, packageId, defaults.text))
  promises.push(parseBoolDefaults(db, packageId, defaults.bool))
  return Promise.all(promises)
}

async function parseTextDefaults(db, pkgRef, textDefaults) {
  if (!textDefaults) return Promise.resolve()

  let promises = []
  for (let optionCategory of Object.keys(textDefaults)) {
    let txt = textDefaults[optionCategory]
    promises.push(
      queryPackage
        .selectSpecificOptionValue(db, pkgRef, optionCategory, txt)
        .then((specificValue) => {
          if (specificValue != null) return specificValue
          if (_.isNumber(txt)) {
            // Try to convert to hex.
            let hex = '0x' + txt.toString(16)
            return queryPackage.selectSpecificOptionValue(
              db,
              pkgRef,
              optionCategory,
              hex
            )
          } else {
            return specificValue
          }
        })
        .then((specificValue) => {
          if (specificValue == null) {
            throw `Default value for: ${optionCategory}/${txt} does not match an option.`
          } else {
            return queryPackage.insertDefaultOptionValue(
              db,
              pkgRef,
              optionCategory,
              specificValue.id
            )
          }
        })
    )
  }
  return Promise.all(promises)
}

async function parseBoolDefaults(db, pkgRef, booleanCategories) {
  if (!booleanCategories) return Promise.resolve()

  let promises = []
  for (let optionCategory of Object.keys(booleanCategories)) {
    promises.push(
      queryPackage
        .selectSpecificOptionValue(
          db,
          pkgRef,
          optionCategory,
          booleanCategories[optionCategory] ? 1 : 0
        )
        .then((specificValue) =>
          queryPackage.insertDefaultOptionValue(
            db,
            pkgRef,
            optionCategory,
            specificValue.id
          )
        )
    )
  }
  return Promise.all(promises)
}

/**
 * Parses a single file. This function is used specifically
 * for adding a package through an existing session because of its reliance
 * on relating the new XML content to the packages associated with that session.
 * e.g. for ClusterExtensions.
 *
 * @param {*} db
 * @param {*} filePath
 * @returns Promise of a loaded file.
 */
async function loadIndividualSilabsFile(
  db,
  filePath,
  boundValidator,
  sessionId
) {
  try {
    let fileContent = await fsp.readFile(filePath)
    let data = {
      filePath: filePath,
      data: fileContent,
      crc: util.checksum(fileContent),
    }

    let result = await zclLoader.qualifyZclFile(
      db,
      data,
      null,
      dbEnum.packageType.zclXmlStandalone,
      true
    )
    let pkgId = result.packageId
    if (boundValidator != null && fileContent != null) {
      result.validation = boundValidator(fileContent)
    }
    if (result.data) {
      result.result = await util.parseXml(result.data)
      delete result.data
    }
    if (result.validation && result.validation.isValid == false) {
      throw new Error('Validation Failed')
    }
    let sessionPackages = await queryPackage.getSessionZclPackages(
      db,
      sessionId
    )
    let packageSet = new Set()
    sessionPackages.map((sessionPackage) => {
      packageSet.add(sessionPackage.packageRef)
    })
    // Where do we get metadata from here???
    let laterPromises = await processParsedZclData(db, result, packageSet, {})
    await Promise.all(
      laterPromises.flat(1).map((promise) => {
        if (promise != null && promise != undefined) return promise()
      })
    )
    await zclLoader.processZclPostLoading(db, pkgId)
    return { succeeded: true, packageId: pkgId }
  } catch (err) {
    env.logError(`Error reading xml file: ${filePath}\n` + err.message)
    return { succeeded: false, err: err }
  }
}

/**
 * If custom device is supported, then this method creates it.
 *
 * @param {*} db
 * @param {*} ctx
 * @returns context
 */
async function processCustomZclDeviceType(db, packageId) {
  let customDeviceTypes = []
  customDeviceTypes.push({
    domain: dbEnum.customDevice.domain,
    code: dbEnum.customDevice.code,
    profileId: dbEnum.customDevice.profileId,
    name: dbEnum.customDevice.name,
    description: dbEnum.customDevice.description,
  })
  let existingCustomDevice =
    await queryDeviceType.selectDeviceTypeByCodeAndName(
      db,
      packageId,
      dbEnum.customDevice.code,
      dbEnum.customDevice.name
    )
  if (existingCustomDevice == null)
    await queryLoader.insertDeviceTypes(db, packageId, customDeviceTypes)
}

/**
 * Toplevel function that loads the toplevel metafile
 * and orchestrates the promise chain.
 *
 * @export
 * @param {*} db
 * @param {*} ctx The context of loading.
 * @returns a Promise that resolves with the db.
 */
async function loadSilabsZcl(db, metafile, isJson = false) {
  let ctx = {
    metadataFile: metafile,
    db: db,
  }
  env.logDebug(`Loading Silabs zcl file: ${ctx.metadataFile}`)
  await dbApi.dbBeginTransaction(db)
  try {
    Object.assign(ctx, await util.readFileContentAndCrc(ctx.metadataFile))
    ctx.packageId = await zclLoader.recordToplevelPackage(
      db,
      ctx.metadataFile,
      ctx.crc
    )
    let ret
    if (isJson) {
      ret = await collectDataFromJsonFile(ctx.metadataFile, ctx.data)
    } else {
      ret = await collectDataFromPropertiesFile(ctx.metadataFile, ctx.data)
    }
    Object.assign(ctx, ret)
    if (
      ctx.version != null ||
      ctx.category != null ||
      ctx.description != null
    ) {
      await zclLoader.recordVersion(
        db,
        ctx.packageId,
        ctx.version,
        ctx.category,
        ctx.description
      )
    }
    await parseZclFiles(db, ctx.packageId, ctx.zclFiles, ctx)
    // Validate that our attributeAccessInterfaceAttributes, if present, is
    // sane.
    if (ctx.attributeAccessInterfaceAttributes) {
      let all_known_clusters = await queryZcl.selectAllClusters(
        db,
        ctx.packageId
      )
      for (let clusterName of Object.keys(
        ctx.attributeAccessInterfaceAttributes
      )) {
        let known_cluster = all_known_clusters.find(
          (c) => c.name == clusterName
        )
        if (!known_cluster) {
          throw new Error(
            `\n\nUnknown cluster "${clusterName}" in attributeAccessInterfaceAttributes\n\n`
          )
        }

        let known_cluster_attributes =
          await queryZcl.selectAttributesByClusterIdIncludingGlobal(
            db,
            known_cluster.id,
            ctx.packageId
          )
        for (let attrName of ctx.attributeAccessInterfaceAttributes[
          clusterName
        ]) {
          if (!known_cluster_attributes.find((a) => a.name == attrName)) {
            throw new Error(
              `\n\nUnknown attribute "${attrName}" in attributeAccessInterfaceAttributes["${clusterName}"]\n\n`
            )
          }
        }
      }
    }

    if (ctx.manufacturersXml) {
      await parseManufacturerData(db, ctx.packageId, ctx.manufacturersXml)
    }
    if (ctx.profilesXml) {
      await parseProfilesData(db, ctx.packageId, ctx.profilesXml)
    }
    if (ctx.supportCustomZclDevice) {
      await processCustomZclDeviceType(db, ctx.packageId)
    }
    if (ctx.options) {
      await parseOptions(db, ctx.packageId, ctx.options)
    }
    if (ctx.defaults) {
      await parseDefaults(db, ctx.packageId, ctx.defaults)
    }
    if (ctx.zclSchema && ctx.zclValidation) {
      await parseZclSchema(db, ctx.packageId, ctx.zclSchema, ctx.zclValidation)
    }
    if (ctx.featureFlags) {
      await parseFeatureFlags(db, ctx.packageId, ctx.featureFlags)
    }
    if (ctx.uiOptions) {
      await parseUiOptions(db, ctx.packageId, ctx.uiOptions)
    }
  } catch (err) {
    env.logError(err)
    throw err
  } finally {
    dbApi.dbCommit(db)
  }
  return ctx
}

exports.loadSilabsZcl = loadSilabsZcl
exports.loadIndividualSilabsFile = loadIndividualSilabsFile
