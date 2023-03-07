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

/** Constants shared between Encoder and Decoder */
public final class Protocol {
  static final byte UNNUMBERED_INFORMATION = 0x03;
  static final byte FLAG = 0x7E;
  static final byte ESCAPE = 0x7D;
  static final byte[] ESCAPED_FLAG = {ESCAPE, 0x5E};
  static final byte[] ESCAPED_ESCAPE = {ESCAPE, 0x5D};
  static final byte ESCAPE_CONSTANT = 0x20;

  private Protocol() {}
}