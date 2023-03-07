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
const path = require('path')
const queryPackage = require('../db/query-package')
const queryCommand = require('../db/query-command')
const queryLoader = require('../db/query-loader')
const dbEnum = require('../../src-shared/db-enum')
const fsp = fs.promises
const sLoad = require('./zcl-loader-silabs')
const dLoad = require('./zcl-loader-dotdot')
const queryZcl = require('../db/query-zcl')
const queryDeviceType = require('../db/query-device-type')
const env = require('../util/env')
const nativeRequire = require('../util/native-require')
const util = require('../util/util')

const defaultValidator = (zclData) => {
  return []
}

/**
 * Records the toplevel package information and resolves into packageId
 * @param {*} db
 * @param {*} metadataFile
 * @param {*} crc
 * @returns packageId
 */
async function recordToplevelPackage(db, metadataFile, crc) {
  return queryPackage.registerTopLevelPackage(
    db,
    metadataFile,
    crc,
    dbEnum.packageType.zclProperties
  )
}

/**
 * Records the version into the database.
 *
 * @param {*} db
 * @param {*} ctx
 */
async function recordVersion(db, packageId, version, category, description) {
  return queryPackage.updateVersion(
    db,
    packageId,
    version,
    category,
    description
  )
}

/**
 * Toplevel function that loads the zcl file and passes it off to the correct zcl loader.
 *
 * @export
 * @param {*} db
 * @param {*} metadataFile array of paths
 * @returns Array of loaded packageIds.
 */
async function loadZclMetafiles(db, metadataFiles) {
  let packageIds = []
  if (Array.isArray(metadataFiles)) {
    for (let f of metadataFiles) {
      let ctx = await loadZcl(db, f)
      packageIds.push(ctx.packageId)
    }
  } else {
    let ctx = await loadZcl(db, metadataFiles)
    packageIds.push(ctx.packageId)
  }
  return packageIds
}

/**
 * Loads individual zcl.json metafile.
 *
 * @param {*} db
 * @param {*} metadataFile
 * @returns Context object that contains .db and .packageId
 */
async function loadZcl(db, metadataFile) {
  let ext = path.extname(metadataFile)
  let resolvedMetafile = path.resolve(metadataFile)

  try {
    await fsp.access(resolvedMetafile, fs.constants.R_OK)
  } catch {
    throw new Error(`Can't access file: ${metadataFile}`)
  }
  if (ext == '.xml') {
    return dLoad.loadDotdotZcl(db, resolvedMetafile)
  } else if (ext == '.properties') {
    return sLoad.loadSilabsZcl(db, resolvedMetafile, false)
  } else if (ext == '.json') {
    return sLoad.loadSilabsZcl(db, resolvedMetafile, true)
  } else {
    throw new Error(`Unknown zcl metafile type: ${metadataFile}`)
  }
}

/**
 * Load individual custom XML files.
 *
 * @param {*} db
 * @param {*} filePath
 * @param {*} sessionId
 */
async function loadIndividualFile(db, filePath, sessionId) {
  let zclPropertiesPackages = await queryPackage.getSessionPackagesByType(
    db,
    sessionId,
    dbEnum.packageType.zclProperties
  )

  let validator
  if (zclPropertiesPackages.length == 0) {
    env.logDebug(`Unable to find a validator for project, skipping validator`)
    // Return an function that returns an empty array
    validator = defaultValidator
  } else {
    validator = await bindValidationScript(db, zclPropertiesPackages[0].id)
  }

  let ext = path.extname(filePath)
  if (ext == '.xml') {
    return sLoad.loadIndividualSilabsFile(db, filePath, validator, sessionId)
  } else {
    let err = new Error(
      `Unable to read file with unknown extension: ${filePath}`
    )
    env.logWarning(err)
    return { succeeded: false, err }
  }
}

/**
 * This function creates a validator function with signatuee fn(stringToValidateOn)
 *
 * @param {*} db
 * @param {*} basePackageId
 */
