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
const env = require('../util/env')
const path = require('path')
const zclLoader = require('./zcl-loader')
const dbApi = require('../db/db-api.js')
const queryLoader = require('../db/query-loader.js')
const queryPackage = require('../db/query-package.js')
const dbEnum = require('../../src-shared/db-enum.js')
const util = require('../util/util.js')

/**
 * Promises to read the properties file, extract all the actual xml files, and resolve with the array of files.
 *
 * @param {*} ctx Context which contains information about the metadataFiles and data
 * @returns Promise of resolved files.
 */
async function collectDataFromLibraryXml(ctx) {
  env.logDebug(`Collecting ZCL files from: ${ctx.metadataFile}`)
  let fileContent = await fsp.readFile(ctx.metadataFile)
  ctx.crc = util.checksum(fileContent)
  let result = await util.parseXml(fileContent)
  let zclLib = result['zcl:library']
  ctx.version = '1.0'
  ctx.zclFiles = zclLib['xi:include'].map((f) =>
    path.join(path.dirname(ctx.metadataFile), f.$.href)
  )
  ctx.zclFiles.push(ctx.metadataFile)
  return ctx
}

// Random internal XML utility functions

function tagContainsEnum(tag) {
  return (
    tag.restriction != null &&
    tag.restriction.length > 0 &&
    'type:enumeration' in tag.restriction[0]
  )
}

function tagContainsStruct(tag) {
  return (
    tag.restriction != null &&
    tag.restriction.length > 0 &&
    'type:sequence' in tag.restriction[0]
  )
}

function tagContainsBitmap(tag) {
  return 'bitmap' in tag
}

async function parseSingleZclFile(db, ctx, file) {
  let fileContent = await fsp.readFile(file)
  let data = {
    filePath: file,
    data: fileContent,
    crc: util.checksum(fileContent),
  }

  let res = await zclLoader.qualifyZclFile(
    db,
    data,
    ctx.packageId,
    dbEnum.packageType.zclXml
  )
  if (!res.data) {
    return []
  }

  let result = await util.parseXml(fileContent)
  if (result['zcl:cluster']) {
    ctx.zclClusters.push(result['zcl:cluster'])
  } else if (result['zcl:global']) {
    let zclGlobal = result['zcl:global']
    ctx.zclGlobalTypes = zclGlobal['type:type']
    ctx.zclGlobalAttributes = zclGlobal.attributes[0].attribute
    ctx.zclGlobalCommands = zclGlobal.commands[0].command
  } else if (result['zcl:library']) {
    let zclLibrary = result['zcl:library']
    ctx.zclTypes = zclLibrary['type:type']
  } else if (result['zcl:device']) {
    let deviceTypes = result['zcl:device']
    if (ctx.zclDeviceTypes === undefined) {
      ctx.zclDeviceTypes = deviceTypes['deviceType']
    } else {
      ctx.zclDeviceTypes = ctx.zclDeviceTypes.concat(deviceTypes['deviceType'])
    }
  } else if (result['map']) {
    let manufacturers = result['map']
    ctx.zclManufacturers = manufacturers['mapping']
  } else {
    //TODO: What to do with "derived clusters", we skip them here but we should probably
    //      extend the DB schema to allow this since we don't really handle it
    env.logDebug(`Didn't find anything relevant, Skipping file ${file}`)
  }
}

/**
 *
 * Promises to iterate over all the XML files and returns an aggregate promise
 * that will be resolved when all the XML files are done, or rejected if at least one fails.
 *
 * @param {*} db
 * @param {*} ctx
 * @returns Promise that resolves when all the individual promises of each file pass.
 */
async function parseZclFiles(db, ctx) {
  let perFilePromise = []

  ctx.zclClusters = []
  ctx.zclGlobalAttributes = []
  ctx.zclGlobalCommands = []
  ctx.zclDeviceTypes = []
  ctx.zclManufacturers = []

  // Load the Types File first such the atomic types are loaded and can be
  //referenced by other types
  let typesFiles = ctx.zclFiles.filter((file) => file.includes('library.xml'))
  let typeFilePromise = typesFiles.map((file) =>
    parseSingleZclFile(db, ctx, file)
  )
  await Promise.all(typeFilePromise)

  // Load everything apart from atomic data types
  let nonTypesFiles = ctx.zclFiles.filter(
    (file) => !file.includes('library.xml')
  )
  nonTypesFiles.map((file) => {
    let p = parseSingleZclFile(db, ctx, file)
    perFilePromise.push(p)
  })

  return Promise.all(perFilePromise)
}

