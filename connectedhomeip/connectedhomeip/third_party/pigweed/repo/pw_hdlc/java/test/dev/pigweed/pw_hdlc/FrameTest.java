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

import static com.google.common.truth.Truth.assertThat;

import java.nio.ByteBuffer;
import org.junit.Test;

public class FrameTest {
  @Test
  public void fields() {
    byte[] payload = {5, (byte) 0xab, 0x42, 0x24, (byte) 0xf9, 0x54, (byte) 0xfb, 0x3d};
    byte[] data = {0x42, 0x24};

    Frame frame = Frame.parse(ByteBuffer.wrap(payload));

    assertThat(frame.getAddress()).isEqualTo(2);
    assertThat(frame.getControl()).isEqualTo((byte) 0xab);
    assertThat(frame.getPayload()).isEqualTo(ByteBuffer.wrap(data));
  }

  @Test
  public void echo() {
    byte[] payload =
        hexStringToByteArray("0095032A070A0568656C6C6F080110101D52D0FB1425E90E478B3000DD5F709F");

    Frame frame = Frame.parse(ByteBuffer.wrap(payload));

    assertThat(frame.getAddress()).isEqualTo(9472);
    assertThat(frame.getControl()).isEqualTo(3);
  }

  private static byte[] hexStringToByteArray(String s) {
    byte[] result = new byte[s.length() / 2];
    for (int i = 0; i < s.length(); i += 2) {
      result[i / 2] = (byte) Integer.parseInt(s.substring(i, i + 2), 16);
    }
    return result;
  }
}