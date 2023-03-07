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

#include "pw_rpc/internal/log_config.h"
// PW_LOG_* macros must be first.

#include "pw_assert/check.h"
#include "pw_log/log.h"
#include "pw_rpc/benchmark.h"
#include "pw_rpc_system_server/rpc_server.h"
#include "pw_rpc_system_server/socket.h"

namespace {

pw::rpc::BenchmarkService benchmark_service;

}  // namespace

int main(int argc, char* argv[]) {
  if (argc != 2) {
    PW_LOG_ERROR("Usage: %s PORT", argv[0]);
    return 1;
  }

  pw::rpc::system_server::set_socket_port(std::atoi(argv[1]));
  pw::rpc::system_server::Init();
  pw::rpc::system_server::Server().RegisterService(benchmark_service);

  PW_CHECK_OK(pw::rpc::system_server::Start());

  return 0;
}