/**
 * The Dotdot ZCL XML doesn't use the 0x prefix, but it's a nice thing to have and Silabs xml
 * does use this so this helper function normalizes the use of hex
 *
 * TODO: Is this the right thing to do?
 *
 * @param {*} value the string value to be normalized
 * @returns Either the normalized hex string (with the 0x prefix) or the original
 */
function normalizeHexValue(value) {
  let ret = value
  try {
    parseInt(value, 16) //check if this is a hex value
    if (!value.includes('0x')) {
      ret = '0x' + value.toUpperCase()
    }
  } catch (error) {
    ret = value
  }
  return ret
}

/**
 * The Dotdot ZCL XML doesn't have a length but it is embedded in the short name,
 * we can scrape the value to get the size
 *
 * TODO: Is this the right thing to do?
 *
 * @param {*} value the string value to be scraped
 * @returns size in bytes or 0 if the # of bytes could not be determined
 */
function getNumBytesFromShortName(value) {
  let ret = 0
  try {
    let sn = value.replace(/[^0-9.]+/g, '')
    if (sn.length > 0) {
      let n = parseInt(sn)
      ret = n / 8
    }
  } catch (error) {
    ret = value
  }
  return ret
}

/**
 * Prepare XML attributes for entry into the DB
 *
 * @param {*} attributes an array of attributes
 * @param {*} side the side the attribute is on either "client" or "server"
 * @returns Array containing all data from XML ready to be inserted into the DB.
 */
function prepareAttributes(attributes, side, types, cluster = null) {
  let ret = []
  let atts =
    attributes.attribute === undefined ? attributes : attributes.attribute
  for (let i = 0; i < atts.length; i++) {
    let a = atts[i]
    if (side == dbEnum.side.both) {
      let attributeDataServer = {
        code: parseInt(normalizeHexValue(a.$.id)),
        //manufacturerCode: '', // TODO: no manuf code in dotdot xml
        name: a.$.name,
        type: a.$.type,
        side: dbEnum.side.server,
        define: a.$.name,
        min: normalizeHexValue(a.$.min),
        max: normalizeHexValue(a.$.max),
        minLength: 0,
        maxLength: null,
        isWritable: a.$.writable == 'true',
        defaultValue: normalizeHexValue(a.$.default),
        isOptional: a.$.required != 'true',
        reportingPolicy:
          a.$.reportRequired === undefined
            ? dbEnum.reportingPolicy.optional
            : a.$.reportRequired == 'true'
            ? dbEnum.reportingPolicy.mandatory
            : dbEnum.reportingPolicy.prohibited,
        isSceneRequired:
          a.$.sceneRequired == undefined ? false : a.$.sceneRequired == 'true',
      }
      let attributeDataClient = {
        code: parseInt(normalizeHexValue(a.$.id)),
        //manufacturerCode: '', // TODO: no manuf code in dotdot xml
        name: a.$.name,
        type: a.$.type,
        side: dbEnum.side.client,
        define: a.$.name,
        min: normalizeHexValue(a.$.min),
        max: normalizeHexValue(a.$.max),
        minLength: 0,
        maxLength: null,
        isWritable: a.$.writable == 'true',
        defaultValue: normalizeHexValue(a.$.default),
        isOptional: a.$.required != 'true',
        reportingPolicy:
          a.$.reportRequired === undefined
            ? dbEnum.reportingPolicy.optional
            : a.$.reportRequired == 'true'
            ? dbEnum.reportingPolicy.mandatory
            : dbEnum.reportingPolicy.prohibited,
        isSceneRequired:
          a.$.sceneRequired == undefined ? false : a.$.sceneRequired == 'true',
      }
      if (a.restriction) {
        if (a.restriction[0]['type:minLength'] != null) {
          a.minLength = a.restriction[0]['type:minLength'][0].$.value
        }
        if (a.restriction[0]['type:maxLength'] != null) {
          a.maxLength = a.restriction[0]['type:maxLength'][0].$.value
        }
      }
      ret.push(attributeDataServer)
      ret.push(attributeDataClient)
    } else {
      let attributeData = {
        code: parseInt(normalizeHexValue(a.$.id)),
        //manufacturerCode: '', // TODO: no manuf code in dotdot xml
        name: a.$.name,
        type: a.$.type,
        side: side,
        define: a.$.name,
        min: normalizeHexValue(a.$.min),
        max: normalizeHexValue(a.$.max),
        minLength: 0,
        maxLength: null,
        isWritable: a.$.writable == 'true',
        defaultValue: normalizeHexValue(a.$.default),
        isOptional: a.$.required != 'true',
        reportingPolicy:
          a.$.reportRequired === undefined
            ? dbEnum.reportingPolicy.optional
            : a.$.reportRequired == 'true'
            ? dbEnum.reportingPolicy.mandatory
            : dbEnum.reportingPolicy.prohibited,
        isSceneRequired:
          a.$.sceneRequired == undefined ? false : a.$.sceneRequired == 'true',
      }
      if (a.restriction) {
        if (a.restriction[0]['type:minLength'] != null) {
          a.minLength = a.restriction[0]['type:minLength'][0].$.value
        }
        if (a.restriction[0]['type:maxLength'] != null) {
          a.maxLength = a.restriction[0]['type:maxLength'][0].$.value
        }
      }
      ret.push(attributeData)
    }
    // TODO: Attributes have types and they may not be unique so we prepend the cluster name
    prepareAttributeType(a, types, cluster)
  }
  return ret
}

