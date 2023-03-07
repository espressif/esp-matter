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

const queryZcl = require('../db/query-zcl')
const queryDeviceType = require('../db/query-device-type')
const queryCommand = require('../db/query-command')
const queryEvent = require('../db/query-event')
const dbEnum = require('../../src-shared/db-enum')
const templateUtil = require('./template-util')
const helperC = require('./helper-c')
const env = require('../util/env')
const types = require('../util/types')
const zclUtil = require('../util/zcl-util')
const _ = require('lodash')

const characterStringTypes = ['CHAR_STRING', 'LONG_CHAR_STRING']
const octetStringTypes = ['OCTET_STRING', 'LONG_OCTET_STRING']
const stringShortTypes = ['CHAR_STRING', 'OCTET_STRING']
const stringLongTypes = ['LONG_CHAR_STRING', 'LONG_OCTET_STRING']

/**
 * This module contains the API for templating. For more detailed instructions, read {@tutorial template-tutorial}
 *
 * @module Templating API: static zcl helpers
 */

/**
 * Block helper iterating over all bitmaps.
 *
 * @param {*} options
 * @returns Promise of content.
 */
async function zcl_bitmaps(options) {
  let packageIds = await templateUtil.ensureZclPackageIds(this)
  let ens
  if (this.id != null) {
    ens = await Promise.all(
      packageIds.map((packageId) =>
        queryZcl.selectClusterBitmaps(this.global.db, packageId, this.id)
      )
    ).then((x) => x.flat())
  } else {
    ens = await Promise.all(
      packageIds.map((packageId) =>
        queryZcl.selectAllBitmaps(this.global.db, packageId)
      )
    ).then((x) => x.flat())
  }
  let promise = templateUtil.collectBlocks(ens, options, this)
  return templateUtil.templatePromise(this.global, promise)
}

/**
 * Iterates over enum items. Valid only inside zcl_enums.
 * @param {*} options
 */
function zcl_bitmap_items(options) {
  let promise = queryZcl
    .selectAllBitmapFieldsById(this.global.db, this.id)
    .then((items) => templateUtil.collectBlocks(items, options, this))
  return templateUtil.templatePromise(this.global, promise)
}

/**
 * Block helper iterating over all enums.
 * If existing independently, it iterates over ALL the enums.
 * Within a context of a cluster, it iterates only over the
 * enums belonging to a cluster.
 *
 * @param {*} options
 * @returns Promise of content.
 */
async function zcl_enums(options) {
  let packageIds = await templateUtil.ensureZclPackageIds(this)
  let ens
  if (this.id != null) {
    ens = await queryZcl.selectClusterEnums(this.global.db, packageIds, this.id)
  } else {
    ens = await Promise.all(
      packageIds.map((packageId) =>
        queryZcl
          .selectAllEnums(this.global.db, packageId)
          //Filtering out all atomic enums
          .then((es) =>
            es.filter((e) => !e.name.toLowerCase().match(/^enum\d+$/g))
          )
      )
    ).then((x) => x.flat())
  }
  let promise = templateUtil.collectBlocks(ens, options, this)
  return templateUtil.templatePromise(this.global, promise)
}

/**
 * Block helper iterating over all structs.
 * If existing independently, it iterates over ALL the structs.
 * Within a context of a cluster, it iterates only over the
 * structs belonging to a cluster.
 *
 * @param {*} options
 * @returns Promise of content.
 */
async function zcl_structs(options) {
  let checkForDoubleNestedArray =
    options.hash.checkForDoubleNestedArray == 'true'
  let packageIds = await templateUtil.ensureZclPackageIds(this)
  let structs
  if (this.id != null) {
    structs = await queryZcl.selectClusterStructsWithItems(
      this.global.db,
      packageIds,
      this.id
    )
  } else {
    structs = await queryZcl.selectAllStructsWithItems(
      this.global.db,
      packageIds
    )
  }
  structs = await zclUtil.sortStructsByDependency(structs)
  structs.forEach((st) => {
    st.struct_contains_array = false
    st.struct_has_fabric_sensitive_fields = false
    st.has_no_clusters = st.struct_cluster_count < 1
    st.has_one_cluster = st.struct_cluster_count == 1
    st.has_more_than_one_cluster = st.struct_cluster_count > 1
    st.items.forEach((i) => {
      if (i.isArray) {
        st.struct_contains_array = true
      }
      if (i.isFabricSensitive) {
        st.struct_has_fabric_sensitive_fields = true
      }
    })
  })
  if (checkForDoubleNestedArray) {
    // If this is set to true in a template, then we populate the
    // struct_contains_nested_array variable with true
    // if struct contains array of structs that contain an array.
    // There is a hit in processing if you turn this on, so it's not automatic.
    for (const st of structs) {
      st.struct_contains_nested_array = false
      for (const i of st.items) {
        if (i.isArray) {
          // Found an array. Now let's check if it points to a struct that also contains an array.
          let sis = await queryZcl.selectAllStructItemsByStructName(
            this.global.db,
            i.type,
            packageIds
          )
          if (sis.length > 0) {
            for (const ss of sis) {
              if (ss.isArray) {
                st.struct_contains_nested_array = true
              }
            }
          }
        }
      }
    }
  }
  let promise = templateUtil.collectBlocks(structs, options, this)
  return templateUtil.templatePromise(this.global, promise)
}

/**
 * Iterates over enum items. Valid only inside zcl_enums.
 * @param {*} options
 */
async function zcl_enum_items(options) {
  let items
  if (this.enum_items) {
    items = this.enum_items
  } else {
    items = await queryZcl.selectAllEnumItemsById(this.global.db, this.id)
    this.enum_items = items
  }
  return templateUtil.templatePromise(
    this.global,
    templateUtil.collectBlocks(items, options, this)
  )
}

/**
 * This helper prints out the first unused enum value.
 * It supports mode="next_larger" and
 * mode="first_unused" (which is the default).
 *
 * @param {*} options
 * @returns the unused enum value
 */
async function first_unused_enum_value(options) {
  let mode = options.hash.mode
  let format = options.hash.format // "hex" or "decimal"
  let items
  if (this.enum_items) {
    items = this.enum_items
  } else {
    items = await queryZcl.selectAllEnumItemsById(this.global.db, this.id)
    this.enum_items = items
  }
  let unusedValue
  if (mode == 'next_larger') {
    // Find the lowest unused integer, larger than all enum values
    unusedValue = 0
    for (let enumItem of items) {
      if (enumItem.value > unusedValue) {
        unusedValue = enumItem.value
      }
    }
    unusedValue += 1
  } else {
    unusedValue = -1
    let isPresent
    do {
      unusedValue++
      isPresent = false
      for (let enumItem of items) {
        if (enumItem.value == unusedValue) {
          isPresent = true
        }
      }
    } while (isPresent)
  }

  if (format == 'hex') {
    let out = unusedValue.toString(16)
    if (this.size && this.size > 0) {
      while (out.length < this.size * 2) {
        out = '0' + out
      }
    }
    return out
  } else {
    return unusedValue
  }
}

/**
 * Block helper iterating over all struct items. Valid only inside zcl_structs.

 * @param {*} options
 * @returns Promise of content.
 */
async function zcl_struct_items(options) {
  let checkForDoubleNestedArray =
    options.hash.checkForDoubleNestedArray == 'true'
  let packageIds = await templateUtil.ensureZclPackageIds(this)
  let sis = await queryZcl.selectAllStructItemsById(this.global.db, this.id)
  if (checkForDoubleNestedArray) {
    for (const si of sis) {
      si.struct_item_contains_nested_array = false
      // For each item, let's check if it's a struct itself
      let structItems = await queryZcl.selectAllStructItemsByStructName(
        this.global.db,
        si.type,
        packageIds
      )
      if (structItems.length > 0) {
        for (const s of structItems) {
          if (s.isArray) si.struct_item_contains_nested_array = true
        }
      }
    }
  }
  let promise = templateUtil.collectBlocks(sis, options, this)
  return templateUtil.templatePromise(this.global, promise)
}

/**
 * Block helper iterating over all struct items given the struct name.
 *
 * @param name
 * @param options
 * @returns Promise of content.
 */
async function zcl_struct_items_by_struct_name(name, options) {
  let packageIds = await templateUtil.ensureZclPackageIds(this)
  let promise = queryZcl
    .selectAllStructItemsByStructName(this.global.db, name, packageIds)
    .then((st) => templateUtil.collectBlocks(st, options, this))
  return templateUtil.templatePromise(this.global, promise)
}

/**
 * Block helper iterating over all deviceTypes.
 *
 * @param {*} options
 * @returns Promise of content.
 */
async function zcl_device_types(options) {
  let packageIds = await templateUtil.ensureZclPackageIds(this)
  let deviceTypes = await Promise.all(
    packageIds.map((packageId) =>
      queryDeviceType.selectAllDeviceTypes(this.global.db, packageId)
    )
  )
  let promise = templateUtil.collectBlocks(deviceTypes.flat(), options, this)
  return templateUtil.templatePromise(this.global, promise)
}

/**
 * Block helper for use inside zcl_device_types
 *
 * @param {*} options
 * @returns blocks for clusters
 */
async function zcl_device_type_clusters(options) {
  let clusters = await queryDeviceType.selectDeviceTypeClustersByDeviceTypeRef(
    this.global.db,
    this.id
  )

  let promise = templateUtil.collectBlocks(clusters, options, this)
  return templateUtil.templatePromise(this.global, promise)
}

