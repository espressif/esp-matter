/***************************************************************************//**
 * @file
 * @brief Common - Preprocessor Utils
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                                   MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _PREPROCESSOR_PRIV_H_
#define  _PREPROCESSOR_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <common/include/toolchains.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                                 DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                             BASE CONSTRUCTS
 *******************************************************************************************************/

#if defined(PP_C_STD_VERSION_C99_PRESENT)
//                                                                 Expands to nothing.
#define  PP_EMPTY(...)
#endif

//                                                                 Basic two-fold concatenation.
#define  PP_CONCAT_IMPL(x, y)               x##y
#define  PP_CONCAT(x, y)                   PP_CONCAT_IMPL(x, y)
#define  PP_CONCAT_UNDERSCORE_IMPL(x, y)   x##_##y
#define  PP_CONCAT_UNDERSCORE(x, y)        PP_CONCAT_UNDERSCORE_IMPL(x, y)

//                                                                 Stringify.
#define  PP_STRINGIFY_IMPL(x)    #x
#define  PP_STRINGIFY(x)         PP_STRINGIFY_IMPL(x)

#if defined(PP_C_STD_VERSION_C99_PRESENT)
//                                                                 Expand to the number of args in __VA_ARGS__.
#define  _PP_NARGS(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, N, ...) N
#define  PP_NARGS(...) _PP_NARGS(__VA_ARGS__, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#endif

#define  COMPILE_TIME_ASSERT(pred)    switch (0) { case 0: case pred:; }

#if defined(PP_C_STD_VERSION_C99_PRESENT)
#define  PP_SCAN(...)      __VA_ARGS__
#endif

#define  PP_PAREN_REM(arg)   PP_SCAN arg

#define  PP_FIRST_PAREN_REM(arg)    PP_EMPTY arg

/********************************************************************************************************
 *                                       PP_IS_ON() / PP_IS_OFF()
 *
 * Description : Apply a macro to the supplied argument list.
 *
 * Argument(s) : macro             Macro to be applied.
 *
 *               __VA_ARGS__       Set of argument lists to apply the given macro to.
 *
 * Expansion   : Depends on the 'macro' parameter.
 *
 * Note(s)     : none.
 *******************************************************************************************************/

#define PP_OFF             ~, 0, 1
#define PP_ON              ~, 1

#if defined(PP_C_STD_VERSION_C99_PRESENT)
#define PP_IS_ON(...)      PP_1D_LIST_ELEM_GET_AT((__VA_ARGS__, 0), 2)
#define PP_IS_OFF(...)     PP_1D_LIST_ELEM_GET_AT((__VA_ARGS__, 0, 0), 3)
#endif

#if defined(PP_C_STD_VERSION_C99_PRESENT)
//                                                                 Eval: enables multiple preprocessor scans.
#define PP_EVAL(...)          PP_EVAL16(__VA_ARGS__)
#define PP_EVAL4096(...)      PP_EVAL2048(PP_EVAL2048(__VA_ARGS__))
#define PP_EVAL2048(...)      PP_EVAL1024(PP_EVAL1024(__VA_ARGS__))
#define PP_EVAL1024(...)      PP_EVAL512(PP_EVAL512(__VA_ARGS__))
#define PP_EVAL512(...)       PP_EVAL256(PP_EVAL256(__VA_ARGS__))
#define PP_EVAL256(...)       PP_EVAL128(PP_EVAL128(__VA_ARGS__))
#define PP_EVAL128(...)       PP_EVAL64(PP_EVAL64(__VA_ARGS__))
#define PP_EVAL64(...)        PP_EVAL32(PP_EVAL32(__VA_ARGS__))
#define PP_EVAL32(...)        PP_EVAL16(PP_EVAL16(__VA_ARGS__))
#define PP_EVAL16(...)        PP_EVAL8(PP_EVAL8(__VA_ARGS__))
#define PP_EVAL8(...)         PP_EVAL4(PP_EVAL4(__VA_ARGS__))
#define PP_EVAL4(...)         PP_EVAL2(PP_EVAL2(__VA_ARGS__))
#define PP_EVAL2(...)         PP_EVAL1(PP_EVAL1(__VA_ARGS__))
#define PP_EVAL1(...)         __VA_ARGS__
#endif

