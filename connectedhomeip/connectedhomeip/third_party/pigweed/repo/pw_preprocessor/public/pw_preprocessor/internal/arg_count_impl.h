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

// Expands to the passed arguments.
#define _PW_EXPAND(...) __VA_ARGS__

// If-like macro for internal use.
#define _PW_IF(boolean, true_expr, false_expr) \
  _PW_PASTE2(_PW_IF_, boolean)(true_expr, false_expr)

#define _PW_IF_0(true_expr, false_expr) false_expr
#define _PW_IF_1(true_expr, false_expr) true_expr

// Token pasting macro that doesn't rely on concat.h
#define _PW_PASTE2(a1, a2) _PW_PASTE2_EXPANDED(a1, a2)
#define _PW_PASTE2_EXPANDED(a1, a2) _PW_PASTE2_IMPL(a1, a2)
#define _PW_PASTE2_IMPL(a1, a2) a1##a2

/*
for i in range(2, 33):
  args = ', '.join(f'a{arg}' for arg in range(1, i))
  print(f'#define _PW_LAST_ARG_{i}({args}, a{i}) a{i}')
*/
// clang-format off
#define _PW_LAST_ARG_0()
#define _PW_LAST_ARG_1(a1) a1
#define _PW_LAST_ARG_2(a1, a2) a2
#define _PW_LAST_ARG_3(a1, a2, a3) a3
#define _PW_LAST_ARG_4(a1, a2, a3, a4) a4
#define _PW_LAST_ARG_5(a1, a2, a3, a4, a5) a5
#define _PW_LAST_ARG_6(a1, a2, a3, a4, a5, a6) a6
#define _PW_LAST_ARG_7(a1, a2, a3, a4, a5, a6, a7) a7
#define _PW_LAST_ARG_8(a1, a2, a3, a4, a5, a6, a7, a8) a8
#define _PW_LAST_ARG_9(a1, a2, a3, a4, a5, a6, a7, a8, a9) a9
#define _PW_LAST_ARG_10(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10) a10
#define _PW_LAST_ARG_11(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11) a11
#define _PW_LAST_ARG_12(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12) a12
#define _PW_LAST_ARG_13(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13) a13
#define _PW_LAST_ARG_14(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14) a14
#define _PW_LAST_ARG_15(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15) a15
#define _PW_LAST_ARG_16(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16) a16
#define _PW_LAST_ARG_17(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17) a17
#define _PW_LAST_ARG_18(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18) a18
#define _PW_LAST_ARG_19(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19) a19
#define _PW_LAST_ARG_20(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20) a20
#define _PW_LAST_ARG_21(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21) a21
#define _PW_LAST_ARG_22(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22) a22
#define _PW_LAST_ARG_23(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23) a23
#define _PW_LAST_ARG_24(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24) a24
#define _PW_LAST_ARG_25(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25) a25
#define _PW_LAST_ARG_26(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26) a26
#define _PW_LAST_ARG_27(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27) a27
#define _PW_LAST_ARG_28(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28) a28
#define _PW_LAST_ARG_29(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29) a29
#define _PW_LAST_ARG_30(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30) a30
#define _PW_LAST_ARG_31(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31) a31
#define _PW_LAST_ARG_32(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32) a32

/*
for i in range(2, 33):
  args = ', '.join(f'a{arg}' for arg in range(1, i))
  print(f'#define _PW_DROP_LAST_ARG_{i}({args}, a{i}) {args}')
*/
#define _PW_DROP_LAST_ARG_0()
#define _PW_DROP_LAST_ARG_1(a1)
#define _PW_DROP_LAST_ARG_2(a1, a2) a1
#define _PW_DROP_LAST_ARG_3(a1, a2, a3) a1, a2
#define _PW_DROP_LAST_ARG_4(a1, a2, a3, a4) a1, a2, a3
#define _PW_DROP_LAST_ARG_5(a1, a2, a3, a4, a5) a1, a2, a3, a4
#define _PW_DROP_LAST_ARG_6(a1, a2, a3, a4, a5, a6) a1, a2, a3, a4, a5
#define _PW_DROP_LAST_ARG_7(a1, a2, a3, a4, a5, a6, a7) a1, a2, a3, a4, a5, a6
#define _PW_DROP_LAST_ARG_8(a1, a2, a3, a4, a5, a6, a7, a8) a1, a2, a3, a4, a5, a6, a7
#define _PW_DROP_LAST_ARG_9(a1, a2, a3, a4, a5, a6, a7, a8, a9) a1, a2, a3, a4, a5, a6, a7, a8
#define _PW_DROP_LAST_ARG_10(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10) a1, a2, a3, a4, a5, a6, a7, a8, a9
#define _PW_DROP_LAST_ARG_11(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11) a1, a2, a3, a4, a5, a6, a7, a8, a9, a10
#define _PW_DROP_LAST_ARG_12(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12) a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11
#define _PW_DROP_LAST_ARG_13(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13) a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12
#define _PW_DROP_LAST_ARG_14(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14) a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13
#define _PW_DROP_LAST_ARG_15(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15) a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14
#define _PW_DROP_LAST_ARG_16(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16) a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15
#define _PW_DROP_LAST_ARG_17(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17) a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16
#define _PW_DROP_LAST_ARG_18(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18) a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17
#define _PW_DROP_LAST_ARG_19(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19) a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18
#define _PW_DROP_LAST_ARG_20(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20) a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19
#define _PW_DROP_LAST_ARG_21(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21) a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20
#define _PW_DROP_LAST_ARG_22(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22) a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21
#define _PW_DROP_LAST_ARG_23(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23) a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22
#define _PW_DROP_LAST_ARG_24(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24) a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23
#define _PW_DROP_LAST_ARG_25(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25) a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24
#define _PW_DROP_LAST_ARG_26(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26) a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25
#define _PW_DROP_LAST_ARG_27(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27) a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26
#define _PW_DROP_LAST_ARG_28(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28) a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27
#define _PW_DROP_LAST_ARG_29(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29) a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28
#define _PW_DROP_LAST_ARG_30(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30) a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29
#define _PW_DROP_LAST_ARG_31(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31) a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30
#define _PW_DROP_LAST_ARG_32(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32) a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31
// clang-format on
