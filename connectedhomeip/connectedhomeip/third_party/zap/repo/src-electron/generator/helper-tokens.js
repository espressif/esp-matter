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

const templateUtil = require('./template-util')
const types = require('../util/types.js')
const string = require('../util/string')
const helperC = require('./helper-c.js')

function token_cluster_create(config) {
  return {
    name: config.name,
    hexCode: config.hexCode,
    side: config.side,
    hasSingletons: false,
    singletons: {},
    hasNonSingletons: false,
    nonSingletons: {},
  }
}

/**
  This function builds creates a new context from the endpoint_config structure
  for use in the zap-tokens.h template. The endpoint_config context provides a
  list of endpoints, and endpointTypes, where each endpointType contains a list
  of clusters, and each cluster contains a list of attributes. However, the
  tokens template requires a list of attributes per endpoint, and per cluster,
  discriminating from singletons and non-singletons, so this function performs
  the required grouping.

  While each attribute contains an isSingleton attribute, the database schema
  allows for the same attribute to be returned both as singleton and non-singleton
  in different clusters, for different endpoints. In consequence, care must be
  taken to remove the singletons from the cluster and endpoint attribute lists.
  This is done in two steps, the first loop creates a global (context) list of
  singletons and non-singletons, and the second loop removes the singletons from
  the endpoint, and clusters.

  Clusters from different endpoints may have different attributes, therefore each
  endpoint keeps a separate list of clusters. Additionally, a context-level
  map of clusters is required in order to gather all attributes (singletons and
  non-singletons) from all endpoint clusters.
*/
async function tokens_context(options) {
  let context = {
    global: this.global,
    endpoints: {},
    clusters: {},
    singletons: {},
    nonSingletons: {},
    dictionary: {},
    maxSize: 1,
    token_id: 0xb000,
  }
  let promises = []
  let packageIds = await templateUtil.ensureZclPackageIds(this)
  // Loop through all the endpoints
  this.endpoints.forEach((endpoint) => {
    // Endpoint
    let ep = {
      id: endpoint.endpointId,
      name: endpoint.name,
      clusters: {},
      dictionary: {},
      hasNonSingletons: false,
      nonSingletons: {},
    }
    context.endpoints[ep.id] = ep
    this.endpointTypes.forEach((ept) => {
      // Endpoint Type
      if (endpoint.endpointTypeRef == ept.id) {
        // WARNING: Two different type may have the same cluster with a different CONFIGURATION
        ept.clusters.forEach((cluster) => {
          let ctx_cl = null // global cluster
          let ep_cl = null // endpoint cluster
          // Global cluster
          if (cluster.code in context.clusters) {
            ctx_cl = context.clusters[cluster.code]
          } else {
            ctx_cl = token_cluster_create(cluster)
            context.clusters[cluster.code] = ctx_cl
          }
          // Endpoint cluster
          if (cluster.code in ep.clusters) {
            ep_cl = ep.clusters[cluster.code]
          } else {
            ep_cl = token_cluster_create(cluster)
            ep.clusters[cluster.code] = ep_cl
          }
          // Attributes
          cluster.attributes.forEach((attribute) => {
            if ('NVM' == attribute.storage) {
              let name = string.toSnakeCase(attribute.name)
              let key = `${cluster.code}:${attribute.code}`
              if (key in context.singletons) {
                // Registered singleton, ignore
              } else if (key in context.nonSingletons) {
                let attr = context.nonSingletons[key]
                // Registered non-singleton
                if (attribute.isSingleton) {
                  // Move to singletons
                  context.singletons[key] = attr
                  ctx_cl.singletons[key] = attr
                  ep_cl.singletons[key] = attr
                  delete context.nonSingletons[key]
                } else {
                  // Add the registered non-singleton to the endpoint and cluster
                  ep_cl.nonSingletons[key] = attr
                  ep.nonSingletons[key] = attr
                }
              } else {
                // New attribute (singleton or non-singleton)
                let attr = {
                  global: this.global,
                  key: key,
                  name: name,
                  define: attribute.define,
                  hexCode: attribute.hexCode,
                  type: attribute.type,
                  typeSize: -1,
                  tokenType: helperC.asUnderscoreLowercase(attribute.define),
                  defaultValue: attribute.defaultValue,
                  longDefault: null,
                  maxLength: attribute.maxLength,
                  serverSide: 'server' == attribute.side,
                  isSingleton: attribute.isSingleton,
                  cluster: ep_cl,
                }
                // Register the new attribute
                if (attr.isSingleton) {
                  context.singletons[key] = attr
                  ctx_cl.singletons[key] = attr
                  ep_cl.singletons[key] = attr
                  delete context.nonSingletons[key]
                } else {
                  context.nonSingletons[key] = attr
                  ep_cl.nonSingletons[key] = attr
                  ep.nonSingletons[key] = attr
                }
                // Create a promise to load the long defaults
                promises.push(
                  types
                    .typeSizeAttribute(
                      this.global.db,
                      packageIds,
                      attribute,
                      `ERROR: ${attribute.name}, invalid size, ${attribute.type}`
                    )
                    .then((size) => {
                      attr.typeSize = size
                      if (size > 2) {
                        attr.longDefault = types.longTypeDefaultValue(
                          attr.typeSize,
                          attr.type,
                          attr.defaultValue
                        )
                      }
                      if (size > context.maxSize) {
                        context.maxSize = size
                      }
                    }) // then size
                ) // push(promise(type))
              } // new attribute
            } // NVM
          }) // attributes
        }) // clusters
      } // ept == ep.ept
    }) // enpoint types
  }) // endpoints

  // Fix global attributes and clusters
  context.hasSingletons = Object.keys(context.singletons).length > 0
  context.hasNonSingletons = Object.keys(context.nonSingletons).length > 0
  context.hasAttributes = context.hasSingletons || context.hasNonSingletons
  Object.entries(context.clusters).forEach(([code, cl]) => {
    cl.hasSingletons = Object.entries(cl.singletons).length > 0
  })

  // Fix endpoints and endpoint clusters
  Object.entries(context.endpoints).forEach(([epCode, ep]) => {
    Object.entries(ep.clusters).forEach(([clCode, cl]) => {
      Object.entries(cl.nonSingletons).forEach(([attCode, attr]) => {
        if (attr.key in context.singletons) {
          // These non-singletons were overridden with singletons
          delete cl.nonSingletons[attr.key]
          delete ep.nonSingletons[attr.key]
        }
      }) // non-singletons
      cl.hasSingletons = Object.entries(cl.singletons).length > 0
      cl.hasNonSingletons = Object.entries(cl.nonSingletons).length > 0
      // if(cl.hasSingletons) {
      //   context.clusters[cl.code] = cl
      // }
    }) // cluster
    ep.hasNonSingletons = Object.entries(ep.nonSingletons).length > 0
  }) // endpoints

  return templateUtil.templatePromise(
    this.global,
    Promise.all(promises).then(() => options.fn(context))
  )
}

/**
  The token template assigns an unique ID to each unique attribute. These IDs
  span all attributes from all clusters from all endpointTypes. This helper
  function allows the template to increment the token ID within the tokens context.
*/
function token_next(context) {
  context.token_id = context.token_id + 1
}

function debug_object(obj) {
  return JSON.stringify(obj)
}

// WARNING! WARNING! WARNING! WARNING! WARNING! WARNING!
//
// Note: these exports are public API. Templates that might have been created in the past and are
// available in the wild might depend on these names.
// If you rename the functions, you need to still maintain old exports list.

exports.tokens_context = tokens_context
exports.token_next = token_next
exports.debug_object = debug_object
