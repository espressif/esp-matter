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

/** Values used to report the progress of an ongoing read or write transfer. */
@AutoValue
public abstract class TransferProgress {
  public static final long UNKNOWN_TRANSFER_SIZE = -1;

  static TransferProgress create(long bytesSent, long bytesConfirmedReceived, long totalSizeBytes) {
    return new AutoValue_TransferProgress(bytesSent, bytesConfirmedReceived, totalSizeBytes);
  }

  /**
   * Number of bytes the sender has sent.
   *
   * <p>For the receiver, this value is the same as bytesConfirmedReceived().
   *
   * <p>For the sender, this is the current send offset, even if the receiver has not confirmed
   * receipt by updating the transfer parameters. This value may decrease if the receiver requests
   * that data is resent due to loss.
   */
  public abstract long bytesSent();

  /**
   * Bytes the receiver has confirmed received. This value is monotonically non-decreasing.
   *
   * <p>For the receiver, this value is the latest offset.
   *
   * <p>For the sender, this value is the latest offset the receiver sent in a transfer parameters
   * update.
   */
  public abstract long bytesConfirmedReceived();

  /**
   * Total bytes expected to be transferred. Set to UNKNOWN_TRANSFER_SIZE if the total is not known.
   *
   * <p>For the receiver, this value is based on the most recent remaining bytes value.
   *
   * <p>For the sender, this value is the number of bytes it expects to send (if known).
   */
  public abstract long totalSizeBytes();

  /** Returns bytesConfirmedReceived() of totalSizeBytes() as a percent or NaN if unknown. */
  public final float percentReceived() {
    if (totalSizeBytes() == UNKNOWN_TRANSFER_SIZE) {
      return Float.NaN;
    }
    return (float) bytesConfirmedReceived() / totalSizeBytes() * 100;
  }
}
