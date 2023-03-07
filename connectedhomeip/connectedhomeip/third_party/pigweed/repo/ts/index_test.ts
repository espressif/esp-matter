/**
 * @jest-environment jsdom
 */

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

import {
  pw_status,
  pw_hdlc,
  pw_rpc,
  pw_tokenizer,
  pw_transfer,
  WebSerial
} from "../dist/index.umd";

import {ProtoCollection} from "../dist/protos/collection.umd";
import * as fs from "fs";

describe('Pigweed Bundle', () => {

  it('proto collection has file list', () => {
    const protoCollection = new ProtoCollection();
    const fd = protoCollection.fileDescriptorSet.getFileList();
    expect(fd.length).toBeGreaterThan(0);
  });

  it('has pw_status enum defined', () => {
    const Status = pw_status.Status;
    expect(Status[Status.OUT_OF_RANGE]).toBeDefined();
  });

  it('has pw_hdlc frame, frame status, decoder and encoder defined', () => {
    expect(pw_hdlc.Frame).toBeDefined();
    expect(pw_hdlc.FrameStatus).toBeDefined();
    expect(pw_hdlc.Decoder).toBeDefined();
    expect(pw_hdlc.Encoder).toBeDefined();
  });

  it('has pw_rpc defined', () => {
    expect(pw_rpc.Client).toBeDefined();
    expect(pw_rpc.Rpc).toBeDefined();
    expect(pw_rpc.Channel).toBeDefined();
  });

  it('has pw_tokenizer defined', () => {
    expect(pw_tokenizer.Detokenizer).toBeDefined();
    expect(pw_tokenizer.PrintfDecoder).toBeDefined();
  });

  it('has pw_transfer defined', () => {
    expect(pw_transfer.Manager).toBeDefined();
  });

  it('has WebSerialTransport defined', () => {
    expect(WebSerial.WebSerialTransport).toBeDefined();
  });

  it('is not referring to any outside Pigweed modules', () => {
    const requireString = "require('pigweedjs";
    const file = fs.readFileSync(require.resolve("../dist/index.umd"));
    expect(file.indexOf(requireString)).toBe(-1)
  });

});
