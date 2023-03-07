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

// PW_SPAN_ENABLE_ASSERTS controls whether pw_span's implementation includes
// asserts for detecting disallowed span operations at runtime. For C++20 and
// later, this replaces std::span with the custom implementation in pw_span to
// ensure bounds-checking asserts have been enabled.
//
// This defaults to disabled because of the significant increase in code size
// caused by enabling this feature. It's strongly recommended to enable this
// in debug and testing builds.
#if !defined(PW_SPAN_ENABLE_ASSERTS)
#define PW_SPAN_ENABLE_ASSERTS 0
#endif  // !defined(PW_SPAN_ENABLE_ASSERTS)
