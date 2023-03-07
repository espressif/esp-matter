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
  BidirectionalStreamingCall,
  BidirectionalStreamingMethodStub,
  ServiceClient,
} from 'pigweedjs/pw_rpc';
import {Status} from 'pigweedjs/pw_status';
import {Chunk} from 'pigweedjs/protos/pw_transfer/transfer_pb';

export class ProgressStats {
  constructor(
    readonly bytesSent: number,
    readonly bytesConfirmedReceived: number,
    readonly totalSizeBytes?: number
  ) {}

  get percentReceived(): number {
    if (this.totalSizeBytes === undefined) {
      return NaN;
    }
    return (this.bytesConfirmedReceived / this.totalSizeBytes) * 100;
  }

  toString(): string {
    const total =
      this.totalSizeBytes === undefined
        ? 'undefined'
        : this.totalSizeBytes.toString();
    const percent = this.percentReceived.toFixed(2);
    return (
      `${percent}% (${this.bytesSent} B sent, ` +
      `${this.bytesConfirmedReceived} B received of ${total} B)`
    );
  }
}

export type ProgressCallback = (stats: ProgressStats) => void;

/** A Timer which invokes a callback after a certain timeout. */
class Timer {
  private task?: ReturnType<typeof setTimeout>;

  constructor(
    readonly timeoutS: number,
    private readonly callback: () => any
  ) {}

  /**
   * Starts a new timer.
   *
   * If a timer is already running, it is stopped and a new timer started.
   * This can be used to implement watchdog-like behavior, where a callback
   * is invoked after some time without a kick.
   */
  start() {
    this.stop();
    this.task = setTimeout(this.callback, this.timeoutS * 1000);
  }

  /** Terminates a running timer. */
  stop() {
    if (this.task !== undefined) {
      clearTimeout(this.task);
      this.task = undefined;
    }
  }
}

/**
 * A client-side data transfer through a Manager.
 *
 * Subclasses are responsible for implementing all of the logic for their type
 * of transfer, receiving messages from the server and sending the appropriate
 * messages in response.
 */
export abstract class Transfer {
  status: Status = Status.OK;
  done: Promise<Status>;
  protected data = new Uint8Array();

  private retries = 0;
  private responseTimer?: Timer;
  private resolve?: (value: Status | PromiseLike<Status>) => void;

  constructor(
    public id: number,
    protected sendChunk: (chunk: Chunk) => void,
    responseTimeoutS: number,
    private maxRetries: number,
    private progressCallback?: ProgressCallback
  ) {
    this.responseTimer = new Timer(responseTimeoutS, this.onTimeout);
    this.done = new Promise<Status>(resolve => {
      this.resolve = resolve!;
    });
  }

  /** Returns the initial chunk to notify the server of the transfer. */
  protected abstract get initialChunk(): Chunk;

  /** Handles a chunk that contains or requests data. */
  protected abstract handleDataChunk(chunk: Chunk): void;

  /** Retries after a timeout occurs. */
  protected abstract retryAfterTimeout(): void;

  /** Handles a timeout while waiting for a chunk. */
  private onTimeout = () => {
    this.retries += 1;
    if (this.retries > this.maxRetries) {
      this.finish(Status.DEADLINE_EXCEEDED);
      return;
    }

    console.debug(
      `Received no responses for ${this.responseTimer?.timeoutS}; retrying ${this.retries}/${this.maxRetries}`
    );

    this.retryAfterTimeout();
    this.responseTimer?.start();
  };

  /** Sends an error chunk to the server and finishes the transfer. */
  protected sendError(error: Status): void {
    const chunk = new Chunk();
    chunk.setStatus(error);
    chunk.setTransferId(this.id);
    chunk.setType(Chunk.Type.COMPLETION);
    this.sendChunk(chunk);
    this.finish(error);
  }

  /** Sends the initial chunk of the transfer. */
  begin(): void {
    this.sendChunk(this.initialChunk);
    this.responseTimer?.start();
  }