/**
 * Block helper for use inside zcl_device_type_clusters
 *
 * @param {*} options
 * @returns blocks for commands
 */
async function zcl_device_type_cluster_commands(options) {
  let deviceTypeClusterId = this.id
  let commands = await queryDeviceType.selectDeviceTypeCommandsByDeviceTypeRef(
    this.global.db,
    deviceTypeClusterId
  )
  let promise = templateUtil.collectBlocks(commands, options, this)
  return templateUtil.templatePromise(this.global, promise)
}

/**
 * Block helper for use inside zcl_device_type_clusters
 *
 * @param {*} options
 * @returns blocks for attributes
 */
async function zcl_device_type_cluster_attributes(options) {
  let deviceTypeClusterId = this.id
  let attributes =
    await queryDeviceType.selectDeviceTypeAttributesByDeviceTypeRef(
      this.global.db,
      deviceTypeClusterId
    )
  let promise = templateUtil.collectBlocks(attributes, options, this)
  return templateUtil.templatePromise(this.global, promise)
}

/**
 * Block helper iterating over all clusters.
 *
 * @param {*} options
 * @returns Promise of content.
 */
async function zcl_clusters(options) {
  let packageIds = await templateUtil.ensureZclPackageIds(this)
  let cl = await Promise.all(
    packageIds.map((id) => queryZcl.selectAllClusters(this.global.db, id))
  )
  let promise = templateUtil.collectBlocks(cl.flat(), options, this)
  return templateUtil.templatePromise(this.global, promise)
}

/**
 * Block helper iterating over all commands.
 * There are two modes of this helper:
 *   when used in a global context, it iterates over ALL commands in the database.
 *   when used inside a `zcl_cluster` block helper, it iterates only over the commands for that cluster.
 *
 * @param {*} options
 * @returns Promise of content.
 */
function zcl_commands(options) {
  let promise = templateUtil
    .ensureZclPackageIds(this)
    .then((packageIds) => {
      if ('id' in this) {
        // We're functioning inside a nested context with an id, so we will only query for this cluster.
        return queryCommand.selectCommandsByClusterId(
          this.global.db,
          this.id,
          packageIds
        )
      } else {
        return queryCommand.selectAllCommands(this.global.db, packageIds)
      }
    })
    .then((cmds) => templateUtil.collectBlocks(cmds, options, this))
  return templateUtil.templatePromise(this.global, promise)
}

/**
 * Block helper iterating over all commands with cluster information.
 * Note: Similar to zcl_commands but has cluster information as well.
 * @param {*} options
 * @returns Promise of content.
 */
async function zcl_commands_with_cluster_info(options) {
  let packageIds = await templateUtil.ensureZclPackageIds(this)
  let cmds = await queryCommand.selectAllCommandsWithClusterInfo(
    this.global.db,
    packageIds
  )
  let promise = templateUtil.collectBlocks(cmds, options, this)
  return templateUtil.templatePromise(this.global, promise)
}

/**
 * Helper that retrieves all commands that contain arguments.
 *
 * @param {*} options
 */
async function zcl_commands_with_arguments(options) {
  let sortBy = options.hash.sortBy
  let packageIds = await templateUtil.ensureZclPackageIds(this)

  let cmds = await Promise.all(
    packageIds.map(async (packageId) => {
      let cmdsPerPackageId = await queryCommand.selectAllCommandsWithArguments(
        this.global.db,
        packageId
      )
      if ('signature' == sortBy) {
        for (const cmd of cmdsPerPackageId) {
          let sig = await zclUtil.createCommandSignature(
            this.global.db,
            packageId,
            cmd
          )
          cmd.signature = sig.signature
          cmd.isSignatureSimple = sig.isSimple
        }
        cmdsPerPackageId.sort((a, b) => {
          if (a.isSignatureSimple && !b.isSignatureSimple) return -1
          if (!a.isSignatureSimple && b.isSignatureSimple) return 1
          return a.signature.localeCompare(b.signature)
        })
      }

      return cmdsPerPackageId
    })
  ).then((x) => x.flat())

  let promise = templateUtil.collectBlocks(cmds, options, this)
  return templateUtil.templatePromise(this.global, promise)
}

/**
 * Block helper iterating over all commands based on the source.
 * There are two modes of this helper:
 *   when used in a global context, it iterates over ALL commands in the database based on the source.
 *   when used inside a `zcl_cluster` block helper, it iterates only over the source commands for that cluster.
 *
 * @param {*} options
 * @param {*} source
 * @returns Promise of content.
 * @ignore
 */
function zcl_commands_by_source(options, source) {
  let promise = templateUtil
    .ensureZclPackageIds(this) // leaving to packageId since not used in template.
    .then((packageIds) => {
      if ('id' in this) {
        // We're functioning inside a nested context with an id, so we will only query for this cluster.
        return queryCommand.selectCommandsByClusterIdAndSource(
          this.global.db,
          this.id,
          source,
          packageIds
        )
      } else {
        return queryCommand.selectAllCommandsBySource(
          this.global.db,
          source,
          packageIds
        )
      }
    })
    .then((cmds) => templateUtil.collectBlocks(cmds, options, this))
  return templateUtil.templatePromise(this.global, promise)
}

/**
 * Block helper iterating over all client commands.
 * There are two modes of this helper:
 *   when used in a global context, it iterates over ALL client commands in the database.
 *   when used inside a `zcl_cluster` block helper, it iterates only over the commands for that client cluster.
 *
 * @param {*} options
 * @returns Promise of content.
 */
function zcl_commands_source_client(options) {
  return zcl_commands_by_source.bind(this)(options, dbEnum.source.client)
}

/**
 * Block helper iterating over all server commands.
 * There are two modes of this helper:
 *   when used in a global context, it iterates over ALL server commands in the database.
 *   when used inside a `zcl_cluster` block helper, it iterates only over the commands for that server cluster.
 *
 * @param {*} options
 * @returns Promise of content.
 */
function zcl_commands_source_server(options) {
  return zcl_commands_by_source.bind(this)(options, dbEnum.source.server)
}

/**
 * Block helper iterating over all events.
 * There are two modes of this helper:
 *   when used in a global context, it iterates over ALL events in the database.
 *   when used inside a `zcl_cluster` block helper, it iterates only over the events for that cluster.
 *
 * @param {*} options
 * @returns Promise of content.
 */
async function zcl_events(options) {
  let packageIds = await templateUtil.ensureZclPackageIds(this)
  let events
  if ('id' in this) {
    // We're functioning inside a nested context with an id, so we will only query for this cluster.
    events = await queryEvent.selectEventsByClusterId(
      this.global.db,
      this.id,
      packageIds
    )
  } else {
    events = await queryEvent.selectAllEvents(this.global.db, packageIds)
  }

  let ps = events.map(async (ev) => {
    ev.items = await queryEvent.selectEventFieldsByEventId(
      this.global.db,
      ev.id
    )
  })
  await Promise.all(ps)

  let promise = templateUtil.collectBlocks(events, options, this)
  return templateUtil.templatePromise(this.global, promise)
}

/**
 * Block helper iterating over all commands, including their arguments and clusters.
 *
 * @param {*} options
 * @returns Promise of content.
 */
function zcl_command_tree(options) {
  let promise = templateUtil
    .ensureZclPackageIds(this)
    .then((packageIds) =>
      queryCommand.selectCommandTree(this.global.db, packageIds)
    )
    .then((cmds) => {
      // Now reduce the array by collecting together arguments.
      let reducedCommands = []
      cmds.forEach((el) => {
        let newCommand
        let lastCommand
        if (reducedCommands.length == 0) {
          newCommand = true
        } else {
          lastCommand = reducedCommands[reducedCommands.length - 1]
          if (
            el.code == lastCommand.code &&
            el.clusterCode == lastCommand.clusterCode &&
            el.source == lastCommand.source
          ) {
            newCommand = false
          } else {
            newCommand = true
          }
        }

        let arg
        if (el.argName == null) {
          arg = null
        } else {
          arg = {
            name: el.argName,
            type: el.argType,
            isArray: el.argIsArray,
            hasLength: el.argIsArray,
            nameLength: el.argName.concat('Len'),
          }
          if (el.argIsArray) {
            arg.formatChar = 'b'
          } else if (types.isOneBytePrefixedString(el.argType)) {
            arg.formatChar = 's'
          } else if (types.isTwoBytePrefixedString(el.argType)) {
            arg.formatChar = 'l'
          } else {
            arg.formatChar = 'u'
          }
        }
        if (newCommand) {
          el.commandArgs = []
          if (arg != null) {
            el.commandArgs.push(arg)
            el.argsstring = arg.formatChar
          } else {
            el.argsstring = ''
          }
          let n = ''
          if (el.clusterCode == null) {
            n = n.concat('Global')
          } else {
            n = n.concat(_.upperFirst(_.camelCase(el.clusterDefineName)))
          }
          if (el.source == dbEnum.source.either) {
            // We will need to create two here.
            n = n.concat('ClientToServer')
          }
          n = n.concat(el.name)
          el.clientMacroName = n
          el.isGlobal = el.clusterCode == null
          reducedCommands.push(el)
        } else {
          if (arg != null) {
            lastCommand.commandArgs.push(arg)
            lastCommand.argsstring = lastCommand.argsstring.concat(
              arg.formatChar
            )
          }
        }
      })
      return reducedCommands
    })
    .then((cmds) => templateUtil.collectBlocks(cmds, options, this))
  return templateUtil.templatePromise(this.global, promise)
}

