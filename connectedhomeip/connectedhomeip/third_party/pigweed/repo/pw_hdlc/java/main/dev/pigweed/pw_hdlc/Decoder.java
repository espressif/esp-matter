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

import com.google.common.io.BaseEncoding;
import dev.pigweed.pw_hdlc.Protocol;
import dev.pigweed.pw_log.Logger;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.zip.CRC32;

/**
 * Decodes Pigweed's HDLC frames received over the wire and passes complete frames to the registered
 * listener.
 */
public class Decoder {
  private static final Logger logger = Logger.forClass(Decoder.class);
  // Should be more than enough
  private static final int MAX_FRAME_SIZE_BYTES = 4096;

  private final OnCompleteFrame listener;
  private State state = State.INTER_FRAME;
  private int currentFrameSize = 0;
  private final byte[] buffer = new byte[MAX_FRAME_SIZE_BYTES];

  /** OnCompleteFrame */
  public interface OnCompleteFrame {
    void onCompleteFrame(Frame frame);
  }

  enum State {
    INTER_FRAME,
    FRAME,
    ESCAPE_FRAME,
  }

  public Decoder(OnCompleteFrame listener) {
    this.listener = listener;
  }

  public void process(ByteBuffer buffer) {
    while (buffer.hasRemaining()) {
      process(buffer.get());
    }
  }

  public void process(byte[] buffer) {
    for (byte b : buffer) {
      process(b);
    }
  }

  public void process(byte b) {
    switch (state) {
      case INTER_FRAME:
        if (b == Protocol.FLAG) {
          state = State.FRAME;

          // Report an error if non-flag bytes were read between frames.
          if (currentFrameSize != 0) {
            reset();
            return;
          }
        } else {
          // Count bytes to track how many are discarded.
          currentFrameSize += 1;
        }
        return;
      case FRAME: {
        if (b == Protocol.FLAG) {
          if (checkFrame()) {
            ByteBuffer message = ByteBuffer.wrap(buffer, 0, currentFrameSize);
            Frame frame = Frame.parse(message);
            if (frame != null) {
              listener.onCompleteFrame(frame);
            }
          }
          reset();
        } else if (b == Protocol.ESCAPE) {
          state = State.ESCAPE_FRAME;
        } else {
          appendByte(b);
        }
        return;
      }
      case ESCAPE_FRAME:
        // The flag character cannot be escaped; return an error.
        if (b == Protocol.FLAG) {
          state = State.FRAME;
          reset();
        }

        if (b == Protocol.ESCAPE) {
          // Two escape characters in a row is illegal -- invalidate this frame.
          // The frame is reported abandoned when the next flag byte appears.
          state = State.INTER_FRAME;

          // Count the escape byte so that the inter-frame state detects an error.
          currentFrameSize += 1;
        } else {
          state = State.FRAME;
          appendByte(escape(b));
        }
    }
  }

  private void reset() {
    currentFrameSize = 0;
  }

  private void appendByte(byte b) {
    if (currentFrameSize < MAX_FRAME_SIZE_BYTES) {
      buffer[currentFrameSize] = b;
    }

    // Always increase size: if it is larger than the buffer, overflow occurred.
    currentFrameSize += 1;
  }

  private static byte escape(byte b) {
    return (byte) (b ^ Protocol.ESCAPE_CONSTANT);
  }

  private boolean checkFrame() {
    // Empty frames are not an error; repeated flag characters are okay.
    if (currentFrameSize == 0) {
      return true;
    }

    if (currentFrameSize < Frame.MIN_FRAME_SIZE_BYTES) {
      logger.atWarning().log(
          "Frame length (%d) less than %d", currentFrameSize, Frame.MIN_FRAME_SIZE_BYTES);
      return false;
    }

    if (!verifyFrameCheckSequence()) {
      logger.atWarning().log("Frame CRC verification failed");
      return false;
    }

    if (currentFrameSize > MAX_FRAME_SIZE_BYTES) {
      logger.atWarning().log("Frame too big (%d > %d)", currentFrameSize, MAX_FRAME_SIZE_BYTES);
      return false;
    }

    return true;
  }

  private boolean verifyFrameCheckSequence() {
    int crcOffset = currentFrameSize - Frame.CRC_SIZE;
    CRC32 crc32 = new CRC32();
    crc32.update(buffer, 0, crcOffset);
    int actualCrc = (int) crc32.getValue();
    return actualCrc
        == ByteBuffer.wrap(buffer, crcOffset, Frame.CRC_SIZE)
               .order(ByteOrder.LITTLE_ENDIAN)
               .asIntBuffer()
               .get();
  }
}
