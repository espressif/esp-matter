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
import static java.nio.charset.StandardCharsets.US_ASCII;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import org.junit.Test;

public class EncoderTest {
  private static final int OUTPUT_SIZE = 1024;
  private static final byte UNNUMBERED_CONTROL = 0x03;
  private static final byte ADDRESS = 0x7B;
  private static final byte ENCODED_ADDRESS = (byte) ((ADDRESS << 1) | 1);
  private final ByteArrayOutputStream outputStream = new ByteArrayOutputStream(OUTPUT_SIZE);

  @Test
  public void emptyPayload() throws IOException {
    ByteBuffer emptyPayload = ByteBuffer.allocate(0);
    byte[] expectedResult = new ExpectedValueBuilder()
                                .writeFlag()
                                .write(ENCODED_ADDRESS)
                                .write(UNNUMBERED_CONTROL)
                                .writeCrc(0x832d343f)
                                .writeFlag()
                                .build();

    Encoder.writeUiFrame(ADDRESS, emptyPayload, outputStream);

    assertThat(outputStream.toByteArray()).isEqualTo(expectedResult);
  }

  @Test
  public void oneBytePayload() throws IOException {
    byte[] payload = {'A'};
    byte[] expectedResult = new ExpectedValueBuilder()
                                .writeFlag()
                                .write(ENCODED_ADDRESS)
                                .write(UNNUMBERED_CONTROL)
                                .write((byte) 'A')
                                .writeCrc(0x653c9e82)
                                .writeFlag()
                                .build();

    Encoder.writeUiFrame(ADDRESS, ByteBuffer.wrap(payload), outputStream);

    assertThat(outputStream.toByteArray()).isEqualTo(expectedResult);
  }

  @Test
  public void oneBytePayload_escape0x7d() throws IOException {
    byte[] payload = {0x7d};
    byte[] expectedResult = new ExpectedValueBuilder()
                                .writeFlag()
                                .write(ENCODED_ADDRESS)
                                .write(UNNUMBERED_CONTROL)
                                .write(Protocol.ESCAPE)
                                .write((byte) (0x7d ^ 0x20))
                                .writeCrc(0x4a53e205)
                                .writeFlag()
                                .build();

    Encoder.writeUiFrame(ADDRESS, ByteBuffer.wrap(payload), outputStream);

    assertThat(outputStream.toByteArray()).isEqualTo(expectedResult);
  }

  @Test
  public void oneBytePayload_escape0x7e() throws IOException {
    byte[] payload = {0x7e};
    byte[] expectedResult = new ExpectedValueBuilder()
                                .writeFlag()
                                .write(ENCODED_ADDRESS)
                                .write(UNNUMBERED_CONTROL)
                                .write(Protocol.ESCAPE)
                                .write((byte) (0x7e ^ 0x20))
                                .writeCrc(0xd35ab3bf)
                                .writeFlag()
                                .build();

    Encoder.writeUiFrame(ADDRESS, ByteBuffer.wrap(payload), outputStream);

    assertThat(outputStream.toByteArray()).isEqualTo(expectedResult);
  }

  @Test
  public void addressNeedsEscaping() throws IOException {
    byte address = 0x7d >> 1;
    byte[] payload = {'A'};
    byte[] expectedResult = new ExpectedValueBuilder()
                                .writeFlag()
                                .write(Protocol.ESCAPE)
                                .write((byte) 0x5d)
                                .write(UNNUMBERED_CONTROL)
                                .write((byte) 'A')
                                .writeCrc(0x899E00D4)
                                .writeFlag()
                                .build();

    Encoder.writeUiFrame(address, ByteBuffer.wrap(payload), outputStream);

    assertThat(outputStream.toByteArray()).isEqualTo(expectedResult);
  }

