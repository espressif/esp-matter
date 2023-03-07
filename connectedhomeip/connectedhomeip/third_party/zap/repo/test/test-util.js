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

const path = require('path')

function testServer(fileName) {
  let testPort
  if (fileName.includes('server-bare.test')) {
    testPort = 9073
  } else if (fileName.includes('server-with-zcl.test')) {
    testPort = 9074
  } else if (fileName.includes('generation.test')) {
    testPort = 9075
  } else if (fileName.includes('server-session.test')) {
    testPort = 9076
  } else {
    let msg = new Error(
      `You must manually assign a port for the given test file: ${fileName}`
    )
    console.log(msg)
    throw msg
  }
  let ret = {
    port: testPort,
    baseUrl: `http://localhost:${testPort++}`,
  }
  return ret
}

const zto = 'ZAP_TEST_TIMEOUT'
const ztoShort = 'ZAP_TEST_TIMEOUT_SHORT'
const ztoMedium = 'ZAP_TEST_TIMEOUT_MEDIUM'
const ztoLong = 'ZAP_TEST_TIMEOUT_LONG'

exports.timeout = {
  short: () => {
    if (ztoShort in process.env) {
      return parseInt(process.env[ztoShort])
    } else if (zto in process.env) {
      return parseInt(process.env[zto])
    } else {
      return 1500
    }
  },
  medium: () => {
    if (ztoMedium in process.env) {
      return parseInt(process.env[ztoMedium])
    } else if (zto in process.env) {
      return parseInt(process.env[zto])
    } else {
      return 10000
    }
  },
  long: () => {
    if (ztoLong in process.env) {
      return parseInt(process.env[ztoLong])
    } else if (zto in process.env) {
      return parseInt(process.env[zto])
    } else {
      return 60000
    }
  },
}

exports.testTemplate = {
  zigbee: './test/gen-template/zigbee/gen-templates.json',
  zigbeeCount: 28,
  matter: './test/gen-template/matter/gen-test.json',
  matterCount: 7,
  matter2: './test/gen-template/matter2/templates.json',
  matter2Count: 1,
  matter3: './test/gen-template/matter3/t.json',
  matter3Count: 17,
  dotdot: './test/gen-template/dotdot/dotdot-templates.json',
  dotdotCount: 5,
  unittest: './test/gen-template/test/gen-test.json',
  testCount: 3,
  meta: './test/resource/meta/gen-test.json',
}

exports.testZclMetafile = path.join(__dirname, './resource/meta/zcl.json')

exports.testServer = testServer

exports.testCustomXml = './test/resource/custom-cluster/test-custom.xml'
exports.testCustomXml2 = './test/resource/custom-cluster/custom-dut.xml'
exports.customClusterXml =
  './test/resource/custom-cluster/custom-bead-cluster.xml'
exports.badTestCustomXml = './test/resource/custom-cluster/bad-test-custom.xml'

exports.totalClusterCount = 111
exports.totalDomainCount = 20
exports.totalCommandArgsCount = 1786
exports.totalCommandCount = 632
exports.totalEventFieldCount = 3
exports.totalEventCount = 1
exports.totalAttributeCount = 3438
exports.totalClusterCommandCount = 609
exports.totalServerAttributeCount = 2962
exports.totalSpecCount = 24
exports.totalEnumCount = 211
exports.totalNonAtomicEnumCount = 209
exports.totalDiscriminatorCount = 6
exports.totalEnumItemCount = 1595
exports.totalDotDotEnums = 106
exports.totalDotDotEnumItems = 637

exports.totalMatterClusters = 58
exports.totalMatterDeviceTypes = 40
exports.totalMatterCommandArgs = 424
exports.totalMatterCommands = 203
exports.totalMatterAttributes = 509
exports.totalMatterTags = 15
exports.totalMatterEvents = 56
exports.totalMatterEventFields = 83
