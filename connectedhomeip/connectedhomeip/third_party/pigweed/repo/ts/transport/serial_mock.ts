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
import {Subject} from 'rxjs';
import type {SerialConnectionEvent, SerialPort, Serial, SerialPortRequestOptions, SerialOptions} from "pigweedjs/types/serial"
/**
 * AsyncQueue is a queue that allows values to be dequeued
 * before they are enqueued, returning a promise that resolves
 * once the value is available.
 */
class AsyncQueue<T> {
  private queue: T[] = [];
  private requestQueue: Array<(val: T) => unknown> = [];

  /**
   * Enqueue val into the queue.
   * @param {T} val
   */
  enqueue(val: T) {
    const callback = this.requestQueue.shift();
    if (callback) {
      callback(val);
    } else {
      this.queue.push(val);
    }
  }

  /**
   * Dequeue a value from the queue, returning a promise
   * if the queue is empty.
   */
  async dequeue(): Promise<T> {
    const val = this.queue.shift();
    if (val !== undefined) {
      return val;
    } else {
      const queuePromise = new Promise<T>(resolve => {
        this.requestQueue.push(resolve);
      });
      return queuePromise;
    }
  }
}

/**
 * SerialPortMock is a mock for Chrome's upcoming SerialPort interface.
 * Since pw_web only depends on a subset of the interface, this mock
 * only implements that subset.
 */
class SerialPortMock implements SerialPort {
  private deviceData = new AsyncQueue<{
    data?: Uint8Array;
    done?: boolean;
    error?: Error;
  }>();

  /**
   * Simulate the device sending data to the browser.
   * @param {Uint8Array} data
   */
  dataFromDevice(data: Uint8Array) {
    this.deviceData.enqueue({data});
  }

  /**
   * Simulate the device closing the connection with the browser.
   */
  closeFromDevice() {
    this.deviceData.enqueue({done: true});
  }

  /**
   * Simulate an error in the device's read stream.
   * @param {Error} error
   */
  errorFromDevice(error: Error) {
    this.deviceData.enqueue({error});
  }

  /**
   * An rxjs subject tracking data sent to the (fake) device.
   */
  dataToDevice = new Subject<Uint8Array>();

  /**
   * The ReadableStream of bytes from the device.
   */
  readable = new ReadableStream<Uint8Array>({
    pull: async controller => {
      const {data, done, error} = await this.deviceData.dequeue();
      if (done) {
        controller.close();
        return;
      }
      if (error) {
        throw error;
      }
      if (data) {
        controller.enqueue(data);
      }
    },
  });

  /**
   * The WritableStream of bytes to the device.
   */
  writable = new WritableStream<Uint8Array>({
    write: chunk => {
      this.dataToDevice.next(chunk);
    },
  });

  /**
   * A spy for opening the serial port.
   */
  open = jest.fn(async (options?: SerialOptions) => { });

  /**
   * A spy for closing the serial port.
   */
  close = jest.fn(() => { });
}

export class SerialMock implements Serial {
  serialPort = new SerialPortMock();
  dataToDevice = this.serialPort.dataToDevice;
  dataFromDevice = (data: Uint8Array) => {
    this.serialPort.dataFromDevice(data);
  };
  closeFromDevice = () => {
    this.serialPort.closeFromDevice();
  };
  errorFromDevice = (error: Error) => {
    this.serialPort.errorFromDevice(error);
  };

  /**
   * Request the port from the browser.
   */
  async requestPort(options?: SerialPortRequestOptions) {
    return this.serialPort;
  }

  // The rest of the methods are unimplemented
  // and only exist to ensure SerialMock implements Serial

  onconnect(): ((this: this, ev: SerialConnectionEvent) => any) | null {
    throw new Error('Method not implemented.');
  }

  ondisconnect(): ((this: this, ev: SerialConnectionEvent) => any) | null {
    throw new Error('Method not implemented.');
  }

  getPorts(): Promise<SerialPort[]> {
    throw new Error('Method not implemented.');
  }

  addEventListener(
    type: 'connect' | 'disconnect',
    listener: (this: this, ev: SerialConnectionEvent) => any,
    useCapture?: boolean
  ): void;

  addEventListener(
    type: string,
    listener: EventListener | EventListenerObject | null,
    options?: boolean | AddEventListenerOptions
  ): void;

  addEventListener(type: any, listener: any, options?: any) {
    throw new Error('Method not implemented.');
  }

  removeEventListener(
    type: 'connect' | 'disconnect',
    callback: (this: this, ev: SerialConnectionEvent) => any,
    useCapture?: boolean
  ): void;

  removeEventListener(
    type: string,
    callback: EventListener | EventListenerObject | null,
    options?: boolean | EventListenerOptions
  ): void;

  removeEventListener(type: any, callback: any, options?: any) {
    throw new Error('Method not implemented.');
  }

  dispatchEvent(event: Event): boolean {
    throw new Error('Method not implemented.');
  }
}
