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

#include "pw_bloat/bloat_this_binary.h"
#include "pw_crypto/ecdsa.h"

namespace {
#define STR_TO_BYTES(s) pw::as_bytes(pw::span(s, sizeof(s) - 1))

// The SHA256 digest of "Hello, Pigweed!", 32 bytes.
#define DIGEST                                                       \
  "\x8D\xCE\x14\xEE\x2C\xD9\xFD\x9B\xBD\x8C\x8D\x57\x68\x50\x2C\x2F" \
  "\xFB\xB3\x52\x36\xCE\x93\x47\x1B\x80\xFC\xA4\x7D\xB5\xF8\x41\x9D"

// The public key in uncompressed form, 65 bytes.
#define PUBKEY                                                       \
  "\x04"                                                             \
  "\xD1\x82\x2E\x6A\xD2\x4B\x2A\x80\x2E\x8F\xBC\x03\x00\x95\x11\xF9" \
  "\x81\x24\xA7\x3C\x45\xC8\xBA\xDD\x5F\x77\x1C\xC3\x71\x8B\xB2\xE9" \
  "\x3A\x0A\x84\xFF\xEA\x13\xC2\x27\xD2\xCF\x42\x7D\xA5\x95\xD6\x88" \
  "\xCD\x23\x00\x3F\xF9\xD9\x75\x46\xFF\x58\xE9\xBE\xC3\x74\x13\xB8"

// The ECDSA P256 signature of `DIGEST`.
#define SIGNATURE                                                    \
  "\x16\x54\x43\xD4\x00\x07\xC4\xD7\x26\x2E\x3C\xB1\x65\x54\x00\x6A" \
  "\x6A\x5B\x4A\xBB\x16\x6F\x44\xD0\x91\x3F\xD3\xC2\x50\xAC\x1A\x87" \
  "\x86\x41\xEE\x56\xDA\x31\xF2\xFF\x38\x3C\xBB\x32\x3E\x2D\xDB\x98" \
  "\xEA\x05\x9E\x8F\x91\x8E\x0E\x99\xE5\x4F\x32\x13\x92\x7F\x17\x68"
}  // namespace

int main() {
  pw::bloat::BloatThisBinary();

  pw::crypto::ecdsa::VerifyP256Signature(
      STR_TO_BYTES(PUBKEY), STR_TO_BYTES(DIGEST), STR_TO_BYTES(SIGNATURE));
  return 0;
}
