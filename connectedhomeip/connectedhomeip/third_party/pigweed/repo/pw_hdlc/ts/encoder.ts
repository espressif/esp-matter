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

import * as protocol from './protocol';
import * as util from './util';

const FLAG_BYTE = Uint8Array.from([protocol.FLAG]);

export class Encoder {
  constructor() {}

  /** Encodes an HDLC UI-frame with a CRC-32 frame check sequence. */
  uiFrame(address: number, data: Uint8Array): Uint8Array {
    const addr = protocol.encodeAddress(address);
    const frameControl = protocol.UI_FRAME_CONTROL;
    const start = util.concatenate(addr, frameControl, data);
    let frame = util.concatenate(start, protocol.frameCheckSequence(start));

    frame = util.replace(frame, 0x7d, [0x7d, 0x5d]);
    frame = util.replace(frame, 0x7e, [0x7d, 0x5e]);
    return util.concatenate(FLAG_BYTE, frame, FLAG_BYTE);
  }
}
