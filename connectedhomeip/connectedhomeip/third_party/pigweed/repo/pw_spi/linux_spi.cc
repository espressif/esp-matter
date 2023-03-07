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

#include "pw_spi/linux_spi.h"

#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <linux/types.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <cstring>

#include "pw_log/log.h"
#include "pw_spi/chip_selector.h"
#include "pw_spi/device.h"
#include "pw_spi/initiator.h"
#include "pw_status/try.h"

namespace pw::spi {

LinuxInitiator::~LinuxInitiator() {
  if (fd_ >= 0) {
    close(fd_);
  }
}

Status LinuxInitiator::LazyInit() {
  if (fd_ >= 0) {
    return OkStatus();
  }
  fd_ = open(path_, O_RDWR | O_EXCL);
  if (fd_ < 0) {
    PW_LOG_ERROR("Unable to open SPI device %s for read/write", path_);
    return Status::Unavailable();
  }
  return OkStatus();
}

Status LinuxInitiator::Configure(const Config& config) {
  PW_TRY(LazyInit());

  // Map clock polarity/phase to Linux userspace equivalents
  uint32_t mode = 0;
  if (config.polarity == ClockPolarity::kActiveLow) {
    mode |= SPI_CPOL;  // Clock polarity -- signal is high when idle
  }
  if (config.phase == ClockPhase::kFallingEdge) {
    mode |= SPI_CPHA;  // Clock phase -- latch on falling edge
  }
  if (ioctl(fd_, SPI_IOC_WR_MODE32, &mode) < 0) {
    PW_LOG_ERROR("Unable to set SPI mode");
    return Status::InvalidArgument();
  }

  // Configure LSB/MSB first
  uint8_t lsb_first = 0;
  if (config.bit_order == BitOrder::kLsbFirst) {
    lsb_first = 1;  // non-zero value indicates LSB first
  }
  if (ioctl(fd_, SPI_IOC_WR_LSB_FIRST, &lsb_first) < 0) {
    PW_LOG_ERROR("Unable to set SPI LSB");
    return Status::InvalidArgument();
  }

  // Configure bits-per-word
  uint8_t bits_per_word = config.bits_per_word();
  if (ioctl(fd_, SPI_IOC_WR_BITS_PER_WORD, &bits_per_word) < 0) {
    PW_LOG_ERROR("Unable to set SPI Bits Per Word");
    return Status::InvalidArgument();
  }

  // Configure maximum bus speed
  if (ioctl(fd_, SPI_IOC_WR_MAX_SPEED_HZ, &max_speed_hz_) < 0) {
    PW_LOG_ERROR("Unable to set SPI Max Speed");
    return Status::InvalidArgument();
  }

  return OkStatus();
}

Status LinuxInitiator::WriteRead(ConstByteSpan write_buffer,
                                 ByteSpan read_buffer) {
  PW_TRY(LazyInit());

  // Configure a full-duplex transfer using ioctl()
  struct spi_ioc_transfer transaction[2];
  memset(transaction, 0, sizeof(transaction));
  transaction[0].tx_buf = reinterpret_cast<uintptr_t>(write_buffer.data());
  transaction[0].len = write_buffer.size();

  transaction[1].rx_buf = reinterpret_cast<uintptr_t>(read_buffer.data());
  transaction[1].len = read_buffer.size();

  if (ioctl(fd_, SPI_IOC_MESSAGE(2), transaction) < 0) {
    PW_LOG_ERROR("Unable to perform SPI transfer");
    return Status::Unknown();
  }

  return OkStatus();
}

Status LinuxChipSelector::SetActive(bool /*active*/) {
  // Note: For Linux' SPI userspace support, chip-select control is not exposed
  // directly to the user.  This limits our ability to use the SPI HAL to do
  // composite (multi read-write) transactions with the PW SPI HAL, as Linux
  // performs composite transactions with a single ioctl() call using an array
  // of descriptors provided as a parameter -- there's no way of separating
  // individual operations from userspace.  This could be addressed with a
  // direct "Composite" transaction HAL API, or by using a raw GPIO
  // to control of chip select from userspace (which is not common practice).
  return OkStatus();
}

}  // namespace pw::spi