  @Test
  public void crcNeedsEscaping() throws IOException {
    byte[] payload = {'a', 'a'};
    // The CRC-32 of {ENCODED_ADDRESS, UNNUMBERED_CONTROL, "aa"} is 0x7ee04473, so
    // the 0x7e must be escaped.
    byte[] expectedCrc = {0x73, 0x44, (byte) 0xe0, 0x7d, 0x5e};
    byte[] expectedResult = new ExpectedValueBuilder()
                                .writeFlag()
                                .write(ENCODED_ADDRESS)
                                .write(UNNUMBERED_CONTROL)
                                .write(payload)
                                .write(expectedCrc)
                                .writeFlag()
                                .build();

    Encoder.writeUiFrame(ADDRESS, ByteBuffer.wrap(payload), outputStream);

    assertThat(outputStream.toByteArray()).isEqualTo(expectedResult);
  }

  @Test
  public void multiplePayloads() throws IOException {
    byte[] payload1 = {'A', 'B', 'C'};
    byte[] payload2 = {'D', 'E', 'F'};
    byte[] expectedResult = new ExpectedValueBuilder()
                                .writeFlag()
                                .write(ENCODED_ADDRESS)
                                .write(UNNUMBERED_CONTROL)
                                .write(payload1)
                                .writeCrc(0x72410ee4)
                                .writeFlag()
                                .writeFlag()
                                .write(ENCODED_ADDRESS)
                                .write(UNNUMBERED_CONTROL)
                                .write(payload2)
                                .writeCrc(0x4ba1ae47)
                                .writeFlag()
                                .build();

    Encoder.writeUiFrame(ADDRESS, ByteBuffer.wrap(payload1), outputStream);
    Encoder.writeUiFrame(ADDRESS, ByteBuffer.wrap(payload2), outputStream);

    assertThat(outputStream.toByteArray()).isEqualTo(expectedResult);
  }

  @Test
  public void payloadWithNoEscapes() throws IOException {
    byte[] payload = "1995 toyota corolla".getBytes(US_ASCII);
    byte[] expectedResult = new ExpectedValueBuilder()
                                .writeFlag()
                                .write(ENCODED_ADDRESS)
                                .write(UNNUMBERED_CONTROL)
                                .write(payload)
                                .writeCrc(0x53ee911c)
                                .writeFlag()
                                .build();

    Encoder.writeUiFrame(ADDRESS, ByteBuffer.wrap(payload), outputStream);

    assertThat(outputStream.toByteArray()).isEqualTo(expectedResult);
  }

  @Test
  public void multiByteAddress() throws IOException {
    long address = 0x3fff;
    byte[] encodedAddress = {(byte) 0xfe, (byte) 0xff};
    byte[] payload = "abc".getBytes(US_ASCII);
    byte[] expectedResult = new ExpectedValueBuilder()
                                .writeFlag()
                                .write(encodedAddress)
                                .write(UNNUMBERED_CONTROL)
                                .write(payload)
                                .writeCrc(0x8cee2978)
                                .writeFlag()
                                .build();

    Encoder.writeUiFrame(address, ByteBuffer.wrap(payload), outputStream);

    assertThat(outputStream.toByteArray()).isEqualTo(expectedResult);
  }

  @Test
  public void payloadWithMultipleEscapes() throws IOException {
    byte[] payload = {0x7E, 0x7B, 0x61, 0x62, 0x63, 0x7D, 0x7E};
    byte[] escapedPayload = {0x7D, 0x5E, 0x7B, 0x61, 0x62, 0x63, 0x7D, 0x5D, 0x7D, 0x5E};
    byte[] expectedResult = new ExpectedValueBuilder()
                                .writeFlag()
                                .write(ENCODED_ADDRESS)
                                .write(UNNUMBERED_CONTROL)
                                .write(escapedPayload)
                                .writeCrc(0x1563a4e6)
                                .writeFlag()
                                .build();

    Encoder.writeUiFrame(ADDRESS, ByteBuffer.wrap(payload), outputStream);

    assertThat(outputStream.toByteArray()).isEqualTo(expectedResult);
  }

  @Test
  public void writeFrame() throws IOException {
    byte[] payload = {5, (byte) 0xab, 0x42, 0x24, (byte) 0xf9, 0x54, (byte) 0xfb, 0x3d};
    byte[] expectedResult =
        new ExpectedValueBuilder().writeFlag().write(payload).writeFlag().build();
    Frame frame = Frame.parse(ByteBuffer.wrap(payload));

    Encoder.writeFrame(frame, outputStream);

    assertThat(outputStream.toByteArray()).isEqualTo(expectedResult);
  }

