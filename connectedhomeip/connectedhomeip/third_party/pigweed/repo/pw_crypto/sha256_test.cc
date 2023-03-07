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

#include "pw_crypto/sha256.h"

#include <cstring>

#include "gtest/gtest.h"
#include "pw_stream/memory_stream.h"

namespace pw::crypto::sha256 {
namespace {

#define ASSERT_OK(expr) ASSERT_EQ(OkStatus(), expr)
#define ASSERT_FAIL(expr) ASSERT_NE(OkStatus(), expr)

#define AS_BYTES(s) as_bytes(span(s, sizeof(s) - 1))

// Generated in Python 3 with:
// `hashlib.sha256('Hello, Pigweed!'.encode('ascii')).hexdigest()`.
#define SHA256_HASH_OF_HELLO_PIGWEED                                 \
  "\x8d\xce\x14\xee\x2c\xd9\xfd\x9b\xbd\x8c\x8d\x57\x68\x50\x2c\x2f" \
  "\xfb\xb3\x52\x36\xce\x93\x47\x1b\x80\xfc\xa4\x7d\xb5\xf8\x41\x9d"

// Generated in Python with `hashlib.sha256().hexdigest()`.
#define SHA256_HASH_OF_EMPTY_STRING                                  \
  "\xe3\xb0\xc4\x42\x98\xfc\x1c\x14\x9a\xfb\xf4\xc8\x99\x6f\xb9\x24" \
  "\x27\xae\x41\xe4\x64\x9b\x93\x4c\xa4\x95\x99\x1b\x78\x52\xb8\x55"

TEST(Hash, ComputesCorrectDigest) {
  std::byte digest[kDigestSizeBytes];

  ASSERT_OK(Hash(AS_BYTES("Hello, Pigweed!"), digest));
  ASSERT_EQ(0,
            std::memcmp(digest, SHA256_HASH_OF_HELLO_PIGWEED, sizeof(digest)));
}

TEST(Hash, ComputesCorrectDigestFromReader) {
  std::byte digest[kDigestSizeBytes];
  ConstByteSpan message = AS_BYTES("Hello, Pigweed!");

  stream::MemoryReader reader(message);
  ASSERT_OK(Hash(reader, digest));
  ASSERT_EQ(0,
            std::memcmp(digest, SHA256_HASH_OF_HELLO_PIGWEED, sizeof(digest)));
}

TEST(Hash, ComputesCorrectDigestOnEmptyMessage) {
  std::byte digest[kDigestSizeBytes];

  ASSERT_OK(Hash({}, digest));
  ASSERT_EQ(0,
            std::memcmp(digest, SHA256_HASH_OF_EMPTY_STRING, sizeof(digest)));
}

TEST(Hash, ComputesCorrectDigestOnEmptyMessageFromReader) {
  std::byte digest[kDigestSizeBytes];

  ConstByteSpan empty;
  stream::MemoryReader reader(empty);
  ASSERT_OK(Hash(reader, digest));
  ASSERT_EQ(0,
            std::memcmp(digest, SHA256_HASH_OF_EMPTY_STRING, sizeof(digest)));
}

TEST(Hash, DigestBufferTooSmall) {
  std::array<std::byte, 31> digest = {};
  ASSERT_FAIL(Hash({}, digest));
}

TEST(Hash, DigestBufferTooSmallForReaderBasedAPI) {
  std::array<std::byte, 31> digest = {};
  ConstByteSpan empty;
  stream::MemoryReader reader(empty);
  ASSERT_FAIL(Hash(reader, digest));
}

TEST(Hash, AcceptsLargerDigestBuffer) {
  std::array<std::byte, 33> digest = {};
  ASSERT_OK(Hash({}, digest));
}

TEST(Hash, AcceptsLargerDigestBufferForReaderBasedAPI) {
  std::array<std::byte, 33> digest = {};

  ConstByteSpan empty;
  stream::MemoryReader reader(empty);
  ASSERT_OK(Hash(reader, digest));
}

TEST(Sha256, AllowsSkippedUpdate) {
  std::byte digest[kDigestSizeBytes];

  ASSERT_OK(Sha256().Final(digest));
  ASSERT_EQ(0,
            std::memcmp(digest, SHA256_HASH_OF_EMPTY_STRING, sizeof(digest)));
}

TEST(Sha256, AllowsEmptyUpdate) {
  std::byte digest[kDigestSizeBytes];
  ASSERT_OK(Sha256().Update({}).Final(digest));
  ASSERT_EQ(0,
            std::memcmp(digest, SHA256_HASH_OF_EMPTY_STRING, sizeof(digest)));
}

TEST(Sha256, AllowsMultipleUpdates) {
  std::byte digest[kDigestSizeBytes];
  ASSERT_OK(Sha256()
                .Update(AS_BYTES("Hello, "))
                .Update(AS_BYTES("Pigweed!"))
                .Final(digest));
  ASSERT_EQ(0,
            std::memcmp(digest, SHA256_HASH_OF_HELLO_PIGWEED, sizeof(digest)));
}

TEST(Sha256, NoFinalAfterFinal) {
  std::byte digest[kDigestSizeBytes];
  auto h = Sha256();

  ASSERT_OK(h.Final(digest));
  ASSERT_FAIL(h.Final(digest));
}

TEST(Sha256, NoUpdateAfterFinal) {
  std::byte digest[kDigestSizeBytes];
  auto h = Sha256();

  ASSERT_OK(h.Final(digest));
  ASSERT_FAIL(h.Update(AS_BYTES("blah")).Final(digest));
}

}  // namespace
}  // namespace pw::crypto::sha256
