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

/** Client for the pw_transfer service, which transmits data over pw_rpc. */

import {
  BidirectionalStreamingCall,
  BidirectionalStreamingMethodStub,
  ServiceClient,
} from 'pigweedjs/pw_rpc';
import {Status} from 'pigweedjs/pw_status';
import {Chunk} from 'pigweedjs/protos/pw_transfer/transfer_pb';

import {
  ReadTransfer,
  ProgressCallback,
  Transfer,
  WriteTransfer,
} from './transfer';

type TransferDict = {
  [key: number]: Transfer;
};

const DEFAULT_MAX_RETRIES = 3;
const DEFAULT_RESPONSE_TIMEOUT_S = 2;
const DEFAULT_INITIAL_RESPONSE_TIMEOUT = 4;

/**
 *  A manager for transmitting data through an RPC TransferService.
 *
 *  This should be initialized with an active Manager over an RPC channel. Only
 *  one instance of this class should exist for a configured RPC TransferService
 *  -- the Manager supports multiple simultaneous transfers.
 *
 *  When created, a Manager starts a separate thread in which transfer
 *  communications and events are handled.
 */
export class Manager {
  // Ongoing transfers in the service by ID
  private readTransfers: TransferDict = {};
  private writeTransfers: TransferDict = {};

  // RPC streams for read and write transfers. These are shareable by
  // multiple transfers of the same type.
  private readStream?: BidirectionalStreamingCall;
  private writeStream?: BidirectionalStreamingCall;

  /**
   * Initializes a Manager on top of a TransferService.
   *
   * Args:
   * @param{ServiceClient} service: the pw_rpc transfer service
   * client
   * @param{number} defaultResponseTimeoutS: max time to wait between receiving
   * packets
   * @param{number} initialResponseTimeoutS: timeout for the first packet; may
   * be longer to account for transfer handler initialization
   * @param{number} maxRetries: number of times to retry after a timeout
   */
  constructor(
    private service: ServiceClient,
    private defaultResponseTimeoutS = DEFAULT_RESPONSE_TIMEOUT_S,
    private initialResponseTimeoutS = DEFAULT_INITIAL_RESPONSE_TIMEOUT,
    private maxRetries = DEFAULT_MAX_RETRIES
  ) {}

  /**
   * Receives ("downloads") data from the server.
   *
   * @throws Throws an error when the transfer fails to complete.
   */
  async read(
    resourceId: number,
    progressCallback?: ProgressCallback
  ): Promise<Uint8Array> {
    if (resourceId in this.readTransfers) {
      throw new Error(
        `Read transfer for resource ${resourceId} already exists`
      );
    }
    const transfer = new ReadTransfer(
      resourceId,
      this.sendReadChunkCallback,
      this.defaultResponseTimeoutS,
      this.maxRetries,
      progressCallback
    );

    this.startReadTransfer(transfer);

    const status = await transfer.done;

    delete this.readTransfers[transfer.id];
    if (status !== Status.OK) {
      throw new TransferError(transfer.id, transfer.status);
    }
    return transfer.data;
  }

  /** Begins a new read transfer, opening the stream if it isn't. */
  startReadTransfer(transfer: Transfer): void {
    this.readTransfers[transfer.id] = transfer;

    if (this.readStream === undefined) {
      this.openReadStream();
    }
    console.debug(`Starting new read transfer ${transfer.id}`);
    transfer.begin();
  }

  /**
  Transmits (uploads) data to the server.
   *
   * @param{number} resourceId: ID of the resource to which to write.
   * @param{Uint8Array} data: Data to send to the server.
   */
  async write(
    resourceId: number,
    data: Uint8Array,
    progressCallback?: ProgressCallback
  ): Promise<void> {
    const transfer = new WriteTransfer(
      resourceId,
      data,
      this.sendWriteChunkCallback,
      this.defaultResponseTimeoutS,
      this.initialResponseTimeoutS,
      this.maxRetries,
      progressCallback
    );
    this.startWriteTransfer(transfer);

    const status = await transfer.done;

    delete this.writeTransfers[transfer.id];
    if (transfer.status !== Status.OK) {
      throw new TransferError(transfer.id, transfer.status);
    }
  }

