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

#include <cstdio>
#include <mutex>

#include "pw_assert/check.h"
#include "pw_stream/socket_stream.h"
#include "pw_stream/stream.h"
#include "pw_system/config.h"
#include "pw_system/io.h"

namespace pw::system {
namespace {

constexpr uint16_t kPort = PW_SYSTEM_SOCKET_IO_PORT;

stream::SocketStream& GetStream() {
  static bool running = false;
  static std::mutex socket_open_lock;
  static stream::SocketStream socket_stream;
  std::lock_guard guard(socket_open_lock);
  if (!running) {
    printf("Awaiting connection on port %d\n", static_cast<int>(kPort));
    PW_CHECK_OK(socket_stream.Serve(kPort));
    printf("Client connected\n");
    running = true;
  }
  return socket_stream;
}

}  // namespace

stream::Reader& GetReader() { return GetStream(); }
stream::Writer& GetWriter() { return GetStream(); }

}  // namespace pw::system