/**
 * Prepare XML commands for entry into the DB
 *
 * @param {*} commands an array of commands
 * @param {*} side the side the command is on either "client" or "server"
 * @param {*} types contained for types, where bitmaps are going to be inserted.
 * @returns Array containing all data from XML ready to be inserted in to the DB.
 */
function prepareCommands(commands, side, types) {
  let ret = []
  let cmds = commands.command === undefined ? commands : commands.command
  for (let i = 0; i < cmds.length; i++) {
    let c = cmds[i]
    env.logDebug(`Preparing command ${side} ${c.$.name}`)
    let pcmd = {
      code: parseInt(normalizeHexValue(c.$.id)),
      //manufacturerCode: '', //TODO: no manuf code for dotdot xml
      name: c.$.name,
      //description: '', // TODO: no description for dotdot xml
      source: side,
      isOptional: c.$.required != 'true',
    }
    if ('fields' in c) {
      pcmd.args = []
      c.fields.forEach((fields) => {
        let fds = fields.field === undefined ? fields : fields.field
        for (let j = 0; j < fds.length; j++) {
          let f = fds[j]
          let type = f.$.type
          if (f.bitmap != null && f.bitmap.length > 0) {
            type = `${c.$.name}${f.$.name}`
            types.bitmaps.push(prepareBitmap(types, f, true, c.$.name))
          }
          if (tagContainsEnum(f)) {
            type = `${c.$.name}${f.$.name}`
            types.enums.push(prepareEnum(f, true, c.$.name))
          }
          pcmd.args.push({
            name: f.$.name,
            type: type,
            fieldIdentifier: j + 1,
            isArray: f.$.array == 'true' ? 1 : 0,
          })
        }
      })
    }
    ret.push(pcmd)
  }
  return ret
}

/**
 * Prepare XML cluster for insertion into the database.
 * This method can also prepare clusterExtensions.
 *
 * @param {*} cluster
 * @param {*} isExtension if this is an extension or not (there are none in dotdot xml)
 * @param {*} types types object into which cluster can put types it might have
 * @returns Object containing all data from XML.
 */
function prepareCluster(cluster, types, isExtension = false) {
  let ret = {
    isExtension: isExtension,
  }

  if (isExtension) {
    // TODO: no current handling of extensions in the dotdot zcl
  } else {
    ret.code = parseInt(normalizeHexValue(cluster.$.id))
    ret.name = cluster.$.name
    //ret.description = '' // TODO: no description in dotdot zcl
    ret.define = cluster.$.name // TODO: no define in dotdot zcl
    ret.domain = cluster.classification[0].$.role
    //ret.manufacturerCode = '' // TODO: no manufacturer code in dotdot zcl
    ret.revision = cluster.$.revision
    ret.isSingleton = false // TODO: dotdot is not supporting singletons
  }
  let sides = [
    { name: 'server', value: cluster.server },
    { name: 'client', value: cluster.client },
  ]
  ret.commands = []
  ret.attributes = []
  sides.forEach((side) => {
    if (side.value !== undefined) {
      if ('attributes' in side.value[0]) {
        side.value[0].attributes.forEach((attributes) => {
          ret.attributes = ret.attributes.concat(
            prepareAttributes(attributes, side.name, types, cluster)
          )
        })
      }
      if ('commands' in side.value[0]) {
        side.value[0].commands.forEach((commands) => {
          ret.commands = ret.commands.concat(
            prepareCommands(commands, side.name, types)
          )
        })
      }
    }
  })
  // give the cluster a chance to populate types
  if ('type:type' in cluster) {
    prepareTypes(cluster['type:type'], types)
  }
  return ret
}

