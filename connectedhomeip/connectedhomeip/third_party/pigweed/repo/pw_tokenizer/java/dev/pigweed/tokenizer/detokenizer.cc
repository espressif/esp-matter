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

// This file provides a Java Native Interface (JNI) version of the Detokenizer
// class. This facilitates using the tokenizer library from Java or other JVM
// languages. A corresponding Java class is provided in Detokenizer.java.

#include <jni.h>

#include <cstring>

#include "pw_preprocessor/concat.h"
#include "pw_span/span.h"
#include "pw_tokenizer/detokenize.h"
#include "pw_tokenizer/token_database.h"

#define DETOKENIZER_METHOD(method) \
  JNICALL PW_CONCAT(Java_dev_pigweed_tokenizer_, Detokenizer_, method)

namespace pw::tokenizer {
namespace {

Detokenizer* HandleToPointer(jlong handle) {
  Detokenizer* detokenizer = nullptr;
  std::memcpy(&detokenizer, &handle, sizeof(detokenizer));
  static_assert(sizeof(detokenizer) <= sizeof(handle));
  return detokenizer;
}

jlong PointerToHandle(Detokenizer* detokenizer) {
  jlong handle = 0;
  std::memcpy(&handle, &detokenizer, sizeof(detokenizer));
  static_assert(sizeof(handle) >= sizeof(detokenizer));
  return handle;
}

}  // namespace

extern "C" {

static_assert(sizeof(jbyte) == 1u);

JNIEXPORT jlong DETOKENIZER_METHOD(newNativeDetokenizer)(JNIEnv* env,
                                                         jclass,
                                                         jbyteArray array) {
  jbyte* const data = env->GetByteArrayElements(array, nullptr);
  const jsize size = env->GetArrayLength(array);

  TokenDatabase tokens = TokenDatabase::Create(span(data, size));
  const jlong handle =
      PointerToHandle(new Detokenizer(tokens.ok() ? tokens : TokenDatabase()));

  env->ReleaseByteArrayElements(array, data, 0);
  return handle;
}

JNIEXPORT void DETOKENIZER_METHOD(deleteNativeDetokenizer)(JNIEnv*,
                                                           jclass,
                                                           jlong handle) {
  delete HandleToPointer(handle);
}

JNIEXPORT jstring DETOKENIZER_METHOD(detokenizeNative)(JNIEnv* env,
                                                       jobject,
                                                       jlong handle,
                                                       jbyteArray array) {
  jbyte* const data = env->GetByteArrayElements(array, nullptr);
  const jsize size = env->GetArrayLength(array);

  DetokenizedString result = HandleToPointer(handle)->Detokenize(data, size);

  env->ReleaseByteArrayElements(array, data, 0);

  return result.matches().empty()
             ? nullptr
             : env->NewStringUTF(result.BestString().c_str());
}

}  // extern "C"

}  // namespace pw::tokenizer
