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

#include "pw_sys_io/sys_io.h"

#include <cinttypes>
#include <string_view>

#include "board.h"
#include "fsl_common.h"
#include "fsl_debug_console.h"
#include "pw_status/status.h"
#include "pw_status/status_with_size.h"

extern "C" void pw_sys_io_mcuxpresso_Init(void) { BOARD_InitDebugConsole(); }

namespace pw::sys_io {

// Wait for a byte to read on USART0. This blocks until a byte is read. This is
// extremely inefficient as it requires the target to burn CPU cycles polling to
// see if a byte is ready yet.
Status ReadByte(std::byte* dest) {
  *dest = static_cast<std::byte>(DbgConsole_Getchar());
  return OkStatus();
}

#if DEBUG_CONSOLE_TRANSFER_NON_BLOCKING
// Read a byte from USART0 if one is available.
Status TryReadByte(std::byte* dest) {
  char ch = 0;
  status_t status = DbgConsole_TryGetchar(&ch);
  if (status != kStatus_Success) {
    return Status::Unavailable();
  }

  *dest = static_cast<std::byte>(ch);
  return OkStatus();
}
#else   // !DEBUG_CONSOLE_TRANSFER_NONBLOCKING
Status TryReadByte(std::byte* /*dest*/) { return Status::Unimplemented(); }
#endif  // DEBUG_CONSOLE_TRANSFER_NONBLOCKING

// Send a byte over USART0. Since this blocks on every byte, it's rather
// inefficient. At the default baud rate of 115200, one byte blocks the CPU for
// ~87 micro seconds. This means it takes only 10 bytes to block the CPU for
// 1ms!
Status WriteByte(std::byte b) {
  int ch = static_cast<int>(b);
  int len = DbgConsole_Putchar(ch);
  return len == 1 ? OkStatus() : Status::Unknown();
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
