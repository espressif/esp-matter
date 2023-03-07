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

import static com.google.common.util.concurrent.MoreExecutors.directExecutor;
import static dev.pigweed.pw_transfer.TransferProgress.UNKNOWN_TRANSFER_SIZE;

import com.google.common.util.concurrent.SettableFuture;
import dev.pigweed.pw_log.Logger;
import dev.pigweed.pw_rpc.Status;
import dev.pigweed.pw_transfer.TransferEventHandler.TransferInterface;
import java.time.Duration;
import java.time.Instant;
import java.util.Locale;
import java.util.function.BooleanSupplier;
import java.util.function.Consumer;

/** Base class for tracking the state of a read or write transfer. */
abstract class Transfer<T> {
  private static final Logger logger = Logger.forClass(Transfer.class);

  // Largest nanosecond instant. Used to block indefinitely when no transfers are pending.
  static final Instant NO_TIMEOUT = Instant.ofEpochSecond(0, Long.MAX_VALUE);

  private final int resourceId;
  private final ProtocolVersion desiredProtocolVersion;
  private final TransferEventHandler.TransferInterface eventHandler;
  private final SettableFuture<T> future;
  private final TransferTimeoutSettings timeoutSettings;
  private final Consumer<TransferProgress> progressCallback;
  private final BooleanSupplier shouldAbortCallback;
  private final Instant startTime;

  private int sessionId = VersionedChunk.UNASSIGNED_SESSION_ID;
  private ProtocolVersion configuredProtocolVersion = ProtocolVersion.UNKNOWN;
  private Instant deadline = NO_TIMEOUT;
  private State state;
  private VersionedChunk lastChunkSent;

  // The number of times this transfer has retried due to an RPC disconnection. Limit this to
  // maxRetries to prevent repeated crashes if reading to / writing from a particular transfer is
  // causing crashes.
  private int disconnectionRetries = 0;
  private int lifetimeRetries = 0;

  /**
   * Creates a new read or write transfer.
   * @param resourceId The resource ID of the transfer
   * @param desiredProtocolVersion protocol version to request
   * @param eventHandler Interface to use to send a chunk.
   * @param timeoutSettings Timeout and retry settings for this transfer.
   * @param progressCallback Called each time a packet is sent.
   * @param shouldAbortCallback BooleanSupplier that returns true if a transfer should be aborted.
   */
  Transfer(int resourceId,
      ProtocolVersion desiredProtocolVersion,
      TransferInterface eventHandler,
      TransferTimeoutSettings timeoutSettings,
      Consumer<TransferProgress> progressCallback,
      BooleanSupplier shouldAbortCallback) {
    this.resourceId = resourceId;
    this.desiredProtocolVersion = desiredProtocolVersion;
    this.eventHandler = eventHandler;

    this.future = SettableFuture.create();
    this.timeoutSettings = timeoutSettings;
    this.progressCallback = progressCallback;
    this.shouldAbortCallback = shouldAbortCallback;

    // If the future is cancelled, tell the TransferEventHandler to cancel the transfer.
    future.addListener(() -> {
      if (future.isCancelled()) {
        eventHandler.cancelTransfer(this);
      }
    }, directExecutor());

    if (desiredProtocolVersion == ProtocolVersion.LEGACY) {
      // Legacy transfers skip protocol negotiation stage and use the resource ID as the session ID.
      configuredProtocolVersion = ProtocolVersion.LEGACY;
      assignSessionId(resourceId);
      state = getWaitingForDataState();
    } else {
      state = new Initiating();
    }

    startTime = Instant.now();
  }

  @Override
  public String toString() {
    return String.format(Locale.ENGLISH,
        "%s(%d:%d)[%s]",
        this.getClass().getSimpleName(),
        resourceId,
        sessionId,
        state.getClass().getSimpleName());
  }

  public final int getResourceId() {
    return resourceId;
  }

  public final int getSessionId() {
    return sessionId;
  }

  private void assignSessionId(int newSessionId) {
    sessionId = newSessionId;
    eventHandler.assignSessionId(this);
  }

  /** Terminates the transfer without sending any packets. */
  public final void terminate(TransferError error) {
    changeState(new Completed(error));
  }

  final Instant getDeadline() {
    return deadline;
  }

  final void setNextChunkTimeout() {
    deadline = Instant.now().plusMillis(timeoutSettings.timeoutMillis());
  }