  /** Ends the transfer with the specified status. */
  finish(status: Status): void {
    this.responseTimer?.stop();
    this.responseTimer = undefined;
    this.status = status;

    if (status === Status.OK) {
      const totalSize = this.data.length;
      this.updateProgress(totalSize, totalSize, totalSize);
    }

    this.resolve!(this.status);
  }

  /** Invokes the provided progress callback, if any, with the progress */
  updateProgress(
    bytesSent: number,
    bytesConfirmedReceived: number,
    totalSizeBytes?: number
  ): void {
    const stats = new ProgressStats(
      bytesSent,
      bytesConfirmedReceived,
      totalSizeBytes
    );
    console.debug(`Transfer ${this.id} progress: ${stats}`);

    if (this.progressCallback !== undefined) {
      this.progressCallback(stats);
    }
  }

  /**
   *  Processes an incoming chunk from the server.
   *
   *  Handles terminating chunks (i.e. those with a status) and forwards
   *  non-terminating chunks to handle_data_chunk.
   */
  handleChunk(chunk: Chunk): void {
    this.responseTimer?.stop();
    this.retries = 0; // Received data from service, so reset the retries.

    console.debug(`Received chunk:(${chunk})`);

    // Status chunks are only used to terminate a transfer. They do not
    // contain any data that requires processing.
    if (chunk.hasStatus()) {
      this.finish(chunk.getStatus());
      return;
    }

    this.handleDataChunk(chunk);

    // Start the timeout for the server to send a chunk in response.
    this.responseTimer?.start();
  }
}

/**
 * A client <= server read transfer.
 *
 * Although typescript can effectively handle an unlimited transfer window, this
 * client sets a conservative window and chunk size to avoid overloading the
 * device. These are configurable in the constructor.
 */
export class ReadTransfer extends Transfer {
  private maxBytesToReceive: number;
  private maxChunkSize: number;
  private chunkDelayMicroS?: number; // Microseconds
  private remainingTransferSize?: number;
  private offset = 0;
  private pendingBytes: number;
  private windowEndOffset: number;

  // The fractional position within a window at which a receive transfer should
  // extend its window size to minimize the amount of time the transmitter
  // spends blocked.
  //
  // For example, a divisor of 2 will extend the window when half of the
  // requested data has been received, a divisor of three will extend at a third
  // of the window, and so on.
  private static EXTEND_WINDOW_DIVISOR = 2;

  data = new Uint8Array();

  constructor(
    id: number,
    sendChunk: (chunk: Chunk) => void,
    responseTimeoutS: number,
    maxRetries: number,
    progressCallback?: ProgressCallback,
    maxBytesToReceive = 8192,
    maxChunkSize = 1024,
    chunkDelayMicroS?: number
  ) {
    super(id, sendChunk, responseTimeoutS, maxRetries, progressCallback);
    this.maxBytesToReceive = maxBytesToReceive;
    this.maxChunkSize = maxChunkSize;
    this.chunkDelayMicroS = chunkDelayMicroS;
    this.pendingBytes = maxBytesToReceive;
    this.windowEndOffset = maxBytesToReceive;
  }

  protected get initialChunk(): Chunk {
    return this.transferParameters(Chunk.Type.START);
  }

  /** Builds an updated transfer parameters chunk to send the server. */
  private transferParameters(type: Chunk.TypeMap[keyof Chunk.TypeMap]): Chunk {
    this.pendingBytes = this.maxBytesToReceive;
    this.windowEndOffset = this.offset + this.maxBytesToReceive;

    const chunk = new Chunk();
    chunk.setTransferId(this.id);
    chunk.setPendingBytes(this.pendingBytes);
    chunk.setMaxChunkSizeBytes(this.maxChunkSize);
    chunk.setOffset(this.offset);
    chunk.setWindowEndOffset(this.windowEndOffset);
    chunk.setType(type);

    if (this.chunkDelayMicroS !== 0) {
      chunk.setMinDelayMicroseconds(this.chunkDelayMicroS!);
    }
    return chunk;
  }