async function bindValidationScript(db, basePackageId) {
  try {
    let data = await getSchemaAndValidationScript(db, basePackageId)

    if (
      !(dbEnum.packageType.zclSchema in data) ||
      !(dbEnum.packageType.zclValidation in data)
    ) {
      return defaultValidator
    } else {
      let zclSchema = data[dbEnum.packageType.zclSchema]
      let zclValidation = data[dbEnum.packageType.zclValidation]
      let module = nativeRequire(zclValidation)
      let validateZclFile = module.validateZclFile

      env.logDebug(`Reading individual file: ${zclSchema}`)
      let schemaFileContent = await fsp.readFile(zclSchema)
      return validateZclFile.bind(null, schemaFileContent)
    }
  } catch (err) {
    env.logError(`Error loading package specific validator: ${err}`)
    return defaultValidator
  }
}

/**
 * Returns an object with zclSchema and zclValidation elements.
 * @param {*} db
 * @param {*} basePackageId
 */
async function getSchemaAndValidationScript(db, basePackageId) {
  let promises = []
  promises.push(
    queryPackage.getPackagesByParentAndType(
      db,
      basePackageId,
      dbEnum.packageType.zclSchema
    )
  )
  promises.push(
    queryPackage.getPackagesByParentAndType(
      db,
      basePackageId,
      dbEnum.packageType.zclValidation
    )
  )
  let data = await Promise.all(promises)
  return data.reduce((result, item) => {
    if (item.length >= 1) {
      result[item[0].type] = item[0].path
    }
    return result
  }, {})
}

/**
 * Promises to qualify whether zcl file needs to be reloaded.
 * If yes, the it will resolve with {filePath, data, packageId}
 * If not, then it will resolve with {error}
 *
 * @param {*} db
 * @param {*} info
 * @param {*} parentPackageId
 * @returns Promise that resolves int he object of data.
 */
async function qualifyZclFile(
  db,
  info,
  parentPackageId,
  packageType,
  isCustom
) {
  let filePath = info.filePath
  let data = info.data
  let actualCrc = info.crc

  let pkg = await queryPackage.getPackageByPathAndParent(
    db,
    filePath,
    parentPackageId,
    isCustom
  )

  if (pkg == null) {
    // This is executed if there is no CRC in the database.
    env.logDebug(`No CRC in the database for file ${filePath}, parsing.`)
    let packageId = await queryPackage.insertPathCrc(
      db,
      filePath,
      actualCrc,
      packageType,
      parentPackageId
    )
    return {
      filePath: filePath,
      data: data,
      packageId: parentPackageId == null ? packageId : parentPackageId,
    }
  } else {
    // This is executed if CRC is found in the database.
    if (pkg.crc == actualCrc) {
      env.logDebug(
        `CRC match for file ${pkg.path} (${pkg.crc}), skipping parsing.`
      )
      return {
        error: `${pkg.path} skipped`,
        packageId: pkg.id,
      }
    } else {
      env.logDebug(
        `CRC missmatch for file ${pkg.path}, (${pkg.crc} vs ${actualCrc}) package id ${pkg.id}, parsing.`
      )
      await queryPackage.updatePathCrc(db, filePath, actualCrc, parentPackageId)
      return {
        filePath: filePath,
        data: data,
        packageId: parentPackageId == null ? packageId : parentPackageId,
      }
    }
  }
}

/**
 * Promises to perform a post loading step.
 *
 * @param {*} db
 * @returns Promise to deal with the post-loading cleanup.
 */
async function processZclPostLoading(db, packageId) {
  // These queries must make sure that they update ONLY the entities under a given packageId.
  await queryLoader.updateDataTypeClusterReferences(db, packageId)
  await queryDeviceType.updateDeviceTypeEntityReferences(db, packageId)
  return queryCommand.updateCommandRequestResponseReferences(db, packageId)
}

/**
 *
 * @param {*} db
 * @param {*} packageIds
 * @returns data type discriminator map
 */
async function getDiscriminatorMap(db, packageIds) {
  let typeMap = new Map()
  let discriminators = await queryZcl.selectAllDiscriminators(db, packageIds)
  discriminators.forEach((d) => {
    typeMap.set(d.name.toLowerCase(), d.id)
  })
  return typeMap
}

exports.loadZcl = loadZcl
exports.loadZclMetafiles = loadZclMetafiles
exports.recordToplevelPackage = recordToplevelPackage
exports.recordVersion = recordVersion
exports.processZclPostLoading = processZclPostLoading
exports.loadIndividualFile = loadIndividualFile
exports.qualifyZclFile = qualifyZclFile
exports.getDiscriminatorMap = getDiscriminatorMap
