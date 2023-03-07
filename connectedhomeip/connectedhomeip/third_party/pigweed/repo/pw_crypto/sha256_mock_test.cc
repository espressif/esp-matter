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

#include <cstring>

#include "gtest/gtest.h"
#include "pw_crypto/sha256.h"
#include "pw_crypto/sha256_backend.h"

namespace pw::crypto::sha256 {
namespace {

#define AS_BYTES(str) as_bytes(span(str, sizeof(str) - 1))

#define ASSERT_OK(expr) ASSERT_EQ(OkStatus(), expr)
#define ASSERT_FAIL(expr) ASSERT_NE(OkStatus(), expr)

TEST(Sha256, HandlesBackendInitFailures) {
  std::byte digest[kDigestSizeBytes];

  backend::ClearError();
  ASSERT_OK(Sha256().Update(AS_BYTES("blahblah")).Final(digest));

  backend::InjectError(backend::ErrorKind::kInit);
  ASSERT_FAIL(Sha256().Update(AS_BYTES("blahblah")).Final(digest));
}

TEST(Sha256, HandlesBackendUpdateFailures) {
  std::byte digest[kDigestSizeBytes];

  backend::ClearError();
  ASSERT_OK(Sha256().Update(AS_BYTES("blahblah")).Final(digest));

  backend::InjectError(backend::ErrorKind::kUpdate);
  ASSERT_FAIL(Sha256().Update(AS_BYTES("blahblah")).Final(digest));
}

TEST(Sha256, HandlesBackendFinalFailures) {
  std::byte digest[kDigestSizeBytes];

  backend::ClearError();
  ASSERT_OK(Sha256().Update(AS_BYTES("blahblah")).Final(digest));

  backend::InjectError(backend::ErrorKind::kFinal);
  ASSERT_FAIL(Sha256().Update(AS_BYTES("blahblah")).Final(digest));
}

}  // namespace
}  // namespace pw::crypto::sha256