  /**
   * Processes an incoming chunk from the server.
   *
   * In a read transfer, the client receives data chunks from the server.
   * Once all pending data is received, the transfer parameters are updated.
   */
  protected handleDataChunk(chunk: Chunk): void {
    if (chunk.getOffset() != this.offset) {
      // Initially, the transfer service only supports in-order transfers.
      // If data is received out of order, request that the server
      // retransmit from the previous offset.
      this.sendChunk(this.transferParameters(Chunk.Type.PARAMETERS_RETRANSMIT));
      return;
    }

    const oldData = this.data;
    const chunkData = chunk.getData() as Uint8Array;
    this.data = new Uint8Array(chunkData.length + oldData.length);
    this.data.set(oldData);
    this.data.set(chunkData, oldData.length);

    this.pendingBytes -= chunk.getData().length;
    this.offset += chunk.getData().length;

    if (chunk.hasRemainingBytes()) {
      if (chunk.getRemainingBytes() === 0) {
        // No more data to read. Acknowledge receipt and finish.
        const endChunk = new Chunk();
        endChunk.setTransferId(this.id);
        endChunk.setStatus(Status.OK);
        endChunk.setType(Chunk.Type.COMPLETION);
        this.sendChunk(endChunk);
        this.finish(Status.OK);
        return;
      }

      this.remainingTransferSize = chunk.getRemainingBytes();
    } else if (this.remainingTransferSize !== undefined) {
      // Update the remaining transfer size, if it is known.
      this.remainingTransferSize -= chunk.getData().length;

      if (this.remainingTransferSize <= 0) {
        this.remainingTransferSize = undefined;
      }
    }

    if (chunk.getWindowEndOffset() !== 0) {
      if (chunk.getWindowEndOffset() < this.offset) {
        console.error(
          `Transfer ${
            this.id
          }: transmitter sent invalid earlier end offset ${chunk.getWindowEndOffset()} (receiver offset ${
            this.offset
          })`
        );
        this.sendError(Status.INTERNAL);
        return;
      }

      if (chunk.getWindowEndOffset() < this.offset) {
        console.error(
          `Transfer ${
            this.id
          }: transmitter sent invalid later end offset ${chunk.getWindowEndOffset()} (receiver end offset ${
            this.windowEndOffset
          })`
        );
        this.sendError(Status.INTERNAL);
        return;
      }

      this.windowEndOffset = chunk.getWindowEndOffset();
      this.pendingBytes -= chunk.getWindowEndOffset() - this.offset;
    }

    const remainingWindowSize = this.windowEndOffset - this.offset;
    const extendWindow =
      remainingWindowSize <=
      this.maxBytesToReceive / ReadTransfer.EXTEND_WINDOW_DIVISOR;

    const totalSize =
      this.remainingTransferSize === undefined
        ? undefined
        : this.remainingTransferSize + this.offset;
    this.updateProgress(this.offset, this.offset, totalSize);

    if (this.pendingBytes === 0) {
      // All pending data was received. Send out a new parameters chunk
      // for the next block.
      this.sendChunk(this.transferParameters(Chunk.Type.PARAMETERS_RETRANSMIT));
    } else if (extendWindow) {
      this.sendChunk(this.transferParameters(Chunk.Type.PARAMETERS_CONTINUE));
    }
  }

  protected retryAfterTimeout(): void {
    this.sendChunk(this.transferParameters(Chunk.Type.PARAMETERS_RETRANSMIT));
  }
}

/**
 * A client => server write transfer.
 */
export class WriteTransfer extends Transfer {
  readonly data: Uint8Array;
  private windowId = 0;
  offset = 0;
  maxChunkSize = 0;
  chunkDelayMicroS?: number;
  windowEndOffset = 0;
  lastChunk: Chunk;

  constructor(
    id: number,
    data: Uint8Array,
    sendChunk: (chunk: Chunk) => void,
    responseTimeoutS: number,
    initialResponseTimeoutS: number,
    maxRetries: number,
    progressCallback?: ProgressCallback
  ) {
    super(id, sendChunk, responseTimeoutS, maxRetries, progressCallback);
    this.data = data;
    this.lastChunk = this.initialChunk;
  }

