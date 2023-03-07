/*
 *
 *    Copyright (c) 2021 Project CHIP Authors
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
const dbEnum = require('../../../../../src-shared/db-enum');

function hasArguments() {
  return !!this.arguments.length;
}

//
// Module exports
//
exports.hasArguments = hasArguments;

exports.meta = {
  category: dbEnum.helperCategory.matter,
  alias: [
    'darwin-framework-tool/templates/helper.js',
    'darwin-framework-tool-helper',
  ],
};
