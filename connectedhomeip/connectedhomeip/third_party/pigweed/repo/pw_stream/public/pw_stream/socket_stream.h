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

#include <netinet/in.h>

#include <cstdint>

#include "pw_span/span.h"
#include "pw_stream/stream.h"

namespace pw::stream {

class SocketStream : public NonSeekableReaderWriter {
 public:
  constexpr SocketStream() = default;

  ~SocketStream() override { Close(); }

  // Listen to the port and return after a client is connected
  Status Serve(uint16_t port);

  // Connect to a local or remote endpoint. Host must be an IPv4 address. If
  // host is nullptr then the locahost address is used instead.
  Status Connect(const char* host, uint16_t port);

  // Close the socket stream and release all resources
  void Close();

  // Exposes the file descriptor for the active connection. This is exposed to
  // allow configuration and introspection of this socket's current
  // configuration using setsockopt() and getsockopt().
  //
  // Returns -1 if there is no active connection.
  int connection_fd() { return connection_fd_; }

 private:
  static constexpr int kInvalidFd = -1;

  Status DoWrite(span<const std::byte> data) override;

  StatusWithSize DoRead(ByteSpan dest) override;

  uint16_t listen_port_ = 0;
  int socket_fd_ = kInvalidFd;
  int connection_fd_ = kInvalidFd;
  struct sockaddr_in sockaddr_client_ = {};
};

}  // namespace pw::stream
