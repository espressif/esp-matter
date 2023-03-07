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

import dev.pigweed.pw_rpc.Status;
import java.util.Locale;

/** Represents errors that terminate a transfer. */
public class TransferError extends Exception {
  private final Status error;

  TransferError(String message, Throwable cause) {
    super(message, cause);
    error = Status.UNKNOWN;
  }

  TransferError(Transfer<?> transfer, Status error) {
    this(String.format(Locale.ENGLISH, "%s failed with status %s", transfer, error.name()), error);
  }

  TransferError(String msg, Status error) {
    super(msg);
    this.error = error;
  }

  /**
   * Returns the pw_transfer error code that terminated this transfer.
   *
   * <p>UNKNOWN indicates that the transfer was terminated by an exception.
   */
  public Status status() {
    return error;
  }
}