//                                                                 Defers macro expansion.
#define PP_DEFER_1(x) x PP_EMPTY()
#define PP_DEFER_2(x) x PP_EMPTY PP_EMPTY()()
#define PP_DEFER_3(x) x PP_EMPTY PP_EMPTY PP_EMPTY()()()

#define  PP_MAKE_LABEL(prefix, suffix)   PP_CONCAT(PP_CONCAT(_, prefix), PP_CONCAT(_, suffix))
#define  PP_UNIQUE_LABEL(prefix)         PP_MAKE_LABEL(prefix, __LINE__)

/********************************************************************************************************
 *                                               PP_APPLY_TO()
 *
 * Description : Apply a macro to the supplied argument list.
 *
 * Argument(s) : macro             Macro to be applied.
 *
 *               __VA_ARGS__       Set of argument lists to apply the given macro to.
 *
 * Expansion   : Depends on the 'macro' parameter.
 *
 * Note(s)     : none.
 *******************************************************************************************************/

#if defined(PP_C_STD_VERSION_C99_PRESENT)
#define  PP_APPLY_TO(macro, ...)                            macro(__VA_ARGS__)
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                              ARITHMETIC
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                               PP_INC()
 *
 * Description : Incrementation.
 *
 * Argument(s) : op                Operand.
 *
 * Expansion   : The numerical value that is equal to op + 1.
 *
 * Note(s)     : none.
 *******************************************************************************************************/

#define  PP_INC(x)              PP_CONCAT(_PP_INC_, x)
#define  _PP_INC_0              1
#define  _PP_INC_1              2
#define  _PP_INC_2              3
#define  _PP_INC_3              4
#define  _PP_INC_4              5
#define  _PP_INC_5              6
#define  _PP_INC_6              7
#define  _PP_INC_7              8
#define  _PP_INC_8              9
#define  _PP_INC_9              10

/********************************************************************************************************
 *                                               PP_DEC()
 *
 * Description : Decrementation.
 *
 * Argument(s) : op                Operand.
 *
 * Expansion   : The numerical value that is equal to op - 1.
 *
 * Note(s)     : none.
 *******************************************************************************************************/

#define  PP_DEC(op)             PP_CONCAT(_PP_DEC_, op)
#define  _PP_DEC_1              0
#define  _PP_DEC_2              1
#define  _PP_DEC_3              2
#define  _PP_DEC_4              3
#define  _PP_DEC_5              4
#define  _PP_DEC_6              5
#define  _PP_DEC_7              6
#define  _PP_DEC_8              7
#define  _PP_DEC_9              8

/********************************************************************************************************
 ********************************************************************************************************
 *                                                LOGIC
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                               PP_NOT()
 *
 * Description : Logical NOT.
 *
 * Argument(s) : expr               Expression to be negated (MUST be either '0' or '1').
 *
 * Expansion   : 0, if 'expr' is 0.
 *               1, if 'expr' is 1.
 *
 * Note(s)     : none.
 *******************************************************************************************************/

#define  PP_NOT(expr)      PP_CONCAT(_PP_NOT_, expr)
#define  _PP_NOT_0         1
#define  _PP_NOT_1         0

/********************************************************************************************************
 *                                               PP_OR()
 *
 * Description : Logical OR.
 *
 * Argument(s) : op1               First operand (MUST be either '0' or '1').
 *
 *               op2               Second operand (MUST be either '0' or '1').
 *
 * Expansion   : '0' or '1' depending on the operands.
 *
 * Note(s)     : none.
 *******************************************************************************************************/

#define  PP_OR(x, y)    PP_CONCAT(PP_OR_, PP_CONCAT(x, y))
#define  PP_OR_00       0
#define  PP_OR_01       1
#define  PP_OR_10       1
#define  PP_OR_11       1

/********************************************************************************************************
 *                                               PP_AND()
 *
 * Description : Logical AND.
 *
 * Argument(s) : op1               First operand (MUST be either '0' or '1').
 *
 *               op2               Second operand (MUST be either '0' or '1').
 *
 * Expansion   : '0' or '1' depending on the operands.
 *
 * Note(s)     : none.
 *******************************************************************************************************/

