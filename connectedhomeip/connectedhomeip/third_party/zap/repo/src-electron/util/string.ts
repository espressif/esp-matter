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
 * @module JS API: string utilities
 */

/**
 * Given a string convert it into a camelCased string
 *
 * @param {*} str
 * @parem {*} firstLower if True the it starts with lowecase.
 * @returns a spaced out string in lowercase
 */
function toCamelCase(label: string, firstLower = true) {
  let str = label.replace(/[+()&]/g, '').split(/ |_|-|\//)
  let res = ''
  for (let i = 0; i < str.length; i++) {
    if (i == 0 && firstLower) {
      res += str[i].charAt(0).toLowerCase()
    } else {
      res += str[i].charAt(0).toUpperCase()
    }
    // Acronyms, such as ZLL become lower-case.
    if (str[i].length > 1 && str[i].toUpperCase() == str[i]) {
      res += str[i].substring(1).toLowerCase()
    } else {
      res += str[i].substring(1)
    }
  }
  return res
}

function toSpacedLowercase(str: string) {
  let res = str
    .replace(/[+()&]/g, '')
    .replace(/\.?([A-Z][a-z])/g, function (x, y) {
      return ' ' + y
    })
  return res.toLowerCase()
}

/**
 * Takes a label, and delimits is on camelcasing.
 * For example:
 *    VerySimpleLabel will turn into VERY_SIMPLE_LABEL
 * @param {*} label
 */
function toSnakeCaseAllCaps(label: string) {
  let ret = ''
  if (label == null) return ret
  label = label
    .replace(/[+()&]/g, '')
    .replace(/\.?([A-Z][a-z])/g, function (x, y) {
      return '_' + y
    })
  let wasUp = false
  for (let i = 0; i < label.length; i++) {
    let ch = label.charAt(i)
    let upch = ch.toUpperCase()
    if (ch == '_') {
      ret = ret.concat('_')
      wasUp = true
    } else if (isDigit(ch)) {
      ret = ret.concat(ch)
      wasUp = false
    } else if (ch == upch) {
      // uppercase
      if (i != 0 && !wasUp) ret = ret.concat('_')
      ret = ret.concat(upch)
      wasUp = true
    } else {
      // lowercase
      ret = ret.concat(upch)
      wasUp = false
    }
  }
  return toCleanMacro(ret)
}

function toSnakeCase(str: string) {
  return toSnakeCaseAllCaps(str).toLowerCase()
}

/**
 * returns a string after converting ':' and '-' into '_'
 * @param {*} label
 */
function toCleanSymbol(label: string) {
  let l = label.trim()
  l = l.replace(/ /g, '_')
  l = l.replace(/[:/-]/g, '_')
  l = l.replace(/__+/g, '_').toLowerCase()
  return l
}

/**
 * returns a string after converting ':' and '_' into '-'
 * @param {*} label
 */
function toCleanSymbolAsKebabCase(label: string) {
  let l = label.trim()
  l = l.replace(/ /g, '-')
  l = l.replace(/[:/_]/g, '-')
  l = l.replace(/--+/g, '-').toLowerCase()
  return l
}

/**
 * Formats label as a C macro. This method performs a very simply substition
 * of illegal characters, such as ' ', ':' and such into a '_' character.
 *
 * @param {*} label
 * @returns Label formatted as C macro.
 */
function toCleanMacro(label: string) {
  let l = label.toUpperCase().replace(/ /g, '_')
  l = l.replace(/[:/-]/g, '_')
  while (l.includes('__')) {
    l = l.replace('__', '_')
  }
  l = l.replace('._', '_')
  l = l.replace('.', '_')
  l = l.replace('-', '_')
  l = l.startsWith('_') ? l.substring(1) : l
  l = l.endsWith('_') ? l.substring(0, l.length - 1) : l
  return l
}

/**
 * Returns true if given character is a digit.
 * @param {*} ch
 */
// @ts-ignore TypeScript doesn't have character type
function isDigit(ch) {
  return ch >= '0' && ch <= '9'
}

exports.toCamelCase = toCamelCase
exports.toCleanSymbol = toCleanSymbol
exports.toCleanMacro = toCleanMacro
exports.toSnakeCase = toSnakeCase
exports.toSpacedLowercase = toSpacedLowercase
exports.toSnakeCaseAllCaps = toSnakeCaseAllCaps
exports.isDigit = isDigit
exports.toCleanSymbolAsKebabCase = toCleanSymbolAsKebabCase
