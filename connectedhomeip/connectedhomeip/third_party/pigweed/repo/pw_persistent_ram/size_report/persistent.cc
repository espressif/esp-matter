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

#include "pw_persistent_ram/persistent.h"

#include "pw_bloat/bloat_this_binary.h"

int main() {
  pw::bloat::BloatThisBinary();

  // Default constructor.
  pw::persistent_ram::Persistent<uint32_t> persistent;

  // Emplace to construct value in place.
  persistent.emplace(42u);

  // Assignment operator.
  persistent = 13u;

  // Invalidate.
  persistent.Invalidate();

  // Has value and value accesstors.
  if (persistent.has_value() && persistent.value() == 0u) {
    return 1;
  }

  return 0;
}