/**
 *
 * Parses xml type into the atomic object for insertion into the DB
 *
 * @param {*} type an xml object which conforms to the atomic format in the dotdot xml
 * @returns object ready for insertion into the DB
 */
function prepareAtomic(type) {
  let desc = type.$.name
  return {
    name: type.$.short,
    id: parseInt(normalizeHexValue(type.$.id)),
    size: getNumBytesFromShortName(type.$.short),
    description: desc,
    isDiscrete: type.$.discrete == 'true' ? true : false,
    isSigned: desc.includes('Signed'),
    isString: desc.includes('string'),
    isLong: desc.includes('string') && desc.includes('Long'),
    isChar: desc.includes('string') && desc.includes('haracter'),
  }
}

/**
 *
 * Parses xml type into the bitmap object for insertion into the DB
 *
 * @param {*} type an xml object which conforms to the bitmap format in the dotdot xml
 * @param {*} isContained a boolean indicating if this is coming from a contained tag or not
 * @returns object ready for insertion into the DB
 */
function prepareBitmap(
  typeContainer,
  type,
  isContained = false,
  namePrefix = null
) {
  let ret
  if (isContained) {
    ret = {
      //TODO: Bitmaps from clusterOrCommand attributes may not be unique by name so we prepend the clusterOrCommand
      //      name to the bitmap name (as we do in the Silabs xml)
      name: namePrefix ? namePrefix + type.$.name : type.$.name,
      type: type.$.type,
    }
  } else {
    ret = {
      name: type.$.short,
      type: type.$.inheritsFrom,
    }
  }
  if (tagContainsBitmap(type)) {
    ret.fields = []
    type.bitmap[0].element.map((e, index) => {
      ret.fields.push({
        name: e.$.name,
        mask: normalizeHexValue(e.$.mask),
        type: e.$.type,
        fieldIdentifier: index + 1,
      })
      if (tagContainsEnum(e)) {
        typeContainer.enums.push(prepareEnum(e, true, type.$.name))
      }
    })
  }
  return ret
}

/**
 *
 * Parses xml type into the enum object for insertion into the DB
 *
 * @param {*} type an xml object which conforms to the enum format in the dotdot xml
 * @returns object ready for insertion into the DB
 */
function prepareEnum(type, fromAttribute = false, namePrefix = null) {
  let ret
  if (fromAttribute) {
    ret = {
      // TODO: Enums from cluster attributes may not be unique by name so we prepend the cluster
      //       name to the enum name (as we do in the Silabs xml)
      name: namePrefix ? namePrefix + type.$.name : type.$.name,
      type: type.$.type,
    }
  } else {
    ret = {
      name: type.$.short,
      type: type.$.inheritsFrom,
    }
  }
  if ('restriction' in type) {
    ret.items = []
    type.restriction[0]['type:enumeration'].map((e, index) => {
      ret.items.push({
        name: e.$.name,
        value: parseInt(normalizeHexValue(e.$.value)),
        fieldIdentifier: index + 1,
      })
    })
  }
  return ret
}

/**
 *
 * Parses xml type into the struct object for insertion into the DB
 *
 * @param {*} type an xml object which conforms to the struct format in the dotdot xml
 * @returns object ready for insertion into the DB
 */
function prepareStruct(type) {
  let ret = { name: type.$.short }
  if ('restriction' in type) {
    ret.items = []
    type.restriction[0]['type:sequence'].map((sequence) => {
      sequence.field.map((field, index) => {
        ret.items.push({
          name: field.$.name,
          type: field.$.type,
          fieldIdentifier: index + 1,
        })
      })
    })
  }
  return ret
}

/**
 *
 * Parses xml types into the types object for insertion into the DB
 *
 * @param {*} zclTypes an array of xml types
 * @param {*} types an object which includes arrays for enums, bitmaps etc...
 */