/**
 * Helper to iterate over all global commands.
 *
 * @param {*} options
 * @returns Promise of global command iteration.
 */
function zcl_global_commands(options) {
  let promise = templateUtil
    .ensureZclPackageIds(this)
    .then((packageIds) =>
      queryCommand.selectAllGlobalCommands(this.global.db, packageIds)
    )
    .then((cmds) => templateUtil.collectBlocks(cmds, options, this))
  return templateUtil.templatePromise(this.global, promise)
}

/**
 * Iterator over the attributes. If it is used at toplevel, if iterates over all the attributes
 * in the database. If used within zcl_cluster context, it iterates over all the attributes
 * that belong to that cluster.
 *
 * @param {*} options
 * @returns Promise of attribute iteration.
 */
function zcl_attributes(options) {
  // If used at the toplevel, 'this' is the toplevel context object.
  // when used at the cluster level, 'this' is a cluster
  let promise = templateUtil
    .ensureZclPackageIds(this)
    .then((packageIds) => {
      if ('id' in this) {
        // We're functioning inside a nested context with an id, so we will only query for this cluster.
        return queryZcl.selectAttributesByClusterIdIncludingGlobal(
          this.global.db,
          this.id,
          packageIds
        )
      } else {
        return queryZcl.selectAllAttributes(this.global.db, packageIds)
      }
    })
    .then((atts) => templateUtil.collectBlocks(atts, options, this))
  return templateUtil.templatePromise(this.global, promise)
}

/**
 * Iterator over the client attributes. If it is used at toplevel, if iterates over all the client attributes
 * in the database. If used within zcl_cluster context, it iterates over all the client attributes
 * that belong to that cluster.
 *
 * @param {*} options
 * @returns Promise of attribute iteration.
 */
function zcl_attributes_client(options) {
  // If used at the toplevel, 'this' is the toplevel context object.
  // when used at the cluster level, 'this' is a cluster
  let promise = templateUtil
    .ensureZclPackageIds(this)
    .then((packageIds) => {
      if ('id' in this) {
        return queryZcl.selectAttributesByClusterIdAndSideIncludingGlobal(
          this.global.db,
          this.id,
          packageIds,
          dbEnum.side.client
        )
      } else {
        return queryZcl.selectAllAttributesBySide(
          this.global.db,
          dbEnum.side.client,
          packageIds
        )
      }
    })
    .then((atts) => templateUtil.collectBlocks(atts, options, this))
  return templateUtil.templatePromise(this.global, promise)
}

/**
 * Iterator over the server attributes. If it is used at toplevel, if iterates over all the server attributes
 * in the database. If used within zcl_cluster context, it iterates over all the server attributes
 * that belong to that cluster.
 *
 * @param {*} options
 * @returns Promise of attribute iteration.
 */
function zcl_attributes_server(options) {
  // If used at the toplevel, 'this' is the toplevel context object.
  // when used at the cluster level, 'this' is a cluster
  let promise = templateUtil
    .ensureZclPackageIds(this)
    .then((packageIds) => {
      if ('id' in this) {
        // We're functioning inside a nested context with an id, so we will only query for this cluster.
        return queryZcl.selectAttributesByClusterIdAndSideIncludingGlobal(
          this.global.db,
          this.id,
          packageIds,
          dbEnum.side.server
        )
      } else {
        return queryZcl.selectAllAttributesBySide(
          this.global.db,
          dbEnum.side.server,
          packageIds
        )
      }
    })
    .then((atts) => templateUtil.collectBlocks(atts, options, this))
  return templateUtil.templatePromise(this.global, promise)
}

/**
 * Block helper iterating over all atomic types.
 *
 * @param {*} options
 * @returns Promise of content.
 */
async function zcl_atomics(options) {
  let promise = templateUtil
    .ensureZclPackageIds(this)
    .then((packageIds) =>
      Promise.all(
        packageIds.map((packageId) =>
          queryZcl.selectAllAtomics(this.global.db, packageId)
        )
      )
    )
    .then((x) => x.flat())
    .then((ats) => templateUtil.collectBlocks(ats, options, this))
  return templateUtil.templatePromise(this.global, promise)
}

/**
 *
 *
 * Given: N/A
 * @returns the length of largest cluster name in a list of clusters
 */
async function zcl_cluster_largest_label_length() {
  let promise = templateUtil
    .ensureZclPackageIds(this)
    .then((packageIds) =>
      Promise.all(
        packageIds.map((packageId) =>
          queryZcl.selectAllClusters(this.global.db, packageId)
        )
      )
    )
    .then((cls) => cls.flat())
    .then((cl) => largestLabelLength(cl))
  return templateUtil.templatePromise(this.global, promise)
}

/**
 *
 *
 * @param {*} An Array
 * @returns the length of largest object name in an array. Helper for
 * zcl_cluster_largest_label_length
 */
function largestLabelLength(arrayOfClusters) {
  return Math.max(...arrayOfClusters.map((cl) => cl.label.length))
}

/**
 * Helper to extract the number of command arguments in a command
 *
 * @param {*} commandId
 * @returns Number of command arguments as an integer
 */
function zcl_command_arguments_count(commandId) {
  let promise = queryCommand.selectCommandArgumentsCountByCommandId(
    this.global.db,
    commandId
  )
  return templateUtil.templatePromise(this.global, promise)
}

/**
 *
 * @param commandId
 * @param fixedLengthReturn
 * @param notFixedLengthReturn
 * @param currentContext
 * Returns fixedLengthReturn or notFixedLengthReturn based on whether the
 * command is fixed length or not
 */
async function ifCommandArgumentsHaveFixedLengthWithCurrentContext(
  commandId,
  fixedLengthReturn,
  notFixedLengthReturn,
  currentContext
) {
  let commandArgs = await queryCommand.selectCommandArgumentsByCommandId(
    currentContext.global.db,
    commandId
  )

  let isFixedLength = true

  for (let argIndex = 0; argIndex < commandArgs.length; argIndex++) {
    if (
      commandArgs[argIndex].isArray ||
      types.isString(commandArgs[argIndex].type)
    ) {
      isFixedLength = false
    }
  }

  if (isFixedLength) {
    return fixedLengthReturn
  } else {
    return notFixedLengthReturn
  }
}

/**
 *
 * @param commandId
 * @param fixedLengthReturn
 * @param notFixedLengthReturn
 * Returns fixedLengthReturn or notFixedLengthReturn based on whether the
 * command is fixed length or not. Does not check if command
 * arguments are always present or not.
 */
function if_command_arguments_have_fixed_length(
  commandId,
  fixedLengthReturn,
  notFixedLengthReturn
) {
  return ifCommandArgumentsHaveFixedLengthWithCurrentContext(
    commandId,
    fixedLengthReturn,
    notFixedLengthReturn,
    this
  )
}

/**
 *
 * @param type
 * @param command
 * @param commandArg
 * @param appendString
 * @param options
 * @returns the underlying zcl type of a command argument if the argument is
 * not fixed length but is always present. If the condition is not met then
 * returns an empty string.
 */
function as_underlying_zcl_type_command_is_not_fixed_length_but_command_argument_is_always_present(
  type,
  command,
  commandArg,
  appendString,
  options
) {
  return queryCommand
    .selectCommandArgumentsByCommandId(this.global.db, command)
    .then(
      (commandArgs) =>
        new Promise((resolve, reject) => {
          for (let ca of commandArgs) {
            if (
              ca.isArray ||
              types.isString(ca.type) ||
              ca.introducedInRef ||
              ca.removedInRef ||
              ca.presentIf
            ) {
              resolve(false)
            }
          }
          resolve(true)
        })
    )
    .then((isFixedLengthCommand) => {
      if (isFixedLengthCommand) {
        return ''
      } else if (
        !(
          commandArg.isArray ||
          commandArg.introducedInRef ||
          commandArg.removedInRef ||
          commandArg.presentIf
        )
      ) {
        return templateUtil
          .ensureZclPackageIds(this)
          .then((packageIds) =>
            zclUtil.asUnderlyingZclTypeWithPackageId(
              type,
              options,
              packageIds,
              this
            )
          )
      }
      return ''
    })
    .then((res) => (res ? res + appendString : res))
    .catch((err) => {
      env.logError(
        'Failure in as_underlying_zcl_type_command_is_not_fixed_length_but_command_argument_is_always_present: ' +
          err
      )
    })
}

/**
 *
 * @param type
 * @param commandId
 * @param appendString
 * @param options
 * Returns: Given the commandId and the type of one of its arguments, based on
 * whether the command is fixed length or not either return nothing or return
 * the underlying zcl type appended with the appendString.
 */
function as_underlying_zcl_type_if_command_is_not_fixed_length(
  type,
  commandId,
  appendString,
  options
) {
  let promise = ifCommandArgumentsHaveFixedLengthWithCurrentContext(
    commandId,
    true,
    false,
    this
  )
    .then((res) => {
      if (res) {
        return ''
      } else {
        return templateUtil
          .ensureZclPackageIds(this)
          .then((packageIds) =>
            zclUtil.asUnderlyingZclTypeWithPackageId(
              type,
              options,
              packageIds,
              this
            )
          )
      }
    })
    .then((res) => (res ? res + appendString : res))
    .catch((err) => {
      env.logError(err)
      throw err
    })
  return templateUtil.templatePromise(this.global, promise)
}

