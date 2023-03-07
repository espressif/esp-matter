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

#pragma once

#include "pw_preprocessor/arguments.h"

// Expands macros and concatenates the results using preprocessor ##
// concatentation. Supports up to 32 arguments.
#define PW_CONCAT(...) \
  _PW_CONCAT_IMPL1(PW_MACRO_ARG_COUNT(__VA_ARGS__), __VA_ARGS__)

// Expand the macro to allow PW_MACRO_ARG_COUNT and any caller-provided macros
// to be evaluated before concatenating the tokens.
#define _PW_CONCAT_IMPL1(count, ...) _PW_CONCAT_IMPL2(count, __VA_ARGS__)
#define _PW_CONCAT_IMPL2(count, ...) _PW_CONCAT_##count(__VA_ARGS__)

// clang-format off
/* This macro implementation was generated with the following Python 3 code:
for i in range(32 + 1):
  args = [f'a{x}' for x in range(1, i + 1)]
  print(f'#define _PW_CONCAT_{i}({", ".join(args)}) {"##".join(args)}  // NOLINT')
*/

#define _PW_CONCAT_0()   // NOLINT
#define _PW_CONCAT_1(a1) a1  // NOLINT
#define _PW_CONCAT_2(a1, a2) a1##a2  // NOLINT
#define _PW_CONCAT_3(a1, a2, a3) a1##a2##a3  // NOLINT
#define _PW_CONCAT_4(a1, a2, a3, a4) a1##a2##a3##a4  // NOLINT
#define _PW_CONCAT_5(a1, a2, a3, a4, a5) a1##a2##a3##a4##a5  // NOLINT
#define _PW_CONCAT_6(a1, a2, a3, a4, a5, a6) a1##a2##a3##a4##a5##a6  // NOLINT
#define _PW_CONCAT_7(a1, a2, a3, a4, a5, a6, a7) a1##a2##a3##a4##a5##a6##a7  // NOLINT
#define _PW_CONCAT_8(a1, a2, a3, a4, a5, a6, a7, a8) a1##a2##a3##a4##a5##a6##a7##a8  // NOLINT
#define _PW_CONCAT_9(a1, a2, a3, a4, a5, a6, a7, a8, a9) a1##a2##a3##a4##a5##a6##a7##a8##a9  // NOLINT
#define _PW_CONCAT_10(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10) a1##a2##a3##a4##a5##a6##a7##a8##a9##a10  // NOLINT
#define _PW_CONCAT_11(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11) a1##a2##a3##a4##a5##a6##a7##a8##a9##a10##a11  // NOLINT
#define _PW_CONCAT_12(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12) a1##a2##a3##a4##a5##a6##a7##a8##a9##a10##a11##a12  // NOLINT
#define _PW_CONCAT_13(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13) a1##a2##a3##a4##a5##a6##a7##a8##a9##a10##a11##a12##a13  // NOLINT
#define _PW_CONCAT_14(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14) a1##a2##a3##a4##a5##a6##a7##a8##a9##a10##a11##a12##a13##a14  // NOLINT
#define _PW_CONCAT_15(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15) a1##a2##a3##a4##a5##a6##a7##a8##a9##a10##a11##a12##a13##a14##a15  // NOLINT
#define _PW_CONCAT_16(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16) a1##a2##a3##a4##a5##a6##a7##a8##a9##a10##a11##a12##a13##a14##a15##a16  // NOLINT
#define _PW_CONCAT_17(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17) a1##a2##a3##a4##a5##a6##a7##a8##a9##a10##a11##a12##a13##a14##a15##a16##a17  // NOLINT
#define _PW_CONCAT_18(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18) a1##a2##a3##a4##a5##a6##a7##a8##a9##a10##a11##a12##a13##a14##a15##a16##a17##a18  // NOLINT
#define _PW_CONCAT_19(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19) a1##a2##a3##a4##a5##a6##a7##a8##a9##a10##a11##a12##a13##a14##a15##a16##a17##a18##a19  // NOLINT
#define _PW_CONCAT_20(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20) a1##a2##a3##a4##a5##a6##a7##a8##a9##a10##a11##a12##a13##a14##a15##a16##a17##a18##a19##a20  // NOLINT
#define _PW_CONCAT_21(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21) a1##a2##a3##a4##a5##a6##a7##a8##a9##a10##a11##a12##a13##a14##a15##a16##a17##a18##a19##a20##a21  // NOLINT
#define _PW_CONCAT_22(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22) a1##a2##a3##a4##a5##a6##a7##a8##a9##a10##a11##a12##a13##a14##a15##a16##a17##a18##a19##a20##a21##a22  // NOLINT
#define _PW_CONCAT_23(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23) a1##a2##a3##a4##a5##a6##a7##a8##a9##a10##a11##a12##a13##a14##a15##a16##a17##a18##a19##a20##a21##a22##a23  // NOLINT
#define _PW_CONCAT_24(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24) a1##a2##a3##a4##a5##a6##a7##a8##a9##a10##a11##a12##a13##a14##a15##a16##a17##a18##a19##a20##a21##a22##a23##a24  // NOLINT
#define _PW_CONCAT_25(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25) a1##a2##a3##a4##a5##a6##a7##a8##a9##a10##a11##a12##a13##a14##a15##a16##a17##a18##a19##a20##a21##a22##a23##a24##a25  // NOLINT
#define _PW_CONCAT_26(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26) a1##a2##a3##a4##a5##a6##a7##a8##a9##a10##a11##a12##a13##a14##a15##a16##a17##a18##a19##a20##a21##a22##a23##a24##a25##a26  // NOLINT
#define _PW_CONCAT_27(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27) a1##a2##a3##a4##a5##a6##a7##a8##a9##a10##a11##a12##a13##a14##a15##a16##a17##a18##a19##a20##a21##a22##a23##a24##a25##a26##a27  // NOLINT
#define _PW_CONCAT_28(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28) a1##a2##a3##a4##a5##a6##a7##a8##a9##a10##a11##a12##a13##a14##a15##a16##a17##a18##a19##a20##a21##a22##a23##a24##a25##a26##a27##a28  // NOLINT
#define _PW_CONCAT_29(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29) a1##a2##a3##a4##a5##a6##a7##a8##a9##a10##a11##a12##a13##a14##a15##a16##a17##a18##a19##a20##a21##a22##a23##a24##a25##a26##a27##a28##a29  // NOLINT
#define _PW_CONCAT_30(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30) a1##a2##a3##a4##a5##a6##a7##a8##a9##a10##a11##a12##a13##a14##a15##a16##a17##a18##a19##a20##a21##a22##a23##a24##a25##a26##a27##a28##a29##a30  // NOLINT
#define _PW_CONCAT_31(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31) a1##a2##a3##a4##a5##a6##a7##a8##a9##a10##a11##a12##a13##a14##a15##a16##a17##a18##a19##a20##a21##a22##a23##a24##a25##a26##a27##a28##a29##a30##a31  // NOLINT
#define _PW_CONCAT_32(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32) a1##a2##a3##a4##a5##a6##a7##a8##a9##a10##a11##a12##a13##a14##a15##a16##a17##a18##a19##a20##a21##a22##a23##a24##a25##a26##a27##a28##a29##a30##a31##a32  // NOLINT
