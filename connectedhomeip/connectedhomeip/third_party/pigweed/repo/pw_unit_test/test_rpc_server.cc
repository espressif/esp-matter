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

#include "gtest/gtest.h"
#include "pw_assert/check.h"
#include "pw_log/log.h"
#include "pw_rpc_system_server/rpc_server.h"
#include "pw_rpc_system_server/socket.h"
#include "pw_unit_test/unit_test_service.h"

namespace {

pw::unit_test::UnitTestService unit_test_service;

TEST(Passing, Zero) {}

TEST(Passing, One) { EXPECT_TRUE(true); }

TEST(Passing, Two) {
  EXPECT_FALSE(0);
  EXPECT_STREQ("Yes!", "Yes!\0extra stuff!");
}

TEST(Passing, DISABLED_Disabled) {
  EXPECT_FALSE(0);
  EXPECT_STREQ("Yes!", "Yes!\0extra stuff!");
}

TEST(Failing, Zero) { FAIL(); }

TEST(Failing, One) { EXPECT_TRUE(false); }

TEST(Failing, Two) {
  EXPECT_FALSE(1);
  EXPECT_STREQ("No!", "No?");
}

TEST(Failing, DISABLED_Disabled) {
  EXPECT_FALSE(1);
  EXPECT_STREQ("No!", "No?");
}

TEST(DISABLED_Disabled, Zero) { FAIL(); }

TEST(DISABLED_Disabled, One) { EXPECT_TRUE(false); }

TEST(DISABLED_Disabled, Two) {
  EXPECT_FALSE(1);
  EXPECT_STREQ("No!", "No?");
}

TEST(DISABLED_Disabled, DISABLED_Disabled) {
  EXPECT_FALSE(1);
  EXPECT_STREQ("No!", "No?");
}

}  // namespace

int main(int argc, char* argv[]) {
  if (argc != 2) {
    PW_LOG_ERROR("Usage: %s PORT", argv[0]);
    return 1;
  }
  pw::rpc::system_server::set_socket_port(std::atoi(argv[1]));
  pw::rpc::system_server::Init();
  pw::rpc::system_server::Server().RegisterService(unit_test_service);

  PW_LOG_INFO("Starting pw_rpc server");
  PW_CHECK_OK(pw::rpc::system_server::Start());

  return 0;
}