/**
 *
 * @param commandId
 * Returns the size of the command by calculating the sum total of the command arguments
 * Note: This helper should be called on fixed length commands only. It should not be
 * called with commands which do not have a fixed length.
 */
function command_arguments_total_length(commandId) {
  return queryCommand
    .selectCommandArgumentsByCommandId(this.global.db, commandId)
    .then((commandArgs) =>
      new Promise((resolve, reject) => {
        let argsLength = []
        for (const commandArg of commandArgs) {
          let argType = commandArg.type
          let argOptions = {}
          argOptions.hash = {}
          argOptions.hash[dbEnum.zclType.zclCharFormatter] = true
          let argLength = templateUtil
            .ensureZclPackageIds(this)
            .then((packageIds) =>
              zclUtil.asUnderlyingZclTypeWithPackageId(
                argType,
                argOptions,
                packageIds,
                this
              )
            )
          argsLength.push(argLength)
        }
        resolve(argsLength)
      }).then((argsLength) => {
        return Promise.all(argsLength).then((lengths) =>
          lengths.reduce((a, b) => a + b, 0)
        )
      })
    )
    .catch((err) =>
      env.logError('Unable to get the length of the command arguments: ' + err)
    )
}

/**
 * Block helper iterating over command arguments within a command
 * or a command tree.
 *
 * @param {*} options
 * @returns Promise of command argument iteration.
 */
async function zcl_command_arguments(options) {
  let commandArgs = this.commandArgs
  let p

  // When we are coming from commant_tree, then
  // the commandArgs are already present and there is no need
  // to do additional queries.
  if (commandArgs == null) {
    p = templateUtil.ensureZclPackageIds(this).then((packageIds) => {
      if ('id' in this) {
        // We're functioning inside a nested context with an id, so we will only query for this cluster.
        return queryCommand.selectCommandArgumentsByCommandId(
          this.global.db,
          this.id
        )
      } else {
        return queryCommand.selectAllCommandArguments(
          this.global.db,
          packageIds
        )
      }
    })
  } else {
    p = Promise.resolve(commandArgs)
  }

  let promise = p.then((args) =>
    templateUtil.collectBlocks(args, options, this)
  )
  return templateUtil.templatePromise(this.global, promise)
}

/**
 * Block helper iterating over the event fields inside an event.
 *
 * @param {*} options
 */
function zcl_event_fields(options) {
  let eventFields = this.eventField
  let p

  if (eventFields == null) {
    p = templateUtil.ensureZclPackageIds(this).then((packageIds) => {
      if ('id' in this) {
        return queryEvent.selectEventFieldsByEventId(this.global.db, this.id)
      } else {
        return queryEvent.selectAllEventFields(this.global.db, packageIds)
      }
    })
  } else {
    p = Promise.resolve(eventFields)
  }

  let promise = p.then((fields) =>
    templateUtil.collectBlocks(fields, options, this)
  )
  return templateUtil.templatePromise(this.global, promise)
}

/**
 * Helper that deals with the type of the argument.
 *
 * @param {*} typeName
 * @param {*} options
 */
function zcl_command_argument_data_type(type, options) {
  let promise = templateUtil
    .ensureZclPackageIds(this)
    .then((packageIds) =>
      Promise.all([
        zclUtil.isEnum(this.global.db, type, packageIds),
        zclUtil.isStruct(this.global.db, type, packageIds),
        zclUtil.isBitmap(this.global.db, type, packageIds),
      ])
        .then(
          (res) =>
            new Promise((resolve, reject) => {
              for (let i = 0; i < res.length; i++) {
                if (res[i] != 'unknown') {
                  resolve(res[i])
                  return
                }
              }
              resolve(dbEnum.zclType.unknown)
            })
        )
        .then((resType) => {
          switch (resType) {
            case dbEnum.zclType.bitmap:
              return helperC.data_type_for_bitmap(
                this.global.db,
                type,
                packageIds
              )
            case dbEnum.zclType.enum:
              return helperC.data_type_for_enum(
                this.global.db,
                type,
                packageIds
              )
            case dbEnum.zclType.struct:
              return options.hash.struct
            case dbEnum.zclType.atomic:
            case dbEnum.zclType.unknown:
            default:
              return helperC.as_cli_type(type)
          }
        })
        .catch((err) => {
          env.logError(err)
          throw err
        })
    )
    .catch((err) => {
      env.logError(err)
      throw err
    })
  return templateUtil.templatePromise(this.global, promise)
}

/**
 * Helper that deals with the type of the argument.
 *
 * @param {*} typeName
 * @param {*} options
 * Note: If the options has zclCharFormatter set to true then the function will
 * return the user defined data associated with the zcl data type and not the
 * actual data type.
 *
 * example:
 * {{asUnderlyingZclType [array type] array="b" one_byte="u" two_byte="v" three_byte="x"
 *  four_byte="w" short_string="s" long_string="l" default="b"
 *  zclCharFormatter="true"}}
 *
 * For the above if asUnderlyingZclType was given [array type] then the above
 * will return 'b'
 */
async function asUnderlyingZclType(type, options) {
  const packageIds = await templateUtil.ensureZclPackageIds(this)
  let promise = zclUtil
    .asUnderlyingZclTypeWithPackageId(type, options, packageIds, this)
    .catch((err) => {
      env.logError(err)
      throw err
    })
  return templateUtil.templatePromise(this.global, promise)
}

/**
 *
 * @param type
 * @param options
 * Returns the data mentioned in the helper options based on whether the type
 * is short string, long string or not a string
 * Example:
 * {{zcl_string_type_return type short_string="short string output"
 *                               long_string="short string output"
 *                               default="Output when not a string")
 *
 */
function zcl_string_type_return(type, options) {
  if (
    !(
      'short_string' in options.hash &&
      'long_string' in options.hash &&
      'default' in options.hash
    )
  ) {
    throw new Error('Specify all options for the helper')
  }
  if (types.isOneBytePrefixedString(type.toLowerCase())) {
    return options.hash.short_string
  } else if (types.isTwoBytePrefixedString(type.toLowerCase())) {
    return options.hash.long_string
  } else {
    return options.hash.default
  }
}

/**
 *
 * @param type
 * Return: true or false based on whether the type is a string or not.
 */
function is_zcl_string(type) {
  return types.isString(type)
}

/**
 * If helper that checks if a type is a string
 *
 * example:
 * {{#if_is_number type}}
 * type is number
 * {{else}}
 * type is not number
 * {{/if_is_number}}
 *
 * @param {*} type
 * @returns Promise of content.
 */
async function if_is_number(type, options) {
  let promise = templateUtil
    .ensureZclPackageIds(this)
    .then((packageIds) =>
      type && typeof type === 'string'
        ? queryZcl.selectNumberByName(
            this.global.db,
            packageIds,
            type.toLowerCase()
          )
        : null
    )
    .then((res) =>
      res ? res : queryZcl.selectNumberById(this.global.db, type)
    )
    .then((res) => (res ? options.fn(this) : options.inverse(this)))
  return templateUtil.templatePromise(this.global, promise)
}

/**
 * If helper that checks if a type is a string
 *
 * example:
 * {{#if_is_string type}}
 * type is string
 * {{else}}
 * type is not string
 * {{/if_is_string}}
 *
 * @param {*} type
 * @returns Promise of content.
 */
function if_is_string(type, options) {
  let promise = templateUtil
    .ensureZclPackageIds(this)
    .then((packageIds) =>
      type && typeof type === 'string'
        ? queryZcl.selectStringByName(
            this.global.db,
            type.toLowerCase(),
            packageIds
          )
        : null
    )
    .then((res) =>
      res ? res : queryZcl.selectStringById(this.global.db, type)
    )
    .then((res) => (res ? options.fn(this) : options.inverse(this)))
  return templateUtil.templatePromise(this.global, promise)
}

/**
 * If helper that checks if a string type is present in the list of char strings
 * i.e. characterStringTypes
 *
 * example:
 * {{#if_is_char_string type}}
 * type is char string
 * {{else}}
 * type is not char string
 * {{/if_is_char_string}}
 *
 * @param {*} type
 * @returns Promise of content.
 */
function if_is_char_string(type, options) {
  let promise = templateUtil
    .ensureZclPackageIds(this)
    .then((packageIds) =>
      type && typeof type === 'string'
        ? queryZcl.selectStringByName(
            this.global.db,
            type.toLowerCase(),
            packageIds
          )
        : null
    )
    .then((res) =>
      res ? res : queryZcl.selectStringById(this.global.db, type)
    )
    .then((res) =>
      res && res.name && characterStringTypes.includes(res.name.toUpperCase())
        ? options.fn(this)
        : options.inverse(this)
    )
  return templateUtil.templatePromise(this.global, promise)
}

/**
 * If helper that checks if a string type is present in the list of octet strings
 * i.e. octetStringTypes
 *
 * example:
 * {{#if_is_octet_string type}}
 * type is octet string
 * {{else}}
 * type is not octet string
 * {{/if_is_octet_string}}
 *
 * @param {*} type
 * @returns Promise of content.
 */
function if_is_octet_string(type, options) {
  let promise = templateUtil
    .ensureZclPackageIds(this)
    .then((packageIds) =>
      type && typeof type === 'string'
        ? queryZcl.selectStringByName(
            this.global.db,
            type.toLowerCase(),
            packageIds
          )
        : null
    )
    .then((res) =>
      res ? res : queryZcl.selectStringById(this.global.db, type)
    )
    .then((res) =>
      res && res.name && octetStringTypes.includes(res.name.toUpperCase())
        ? options.fn(this)
        : options.inverse(this)
    )
  return templateUtil.templatePromise(this.global, promise)
}