  private void setInitialTimeout() {
    deadline = Instant.now().plusMillis(timeoutSettings.initialTimeoutMillis());
  }

  final void setTimeoutMicros(int timeoutMicros) {
    deadline = Instant.now().plusNanos((long) timeoutMicros * 1000);
  }

  final SettableFuture<T> getFuture() {
    return future;
  }

  final void start() {
    logger.atInfo().log(
        "%s starting with parameters: default timeout %d ms, initial timeout %d ms, %d max retires",
        this,
        timeoutSettings.timeoutMillis(),
        timeoutSettings.initialTimeoutMillis(),
        timeoutSettings.maxRetries());
    VersionedChunk.Builder chunk =
        VersionedChunk.createInitialChunk(desiredProtocolVersion, resourceId);
    prepareInitialChunk(chunk);
    try {
      sendChunk(chunk.build());
    } catch (TransferAbortedException e) {
      return; // Sending failed, transfer is cancelled
    }
    setInitialTimeout();
  }

  /** Processes an incoming chunk from the server. */
  final void handleChunk(VersionedChunk chunk) {
    // Since a packet has been received, don't allow retries on disconnection; abort instead.
    disconnectionRetries = Integer.MAX_VALUE;

    try {
      if (chunk.type() == Chunk.Type.COMPLETION) {
        state.handleFinalChunk(chunk.status().orElseGet(() -> {
          logger.atWarning().log("Received terminating chunk with no status set; using INTERNAL");
          return Status.INTERNAL.code();
        }));
      } else {
        state.handleDataChunk(chunk);
      }
    } catch (TransferAbortedException e) {
      // Transfer was aborted; nothing else to do.
    }
  }

  final void handleTimeoutIfDeadlineExceeded() {
    if (Instant.now().isAfter(deadline)) {
      logger.atFinest().log("%s timed out since the deadline %s has passed", this, deadline);
      try {
        state.handleTimeout();
      } catch (TransferAbortedException e) {
        // Transfer was aborted; nothing else to do.
      }
    }
  }

  final void handleTermination() {
    state.handleTermination();
  }

  final void handleCancellation() {
    state.handleCancellation();
  }

  /** Restarts a transfer after an RPC disconnection. */
  final void handleDisconnection() {
    // disconnectionRetries is set to Int.MAX_VALUE when a packet is received to prevent retries
    // after the initial packet.
    if (disconnectionRetries++ < timeoutSettings.maxRetries()) {
      logger.atFine().log("Restarting the pw_transfer RPC for %s (attempt %d/%d)",
          this,
          disconnectionRetries,
          timeoutSettings.maxRetries());
      try {
        sendChunk(getChunkForRetry());
      } catch (TransferAbortedException e) {
        return; // Transfer is aborted; nothing else to do.
      }
      setInitialTimeout();
    } else {
      changeState(new Completed(new TransferError("Transfer " + sessionId + " restarted "
              + timeoutSettings.maxRetries() + " times, aborting",
          Status.INTERNAL)));
    }
  }

  /** Returns the State to enter immediately after sending the first packet. */
  abstract State getWaitingForDataState();

  abstract void prepareInitialChunk(VersionedChunk.Builder chunk);

  /**
   * Returns the chunk to send for a retry. Returns the initial chunk if no chunks have been sent.
   */
  abstract VersionedChunk getChunkForRetry();

  /** Sets the result for the future after a successful transfer. */
  abstract void setFutureResult();

  final VersionedChunk.Builder newChunk(Chunk.Type type) {
    return VersionedChunk.builder()
        .setVersion(configuredProtocolVersion != ProtocolVersion.UNKNOWN ? configuredProtocolVersion
                                                                         : desiredProtocolVersion)
        .setType(type)
        .setSessionId(sessionId);
  }

  final VersionedChunk getLastChunkSent() {
    return lastChunkSent;
  }

  final State changeState(State newState) {
    if (newState != state) {
      logger.atFinest().log("%s state %s -> %s",
          this,
          state.getClass().getSimpleName(),
          newState.getClass().getSimpleName());
    }
    state = newState;
    return state;
  }

  /** Exception thrown when the transfer is aborted. */
  static class TransferAbortedException extends Exception {}

