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

#include "pw_assert/check.h"
#include "pw_bloat/bloat_this_binary.h"
#include "pw_log/log.h"
#include "pw_sys_io/sys_io.h"

namespace {

struct BasicPacket {
  static constexpr uint32_t kMagic = 0x8badf00d;

  constexpr BasicPacket(uint32_t addr, uint64_t data)
      : magic(kMagic), address(addr), payload(data) {}

  uint32_t magic;
  uint32_t address;
  uint64_t payload;
};

}  // namespace

int main() {
  pw::bloat::BloatThisBinary();

  // Ensure we are paying the cost for log and assert.
  BasicPacket packet(0x1, 0x2);
  PW_CHECK_UINT_EQ(packet.magic, BasicPacket::kMagic, "Some CHECK logic");
  PW_LOG_INFO("Packet has address %u", static_cast<unsigned>(packet.address));
  PW_LOG_INFO("pw_StatusString %s", pw::OkStatus().str());

  std::array<std::byte, sizeof(BasicPacket)> packet_buffer;
  pw::sys_io::ReadBytes(packet_buffer);
  pw::sys_io::WriteBytes(packet_buffer);

  return static_cast<int>(packet.payload);
}