function prepareTypes(zclTypes, types) {
  if (zclTypes == undefined) return
  let droppedTypes = []
  zclTypes.map((type) => {
    if (tagContainsBitmap(type)) {
      types.bitmaps.push(prepareBitmap(types, type))
    } else if (tagContainsEnum(type)) {
      types.enums.push(prepareEnum(type))
    } else if (tagContainsStruct(type)) {
      types.structs.push(prepareStruct(type))
    } else if (type.$.inheritsFrom === undefined) {
      types.atomics.push(prepareAtomic(type))
    } else {
      // TODO: Need to handle sub-atomic types, these are types that impose restrictions
      //       and inherit from an atomic type but are not a struct, bitmap or enum
      droppedTypes.push(type.$.name)
    }
  })
  if (droppedTypes.length > 0) {
    env.logDebug(`Dropped types in DotDot loader: ${droppedTypes}`)
  }
}

/**
 *
 * Parses xml types into the types object for insertion into the DB
 *
 * @param {*} attribute an attribute with the type in it
 * @param {*} types an object which includes arrays for enums, bitmaps etc...
 * @param {*} cluster the cluster that the attribute belongs to (used presently for uniqueness of the type name)
 */
function prepareAttributeType(attribute, types, cluster) {
  if (tagContainsBitmap(attribute)) {
    types.bitmaps.push(prepareBitmap(types, attribute, true, cluster.$.name))
  } else if (tagContainsEnum(attribute)) {
    types.enums.push(
      prepareEnum(attribute, true, cluster == null ? null : cluster.$.name)
    )
  }
}

/**
 * Preparation step for the device types.
 *
 * @param {*} deviceType
 * @returns an object containing the prepared device types.
 */
