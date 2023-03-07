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

#include <array>
#include <string_view>

#include "pw_assert/check.h"
#include "pw_hdlc/encoder.h"
#include "pw_hdlc/rpc_packets.h"
#include "pw_log/log.h"
#include "pw_rpc/echo_service_nanopb.h"
#include "pw_rpc/server.h"
#include "pw_rpc_system_server/rpc_server.h"
#include "pw_span/span.h"

namespace hdlc_example {
namespace {

pw::rpc::EchoService echo_service;

void RegisterServices() {
  pw::rpc::system_server::Server().RegisterService(echo_service);
}

}  // namespace

void Start() {
  pw::rpc::system_server::Init();

  // Set up the server and start processing data.
  RegisterServices();

  PW_LOG_INFO("Starting pw_rpc server");
  PW_CHECK_OK(pw::rpc::system_server::Start());
}

}  // namespace hdlc_example
