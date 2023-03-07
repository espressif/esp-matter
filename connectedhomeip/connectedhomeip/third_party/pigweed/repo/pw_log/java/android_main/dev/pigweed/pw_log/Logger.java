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

import android.util.Log;
import java.util.logging.Level;

/**
 * Partial implementation of the com.google.common.flogger.FluentLogger API that
 * logs to android.util.Log.
 */
public final class Logger {
  private final String tag;

  public final class AndroidLogApi {
    private final int level;

    private Throwable cause = null;

    private AndroidLogApi(Level level) {
      if (level == Level.FINEST || level == Level.FINER) {
        this.level = Log.VERBOSE;
      } else if (level == Level.FINE || level == Level.CONFIG) {
        this.level = Log.DEBUG;
      } else if (level == Level.WARNING) {
        this.level = Log.WARN;
      } else if (level == Level.SEVERE) {
        this.level = Log.ERROR;
      } else {
        this.level = Log.INFO;
      }
    }

    public AndroidLogApi withCause(Throwable cause) {
      this.cause = cause;
      return this;
    }

    public void log(String message) {
      if (cause != null) {
        message = String.format("%s: %s", cause, message);
      }

      Log.println(level, tag, message);
    }

    public void log(String message, Object... args) {
      log(String.format(message, args));
    }
  }

  public static Logger forClass(Class<?> enclosingClass) {
    return new Logger(enclosingClass.getSimpleName());
  }

  private Logger(String tag) {
    this.tag = tag;
  }

  public AndroidLogApi at(Level level) {
    return new AndroidLogApi(level);
  }

  public AndroidLogApi atSevere() {
    return at(Level.SEVERE);
  }

  public AndroidLogApi atWarning() {
    return at(Level.WARNING);
  }

  public AndroidLogApi atInfo() {
    return at(Level.INFO);
  }

  public AndroidLogApi atConfig() {
    return at(Level.CONFIG);
  }

  public AndroidLogApi atFine() {
    return at(Level.FINE);
  }

  public AndroidLogApi atFiner() {
    return at(Level.FINER);
  }

  public AndroidLogApi atFinest() {
    return at(Level.FINEST);
  }
}