function prepareDeviceType(deviceType) {
  let ret = {
    code: deviceType.deviceId[0]['_'],
    profileId: '0x0000', //There is no profileId in Dotdot device descriptions
    domain: deviceType.domain[0],
    name: deviceType.name[0],
    description: deviceType.typeName[0],
  }

  if (!('clusters' in deviceType)) return ret

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
  return ret
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
 * @param {*} packageId
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
  // The following is when the dataType is atomic/baseline
  if (!dataType && a.name.toLowerCase().includes('map')) {
    dataTypeRef = typeMap.get(dbEnum.zclType.bitmap)
  } else if (!dataType && a.name.toLowerCase().includes(dbEnum.zclType.enum)) {
    dataTypeRef = typeMap.get(dbEnum.zclType.enum)
  } else if (
    !dataType &&
    a.name.toLowerCase().includes(dbEnum.zclType.string)
  ) {
    dataTypeRef = typeMap.get(dbEnum.zclType.string)
  } else if (
    !dataType &&
    a.name.toLowerCase().includes(dbEnum.zclType.struct)
  ) {
    dataTypeRef = typeMap.get(dbEnum.zclType.struct)
  } else if (!dataType) {
    dataTypeRef = typeMap.get(dbEnum.zclType.number)
  }
  return {
    name: a.name,
    id: parseInt(a.id),
    description: a.description ? a.description : a.name,
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
 * @param {*} data
 * @param {*} dataType
 * @returns Promise of inserted Data Types into the Data Type table.
 */
async function processDataType(db, filePath, packageId, data, dataType) {
  let typeMap = await zclLoader.getDiscriminatorMap(db, packageId)

  if (dataType == dbEnum.zclType.atomic) {
    env.logDebug(`${filePath}, ${packageId}: ${data.length} Atomic Data Types.`)
    return queryLoader.insertDataType(
      db,
      packageId,
      data.map((x) => prepareDataType(x, 0, typeMap))
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
    size: a.size,
    is_signed: a.name.endsWith('u') || !a.name.includes('int') ? 0 : 1,
    name: a.name,
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
 * @param {*} data
 * @returns Promise of inserted numbers into the number table.
 */
async function processNumber(db, filePath, packageId, data) {
  let typeMap = await zclLoader.getDiscriminatorMap(db, packageId)
  let numbers = data.filter(function (item) {
    return (
      !item.name.toLowerCase().includes(dbEnum.zclType.bitmap) &&
      !item.name.toLowerCase().includes(dbEnum.zclType.enum) &&
      !item.name.toLowerCase().includes(dbEnum.zclType.string) &&
      !item.name.toLowerCase().includes(dbEnum.zclType.struct)
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
    is_long: a.long && a.long.toLowerCase() == 'true' ? 1 : 0,
    size: a.size,
    is_char: 0,
    name: a.name,
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
 * @param {*} data
 * @returns Promise of inserted strings into the String table.
 */
async function processString(db, filePath, packageId, data) {
  let typeMap = await zclLoader.getDiscriminatorMap(db, packageId)
  let strings = data.filter(function (item) {
    return item.isString
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
function prepareEnumsOrBitmaps(a, dataType) {
  return {
    name: a.name,
    type: a.type.toLowerCase(),
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
 * @param {*} data
 * @returns A promise of inserted enums.
 */
async function processEnums(db, filePath, packageId, data) {
  env.logDebug(`${filePath}, ${packageId}: ${data.length} Enum Types.`)
  let typeMap = await zclLoader.getDiscriminatorMap(db, packageId)
  return queryLoader.insertEnum(
    db,
    [packageId],
    data.map((x) => prepareEnumsOrBitmaps(x, typeMap.get(dbEnum.zclType.enum)))
  )
}

/**
 * Processes the enum Items.
 *
 * @param {*} db
 * @param {*} filePath
 * @param {*} packageId
 * @param {*} data
 * @returns A promise of inserted enum items.
 */
async function processEnumItems(db, filePath, packageId, data) {
  env.logDebug(`${filePath}, ${packageId}: ${data.length} Enum Items.`)
  let enumItems = []
  let lastFieldId = -1
  data.forEach((e) => {
    if ('items' in e) {
      e.items.forEach((item) => {
        let defaultFieldId = lastFieldId + 1
        lastFieldId = item.fieldIdentifier
          ? parseInt(item.fieldIdentifier)
          : defaultFieldId
        enumItems.push({
          enumName: e.name,
          enumClusterCode: e.cluster ? parseInt(e.clusterCode) : null,
          name: item.name,
          value: parseInt(item.value),
          fieldIdentifier: lastFieldId,
        })
      })
    }
  })
  return queryLoader.insertEnumItems(db, packageId, [packageId], enumItems)
}

/**
 * Processes the bitmaps.
 *
 * @param {*} db
 * @param {*} filePath
 * @param {*} packageId
 * @param {*} data
 * @returns A promise of inserted bitmaps.
 */
async function processBitmaps(db, filePath, packageId, data) {
  env.logDebug(`${filePath}, ${packageId}: ${data.length} Bitmap Types.`)
  let typeMap = await zclLoader.getDiscriminatorMap(db, packageId)
  return queryLoader.insertBitmap(
    db,
    [packageId],
    data.map((x) =>
      prepareEnumsOrBitmaps(x, typeMap.get(dbEnum.zclType.bitmap))
    )
  )
}

/**
 * Processes the bitmap fields.
 *
 * @param {*} db
 * @param {*} filePath
 * @param {*} packageId
 * @param {*} data
 * @returns A promise of inserted bitmap fields.
 */
async function processBitmapFields(db, filePath, packageId, data) {
  env.logDebug(`${filePath}, ${packageId}: ${data.length} Bitmap Fields.`)
  let bitmapFields = []
  let lastFieldId = -1
  data.forEach((bm) => {
    if ('fields' in bm) {
      bm.fields.forEach((item) => {
        let defaultFieldId = lastFieldId + 1
        lastFieldId = item.fieldIdentifier
          ? parseInt(item.fieldIdentifier)
          : defaultFieldId
        bitmapFields.push({
          bitmapName: bm.name,
          bitmapClusterCode: bm.cluster ? parseInt(bm.clusterCode) : null,
          name: item.name,
          mask: parseInt(item.mask),
          fieldIdentifier: lastFieldId,
        })
      })
    }
  })
  return queryLoader.insertBitmapFields(
    db,
    packageId,
    [packageId],
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
function prepareStruct2(a, dataType) {
  return {
    name: a.name,
    cluster_code: a.cluster ? a.cluster : null,
    discriminator_ref: dataType,
  }
}

/**
 * Processes the structs.
 *
 * @param {*} db
 * @param {*} filePath
 * @param {*} packageId
 * @param {*} data
 * @returns A promise of inserted structs.
 */
async function processStruct(db, filePath, packageId, data) {
  env.logDebug(`${filePath}, ${packageId}: ${data.length} Struct Types.`)
  let typeMap = await zclLoader.getDiscriminatorMap(db, packageId)
  return queryLoader.insertStruct(
    db,
    [packageId],
    data.map((x) => prepareStruct2(x, typeMap.get(dbEnum.zclType.struct)))
  )
}

/**
 * Processes the struct Items.
 *
 * @param {*} db
 * @param {*} filePath
 * @param {*} packageId
 * @param {*} data
 * @returns A promise of inserted struct items.
 */
async function processStructItems(db, filePath, packageId, data) {
  env.logDebug(`${filePath}, ${packageId}: ${data.length} Struct Items.`)
  let structItems = []
  let lastFieldId = -1
  data.forEach((si) => {
    if ('items' in si) {
      si.items.forEach((item) => {
        let defaultFieldId = lastFieldId + 1
        lastFieldId = item.fieldIdentifier
          ? parseInt(item.fieldIdentifier)
          : defaultFieldId
        structItems.push({
          structName: si.name,
          structClusterCode: si.cluster ? parseInt(si.clusterCode) : null,
          name: item.name,
          type:
            item.type == item.type.toUpperCase() && item.type.length > 1
              ? item.type.toLowerCase()
              : item.type,
          fieldIdentifier: lastFieldId,
          minLength: 0,
          maxLength: item.length ? item.length : null,
          isWritable: item.writable == 'true',
          isArray: item.array == 'true' ? true : false,
          isEnum: item.enum == 'true' ? true : false,
          isNullable: item.isNullable == 'true' ? true : false,
          isOptional: item.optional == 'true' ? true : false,
          isFabricSensitive: item.isFabricSensitive == 'true' ? true : false,
        })
      })
    }
  })
  return queryLoader.insertStructItems(db, [packageId], structItems)
}

/**
 * Prepare enums or bitmaps for database table insertion.
 *
 * @param {*} a
 * @param {*} dataType
 * @returns An Object
 */
function prepareEnumsOrBitmapsAtomic(a, dataType) {
  return {
    size: a.size,
    name: a.name,
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
 * @param {*} data
 * @returns A promise of inserted enums.
 */
async function processEnumsFromAtomics(db, filePath, packageId, data) {
  let typeMap = await zclLoader.getDiscriminatorMap(db, packageId)
  let enums = data.filter(function (item) {
    return item.name.toLowerCase().includes(dbEnum.zclType.enum)
  })
  env.logDebug(`${filePath}, ${packageId}: ${data.length} Baseline Enum Types.`)
  return queryLoader.insertEnumAtomic(
    db,
    packageId,
    enums.map((x) =>
      prepareEnumsOrBitmapsAtomic(x, typeMap.get(dbEnum.zclType.enum))
    )
  )
}

/**
 * Processes the bitmaps.
 *
 * @param {*} db
 * @param {*} filePath
 * @param {*} packageId
 * @param {*} data
 * @returns A promise of inserted bitmaps.
 */
async function processBitmapsFromAtomics(db, filePath, packageId, data) {
  let typeMap = await zclLoader.getDiscriminatorMap(db, packageId)
  let bitmaps = data.filter(function (item) {
    return item.name.toLowerCase().includes('map')
  })
  env.logDebug(
    `${filePath}, ${packageId}: ${data.length} Baseline Bitmap Types.`
  )
  return queryLoader.insertBitmapAtomic(
    db,
    packageId,
    bitmaps.map((x) =>
      prepareEnumsOrBitmapsAtomic(x, typeMap.get(dbEnum.zclType.bitmap))
    )
  )
}

/**
 *
 * Promises to iterate over all the XML files and returns an aggregate promise
 * that will be resolved when all the XML files are done, or rejected if at least one fails.
 *
 * @param {*} db
 * @param {*} ctx
 * @returns Promise that resolves when all the individual promises of each file pass.
 */
async function loadZclData(db, ctx) {
  env.logDebug(
    `Starting to load Dotdot ZCL data in to DB for: ${ctx.metadataFile}, clusters length=${ctx.zclClusters.length}`
  )
  let types = { atomics: [], enums: [], bitmaps: [], structs: [] }
  prepareTypes(ctx.zclTypes, types)
  prepareTypes(ctx.zclGlobalTypes, types)
  let preparedClusters = []
  ctx.zclClusters.forEach((cluster) => {
    env.logDebug(`loading cluster: ${cluster.$.name}`)
    preparedClusters.push(prepareCluster(cluster, types))
  })
  // Global attributes don't have a side listed, so they have to be looped through once for each side
  let gas = []
  ctx.zclGlobalAttributes.forEach((a) => {
    let pa = prepareAttributes([a], dbEnum.side.both, types)
    gas = gas.concat(pa)
  })
  let gs = [
    {
      attributes: gas,
      commands: prepareCommands(ctx.zclGlobalCommands, '', types),
    },
  ]
  let ds = []
  ctx.zclDeviceTypes.forEach((deviceType) => {
    env.logDebug(`loading device: ${deviceType.typeName[0]}`)
    let d = prepareDeviceType(deviceType)
    ds.push(d)
  })
  await queryLoader.insertClusters(db, ctx.packageId, preparedClusters)

  await queryPackage.insertOptionsKeyValues(
    db,
    ctx.packageId,
    dbEnum.packageOptionCategory.manufacturerCodes,
    ctx.zclManufacturers.map((data) => {
      let mfgPair = data['$']
      return { code: mfgPair['code'], label: mfgPair['translation'] }
    })
  )
  await queryLoader.insertDeviceTypes(db, ctx.packageId, ds)
  await queryLoader.insertGlobals(db, ctx.packageId, gs)
  await queryLoader.insertAtomics(db, ctx.packageId, types.atomics)
  ctx.ZCLDataTypes = ['ARRAY', 'BITMAP', 'ENUM', 'NUMBER', 'STRING', 'STRUCT']
  await processDataTypeDiscriminator(db, ctx.packageId, ctx.ZCLDataTypes)
  await processDataType(
    db,
    ctx.metadataFile,
    ctx.packageId,
    types.atomics,
    dbEnum.zclType.atomic
  )
  await processDataType(
    db,
    ctx.metadataFile,
    ctx.packageId,
    types.enums,
    dbEnum.zclType.enum
  )
  await processDataType(
    db,
    ctx.metadataFile,
    ctx.packageId,
    types.bitmaps,
    dbEnum.zclType.bitmap
  )
  await processDataType(
    db,
    ctx.metadataFile,
    ctx.packageId,
    types.structs,
    dbEnum.zclType.struct
  )

  await processEnumsFromAtomics(
    db,
    ctx.metadataFile,
    ctx.packageId,
    types.atomics
  )
  await processBitmapsFromAtomics(
    db,
    ctx.metadataFile,
    ctx.packageId,
    types.atomics
  )
  await processNumber(db, ctx.metadataFile, ctx.packageId, types.atomics)
  await processString(db, ctx.metadataFile, ctx.packageId, types.atomics)
  await processEnums(db, ctx.metadataFile, ctx.packageId, types.enums)
  await processBitmaps(db, ctx.metadataFile, ctx.packageId, types.bitmaps)
  await processStruct(db, ctx.metadataFile, ctx.packageId, types.structs)

  await processEnumItems(db, ctx.metadataFile, ctx.packageId, types.enums)
  await processBitmapFields(db, ctx.metadataFile, ctx.packageId, types.bitmaps)
  return processStructItems(db, ctx.metadataFile, ctx.packageId, types.structs)
}

/**
 * TODO This is not supported at this time.
 * @param {*} db
 * @param {*} filePath
 * @return {*} object w/ following: { packageId: pkgId } or { err: err }
 */
async function loadIndividualDotDotFile(db, filePath) {
  let content = await fsp.readFile(filePath)
  console.log(`UNSUPPORTED: ${content}`)
}

/**
 * Toplevel function that loads the xml library file
 * and orchestrates the promise chain.
 *
 * @export
 * @param {*} db
 * @param {*} ctx Context of loading.
 * @returns a Promise that resolves with the db.
 */
async function loadDotdotZcl(db, metafile) {
  let ctx = {
    metadataFile: metafile,
    db: db,
  }
  env.logDebug(`Loading Dotdot zcl file: ${metafile}`)
  try {
    await dbApi.dbBeginTransaction(db)
    Object.assign(ctx, await util.readFileContentAndCrc(ctx.metadataFile))
    ctx.packageId = await zclLoader.recordToplevelPackage(
      db,
      ctx.metadataFile,
      ctx.crc
    )
    await collectDataFromLibraryXml(ctx)
    if (ctx.version != null) {
      await zclLoader.recordVersion(
        db,
        ctx.packageId,
        ctx.version,
        ctx.category,
        ctx.description
      )
    }
    await parseZclFiles(db, ctx)
    await loadZclData(db, ctx)
    await zclLoader.processZclPostLoading(db, ctx.packageId)
  } catch (err) {
    env.logError(err)
    throw err
  } finally {
    await dbApi.dbCommit(db)
  }
  return ctx
}

exports.loadDotdotZcl = loadDotdotZcl
exports.loadIndividualDotDotFile = loadIndividualDotDotFile
