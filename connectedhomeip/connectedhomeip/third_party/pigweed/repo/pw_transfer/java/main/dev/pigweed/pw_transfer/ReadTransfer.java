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

import static dev.pigweed.pw_transfer.TransferProgress.UNKNOWN_TRANSFER_SIZE;
import static java.lang.Math.max;

import dev.pigweed.pw_log.Logger;
import dev.pigweed.pw_rpc.Status;
import dev.pigweed.pw_transfer.TransferEventHandler.TransferInterface;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;
import java.util.function.BooleanSupplier;
import java.util.function.Consumer;

class ReadTransfer extends Transfer<byte[]> {
  private static final Logger logger = Logger.forClass(ReadTransfer.class);

  // The fractional position within a window at which a receive transfer should
  // extend its window size to minimize the amount of time the transmitter
  // spends blocked.
  //
  // For example, a divisor of 2 will extend the window when half of the
  // requested data has been received, a divisor of three will extend at a third
  // of the window, and so on.
  private static final int EXTEND_WINDOW_DIVISOR = 2;

  // To minimize copies, store the ByteBuffers directly from the chunk protos in a list.
  private final List<ByteBuffer> dataChunks = new ArrayList<>();
  private int totalDataSize = 0;

  private final TransferParameters parameters;

  private long remainingTransferSize = UNKNOWN_TRANSFER_SIZE;

  private int offset = 0;
  private int windowEndOffset = 0;

  private int lastReceivedOffset = 0;

  ReadTransfer(int resourceId,
      ProtocolVersion desiredProtocolVersion,
      TransferInterface transferManager,
      TransferTimeoutSettings timeoutSettings,
      TransferParameters transferParameters,
      Consumer<TransferProgress> progressCallback,
      BooleanSupplier shouldAbortCallback) {
    super(resourceId,
        desiredProtocolVersion,
        transferManager,
        timeoutSettings,
        progressCallback,
        shouldAbortCallback);
    this.parameters = transferParameters;
    this.windowEndOffset = parameters.maxPendingBytes();
  }

  @Override
  State getWaitingForDataState() {
    return new ReceivingData();
  }

  @Override
  void prepareInitialChunk(VersionedChunk.Builder chunk) {
    setTransferParameters(chunk);
  }

  @Override
  VersionedChunk getChunkForRetry() {
    VersionedChunk chunk = getLastChunkSent();
    // If the last chunk sent was transfer parameters, send an updated RETRANSMIT chunk.
    if (chunk.type() == Chunk.Type.PARAMETERS_CONTINUE
        || chunk.type() == Chunk.Type.PARAMETERS_RETRANSMIT) {
      return prepareTransferParameters(/*extend=*/false);
    }
    return chunk;
  }

  private class ReceivingData extends ActiveState {
    @Override
    public void handleDataChunk(VersionedChunk chunk) throws TransferAbortedException {
      // Track the last seen offset so the DropRecovery state can detect retried packets.
      lastReceivedOffset = chunk.offset();

      if (chunk.offset() != offset) {
        logger.atFine().log("%s expected offset %d, received %d; resending transfer parameters",
            ReadTransfer.this,
            offset,
            chunk.offset());

        // For now, only in-order transfers are supported. If data is received out of order,
        // discard this data and retransmit from the last received offset.
        sendChunk(prepareTransferParameters(/*extend=*/false));
        changeState(new DropRecovery());
        setNextChunkTimeout();
        return;
      }

      // Add the underlying array(s) to a list to avoid making copies of the data.
      dataChunks.addAll(chunk.data().asReadOnlyByteBufferList());
      totalDataSize += chunk.data().size();

      offset += chunk.data().size();

      if (chunk.remainingBytes().isPresent()) {
        if (chunk.remainingBytes().getAsLong() == 0) {
          setStateTerminatingAndSendFinalChunk(Status.OK);
          return;
        }

        remainingTransferSize = chunk.remainingBytes().getAsLong();
      } else if (remainingTransferSize != UNKNOWN_TRANSFER_SIZE) {
        // If remaining size was not specified, update based on the most recent estimate, if any.
        remainingTransferSize = max(remainingTransferSize - chunk.data().size(), 0);
      }

      if (remainingTransferSize == UNKNOWN_TRANSFER_SIZE || remainingTransferSize == 0) {
        updateProgress(offset, offset, UNKNOWN_TRANSFER_SIZE);
      } else {
        updateProgress(offset, offset, offset + remainingTransferSize);
      }

      int remainingWindowSize = windowEndOffset - offset;
      boolean extendWindow =
          remainingWindowSize <= parameters.maxPendingBytes() / EXTEND_WINDOW_DIVISOR;

      if (remainingWindowSize == 0) {
        logger.atFiner().log(
            "%s received all pending bytes; sending transfer parameters update", ReadTransfer.this);
        sendChunk(prepareTransferParameters(/*extend=*/false));
      } else if (extendWindow) {
        sendChunk(prepareTransferParameters(/*extend=*/true));
      }
      setNextChunkTimeout();
    }
  }

  /** State for recovering from dropped packets. */
  private class DropRecovery extends ActiveState {
    @Override
    public void handleDataChunk(VersionedChunk chunk) throws TransferAbortedException {
      if (chunk.offset() == offset) {
        logger.atFine().log(
            "%s received expected offset %d, resuming transfer", ReadTransfer.this, offset);
        changeState(new ReceivingData()).handleDataChunk(chunk);
        return;
      }

      // To avoid a flood of identical parameters packets, only send one if a retry is detected.
      if (chunk.offset() == lastReceivedOffset) {
        logger.atFiner().log(
            "%s received repeated offset %d: retry detected, resending transfer parameters",
            ReadTransfer.this,
            lastReceivedOffset);
        sendChunk(prepareTransferParameters(/*extend=*/false));
      } else {
        lastReceivedOffset = chunk.offset();
        logger.atFiner().log("%s expecting offset %d, ignoring received offset %d",
            ReadTransfer.this,
            offset,
            chunk.offset());
      }
      setNextChunkTimeout();
    }
  }

  @Override
  void setFutureResult() {
    updateProgress(totalDataSize, totalDataSize, totalDataSize);

    ByteBuffer result = ByteBuffer.allocate(totalDataSize);
    dataChunks.forEach(result::put);
    getFuture().set(result.array());
  }

  private VersionedChunk prepareTransferParameters(boolean extend) {
    windowEndOffset = offset + parameters.maxPendingBytes();

    Chunk.Type type = extend ? Chunk.Type.PARAMETERS_CONTINUE : Chunk.Type.PARAMETERS_RETRANSMIT;
    return setTransferParameters(newChunk(type)).build();
  }

  private VersionedChunk.Builder setTransferParameters(VersionedChunk.Builder chunk) {
    chunk.setWindowEndOffset(offset + parameters.maxPendingBytes())
        .setMaxChunkSizeBytes(parameters.maxChunkSizeBytes())
        .setOffset(offset)
        .setWindowEndOffset(windowEndOffset);
    if (parameters.chunkDelayMicroseconds() > 0) {
      chunk.setMinDelayMicroseconds(parameters.chunkDelayMicroseconds());
    }
    return chunk;
  }
}
