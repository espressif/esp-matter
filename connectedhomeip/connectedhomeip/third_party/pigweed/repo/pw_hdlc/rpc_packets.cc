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

#include "pw_hdlc/rpc_packets.h"

#include "pw_status/try.h"
#include "pw_sys_io/sys_io.h"

namespace pw::hdlc {

Status ReadAndProcessPackets(rpc::Server& server,
                             span<std::byte> decode_buffer,
                             unsigned rpc_address) {
  Decoder decoder(decode_buffer);

  while (true) {
    std::byte data;
    PW_TRY(sys_io::ReadByte(&data));

    if (auto result = decoder.Process(data); result.ok()) {
      Frame& frame = result.value();
      if (frame.address() == rpc_address) {
        server.ProcessPacket(frame.data())
            .IgnoreError();  // TODO(b/242598609): Handle Status properly
      }
    }
  }
}

}  // namespace pw::hdlc
