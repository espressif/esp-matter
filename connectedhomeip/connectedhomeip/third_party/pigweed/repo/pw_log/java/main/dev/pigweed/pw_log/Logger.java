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

package dev.pigweed.pw_log;

import com.google.common.flogger.FluentLogger;
import java.util.logging.Level;

/**
 * Partial implementation of the com.google.common.flogger.FluentLogger API that
 * wraps a FluentLogger instance.
 *
 * This class is used instead of directly logging to FluentLogger to support
 * swapping the implementation on systems that don't support FluentLogger (i.e.
 * Android).
 */
@SuppressWarnings("FloggerSplitLogStatement")
public final class Logger {
  private final FluentLogger wrappedLogger;

  public static Logger forClass(Class<?> enclosingClass) {
    return new Logger(FluentLogger.forEnclosingClass());
  }

  private Logger(FluentLogger fluentLogger) {
    this.wrappedLogger = fluentLogger;
  }

  public FluentLogger.Api at(Level level) {
    return wrappedLogger.at(level);
  }

  public FluentLogger.Api atSevere() {
    return at(Level.SEVERE);
  }

  public FluentLogger.Api atWarning() {
    return at(Level.WARNING);
  }

  public FluentLogger.Api atInfo() {
    return at(Level.INFO);
  }

  public FluentLogger.Api atConfig() {
    return at(Level.CONFIG);
  }

  public FluentLogger.Api atFine() {
    return at(Level.FINE);
  }

  public FluentLogger.Api atFiner() {
    return at(Level.FINER);
  }

  public FluentLogger.Api atFinest() {
    return at(Level.FINEST);
  }
}
