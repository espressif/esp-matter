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

package dev.pigweed.pw_hdlc;

import java.nio.ByteBuffer;

/**
 * Encodes and decodes Varint in pigweed's custom, one terminated, least significant format.
 * Regular protobuf format is zero terminated, most significant.
 */
public final class CustomVarInt {
  private static final int MAX_COUNT = 10;

  public static void putVarLong(long v, ByteBuffer sink) {
    while (true) {
      int bits = (((int) v) & 0x7f) << 1;
      v >>>= 7;
      if (v == 0) {
        sink.put((byte) (bits | 0x01));
        return;
      }
      sink.put((byte) bits);
    }
  }

  public static int varLongSize(long v) {
    int result = 0;
    do {
      result++;
      v >>>= 7;
    } while (v != 0);
    return result;
  }

  public static long getVarLong(ByteBuffer src) {
    long decodedValue = 0;
    int count = 0;

    while (src.hasRemaining()) {
      if (count >= MAX_COUNT) {
        return 0;
      }
      byte b = src.get();
      // Add the bottom seven bits of the next byte to the result.
      decodedValue |= ((long) ((b >> 1) & 0x7F)) << (7 * count);

      // Stop decoding if the end is reached.
      if (isLastByte(b)) {
        return decodedValue;
      }
      count++;
    }
    return 0;
  }

  private static boolean isLastByte(byte b) {
    return (b & 1) == 1;
  }

  private CustomVarInt() {}
}