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

#include "pw_tokenizer/token_database.h"

namespace pw::tokenizer {

TokenDatabase::Entry TokenDatabase::Entries::operator[](size_t index) const {
  Iterator it = begin();
  for (size_t i = 0; i < index; ++i) {
    ++it;
  }
  return it.entry();
}

TokenDatabase::Entries TokenDatabase::Find(const uint32_t token) const {
  Iterator first = begin();
  while (first != end() && token > first->token) {
    ++first;
  }

  Iterator last = first;
  while (last != end() && token == last->token) {
    ++last;
  }

  return Entries(first, last);
}

}  // namespace pw::tokenizer
