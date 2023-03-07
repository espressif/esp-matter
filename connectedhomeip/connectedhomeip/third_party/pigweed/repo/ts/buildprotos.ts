// Copyright 2022 The Pigweed Authors
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

import {exec, ExecException} from 'child_process';
import fs from 'fs';

const run = function (executable: string, args: string[]) {
  console.log(args)
  return new Promise<void>(resolve => {
    exec(`${executable} ${args.join(" ")}`, {cwd: process.cwd()}, (error: ExecException | null, stdout: string | Buffer) => {
      if (error) {
        throw error;
      }

      console.log(stdout);
      resolve();
    });
  });
};

const protos = [
  'pw_transfer/transfer.proto',
  'pw_rpc/ts/test.proto',
  'pw_rpc/echo.proto',
  'pw_protobuf/pw_protobuf_protos/status.proto',
  'pw_protobuf/pw_protobuf_protos/common.proto',
  'pw_tokenizer/options.proto',
  'pw_log/log.proto',
  'pw_rpc/ts/test2.proto',
  'pw_rpc/internal/packet.proto',
  'pw_protobuf_compiler/pw_protobuf_compiler_protos/nested/more_nesting/test.proto',
  'pw_protobuf_compiler/pw_protobuf_compiler_protos/test.proto'
];

// Replace these import statements so they are actual paths to proto files.
const remapImports = {
  'pw_protobuf_protos/common.proto': 'pw_protobuf/pw_protobuf_protos/common.proto',
  'pw_tokenizer/proto/options.proto': 'pw_tokenizer/options.proto'
}

// Only modify the .proto files when running this builder and then restore any
// modified .proto files to their original states after the builder has finished
// running.
let restoreProtoList = [];
protos.forEach((protoPath) => {
  const protoData = fs.readFileSync(protoPath, 'utf-8');
  let newProtoData = protoData;
  Object.keys(remapImports).forEach((remapImportFrom) => {
    if (protoData.indexOf(`import "${remapImportFrom}"`) !== -1) {
      newProtoData = newProtoData
        .replaceAll(remapImportFrom, remapImports[remapImportFrom]);
    }
  });
  if (protoData !== newProtoData) {
    restoreProtoList.push([protoPath, protoData]);
    fs.writeFileSync(protoPath, newProtoData);
  }
});

run('ts-node', [
  `./pw_protobuf_compiler/ts/build.ts`,
  `--out dist/protos`
].concat(
  protos.map(proto => `-p ${proto}`)
))
  .then(() => {
    restoreProtoList.forEach((restoreProtoData) => {
      fs.writeFileSync(restoreProtoData[0], restoreProtoData[1]);
    });
  });
