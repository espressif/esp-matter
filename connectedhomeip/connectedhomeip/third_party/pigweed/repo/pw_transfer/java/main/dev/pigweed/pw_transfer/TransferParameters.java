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

import com.google.auto.value.AutoValue;

/**
 * Transfer parameters set by the receiver.
 *
 * <p>In a client, these are only used for read transfers. These values can be adjusted to optimize
 * for the service/client limitations and the transport between them.
 */
@AutoValue
public abstract class TransferParameters {
  public static TransferParameters create(
      int maxPendingBytes, int maxChunkSizeBytes, int chunkDelayMicroseconds) {
    return new AutoValue_TransferParameters(
        maxPendingBytes, maxChunkSizeBytes, chunkDelayMicroseconds);
  }

  /** Max number of bytes to request at once. Should be a multiple of maxChunkSizeBytes. */
  public abstract int maxPendingBytes();

  /** Max number of bytes to send in a single chunk. Should be a factor of maxPendingBytes. */
  public abstract int maxChunkSizeBytes();

  /** How long to require the sender to wait between sending chunks. */
  public abstract int chunkDelayMicroseconds();
}
