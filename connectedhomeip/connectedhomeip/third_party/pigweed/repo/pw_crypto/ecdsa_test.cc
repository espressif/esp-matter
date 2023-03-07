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

#include "pw_crypto/ecdsa.h"

#include <cstring>

#include "gtest/gtest.h"

namespace pw::crypto::ecdsa {
namespace {

#define AS_BYTES(s) as_bytes(span(s, sizeof(s) - 1))

#define ASSERT_OK(expr) ASSERT_EQ(OkStatus(), expr)
#define ASSERT_FAIL(expr) ASSERT_NE(OkStatus(), expr)

// TEST_DIGEST/PUBKEY/SIGNATURE are generated using the pkey/ecdsa.c
// example in Mbed TLS.

// The SHA256 digest of "Hello, Pigweed!", 32 bytes.
#define TEST_DIGEST                                                  \
  "\x8D\xCE\x14\xEE\x2C\xD9\xFD\x9B\xBD\x8C\x8D\x57\x68\x50\x2C\x2F" \
  "\xFB\xB3\x52\x36\xCE\x93\x47\x1B\x80\xFC\xA4\x7D\xB5\xF8\x41\x9D"

// The public key in uncompressed form, 65 bytes.
#define TEST_PUBKEY                                                  \
  "\x04"                                                             \
  "\xD1\x82\x2E\x6A\xD2\x4B\x2A\x80\x2E\x8F\xBC\x03\x00\x95\x11\xF9" \
  "\x81\x24\xA7\x3C\x45\xC8\xBA\xDD\x5F\x77\x1C\xC3\x71\x8B\xB2\xE9" \
  "\x3A\x0A\x84\xFF\xEA\x13\xC2\x27\xD2\xCF\x42\x7D\xA5\x95\xD6\x88" \
  "\xCD\x23\x00\x3F\xF9\xD9\x75\x46\xFF\x58\xE9\xBE\xC3\x74\x13\xB8"

// The ECDSA P256 signature of `DIGEST`.
#define TEST_SIGNATURE                                               \
  "\x16\x54\x43\xD4\x00\x07\xC4\xD7\x26\x2E\x3C\xB1\x65\x54\x00\x6A" \
  "\x6A\x5B\x4A\xBB\x16\x6F\x44\xD0\x91\x3F\xD3\xC2\x50\xAC\x1A\x87" \
  "\x86\x41\xEE\x56\xDA\x31\xF2\xFF\x38\x3C\xBB\x32\x3E\x2D\xDB\x98" \
  "\xEA\x05\x9E\x8F\x91\x8E\x0E\x99\xE5\x4F\x32\x13\x92\x7F\x17\x68"

// The public key in uncompressed form, missing the header byte.
#define MALFORMED_PUBKEY_MISSING_HEADER                              \
  "\xD1\x82\x2E\x6A\xD2\x4B\x2A\x80\x2E\x8F\xBC\x03\x00\x95\x11\xF9" \
  "\x81\x24\xA7\x3C\x45\xC8\xBA\xDD\x5F\x77\x1C\xC3\x71\x8B\xB2\xE9" \
  "\x3A\x0A\x84\xFF\xEA\x13\xC2\x27\xD2\xCF\x42\x7D\xA5\x95\xD6\x88" \
  "\xCD\x23\x00\x3F\xF9\xD9\x75\x46\xFF\x58\xE9\xBE\xC3\x74\x13\xB8"

// The public key in compressed form, wrong the header byte (03 instead of 02).
#define MALFORMED_PUBKEY_WRONG_HEADER                                \
  "\x03"                                                             \
  "\xD1\x82\x2E\x6A\xD2\x4B\x2A\x80\x2E\x8F\xBC\x03\x00\x95\x11\xF9" \
  "\x81\x24\xA7\x3C\x45\xC8\xBA\xDD\x5F\x77\x1C\xC3\x71\x8B\xB2\xE9"

// Tampered signature (first bit flipped).
#define TAMPERED_SIGNATURE                                           \
  "\x17\x54\x43\xD4\x00\x07\xC4\xD7\x26\x2E\x3C\xB1\x65\x54\x00\x6A" \
  "\x6A\x5B\x4A\xBB\x16\x6F\x44\xD0\x91\x3F\xD3\xC2\x50\xAC\x1A\x87" \
  "\x86\x41\xEE\x56\xDA\x31\xF2\xFF\x38\x3C\xBB\x32\x3E\x2D\xDB\x98" \
  "\xEA\x05\x9E\x8F\x91\x8E\x0E\x99\xE5\x4F\x32\x13\x92\x7F\x17\x68"

// Short signature (last byte removed).
#define SHORT_SIGNATURE                                              \
  "\x16\x54\x43\xD4\x00\x07\xC4\xD7\x26\x2E\x3C\xB1\x65\x54\x00\x6A" \
  "\x6A\x5B\x4A\xBB\x16\x6F\x44\xD0\x91\x3F\xD3\xC2\x50\xAC\x1A\x87" \
  "\x86\x41\xEE\x56\xDA\x31\xF2\xFF\x38\x3C\xBB\x32\x3E\x2D\xDB\x98" \
  "\xEA\x05\x9E\x8F\x91\x8E\x0E\x99\xE5\x4F\x32\x13\x92\x7F\x17"

// Short digest (last byte removed)
#define SHORT_DIGEST                                                 \
  "\x8D\xCE\x14\xEE\x2C\xD9\xFD\x9B\xBD\x8C\x8D\x57\x68\x50\x2C\x2F" \
  "\xFB\xB3\x52\x36\xCE\x93\x47\x1B\x80\xFC\xA4\x7D\xB5\xF8\x41"

// Tampered digest (first bit flipped)
#define TAMPERED_DIGEST                                              \
  "\x8C\xCE\x14\xEE\x2C\xD9\xFD\x9B\xBD\x8C\x8D\x57\x68\x50\x2C\x2F" \
  "\xFB\xB3\x52\x36\xCE\x93\x47\x1B\x80\xFC\xA4\x7D\xB5\xF8\x41\x9D"

// Tampered public key (last bit flipped).
#define TAMPERED_PUBKEY                                              \
  "\x04"                                                             \
  "\xD1\x82\x2E\x6A\xD2\x4B\x2A\x80\x2E\x8F\xBC\x03\x00\x95\x11\xF9" \
  "\x81\x24\xA7\x3C\x45\xC8\xBA\xDD\x5F\x77\x1C\xC3\x71\x8B\xB2\xE9" \
  "\x3A\x0A\x84\xFF\xEA\x13\xC2\x27\xD2\xCF\x42\x7D\xA5\x95\xD6\x88" \
  "\xCD\x23\x00\x3F\xF9\xD9\x75\x46\xFF\x58\xE9\xBE\xC3\x74\x13\xB9"

TEST(EcdsaP256, ValidSignature) {
  ASSERT_OK(VerifyP256Signature(
      AS_BYTES(TEST_PUBKEY), AS_BYTES(TEST_DIGEST), AS_BYTES(TEST_SIGNATURE)));
}

TEST(EcdsaP256, LongerDigestGetsTruncated) {
  ASSERT_OK(VerifyP256Signature(AS_BYTES(TEST_PUBKEY),
                                AS_BYTES(TEST_DIGEST "extra stuff"),
                                AS_BYTES(TEST_SIGNATURE)));
}

TEST(EcdsaP256, MalformedPublicKeyMissingHeader) {
  ASSERT_EQ(Status::InvalidArgument(),
            VerifyP256Signature(AS_BYTES(MALFORMED_PUBKEY_MISSING_HEADER),
                                AS_BYTES(TEST_DIGEST),
                                AS_BYTES(TEST_SIGNATURE)));
}

TEST(EcdsaP256, MalformedPublicKeyWrongHeader) {
  ASSERT_FAIL(VerifyP256Signature(AS_BYTES(MALFORMED_PUBKEY_WRONG_HEADER),
                                  AS_BYTES(TEST_DIGEST),
                                  AS_BYTES(TEST_SIGNATURE)));
}

TEST(EcdsaP256, TamperedSignature) {
  ASSERT_EQ(Status::Unauthenticated(),
            VerifyP256Signature(AS_BYTES(TEST_PUBKEY),
                                AS_BYTES(TEST_DIGEST),
                                AS_BYTES(TAMPERED_SIGNATURE)));
}

TEST(EcdsaP256, SignatureTooLong) {
  ASSERT_EQ(Status::InvalidArgument(),
            VerifyP256Signature(AS_BYTES(TEST_PUBKEY),
                                AS_BYTES(TEST_DIGEST),
                                AS_BYTES(TEST_SIGNATURE "extra stuff")));
}

TEST(EcdsaP256, SignatureTooShort) {
  ASSERT_EQ(Status::InvalidArgument(),
            VerifyP256Signature(AS_BYTES(TEST_PUBKEY),
                                AS_BYTES(TEST_DIGEST),
                                AS_BYTES(SHORT_SIGNATURE)));
}

TEST(EcdsaP256, DigestTooShort) {
  ASSERT_EQ(Status::InvalidArgument(),
            VerifyP256Signature(AS_BYTES(TEST_PUBKEY),
                                AS_BYTES(SHORT_DIGEST),
                                AS_BYTES(TEST_SIGNATURE)));
}

TEST(EcdsaP256, TamperedDigest) {
  ASSERT_EQ(Status::Unauthenticated(),
            VerifyP256Signature(AS_BYTES(TEST_PUBKEY),
                                AS_BYTES(TAMPERED_DIGEST),
                                AS_BYTES(TEST_SIGNATURE)));
}

TEST(EcdsaP256, TamperedPubkey) {
  ASSERT_FAIL(VerifyP256Signature(AS_BYTES(TAMPERED_PUBKEY),
                                  AS_BYTES(TEST_DIGEST),
                                  AS_BYTES(TEST_SIGNATURE)));
}

}  // namespace
}  // namespace pw::crypto::ecdsa