/**
 * If helper that checks if a string type is present in the list of short strings
 * i.e. stringShortTypes
 *
 * example:
 * {{#if_is_short_string type}}
 * type is short string
 * {{else}}
 * type is not short string
 * {{/if_is_short_string}}
 *
 * @param {*} type
 * @returns Promise of content.
 */
function if_is_short_string(type, options) {
  let promise = templateUtil
    .ensureZclPackageIds(this)
    .then((packageIds) =>
      type && typeof type === 'string'
        ? queryZcl.selectStringByName(
            this.global.db,
            type.toLowerCase(),
            packageIds
          )
        : null
    )
    .then((res) =>
      res ? res : queryZcl.selectStringById(this.global.db, type)
    )
    .then((res) =>
      res && res.name && stringShortTypes.includes(res.name.toUpperCase())
        ? options.fn(this)
        : options.inverse(this)
    )
  return templateUtil.templatePromise(this.global, promise)
}

/**
 * If helper that checks if a string type is present in the list of long strings
 * i.e. stringLongTypes
 *
 * example:
 * {{#if_is_long_string type}}
 * type is long string
 * {{else}}
 * type is not long string
 * {{/if_is_long_string}}
 *
 * @param {*} type
 * @returns Promise of content.
 */
function if_is_long_string(type, options) {
  let promise = templateUtil
    .ensureZclPackageIds(this)
    .then((packageIds) =>
      type && typeof type === 'string'
        ? queryZcl.selectStringByName(
            this.global.db,
            type.toLowerCase(),
            packageIds
          )
        : null
    )
    .then((res) =>
      res ? res : queryZcl.selectStringById(this.global.db, type)
    )
    .then((res) =>
      res && res.name && stringLongTypes.includes(res.name.toUpperCase())
        ? options.fn(this)
        : options.inverse(this)
    )
  return templateUtil.templatePromise(this.global, promise)
}

/**
 * If helper that checks if a type is an atomic
 *
 * example:
 * {{#if_is_atomic type}}
 * type is atomic
 * {{else}}
 * type is not atomic
 * {{/if_is_atomic}}
 *
 * @param {*} type: string
 * @returns Promise of content.
 */
async function if_is_atomic(type, options) {
  let result = null
  if (typeof type === 'string') {
    result = await templateUtil
      .ensureZclPackageIds(this)
      .then((packageIds) =>
        queryZcl.selectAtomicType(this.global.db, packageIds, type)
      )
  } else {
    env.logWarning(
      'Passing a type which is invlaid for if_is_atomic helper: ' + type
    )
  }
  if (result) {
    return options.fn(this)
  } else {
    return options.inverse(this)
  }
}

/**
 * If helper that checks if a type is a bitmap
 *
 * example:
 * {{#if_is_bitmap type}}
 * type is bitmap
 * {{else}}
 * type is not bitmap
 * {{/if_is_bitmap}}
 *
 * @param {*} type
 * @returns Promise of content.
 */
async function if_is_bitmap(type, options) {
  let promise = templateUtil
    .ensureZclPackageIds(this)
    .then((packageIds) =>
      type && typeof type === 'string'
        ? queryZcl.selectBitmapByName(this.global.db, packageIds, type)
        : null
    )
    .then((res) =>
      res ? res : queryZcl.selectBitmapById(this.global.db, type)
    )
    .then((res) =>
      res || (typeof type === 'string' && type.startsWith('map'))
        ? options.fn(this)
        : options.inverse(this)
    )
  return templateUtil.templatePromise(this.global, promise)
}

/**
 * If helper that checks if a type is an enum
 *
 * * example:
 * {{#if_is_enum type}}
 * type is enum
 * {{else}}
 * type is not enum
 * {{/if_is_enum}}
 *
 * @param {*} type
 * @returns Promise of content.
 */
async function if_is_enum(type, options) {
  let promise = templateUtil
    .ensureZclPackageIds(this)
    .then((packageIds) =>
      type && typeof type === 'string'
        ? queryZcl.selectEnumByName(this.global.db, type, packageIds)
        : null
    )
    .then((res) => (res ? res : queryZcl.selectEnumById(this.global.db, type)))
    .then((res) => (res ? options.fn(this) : options.inverse(this)))
  return templateUtil.templatePromise(this.global, promise)
}

/**
 * If helper that checks if a type is an struct
 *
 * * example:
 * {{#if_is_struct type}}
 * type is struct
 * {{else}}
 * type is not struct
 * {{/if_is_struct}}
 *
 * @param type
 * @returns Promise of content.
 */
async function if_is_struct(type, options) {
  let promise = templateUtil
    .ensureZclPackageIds(this)
    .then((packageIds) =>
      type && typeof type === 'string'
        ? queryZcl.selectStructByName(this.global.db, type, packageIds)
        : null
    )
    .then((res) =>
      res ? res : queryZcl.selectStructById(this.global.db, type)
    )
    .then((res) => (res ? options.fn(this) : options.inverse(this)))
  return templateUtil.templatePromise(this.global, promise)
}

/**
 * Checks if the side is client or not
 *
 * @param {*} side
 * @returns boolean
 */
function isClient(side) {
  return 0 == side.localeCompare(dbEnum.side.client)
}

/**
 * Checks if the side is server or not
 *
 * @param {*} side
 * @returns boolean
 */
function isServer(side) {
  return 0 == side.localeCompare(dbEnum.side.server)
}

function isStrEqual(str1, str2) {
  return 0 == str1.localeCompare(str2)
}

function isLastElement(index, count) {
  return index == count - 1
}

function isFirstElement(index, count) {
  return index == 0
}

function isEnabled(enable) {
  return 1 == enable
}

function isCommandAvailable(clusterSide, incoming, outgoing, source, name) {
  if (0 == clusterSide.localeCompare(source)) {
    return false
  }

  return (
    (isClient(clusterSide) && incoming) || (isServer(clusterSide) && incoming)
  )
}

/**
 *
 *
 * @param type: type of argument
 * @param commandId: command id
 * @param appendString: append the string to the argument
 * @param introducedInRef: If the command argument is not present in all zcl
 * specifications and was introduced in a certain specification version then this will not be null
 * @param removedInRef: If the command argument is not present in all zcl
 * specifications and was removed in a certain specification version then this will not be null
 * @param presentIf: If the command argument is present conditionally then this will be a condition
 * and not null
 *
 * @param options: options which can be passed to zclUtil.asUnderlyingZclTypeWithPackageId
 * for determining the underlying zcl type for the provided argument type
 * @returns A string as an underlying zcl type if the command is not fixed length and the command
 * argument is always present in all zcl specifications.
 */
function as_underlying_zcl_type_command_argument_always_present(
  type,
  commandId,
  appendString,
  introducedInRef,
  removedInRef,
  presentIf,
  options
) {
  let promise = ifCommandArgumentsHaveFixedLengthWithCurrentContext(
    commandId,
    true,
    false,
    this
  )
    .then((res) => {
      if (res) {
        return ''
      } else {
        // Return the underlying zcl type since command argument is always present
        if (introducedInRef || removedInRef || presentIf) {
          // Return nothing if the command argument is not always present
          return ''
        } else {
          // Return the underlying zcl type if the command argument is always present.
          return templateUtil
            .ensureZclPackageIds(this)
            .then((packageIds) =>
              zclUtil.asUnderlyingZclTypeWithPackageId(
                type,
                options,
                packageIds,
                this
              )
            )
        }
      }
    })
    // Adding the appendString for the underlying zcl type
    .then((res) => (res ? res + appendString : res))
    .catch((err) => {
      env.logError(err)
      throw err
    })
  return templateUtil.templatePromise(this.global, promise)
}

/**
 *
 *
 * @param commandId
 * @param introducedInRef
 * @param removedInRef
 * @param presentIf
 * @param argumentPresentReturn
 * @param argumentNotPresentReturn
 * @returns argumentPresentReturn if the command is not fixed length and command
 * argument is always present without conditions(introducedInRef, removedInRef,
 * presentIf) else returns argumentNotPresentReturn
 */
function if_command_argument_always_present(
  commandId,
  introducedInRef,
  removedInRef,
  presentIf,
  argumentPresentReturn,
  argumentNotPresentReturn
) {
  return ifCommandArgumentsHaveFixedLengthWithCurrentContext(
    commandId,
    true,
    false,
    this
  ).then((res) => {
    if (res) {
      return '' // Return nothing since command is a fixed length command
    } else {
      if (introducedInRef || removedInRef || presentIf) {
        return argumentNotPresentReturn
      }
      return argumentPresentReturn
    }
  })
}

/**
 *
 *
 * @param type: type of argument
 * @param commandId: command id
 * @param appendString: append the string to the argument
 * @param introducedInRef: If the command argument is not present in all zcl
 * specifications and was introduced in a certain specification version then this will not be null
 * @param removedInRef: If the command argument is not present in all zcl
 * specifications and was removed in a certain specification version then this will not be null
 * @param presentIf: If the command argument is present conditionally then this will be a condition
 * and not null
 * @param options: options which can be passed to zclUtil.asUnderlyingZclTypeWithPackageId
 * for determining the underlying zcl type for the provided argument type
 * @returns A string as an underlying zcl type if the command is not fixed length, the command
 * argument is not always present in all zcl specifications and there is no present if conditionality
 * on the command argument.
 */

