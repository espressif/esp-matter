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
import com.google.common.base.Preconditions;

/**
 * Transfer client settings for timeouts and retries.
 */
@AutoValue
public abstract class TransferTimeoutSettings {
  /** Amount of time to wait for a packet before resending the last packet. */
  public abstract int timeoutMillis();

  /** Amount of time to wait for the first packet before retrying the transfer. */
  public abstract int initialTimeoutMillis();

  /** Maximum number of times to retry sending a packet. */
  public abstract int maxRetries();

  /** Maximum number of retries to allow before aborting the transfer. */
  public abstract int maxLifetimeRetries();

  /** Creates a builder with defaults applied to all fields. */
  public static TransferTimeoutSettings.Builder builder() {
    return new AutoValue_TransferTimeoutSettings.Builder()
        .setTimeoutMillis(3000)
        .setInitialTimeoutMillis(6000)
        .setMaxRetries(5)
        .setMaxLifetimeRetries(5000);
  }

  @AutoValue.Builder
  public abstract static class Builder {
    public abstract Builder setTimeoutMillis(int timeoutMillis);

    public abstract Builder setInitialTimeoutMillis(int initialTimeoutMillis);

    public abstract Builder setMaxRetries(int maxRetries);

    public abstract Builder setMaxLifetimeRetries(int maxLifetimeRetries);

    public final TransferTimeoutSettings build() {
      TransferTimeoutSettings settings = autoBuild();
      Preconditions.checkState(
          settings.timeoutMillis() >= 0, "Negative timeouts are not permitted");
      Preconditions.checkState(settings.initialTimeoutMillis() >= settings.timeoutMillis(),
          "The initial timeout must be at least as long as the regular timeout");
      Preconditions.checkState(settings.maxRetries() >= 0, "Retries must be positive");
      Preconditions.checkState(settings.maxLifetimeRetries() >= settings.maxRetries(),
          "Lifetime max retries cannot be smaller than max retries");
      return settings;
    }

    abstract TransferTimeoutSettings autoBuild();
  }
}
