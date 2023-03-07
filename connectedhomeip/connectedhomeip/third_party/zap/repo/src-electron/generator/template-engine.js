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
 * @module JS API: generator logic
 */

const _ = require('lodash')
const nativeRequire = require('../util/native-require')
const fsPromise = require('fs').promises
const promisedHandlebars = require('promised-handlebars')
const defaultHandlebars = require('handlebars')

const includedHelpers = [
  require('./helper-zcl'),
  require('./helper-zap'),
  require('./helper-c'),
  require('./helper-session'),
  require('./helper-endpointconfig'),
  require('./helper-sdkextension'),
  require('./helper-tokens'),
  require('./helper-attribute'),
  require('./helper-command'),
  require('./helper-future'),
  require('./helper-access'),
  require('./helper-zigbee-zcl'),

  require('./matter/controller/java/templates/helper'),
  require('./matter/controller/python/templates/helper'),
  require('./matter/darwin-framework-tool/templates/helper'),
  require('./matter/darwin/Framework/CHIP/templates/helper'),
  require('./matter/chip-tool/templates/tests/helper'),
  require('./matter/chip-tool/templates/helper'),
  require('./matter/app/zap-templates/templates/chip/helper'),
  require('./matter/app/zap-templates/templates/app/helper'),
  require('./matter/app/zap-templates/partials/helper'),
  require('./matter/app/zap-templates/common/ListHelper'),
  require('./matter/app/zap-templates/common/ClusterTestGeneration'),
  require('./matter/app/zap-templates/common/ChipTypesHelper'),
  require('./matter/app/zap-templates/common/attributes/Accessors'),
  require('./matter/app/zap-templates/common/StringHelper'),

  require('./meta/helper-meta'),
  require('./meta/helper-meta-2'),
]

const templateCompileOptions = {
  noEscape: true,
}

const precompiledTemplates = {}

const handlebarsInstance = {}

/**
 * Resolves into a precompiled template, either from previous precompile or freshly compiled.
 * @param {*} singleTemplatePkg
 * @returns templates
 */
async function produceCompiledTemplate(hb, singleTemplatePkg) {
  if (singleTemplatePkg.id in precompiledTemplates) {
    return precompiledTemplates[singleTemplatePkg.id]
  } else {
    let data = await fsPromise.readFile(singleTemplatePkg.path, 'utf8')
    let template = hb.compile(data, templateCompileOptions)
    precompiledTemplates[singleTemplatePkg.id] = template
    return template
  }
}

/**
 * Given db connection, session and a single template package, produce the output.
 *
 * @param {*} hb
 * @param {*} metaInfo
 * @param {*} db
 * @param {*} sessionId
 * @param {*} singlePkg
 * @param {*} overridePath: if passed, it provides a path to the override file that can override the overridable.js
 * @returns Promise that resolves with the 'utf8' string that contains the generated content.
 */
async function produceContent(
  hb,
  metaInfo,
  db,
  sessionId,
  singleTemplatePkg,
  genTemplateJsonPackageId,
  options = {
    overridePath: null,
    disableDeprecationWarnings: false,
  }
) {
  let template = await produceCompiledTemplate(hb, singleTemplatePkg)
  let context = {
    global: {
      disableDeprecationWarnings: options.disableDeprecationWarnings,
      deprecationWarnings: {},
      db: db,
      sessionId: sessionId,
      templatePath: singleTemplatePkg.path,
      promises: [],
      genTemplatePackageId: genTemplateJsonPackageId,
      overridable: loadOverridable(options.overridePath),
      resource: (key) => {
        if (key in metaInfo.resources) {
          return metaInfo.resources[key]
        } else {
          throw new Error(
            `Resource "${key}" not found among the context resources. Check your template.json file.`
          )
        }
      },
      stats: {},
    },
  }
  let content = await template(context)
  return {
    content: content,
    stats: context.global.stats,
  }
}

/**
 * This function attemps to call override function, but if override function
 * throws an exception, it calls the original function.
 *
 * @param {*} originalFn
 * @param {*} overrideFn
 * @returns result from override function, unless it throws an exception, in which case return result from original function.
 */
function wrapOverridable(originalFn, overrideFn) {
  return function () {
    try {
      return overrideFn.apply(this, arguments)
    } catch {
      return originalFn.apply(this, arguments)
    }
  }
}

/**
 * This function is responsible to load the overridable function container.
 *
 * @param {*} genTemplatePackageId
 */
function loadOverridable(overridePath) {
  let originals = require('./overridable.js')
  let shallowCopy = Object.assign({}, originals)
  if (overridePath == null) {
    return shallowCopy
  } else {
    let overrides = nativeRequire(overridePath)
    Object.keys(overrides).forEach((name) => {
      if (name in shallowCopy) {
        shallowCopy[name] = wrapOverridable(shallowCopy[name], overrides[name])
      } else {
        shallowCopy[name] = overrides[name]
      }
    })
    return shallowCopy
  }
}

/**
 * Function that loads the partials.
 *
 * @param {*} path
 */
async function loadPartial(hb, name, path) {
  try {
    let data = await fsPromise.readFile(path, 'utf8')
    hb.registerPartial(name, data)
  } catch (err) {
    console.log('Could not load partial ' + name + ': ' + err)
  }
}

