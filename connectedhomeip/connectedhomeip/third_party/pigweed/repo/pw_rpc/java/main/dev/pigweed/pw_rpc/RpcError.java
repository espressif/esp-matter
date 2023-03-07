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

/**
 * Represents a pw_rpc-layer error.
 *
 * <p>The RPC server error status codes are documented at https://pigweed.dev/pw_rpc/.
 */
public class RpcError extends Exception {
  private final PendingRpc rpc;
  private final Status status;

  public RpcError(PendingRpc rpc, Status status) {
    super(String.format("%s failed with status %s", rpc, status.name()));
    this.rpc = rpc;
    this.status = status;
  }

  public PendingRpc rpc() {
    return rpc;
  }

  public Status status() {
    return status;
  }
}
