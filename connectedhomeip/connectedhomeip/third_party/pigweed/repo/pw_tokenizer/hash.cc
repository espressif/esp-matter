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

#include "pw_tokenizer/hash.h"

#if PW_CXX_STANDARD_IS_SUPPORTED(17)

namespace pw::tokenizer {

extern "C" uint32_t pw_tokenizer_65599FixedLengthHash(const char* string,
                                                      size_t string_length,
                                                      size_t hash_length) {
  return PwTokenizer65599FixedLengthHash(
      std::string_view(string, string_length), hash_length);
}

}  // namespace pw::tokenizer

#endif  // PW_CXX_STANDARD_IS_SUPPORTED(17)
