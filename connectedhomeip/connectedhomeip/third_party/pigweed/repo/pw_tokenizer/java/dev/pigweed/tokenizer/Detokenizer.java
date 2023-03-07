// Copyright 2020 The Pigweed Authors
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

/**
 * This is a Java Native Interface (JNI) wrapper for the Detokenizer class.
 *
 * <p>This classes uses the Android Base64 library instead of the standard Java Base64, which is not
 * yet available on Android. To use this class outside of Android, replace android.util.Base64 with
 * java.util.Base64.
 */
package dev.pigweed.tokenizer;

import android.util.Base64;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/** This class provides the Java interface for the C++ Detokenizer class. */
public class Detokenizer {
  // Android's Base64 library doesn't seem to check if the Base64-encoded data is valid. This
  // regular expression checks that it is. Does not match URL-safe or unpadded Base64.
  private static final Pattern TOKENIZED_STRING =
      Pattern.compile("\\$([A-Za-z0-9+/]{4})*([A-Za-z0-9+/]{3}=|[A-Za-z0-9+/]{2}==)?");

  static {
    System.loadLibrary("detokenizer");
  }

  // The handle to the C++ detokenizer instance.
  private final long handle;

  public Detokenizer() {
    this(new byte[0]);
  }

  public Detokenizer(byte[] tokenDatabase) {
    handle = newNativeDetokenizer(tokenDatabase);
  }

  /**
   * Detokenizes and replaces all recognized tokenized messages ($ followed by Base64) in the
   * provided string. Unrecognized tokenized strings are left unchanged.
   */
  public String detokenize(String message) {
    Matcher matcher = TOKENIZED_STRING.matcher(message);
    StringBuilder result = new StringBuilder();
    int lastIndex = 0;

    while (matcher.find()) {
      result.append(message, lastIndex, matcher.start());

      String decoded =
          detokenizeNative(handle, Base64.decode(matcher.group().substring(1), Base64.DEFAULT));
      result.append(decoded != null ? decoded : matcher.group());

      lastIndex = matcher.end();
    }

    result.append(message, lastIndex, message.length());
    return result.toString();
  }

  /** Deletes memory allocated in C++ when this class is garbage collected. */
  @Override
  protected void finalize() {
    deleteNativeDetokenizer(handle);
  }

  /** Creates a new detokenizer using the provided data as the database. */
  private static native long newNativeDetokenizer(byte[] data);

  /** Deletes the detokenizer object with the provided handle, which MUST be valid. */
  private static native void deleteNativeDetokenizer(long handle);

  /**
   * Returns the detokenized version of the provided data. This is non-static so this object has a
   * reference held while the function is running, which prevents finalize from running before
   * detokenizeNative finishes.
   */
  private native String detokenizeNative(long handle, byte[] data);
}