  protected get initialChunk(): Chunk {
    // TODO(frolv): The session ID should not be set here but assigned by the
    // server during an initial handshake.
    const chunk = new Chunk();
    chunk.setTransferId(this.id);
    chunk.setResourceId(this.id);
    chunk.setType(Chunk.Type.START);
    return chunk;
  }

  /**
   * Processes an incoming chunk from the server.
   *
   * In a write transfer, the server only sends transfer parameter updates
   * to the client. When a message is received, update local parameters and
   * send data accordingly.
   */
  protected handleDataChunk(chunk: Chunk): void {
    this.windowId += 1;
    const initialWindowId = this.windowId;

    if (!this.handleParametersUpdate(chunk)) {
      return;
    }

    const bytesAknowledged = chunk.getOffset();

    let writeChunk: Chunk;
    while (true) {
      writeChunk = this.nextChunk();
      this.offset += writeChunk.getData().length;
      const sentRequestedBytes = this.offset === this.windowEndOffset;

      this.updateProgress(this.offset, bytesAknowledged, this.data.length);
      this.sendChunk(writeChunk);

      if (sentRequestedBytes) {
        break;
      }
    }

    this.lastChunk = writeChunk;
  }

  /** Updates transfer state base on a transfer parameters update. */
  private handleParametersUpdate(chunk: Chunk): boolean {
    let retransmit = true;
    if (chunk.hasType()) {
      retransmit = chunk.getType() === Chunk.Type.PARAMETERS_RETRANSMIT;
    }

    if (chunk.getOffset() > this.data.length) {
      // Bad offset; terminate the transfer.
      console.error(
        `Transfer ${
          this.id
        }: server requested invalid offset ${chunk.getOffset()} (size ${
          this.data.length
        })`
      );

      this.sendError(Status.OUT_OF_RANGE);
      return false;
    }

    if (chunk.getPendingBytes() === 0) {
      console.error(
        `Transfer ${this.id}: service requested 0 bytes (invalid); aborting`
      );
      this.sendError(Status.INTERNAL);
      return false;
    }

    if (retransmit) {
      // Check whether the client has sent a previous data offset, which
      // indicates that some chunks were lost in transmission.
      if (chunk.getOffset() < this.offset) {
        console.debug(
          `Write transfer ${
            this.id
          } rolling back to offset ${chunk.getOffset()} from ${this.offset}`
        );
      }

      this.offset = chunk.getOffset();

      // Retransmit is the default behavior for older versions of the
      // transfer protocol. The window_end_offset field is not guaranteed
      // to be set in these version, so it must be calculated.
      const maxBytesToSend = Math.min(
        chunk.getPendingBytes(),
        this.data.length - this.offset
      );
      this.windowEndOffset = this.offset + maxBytesToSend;
    } else {
      // Extend the window to the new end offset specified by the server.
      this.windowEndOffset = Math.min(
        chunk.getWindowEndOffset(),
        this.data.length
      );
    }

    if (chunk.hasMaxChunkSizeBytes()) {
      this.maxChunkSize = chunk.getMaxChunkSizeBytes();
    }

    if (chunk.hasMinDelayMicroseconds()) {
      this.chunkDelayMicroS = chunk.getMinDelayMicroseconds();
    }
    return true;
  }

  /** Returns the next Chunk message to send in the data transfer. */
  private nextChunk(): Chunk {
    const chunk = new Chunk();
    chunk.setTransferId(this.id);
    chunk.setOffset(this.offset);
    chunk.setType(Chunk.Type.DATA);

    const maxBytesInChunk = Math.min(
      this.maxChunkSize,
      this.windowEndOffset - this.offset
    );

    chunk.setData(this.data.slice(this.offset, this.offset + maxBytesInChunk));

    // Mark the final chunk of the transfer.
    if (this.data.length - this.offset <= maxBytesInChunk) {
      chunk.setRemainingBytes(0);
    }
    return chunk;
  }

  protected retryAfterTimeout(): void {
    this.sendChunk(this.lastChunk);
  }
}
