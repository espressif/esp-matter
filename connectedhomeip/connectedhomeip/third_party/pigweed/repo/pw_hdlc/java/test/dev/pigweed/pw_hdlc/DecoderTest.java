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
import java.util.ArrayList;
import org.junit.Test;

public class DecoderTest {
  private final ArrayList<Frame> frames = new ArrayList<>();
  private final Decoder decoder = new Decoder(frames::add);

  @Test
  public void processSingleFrame() {
    byte[] data = {'~', '1', '2', '3', '4', (byte) 0xa3, (byte) 0xe0, (byte) 0xe3, (byte) 0x9b};
    decoder.process(data);
    assertThat(frames).isEmpty();
    decoder.process(Protocol.FLAG);

    assertThat(frames).hasSize(1);
    Frame frame = frames.get(0);
    assertThat(frame.getAddress()).isEqualTo(24);
    assertThat(frame.getControl()).isEqualTo(50);
    ByteBuffer payload = frame.getPayload();
    assertThat(payload.remaining()).isEqualTo(2);
    assertThat(payload.get()).isEqualTo((byte) 51);
    assertThat(payload.get()).isEqualTo((byte) 52);
  }

  @Test
  public void processTwoFrames() {
    byte[] data = {'~',
        '1',
        '2',
        '3',
        '4',
        (byte) 0xa3,
        (byte) 0xe0,
        (byte) 0xe3,
        (byte) 0x9b,
        '~',
        '~',
        '5',
        '6',
        '7',
        '8',
        0x07,
        0x56,
        0x52,
        0x7d,
        0x7e ^ 0x20,
        '~'};

    decoder.process(data);

    assertThat(frames).hasSize(2);
    // First frame
    Frame frame = frames.get(0);
    assertThat(frame.getAddress()).isEqualTo(24);
    assertThat(frame.getControl()).isEqualTo(50);
    ByteBuffer payload = frame.getPayload();
    assertThat(payload.remaining()).isEqualTo(2);
    assertThat(payload.get()).isEqualTo((byte) 51);
    assertThat(payload.get()).isEqualTo((byte) 52);
    // Second frame
    frame = frames.get(1);
    assertThat(frame.getAddress()).isEqualTo(26);
    assertThat(frame.getControl()).isEqualTo(54);
    payload = frame.getPayload();
    assertThat(payload.remaining()).isEqualTo(2);
    assertThat(payload.get()).isEqualTo((byte) 55);
    assertThat(payload.get()).isEqualTo((byte) 56);
  }
}