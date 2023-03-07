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

#include "src/crypto/fipsmodule/rand/internal.h"

extern "C" {
// OPENSSL_URANDOM is defined automatically based on platform flags.
// See crypto/fipsmodule/rand/internal.h
#ifdef OPENSSL_URANDOM
// When OPENSSL_URANDOM is defined, boringssl assumes linux and
// reads from "dev/urandom" for generating randoms bytes.
// We mock the required file io functions to accomodate it for now.
// TODO(zyecheng): Ask BoringSSL team if there are ways to disable
// OPENSSL_URANDOM, potentially by adding a OPENSSL_PIGWEED flag in
// crypto/fipsmodule/rand/internal.h. If not, we need to keep these
// mockings.

#define URANDOM_FILE_FD 123
int open(const char* file, int, ...) {
  if (strcmp(file, "/dev/urandom") == 0) {
    return URANDOM_FILE_FD;
  }
  return -1;
}

ssize_t read(int fd, void*, size_t len) {
  if (fd == URANDOM_FILE_FD) {
    // TODO(zyecheng): Add code to generate random bytes.
  }
  return static_cast<ssize_t>(len);
}

#else
// When OPENSSL_URANDOM is not defined, BoringSSL expects an implementation of
// the following function for generating random bytes.
void CRYPTO_sysrand(uint8_t*, size_t) {
  // TODO(zyecheng): Add code to generate random bytes.
}
#endif
}