#define  PP_AND(x, y)           PP_CONCAT(PP_AND_, PP_CONCAT(x, y))
#define  PP_AND_00              0
#define  PP_AND_01              0
#define  PP_AND_10              0
#define  PP_AND_11              1

/********************************************************************************************************
 *                                               PP_BOOL()
 *
 * Description : Cast to boolean.
 *
 * Argument(s) : expr               Expression to casted.
 *
 * Expansion   : 0, if 'expr' is 0.
 *               1, otherwise.
 *
 * Note(s)     : none.
 *******************************************************************************************************/

#define PP_BOOL(expr)                 PP_NOT(PP_IS_ON(PP_CONCAT(_PP_BOOL_, expr)))
#define _PP_BOOL_0                    PP_ON

/********************************************************************************************************
 ********************************************************************************************************
 *                                            CONTROL FLOW
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                               PP_IF()
 *
 * Description : IF control flow construct.
 *
 * Argument(s) : cond      Expression to be tested (MUST be either '0' or '1').
 *
 * Expansion   : What comes next (between parentheses) if 'expr' is '1'.
 *               Nothing if 'expr' is '0'.
 *
 *               For instance,
 *
 *                   PP_IF(1)(foo)
 *
 *               expands to 'foo', while
 *
 *                   PP_IF(0)(foo)
 *
 *               expands to nothing.
 *
 * Note(s)     : none.
 *******************************************************************************************************/

#define  PP_IF(cond)                 PP_IF_ELSE(PP_NOT(cond))()

/********************************************************************************************************
 *                                               PP_IF_ELSE()
 *
 * Description : IF-ELSE control flow construct.
 *
 * Argument(s) : cond               Expression that expands to either '0' or '1'.
 *
 * Expansion   : What comes next (between parentheses) if 'expr' is '1'.
 *               What comes after the next parentheses (also between parentheses) if 'expr' is '0'.
 *
 *               For instance,
 *
 *                   PP_IF_ELSE(1)(foo)(bar)
 *
 *               expands to 'foo', while
 *
 *                   PP_IF_ELSE(0)(foo)(bar)
 *
 *               expands to 'bar'.
 *
 * Note(s)     : none.
 *******************************************************************************************************/

#if defined(PP_C_STD_VERSION_C99_PRESENT)
#define  PP_IF_ELSE(cond)            PP_CONCAT(_PP_IF_, cond)

#define  _PP_IF_1(...)               __VA_ARGS__ _PP_IF_1_ELSE
#define  _PP_IF_0(...)               _PP_IF_0_ELSE

#define  _PP_IF_1_ELSE(...)
#define  _PP_IF_0_ELSE(...)          __VA_ARGS__
#endif

/********************************************************************************************************
 *                                       PP_1D_LIST_ELEM_GET_AT()
 *
 * Description : Extract element at supplied index inside a given list.
 *
 * Argument(s) : list            List to get element from.
 *
 *               elem_ix         Element index.
 *
 * Expansion   : elem_ix'th argument in the list.
 *
 * Note(s)     : (1) The empty argument is needed to avoid warning/error in the case where __VA_ARGS__
 *                   contains 'elem_ix' elements.
 *******************************************************************************************************/

#if defined(PP_C_STD_VERSION_C99_PRESENT)
#define  PP_1D_LIST_ELEM_GET_AT(list, elem_ix)    PP_APPLY_TO(_PP_1D_LIST_ELEM_GET_AT, elem_ix, PP_SCAN list)
#define  _PP_1D_LIST_ELEM_GET_AT(elem_ix, ...)    PP_CONCAT(_PP_1D_LIST_ELEM_GET_AT_, elem_ix) (__VA_ARGS__, /* empty arg (see Note #1) */)

