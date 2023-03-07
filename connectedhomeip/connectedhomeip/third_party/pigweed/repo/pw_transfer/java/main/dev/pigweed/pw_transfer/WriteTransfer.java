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

package dev.pigweed.pw_transfer;

import static java.lang.Math.min;

import com.google.protobuf.ByteString;
import dev.pigweed.pw_log.Logger;
import dev.pigweed.pw_rpc.Status;
import dev.pigweed.pw_transfer.TransferEventHandler.TransferInterface;
import java.util.function.BooleanSupplier;
import java.util.function.Consumer;

class WriteTransfer extends Transfer<Void> {
  private static final Logger logger = Logger.forClass(WriteTransfer.class);

  // Short chunk delays often turn into much longer delays. Ignore delays <10ms to avoid impacting
  // performance.
  private static final int MIN_CHUNK_DELAY_TO_SLEEP_MICROS = 10000;

  private int maxChunkSizeBytes = 0;
  private int minChunkDelayMicros = 0;
  private int sentOffset;
  private long totalDroppedBytes;

  private final byte[] data;

  protected WriteTransfer(int resourceId,
      ProtocolVersion desiredProtocolVersion,
      TransferInterface transferManager,
      TransferTimeoutSettings timeoutSettings,
      byte[] data,
      Consumer<TransferProgress> progressCallback,
      BooleanSupplier shouldAbortCallback) {
    super(resourceId,
        desiredProtocolVersion,
        transferManager,
        timeoutSettings,
        progressCallback,
        shouldAbortCallback);
    this.data = data;
  }

  @Override
  void prepareInitialChunk(VersionedChunk.Builder chunk) {
    chunk.setRemainingBytes(data.length);
  }

  @Override
  State getWaitingForDataState() {
    return new WaitingForTransferParameters();
  }

  private class WaitingForTransferParameters extends ActiveState {
    @Override
    public void handleDataChunk(VersionedChunk chunk) throws TransferAbortedException {
      updateTransferParameters(chunk);
    }
  }

  /** Transmitting a transfer window. */
  private class Transmitting extends ActiveState {
    private final int windowStartOffset;
    private final int windowEndOffset;

    Transmitting(int windowStartOffset, int windowEndOffset) {
      this.windowStartOffset = windowStartOffset;
      this.windowEndOffset = windowEndOffset;
    }

    @Override
    public void handleDataChunk(VersionedChunk chunk) throws TransferAbortedException {
      updateTransferParameters(chunk);
    }

    @Override
    public void handleTimeout() throws TransferAbortedException {
      ByteString chunkData = ByteString.copyFrom(
          data, sentOffset, min(windowEndOffset - sentOffset, maxChunkSizeBytes));

      logger.atFiner().log("%s sending bytes %d-%d (%d B chunk, max size %d B)",
          WriteTransfer.this,
          sentOffset,
          sentOffset + chunkData.size() - 1,
          chunkData.size(),
          maxChunkSizeBytes);

      sendChunk(buildDataChunk(chunkData));

      sentOffset += chunkData.size();
      updateProgress(sentOffset, windowStartOffset, data.length);

      if (sentOffset < windowEndOffset) {
        setTimeoutMicros(minChunkDelayMicros);
        return; // Keep transmitting packets
      }
      setNextChunkTimeout();
      changeState(new WaitingForTransferParameters());
    }
  }

  @Override
  VersionedChunk getChunkForRetry() {
    // The service should resend transfer parameters if there was a timeout. In case the service
    // doesn't support timeouts and to avoid unnecessary waits, resend the last chunk. If there
    // were drops, this will trigger a transfer parameters update.
    return getLastChunkSent();
  }

  @Override
  void setFutureResult() {
    updateProgress(data.length, data.length, data.length);
    getFuture().set(null);
  }

  private void updateTransferParameters(VersionedChunk chunk) throws TransferAbortedException {
    logger.atFiner().log("%s received new chunk %s", this, chunk);

    if (chunk.offset() > data.length) {
      setStateTerminatingAndSendFinalChunk(Status.OUT_OF_RANGE);
      return;
    }

    int windowEndOffset = min(chunk.windowEndOffset(), data.length);
    if (chunk.requestsTransmissionFromOffset()) {
      long droppedBytes = sentOffset - chunk.offset();
      if (droppedBytes > 0) {
        totalDroppedBytes += droppedBytes;
        logger.atFine().log("%s retransmitting %d B (%d retransmitted of %d sent)",
            this,
            droppedBytes,
            totalDroppedBytes,
            sentOffset);
      }
      sentOffset = chunk.offset();
    } else if (windowEndOffset <= sentOffset) {
      logger.atFiner().log("%s ignoring old rolling window packet", this);
      setNextChunkTimeout();
      return; // Received an old rolling window packet, ignore it.
    }

    // Update transfer parameters if they're set.
    chunk.maxChunkSizeBytes().ifPresent(size -> maxChunkSizeBytes = size);
    chunk.minDelayMicroseconds().ifPresent(delay -> {
      if (delay > MIN_CHUNK_DELAY_TO_SLEEP_MICROS) {
        minChunkDelayMicros = delay;
      }
    });

    if (maxChunkSizeBytes == 0) {
      if (windowEndOffset == sentOffset) {
        logger.atWarning().log("%s server requested 0 bytes; aborting", this);
        setStateTerminatingAndSendFinalChunk(Status.INVALID_ARGUMENT);
        return;
      }
      // Default to sending the entire window if the max chunk size is not specified (or is 0).
      maxChunkSizeBytes = windowEndOffset - sentOffset;
    }

    // Enter the transmitting state and immediately send the first packet
    changeState(new Transmitting(chunk.offset(), windowEndOffset)).handleTimeout();
  }

  private VersionedChunk buildDataChunk(ByteString chunkData) {
    VersionedChunk.Builder chunk =
        newChunk(Chunk.Type.DATA).setOffset(sentOffset).setData(chunkData);

    // If this is the last data chunk, setRemainingBytes to 0.
    if (sentOffset + chunkData.size() == data.length) {
      logger.atFiner().log("%s sending final chunk with %d B", this, chunkData.size());
      chunk.setRemainingBytes(0);
    }
    return chunk.build();
  }
}
