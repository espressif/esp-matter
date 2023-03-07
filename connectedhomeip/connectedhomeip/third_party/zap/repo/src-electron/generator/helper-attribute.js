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

const queryAttribute = require('../db/query-attribute')
const templateUtil = require('./template-util')

async function featureBits(options) {
  if ('featureBits' in this) {
    let p = templateUtil.collectBlocks(this.featureBits, options, this)
    return templateUtil.templatePromise(this.global, p)
  } else {
    return ''
  }
}

/**
 * Valid within a cluster context, requires code.
 *
 * @returns Produces attribute defaults.
 */
async function attributeDefault(options) {
  if (!('id' in this)) throw new Error('Requires an id inside the context.')
  // If used at the toplevel, 'this' is the toplevel context object.
  // when used at the cluster level, 'this' is a cluster
  let code = parseInt(options.hash.code)

  let packageIds = await templateUtil.ensureZclPackageIds(this)
  let attr = await queryAttribute.selectAttributeByCode(
    this.global.db,
    packageIds,
    this.id,
    code,
    this.mfgCode
  )
  if (attr == null) {
    // Check if it's global attribute
    attr = await queryAttribute.selectAttributeByCode(
      this.global.db,
      packageIds,
      null,
      code,
      this.mfgCode
    )
  }
  let defs = await queryAttribute.selectGlobalAttributeDefaults(
    this.global.db,
    this.id,
    attr.id
  )
  let p = templateUtil.collectBlocks([defs], options, this)
  return templateUtil.templatePromise(this.global, p)
}

// WARNING! WARNING! WARNING! WARNING! WARNING! WARNING!
//
// Note: these exports are public API. Templates that might have been created in the past and are
// available in the wild might depend on these names.
// If you rename the functions, you need to still maintain old exports list.

exports.global_attribute_default = attributeDefault
exports.feature_bits = featureBits
