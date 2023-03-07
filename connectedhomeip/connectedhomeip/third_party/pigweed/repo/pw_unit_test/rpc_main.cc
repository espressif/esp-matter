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

#include "pw_log/log.h"
#include "pw_rpc_system_server/rpc_server.h"
#include "pw_unit_test/unit_test_service.h"

namespace {

pw::unit_test::UnitTestService unit_test_service;

}  // namespace

int main() {
  pw::rpc::system_server::Init();
  pw::rpc::system_server::Server().RegisterService(unit_test_service);

  PW_LOG_INFO("Starting pw_rpc server");
  pw::rpc::system_server::Start();

  return 0;
}
