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
import dev.pigweed.pw_log.Logger;
import java.util.HashMap;
import java.util.Map;
import javax.annotation.Nullable;

/** Tracks the state of service method invocations. */
public class RpcManager {
  private static final Logger logger = Logger.forClass(RpcManager.class);

  private final Map<PendingRpc, StreamObserverCall<?, ?>> pending = new HashMap<>();

  /**
   * Invokes an RPC.
   *
   * @param rpc channel / service / method tuple that unique identifies this RPC
   * @param call object for this RPC
   * @param payload the request
   */
  @Nullable
  public synchronized StreamObserverCall<?, ?> start(
      PendingRpc rpc, StreamObserverCall<?, ?> call, @Nullable MessageLite payload)
      throws ChannelOutputException {
    logger.atFine().log("%s starting", rpc);
    rpc.channel().send(Packets.request(rpc, payload));
    return pending.put(rpc, call);
  }

  /**
   * Listens to responses to an RPC without sending a request.
   *
   * <p>The RPC remains open until it is closed by the server (either with a response or error
   * packet) or cancelled.
   */
  @Nullable
  public synchronized StreamObserverCall<?, ?> open(PendingRpc rpc, StreamObserverCall<?, ?> call) {
    logger.atFine().log("%s opening", rpc);
    return pending.put(rpc, call);
  }

  /** Cancels an ongoing RPC */
  @Nullable
  public synchronized StreamObserverCall<?, ?> cancel(PendingRpc rpc)
      throws ChannelOutputException {
    StreamObserverCall<?, ?> call = pending.remove(rpc);
    if (call != null) {
      logger.atFine().log("%s was cancelled", rpc);
      rpc.channel().send(Packets.cancel(rpc));
    }
    return call;
  }

  /** Cancels an ongoing RPC without sending a cancellation packet. */
  public synchronized void abandon(PendingRpc rpc) {
    StreamObserverCall<?, ?> call = pending.remove(rpc);
    if (call != null) {
      logger.atFine().log("%s was abandoned", rpc);
    }
  }

  @Nullable
  public synchronized StreamObserverCall<?, ?> clientStream(PendingRpc rpc, MessageLite payload)
      throws ChannelOutputException {
    StreamObserverCall<?, ?> call = pending.get(rpc);
    if (call != null) {
      rpc.channel().send(Packets.clientStream(rpc, payload));
    }
    return call;
  }

  @Nullable
  public synchronized StreamObserverCall<?, ?> clientStreamEnd(PendingRpc rpc)
      throws ChannelOutputException {
    StreamObserverCall<?, ?> call = pending.get(rpc);
    if (call != null) {
      logger.atFiner().log("%s client stream closed", rpc);
      rpc.channel().send(Packets.clientStreamEnd(rpc));
    }
    return call;
  }

  @Nullable
  public synchronized StreamObserverCall<?, ?> clear(PendingRpc rpc) {
    return pending.remove(rpc);
  }

  @Nullable
  public synchronized StreamObserverCall<?, ?> getPending(PendingRpc rpc) {
    return pending.get(rpc);
  }

  @Override
  public synchronized String toString() {
    return "RpcManager{"
        + "pending=" + pending + '}';
  }
}