  /**
   * Sends a chunk.
   *
   * If sending fails, the transfer cannot proceed. sendChunk() sets the state to completed and
   * throws a TransferAbortedException.
   */
  final void sendChunk(VersionedChunk chunk) throws TransferAbortedException {
    lastChunkSent = chunk;
    if (shouldAbortCallback.getAsBoolean()) {
      logger.atWarning().log("Abort signal received.");
      changeState(new Completed(new TransferError(this, Status.ABORTED)));
      throw new TransferAbortedException();
    }

    try {
      logger.atFinest().log("%s sending %s", this, chunk);
      eventHandler.sendChunk(chunk.toMessage());
    } catch (TransferError transferError) {
      changeState(new Completed(transferError));
      throw new TransferAbortedException();
    }
  }

  /** Sends a status chunk to the server and finishes the transfer. */
  final void setStateTerminatingAndSendFinalChunk(Status status) throws TransferAbortedException {
    logger.atFine().log("%s sending final chunk with status %s", this, status);
    sendChunk(newChunk(Chunk.Type.COMPLETION).setStatus(status).build());
    if (configuredProtocolVersion == ProtocolVersion.VERSION_TWO) {
      changeState(new Terminating(status));
    } else {
      changeState(new Completed(status));
    }
  }

  /** Invokes the transfer progress callback and logs the progress. */
  final void updateProgress(long bytesSent, long bytesConfirmedReceived, long totalSizeBytes) {
    TransferProgress progress =
        TransferProgress.create(bytesSent, bytesConfirmedReceived, totalSizeBytes);
    progressCallback.accept(progress);

    long durationNanos = Duration.between(startTime, Instant.now()).toNanos();
    long totalRate = durationNanos == 0 ? 0 : (bytesSent * 1_000_000_000 / durationNanos);

    logger.atFine().log("%s progress: "
            + "%5.1f%% (%d B sent, %d B confirmed received of %s B total) at %d B/s",
        this,
        progress.percentReceived(),
        bytesSent,
        bytesConfirmedReceived,
        totalSizeBytes == UNKNOWN_TRANSFER_SIZE ? "unknown" : totalSizeBytes,
        totalRate);
  }

  interface State {
    /**
     * Called to handle a non-final chunk for this transfer.
     */
    void handleDataChunk(VersionedChunk chunk) throws TransferAbortedException;

    /**
     * Called to handle the final chunk for this transfer.
     */
    void handleFinalChunk(int statusCode) throws TransferAbortedException;

    /**
     * Called when this transfer's deadline expires.
     */
    void handleTimeout() throws TransferAbortedException;

    /**
     * Called if the transfer is cancelled by the user.
     */
    void handleCancellation();

    /**
     * Called when the transfer thread is shutting down.
     */
    void handleTermination();
  }

  /** Represents an active state in the transfer state machine. */
  abstract class ActiveState implements State {
    @Override
    public final void handleFinalChunk(int statusCode) throws TransferAbortedException {
      Status status = Status.fromCode(statusCode);
      if (status == null) {
        logger.atWarning().log("Received invalid status value %d, using INTERNAL", statusCode);
        status = Status.INTERNAL;
      }

      // If this is not version 2, immediately clean up. If it is, send the COMPLETION_ACK first and
      // clean up if that succeeded.
      if (configuredProtocolVersion == ProtocolVersion.VERSION_TWO) {
        sendChunk(newChunk(Chunk.Type.COMPLETION_ACK).build());
      }
      changeState(new Completed(status));
    }

    /** Enters the recovery state and returns to this state if recovery succeeds. */
    @Override
    public void handleTimeout() throws TransferAbortedException {
      changeState(new TimeoutRecovery(this)).handleTimeout();
    }

    @Override
    public final void handleCancellation() {
      try {
        setStateTerminatingAndSendFinalChunk(Status.CANCELLED);
      } catch (TransferAbortedException e) {
        // Transfer was aborted; nothing to do.
      }
    }

    @Override
    public final void handleTermination() {
      try {
        setStateTerminatingAndSendFinalChunk(Status.ABORTED);
      } catch (TransferAbortedException e) {
        // Transfer was aborted; nothing to do.
      }
    }
  }

