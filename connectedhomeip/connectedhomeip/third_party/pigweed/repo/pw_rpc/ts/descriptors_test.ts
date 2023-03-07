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

/* eslint-env browser */

import {ProtoCollection} from 'pigweedjs/protos/collection';
import {
  Request,
  Response,
} from 'pigweedjs/protos/pw_rpc/ts/test_pb';

import * as descriptors from './descriptors';

const TEST_PROTO_PATH = 'pw_rpc/ts/test_protos-descriptor-set.proto.bin';

describe('Descriptors', () => {
  it('parses from ServiceDescriptor binary', async () => {
    const protoCollection = new ProtoCollection();
    const fd = protoCollection.fileDescriptorSet.getFileList()
      .find((file: any) => file.array[1].indexOf("pw.rpc.test1") !== -1);
    const sd = fd.getServiceList()[0];
    const service = new descriptors.Service(
      sd,
      protoCollection,
      fd.getPackage()!
    );

    expect(service.name).toEqual('pw.rpc.test1.TheTestService');
    expect(service.methods.size).toEqual(4);

    const unaryMethod = service.methodsByName.get('SomeUnary')!;
    expect(unaryMethod.name).toEqual('SomeUnary');
    expect(unaryMethod.clientStreaming).toBe(false);
    expect(unaryMethod.serverStreaming).toBe(false);
    expect(unaryMethod.service).toEqual(service);
    expect(unaryMethod.requestType).toEqual(Request);
    expect(unaryMethod.responseType).toEqual(Response);

    const someBidiStreaming = service.methodsByName.get('SomeBidiStreaming')!;
    expect(someBidiStreaming.name).toEqual('SomeBidiStreaming');
    expect(someBidiStreaming.clientStreaming).toBe(true);
    expect(someBidiStreaming.serverStreaming).toBe(true);
    expect(someBidiStreaming.service).toEqual(service);
    expect(someBidiStreaming.requestType).toEqual(Request);
    expect(someBidiStreaming.responseType).toEqual(Response);
  });
});
