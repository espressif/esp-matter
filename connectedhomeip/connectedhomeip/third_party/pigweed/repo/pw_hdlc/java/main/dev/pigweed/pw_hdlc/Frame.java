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
import javax.annotation.Nullable;

/** Contents of an HDLC frame (after removing flags and CRC) */
public class Frame {
  // address, control + CRC32
  public static final int MIN_FRAME_SIZE_BYTES = 6;
  public static final int CRC_SIZE = 4;

  private final long address;
  private final byte control;
  private final ByteBuffer payload;

  @Nullable
  public static Frame parse(ByteBuffer payload) {
    long address = CustomVarInt.getVarLong(payload);
    if (address == 0) {
      return null;
    }
    byte control = payload.get();
    int dataSize = payload.remaining() - CRC_SIZE;
    if (dataSize < 0) {
      return null;
    }
    ByteBuffer data = ByteBuffer.allocate(dataSize);
    payload.get(data.array());
    return new Frame(address, control, data);
  }

  private Frame(long address, byte control, ByteBuffer payload) {
    this.address = address;
    this.control = control;
    this.payload = payload;
  }

  public long getAddress() {
    return address;
  }

  public byte getControl() {
    return control;
  }

  public ByteBuffer getPayload() {
    return payload;
  }

  @Override
  public String toString() {
    return String.format(
        "Frame{address=0x%x, control=0x%x, payload=%s}", address, control, payload);
  }
}