#define _PP_1D_LIST_ELEM_GET_AT_1(a1, ...)                                                                 a1
#define _PP_1D_LIST_ELEM_GET_AT_2(a1, a2, ...)                                                             a2
#define _PP_1D_LIST_ELEM_GET_AT_3(a1, a2, a3, ...)                                                         a3
#define _PP_1D_LIST_ELEM_GET_AT_4(a1, a2, a3, a4, ...)                                                     a4
#define _PP_1D_LIST_ELEM_GET_AT_5(a1, a2, a3, a4, a5, ...)                                                 a5
#define _PP_1D_LIST_ELEM_GET_AT_6(a1, a2, a3, a4, a5, a6, ...)                                             a6
#define _PP_1D_LIST_ELEM_GET_AT_7(a1, a2, a3, a4, a5, a6, a7, ...)                                         a7
#define _PP_1D_LIST_ELEM_GET_AT_8(a1, a2, a3, a4, a5, a6, a7, a8, ...)                                     a8
#define _PP_1D_LIST_ELEM_GET_AT_9(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, ...)                            a10
#define _PP_1D_LIST_ELEM_GET_AT_10(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, ...)                      a11
#define _PP_1D_LIST_ELEM_GET_AT_11(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, ...)                 a12
#define _PP_1D_LIST_ELEM_GET_AT_12(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, ...)            a13
#define _PP_1D_LIST_ELEM_GET_AT_13(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, ...)       a14
#define _PP_1D_LIST_ELEM_GET_AT_14(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, ...)  a15
#endif

/********************************************************************************************************
 *                                           PP_IS_DECIMAL_DIGIT()
 *
 * Description : Test if the given expression is a decimal digit.
 *
 * Argument(s) : expr            Expression to be tested.
 *
 * Expansion   : '1', if 'expr' is a decimal digit.
 *               '0', if 'expr' is not a decimal digit.
 *
 * Note(s)     : none.
 *******************************************************************************************************/

#define  PP_IS_DECIMAL_DIGIT(expr)        PP_IS_ON(PP_CONCAT(PP_IS_DECIMAL_DIGIT_, expr))

#define  PP_IS_DECIMAL_DIGIT_0            PP_ON
#define  PP_IS_DECIMAL_DIGIT_1            PP_ON
#define  PP_IS_DECIMAL_DIGIT_2            PP_ON
#define  PP_IS_DECIMAL_DIGIT_3            PP_ON
#define  PP_IS_DECIMAL_DIGIT_4            PP_ON
#define  PP_IS_DECIMAL_DIGIT_5            PP_ON
#define  PP_IS_DECIMAL_DIGIT_6            PP_ON
#define  PP_IS_DECIMAL_DIGIT_7            PP_ON
#define  PP_IS_DECIMAL_DIGIT_8            PP_ON
#define  PP_IS_DECIMAL_DIGIT_9            PP_ON

/********************************************************************************************************
 *                                           PP_ARE_EQUAL()
 *
 * Description : Check if two decimal digit are equal.
 *
 * Argument(s) : op1     Operand 1.
 *
 *               op2     Operand 2.
 *
 * Expansion   : '1', if both digits are equal.
 *               '0', if both digits are not equal or at least one of the operands is not a decimal digit.
 *
 * Note(s)     : none.
 *******************************************************************************************************/

#define  PP_ARE_EQUAL(x, y)         PP_IS_ON(PP_CONCAT(PP_CONCAT(PP_ARE_EQUAL_, x), y))
#define  PP_ARE_EQUAL_00            PP_ON
#define  PP_ARE_EQUAL_11            PP_ON
#define  PP_ARE_EQUAL_22            PP_ON
#define  PP_ARE_EQUAL_33            PP_ON
#define  PP_ARE_EQUAL_44            PP_ON
#define  PP_ARE_EQUAL_55            PP_ON
#define  PP_ARE_EQUAL_66            PP_ON
#define  PP_ARE_EQUAL_77            PP_ON
#define  PP_ARE_EQUAL_88            PP_ON
#define  PP_ARE_EQUAL_99            PP_ON

/********************************************************************************************************
 *                                               PP_HAS_ARGS()
 *
 * Description : Test whether the given argument list is empty or not.
 *
 * Argument(s) : __VA_ARGS__      Argument list to be tested for emptiness.
 *
 * Expansion   : 0, if the argument list is empty,
 *               1, otherwise.
 *
 * Note(s)     : none.
 *******************************************************************************************************/

