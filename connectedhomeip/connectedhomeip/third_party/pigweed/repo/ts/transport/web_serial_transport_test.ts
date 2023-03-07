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
import {last, take} from 'rxjs/operators';

import {SerialMock} from './serial_mock';
import {WebSerialTransport, DeviceLockedError} from './web_serial_transport';
import type {Serial} from "pigweedjs/types/serial"

describe('WebSerialTransport', () => {
  let serialMock: SerialMock;
  beforeEach(() => {
    serialMock = new SerialMock();
  });

  it('is disconnected before connecting', () => {
    const transport = new WebSerialTransport(serialMock as Serial);
    expect(transport.connected.getValue()).toBe(false);
  });

  it('reports that it has connected', async () => {
    const transport = new WebSerialTransport(serialMock as Serial);
    await transport.connect();
    expect(serialMock.serialPort.open).toHaveBeenCalled();
    expect(transport.connected.getValue()).toBe(true);
  });

  it('emits chunks as they arrive from the device', async () => {
    const transport = new WebSerialTransport(serialMock as Serial);
    await transport.connect();
    const data = new Uint8Array([1, 2, 3]);
    const emitted = transport.chunks.pipe(take(1)).toPromise();
    serialMock.dataFromDevice(data);

    expect(await emitted).toEqual(data);
    expect(transport.connected.getValue()).toBe(true);
    expect(serialMock.serialPort.readable.locked).toBe(true);
    expect(serialMock.serialPort.writable.locked).toBe(true);
  });

  it('is disconnected when it reaches the final chunk', async () => {
    const transport = new WebSerialTransport(serialMock as Serial);
    await transport.connect();
    const disconnectPromise = transport.connected
      .pipe(take(2), last())
      .toPromise();
    serialMock.closeFromDevice();

    expect(await disconnectPromise).toBe(false);
  });

  it('waits for the writer to be ready', async () => {
    const transport = new WebSerialTransport(serialMock as Serial);
    await transport.connect();
    const data = new Uint8Array([1, 2, 3]);

    const dataToDevice = serialMock.dataToDevice.pipe(take(1)).toPromise();

    let writtenData: Uint8Array | undefined = undefined;
    dataToDevice.then(data => {
      writtenData = data;
    });

    const sendPromise = transport.sendChunk(data);
    expect(writtenData).toBeUndefined();
    await sendPromise;
    expect(writtenData).toBeDefined();
  });

  it('sends chunks to the device', async () => {
    const transport = new WebSerialTransport(serialMock as Serial);
    await transport.connect();
    const data = new Uint8Array([1, 2, 3]);

    const dataToDevice = serialMock.dataToDevice.pipe(take(1)).toPromise();

    await transport.sendChunk(data);
    expect(await dataToDevice).toEqual(data);
  });

  it('throws an error on failing to connect', async () => {
    const connectError = new Error('Example connection error');
    const spy = jest.spyOn(serialMock, 'requestPort').mockImplementation(() => {
      throw connectError;
    });
    const transport = new WebSerialTransport(serialMock as Serial);
    await expect(transport.connect()).rejects.toThrow(connectError.message);
  });

  it("emits connection errors in the 'errors' observable", async () => {
    const transport = new WebSerialTransport(serialMock as Serial);
    await transport.connect();

    const reportedErrorPromise = transport.errors.pipe(take(1)).toPromise();
    serialMock.serialPort.errorFromDevice(new Error());

    expect(await reportedErrorPromise).toEqual(new DeviceLockedError());
  });
});