  @Test
  public void getUiFrameEncodedSize_empty() throws IOException {
    byte[] payload = {};

    Encoder.writeUiFrame(ADDRESS, ByteBuffer.wrap(payload), outputStream);

    assertThat(Encoder.getUiFrameEncodedSize(ADDRESS, payload)).isEqualTo(outputStream.size());
  }

  @Test
  public void getUiFrameEncodedSize_oneByte() throws IOException {
    byte[] payload = {0x61};

    Encoder.writeUiFrame(ADDRESS, ByteBuffer.wrap(payload), outputStream);

    assertThat(Encoder.getUiFrameEncodedSize(ADDRESS, payload)).isEqualTo(outputStream.size());
  }

  @Test
  public void getUiFrameEncodedSize_multipleEscapes() throws IOException {
    byte[] payload = {0x7E, 0x7B, 0x61, 0x62, 0x63, 0x7D, 0x7E};

    Encoder.writeUiFrame(ADDRESS, ByteBuffer.wrap(payload), outputStream);

    assertThat(Encoder.getUiFrameEncodedSize(ADDRESS, payload)).isEqualTo(outputStream.size());
  }

  @Test
  public void getMaxPayloadBytesInUiFrameOfSize_emptyPayload() {
    byte[] payload = {};

    assertThat(Encoder.getMaxPayloadBytesInUiFrameOfSize(ADDRESS, payload, 1000)).isEqualTo(0);
  }

  @Test
  public void getMaxPayloadBytesInUiFrameOfSize_emptyFrame() {
    byte[] payload = {1, 2, 3, 4};

    assertThat(Encoder.getMaxPayloadBytesInUiFrameOfSize(ADDRESS, payload, 0)).isEqualTo(0);
  }

  @Test
  public void getMaxPayloadBytesInUiFrameOfSize_exactFit() throws IOException {
    byte[] payload = {0x7E, 0x7B, 0x61, 0x62, 0x63, 0x7D, 0x7E};
    final int requiredReservedSize = Encoder.getUiFrameEncodedSize(ADDRESS, payload);

    Encoder.writeUiFrame(ADDRESS, ByteBuffer.wrap(payload), outputStream);

    assertThat(Encoder.getMaxPayloadBytesInUiFrameOfSize(ADDRESS, payload, requiredReservedSize))
        .isEqualTo(payload.length);
    // Not enough room for the last payload byte, which is escaped, so needs two bytes.
    assertThat(
        Encoder.getMaxPayloadBytesInUiFrameOfSize(ADDRESS, payload, requiredReservedSize - 1))
        .isEqualTo(payload.length - 1);
    assertThat(
        Encoder.getMaxPayloadBytesInUiFrameOfSize(ADDRESS, payload, requiredReservedSize - 2))
        .isEqualTo(payload.length - 1);
  }

  @Test
  public void maxPayloadBytesInUiFrameOfSize_extraRoom() {
    byte[] payload = {1, 2, 3, 4};

    assertThat(Encoder.getMaxPayloadBytesInUiFrameOfSize(ADDRESS, payload, 1000))
        .isEqualTo(payload.length);
  }

  private static class ExpectedValueBuilder {
    private final ByteArrayOutputStream outputStream = new ByteArrayOutputStream(OUTPUT_SIZE);

    public ExpectedValueBuilder writeFlag() {
      outputStream.write(Protocol.FLAG);
      return this;
    }

    public ExpectedValueBuilder write(byte b) {
      outputStream.write(b);
      return this;
    }

    public ExpectedValueBuilder write(byte[] buffer) throws IOException {
      outputStream.write(buffer);
      return this;
    }

    public ExpectedValueBuilder writeCrc(int crc) {
      outputStream.write(0xFF & crc);
      outputStream.write(0xFF & (crc >> 8));
      outputStream.write(0xFF & (crc >> 16));
      outputStream.write(0xFF & (crc >> 24));
      return this;
    }

    byte[] build() {
      return outputStream.toByteArray();
    }
  }
}