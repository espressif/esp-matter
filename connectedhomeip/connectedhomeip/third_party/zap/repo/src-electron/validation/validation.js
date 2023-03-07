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
 * This module provides the APIs for validating inputs to the database, and returning flags indicating if
 * things were successful or not.
 *
 */

const queryZcl = require('../db/query-zcl.js')
const queryConfig = require('../db/query-config.js')
const queryEndpoint = require('../db/query-endpoint.js')
const types = require('../util/types.js')

async function validateAttribute(db, endpointTypeId, attributeRef, clusterRef) {
  let endpointAttribute = await queryZcl.selectEndpointTypeAttribute(
    db,
    endpointTypeId,
    attributeRef,
    clusterRef
  )
  let attribute = await queryZcl.selectAttributeById(db, attributeRef)
  return validateSpecificAttribute(endpointAttribute, attribute)
}

async function validateEndpoint(db, endpointId) {
  let endpoint = await queryEndpoint.selectEndpoint(db, endpointId)
  let currentIssues = validateSpecificEndpoint(endpoint)
  let noDuplicates = await validateNoDuplicateEndpoints(
    db,
    endpoint.endpointId,
    endpoint.sessionRef
  )
  if (!noDuplicates) {
    currentIssues.endpointId.push('Duplicate EndpointIds Exist')
  }
  return currentIssues
}

async function validateNoDuplicateEndpoints(
  db,
  endpointIdentifier,
  sessionRef
) {
  let count =
    await queryConfig.selectCountOfEndpointsWithGivenEndpointIdentifier(
      db,
      endpointIdentifier,
      sessionRef
    )
  return count.length <= 1
}

function validateSpecificAttribute(endpointAttribute, attribute) {
  let defaultAttributeIssues = []
  if (attribute.isNullable && endpointAttribute.defaultValue == null) {
    return { defaultValue: defaultAttributeIssues }
  } else if (!types.isString(attribute.type)) {
    if (types.isFloat(attribute.type)) {
      if (!isValidFloat(endpointAttribute.defaultValue))
        defaultAttributeIssues.push('Invalid Float')
      //Interpreting float values
      if (!checkAttributeBoundsFloat(attribute, endpointAttribute))
        defaultAttributeIssues.push('Out of range')
    } else if (types.isSignedInteger(attribute.type)) {
      if (!isValidSignedNumberString(endpointAttribute.defaultValue))
        defaultAttributeIssues.push('Invalid Integer')
      if (!checkAttributeBoundsInteger(attribute, endpointAttribute))
        defaultAttributeIssues.push('Out of range')
    } else {
      if (!isValidNumberString(endpointAttribute.defaultValue))
        defaultAttributeIssues.push('Invalid Integer')
      if (!checkAttributeBoundsInteger(attribute, endpointAttribute))
        defaultAttributeIssues.push('Out of range')
    }
  } else if (types.isString(attribute.type)) {
    let maxLengthForString =
      attribute.type == 'char_string' || attribute.type == 'octet_string'
        ? 254
        : 65534
    let maxAllowedLength = attribute.maxLength
      ? attribute.maxLength
      : maxLengthForString
    if (endpointAttribute.defaultValue.length > maxAllowedLength) {
      defaultAttributeIssues.push('String length out of range')
    }
  }
  return { defaultValue: defaultAttributeIssues }
}

function validateSpecificEndpoint(endpoint) {
  let zclEndpointIdIssues = []
  let zclNetworkIdIssues = []
  if (!isValidNumberString(endpoint.endpointId))
    zclEndpointIdIssues.push('EndpointId is invalid number string')
  if (
    extractIntegerValue(endpoint.endpointId) > 0xffff ||
    extractIntegerValue(endpoint.endpointId) < 0
  )
    zclEndpointIdIssues.push('EndpointId is out of valid range')
  if (!isValidNumberString(endpoint.networkId))
    zclNetworkIdIssues.push('NetworkId is invalid number string')
  if (extractIntegerValue(endpoint.endpointId) == 0)
    zclEndpointIdIssues.push('0 is not a valid endpointId')
  return {
    endpointId: zclEndpointIdIssues,
    networkId: zclNetworkIdIssues,
  }
}

//This applies to both actual numbers as well as octet strings.
function isValidNumberString(value) {
  //We test to see if the number is valid in hex. Decimals numbers also pass this test
  return /^(0x)?[\dA-F]+$/i.test(value) || Number.isInteger(Number(value))
}

function isValidSignedNumberString(value) {
  return /^(0x)?[\dA-F]+$/i.test(value) || Number.isInteger(Number(value))
}

function isValidFloat(value) {
  return !/^0x/i.test(value) && !isNaN(Number(value))
}

function extractFloatValue(value) {
  return parseFloat(value)
}

function extractIntegerValue(value) {
  if (/^-?\d+$/.test(value)) {
    return parseInt(value)
  } else if (/^[0-9A-F]+$/i.test(value)) {
    return parseInt(value, 16)
  } else {
    return parseInt(value, 16)
  }
}

function getBoundsInteger(attribute) {
  return {
    min: extractIntegerValue(attribute.min),
    max: extractIntegerValue(attribute.max),
  }
}

function checkAttributeBoundsInteger(attribute, endpointAttribute) {
  let { min, max } = getBoundsInteger(attribute)
  let defaultValue = extractIntegerValue(endpointAttribute.defaultValue)
  return checkBoundsInteger(defaultValue, min, max)
}

function checkBoundsInteger(defaultValue, min, max) {
  if (Number.isNaN(min)) min = Number.MIN_SAFE_INTEGER
  if (Number.isNaN(max)) max = Number.MAX_SAFE_INTEGER
  return defaultValue >= min && defaultValue <= max
}

function checkAttributeBoundsFloat(attribute, endpointAttribute) {
  let { min, max } = getBoundsFloat(attribute)
  let defaultValue = extractFloatValue(endpointAttribute.defaultValue)
  return checkBoundsFloat(defaultValue, min, max)
}

function getBoundsFloat(attribute) {
  return {
    min: extractFloatValue(attribute.min),
    max: extractFloatValue(attribute.max),
  }
}

function checkBoundsFloat(defaultValue, min, max) {
  if (Number.isNaN(min)) min = Number.MIN_VALUE
  if (Number.isNaN(max)) max = Number.MAX_VALUE
  return defaultValue >= min && defaultValue <= max
}

// exports
exports.validateAttribute = validateAttribute
exports.validateEndpoint = validateEndpoint
exports.validateNoDuplicateEndpoints = validateNoDuplicateEndpoints
exports.validateSpecificAttribute = validateSpecificAttribute
exports.validateSpecificEndpoint = validateSpecificEndpoint
exports.isValidNumberString = isValidNumberString
exports.isValidFloat = isValidFloat
exports.extractFloatValue = extractFloatValue
exports.extractIntegerValue = extractIntegerValue
exports.getBoundsInteger = getBoundsInteger
exports.checkBoundsInteger = checkBoundsInteger
exports.getBoundsFloat = getBoundsFloat
exports.checkBoundsFloat = checkBoundsFloat
