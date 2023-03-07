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

#pragma once

#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <linux/types.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "pw_log/log.h"
#include "pw_spi/chip_selector.h"
#include "pw_spi/device.h"
#include "pw_spi/initiator.h"
namespace pw::spi {

// Linux userspace implementation of the SPI Initiator
class LinuxInitiator : public Initiator {
 public:
  // Configure the Linux Initiator object for use with a bus specified by path,
  // with a maximum bus-speed (in hz).
  constexpr LinuxInitiator(const char* path, uint32_t max_speed_hz)
      : path_(path), max_speed_hz_(max_speed_hz), fd_(-1) {}
  ~LinuxInitiator();

  // Implements pw::spi::Initiator
  Status Configure(const Config& config) override;
  Status WriteRead(ConstByteSpan write_buffer, ByteSpan read_buffer) override;

 private:
  Status LazyInit();

  const char* path_;
  uint32_t max_speed_hz_;
  int fd_;
};

// Linux userspace implementation of SPI ChipSelector
class LinuxChipSelector : public ChipSelector {
 public:
  Status SetActive(bool active) override;
};

}  // namespace pw::spi
