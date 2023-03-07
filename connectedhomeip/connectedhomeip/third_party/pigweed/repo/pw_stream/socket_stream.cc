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

#include "pw_stream/socket_stream.h"

#include <arpa/inet.h>
#include <unistd.h>

#include <cstring>

#include "pw_log/log.h"

namespace pw::stream {
namespace {

constexpr uint32_t kMaxConcurrentUser = 1;
constexpr const char* kLocalhostAddress = "127.0.0.1";

}  // namespace

// TODO(b/240982565): Implement SocketStream for Windows.

// Listen to the port and return after a client is connected
Status SocketStream::Serve(uint16_t port) {
  listen_port_ = port;
  socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd_ == kInvalidFd) {
    PW_LOG_ERROR("Failed to create socket: %s", std::strerror(errno));
    return Status::Unknown();
  }

  struct sockaddr_in addr = {};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(listen_port_);
  addr.sin_addr.s_addr = INADDR_ANY;

  // Configure the socket to allow reusing the address. Closing a socket does
  // not immediately close it. Instead, the socket waits for some period of time
  // before it is actually closed. Setting SO_REUSEADDR allows this socket to
  // bind to an address that may still be in use by a recently closed socket.
  // Without this option, running a program multiple times in series may fail
  // unexpectedly.
  constexpr int value = 1;

  if (setsockopt(socket_fd_, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(int)) <
      0) {
    PW_LOG_WARN("Failed to set SO_REUSEADDR: %s", std::strerror(errno));
  }

  if (bind(socket_fd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
    PW_LOG_ERROR("Failed to bind socket to localhost:%hu: %s",
                 listen_port_,
                 std::strerror(errno));
    return Status::Unknown();
  }

  if (listen(socket_fd_, kMaxConcurrentUser) < 0) {
    PW_LOG_ERROR("Failed to listen to socket: %s", std::strerror(errno));
    return Status::Unknown();
  }

  socklen_t len = sizeof(sockaddr_client_);

  connection_fd_ =
      accept(socket_fd_, reinterpret_cast<sockaddr*>(&sockaddr_client_), &len);
  if (connection_fd_ < 0) {
    return Status::Unknown();
  }
  return OkStatus();
}

Status SocketStream::SocketStream::Connect(const char* host, uint16_t port) {
  connection_fd_ = socket(AF_INET, SOCK_STREAM, 0);

  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);

  if (host == nullptr || std::strcmp(host, "localhost") == 0) {
    host = kLocalhostAddress;
  }

  if (inet_pton(AF_INET, host, &addr.sin_addr) <= 0) {
    PW_LOG_ERROR("Failed to configure connection address for socket");
    return Status::InvalidArgument();
  }

  if (connect(connection_fd_,
              reinterpret_cast<sockaddr*>(&addr),
              sizeof(addr)) < 0) {
    PW_LOG_ERROR(
        "Failed to connect to %s:%d: %s", host, port, std::strerror(errno));
    return Status::Unknown();
  }

  return OkStatus();
}

void SocketStream::Close() {
  if (socket_fd_ != kInvalidFd) {
    close(socket_fd_);
    socket_fd_ = kInvalidFd;
  }

  if (connection_fd_ != kInvalidFd) {
    close(connection_fd_);
    connection_fd_ = kInvalidFd;
  }
}

Status SocketStream::DoWrite(span<const std::byte> data) {
  // Use MSG_NOSIGNAL to avoid getting a SIGPIPE signal when the remote
  // peer drops the connection.
  ssize_t bytes_sent =
      send(connection_fd_, data.data(), data.size_bytes(), MSG_NOSIGNAL);

  if (bytes_sent < 0 || static_cast<size_t>(bytes_sent) != data.size()) {
    if (errno == EPIPE) {
      // An EPIPE indicates that the connection is closed.  Return an OutOfRange
      // error.
      return Status::OutOfRange();
    }

    return Status::Unknown();
  }
  return OkStatus();
}

StatusWithSize SocketStream::DoRead(ByteSpan dest) {
  ssize_t bytes_rcvd = recv(connection_fd_, dest.data(), dest.size_bytes(), 0);
  if (bytes_rcvd == 0) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      // Socket timed out when trying to read.
      // This should only occur if SO_RCVTIMEO was configured to be nonzero, or
      // if the socket was opened with the O_NONBLOCK flag to prevent any
      // blocking when performing reads or writes.
      return StatusWithSize::ResourceExhausted();
    }
    // Remote peer has closed the connection.
    Close();
    return StatusWithSize::OutOfRange();
  } else if (bytes_rcvd < 0) {
    return StatusWithSize::Unknown();
  }
  return StatusWithSize(bytes_rcvd);
}

}  // namespace pw::stream
