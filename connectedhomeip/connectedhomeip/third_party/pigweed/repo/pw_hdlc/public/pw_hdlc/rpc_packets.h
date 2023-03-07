// Copyright 2020 The Pigweed Authors
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

#include <cstdint>

#include "pw_hdlc/decoder.h"
#include "pw_rpc/channel.h"
#include "pw_rpc/server.h"
#include "pw_status/status.h"

namespace pw::hdlc {

inline constexpr uint8_t kDefaultRpcAddress = 'R';

// Reads HDLC frames with sys_io::ReadByte, using decode_buffer to store frames.
// HDLC frames sent to rpc_address are passed to the RPC server.
Status ReadAndProcessPackets(rpc::Server& server,
                             span<std::byte> decode_buffer,
                             unsigned rpc_address = kDefaultRpcAddress);

}  // namespace pw::hdlc
