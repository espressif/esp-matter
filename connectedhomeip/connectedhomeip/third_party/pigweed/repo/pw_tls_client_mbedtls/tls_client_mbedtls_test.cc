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
#include "pw_stream/null_stream.h"
#include "pw_tls_client/session.h"

namespace pw::tls_client {

TEST(TLSClientMbedTLS, CreateSucceed) {
  auto options = SessionOptions().set_transport(stream::NullStream::Instance());
  auto res = Session::Create(options);
  ASSERT_EQ(res.status(), OkStatus());
  ASSERT_NE(res.value(), nullptr);
}

TEST(TLSClientMbedTLS, CreateFailOnMissingTransport) {
  auto options = SessionOptions();
  auto res = Session::Create(options);
  ASSERT_NE(res.status(), OkStatus());
}

TEST(TLSClientMbedTLS, EntropySourceFail) {
  backend::SessionImplementation::SetEntropySourceStatus(Status::Internal());
  auto options = SessionOptions().set_transport(stream::NullStream::Instance());
  auto res = Session::Create(options);
  ASSERT_NE(res.status(), OkStatus());
}

}  // namespace pw::tls_client