  private class Initiating extends ActiveState {
    @Override
    public void handleDataChunk(VersionedChunk chunk) throws TransferAbortedException {
      assignSessionId(chunk.sessionId());

      if (chunk.version() == ProtocolVersion.UNKNOWN) {
        logger.atWarning().log(
            "%s aborting due to unsupported protocol version: %s", Transfer.this, chunk);
        setStateTerminatingAndSendFinalChunk(Status.INVALID_ARGUMENT);
        return;
      }

      changeState(getWaitingForDataState());

      if (chunk.type() != Chunk.Type.START_ACK) {
        logger.atFine().log(
            "%s got non-handshake chunk; reverting to legacy protocol", Transfer.this);
        configuredProtocolVersion = ProtocolVersion.LEGACY;
        state.handleDataChunk(chunk);
        return;
      }

      if (chunk.version().compareTo(desiredProtocolVersion) <= 0) {
        configuredProtocolVersion = chunk.version();
      } else {
        configuredProtocolVersion = desiredProtocolVersion;
      }

      logger.atFine().log("%s negotiated protocol %s (ours=%s, theirs=%s)",
          Transfer.this,
          configuredProtocolVersion,
          desiredProtocolVersion,
          chunk.version());

      VersionedChunk.Builder startAckConfirmation = newChunk(Chunk.Type.START_ACK_CONFIRMATION);
      prepareInitialChunk(startAckConfirmation);
      sendChunk(startAckConfirmation.build());
    }
  }

  /** Recovering from an expired timeout. */
  class TimeoutRecovery extends ActiveState {
    private final State nextState;
    private int retries;

    TimeoutRecovery(State nextState) {
      this.nextState = nextState;
    }

    @Override
    public void handleDataChunk(VersionedChunk chunk) throws TransferAbortedException {
      changeState(nextState).handleDataChunk(chunk);
    }

    @Override
    public void handleTimeout() throws TransferAbortedException {
      // If the transfer timed out, skip to the completed state. Don't send any more packets.
      if (retries >= timeoutSettings.maxRetries()) {
        logger.atFine().log("%s exhausted its %d retries", Transfer.this, retries);
        changeState(new Completed(Status.DEADLINE_EXCEEDED));
        return;
      }

      if (lifetimeRetries >= timeoutSettings.maxLifetimeRetries()) {
        logger.atFine().log("%s exhausted its %d lifetime retries", Transfer.this, retries);
        changeState(new Completed(Status.DEADLINE_EXCEEDED));
        return;
      }

      logger.atFiner().log("%s received no chunks for %d ms; retrying %d/%d",
          Transfer.this,
          timeoutSettings.timeoutMillis(),
          retries,
          timeoutSettings.maxRetries());
      sendChunk(getChunkForRetry());
      retries += 1;
      lifetimeRetries += 1;
      setNextChunkTimeout();
    }
  }

  /** Transfer completed. Do nothing if the transfer is terminated or cancelled. */
  class Terminating extends ActiveState {
    private final Status status;

    Terminating(Status status) {
      this.status = status;
    }

    @Override
    public void handleDataChunk(VersionedChunk chunk) {
      if (chunk.type() == Chunk.Type.COMPLETION_ACK) {
        changeState(new Completed(status));
      }
    }
  }

  class Completed implements State {
    /** Performs final cleanup of a completed transfer. No packets are sent to the server. */
    Completed(Status status) {
      cleanUp();
      logger.atInfo().log("%s completed with status %s", Transfer.this, status);
      if (status.ok()) {
        setFutureResult();
      } else {
        future.setException(new TransferError(Transfer.this, status));
      }
    }

    /** Finishes the transfer due to an exception. No packets are sent to the server. */
    Completed(TransferError exception) {
      cleanUp();
      logger.atWarning().withCause(exception).log("%s terminated with exception", Transfer.this);
      future.setException(exception);
    }

    private void cleanUp() {
      deadline = NO_TIMEOUT;
      eventHandler.unregisterTransfer(Transfer.this);
    }

    @Override
    public void handleDataChunk(VersionedChunk chunk) {
      logger.atFiner().log("%s [Completed state]: Received unexpected data chunk", Transfer.this);
    }

    @Override
    public void handleFinalChunk(int statusCode) {
      logger.atFiner().log("%s [Completed state]: Received unexpected data chunk", Transfer.this);
    }

    @Override
    public void handleTimeout() {}

    @Override
    public void handleTermination() {}

    @Override
    public void handleCancellation() {}
  }
}
