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

const queryAccess = require('../db/query-access')
const templateUtil = require('./template-util')
const dbEnum = require('../../src-shared/db-enum')

/**
 * This module contains the API for templating. For more detailed instructions, read {@tutorial template-tutorial}
 *
 * @module Templating API: Access helpers
 */

async function collectDefaultAccessList(ctx, entityType) {
  let packageIds = await templateUtil.ensureZclPackageIds(ctx)
  let defaultAccess = await queryAccess.selectDefaultAccess(
    ctx.global.db,
    packageIds,
    entityType
  )
  return defaultAccess
}

async function collectAccesslist(ctx, options) {
  let entityType = null
  let includeDefault = true

  if ('entity' in options.hash) {
    entityType = options.hash.entity
  } else {
    entityType = ctx.entityType
  }

  if ('includeDefault' in options.hash) {
    includeDefault = options.hash.includeDefault == 'true'
  }

  if (entityType == null) {
    throw new Error(
      'Access helper requires entityType, either from context, or from the entity="<entityType>" option.'
    )
  }

  let accessList

  switch (entityType) {
    case 'attribute':
      accessList = await queryAccess.selectAttributeAccess(
        ctx.global.db,
        ctx.id
      )
      break
    case 'command':
      accessList = await queryAccess.selectCommandAccess(ctx.global.db, ctx.id)
      break
    case 'event':
      accessList = await queryAccess.selectEventAccess(ctx.global.db, ctx.id)
      break
    default:
      throw new Error(
        `Entity type ${entityType} not supported. Requires: attribute/command/event.`
      )
  }

  if (includeDefault) {
    let defaultAccess = await collectDefaultAccessList(ctx, entityType)
    accessList.push(...defaultAccess)
  }

  return accessList
}

/**
 * This helper creates a context for the aggregates of access.
 *
 * @param {*} options
 */
async function access_aggregate(options) {
  let packageIds = await templateUtil.ensureZclPackageIds(this)
  let accessList = await collectAccesslist(this, options)
  let ignoreEmpty
  if ('ignoreEmpty' in options.hash) {
    ignoreEmpty = options.hash.ignoreEmpty == 'true'
  } else {
    ignoreEmpty = false
  }

  let allOps = await queryAccess.selectAccessOperations(
    this.global.db,
    packageIds
  )
  let allMods = await queryAccess.selectAccessModifiers(
    this.global.db,
    packageIds
  )
  let allRoles = await queryAccess.selectAccessRoles(this.global.db, packageIds)
  let roleLevels = {}
  allRoles.forEach((r) => {
    roleLevels[r.name] = r.level
  })

  let aggregate = {
    count: accessList.length,
  }

  allOps.forEach((r) => {
    aggregate[r.name + 'Highest'] = 'NONE'
    aggregate[r.name + 'Lowest'] = 'NONE'
  })
  allMods.forEach((r) => {
    aggregate[r.name] = false
  })

  accessList.forEach((a) => {
    let role = a.role
    let operation = a.operation
    let accessModifier = a.accessModifier
    if (accessModifier != null) {
      aggregate[accessModifier] = true
    }
    if (role != null) {
      if (aggregate[operation + 'Highest'] === 'NONE') {
        aggregate[operation + 'Highest'] = role
        aggregate[operation + 'Lowest'] = role
      } else {
        let highestRole = aggregate[operation + 'Highest']
        let lowestRole = aggregate[operation + 'Lowest']
        if (roleLevels[role] < roleLevels[lowestRole]) {
          aggregate[operation + 'Lowest'] = role
        }
        if (roleLevels[role] > roleLevels[highestRole]) {
          aggregate[operation + 'Highest'] = role
        }
      }
    }
  })

  let blocks
  if (ignoreEmpty && aggregate.count == 0) {
    blocks = []
  } else {
    blocks = [aggregate]
  }
  let p = templateUtil.collectBlocks(blocks, options, this)
  return templateUtil.templatePromise(this.global, p)
}

/**
 * Access helper iterates across all the access triplets associated with the element.
 * For each element, context contains role, operation, accessModifier.
 * Additionally it creates booleans hasRole, hasOperation and hasAccessModifier
 * and hasAtLeastOneAccessElement and hasAllAccessElements
 * @param {*} options
 */
async function access(options) {
  let accessList = await collectAccesslist(this, options)

  accessList.forEach((element) => {
    element.hasRole = element.role != null && element.role.length > 0
    element.hasOperation =
      element.operation != null && element.operation.length > 0
    element.hasAccessModifier =
      element.accessModifier != null && element.accessModifier.length > 0
    element.hasAllAccessElements =
      element.hasRole && element.hasOperation && element.hasAccessModifier
    element.hasAtLeastOneAccessElement =
      element.hasRole || element.hasOperation || element.hasAccessModifier
  })

  let p = templateUtil.collectBlocks(accessList, options, this)
  return templateUtil.templatePromise(this.global, p)
}

async function default_access(options) {
  let entityType = null

  if ('entity' in options.hash) {
    entityType = options.hash.entity
  } else {
    entityType = ctx.entityType
  }

  if (entityType == null) {
    throw new Error(
      'Access helper requires entityType, either from context, or from the entity="<entityType>" option.'
    )
  }

  let accessList = await collectDefaultAccessList(this, entityType)
  accessList.forEach((element) => {
    element.hasRole = element.role != null && element.role.length > 0
    element.hasOperation =
      element.operation != null && element.operation.length > 0
    element.hasAccessModifier =
      element.accessModifier != null && element.accessModifier.length > 0
    element.hasAllAccessElements =
      element.hasRole && element.hasOperation && element.hasAccessModifier
    element.hasAtLeastOneAccessElement =
      element.hasRole || element.hasOperation || element.hasAccessModifier
  })

  let p = templateUtil.collectBlocks(accessList, options, this)
  return templateUtil.templatePromise(this.global, p)
}

exports.access = access
exports.access_aggregate = access_aggregate
exports.default_access = default_access