function as_underlying_zcl_type_command_argument_not_always_present_no_presentif(
  type,
  commandId,
  appendString,
  introducedInRef,
  removedInRef,
  presentIf,
  options
) {
  let promise = ifCommandArgumentsHaveFixedLengthWithCurrentContext(
    commandId,
    true,
    false,
    this
  )
    .then((res) => {
      if (res) {
        return '' // Return nothing since the command is of fixed length
      } else {
        // Return the underlying zcl type since command argument is not always present and there is no present if conditionality
        if ((introducedInRef || removedInRef) && !presentIf) {
          return templateUtil
            .ensureZclPackageIds(this)
            .then((packageIds) =>
              zclUtil.asUnderlyingZclTypeWithPackageId(
                type,
                options,
                packageIds,
                this
              )
            )
        } else {
          return ''
        }
      }
    })
    // Adding the appendString for the underlying zcl type
    .then((res) => (res ? res + appendString : res))
    .catch((err) => {
      env.logError(err)
      throw err
    })
  return templateUtil.templatePromise(this.global, promise)
}

/**
 * @param commandArg command argument
 * @param appendString append the string to the argument
 * @param options options which can be passed to zclUtil.asUnderlyingZclTypeWithPackageId
 * for determining the underlying zcl type for the provided argument type
 * @returns A string as an underlying zcl type if the command is not fixed
 * length, the command argument is not always present in all zcl specifications
 * and there is no present if conditionality on the command argument.
 */
function as_underlying_zcl_type_ca_not_always_present_no_presentif(
  commandArg,
  appendString,
  options
) {
  // Return the underlying zcl type since command argument is not always
  // present and there is no present if conditionality
  if (
    (commandArg.introducedInRef || commandArg.removedInRef) &&
    !commandArg.presentIf
  ) {
    let promise = templateUtil
      .ensureZclPackageIds(this)
      .then((packageIds) =>
        zclUtil.asUnderlyingZclTypeWithPackageId(
          commandArg.type,
          options,
          packageIds,
          this
        )
      ) // Adding the appendString for the underlying zcl type
      .then((res) => (res ? res + appendString : res))
      .catch((err) => {
        env.logError(
          'Error in as_underlying_zcl_type_ca_not_always_present_no_presentif ' +
            err
        )
        throw err
      })
    return templateUtil.templatePromise(this.global, promise)
  } else {
    return ''
  }
}

/**
 *
 *
 * @param commandId
 * @param introducedInRef
 * @param removedInRef
 * @param presentIf
 * @param argumentNotInAllVersionsReturn
 * @param argumentInAllVersionsReturn
 * @returns argumentNotInAllVersionsReturn if the command is not fixed length and command
 * argument is present with conditions introducedInRef or removedInRef but no presentIf
 * conditions else returns argumentNotPresentReturn
 */
function if_command_argument_not_always_present_no_presentif(
  commandId,
  introducedInRef,
  removedInRef,
  presentIf,
  argumentNotInAllVersionsReturn,
  argumentInAllVersionsReturn
) {
  return ifCommandArgumentsHaveFixedLengthWithCurrentContext(
    commandId,
    true,
    false,
    this
  ).then((res) => {
    if (res) {
      return '' // Return nothing since it is a fixed length command
    } else {
      if ((introducedInRef || removedInRef) && !presentIf) {
        return argumentNotInAllVersionsReturn
      }
      return argumentInAllVersionsReturn
    }
  })
}

/**
 *
 *
 * @param type: type of argument
 * @param commandId: command id
 * @param appendString: append the string to the argument
 * @param introducedInRef: If the command argument is not present in all zcl
 * specifications and was introduced in a certain specification version then this will not be null
 * @param removedInRef: If the command argument is not present in all zcl
 * specifications and was removed in a certain specification version then this will not be null
 * @param presentIf: If the command argument is present conditionally then this will be a condition
 * and not null
 * @param options: options which can be passed to zclUtil.asUnderlyingZclTypeWithPackageId
 * for determining the underlying zcl type for the provided argument type
 * @returns A string as an underlying zcl type if the command is not fixed length, the command
 * argument is not always present in all zcl specifications and there is a present if conditionality
 * on the command argument.
 */
function as_underlying_zcl_type_command_argument_not_always_present_with_presentif(
  type,
  commandId,
  appendString,
  introducedInRef,
  removedInRef,
  presentIf,
  options
) {
  let promise = ifCommandArgumentsHaveFixedLengthWithCurrentContext(
    commandId,
    true,
    false,
    this
  )
    .then((res) => {
      if (res) {
        return '' // Return nothing since the command is of fixed length
      } else {
        // Return the underlying zcl type since command argument is not always present and there is present if conditionality.
        if ((introducedInRef || removedInRef) && presentIf) {
          return templateUtil
            .ensureZclPackageIds(this)
            .then((packageIds) =>
              zclUtil.asUnderlyingZclTypeWithPackageId(
                type,
                options,
                packageIds,
                this
              )
            )
        } else {
          return ''
        }
      }
    })
    // Adding the appendString for the underlying zcl type
    .then((res) => (res ? res + appendString : res))
    .catch((err) => {
      env.logError(err)
      throw err
    })
  return templateUtil.templatePromise(this.global, promise)
}

/**
 * @param commandArg command argument
 * @param appendString append the string to the argument
 * @param options options which can be passed to zclUtil.asUnderlyingZclTypeWithPackageId
 * for determining the underlying zcl type for the provided argument type
 * @returns A string as an underlying zcl type if the command is not fixed
 * length, the command argument is not always present in all zcl specifications
 * but there is a present if conditionality on the command argument.
 */
function as_underlying_zcl_type_ca_not_always_present_with_presentif(
  commandArg,
  appendString,
  options
) {
  // Return the underlying zcl type since command argument is not always
  // present and there is a present if conditionality
  if (
    (commandArg.introducedInRef || commandArg.removedInRef) &&
    commandArg.presentIf
  ) {
    let promise = templateUtil
      .ensureZclPackageIds(this)
      .then((packageIds) =>
        zclUtil.asUnderlyingZclTypeWithPackageId(
          commandArg.type,
          options,
          packageIds,
          this
        )
      ) // Adding the appendString for the underlying zcl type
      .then((res) => (res ? res + appendString : res))
      .catch((err) => {
        env.logError(
          'Error in as_underlying_zcl_type_ca_not_always_present_with_presentif ' +
            err
        )
        throw err
      })
    return templateUtil.templatePromise(this.global, promise)
  } else {
    return ''
  }
}

/**
 *
 *
 * @param commandId
 * @param introducedInRef
 * @param removedInRef
 * @param presentIf
 * @param argumentNotInAllVersionsPresentIfReturn
 * @param argumentInAllVersionsReturn
 * @returns argumentNotInAllVersionsReturn if the command is not fixed length, command
 * argument is present with conditions introducedInRef or removedInRef and presentIf
 * conditions exist as well else returns argumentNotPresentReturn
 */
function if_command_argument_not_always_present_with_presentif(
  commandId,
  introducedInRef,
  removedInRef,
  presentIf,
  argumentNotInAllVersionsPresentIfReturn,
  argumentInAllVersionsReturn
) {
  return ifCommandArgumentsHaveFixedLengthWithCurrentContext(
    commandId,
    true,
    false,
    this
  ).then((res) => {
    if (res) {
      return '' // Return nothing since it is a fixed length command
    } else {
      if ((introducedInRef || removedInRef) && presentIf) {
        return argumentNotInAllVersionsPresentIfReturn
      }
      return argumentInAllVersionsReturn
    }
  })
}

/**
 *
 *
 * @param type: type of argument
 * @param commandId: command id
 * @param appendString: append the string to the argument
 * @param introducedInRef: If the command argument is not present in all zcl
 * specifications and was introduced in a certain specification version then this will not be null
 * @param removedInRef: If the command argument is not present in all zcl
 * specifications and was removed in a certain specification version then this will not be null
 * @param presentIf: If the command argument is present conditionally then this will be a condition
 * and not null
 * @param options: options which can be passed to zclUtil.asUnderlyingZclTypeWithPackageId
 * for determining the underlying zcl type for the provided argument type
 * @returns A string as an underlying zcl type if the command is not fixed length, the command
 * argument is always present in all zcl specifications and there is a present if conditionality
 * on the command argument.
 */
function as_underlying_zcl_type_command_argument_always_present_with_presentif(
  type,
  commandId,
  appendString,
  introducedInRef,
  removedInRef,
  presentIf,
  options
) {
  let promise = ifCommandArgumentsHaveFixedLengthWithCurrentContext(
    commandId,
    true,
    false,
    this
  )
    .then((res) => {
      if (res) {
        return '' // Return nothing since the command is of fixed length
      } else {
        // Return the underlying zcl type since command argument is always present and there is a present if condition
        if (!(introducedInRef || removedInRef) && presentIf) {
          return templateUtil
            .ensureZclPackageIds(this)
            .then((packageIds) =>
              zclUtil.asUnderlyingZclTypeWithPackageId(
                type,
                options,
                packageIds,
                this
              )
            )
        } else {
          return ''
        }
      }
    })
    // Adding the appendString for the underlying zcl type
    .then((res) => (res ? res + appendString : res))
    .catch((err) => {
      env.logError(err)
      throw err
    })
  return templateUtil.templatePromise(this.global, promise)
}

