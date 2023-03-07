// Copyright 2021 The Pigweed Authors
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy of
// the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations under
// the License.

/** Tools for compiling and importing Javascript protos on the fly. */

import {
  ProtoCollection as Base,
  ModuleMap,
} from 'pigweedjs/pw_protobuf_compiler';
import {FileDescriptorSet} from 'google-protobuf/google/protobuf/descriptor_pb';
import * as base64 from 'base64-js';

// Generated proto imports added during build
// TEMPLATE_proto_imports

const MODULE_MAP = new ModuleMap([
  // TEMPLATE_module_map
]);

const DESCRIPTOR_BASE64_BINARY = '{TEMPLATE_descriptor_binary}';

/**
 * A wrapper class of protocol buffer modules to provide convenience methods.
 */
export class ProtoCollection extends Base {
  constructor() {
    const fileDescriptorSet = FileDescriptorSet.deserializeBinary(
      base64.toByteArray(DESCRIPTOR_BASE64_BINARY)
    );
    super(fileDescriptorSet, MODULE_MAP);
  }
}
