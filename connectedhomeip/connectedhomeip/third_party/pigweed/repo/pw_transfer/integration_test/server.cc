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

// Simple RPC server with the transfer service registered. Reads HDLC frames
// with RPC packets through a socket. This server has a single resource ID that
// is available, and data must be written to the server before data can be read
// from the resource ID.
//
// Usage:
//
//   integration_test_server 3300 <<< "resource_id: 12 file: '/tmp/gotbytes'"

#include <sys/socket.h>

#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <deque>
#include <map>
#include <memory>
#include <string>
#include <thread>
#include <utility>
#include <variant>
#include <vector>

#include "google/protobuf/text_format.h"
#include "pw_assert/check.h"
#include "pw_chrono/system_clock.h"
#include "pw_log/log.h"
#include "pw_rpc_system_server/rpc_server.h"
#include "pw_rpc_system_server/socket.h"
#include "pw_stream/std_file_stream.h"
#include "pw_thread/thread.h"
#include "pw_thread_stl/options.h"
#include "pw_transfer/integration_test/config.pb.h"
#include "pw_transfer/transfer.h"

namespace pw::transfer {
namespace {

using stream::MemoryReader;
using stream::MemoryWriter;

// This is the maximum size of the socket send buffers. Ideally, this is set
// to the lowest allowed value to minimize buffering between the proxy and
// clients so rate limiting causes the client to block and wait for the
// integration test proxy to drain rather than allowing OS buffers to backlog
// large quantities of data.
//
// Note that the OS may chose to not strictly follow this requested buffer size.
// Still, setting this value to be as small as possible does reduce bufer sizes
// significantly enough to better reflect typical inter-device communication.
//
// For this to be effective, servers should also configure their sockets to a
// smaller receive buffer size.
constexpr int kMaxSocketSendBufferSize = 1;

class FileTransferHandler final : public ReadWriteHandler {
 public:
  FileTransferHandler(uint32_t resource_id,
                      std::deque<std::string>&& sources,
                      std::deque<std::string>&& destinations,
                      std::string default_source_path,
                      std::string default_destination_path)
      : ReadWriteHandler(resource_id),
        sources_(sources),
        destinations_(destinations),
        default_source_path_(default_source_path),
        default_destination_path_(default_destination_path) {}

  ~FileTransferHandler() = default;

  Status PrepareRead() final {
    if (sources_.empty() && default_source_path_.length() == 0) {
      PW_LOG_ERROR("Source paths exhausted");
      return Status::ResourceExhausted();
    }

    std::string path;
    if (!sources_.empty()) {
      path = sources_.front();
      sources_.pop_front();
    } else {
      path = default_source_path_;
    }

    PW_LOG_DEBUG("Preparing read for file %s", path.c_str());
    set_reader(stream_.emplace<stream::StdFileReader>(path.c_str()));
    return OkStatus();
  }

  void FinalizeRead(Status) final {
    std::get<stream::StdFileReader>(stream_).Close();
  }

  Status PrepareWrite() final {
    if (destinations_.empty() && default_destination_path_.length() == 0) {
      PW_LOG_ERROR("Destination paths exhausted");
      return Status::ResourceExhausted();
    }

    std::string path;
    if (!destinations_.empty()) {
      path = destinations_.front();
      destinations_.pop_front();
    } else {
      path = default_destination_path_;
    }

    PW_LOG_DEBUG("Preparing write for file %s", path.c_str());
    set_writer(stream_.emplace<stream::StdFileWriter>(path.c_str()));
    return OkStatus();
  }

  Status FinalizeWrite(Status) final {
    std::get<stream::StdFileWriter>(stream_).Close();
    return OkStatus();
  }

 private:
  std::deque<std::string> sources_;
  std::deque<std::string> destinations_;
  std::string default_source_path_;
  std::string default_destination_path_;
  std::variant<std::monostate, stream::StdFileReader, stream::StdFileWriter>
      stream_;
};

void RunServer(int socket_port, ServerConfig config) {
  std::vector<std::byte> chunk_buffer(config.chunk_size_bytes());
  std::vector<std::byte> encode_buffer(config.chunk_size_bytes());
  transfer::Thread<4, 4> transfer_thread(chunk_buffer, encode_buffer);
  TransferService transfer_service(
      transfer_thread,
      config.pending_bytes(),
      std::chrono::seconds(config.chunk_timeout_seconds()),
      config.transfer_service_retries(),
      config.extend_window_divisor());

  rpc::system_server::set_socket_port(socket_port);

  rpc::system_server::Init();
  rpc::system_server::Server().RegisterService(transfer_service);

  // Start transfer thread.
  thread::Thread transfer_thread_handle =
      thread::Thread(thread::stl::Options(), transfer_thread);

  int retval = setsockopt(rpc::system_server::GetServerSocketFd(),
                          SOL_SOCKET,
                          SO_SNDBUF,
                          &kMaxSocketSendBufferSize,
                          sizeof(kMaxSocketSendBufferSize));
  PW_CHECK_INT_EQ(retval,
                  0,
                  "Failed to configure socket send buffer size with errno=%d",
                  errno);

  std::vector<std::unique_ptr<FileTransferHandler>> handlers;
  for (const auto& resource : config.resources()) {
    uint32_t id = resource.first;

    std::deque<std::string> source_paths(resource.second.source_paths().begin(),
                                         resource.second.source_paths().end());
    std::deque<std::string> destination_paths(
        resource.second.destination_paths().begin(),
        resource.second.destination_paths().end());

    auto handler = std::make_unique<FileTransferHandler>(
        id,
        std::move(source_paths),
        std::move(destination_paths),
        resource.second.default_source_path(),
        resource.second.default_destination_path());

    transfer_service.RegisterHandler(*handler);
    handlers.push_back(std::move(handler));
  }

  PW_LOG_INFO("Starting pw_rpc server");
  PW_CHECK_OK(rpc::system_server::Start());

  // Unregister transfer handler before cleaning up the thread since doing so
  // requires the transfer thread to be running.
  for (auto& handler : handlers) {
    transfer_service.UnregisterHandler(*handler);
  }

  // End transfer thread.
  transfer_thread.Terminate();
  transfer_thread_handle.join();
}

}  // namespace
}  // namespace pw::transfer

int main(int argc, char* argv[]) {
  if (argc != 2) {
    PW_LOG_INFO("Usage: %s PORT <<< config textproto", argv[0]);
    return 1;
  }

  int port = std::atoi(argv[1]);
  PW_CHECK_UINT_GT(port, 0, "Invalid port!");

  std::string config_string;
  std::string line;
  while (std::getline(std::cin, line)) {
    config_string = config_string + line + '\n';
  }
  pw::transfer::ServerConfig config;

  bool ok =
      google::protobuf::TextFormat::ParseFromString(config_string, &config);
  if (!ok) {
    PW_LOG_INFO("Failed to parse config: %s", config_string.c_str());
    PW_LOG_INFO("Usage: %s PORT <<< config textproto", argv[0]);
    return 1;
  } else {
    PW_LOG_INFO("Server loaded config:\n%s", config.DebugString().c_str());
  }

  pw::transfer::RunServer(port, config);
  return 0;
}
