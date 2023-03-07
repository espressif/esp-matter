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

// PW_STRING_ENABLE_DECIMAL_FLOAT_EXPANSION controls whether floating point
// values passed to the ToString function will be expanded after a decimal
// point, or just rounded to the nearest int. Enabling decimal expansion may
// significantly increase code size.
//
// Note: This currently relies on floating point support for `snprintf`, which
// might require extra compiler configuration, e.g. `-u_printf_float` for
// newlib-nano.
#ifndef PW_STRING_ENABLE_DECIMAL_FLOAT_EXPANSION
#define PW_STRING_ENABLE_DECIMAL_FLOAT_EXPANSION 0
#endif

namespace pw::string::internal::config {

constexpr bool kEnableDecimalFloatExpansion =
    PW_STRING_ENABLE_DECIMAL_FLOAT_EXPANSION;

}

#undef PW_STRING_ENABLE_DECIMAL_FLOAT_EXPANSION
