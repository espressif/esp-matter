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

import dev.pigweed.pw_hdlc.CustomVarInt;
import dev.pigweed.pw_hdlc.Protocol;
import java.io.IOException;
import java.io.OutputStream;
import java.nio.ByteBuffer;
import java.util.zip.CRC32;

/** Encodes a payload with pigweed's HDLC format */
public class Encoder {
  private final CRC32 crc32 = new CRC32();
  private final OutputStream output;

  /** Writes the payload as an HDLC unnumbered information (UI) frame. */
  public static void writeUiFrame(long address, ByteBuffer payload, OutputStream output)
      throws IOException {
    Encoder encoder = new Encoder(output);
    encoder.startUnnumberedFrame(address);
    encoder.writeData(payload);
    encoder.finishFrame();
  }

  public static void writeFrame(Frame frame, OutputStream output) throws IOException {
    Encoder encoder = new Encoder(output);
    encoder.startFrame(frame.getAddress(), frame.getControl());
    encoder.writeData(frame.getPayload());
    encoder.finishFrame();
  }

  /** Returns the encoded size of a UI frame with the provided payload. */
  public static int getUiFrameEncodedSize(long address, byte[] payload) {
    return getUiFrameEncodedSize(address, ByteBuffer.wrap(payload));
  }

  public static int getUiFrameEncodedSize(long address, ByteBuffer payload) {
    ByteCountingOutputStream output = new ByteCountingOutputStream();
    try {
      writeUiFrame(address, payload, output);
    } catch (IOException e) {
      throw new AssertionError("ByteCountingOutputStream does not throw exceptions!", e);
    }
    return output.writtenBytes;
  }

  /** Returns the number of payload bytes that can be encoded in a UI frame of the given size. */
  public static int getMaxPayloadBytesInUiFrameOfSize(
      long address, byte[] payload, int maxEncodedUiFrameSizeBytes) {
    return getMaxPayloadBytesInUiFrameOfSize(
        address, ByteBuffer.wrap(payload), maxEncodedUiFrameSizeBytes);
  }

  public static int getMaxPayloadBytesInUiFrameOfSize(
      long address, ByteBuffer payload, int maxEncodedUiFrameSizeBytes) {
    ByteCountingOutputStream output = new ByteCountingOutputStream();
    Encoder encoder = new Encoder(output);
    int payloadBytesProcessed = 0;

    try {
      encoder.startUnnumberedFrame(address);
      while (payload.hasRemaining()) {
        encoder.write(payload.get());
        if (output.writtenBytes + encoder.getFrameSuffixSize() > maxEncodedUiFrameSizeBytes) {
          break;
        }
        payloadBytesProcessed += 1;
      }
    } catch (IOException e) {
      throw new AssertionError("ByteCountingOutputStream does not throw exceptions!", e);
    }
    return payloadBytesProcessed;
  }

  private Encoder(OutputStream output) {
    this.output = output;
  }

  private void startUnnumberedFrame(long address) throws IOException {
    startFrame(address, Protocol.UNNUMBERED_INFORMATION);
  }

  private void startFrame(long address, byte control) throws IOException {
    crc32.reset();
    output.write(Protocol.FLAG);
    writeVarLong(address);
    byte[] wrappedControl = {control};
    writeData(ByteBuffer.wrap(wrappedControl));
  }

  private void finishFrame() throws IOException {
    writeCrc();
    output.write(Protocol.FLAG);
  }

  private void writeCrc() throws IOException {
    long crc = crc32.getValue();
    byte[] buffer = {(byte) crc, (byte) (crc >> 8), (byte) (crc >> 16), (byte) (crc >> 24)};
    writeData(ByteBuffer.wrap(buffer));
  }

  private void writeVarLong(long data) throws IOException {
    ByteBuffer buffer = ByteBuffer.allocate(CustomVarInt.varLongSize(data));
    CustomVarInt.putVarLong(data, buffer);
    buffer.rewind();
    writeData(buffer);
  }

  private void writeData(ByteBuffer data) throws IOException {
    while (data.hasRemaining()) {
      byte b = data.get();
      write(b);
      crc32.update(b);
    }
  }

  private void write(byte b) throws IOException {
    if (b == Protocol.FLAG) {
      output.write(Protocol.ESCAPED_FLAG);
    } else if (b == Protocol.ESCAPE) {
      output.write(Protocol.ESCAPED_ESCAPE);
    } else {
      output.write(b);
    }
  }

  private int getFrameSuffixSize() {
    int crc32Escapes = 0;
    for (int value = (int) crc32.getValue(); value != 0; value >>>= 8) {
      crc32Escapes += (value & 0xFF) == Protocol.FLAG || (value & 0xFF) == Protocol.ESCAPE ? 1 : 0;
    }
    return 4 /* CRC32 */ + crc32Escapes + 1 /* final flag byte */;
  }

  private static class ByteCountingOutputStream extends OutputStream {
    private int writtenBytes;

    @Override
    public void write(int b) {
      writtenBytes += 1;
    }
  }
}