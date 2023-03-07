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

import commonjs from '@rollup/plugin-commonjs';
import resolve from '@rollup/plugin-node-resolve';
import pluginTypescript from '@rollup/plugin-typescript';
import path from 'path';
import dts from 'rollup-plugin-dts';
import nodePolyfills from 'rollup-plugin-node-polyfills';
import sourceMaps from 'rollup-plugin-sourcemaps';

import tsConfig from './tsconfig.json';

export default [
  // Bundle proto collection script
  {
    input: path.join('pw_protobuf_compiler', 'ts', 'build.ts'),
    output: [{
      file: path.join('dist', 'bin', 'pw_protobuf_compiler.js'),
      format: 'cjs',
      banner: '#!/usr/bin/env node\n\nconst window = null;'
    }],
    plugins: [
      pluginTypescript(
          {tsconfig: './tsconfig.json', exclude: ['**/*_test.ts']}),
      resolve(),
      commonjs(),

      // Resolve source maps to the original source
      sourceMaps()
    ]
  },
  // bundle proto collection template used by the above script
  {
    input: path.join(
        'pw_protobuf_compiler', 'ts', 'ts_proto_collection.template.ts'),
    output: [{
      file: path.join('dist', 'bin', 'ts_proto_collection.template.js'),
      format: 'esm',
      banner: '/* eslint-disable */'
    }],
    plugins: [
      pluginTypescript(
          {tsconfig: './tsconfig.json', exclude: ['**/*_test.ts']}),
      resolve(),
      commonjs(),

      // Resolve source maps to the original source
      sourceMaps()
    ]
  },
  // Bundle proto collection into one UMD file for consumption from browser
  {
    input: path.join('dist', 'protos', 'collection.ts'),
    output: [{
      file: path.join('dist', 'protos', 'collection.umd.js'),
      format: 'umd',
      sourcemap: true,
      name: 'PigweedProtoCollection',
    }],
    plugins: [
      pluginTypescript({tsconfig: './tsconfig.json'}),
      commonjs(),
      resolve(),

      // Resolve source maps to the original source
      sourceMaps()
    ]
  },
  // Types for proto collection
  {
    input: path.join(
        'dist', 'protos', 'types', 'dist', 'protos', 'collection.d.ts'),
    output:
        [{file: path.join('dist', 'protos', 'collection.d.ts'), format: 'es'}],
    plugins: [dts({compilerOptions: tsConfig.compilerOptions})]
  },
  // Bundle Pigweed modules
  {
    input: path.join('ts', 'index.ts'),
    output: [
      {
        file: path.join('dist', 'index.umd.js'),
        format: 'umd',
        sourcemap: true,
        name: 'Pigweed',
      },
      {
        file: path.join('dist', 'index.mjs'),
        format: 'esm',
        sourcemap: true,
      }
    ],
    plugins: [
      pluginTypescript(
          {tsconfig: './tsconfig.json', exclude: ['**/*_test.ts']}),
      nodePolyfills(),
      resolve(),
      commonjs(),

      // Resolve source maps to the original source
      sourceMaps()
    ]
  }
];
