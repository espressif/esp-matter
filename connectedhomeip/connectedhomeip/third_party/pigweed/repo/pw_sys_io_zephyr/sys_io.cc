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

#include <zephyr/console/console.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/usb/usb_device.h>

static int sys_io_init(const struct device* dev) {
  int err;
  ARG_UNUSED(dev);

  if (IS_ENABLED(CONFIG_PIGWEED_SYS_IO_USB)) {
    err = usb_enable(nullptr);
    if (err) {
      return err;
    }
  }
  err = console_init();
  return err;
}

SYS_INIT(sys_io_init, APPLICATION, CONFIG_PIGWEED_SYS_IO_INIT_PRIORITY);

namespace pw::sys_io {

Status ReadByte(std::byte* dest) {
  if (dest == nullptr) {
    return Status::InvalidArgument();
  }

  const int c = console_getchar();
  *dest = static_cast<std::byte>(c);

  return c < 0 ? Status::FailedPrecondition() : OkStatus();
}

Status TryReadByte(std::byte* dest) {
  if (dest == nullptr) {
    return Status::InvalidArgument();
  }

  uint8_t byte;
  int result = console_read(nullptr, &byte, 1);

  if (result >= 0) {
    *dest = static_cast<std::byte>(byte);
    return OkStatus();
  }

  return Status::Unavailable();
}

Status WriteByte(std::byte b) {
  return console_putchar(static_cast<char>(b)) < 0
             ? Status::FailedPrecondition()
             : OkStatus();
}

StatusWithSize WriteLine(const std::string_view& s) {
  size_t chars_written = 0;
  StatusWithSize size_result = WriteBytes(as_bytes(span(s)));
  if (!size_result.ok()) {
    return size_result;
  }
  chars_written += size_result.size();

  // Write trailing newline character.
  Status result = WriteByte(static_cast<std::byte>('\n'));
  if (result.ok()) {
    chars_written++;
  }

  return StatusWithSize(result, chars_written);
}

}  // namespace pw::sys_io
