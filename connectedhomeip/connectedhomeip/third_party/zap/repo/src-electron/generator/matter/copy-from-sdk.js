#!/usr/bin/env node
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

// NOTE: This is temporary script. It is used to temporarily provide
// a mechanism to keep Matter source in sync, as part of the larger
// cleanup exercise. Once that is done, this script will go away.

const fs = require('fs');
const path = require('path');

let args = process.argv.slice(2);

if (args[0] == null) {
  console.log(
    'Usage: node copy-from-sdk.js <MATTER_SDK_ROOT> [all|examples|src]'
  );
  process.exit(1);
}

let matterRoot = args[0];

let what = 'all';
if (args[1] != null) {
  what = args[1];
}

const srcDirectories = [
  'controller/java/templates',
  'controller/python/templates',
  'app/tests/suites',
  'app/tests/suites/examples/templates',
  'darwin/Framework/CHIP/templates',
  'app/zap-templates/templates/chip',
  'app/zap-templates/templates/app',
  'app/zap-templates/partials',
  'app/zap-templates/common',
  'app/zap-templates/common/simulated-clusters',
  'app/zap-templates/common/simulated-clusters/clusters',
  'app/zap-templates/common/attributes',
  'app/zap-templates/common/variables',
];

const examplesDirectories = [
  'darwin-framework-tool/templates',
  'darwin-framework-tool/templates/tests',
  'chip-tool/templates/tests',
  'chip-tool/templates',
  'placeholder/templates',
  'placeholder/linux/apps/app1',
];

if (what == 'all' || what == 'src')
  srcDirectories.forEach((dir) => {
    fs.mkdirSync(dir, { recursive: true });
    let dirPath = path.join(path.join(matterRoot, 'src'), dir);
    if (!fs.existsSync(dirPath)) {
      console.log(`Failed to locate: ${dirPath}`);
      process.exit(1);
    }
    console.log(`Reading: ${dirPath}`);
    fs.readdir(dirPath, (err, files) => {
      let jsFiles = files
        .filter((f) => f.endsWith('.js'))
        .map((f) => path.join(dirPath, f));

      jsFiles.forEach((f) => {
        console.log(`Copying: ${f}`);
        fs.copyFileSync(f, path.join(dir, path.basename(f)));
      });
    });
  });

if (what == 'all' || what == 'examples')
  examplesDirectories.forEach((dir) => {
    fs.mkdirSync(dir, { recursive: true });
    let dirPath = path.join(path.join(matterRoot, 'examples'), dir);
    if (!fs.existsSync(dirPath)) {
      console.log(`Failed to locate: ${dirPath}`);
      process.exit(1);
    }
    console.log(`Reading: ${dirPath}`);
    fs.readdir(dirPath, (err, files) => {
      let jsFiles = files
        .filter((f) => f.endsWith('.js'))
        .map((f) => path.join(dirPath, f));

      jsFiles.forEach((f) => {
        console.log(`Copying: ${f}`);
        fs.copyFileSync(f, path.join(dir, path.basename(f)));
      });
    });
  });
