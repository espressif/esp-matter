// Copyright 2019 The Pigweed Authors
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
//
// Small, general preprocessor macros for C and C++ code.
#pragma once

// Returns the number of elements in a C array.
#define PW_ARRAY_SIZE(array) (sizeof(array) / sizeof(*array))

// Returns a string literal of the arguments after expanding macros.
#define PW_STRINGIFY(...) _PW_STRINGIFY(__VA_ARGS__)
#define _PW_STRINGIFY(...) #__VA_ARGS__

#ifdef __cplusplus

// Macro for inline extern "C" declarations. The following will compile
// correctly for C and C++:
//
//   PW_EXTERN_C ThisFunctionHasCLinkage(void);
//
#define PW_EXTERN_C extern "C"

// Macros for opening and closing an extern "C" block. This avoids the need for
// an #ifdef __cplusplus check around the extern "C" { and closing }. Example:
//
//   PW_EXTERN_C_START
//
//   void FunctionDeclarationForCppAndC(void);
//
//   void AnotherFunctionDeclarationForCppAndC(int, char);
//
//   PW_EXTERN_C_END
//
#define PW_EXTERN_C_START extern "C" {
#define PW_EXTERN_C_END }  // extern "C"

#else  // extern "C" is removed from C code

#define PW_EXTERN_C
#define PW_EXTERN_C_START
#define PW_EXTERN_C_END

#endif  // __cplusplus
