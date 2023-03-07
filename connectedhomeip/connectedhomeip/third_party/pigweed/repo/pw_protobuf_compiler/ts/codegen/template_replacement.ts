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

import * as fs from 'fs';
import {FileDescriptorSet} from 'google-protobuf/google/protobuf/descriptor_pb';

function buildModulePath(rootDir: string, fileName: string): string {
  const name = `${rootDir}/${fileName}`;
  return name.replace(/\.proto$/, '_pb');
}

export default function generateTemplate(outputPath: string, descriptorDataPath: string, templatePath: string, protoRootDir: string) {
  let template = fs.readFileSync(templatePath).toString();

  const descriptorSetBinary = fs.readFileSync(descriptorDataPath);
  const base64DescriptorSet = descriptorSetBinary.toString('base64');
  const fileDescriptorSet = FileDescriptorSet.deserializeBinary(
    Buffer.from(descriptorSetBinary)
  );

  const imports = [];
  const moduleDictionary = [];
  const fileList = fileDescriptorSet.getFileList();
  for (let i = 0; i < fileList.length; i++) {
    const file = fileList[i];
    const modulePath = buildModulePath(".", file.getName()!);
    const moduleName = 'proto_' + i;
    imports.push(`import * as ${moduleName} from '${modulePath}';`);
    const key = file.getName()!;
    moduleDictionary.push(`['${key}', ${moduleName}],`);
  }

  template = template.replace(
    '{TEMPLATE_descriptor_binary}',
    base64DescriptorSet
  );
  template = template.replace('// TEMPLATE_proto_imports', imports.join('\n'));
  template = template.replace(
    '// TEMPLATE_module_map',
    moduleDictionary.join('\n')
  );

  fs.writeFileSync(outputPath, template);
}