/**
 * @param commandArg command argument
 * @param appendString append the string to the argument
 * @param options options which can be passed to zclUtil.asUnderlyingZclTypeWithPackageId
 * for determining the underlying zcl type for the provided argument type
 * @returns A string as an underlying zcl type if the command is not fixed
 * length, the command argument is always present in all zcl specifications
 * but there is a present if conditionality on the command argument.
 */
function as_underlying_zcl_type_ca_always_present_with_presentif(
  commandArg,
  appendString,
  options
) {
  // Return the underlying zcl type since command argument is always
  // present and there is a present if conditionality
  if (
    !(commandArg.introducedInRef || commandArg.removedInRef) &&
    commandArg.presentIf
  ) {
    let promise = templateUtil
      .ensureZclPackageIds(this)
      .then((packageIds) =>
        zclUtil.asUnderlyingZclTypeWithPackageId(
          commandArg.type,
          options,
          packageIds,
          this
        )
      ) // Adding the appendString for the underlying zcl type
      .then((res) => (res ? res + appendString : res))
      .catch((err) => {
        env.logError(
          'Error in as_underlying_zcl_type_ca_always_present_with_presentif ' +
            err
        )
        throw err
      })
    return templateUtil.templatePromise(this.global, promise)
  } else {
    return ''
  }
}

/**
 *
 *
 * @param commandId
 * @param introducedInRef
 * @param removedInRef
 * @param presentIf
 * @param argumentNotInAllVersionsPresentIfReturn
 * @param argumentInAllVersionsReturn
 * @returns argumentInAllVersionsPresentIfReturn if the command is not fixed length, command
 * argument is always present and presentIf conditions exist else returns argumentNotPresentReturn
 */
async function if_command_argument_always_present_with_presentif(
  commandId,
  introducedInRef,
  removedInRef,
  presentIf,
  argumentInAllVersionsPresentIfReturn,
  argumentNotAlwaysThereReturn
) {
  let res = await ifCommandArgumentsHaveFixedLengthWithCurrentContext(
    commandId,
    true,
    false,
    this
  )
  if (res) {
    return '' // Return nothing since it is a fixed length command
  } else {
    if (!(introducedInRef || removedInRef) && presentIf) {
      return argumentInAllVersionsPresentIfReturn
    }
    return argumentNotAlwaysThereReturn
  }
}

/**
 *
 *
 * @param {*} clusterId
 * @param {*} manufacturer_specific_return
 * @param {*} null_manufacturer_specific_return
 * @returns manufacturer_specific_return if the cluster is manufacturer
 * specific or returns null_manufacturer_specific_return if cluster is
 * not manufacturer specific.
 */
function if_manufacturing_specific_cluster(
  clusterId,
  manufacturer_specific_return,
  null_manufacturer_specific_return
) {
  let promise = templateUtil
    .ensureZclPackageIds(this)
    .then((packageIds) => queryZcl.selectClusterById(this.global.db, clusterId))
    .then((res) => {
      if (res.manufacturerCode != null) {
        return manufacturer_specific_return
      } else {
        return null_manufacturer_specific_return
      }
    })
  return templateUtil.templatePromise(this.global, promise)
}

/**
 * If helper which checks if cluster is manufacturing specific or not
 * example:
 * {{#if_mfg_specific_cluster clusterId}}
 *  cluster is manufacturing specific
 * {{else}}
 *  cluster is not manufacturing specific
 * {{/if_mfg_specific_cluster}}
 *
 * @param clusterId
 * @param options
 * @returns Returns content in the handlebar template based on whether the
 * command is manufacturing specific or not.
 */
async function if_mfg_specific_cluster(clusterId, options) {
  let res = await queryZcl.selectClusterById(this.global.db, clusterId)
  if (res.manufacturerCode != null) {
    return options.fn(this)
  } else {
    return options.inverse(this)
  }
}

/**
 * Given the value and size of an attribute along with endian as an option.
 * This helper returns the attribute value as big/little endian.
 * Example: {{as_generated_default_macro 0x00003840 4 endian="big"}}
 * will return: 0x00, 0x00, 0x38, 0x40,
 * @param value
 * @param attributeSize
 * @param options
 * @returns Formatted attribute value based on given arguments
 */
async function as_generated_default_macro(value, attributeSize, options) {
  let default_macro_signature = ''
  let temp = ''
  if (attributeSize > 2) {
    // String value
    if (isNaN(value)) {
      return format_zcl_string_as_characters_for_generated_defaults(
        value,
        attributeSize
      )
    }
    // Float value
    if (!isNaN(value) && value.toString().indexOf('.') != -1) {
      temp = types.convertFloatToBigEndian(value, attributeSize)
    } else {
      if (value > 0) {
        // Positive value
        temp = helperC.asHex(value, null, null)
      } else {
        // Negative value
        temp = types.convertIntToBigEndian(value, attributeSize)
      }
    }
    // Padding based on attribute size
    let default_macro = temp.replace('0x', '').match(/.{1,2}/g)
    let padding_length = attributeSize - default_macro.length
    for (let i = 0; i < padding_length; i++) {
      default_macro_signature += '0x00, '
    }
    for (let m of default_macro) {
      default_macro_signature += ' 0x' + m.toUpperCase() + ','
    }
  }

  // Applying endianess to attributes with size less than equal to 8 bytes.
  // Thus only swapping int64u or smaller
  if (options.hash.endian != 'big' && attributeSize <= 8) {
    default_macro_signature = default_macro_signature
      .split(' ')
      .reverse()
      .join(' ')
  }
  return default_macro_signature
}

/**
 * Given the attributes of a zcl attribute. Creates an attribute mask based on
 * the given options
 * @param writable
 * @param storageOption
 * @param minMax
 * @param mfgSpecific
 * @param clusterCode
 * @param client
 * @param isSingleton
 * @param prefixString
 * @param postfixString
 * @returns attribute mask based on given values
 */
async function attribute_mask(
  writable,
  storageOption,
  minMax,
  mfgSpecific,
  clusterCode,
  client,
  isSingleton,
  prefixString,
  postfixString
) {
  let attributeMask = ''
  // mask for isWritable
  if (writable) {
    attributeMask +=
      (attributeMask ? '| ' : '') + prefixString + 'WRITABLE' + postfixString
  }

  // mask for storage option
  if (storageOption === 'NVM') {
    attributeMask +=
      (attributeMask ? '| ' : '') + prefixString + 'TOKENIZE' + postfixString
  } else if (storageOption === 'External') {
    attributeMask +=
      (attributeMask ? '| ' : '') +
      prefixString +
      'EXTERNAL_STORAGE' +
      postfixString
  }

  // mask for bound
  if (minMax) {
    attributeMask +=
      (attributeMask ? '| ' : '') + prefixString + 'MIN_MAX' + postfixString
  }

  // mask for manufacturing specific attributes
  if (mfgSpecific && clusterCode < 64512) {
    attributeMask +=
      (attributeMask ? '| ' : '') +
      prefixString +
      'MANUFACTURER_SPECIFIC' +
      postfixString
  }

  // mask for client side attribute
  if (client === 'client') {
    attributeMask +=
      (attributeMask ? '| ' : '') + prefixString + 'CLIENT' + postfixString
  }

  //mask for singleton attirbute
  if (isSingleton) {
    attributeMask +=
      (attributeMask ? '| ' : '') + prefixString + 'SINGLETON' + postfixString
  }

  if (!attributeMask) {
    attributeMask = '0x00'
  }
  return attributeMask
}

/**
 * Given the attributes of a zcl command. Creates a command mask based on
 * the given options
 * @param commmandSource
 * @param clusterSide
 * @param isIncomingEnabled
 * @param isOutgoingEnabled
 * @param manufacturingCode
 * @param prefixForMask
 * @returns command mask based on given values
 */
async function command_mask(
  commmandSource,
  clusterSide,
  isIncomingEnabled,
  isOutgoingEnabled,
  manufacturingCode,
  prefixForMask
) {
  let commandMask = ''
  if (isClient(commmandSource)) {
    if (
      (isIncomingEnabled && commmandSource != clusterSide) ||
      (isIncomingEnabled && clusterSide == 'either')
    ) {
      commandMask += command_mask_sub_helper(
        commandMask,
        prefixForMask + 'INCOMING_SERVER'
      )
    }
    if (
      (isOutgoingEnabled && commmandSource == clusterSide) ||
      (isOutgoingEnabled && clusterSide == 'either')
    ) {
      commandMask += command_mask_sub_helper(
        commandMask,
        prefixForMask + 'OUTGOING_CLIENT'
      )
    }
  } else {
    if (
      (isIncomingEnabled && commmandSource != clusterSide) ||
      (isIncomingEnabled && clusterSide == 'either')
    ) {
      commandMask += command_mask_sub_helper(
        commandMask,
        prefixForMask + 'INCOMING_CLIENT'
      )
    }
    if (
      (isOutgoingEnabled && commmandSource == clusterSide) ||
      (isOutgoingEnabled && clusterSide == 'either')
    ) {
      commandMask += command_mask_sub_helper(
        commandMask,
        prefixForMask + 'OUTGOING_SERVER'
      )
    }
  }
  if (manufacturingCode && commandMask) {
    commandMask += command_mask_sub_helper(
      commandMask,
      prefixForMask + 'MANUFACTURER_SPECIFIC'
    )
  }
  return commandMask
}

/**
 * A Sub helper api for command_mask to reduce code redundancy
 * @param commandMask
 * @param str
 * @returns command mask addition based on the arguments
 */
function command_mask_sub_helper(commandMask, str) {
  if (commandMask) {
    return ' | ' + str
  } else {
    return str
  }
}

