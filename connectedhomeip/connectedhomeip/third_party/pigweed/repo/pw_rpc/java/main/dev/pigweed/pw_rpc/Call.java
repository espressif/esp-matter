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

import com.google.common.util.concurrent.ListenableFuture;
import com.google.protobuf.MessageLite;
import javax.annotation.Nullable;

/** Represents an ongoing RPC call. */
public interface Call {
  /** Cancels the RPC. Sends a cancellation packet to the server and sets error() to CANCELLED. */
  void cancel() throws ChannelOutputException;

  /** Cancels the RPC as in cancel(), but does not send a cancellation packet to the server. */
  void abandon();

  /** True if the RPC has not yet completed. */
  default boolean active() {
    return status() == null && error() == null;
  }

  /** The RPC status, if the RPC completed. */
  @Nullable Status status();

  /** The error that terminated this RPC, if any. */
  @Nullable Status error();

  /** Represents a call to a unary or client streaming RPC that uses a future. */
  @SuppressWarnings("ShouldNotSubclass")
  interface UnaryFuture<ResponseT extends MessageLite>
      extends Call, ListenableFuture<UnaryResult<ResponseT>> {}

  /** Represents a call to a server or bidirectional streaming RPC that uses a future. */
  @SuppressWarnings("ShouldNotSubclass")
  interface ServerStreamingFuture extends Call, ListenableFuture<Status> {}

  /** Represents a call to a client or bidirectional streaming RPC. */
  interface ClientStreaming<RequestT extends MessageLite> extends Call {
    /**
     * Sends a request to a pending client streaming RPC.
     *
     * <p>The semantics of send() reflect the Channel.Output implementation for this channel.
     *
     * @throws ChannelOutputException the Channel.Output was unable to send the request
     * @throws RpcError the RPC is not currently active
     */
    void send(RequestT request) throws ChannelOutputException, RpcError;

    /** Signals to the server that the client stream has completed. */
    void finish() throws ChannelOutputException;
  }

  /** Represents a call to a client streaming RPC that uses a future. */
  interface ClientStreamingFuture<RequestT extends MessageLite, ResponseT extends MessageLite>
      extends ClientStreaming<RequestT>, UnaryFuture<ResponseT> {}

  /** Represents a call to a bidirectional streaming RPC that uses a future. */
  interface BidirectionalStreamingFuture<RequestT extends MessageLite>
      extends ClientStreaming<RequestT>, ServerStreamingFuture {}
}
