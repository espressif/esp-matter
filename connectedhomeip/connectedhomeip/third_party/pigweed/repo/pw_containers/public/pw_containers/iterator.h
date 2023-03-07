// Copyright 2022 The Pigweed Authors
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
#pragma once

#include <iterator>

#include "pw_polyfill/standard.h"

namespace pw::containers {

#if PW_CXX_STANDARD_IS_SUPPORTED(20)

// If std::contiguous_iterator_tag exists, use it directly.
using std::contiguous_iterator_tag;

#else

// If std::contiguous_iterator_tag does not exist, define a stand-in type.
struct contiguous_iterator_tag : public std::random_access_iterator_tag {};

#endif  // PW_CXX_STANDARD_IS_SUPPORTED(20)

}  // namespace pw::containers
