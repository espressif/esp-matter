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

// Preprocessor boolean operation macros that evaluate to 0 or 1.
//
// These macros perform boolean operations in the C preprocessor that evaluate
// to a literal 1 or 0. They can be used for a few purposes:
//
//   - Generate other macros that evaluate to a 1 or 0, instead of a
//     parenthesized boolean expression.
//   - Ensure that the operands are defined and evaluate to 1 or 0 themselves.
//   - Write macros that conditionally use other macros by token pasting the
//     resulting 1 or 0 to form a new macro name.
//
// These macros should not be used outside of macro definitions. Use normal C
// operators (&&, ||, !, ==, !=) instead. For example, to check whether two
// flags are set, the C operators are the best choice:
//
//   #if RELEASE && OPTIMIZED
//
// However, there are cases when a literal 0 or 1 is required. For example:
//
//   #define SELECT_ALGORITHM() PW_CONCAT(ALGO_, PW_AND(RELEASE, OPTIMIZED))
//
// SELECT_ALGORITHM evaluates to ALGO_0 or ALGO_1, depending on whether RELEASE
// and OPTIMIZED are set to 1.

// Boolean AND of two preprocessor expressions that evaluate to 0 or 1.
#define PW_AND(a, b) _PW_AND(a, b)      // Expand the macro an extra time to
#define _PW_AND(a, b) _PW_AND_##a##b()  // allow macro substitution to occur.
#define _PW_AND_00() 0
#define _PW_AND_01() 0
#define _PW_AND_10() 0
#define _PW_AND_11() 1

// Boolean OR of two preprocessor expressions that evaluate to 0 or 1.
#define PW_OR(a, b) _PW_OR(a, b)
#define _PW_OR(a, b) _PW_OR_##a##b()
#define _PW_OR_00() 0
#define _PW_OR_01() 1
#define _PW_OR_10() 1
#define _PW_OR_11() 1

// Boolean NOT of a preprocessor expression that evaluates to 0 or 1.
#define PW_NOT(value) _PW_NOT(value)
#define _PW_NOT(value) _PW_NOT_##value()
#define _PW_NOT_0() 1
#define _PW_NOT_1() 0

// Boolean XOR of two preprocessor expressions that evaluate to 0 or 1.
#define PW_XOR(a, b) _PW_XOR(a, b)
#define _PW_XOR(a, b) _PW_XOR_##a##b()
#define _PW_XOR_00() 0
#define _PW_XOR_01() 1
#define _PW_XOR_10() 1
#define _PW_XOR_11() 0

// Boolean NAND, NOR, and XNOR of expressions that evaluate to 0 or 1.
#define PW_NAND(a, b) PW_NOT(PW_AND(a, b))
#define PW_NOR(a, b) PW_NOT(PW_OR(a, b))
#define PW_XNOR(a, b) PW_NOT(PW_XOR(a, b))