/**
 * This may be used within all_user_cluster_attributes_for_generated_defaults
 * for example:
 * {{format_zcl_string_as_characters_for_generated_defaults 'abc' 5}}
 * will return as follows:
 * 3, 'a', 'b', 'c' 0, 0
 * @param stringVal
 * @param sizeOfString
 * @returns Formatted string for generated defaults starting with the lenth of a
 * string then each character and then filler for the size allocated for the
 * string. Long strings prefixed by 2 byte length field.
 */
async function format_zcl_string_as_characters_for_generated_defaults(
  stringVal,
  sizeOfString
) {
  let lengthOfString = types.convertIntToBigEndian(
    stringVal.length,
    sizeOfString < 255 ? 1 : 2
  )
  lengthOfString = lengthOfString.replace('0x', '').match(/.{1,2}/g)
  let lengthPrefix = ''
  for (let m of lengthOfString) {
    lengthPrefix += ' 0x' + m.toUpperCase() + ','
  }
  // Switching length to little endian by default.
  lengthPrefix = lengthPrefix.split(' ').reverse().join(' ')
  let formatted_string = lengthPrefix
  for (let i = 0; i < stringVal.length; i++) {
    formatted_string += "'" + stringVal.charAt(i) + "', "
  }
  for (let i = stringVal.length + 1; i < sizeOfString; i++) {
    formatted_string += '0' + ', '
  }
  return formatted_string
}

const dep = templateUtil.deprecatedHelper

// WARNING! WARNING! WARNING! WARNING! WARNING! WARNING!
//
// Note: these exports are public API. Templates that might have been created in the past and are
// available in the wild might depend on these names.
// If you rename the functions, you need to still maintain old exports list.
exports.zcl_bitmaps = zcl_bitmaps
exports.zcl_bitmap_items = zcl_bitmap_items
exports.zcl_enums = zcl_enums
exports.zcl_enum_items = zcl_enum_items
exports.zcl_structs = zcl_structs
exports.zcl_struct_items = zcl_struct_items
exports.zcl_struct_items_by_struct_name = zcl_struct_items_by_struct_name
exports.zcl_clusters = zcl_clusters
exports.zcl_device_types = zcl_device_types
exports.zcl_device_type_clusters = zcl_device_type_clusters
exports.zcl_device_type_cluster_commands = zcl_device_type_cluster_commands
exports.zcl_device_type_cluster_attributes = zcl_device_type_cluster_attributes

exports.zcl_commands = zcl_commands
exports.zcl_commands_source_client = zcl_commands_source_client
exports.zcl_commands_source_server = zcl_commands_source_server
exports.zcl_events = zcl_events
exports.zcl_event_fields = zcl_event_fields
exports.zcl_command_tree = zcl_command_tree
exports.zcl_attributes = zcl_attributes
exports.zcl_attributes_client = zcl_attributes_client
exports.zcl_attributes_server = zcl_attributes_server
exports.zcl_atomics = zcl_atomics
exports.zcl_global_commands = zcl_global_commands
exports.zcl_cluster_largest_label_length = zcl_cluster_largest_label_length
exports.zcl_command_arguments_count = zcl_command_arguments_count
exports.zcl_command_arguments = zcl_command_arguments
exports.zcl_command_argument_data_type = zcl_command_argument_data_type

exports.is_client = isClient
exports.isClient = dep(isClient, { to: 'is_client' })

exports.is_server = isServer
exports.isServer = dep(isServer, { to: 'is_server' })

exports.is_str_equal = isStrEqual
exports.isStrEqual = dep(isStrEqual, { to: 'is_str_equal' })

exports.is_last_element = isLastElement
exports.isLastElement = dep(isLastElement, {
  to: 'is_last_element',
})

exports.is_first_element = isFirstElement
exports.isFirstElement = dep(isFirstElement, {
  to: 'is_first_element',
})

exports.is_enabled = isEnabled
exports.isEnabled = dep(isEnabled, { to: 'is_enabled' })

exports.is_command_available = isCommandAvailable
exports.isCommandAvailable = dep(isCommandAvailable, {
  to: 'is_command_available',
})

exports.as_underlying_zcl_type = asUnderlyingZclType
exports.asUnderlyingZclType = dep(asUnderlyingZclType, {
  to: 'as_underlying_zcl_type',
})

exports.if_is_bitmap = if_is_bitmap

exports.if_is_enum = if_is_enum

exports.is_bitmap = zclUtil.isBitmap
exports.isBitmap = dep(zclUtil.isBitmap, { to: 'is_bitmap' })

exports.is_struct = zclUtil.isStruct
exports.isStruct = dep(zclUtil.isStruct, { to: 'is_struct' })

exports.is_enum = zclUtil.isEnum
exports.isEnum = dep(zclUtil.isEnum, { to: 'is_enum' })

exports.is_event = zclUtil.isEvent
exports.isEvent = dep(zclUtil.isEvent, { to: 'is_event' })

exports.if_manufacturing_specific_cluster = dep(
  if_manufacturing_specific_cluster,
  { to: 'if_mfg_specific_cluster' }
)
exports.zcl_string_type_return = zcl_string_type_return
exports.is_zcl_string = is_zcl_string
exports.if_command_arguments_have_fixed_length =
  if_command_arguments_have_fixed_length
exports.command_arguments_total_length = command_arguments_total_length
exports.as_underlying_zcl_type_if_command_is_not_fixed_length =
  as_underlying_zcl_type_if_command_is_not_fixed_length
exports.if_command_argument_always_present = dep(
  if_command_argument_always_present,
  {
    to: 'if_command_is_not_fixed_length_but_command_argument_is_always_present',
  }
)
exports.as_underlying_zcl_type_command_argument_always_present = dep(
  as_underlying_zcl_type_command_argument_always_present,
  {
    to: 'as_underlying_zcl_type_command_is_not_fixed_length_but_command_argument_is_always_present',
  }
)
exports.if_command_argument_always_present_with_presentif = dep(
  if_command_argument_always_present_with_presentif,
  { to: 'if_ca_always_present_with_presentif' }
)
exports.as_underlying_zcl_type_command_argument_always_present_with_presentif =
  dep(as_underlying_zcl_type_command_argument_always_present_with_presentif, {
    to: 'as_underlying_zcl_type_ca_always_present_with_presentif',
  })
exports.if_command_argument_not_always_present_with_presentif = dep(
  if_command_argument_not_always_present_with_presentif,
  { to: 'if_ca_not_always_present_with_presentif' }
)
exports.as_underlying_zcl_type_command_argument_not_always_present_with_presentif =
  dep(
    as_underlying_zcl_type_command_argument_not_always_present_with_presentif,
    { to: 'as_underlying_zcl_type_ca_not_always_present_with_presentif' }
  )
exports.if_command_argument_not_always_present_no_presentif = dep(
  if_command_argument_not_always_present_no_presentif,
  { to: 'if_ca_not_always_present_no_presentif' }
)
exports.as_underlying_zcl_type_command_argument_not_always_present_no_presentif =
  dep(as_underlying_zcl_type_command_argument_not_always_present_no_presentif, {
    to: 'as_underlying_zcl_type_ca_not_always_present_no_presentif',
  })
exports.as_generated_default_macro = as_generated_default_macro
exports.attribute_mask = attribute_mask
exports.command_mask = command_mask
exports.format_zcl_string_as_characters_for_generated_defaults =
  format_zcl_string_as_characters_for_generated_defaults
exports.as_underlying_zcl_type_command_is_not_fixed_length_but_command_argument_is_always_present =
  dep(
    as_underlying_zcl_type_command_is_not_fixed_length_but_command_argument_is_always_present,
    'as_underlying_zcl_type_command_is_not_fixed_length_but_command_argument_is_always_present has been deprecated. Use as_underlying_zcl_type and if_command_not_fixed_length_command_argument_always_present instead'
  )
exports.as_underlying_zcl_type_ca_not_always_present_no_presentif = dep(
  as_underlying_zcl_type_ca_not_always_present_no_presentif,
  'as_underlying_zcl_type_ca_not_always_present_no_presentif has been deprecated. Use as_underlying_zcl_type and if_command_arg_not_always_present_no_presentif instead'
)
exports.as_underlying_zcl_type_ca_not_always_present_with_presentif = dep(
  as_underlying_zcl_type_ca_not_always_present_with_presentif,
  'as_underlying_zcl_type_ca_not_always_present_with_presentif has been deprecated. Use as_underlying_zcl_type and if_command_arg_not_always_present_with_presentif instead'
)
exports.as_underlying_zcl_type_ca_always_present_with_presentif = dep(
  as_underlying_zcl_type_ca_always_present_with_presentif,
  'as_underlying_zcl_type_ca_always_present_with_presentif has been deprecated. Use as_underlying_zcl_type and if_command_arg_always_present_with_presentif instead.'
)
exports.if_is_struct = if_is_struct
exports.if_mfg_specific_cluster = if_mfg_specific_cluster
exports.first_unused_enum_value = first_unused_enum_value
exports.zcl_commands_with_cluster_info = zcl_commands_with_cluster_info
exports.zcl_commands_with_arguments = zcl_commands_with_arguments
exports.if_is_string = if_is_string
exports.if_is_atomic = if_is_atomic
exports.if_is_number = if_is_number
exports.if_is_char_string = if_is_char_string
exports.if_is_octet_string = if_is_octet_string
exports.if_is_short_string = if_is_short_string
exports.if_is_long_string = if_is_long_string
