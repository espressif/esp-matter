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
#pragma once

// libc++ uses a special namespace for standard library headers. Use this
// namespace via the defines in <__config>.
#if defined(_LIBCPP_VERSION) && __has_include(<__config>)

#include <__config>

#define _PW_POLYFILL_BEGIN_NAMESPACE_STD _LIBCPP_BEGIN_NAMESPACE_STD
#define _PW_POLYFILL_END_NAMESPACE_STD _LIBCPP_END_NAMESPACE_STD

#else  // Directly use the std namespace in GCC.

#define _PW_POLYFILL_BEGIN_NAMESPACE_STD namespace std {
#define _PW_POLYFILL_END_NAMESPACE_STD }  // namespace std

// Cannot compile when using libc++ without the <__config> header.
#ifdef _LIBCPP_VERSION
static_assert(
    false,
    "Compiling against libc++, but the <__config> header is not available. "
    "The <__config> header provides various _LIBCPP defines used internally "
    "by libc++. pw_polyfill needs this header for the "
    "_LIBCPP_BEGIN_NAMESPACE_STD and _LIBCPP_END_NAMESPACE_STD macros, which "
    "specify the namespace to use for the standard library. "
    ""
    "If you see this message, you may be compiling with Clang, but without "
    "libc++, in which case a fake <__config> header should be provided. "
    "Alternately, libc++ may have been updated and no longer provides "
    "<__config>, in which this file should be updated to properly "
    "set _PW_POLYFILL_BEGIN_NAMESPACE_STD and _PW_POLYFILL_END_NAMESPACE_STD.");

#endif  // _LIBCPP_VERSION

#endif  // defined(_LIBCPP_VERSION) && __has_include(<__config>)
