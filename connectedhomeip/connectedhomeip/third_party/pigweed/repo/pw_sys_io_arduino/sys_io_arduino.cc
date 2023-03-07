// Copyright 2019 The Pigweed Authors
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

#include <Arduino.h>

#include <cinttypes>
#include <cstdint>

#include "pw_preprocessor/compiler.h"
#include "pw_sys_io/sys_io.h"

extern "C" void pw_sys_io_arduino_Init() {
  // On Linux serial output may still not work if the serial monitor is not
  // connected to ttyACM0 quickly enough after reset. This check forces the
  // device to wait for a serial connection for up to 3 seconds.
  //
  // If you get no serial output, try to connect minicom on the port and then
  // reboot the chip (reset button). If using Python miniterm, start it right
  // after pushing the reset switch.
  while (!Serial && millis() < 3000) {
  }
  Serial.begin(115200);
}

namespace pw::sys_io {

// Wait for a byte to read on USART1. This blocks until a byte is read. This is
// extremely inefficient as it requires the target to burn CPU cycles polling to
// see if a byte is ready yet.

Status ReadByte(std::byte* dest) {
  while (true) {
    if (TryReadByte(dest).ok()) {
      return OkStatus();
    }
  }
}

Status TryReadByte(std::byte* dest) {
  if (!Serial.available()) {
    return Status::Unavailable();
  }
  *dest = static_cast<std::byte>(Serial.read());
  return OkStatus();
}

// Send a byte over the default Arduino Serial port.
Status WriteByte(std::byte b) {
  // Serial.write() will block until data can be written.
  Serial.write((uint8_t)b);
  return OkStatus();
}

// Writes a string using pw::sys_io, and add newline characters at the end.
StatusWithSize WriteLine(const std::string_view& s) {
  size_t chars_written = 0;
  StatusWithSize result = WriteBytes(as_bytes(span(s)));
  if (!result.ok()) {
    return result;
  }
  chars_written += result.size();

  // Write trailing newline.
  result = WriteBytes(as_bytes(span("\r\n", 2)));
  chars_written += result.size();

  return StatusWithSize(result.status(), chars_written);
}

}  // namespace pw::sys_io
