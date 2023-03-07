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

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>

#include "pw_assert/check.h"
#include "pw_hdlc/encoded_size.h"
#include "pw_hdlc/rpc_channel.h"
#include "pw_hdlc/rpc_packets.h"
#include "pw_log/log.h"
#include "pw_sync/mutex.h"
#include "pw_system/config.h"
#include "pw_system/io.h"
#include "pw_system/rpc_server.h"

namespace pw::system {
namespace {

constexpr size_t kMaxTransmissionUnit = PW_SYSTEM_MAX_TRANSMISSION_UNIT;

static_assert(kMaxTransmissionUnit ==
              hdlc::MaxEncodedFrameSize(rpc::cfg::kEncodingBufferSizeBytes));

hdlc::FixedMtuChannelOutput<kMaxTransmissionUnit> hdlc_channel_output(
    GetWriter(), PW_SYSTEM_DEFAULT_RPC_HDLC_ADDRESS, "HDLC channel");
rpc::Channel channels[] = {
    rpc::Channel::Create<kDefaultRpcChannelId>(&hdlc_channel_output)};
rpc::Server server(channels);

constexpr size_t kDecoderBufferSize =
    hdlc::Decoder::RequiredBufferSizeForFrameSize(kMaxTransmissionUnit);
// Declare a buffer for decoding incoming HDLC frames.
std::array<std::byte, kDecoderBufferSize> input_buffer;
hdlc::Decoder decoder(input_buffer);

std::array<std::byte, 1> data;

}  // namespace

rpc::Server& GetRpcServer() { return server; }

class RpcDispatchThread final : public thread::ThreadCore {
 public:
  RpcDispatchThread() = default;
  RpcDispatchThread(const RpcDispatchThread&) = delete;
  RpcDispatchThread(RpcDispatchThread&&) = delete;
  RpcDispatchThread& operator=(const RpcDispatchThread&) = delete;
  RpcDispatchThread& operator=(RpcDispatchThread&&) = delete;

  void Run() override {
    PW_LOG_INFO("Running RPC server");
    while (true) {
      auto ret_val = GetReader().Read(data);
      if (ret_val.ok()) {
        for (std::byte byte : ret_val.value()) {
          if (auto result = decoder.Process(byte); result.ok()) {
            hdlc::Frame& frame = result.value();
            if (frame.address() == PW_SYSTEM_DEFAULT_RPC_HDLC_ADDRESS) {
              server.ProcessPacket(frame.data());
            }
          }
        }
      }
    }
  }
};

thread::ThreadCore& GetRpcDispatchThread() {
  static RpcDispatchThread rpc_dispatch_thread;
  return rpc_dispatch_thread;
}

}  // namespace pw::system
