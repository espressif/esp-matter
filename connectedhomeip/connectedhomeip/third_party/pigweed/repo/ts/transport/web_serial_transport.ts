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
import {BehaviorSubject, Observable, Subject, Subscription} from 'rxjs';
import DeviceTransport from './device_transport';
import type {SerialPort, Serial, SerialOptions, Navigator, SerialPortFilter} from "pigweedjs/types/serial"

const DEFAULT_SERIAL_OPTIONS: SerialOptions & {baudRate: number} = {
  // Some versions of chrome use `baudrate` (linux)
  baudrate: 115200,
  // Some versions use `baudRate` (chromebook)
  baudRate: 115200,
  databits: 8,
  parity: 'none',
  stopbits: 1,
};

interface PortReadConnection {
  chunks: Observable<Uint8Array>;
  errors: Observable<Error>;
}

interface PortConnection extends PortReadConnection {
  sendChunk: (chunk: Uint8Array) => Promise<void>;
}

export class DeviceLostError extends Error {
  message = 'The device has been lost';
}

export class DeviceLockedError extends Error {
  message =
    "The device's port is locked. Try unplugging it" +
    ' and plugging it back in.';
}

/**
 * WebSerialTransport sends and receives UInt8Arrays to and
 * from a serial device connected over USB.
 */
export class WebSerialTransport implements DeviceTransport {
  chunks = new Subject<Uint8Array>();
  errors = new Subject<Error>();
  connected = new BehaviorSubject<boolean>(false);
  private portConnections: Map<SerialPort, PortConnection> = new Map();
  private activePortConnectionConnection: PortConnection | undefined;
  private rxSubscriptions: Subscription[] = [];

  constructor(
    private serial: Serial = (navigator as unknown as Navigator).serial,
    private filters: SerialPortFilter[] = [],
    private serialOptions = DEFAULT_SERIAL_OPTIONS
  ) { }

  /**
   * Send a UInt8Array chunk of data to the connected device.
   * @param {Uint8Array} chunk The chunk to send
   */
  async sendChunk(chunk: Uint8Array): Promise<void> {
    if (this.activePortConnectionConnection) {
      return this.activePortConnectionConnection.sendChunk(chunk);
    }
    throw new Error('Device not connected');
  }

  /**
   * Attempt to open a connection to a device. This includes
   * asking the user to select a serial port and should only
   * be called in response to user interaction.
   */
  async connect(): Promise<void> {
    const port = await this.serial.requestPort({filters: this.filters});
    await this.connectPort(port);
  }

  private disconnect() {
    for (const subscription of this.rxSubscriptions) {
      subscription.unsubscribe();
    }
    this.rxSubscriptions = [];

    this.activePortConnectionConnection = undefined;
    this.connected.next(false);
  }

  /**
   * Connect to a given SerialPort. This involves no user interaction.
   * and can be called whenever a port is available.
   */
  async connectPort(port: SerialPort): Promise<void> {
    this.disconnect();

    this.activePortConnectionConnection =
      this.portConnections.get(port) ?? (await this.conectNewPort(port));

    this.connected.next(true);

    this.rxSubscriptions.push(
      this.activePortConnectionConnection.chunks.subscribe(
        (chunk: any) => {
          this.chunks.next(chunk);
        },
        (err: any) => {
          throw new Error(`Chunks observable had an unexpected error ${err}`);
        },
        () => {
          this.connected.next(false);
          this.portConnections.delete(port);
          // Don't complete the chunks observable because then it would not
          // be able to forward any future chunks.
        }
      )
    );

    this.rxSubscriptions.push(
      this.activePortConnectionConnection.errors.subscribe((error: any) => {
        this.errors.next(error);
        if (error instanceof DeviceLostError) {
          // The device has been lost
          this.connected.next(false);
        }
      })
    );
  }

  private async conectNewPort(port: SerialPort): Promise<PortConnection> {
    await port.open(this.serialOptions);
    const writer = port.writable.getWriter();

    async function sendChunk(chunk: Uint8Array) {
      await writer.ready;
      await writer.write(chunk);
    }

    const {chunks, errors} = this.getChunks(port);

    const connection: PortConnection = {sendChunk, chunks, errors};
    this.portConnections.set(port, connection);
    return connection;
  }

  private getChunks(port: SerialPort): PortReadConnection {
    const chunks = new Subject<Uint8Array>();
    const errors = new Subject<Error>();

    async function read() {
      if (!port.readable) {
        throw new DeviceLostError();
      }
      if (port.readable.locked) {
        throw new DeviceLockedError();
      }
      await port.readable.pipeTo(
        new WritableStream({
          write: chunk => {
            chunks.next(chunk);
          },
          close: () => {
            chunks.complete();
            errors.complete();
          },
          abort: () => {
            // Reconnect to the port.
            connect();
          },
        })
      );
    }

    function connect() {
      read().catch(err => {
        // Don't error the chunks observable since that stops it from
        // reading any more packets, and we often want to continue
        // despite an error. Instead, push errors to the 'errors'
        // observable.
        errors.next(err);
      });
    }

    connect();

    return {chunks, errors};
  }
}
