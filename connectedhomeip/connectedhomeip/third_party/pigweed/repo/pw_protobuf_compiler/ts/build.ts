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
import path from 'path';
import generateTemplate from './codegen/template_replacement';
// eslint-disable-next-line node/no-extraneous-import
import * as argModule from 'arg';
const arg = argModule.default;

const googProtobufPath = require.resolve('google-protobuf');
const googProtobufModule = fs.readFileSync(googProtobufPath, 'utf-8');

const args = arg({
  // Types
  '--proto': [String],
  '--out': String,

  // Aliases
  '-p': '--proto',
});

const protos = args['--proto'];
const outDir = args['--out'] || 'protos';

fs.mkdirSync(outDir, {recursive: true});

const run = function (executable: string, args: string[]) {
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

const protoc = async function (protos: string[], outDir: string) {
  const PROTOC_GEN_TS_PATH = path.resolve(
    path.dirname(require.resolve('ts-protoc-gen/generate.js')),
    '..',
    '.bin',
    'protoc-gen-ts'
  );

  await run('protoc', [
    `--plugin="protoc-gen-ts=${PROTOC_GEN_TS_PATH}"`,
    `--descriptor_set_out=${outDir}/descriptor.bin`,
    `--js_out=import_style=commonjs,binary:${outDir}`,
    `--ts_out=${outDir}`,
    `--proto_path=${process.cwd()}`,
    ...protos,
  ]);

  // ES6 workaround: Replace google-protobuf imports with entire library.
  protos.forEach(protoPath => {
    const outPath = path.join(outDir, protoPath.replace('.proto', '_pb.js'));

    if (fs.existsSync(outPath)) {
      let data = fs.readFileSync(outPath, 'utf8');
      data = data.replace("var jspb = require('google-protobuf');", googProtobufModule);
      data = data.replace('var goog = jspb;', '');
      fs.writeFileSync(outPath, data);
    }
  });
};

const makeProtoCollection = function (
  descriptorBinPath: string,
  protoPath: string,
  importPath: string
) {
  const outputCollectionName = path.extname(require.resolve("./ts_proto_collection.template")) === ".ts" ? "collection.ts" : "collection.js";
  generateTemplate(`${protoPath}/${outputCollectionName}`, descriptorBinPath, require.resolve("./ts_proto_collection.template"), importPath)
};

protoc(protos, outDir).then(() => {
  makeProtoCollection(
    path.join(outDir, 'descriptor.bin'),
    outDir,
    'pigweedjs/protos'
  );
});