#if defined(PP_C_STD_VERSION_C99_PRESENT)
#define PP_HAS_ARGS(...)               PP_BOOL(PP_1D_LIST_ELEM_GET_AT((_PP_HAS_ARGS __VA_ARGS__), 1)(NO_ARG_PRESENT))

#define _PP_HAS_ARGS(arg)              PP_CONCAT(_PP_HAS_ARGS_, arg)

#define _PP_HAS_ARGS_NO_ARG_PRESENT    0
#endif

/********************************************************************************************************
 *                                               PP_FOR_EACH()
 *
 * Description : Apply a given macro on a variable number of arguments.
 *
 * Argument(s) : __VA_ARGS__      Argument list to apply the given macro to.
 *
 * Expansion   : Depends on the macro to be applied.
 *
 * Note(s)     : none.
 *******************************************************************************************************/

#if defined(PP_C_STD_VERSION_C99_PRESENT)
#define  PP_FOR_EACH(macro, ...)              PP_EVAL(_PP_FOR_EACH(macro, __VA_ARGS__))

#define  PP_FOR_EACH_N(macro, iter_cnt, ...)   PP_CONCAT(PP_EVAL, iter_cnt)(_PP_FOR_EACH(macro, __VA_ARGS__))

#define _PP_FOR_EACH(macro, cur_arg, ...)    macro(cur_arg)     \
  PP_IF(PP_HAS_ARGS(__VA_ARGS__)) (                             \
    PP_DEFER_2(__PP_FOR_EACH)()(macro, __VA_ARGS__, PP_EMPTY()) \
    )

#define __PP_FOR_EACH()                      _PP_FOR_EACH
#endif

/********************************************************************************************************
 *                                       PP_FOR_EACH_WITH_CTXT()
 *
 * Description : Successively apply a macro to each argument in the given argument list.
 *
 * Argument(s) : iter_macro        Expands once for each argument in the argument list, based on the current
 *                                   argument and context. The macro signature is iter_macro(cur_arg, ctxt).
 *
 *               ctxt_set_macro    Modifies the current context after each iteration if needed, taking into
 *                                   account the current argument. The macro signature is
 *                                   ctxt_set_macro(cur_arg, ctxt).
 *
 *               term_macro        Performs one final expansion based on the last argument and the final
 *                                   context. The macro signature is term_macro(last_arg, ctxt).
 *
 *               init_ctxt         Initial context.
 *
 *               __VA_ARGS__       Set of argument lists to apply the given macro to.
 *
 * Expansion   : Depends on 'iter_macro', 'ctxt_set_macro' and 'term_macro' parameters.
 *
 * Note(s)     : (1) The next argument (i.e. the one after 'cur_arg' in the parameter list) is tested for
 *                   emptiness at the begining of each iteration. If the test is positive, there is no
 *                   argument left to process: the termination macro is expanded and the recursion
 *                   ends.
 *******************************************************************************************************/

#if defined(PP_C_STD_VERSION_C99_PRESENT)
#define  PP_FOR_EACH_WITH_CTXT(iter_macro, ctxt_set_macro, term_macro, init_ctxt, ...) \
  PP_EVAL(_PP_FOR_EACH_WITH_CTXT_ITER(iter_macro,                                      \
                                      ctxt_set_macro,                                  \
                                      term_macro,                                      \
                                      init_ctxt,                                       \
                                      __VA_ARGS__,                                     \
                                      /* empty arg (see Note #1) */))

#define  _PP_FOR_EACH_WITH_CTXT_ITER(iter_macro, ctxt_set_macro, term_macro, cur_ctxt, cur_arg, ...) \
  PP_IF_ELSE(PP_HAS_ARGS(__VA_ARGS__)) (                                                             \
    iter_macro(cur_arg, cur_ctxt)                                                                    \
    PP_DEFER_2(__PP_FOR_EACH_WITH_CTXT_ITER)()(iter_macro,                                           \
                                               ctxt_set_macro,                                       \
                                               term_macro,                                           \
                                               ctxt_set_macro(cur_arg, cur_ctxt),                    \
                                               __VA_ARGS__,                                          \
                                               /* empty arg (see Note #1) */)                        \
    )(                                                                                               \
    term_macro(cur_arg, cur_ctxt)                                                                    \
    )

