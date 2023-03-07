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

import com.google.protobuf.MessageLite;
import dev.pigweed.pw_rpc.StreamObserverCall.StreamResponseFuture;
import dev.pigweed.pw_rpc.StreamObserverCall.UnaryResponseFuture;
import java.util.function.Consumer;

/**
 * Represents a method ready to be invoked on a particular RPC channel.
 *
 * <p>The Client has the concrete MethodClient as a type parameter. This allows implementations to
 * fully define the interface and semantics for RPC calls.
 */
public class MethodClient {
  protected final StreamObserver<? extends MessageLite> defaultObserver;
  private final RpcManager rpcs;
  private final PendingRpc rpc;

  MethodClient(RpcManager rpcs, PendingRpc rpc, StreamObserver<MessageLite> defaultObserver) {
    this.rpcs = rpcs;
    this.rpc = rpc;
    this.defaultObserver = defaultObserver;
  }

  public final Method method() {
    return rpc.method();
  }

  /** Gives implementations access to the RpcManager shared with the Client. */
  protected final RpcManager rpcs() {
    return rpcs;
  }

  /** Gives implementations access to the PendingRpc this MethodClient represents. */
  protected final PendingRpc rpc() {
    return rpc;
  }

  /** Invokes a unary RPC. Uses the default StreamObserver for RPC events. */
  public Call invokeUnary(MessageLite request) throws ChannelOutputException {
    return invokeUnary(request, defaultObserver());
  }

  /** Invokes a unary RPC. Uses the provided StreamObserver for RPC events. */
  public Call invokeUnary(MessageLite request, StreamObserver<? extends MessageLite> observer)
      throws ChannelOutputException {
    checkCallType(Method.Type.UNARY);
    return StreamObserverCall.start(rpcs(), rpc(), observer, request);
  }

  /** Invokes a unary RPC with a future that collects the response. */
  public <ResponseT extends MessageLite> Call.UnaryFuture<ResponseT> invokeUnaryFuture(
      MessageLite request) {
    checkCallType(Method.Type.UNARY);
    return new UnaryResponseFuture<>(rpcs(), rpc(), request);
  }

  /**
   * Creates a call object for a unary RPC without starting the RPC on the server. This can be used
   * to start listening to responses to an RPC before the RPC server is available.
   *
   * <p>The RPC remains open until it is completed by the server with a response or error packet or
   * cancelled.
   */
  public Call openUnary(StreamObserver<? extends MessageLite> observer) {
    checkCallType(Method.Type.UNARY);
    return StreamObserverCall.open(rpcs(), rpc(), observer);
  }

  /**
   * @deprecated The request argument is deprecated and unused. Use the single-argument version.
   */
  @Deprecated
  public Call openUnary(MessageLite unusedRequest, StreamObserver<? extends MessageLite> observer) {
    checkCallType(Method.Type.UNARY);
    return StreamObserverCall.open(rpcs(), rpc(), observer);
  }

  /** Invokes a server streaming RPC. Uses the default StreamObserver for RPC events. */
  public Call invokeServerStreaming(MessageLite request) throws ChannelOutputException {
    return invokeServerStreaming(request, defaultObserver());
  }

  /** Invokes a server streaming RPC. Uses the provided StreamObserver for RPC events. */
  public Call invokeServerStreaming(MessageLite request,
      StreamObserver<? extends MessageLite> observer) throws ChannelOutputException {
    checkCallType(Method.Type.SERVER_STREAMING);
    return StreamObserverCall.start(rpcs(), rpc(), observer, request);
  }

  /** Invokes a server streaming RPC with a future that collects the responses. */
  public Call.ServerStreamingFuture invokeServerStreamingFuture(
      MessageLite request, Consumer<? extends MessageLite> onNext) {
    checkCallType(Method.Type.SERVER_STREAMING);
    return new StreamResponseFuture<>(rpcs(), rpc(), onNext, request);
  }

  /**
   * Creates a call object for a server streaming RPC without starting the RPC on the server. This
   * can be used to start listening to responses to an RPC before the RPC server is available.
   *
   * <p>The RPC remains open until it is completed by the server with a response or error packet or
   * cancelled.
   */
  public Call openServerStreaming(StreamObserver<? extends MessageLite> observer) {
    checkCallType(Method.Type.SERVER_STREAMING);
    return StreamObserverCall.open(rpcs(), rpc(), observer);
  }

