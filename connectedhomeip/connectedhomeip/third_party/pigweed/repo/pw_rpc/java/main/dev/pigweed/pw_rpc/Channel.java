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
 * Represents an RPC communications channel. Each endpoint must connect to the server with a unique
 * channel ID.
 *
 * <p>The client sends outgoing packets through a Channel object. Incoming packets must match a
 * known channel ID.
 */
public class Channel {
  /**
   * Writes a single packet to the output.
   *
   * <p>The Channel.Output implementation for the each channel determines the semantics for all
   * client functions that send packets. For example, if there is congestion, the Channel.Output may
   * block, enqueue the response and return immediately, or throw an exception that signals that the
   * user should retry. Deployments should keep this in mind when implementing Channel.Outputs and
   * interacting with RPCs.
   */
  public interface Output {
    void send(byte[] packet) throws ChannelOutputException;
  }

  private final int id;
  private final Output output;

  public Channel(int id, Output output) {
    this.id = id;
    this.output = output;
  }

  public int id() {
    return id;
  }

  public void send(byte[] data) throws ChannelOutputException {
    output.send(data);
  }
}