#define  __PP_FOR_EACH_WITH_CTXT_ITER()                    _PP_FOR_EACH_WITH_CTXT_ITER
#endif

/********************************************************************************************************
 *                                           PP_LIST_ELEM_GET_AT()
 *
 * Description : Get an element inside an (possibly multidimensional) list, where a list is written as
 *               comma separated elements between parentheses and where an element may be a list itself
 *               (hence multidimensional).
 *
 *               For instance,
 *
 *                   PP_LIST_ELEM_GET_AT((a, b, (c, (d, e, f))), (3, 2))
 *
 *               expands to (d, e, f) while
 *
 *                   PP_LIST_ELEM_GET_AT((a, b, (c, (d, e, f))), (3, 2, 3))
 *
 *               expands to f.
 *
 * Argument(s) : list             List to get the element from.
 *
 *               coord            Comma separated indexes between parentheses.
 *
 * Expansion   : Element extracted from the list.
 *
 * Note(s)     : none.
 *******************************************************************************************************/

#define  PP_LIST_ELEM_GET_AT(list, coord)                     PP_FOR_EACH_WITH_CTXT(PP_EMPTY,                      \
                                                                                    _PP_LIST_ELEM_GET_AT_CTXT_SET, \
                                                                                    _PP_LIST_ELEM_GET_AT_TERM,     \
                                                                                    list,                          \
                                                                                    PP_SCAN coord)

#define  _PP_LIST_ELEM_GET_AT_TERM(cur_arg, cur_ctxt)         PP_1D_LIST_ELEM_GET_AT((PP_SCAN cur_ctxt), cur_arg)

#define  _PP_LIST_ELEM_GET_AT_CTXT_SET(cur_arg, cur_ctxt)     PP_1D_LIST_ELEM_GET_AT((PP_SCAN cur_ctxt), cur_arg)

/********************************************************************************************************
 *                                           PP_CONCAT_MULT()
 *
 * Description : Concatenante all arguments.
 *
 * Argument(s) : __VA_ARGS__     Tokens to be concatenated.
 *
 * Expansion   : The result of the concatenation. For instance, PP_CONCAT_MULT(foo, bar, baz) expands to
 *               foobarbaz.
 *
 * Note(s)     : none.
 *******************************************************************************************************/

#if defined(PP_C_STD_VERSION_C99_PRESENT)
#define  PP_CONCAT_MULT(...)                                PP_FOR_EACH_WITH_CTXT(PP_EMPTY,           \
                                                                                  PP_CONCAT_CTXT_SET, \
                                                                                  PP_CONCAT_TERM,     \
                                                                                  /* empty arg. */,   \
                                                                                  __VA_ARGS__)

#define  PP_CONCAT_CTXT_SET(cur_arg, cur_ctxt)              PP_CONCAT(cur_ctxt, cur_arg)
#define  PP_CONCAT_TERM(cur_arg, cur_ctxt)                  PP_CONCAT(cur_ctxt, cur_arg)
#endif

/********************************************************************************************************
 *                                       PP_CONCAT_UNDERSCORE_MULT()
 *
 * Description : Concatenante all arguments with underscrores between each token.
 *
 * Argument(s) : __VA_ARGS__     Tokens to be concatenated.
 *
 * Expansion   : The result of the concatenation. For instance, PP_CONCAT_MULT(foo, bar, baz) expands to
 *               foo_bar_baz.
 *
 * Note(s)     : none.
 *******************************************************************************************************/

#if defined(PP_C_STD_VERSION_C99_PRESENT)
#define  PP_CONCAT_UNDERSCORE_MULT(...)                     PP_FOR_EACH_WITH_CTXT(PP_EMPTY,                      \
                                                                                  PP_CONCAT_UNDERSCORE_CTXT_SET, \
                                                                                  PP_CONCAT_UNDERSCORE_TERM,     \
                                                                                  ,                              \
                                                                                  __VA_ARGS__)

