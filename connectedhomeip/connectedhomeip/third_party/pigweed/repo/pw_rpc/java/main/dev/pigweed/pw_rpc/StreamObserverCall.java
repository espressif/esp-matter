// Copyright 2021 The Pigweed Authors
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

package dev.pigweed.pw_rpc;

import com.google.common.util.concurrent.AbstractFuture;
import com.google.protobuf.ByteString;
import com.google.protobuf.InvalidProtocolBufferException;
import com.google.protobuf.MessageLite;
import dev.pigweed.pw_log.Logger;
import dev.pigweed.pw_rpc.Call.ClientStreaming;
import java.util.function.Consumer;
import javax.annotation.Nullable;

/**
 * Represents an ongoing RPC call.
 *
 * <p>This call class implements all features of unary, server streaming, client streaming, and
 * bidirectional streaming RPCs. It provides static methods for creating call objects for each RPC
 * type.
 *
 * @param <RequestT> request type of the RPC; used for client or bidirectional streaming RPCs
 * @param <ResponseT> response type of the RPC; used for all types of RPCs
 */
class StreamObserverCall<RequestT extends MessageLite, ResponseT extends MessageLite>
    implements ClientStreaming<RequestT> {
  private static final Logger logger = Logger.forClass(StreamObserverCall.class);

  private final RpcManager rpcs;
  private final PendingRpc rpc;
  private final StreamObserver<ResponseT> observer;

  @Nullable private Status status = null;
  @Nullable private Status error = null;

  /** Base class for a Call that is a ListenableFuture. */
  private abstract static class StreamObserverFutureCall<RequestT extends MessageLite, ResponseT
                                                             extends MessageLite, ResultT>
      extends AbstractFuture<ResultT>
      implements ClientStreaming<RequestT>, StreamObserver<ResponseT> {
    private final StreamObserverCall<RequestT, ResponseT> call;

    private StreamObserverFutureCall(RpcManager rpcs, PendingRpc rpc) {
      call = new StreamObserverCall<>(rpcs, rpc, this);
    }

    void start(@Nullable RequestT request) {
      try {
        call.rpcs.start(call.rpc, call, request);
      } catch (ChannelOutputException e) {
        call.error = Status.UNKNOWN;
        setException(e);
      }
    }

    @Override
    public boolean cancel(boolean mayInterruptIfRunning) {
      try {
        call.cancel();
      } catch (ChannelOutputException e) {
        setException(e);
      }
      return super.cancel(mayInterruptIfRunning);
    }

    @Override
    public void cancel() {
      cancel(false);
    }

    @Override
    public void abandon() {
      call.abandon();
      super.cancel(false);
    }

    @Nullable
    @Override
    public Status status() {
      return call.status();
    }

    @Nullable
    @Override
    public Status error() {
      return call.error();
    }

    @Override
    public void send(RequestT request) throws ChannelOutputException, RpcError {
      call.send(request);
    }

    @Override
    public void finish() throws ChannelOutputException {
      call.finish();
    }

    @Override
    public void onError(Status status) {
      setException(new RpcError(call.rpc, status));
    }
  }

  /** Future-based Call class for unary and client streaming RPCs. */
  static class UnaryResponseFuture<RequestT extends MessageLite, ResponseT extends MessageLite>
      extends StreamObserverFutureCall<RequestT, ResponseT, UnaryResult<ResponseT>>
      implements Call.ClientStreamingFuture<RequestT, ResponseT> {
    @Nullable ResponseT response = null;

    UnaryResponseFuture(RpcManager rpcs, PendingRpc rpc, @Nullable RequestT request) {
      super(rpcs, rpc);
      start(request);
    }

    @Override
    public void onNext(ResponseT value) {
      if (response == null) {
        response = value;
      } else {
        setException(new IllegalStateException("Unary RPC received multiple responses."));
      }
    }

    @Override
    public void onCompleted(Status status) {
      if (response == null) {
        setException(new IllegalStateException("Unary RPC completed without a response payload"));
      } else {
        set(UnaryResult.create(response, status));
      }
    }
  }

  /** Future-based Call class for server and bidirectional streaming RPCs. */
  static class StreamResponseFuture<RequestT extends MessageLite, ResponseT extends MessageLite>
      extends StreamObserverFutureCall<RequestT, ResponseT, Status>
      implements Call.BidirectionalStreamingFuture<RequestT> {
    private final Consumer<ResponseT> onNext;

    StreamResponseFuture(
        RpcManager rpcs, PendingRpc rpc, Consumer<ResponseT> onNext, @Nullable RequestT request) {
      super(rpcs, rpc);
      this.onNext = onNext;
      start(request);
    }

    @Override
    public void onNext(ResponseT value) {
      onNext.accept(value);
    }

    @Override
    public void onCompleted(Status status) {
      set(status);
    }
  }

  /** Invokes the specified RPC. */
  static <RequestT extends MessageLite, ResponseT extends MessageLite>
      StreamObserverCall<RequestT, ResponseT> start(RpcManager rpcs,
          PendingRpc rpc,
          StreamObserver<ResponseT> observer,
          @Nullable MessageLite request) throws ChannelOutputException {
    StreamObserverCall<RequestT, ResponseT> call = new StreamObserverCall<>(rpcs, rpc, observer);
    rpcs.start(rpc, call, request);
    return call;
  }

  /** Invokes the specified RPC, ignoring errors that occur when the RPC is invoked. */
  static <RequestT extends MessageLite, ResponseT extends MessageLite>
      StreamObserverCall<RequestT, ResponseT> open(
          RpcManager rpcs, PendingRpc rpc, StreamObserver<ResponseT> observer) {
    StreamObserverCall<RequestT, ResponseT> call = new StreamObserverCall<>(rpcs, rpc, observer);
    rpcs.open(rpc, call);
    return call;
  }

  private StreamObserverCall(RpcManager rpcs, PendingRpc rpc, StreamObserver<ResponseT> observer) {
    this.rpcs = rpcs;
    this.rpc = rpc;
    this.observer = observer;
  }

  @Override
  public void cancel() throws ChannelOutputException {
    if (active()) {
      error = Status.CANCELLED;
      rpcs.cancel(rpc);
    }
  }

  @Override
  public void abandon() {
    if (active()) {
      error = Status.CANCELLED;
      rpcs.abandon(rpc);
    }
  }

  @Override
  @Nullable
  public Status status() {
    return status;
  }

  @Nullable
  @Override
  public Status error() {
    return error;
  }

  @Override
  public void send(RequestT request) throws ChannelOutputException, RpcError {
    if (error != null) {
      throw new RpcError(rpc, error);
    }
    if (status != null) {
      throw new RpcError(rpc, Status.FAILED_PRECONDITION);
    }
    rpcs.clientStream(rpc, request);
  }

  @Override
  public void finish() throws ChannelOutputException {
    if (active()) {
      rpcs.clientStreamEnd(rpc);
    }
  }

  void onNext(ByteString payload) {
    if (active()) {
      ResponseT message = parseResponse(payload);
      if (message != null) {
        observer.onNext(message);
      }
    }
  }

  void onCompleted(Status status) {
    if (active()) {
      this.status = status;
      observer.onCompleted(status);
    }
  }

  void onError(Status status) {
    if (active()) {
      this.error = status;
      observer.onError(status);
    }
  }

  @SuppressWarnings("unchecked")
  @Nullable
  private ResponseT parseResponse(ByteString payload) {
    try {
      return (ResponseT) rpc.method().decodeResponsePayload(payload);
    } catch (InvalidProtocolBufferException e) {
      logger.atWarning().withCause(e).log(
          "Failed to decode response for method %s; skipping packet", rpc.method().name());
      return null;
    }
  }
}