  sendReadChunkCallback = (chunk: Chunk) => {
    this.readStream!.send(chunk);
  };

  sendWriteChunkCallback = (chunk: Chunk) => {
    this.writeStream!.send(chunk);
  };

  /** Begins a new write transfer, opening the stream if it isn't */
  startWriteTransfer(transfer: Transfer): void {
    this.writeTransfers[transfer.id] = transfer;

    if (!this.writeStream) {
      this.openWriteStream();
    }

    console.debug(`Starting new write transfer ${transfer.id}`);
    transfer.begin();
  }

  private openReadStream(): void {
    const readRpc = this.service.method(
      'Read'
    )! as BidirectionalStreamingMethodStub;
    this.readStream = readRpc.invoke(
      (chunk: Chunk) => {
        this.handleChunk(this.readTransfers, chunk);
      },
      () => {},
      this.onReadError
    );
  }

  private openWriteStream(): void {
    const writeRpc = this.service.method(
      'Write'
    )! as BidirectionalStreamingMethodStub;
    this.writeStream = writeRpc.invoke(
      (chunk: Chunk) => {
        this.handleChunk(this.writeTransfers, chunk);
      },
      () => {},
      this.onWriteError
    );
  }

  /**
   * Callback for an RPC error in the read stream.
   */
  private onReadError = (status: Status) => {
    if (status === Status.FAILED_PRECONDITION) {
      // FAILED_PRECONDITION indicates that the stream packet was not
      // recognized as the stream is not open. This could occur if the
      // server resets during an active transfer. Re-open the stream to
      // allow pending transfers to continue.
      this.openReadStream();
      return;
    }

    // Other errors are unrecoverable. Clear the stream and cancel any
    // pending transfers with an INTERNAL status as this is a system
    // error.
    this.readStream = undefined;

    for (const key in this.readTransfers) {
      const transfer = this.readTransfers[key];
      transfer.finish(Status.INTERNAL);
    }
    this.readTransfers = {};
    console.error(`Read stream shut down ${Status[status]}`);
  };

  private onWriteError = (status: Status) => {
    if (status === Status.FAILED_PRECONDITION) {
      // FAILED_PRECONDITION indicates that the stream packet was not
      // recognized as the stream is not open. This could occur if the
      // server resets during an active transfer. Re-open the stream to
      // allow pending transfers to continue.
      this.openWriteStream();
    } else {
      // Other errors are unrecoverable. Clear the stream and cancel any
      // pending transfers with an INTERNAL status as this is a system
      // error.
      this.writeStream = undefined;

      for (const key in this.writeTransfers) {
        const transfer = this.writeTransfers[key];
        transfer.finish(Status.INTERNAL);
      }
      this.writeTransfers = {};
      console.error(`Write stream shut down: ${Status[status]}`);
    }
  };

  /**
   * Processes an incoming chunk from a stream.
   *
   * The chunk is dispatched to an active transfer based on its ID. If the
   * transfer indicates that it is complete, the provided completion callback
   * is invoked.
   */
  private async handleChunk(transfers: TransferDict, chunk: Chunk) {
    const transfer = transfers[chunk.getTransferId()];
    if (transfer === undefined) {
      console.error(
        `TransferManager received chunk for unknown transfer ${chunk.getTransferId()}`
      );
      return;
    }
    transfer.handleChunk(chunk);
  }
}

/**
 * Exception raised when a transfer fails.
 *
 * Stores the ID of the failed transfer and the error that occured.
 */
class TransferError extends Error {
  id: number;
  status: Status;

  constructor(id: number, status: Status) {
    super(`Transfer ${id} failed with status ${Status[status]}`);
    this.status = status;
    this.id = id;
  }
}