#define  PP_CONCAT_UNDERSCORE_CTXT_SET(cur_arg, cur_ctxt)   PP_IF_ELSE(PP_HAS_ARGS(cur_ctxt))(PP_CONCAT_UNDERSCORE(cur_ctxt, cur_arg))(cur_arg)
#define  PP_CONCAT_UNDERSCORE_TERM(cur_arg, cur_ctxt)       PP_IF_ELSE(PP_HAS_ARGS(cur_ctxt))(PP_CONCAT_UNDERSCORE(cur_ctxt, cur_arg))(cur_arg)
#endif

/********************************************************************************************************
 *                                           PP_LIST_BUILD()
 *
 * Description : Create a list (i.e. comma separated arguments) from given arguments.
 *
 * Argument(s) : __VA_ARGS__     Elements of the list.
 *
 * Expansion   : The result of the concatenation. For instance, PP_CONCAT_MULT(foo, bar, baz) expands to
 *               foo, bar, baz.
 *
 * Note(s)     : none.
 *******************************************************************************************************/

#if defined(PP_C_STD_VERSION_C99_PRESENT)
#define  PP_LIST_BUILD(...)                                 PP_FOR_EACH_WITH_CTXT(_PP_LIST_BUILD_ITER, \
                                                                                  PP_EMPTY,            \
                                                                                  _PP_LIST_BUILD_TERM, \
                                                                                  ,                    \
                                                                                  __VA_ARGS__)
#define  _PP_LIST_BUILD_ITER(cur_arg, cur_ctxt)             cur_arg,
#define  _PP_LIST_BUILD_TERM(cur_arg, cur_ctxt)             cur_arg
#endif

/********************************************************************************************************
 *                                               PP_IS_LT()
 *
 * Description : Is less than.
 *
 * Argument(s) : op1         Operand 1.
 *
 *               op2         Operand 2.
 *
 * Expansion   : '1', if op1 < op2.
 *               '0', otherwise.
 *
 * Note(s)     : none.
 *******************************************************************************************************/

#define  PP_IS_LT(op1, op2)      PP_CONCAT(PP_IS_LT_, op2)(op1)
#define  PP_IS_LT_0(op1)         0
#define  PP_IS_LT_1(op1)         PP_OR(PP_IS_LT_0(op1), PP_ARE_EQUAL(op1, 0))
#define  PP_IS_LT_2(op1)         PP_OR(PP_IS_LT_1(op1), PP_ARE_EQUAL(op1, 1))
#define  PP_IS_LT_3(op1)         PP_OR(PP_IS_LT_2(op1), PP_ARE_EQUAL(op1, 2))
#define  PP_IS_LT_4(op1)         PP_OR(PP_IS_LT_3(op1), PP_ARE_EQUAL(op1, 3))
#define  PP_IS_LT_5(op1)         PP_OR(PP_IS_LT_4(op1), PP_ARE_EQUAL(op1, 4))
#define  PP_IS_LT_6(op1)         PP_OR(PP_IS_LT_5(op1), PP_ARE_EQUAL(op1, 5))
#define  PP_IS_LT_7(op1)         PP_OR(PP_IS_LT_6(op1), PP_ARE_EQUAL(op1, 6))
#define  PP_IS_LT_8(op1)         PP_OR(PP_IS_LT_7(op1), PP_ARE_EQUAL(op1, 7))
#define  PP_IS_LT_9(op1)         PP_OR(PP_IS_LT_8(op1), PP_ARE_EQUAL(op1, 8))

/********************************************************************************************************
 *                                               PP_IS_LTE()
 *
 * Description : Is less than or equal to.
 *
 * Argument(s) : op1         Operand 1.
 *
 *               op2         Operand 2.
 *
 * Expansion   : '1', if op1 <= op2.
 *               '0', otherwise.
 *
 * Note(s)     : none.
 *******************************************************************************************************/

#define  PP_IS_LTE(op1, op2)      PP_OR(PP_IS_LT(op1, op2), PP_ARE_EQUAL(op1, op2))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // End of private preprocessor module include.