function helperWrapper(wrappedHelper) {
  return function w(...args) {
    let helperName = wrappedHelper.name
    if (wrappedHelper.originalHelper != null) {
      helperName = wrappedHelper.originalHelper
    }
    let isDeprecated = false
    if (wrappedHelper.isDeprecated) {
      isDeprecated = true
    }
    if (helperName in this.global.stats) {
      this.global.stats[helperName].useCount++
    } else {
      this.global.stats[helperName] = {
        useCount: 1,
        isDeprecated: isDeprecated,
      }
    }
    try {
      return wrappedHelper.call(this, ...args)
    } catch (err) {
      let thrownObject
      let opts = args[args.length - 1]
      if ('loc' in opts) {
        let locMsg = ` [line: ${opts.loc.start.line}, column: ${opts.loc.start.column}, file: ${this.global.templatePath} ]`
        if (_.isString(err)) {
          thrownObject = new Error(err + locMsg)
        } else {
          thrownObject = err
          thrownObject.message = err.message + locMsg
        }
      }
      throw thrownObject
    }
  }
}
/**
 * Function that loads the helpers.
 *
 * @param {*} helpers - a string path if value is passed through CLI,
 *                      the nativeRequire() is leverage the native js function instead
 *                      of webpack's special sauce.
 *                      a required() module if invoked by backend js code.
 *                      this is required to force webpack to resolve the included files
 *                      as path will be difference after being packed for production.
 */
function loadHelper(hb, helpers) {
  // helper
  // when template path are passed via CLI
  // Other paths are 'required()' to workaround webpack path issue.
  if (_.isString(helpers)) {
    helpers = nativeRequire(helpers)
  }

  for (const singleHelper of Object.keys(helpers)) {
    if (singleHelper === 'meta') continue
    try {
      hb.registerHelper(singleHelper, helperWrapper(helpers[singleHelper]))
    } catch (err) {
      console.log('Could not load helper: ' + err)
    }
  }
}

/**
 * Returns an object that contains all the helper functions, keyed
 * by their name
 *
 * NOTE: This method is ONLY used for API testing. You should not use
 * this method for any real work inside the engine or something.
 *
 * @returns Object containing all the helper functions.
 */
function allBuiltInHelpers() {
  let allHelpers = {
    hasDuplicates: false,
    duplicates: [],
  }
  let h = []
  includedHelpers.forEach((helperPkg) => {
    for (const singleHelper of Object.keys(helperPkg)) {
      if (singleHelper === 'meta') continue
      let helperObject = {
        name: singleHelper,
        isDeprecated: helperPkg[singleHelper].isDeprecated ? true : false,
        category: helperPkg.meta?.category,
        alias: helperPkg.meta?.alias,
      }
      h.push(helperObject)
    }
  })
  allHelpers.helpers = h.sort((a, b) => {
    if (a.name != b.name) return a.name.localeCompare(b.name)
    if (a.category != null && b.category != null && a.category != b.category)
      return a.category.localeCompare(b.category)
    if (
      a.alias != null &&
      b.alias != null &&
      a.alias.length > 0 &&
      b.alias.length > 0 &&
      a.alias[0] != b.alias[0]
    )
      return a.alias[0].localeCompare(b.alias[0])

    allHelpers.duplicates.push(a.name)
    allHelpers.hasDuplicates = true
    return 0
  })
  return allHelpers
}

/**
 * Given an alias, this method finds a builtin helper package
 * by its alias.
 *
 * @param {*} alias
 * @returns Helper package or undefined if none was found.
 */
function findHelperPackageByAlias(alias) {
  let helpers = includedHelpers.filter((helperPkg) => {
    if (helperPkg.meta != null && helperPkg.meta.alias != null) {
      return helperPkg.meta.alias.includes(alias)
    }
  })
  if (helpers.length > 0) return helpers[0]
  else return undefined
}

/**
 * Global helper initialization
 */
function initializeBuiltInHelpersForPackage(
  hb,
  included = {
    aliases: [],
    categories: [],
  }
) {
  includedHelpers.forEach((helperPkg) => {
    let hasMatchingCategory = false
    let hasMeta = false
    let hasMatchingAlias = false

    // Let's analyze the things.
    if (helperPkg.meta != null) {
      hasMeta = true
      // Let's check if category matches.
      if (helperPkg.meta.category != null) {
        hasMatchingCategory = included.categories.includes(
          helperPkg.meta.category
        )
      }
      if (helperPkg.meta.alias != null && helperPkg.meta.alias.length > 0) {
        helperPkg.meta.alias.forEach((a) => {
          if (included.aliases.includes(a)) hasMatchingAlias = true
        })
      }
    }

    // Now let's see if we need to load it.
    // We will load all the helpers that have no `meta` object,
    // but if they do have 'meta' object, than either category or alias has to match.
    let loadIt
    if (hasMeta) {
      loadIt = hasMatchingAlias || hasMatchingCategory
    } else {
      loadIt = true
    }

    // We are not loading the helper if it has category or is aliased,
    // but that category or alias is not mentioned in the gen-templates.json.
    if (loadIt) {
      loadHelper(hb, helperPkg)
    }
  })
}

/**
 * This method returns the correct instance for a given generation flow.
 *
 * TBD: At this point it doesn't do anything yet, it's just
 * a central point to get the correct instance.
 *
 * @returns Instance of handlebars to be used.
 */
function hbInstance() {
  if (handlebarsInstance.default == null) {
    handlebarsInstance.default = promisedHandlebars(defaultHandlebars)
  }
  return handlebarsInstance.default
}

exports.produceContent = produceContent
exports.loadHelper = loadHelper
exports.loadPartial = loadPartial
exports.initializeBuiltInHelpersForPackage = initializeBuiltInHelpersForPackage
exports.allBuiltInHelpers = allBuiltInHelpers
exports.hbInstance = hbInstance
exports.findHelperPackageByAlias = findHelperPackageByAlias