  /** Invokes a client streaming RPC. Uses the default StreamObserver for RPC events. */
  public <RequestT extends MessageLite> Call.ClientStreaming<RequestT> invokeClientStreaming()
      throws ChannelOutputException {
    return invokeClientStreaming(defaultObserver());
  }

  /** Invokes a client streaming RPC. Uses the provided StreamObserver for RPC events. */
  public <RequestT extends MessageLite> Call.ClientStreaming<RequestT> invokeClientStreaming(
      StreamObserver<? extends MessageLite> observer) throws ChannelOutputException {
    checkCallType(Method.Type.CLIENT_STREAMING);
    return StreamObserverCall.start(rpcs(), rpc(), observer, null);
  }

  /** Invokes a client streaming RPC with a future that collects the response. */
  public <RequestT extends MessageLite> Call.ClientStreaming<RequestT>
  invokeClientStreamingFuture() {
    checkCallType(Method.Type.CLIENT_STREAMING);
    return new UnaryResponseFuture<>(rpcs(), rpc(), null);
  }

  /**
   * Creates a call object for a client streaming RPC without starting the RPC on the server. This
   * can be used to start listening to responses to an RPC before the RPC server is available.
   *
   * <p>The RPC remains open until it is completed by the server with a response or error packet or
   * cancelled.
   */
  public <RequestT extends MessageLite> Call.ClientStreaming<RequestT> openClientStreaming(
      StreamObserver<? extends MessageLite> observer) {
    checkCallType(Method.Type.CLIENT_STREAMING);
    return StreamObserverCall.open(rpcs(), rpc(), observer);
  }

  /** Invokes a bidirectional streaming RPC. Uses the default StreamObserver for RPC events. */
  public <RequestT extends MessageLite> Call.ClientStreaming<RequestT>
  invokeBidirectionalStreaming() throws ChannelOutputException {
    return invokeBidirectionalStreaming(defaultObserver());
  }

  /** Invokes a bidirectional streaming RPC. Uses the provided StreamObserver for RPC events. */
  public <RequestT extends MessageLite> Call.ClientStreaming<RequestT> invokeBidirectionalStreaming(
      StreamObserver<? extends MessageLite> observer) throws ChannelOutputException {
    checkCallType(Method.Type.BIDIRECTIONAL_STREAMING);
    return StreamObserverCall.start(rpcs(), rpc(), observer, null);
  }

  /** Invokes a bidirectional streaming RPC with a future that finishes when the RPC finishes. */
  public <RequestT extends MessageLite, ResponseT extends MessageLite>
      Call.BidirectionalStreamingFuture<RequestT> invokeBidirectionalStreamingFuture(
          Consumer<ResponseT> onNext) {
    checkCallType(Method.Type.BIDIRECTIONAL_STREAMING);
    return new StreamResponseFuture<>(rpcs(), rpc(), onNext, null);
  }

  /**
   * Creates a call object for a bidirectional streaming RPC without starting the RPC on the server.
   * This can be used to start listening to responses to an RPC before the RPC server is available.
   *
   * <p>The RPC remains open until it is completed by the server with a response or error packet or
   * cancelled.
   */
  public <RequestT extends MessageLite> Call.ClientStreaming<RequestT> openBidirectionalStreaming(
      StreamObserver<? extends MessageLite> observer) {
    checkCallType(Method.Type.BIDIRECTIONAL_STREAMING);
    return StreamObserverCall.open(rpcs(), rpc(), observer);
  }

  @SuppressWarnings("unchecked")
  private <ResponseT extends MessageLite> StreamObserver<ResponseT> defaultObserver() {
    return (StreamObserver<ResponseT>) defaultObserver;
  }

  private void checkCallType(Method.Type expected) {
    if (!rpc().method().type().equals(expected)) {
      throw new UnsupportedOperationException(String.format(
          "%s is a %s method, but it was invoked as a %s method. RPCs must be invoked by the"
              + " appropriate invoke function.",
          method().fullName(),
          method().type().sentenceName(),
          expected.sentenceName()));
    }
  }
}
