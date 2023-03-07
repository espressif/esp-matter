// Copyright 2021 The Pigweed Authors
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

/** Decoder class for decoding bytes using HDLC protocol */

import * as protocol from './protocol';
import * as util from './util';

const _MIN_FRAME_SIZE = 6; // 1 B address + 1 B control + 4 B CRC-32

/** Indicates if an error occurred */
export enum FrameStatus {
  OK = 'OK',
  FCS_MISMATCH = 'frame check sequence failure',
  FRAMING_ERROR = 'invalid flag or escape characters',
  BAD_ADDRESS = 'address field too long',
}

/**
 * A single HDLC frame
 */
export class Frame {
  rawEncoded: Uint8Array;
  rawDecoded: Uint8Array;
  status: FrameStatus;

  address = -1;
  control: Uint8Array = new Uint8Array();
  data: Uint8Array = new Uint8Array();

  constructor(
    rawEncoded: Uint8Array,
    rawDecoded: Uint8Array,
    status: FrameStatus = FrameStatus.OK
  ) {
    this.rawEncoded = rawEncoded;
    this.rawDecoded = rawDecoded;
    this.status = status;

    if (status === FrameStatus.OK) {
      const [address, addressLength] = protocol.decodeAddress(rawDecoded);
      if (addressLength === 0) {
        this.status = FrameStatus.BAD_ADDRESS;
        return;
      }
      this.address = address;
      this.control = rawDecoded.slice(addressLength, addressLength + 1);
      this.data = rawDecoded.slice(addressLength + 1, -4);
    }
  }

  /**
   * True if this represents a valid frame.
   *
   * If false, then parsing failed. The status is set to indicate what type of
   * error occurred, and the data field contains all bytes parsed from the frame
   * (including bytes parsed as address or control bytes).
   */
  ok(): boolean {
    return this.status === FrameStatus.OK;
  }
}

enum DecoderState {
  INTERFRAME,
  FRAME,
  FRAME_ESCAPE,
}

/** Decodes one or more HDLC frames from a stream of data. */
export class Decoder {
  private decodedData = new Uint8Array();
  private rawData = new Uint8Array();
  private state = DecoderState.INTERFRAME;

  /**
   *  Decodes and yields HDLC frames, including corrupt frames
   *
   *  The ok() method on Frame indicates whether it is valid or represents a
   *  frame parsing error.
   *
   *  @yield Frames, which may be valid (frame.ok)) okr corrupt (!frame.ok())
   */
  *process(data: Uint8Array): IterableIterator<Frame> {
    for (const byte of data) {
      const frame = this.processByte(byte);
      if (frame != null) {
        yield frame;
      }
    }
  }

  /**
   *  Decodes and yields valid HDLC frames, logging any errors.
   *
   *  @yield Valid HDLC frames
   */
  *processValidFrames(data: Uint8Array): IterableIterator<Frame> {
    const frames = this.process(data);
    for (const frame of frames) {
      if (frame.ok()) {
        yield frame;
      } else {
        console.warn(
          'Failed to decode frame: %s; discarded %d bytes',
          frame.status,
          frame.rawEncoded.length
        );
        console.debug('Discarded data: %s', frame.rawEncoded);
      }
    }
  }

  private checkFrame(data: Uint8Array): FrameStatus {
    if (data.length < _MIN_FRAME_SIZE) {
      return FrameStatus.FRAMING_ERROR;
    }
    const frameCrc = new DataView(data.slice(-4).buffer).getInt8(0);
    const crc = new DataView(
      protocol.frameCheckSequence(data.slice(0, -4)).buffer
    ).getInt8(0);
    if (crc !== frameCrc) {
      return FrameStatus.FCS_MISMATCH;
    }
    return FrameStatus.OK;
  }

  private finishFrame(status: FrameStatus): Frame {
    const frame = new Frame(
      new Uint8Array(this.rawData),
      new Uint8Array(this.decodedData),
      status
    );
    this.rawData = new Uint8Array();
    this.decodedData = new Uint8Array();
    return frame;
  }

  private processByte(byte: number): Frame | undefined {
    let frame;

    // Record every byte except the flag character.
    if (byte != protocol.FLAG) {
      this.rawData = util.concatenate(this.rawData, Uint8Array.of(byte));
    }

    switch (this.state) {
      case DecoderState.INTERFRAME:
        if (byte === protocol.FLAG) {
          if (this.rawData.length > 0) {
            frame = this.finishFrame(FrameStatus.FRAMING_ERROR);
          }
          this.state = DecoderState.FRAME;
        }
        break;

      case DecoderState.FRAME:
        if (byte == protocol.FLAG) {
          if (this.rawData.length > 0) {
            frame = this.finishFrame(this.checkFrame(this.decodedData));
          }
        } else if (byte == protocol.ESCAPE) {
          this.state = DecoderState.FRAME_ESCAPE;
        } else {
          this.decodedData = util.concatenate(
            this.decodedData,
            Uint8Array.of(byte)
          );
        }
        break;

      case DecoderState.FRAME_ESCAPE:
        if (byte === protocol.FLAG) {
          frame = this.finishFrame(FrameStatus.FRAMING_ERROR);
          this.state = DecoderState.FRAME;
        } else if (protocol.VALID_ESCAPED_BYTES.includes(byte)) {
          this.state = DecoderState.FRAME;
          this.decodedData = util.concatenate(
            this.decodedData,
            Uint8Array.of(protocol.escape(byte))
          );
        } else {
          this.state = DecoderState.INTERFRAME;
        }
        break;
    }
    return frame;
  }
}
