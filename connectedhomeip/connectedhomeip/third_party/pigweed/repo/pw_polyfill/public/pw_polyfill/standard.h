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

#ifdef __cplusplus
#define PW_CXX_STANDARD_IS_SUPPORTED(std) \
  (__cplusplus >= _PW_CXX_STANDARD_##std())
#else
#define PW_CXX_STANDARD_IS_SUPPORTED(std) (0 >= _PW_CXX_STANDARD_##std())
#endif

#define _PW_CXX_STANDARD_98() 199711L
#define _PW_CXX_STANDARD_11() 201103L
#define _PW_CXX_STANDARD_14() 201402L
#define _PW_CXX_STANDARD_17() 201703L
#define _PW_CXX_STANDARD_20() 202002L
