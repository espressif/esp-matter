/***************************************************************************//**
 * @file
 * @brief Common - Logging
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

#ifndef  _LOGGING_PRIV_H_
#define  _LOGGING_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <em_core.h>

#include  <cpu/include/cpu.h>

#include  <common/include/lib_str.h>
#include  <common/source/lib/lib_str_priv.h>
#include  <common/source/preprocessor/preprocessor_priv.h>
#include  <common/source/ring_buf/ring_buf_priv.h>
#include  <common/include/logging.h>
#include  <rtos_cfg.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef struct logging_output_func_cb_data {
  int (* ByteOutCb)(int c);
} LOGGING_OUTPUT_FUNC_CB_DATA;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (RTOS_CFG_LOG_EN == DEF_ENABLED)
extern RING_BUF Log_RingBuf;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (RTOS_CFG_LOG_EN == DEF_ENABLED)
void Log_Init(COMMON_CFG_LOGGING *p_cfg,
              RTOS_ERR           *p_err);
#endif

int Logging_OutputFunc(int  c,
                       void *p_arg);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOGGING MACRO'S
 ********************************************************************************************************
 *******************************************************************************************************/

#if ((RTOS_CFG_LOG_EN == DEF_ENABLED) \
  && !defined(PP_C_STD_VERSION_C99_PRESENT))                    // Variadic macros supported only in C99.
#error "The logging module requires C99 to work correctly since it uses variadic macros and this feature has been added in C99."
#endif

//                                                                 ------------- DEFAULT LOGGING CHANNEL --------------
#if ((RTOS_CFG_LOG_EN == DEF_ENABLED) \
  && (!defined(RTOS_CFG_LOG_ALL)))
#include  <stdio.h>
#define  RTOS_CFG_LOG_ALL                                   VRB, SYNC, FUNC_DIS, TS_DIS, putchar
#endif

/*
 ********************************************************************************************************
 *                                           INTERFACE MACRO'S
 *******************************************************************************************************/

/********************************************************************************************************
 *                                   LOG_ERR() / LOG_DBG() / LOG_VRB()
 *
 * Description : Convenience wrappers to write to the default channel, LOG_DFLT_CH.
 *
 *               LOG_DFLT_CH is intended to be defined on a per module basis and must be defined in the
 *               context where LOG_ERR()/LOG_DBG()/LOG_VRB() occur.
 *
 * Argument(s) : List of print variables in the following format:
 *
 *                                       [(<specifier>)]expr
 *
 *                                   where the specifier may be chosen from
 *
 *                                           (u)       unsigned int, decimal,
 *                                           (x)       unsigned int, lower case hex,
 *                                           (X)       unsigned int, upper case hex,
 *                                           (d)       signed int, decimal,
 *                                           (p)       address (contained in expr), toolchain specific format,
 *                                           (s)       string (pointed by expr),
 *                                           <blank>   string literal.
 *
 *                                   Usage example:
 *
 *                                       LOG_ERR("Volume ", (s)p_vol_name, ":", (u)vol_id, " has been closed.\r\n");
 *
 * Note(s)     : (1) The output mode (sync or async) may be selected on a per channel basis using the dedicated
 *                   configuration syntax in rtos_cfg.h
 *
 *               (2) Each print variable argument must be preceded by a compatible specifier inside parentheses,
 *                   except for the string literals. If no specifier is given, string literal type is assumed.
 *******************************************************************************************************/

#define  LOG_ERR(args)                              LOG_ERR_TO(LOG_DFLT_CH, args)
#define  LOG_DBG(args)                              LOG_DBG_TO(LOG_DFLT_CH, args)
#define  LOG_VRB(args)                              LOG_VRB_TO(LOG_DFLT_CH, args)

/********************************************************************************************************
 *                           LOG_ERR_IS_EN() / LOG_DBG_IS_EN() / LOG_VRB_IS_EN()
 *
 * Description : Convenience wrappers (with default channel) around the corresponding LOG_ERR_TO_IS_EN(),
 *               LOG_DBG_TO_IS_EN(), LOG_VRB_TO_IS_EN() macro's.
 *
 * Argument(s) : none.
 *
 * Note(s)     : none.
 *******************************************************************************************************/

#define  LOG_ERR_IS_EN()                       LOG_TO_IS_EN(LOG_DFLT_CH, ERR)
#define  LOG_DBG_IS_EN()                       LOG_TO_IS_EN(LOG_DFLT_CH, DBG)
#define  LOG_VRB_IS_EN()                       LOG_TO_IS_EN(LOG_DFLT_CH, VRB)

/********************************************************************************************************
 *                               LOG_TO_ERR() / LOG_TO_DBG() / LOG_TO_VRB()
 *
 * Description : Convenience wrappers (with implied log level) around the LOG_TO_IS_EN() macro.
 *
 * Argument(s) : ch               Output channel to write message to.
 *
 *               __VA_ARGS__      List of print variables (see LOG_ERR()/LOG_DBG()/LOG_VRB()).
 *
 * Note(s)     : See LOG_ERR()/LOG_DBG()/LOG_VRB().
 *******************************************************************************************************/

#define  LOG_ERR_TO(ch, args)                       LOG_TO(ch, ERR, args)
#define  LOG_DBG_TO(ch, args)                       LOG_TO(ch, DBG, args)
#define  LOG_VRB_TO(ch, args)                       LOG_TO(ch, VRB, args)

/********************************************************************************************************
 *                       LOG_TO_ERR_IS_EN() / LOG_TO_DBG_IS_EN() / LOG_TO_VRB_IS_EN()
 *
 * Description : Convenience wrappers (with implied log level) around the LOG_TO_IS_EN() macro.
 *
 * Argument(s) : ch               Output channel to write message to.
 *
 * Note(s)     : none.
 *******************************************************************************************************/

#define  LOG_ERR_TO_IS_EN(ch)                       LOG_TO_IS_EN(ch, ERR)
#define  LOG_DBG_TO_IS_EN(ch)                       LOG_TO_IS_EN(ch, DBG)
#define  LOG_VRB_TO_IS_EN(ch)                       LOG_TO_IS_EN(ch, VRB)

/********************************************************************************************************
 *                                               LOG_TO()
 *
 * Description : Write message through an associated output function.
 *
 * Argument(s) : ch               Output channel to write message to.
 *
 *               lvl              Log level associated with the message.
 *
 *               __VA_ARGS__      List of print variables (see LOG_ERR()/LOG_DBG()/LOG_VRB()).
 *
 * Note(s)     : See LOG_ERR()/LOG_DBG()/LOG_VRB().
 *******************************************************************************************************/

#if (RTOS_CFG_LOG_EN == DEF_ENABLED \
     && defined(PP_C_STD_VERSION_C99_PRESENT))

#define  LOG_TO(ch, lvl, args)                      PP_IF(LOG_TO_IS_EN(ch, lvl)) (                      \
    PP_IF_ELSE(PP_ARE_EQUAL(_LOG_CH_CFG_EFFECTIVE_OUTPUT_MODE_GET ch, RTOS_CFG_LOG_OUTPUT_MODE_SYNC)) ( \
      LOG_SYNC(_LOG_CH_CFG_EFFECTIVE_OUTPUT_FUNC_GET(PP_SCAN ch), LOG_ARG_FMT(ch, args))                \
      )(                                                                                                \
      LOG_ASYNC(_LOG_CH_CFG_EFFECTIVE_OUTPUT_FUNC_GET(PP_SCAN ch), LOG_ARG_FMT(ch, args))               \
      )                                                                                                 \
    )

#define  LOG_TO_IS_EN(ch, lvl)                      PP_IS_LTE(PP_CONCAT(RTOS_CFG_LOG_LVL_, lvl), _LOG_CH_CFG_EFFECTIVE_LVL_GET ch)

#if (CPU_CFG_TS_64_EN == DEF_ENABLED)
#define  LOG_TS_GET()                               CPU_TS_Get64()
#elif (CPU_CFG_TS_32_EN == DEF_ENABLED)
#define  LOG_TS_GET()                               CPU_TS_Get32()
#else
#define  LOG_TS_GET()                               0
#endif

#define  LOG_ARG_FMT(ch, args)                      (PP_IF(PP_ARE_EQUAL(_LOG_CH_CFG_EFFECTIVE_TS_GET ch, RTOS_CFG_LOG_TS_TS_EN))(                 \
                                                       "[", (u)LOG_TS_GET(), "]",                                                                 \
                                                       )                                                                                          \
                                                     PP_IF(PP_ARE_EQUAL(_LOG_CH_CFG_EFFECTIVE_FUNC_NAME_GET ch, RTOS_CFG_LOG_FUNC_NAME_FUNC_EN))( \
                                                       (s) & __func__[0], "(): ",                                                                 \
                                                       )                                                                                          \
                                                     PP_SCAN args,                                                                                \
                                                     "\r\n")
#else
#define  LOG_TO(ch, lvl, args)

#define  LOG_TO_IS_EN(ch, lvl)                      DEF_NO
#endif

/********************************************************************************************************
 *                                       LOG_SYNC() / LOG_ASYNC()
 *
 * Description : Synchronously/asynchronously write message through an associated output function. Expands
 *               to an output function call preceded by a compile time argument checking.
 *
 * Argument(s) : __VA_ARGS__       List of print variables (see LOG_ERR()/LOG_DBG()/LOG_VRB()).
 *
 * Note(s)     : See LOG_ERR()/LOG_DBG()/LOG_VRB().
 *******************************************************************************************************/

#if (RTOS_CFG_LOG_EN == DEF_ENABLED \
     && defined(PP_C_STD_VERSION_C99_PRESENT))
#define  LOG_SYNC(output_func, arg)                PP_FOR_EACH(_ARG_SPEC_CHK, PP_SCAN arg) \
  _LOG_SYNC_IMPL(output_func, PP_SCAN arg)

#define  LOG_ASYNC(output_func, arg)               PP_FOR_EACH(_ARG_SPEC_CHK, PP_SCAN arg) \
  _LOG_ASYNC_IMPL(output_func, PP_SCAN arg)
#else
#define  LOG_SYNC(output_func, arg)

#define  LOG_ASYNC(output_func, arg)
#endif

#if (RTOS_CFG_LOG_EN == DEF_ENABLED \
     && defined(PP_C_STD_VERSION_C99_PRESENT))

/********************************************************************************************************
 *                                       LOGGING ARGUMENTS DEFINES
 *******************************************************************************************************/

typedef struct printf_async_fmt_data {
  const CPU_CHAR     *FmtStr;
  const CPU_INT08U   *ArgLenTbl;
  int (*OutputFunc)(int c, void *p_arg);
  const void * const OutputFuncArgPtr;
} LOG_ASYNC_FMT_DATA;

#define _LOG_ARG_SPEC_ID_p      0
#define _LOG_ARG_SPEC_ID_s      1
#define _LOG_ARG_SPEC_ID_u      2
#define _LOG_ARG_SPEC_ID_x      3
#define _LOG_ARG_SPEC_ID_X      4
#define _LOG_ARG_SPEC_ID_d      5
#define _LOG_ARG_SPEC_ID_c      6

#define _LOG_ARG_SPEC_IS_PTR(spec)             PP_ARE_EQUAL(PP_CONCAT(_LOG_ARG_SPEC_ID_, spec), _LOG_ARG_SPEC_ID_p)
#define _LOG_ARG_SPEC_IS_STR(spec)             PP_ARE_EQUAL(PP_CONCAT(_LOG_ARG_SPEC_ID_, spec), _LOG_ARG_SPEC_ID_s)
#define _LOG_ARG_SPEC_IS_INT(spec)             PP_ARE_EQUAL(PP_CONCAT(_LOG_ARG_SPEC_ID_, spec), _LOG_ARG_SPEC_ID_d)
#define _LOG_ARG_SPEC_IS_UINT(spec)            PP_ARE_EQUAL(PP_CONCAT(_LOG_ARG_SPEC_ID_, spec), _LOG_ARG_SPEC_ID_u)
#define _LOG_ARG_SPEC_IS_LOWERCASE_HEX(spec)   PP_ARE_EQUAL(PP_CONCAT(_LOG_ARG_SPEC_ID_, spec), _LOG_ARG_SPEC_ID_x)
#define _LOG_ARG_SPEC_IS_UPPERCASE_HEX(spec)   PP_ARE_EQUAL(PP_CONCAT(_LOG_ARG_SPEC_ID_, spec), _LOG_ARG_SPEC_ID_X)
#define _LOG_ARG_SPEC_IS_CHAR(spec)            PP_ARE_EQUAL(PP_CONCAT(_LOG_ARG_SPEC_ID_, spec), _LOG_ARG_SPEC_ID_c)

#define _LOG_ARG_SPEC_IS_VALID(spec)           PP_OR(_LOG_ARG_SPEC_IS_PTR(spec),                                         \
                                                     PP_OR(_LOG_ARG_SPEC_IS_STR(spec),                                   \
                                                           PP_OR(_LOG_ARG_SPEC_IS_INT(spec),                             \
                                                                 PP_OR(_LOG_ARG_SPEC_IS_UINT(spec),                      \
                                                                       PP_OR(_LOG_ARG_SPEC_IS_LOWERCASE_HEX(spec),       \
                                                                             PP_OR(_LOG_ARG_SPEC_IS_UPPERCASE_HEX(spec), \
                                                                                   _LOG_ARG_SPEC_IS_CHAR(spec)))))))

#define _LOG_ARG_LOG_TYPE_SPEC_GET(spec, val)          PP_IF(PP_OR(_LOG_ARG_SEPC_IS_PTR(spec), _LOG_ARG_SPEC_IS_LOWERCASE_HEX(spec))(                         \
                                                               "%p"                                                                                           \
                                                               )                                                                                              \
                                                             PP_IF(_LOG_ARG_SPEC_IS_STR(spec))(                                                               \
                                                               "%s"                                                                                           \
                                                               )                                                                                              \
                                                             PP_IF(_LOG_ARG_SPEC_IS_INT(spec))(                                                               \
                                                               (sizeof(val) == 1 ? "%d" : (sizeof(val) == 2) ? "%d" : (sizeof(val) == 4) ? "%ld" : "%lld"     \
                                                               )                                                                                              \
                                                               PP_IF(_LOG_ARG_SPEC_IS_UINT(spec))(                                                            \
                                                                 (sizeof(val) == 1 ? "%u" : (sizeof(val) == 2) ? "%u" : (sizeof(val) == 4) ? "%lu" : "%llu"   \
                                                                 )                                                                                            \
                                                                 PP_IF(_LOG_ARG_SPEC_IS_UPPERCASE_HEX(spec))(                                                 \
                                                                   (sizeof(val) == 1 ? "%X" : (sizeof(val) == 2) ? "%X" : (sizeof(val) == 4) ? "%lX" : "%llX" \
                                                                   )                                                                                          \
                                                                   PP_IF(_LOG_ARG_SPEC_IS_CHAR(spec))(                                                        \
                                                                     "%c"                                                                                     \
                                                                     )

/********************************************************************************************************
 *                                       IMPLEMENTATION MACRO'S
 *******************************************************************************************************/

#define _LOG_HEX_PREFIX(arg)                                         PP_CONCAT(0x, arg)

/********************************************************************************************************
 *                                           _LOG_ARG_HAS_SPEC()
 *
 * Description : Check whether the given printf argument has a type specifier or not.
 *
 * Argument(s) : printf_arg          Log argument to be checked.
 *
 * Expansion   : '1', if the argument has a specifier.
 *               '0', otherwise.
 *
 * Note(s)     : none.
 *******************************************************************************************************/

#define  _LOG_ARG_HAS_SPEC(printf_arg)                                  PP_DEC(PP_NARGS(__LOG_ARG_HAS_SPEC printf_arg))
#define  __LOG_ARG_HAS_SPEC(printf_arg)                                 ~, ~

/********************************************************************************************************
 *                                       _LOG_ARG_SPEC_EXTRACT()
 *
 * Description : Extract the printf argument's type specifier.
 *
 * Argument(s) : printf_arg          Log argument to extract specifier from.
 *
 * Expansion   : The type specifier
 *
 * Note(s)     : none.
 *******************************************************************************************************/

#define  _LOG_ARG_SPEC_EXTRACT(printf_arg)                              PP_1D_LIST_ELEM_GET_AT((__LOG_ARG_SPEC_EXTRACT printf_arg), 1)
#define  __LOG_ARG_SPEC_EXTRACT(printf_arg)                             printf_arg,

/********************************************************************************************************
 *                                       _LOG_ARG_NAME_EXTRACT()
 *
 * Description : Extract the printf argument's name.
 *
 * Argument(s) : printf_arg          printf argument to extract name from.
 *
 * Expansion   : The argument's name.
 *
 * Note(s)     : none.
 *******************************************************************************************************/

#define  _LOG_ARG_NAME_EXTRACT(printf_arg)                              PP_1D_LIST_ELEM_GET_AT((__LOG_ARG_NAME_EXTRACT printf_arg), 2)
#define  __LOG_ARG_NAME_EXTRACT(printf_arg)                             printf_arg,

/********************************************************************************************************
 *                                           _LOG_ARG_SIZE_GET()
 *
 * Description : Get the size of a printf argument based on its type specifier.
 *
 * Argument(s) : printf_arg          Log argument to get the size of.
 *
 * Expansion   : The type specifier
 *
 * Note(s)     : none.
 *******************************************************************************************************/

#define _LOG_ARG_SIZE_GET(printf_arg)                             PP_IF_ELSE(_LOG_ARG_HAS_SPEC(printf_arg))( \
    sizeof(_LOG_ARG_NAME_EXTRACT(printf_arg))                                                                \
    )(                                                                                                       \
    CPU_CFG_ADDR_SIZE                                                                                        \
    )

#define  _ARG_SPEC_CHK(printf_arg)                                PP_IF_ELSE(_LOG_ARG_HAS_SPEC(printf_arg))( \
  {                                                                                                          \
    PP_IF(_LOG_ARG_SPEC_IS_PTR(_LOG_ARG_SPEC_EXTRACT(printf_arg)))(                                          \
      const void *_LOG_ARG_SPEC_CHK_VAR = _LOG_ARG_NAME_EXTRACT(printf_arg);                                 \
      (void)&_LOG_ARG_SPEC_CHK_VAR;                                                                          \
      )                                                                                                      \
    PP_IF(_LOG_ARG_SPEC_IS_STR(_LOG_ARG_SPEC_EXTRACT(printf_arg)))(                                          \
      const CPU_CHAR * _LOG_ARG_SPEC_CHK_VAR = _LOG_ARG_NAME_EXTRACT(printf_arg);                            \
      (void)&_LOG_ARG_SPEC_CHK_VAR;                                                                          \
      )                                                                                                      \
  }                                                                                                          \
    )(                                                                                                       \
  {                                                                                                          \
    const CPU_CHAR *_LOG_ARG_SPEC_CHK_VAR = printf_arg;                                                      \
    (void)&_LOG_ARG_SPEC_CHK_VAR;                                                                            \
  }                                                                                                          \
    )

/********************************************************************************************************
 *                                   SYNCHRONOUS MODE SPECIFIC MACRO'S
 *******************************************************************************************************/

#define _LOG_SYNC_FMT_STR_BUILD(...)                          PP_FOR_EACH(_LOG_SYNC_FMT_STR_BUILD_ITER, __VA_ARGS__)

#define _LOG_SYNC_FMT_STR_BUILD_ITER(printf_arg)                 PP_IF_ELSE(_LOG_ARG_HAS_SPEC(printf_arg))( \
    _LOG_ARG_LOG_TYPE_SPEC_GET(printf_arg)                                                                  \
    )(                                                                                                      \
    printf_arg                                                                                              \
    )

#define _LOG_SYNC_ARG_LIST_BUILD(...)                         PP_FOR_EACH(_LOG_SYNC_ARG_LIST_BUILD_ITER, __VA_ARGS__)

#define _LOG_SYNC_ARG_LIST_BUILD_ITER(printf_arg)                PP_IF(_LOG_ARG_HAS_SPEC(printf_arg))( \
    , (_LOG_ARG_TYPE_CAST_GET(printf_arg))_LOG_ARG_NAME_EXTRACT(printf_arg)                            \
    )

#define _LOG_SYNC_PRINTF(...)                                (void)Str_Printf(Logging_OutputFunc, (void *)&_log_out_data, __VA_ARGS__)

#define _LOG_SYNC_OUTPUT(printf_arg, out_func)               {                                                                                \
  static const LOGGING_OUTPUT_FUNC_CB_DATA _log_out_data = { (int (*)(int))out_func };                                                        \
  PP_IF_ELSE(_LOG_ARG_HAS_SPEC(printf_arg))(                                                                                                  \
    PP_IF(_LOG_ARG_SPEC_IS_PTR(_LOG_ARG_SPEC_EXTRACT(printf_arg)))(                                                                           \
      _LOG_SYNC_PRINTF("%p", _LOG_ARG_NAME_EXTRACT(printf_arg));                                                                              \
      )                                                                                                                                       \
    PP_IF(_LOG_ARG_SPEC_IS_STR(_LOG_ARG_SPEC_EXTRACT(printf_arg)))(                                                                           \
      _LOG_SYNC_PRINTF("%s", _LOG_ARG_NAME_EXTRACT(printf_arg));                                                                              \
      )                                                                                                                                       \
    PP_IF(_LOG_ARG_SPEC_IS_CHAR(_LOG_ARG_SPEC_EXTRACT(printf_arg)))(                                                                          \
      _LOG_SYNC_PRINTF("%c", _LOG_ARG_NAME_EXTRACT(printf_arg));                                                                              \
      )                                                                                                                                       \
    PP_IF(_LOG_ARG_SPEC_IS_UINT(_LOG_ARG_SPEC_EXTRACT(printf_arg)))(                                                                          \
      if ((_LOG_ARG_SIZE_GET(printf_arg)) == 1) { _LOG_SYNC_PRINTF("%u", (unsigned int)_LOG_ARG_NAME_EXTRACT(printf_arg)); }                  \
      else if ((_LOG_ARG_SIZE_GET(printf_arg)) == 2) { _LOG_SYNC_PRINTF("%u", (unsigned int)_LOG_ARG_NAME_EXTRACT(printf_arg)); }             \
      else if ((_LOG_ARG_SIZE_GET(printf_arg)) == 4) { _LOG_SYNC_PRINTF("%lu", (unsigned long int)_LOG_ARG_NAME_EXTRACT(printf_arg)); }       \
      else if ((_LOG_ARG_SIZE_GET(printf_arg)) == 8) { _LOG_SYNC_PRINTF("%llu", (unsigned long long int)_LOG_ARG_NAME_EXTRACT(printf_arg)); } \
      )                                                                                                                                       \
    PP_IF(_LOG_ARG_SPEC_IS_LOWERCASE_HEX(_LOG_ARG_SPEC_EXTRACT(printf_arg)))(                                                                 \
      if ((_LOG_ARG_SIZE_GET(printf_arg)) == 1) { _LOG_SYNC_PRINTF("%x", (unsigned int)_LOG_ARG_NAME_EXTRACT(printf_arg)); }                  \
      else if ((_LOG_ARG_SIZE_GET(printf_arg)) == 2) { _LOG_SYNC_PRINTF("%x", (unsigned int)_LOG_ARG_NAME_EXTRACT(printf_arg)); }             \
      else if ((_LOG_ARG_SIZE_GET(printf_arg)) == 4) { _LOG_SYNC_PRINTF("%lx", (unsigned long int)_LOG_ARG_NAME_EXTRACT(printf_arg)); }       \
      else if ((_LOG_ARG_SIZE_GET(printf_arg)) == 8) { _LOG_SYNC_PRINTF("%llx", (unsigned long long int)_LOG_ARG_NAME_EXTRACT(printf_arg)); } \
      )                                                                                                                                       \
    PP_IF(_LOG_ARG_SPEC_IS_UPPERCASE_HEX(_LOG_ARG_SPEC_EXTRACT(printf_arg)))(                                                                 \
      if ((_LOG_ARG_SIZE_GET(printf_arg)) == 1) { _LOG_SYNC_PRINTF("%X", (unsigned int)_LOG_ARG_NAME_EXTRACT(printf_arg)); }                  \
      else if ((_LOG_ARG_SIZE_GET(printf_arg)) == 2) { _LOG_SYNC_PRINTF("%X", (unsigned int)_LOG_ARG_NAME_EXTRACT(printf_arg)); }             \
      else if ((_LOG_ARG_SIZE_GET(printf_arg)) == 4) { _LOG_SYNC_PRINTF("%lX", (unsigned long int)_LOG_ARG_NAME_EXTRACT(printf_arg)); }       \
      else if ((_LOG_ARG_SIZE_GET(printf_arg)) == 8) { _LOG_SYNC_PRINTF("%llX", (unsigned long long int)_LOG_ARG_NAME_EXTRACT(printf_arg)); } \
      )                                                                                                                                       \
    PP_IF(_LOG_ARG_SPEC_IS_INT(_LOG_ARG_SPEC_EXTRACT(printf_arg)))(                                                                           \
      if ((_LOG_ARG_SIZE_GET(printf_arg)) == 1) { _LOG_SYNC_PRINTF("%d", (int)_LOG_ARG_NAME_EXTRACT(printf_arg)); }                           \
      else if ((_LOG_ARG_SIZE_GET(printf_arg)) == 2) { _LOG_SYNC_PRINTF("%d", (int)_LOG_ARG_NAME_EXTRACT(printf_arg)); }                      \
      else if ((_LOG_ARG_SIZE_GET(printf_arg)) == 4) { _LOG_SYNC_PRINTF("%ld", (long int)_LOG_ARG_NAME_EXTRACT(printf_arg)); }                \
      else if ((_LOG_ARG_SIZE_GET(printf_arg)) == 8) { _LOG_SYNC_PRINTF("%lld", (long long int)_LOG_ARG_NAME_EXTRACT(printf_arg)); }          \
      )                                                                                                                                       \
    )(                                                                                                                                        \
    _LOG_SYNC_PRINTF(printf_arg);                                                                                                             \
    )                                                                                                                                         \
}

#define _LOG_SYNC_IMPL(output_func, ...)                       PP_FOR_EACH_WITH_CTXT(_LOG_SYNC_OUTPUT,        \
                                                                                     _LOG_SYNC_IMPL_CTXT_SET, \
                                                                                     _LOG_SYNC_OUTPUT,        \
                                                                                     output_func,             \
                                                                                     __VA_ARGS__)

#define _LOG_SYNC_IMPL_CTXT_SET(cur_arg, cur_ctxt)             cur_ctxt

/********************************************************************************************************
 *                                   ASYNCHRONOUS MODE SPECIFIC MACRO'S
 *******************************************************************************************************/

typedef CPU_INT16U _LOG_STR_LEN;

#define _LOG_ASYNC_FMT_STR_BUILD(...)                               PP_FOR_EACH(_LOG_ASYNC_FMT_STR_BUILD_ITER, __VA_ARGS__)

#define _LOG_ASYNC_FMT_STR_BUILD_ITER(printf_arg)                      PP_IF_ELSE(_LOG_ARG_HAS_SPEC(printf_arg))( \
    PP_IF_ELSE(_LOG_ARG_SPEC_IS_VALID(_LOG_ARG_SPEC_EXTRACT(printf_arg)))(                                        \
      "%" PP_STRINGIFY(_LOG_ARG_SPEC_EXTRACT(printf_arg))                                                         \
      )(                                                                                                          \
      Unsupported type spec                                                                                       \
      )                                                                                                           \
    )(                                                                                                            \
    printf_arg                                                                                                    \
    )

#define  _LOG_ASYNC_ARG_LEN_TBL_BUILD(...)                         PP_FOR_EACH_WITH_CTXT(_LOG_ASYNC_ARG_LEN_TBL_BUILD_ITER, \
                                                                                         PP_EMPTY,                          \
                                                                                         _LOG_ASYNC_ARG_LEN_TBL_BUILD_TERM, \
                                                                                         /* */,                             \
                                                                                         __VA_ARGS__)

#define _LOG_ASYNC_ARG_LEN_TBL_BUILD_ITER(printf_arg, cur_ctxt)        PP_IF(_LOG_ARG_HAS_SPEC(printf_arg))( \
    PP_IF_ELSE(_LOG_ARG_SPEC_IS_STR(_LOG_ARG_SPEC_EXTRACT(printf_arg)))(                                     \
      sizeof(_LOG_STR_LEN),                                                                                  \
      )(                                                                                                     \
      sizeof(_LOG_ARG_NAME_EXTRACT(printf_arg)),                                                             \
      )                                                                                                      \
    )
#define _LOG_ASYNC_ARG_LEN_TBL_BUILD_TERM(printf_arg, cur_ctxt)        PP_IF(_LOG_ARG_HAS_SPEC(printf_arg))( \
    PP_IF_ELSE(_LOG_ARG_SPEC_IS_STR(_LOG_ARG_SPEC_EXTRACT(printf_arg)))(                                     \
      sizeof(_LOG_STR_LEN),                                                                                  \
      )(                                                                                                     \
      sizeof(_LOG_ARG_NAME_EXTRACT(printf_arg)),                                                             \
      )                                                                                                      \
    )

#define _LOG_ASYNC_ARG_LIST_BUILD(...)                              PP_FOR_EACH_WITH_CTXT(_LOG_ASYNC_ARG_LIST_BUILD_ITER,     \
                                                                                          _LOG_ASYNC_ARG_LIST_BUILD_CTXT_SET, \
                                                                                          _LOG_ASYNC_ARG_LIST_BUILD_TERM,     \
                                                                                          1,                                  \
                                                                                          __VA_ARGS__)

#define  _LOG_ASYNC_ARG_LIST_BUILD_CTXT_SET(cur_printf_arg, cur_ctxt)  PP_AND(cur_ctxt, PP_NOT(_LOG_ARG_HAS_SPEC(cur_printf_arg)))

#define _LOG_ASYNC_ARG_LIST_BUILD_ITER(cur_printf_arg, cur_ctxt)       PP_IF(_LOG_ARG_HAS_SPEC(cur_printf_arg)) ( \
    PP_IF(PP_NOT(cur_ctxt)) (                                                                                     \
      ,                                                                                                           \
      )                                                                                                           \
    cur_printf_arg                                                                                                \
    )

#define  _LOG_ASYNC_ARG_LIST_BUILD_TERM(cur_printf_arg, cur_ctxt)      _LOG_ASYNC_ARG_LIST_BUILD_ITER(cur_printf_arg, cur_ctxt)

#define _LOG_RING_BUF_WR(cur_printf_arg, cur_wr_pos)                   PP_IF(_LOG_ARG_HAS_SPEC(cur_printf_arg)) (           \
    PP_IF_ELSE(_LOG_ARG_SPEC_IS_STR(_LOG_ARG_SPEC_EXTRACT(cur_printf_arg)))(                                                \
      _LOG_RING_BUF_VAL_WR(cur_wr_pos,                                                                                      \
                           (_LOG_STR_LEN)(Str_Len((const CPU_CHAR *)(_LOG_ARG_NAME_EXTRACT(cur_printf_arg))) + 1u));        \
      Str_Copy((CPU_CHAR *)(cur_wr_pos + sizeof(_LOG_STR_LEN)), (const CPU_CHAR *)(_LOG_ARG_NAME_EXTRACT(cur_printf_arg))); \
      )(                                                                                                                    \
      PP_IF_ELSE(_LOG_ARG_SPEC_IS_PTR(_LOG_ARG_SPEC_EXTRACT(cur_printf_arg)))(                                              \
        _LOG_RING_BUF_VAL_WR(cur_wr_pos, (CPU_ADDR)_LOG_ARG_NAME_EXTRACT(cur_printf_arg));                                  \
        )(                                                                                                                  \
        _LOG_RING_BUF_VAL_WR(cur_wr_pos, _LOG_ARG_NAME_EXTRACT(cur_printf_arg));                                            \
        )                                                                                                                   \
      )                                                                                                                     \
    )

#define _LOG_RING_BUF_VAL_WR(p_wr, val)                             { \
  CPU_INT64U tmp = (CPU_INT64U)(val);                                 \
  CPU_SIZE_T k = 0u;                                                  \
  for (k = 0u; k < sizeof(val); k++) {                                \
    (p_wr)[k] = (tmp >> (8u * k)) & 0xFF;                             \
  }                                                                   \
}

#define _LOG_ARG_SIZE_ADD(printf_arg)                                  + PP_IF_ELSE(_LOG_ARG_HAS_SPEC(printf_arg))( \
    PP_IF_ELSE(_LOG_ARG_SPEC_IS_STR(_LOG_ARG_SPEC_EXTRACT(printf_arg)))(                                            \
      Str_Len((const CPU_CHAR *)(_LOG_ARG_NAME_EXTRACT(printf_arg))) + 1u + sizeof(_LOG_STR_LEN)                    \
      )(                                                                                                            \
      _LOG_ARG_SIZE_GET(printf_arg)                                                                                 \
      )                                                                                                             \
    )(                                                                                                              \
    CPU_CFG_ADDR_SIZE                                                                                               \
    )

#define _LOG_ARGS_TOTAL_SIZE_GET(...)                               0 PP_FOR_EACH(_LOG_ARG_SIZE_ADD, _LOG_ASYNC_FMT_STR_BUILD(__VA_ARGS__), _LOG_ASYNC_ARG_LIST_BUILD(__VA_ARGS__), /* Empty arg. */)

#define _LOG_ASYNC_IMPL(output_func, ...)                           do {                                                                 \
  CPU_INT08U                               *p_buf;                                                                                       \
  static const LOGGING_OUTPUT_FUNC_CB_DATA log_out_func_data = { (int (*)(int))output_func };                                            \
  static const CPU_INT08U                  _printf_arg_len_tbl[] = { PP_IF_ELSE(PP_HAS_ARGS(_LOG_ASYNC_ARG_LEN_TBL_BUILD(__VA_ARGS__)))( \
                                                                       _LOG_ASYNC_ARG_LEN_TBL_BUILD(__VA_ARGS__)                         \
                                                                       )(                                                                \
                                                                       0                                                                 \
                                                                       )                                                                 \
  };                                                                                                                                     \
  static const LOG_ASYNC_FMT_DATA          _printf_fmt_data = { .FmtStr = _LOG_ASYNC_FMT_STR_BUILD(__VA_ARGS__),                         \
                                                                .ArgLenTbl = &_printf_arg_len_tbl[0],                                    \
                                                                .OutputFunc = Logging_OutputFunc,                                        \
                                                                .OutputFuncArgPtr = &log_out_func_data                                   \
  };                                                                                                                                     \
  CORE_DECLARE_IRQ_STATE;                                                                                                                \
  CORE_ENTER_ATOMIC();                                                                                                                   \
  p_buf = RingBufWrAlloc(&Log_RingBuf, (CPU_INT16U)(_LOG_ARGS_TOTAL_SIZE_GET(__VA_ARGS__)));                                             \
  if (p_buf != DEF_NULL) {                                                                                                               \
    _LOG_RING_BUF_VAL_WR(p_buf, (CPU_ADDR)(&_printf_fmt_data));                                                                          \
    PP_FOR_EACH_WITH_CTXT(_LOG_RING_BUF_WR,                                                                                              \
                          _LOG_RING_BUF_WR_POS_INC,                                                                                      \
                          _LOG_RING_BUF_WR,                                                                                              \
                          p_buf + CPU_CFG_ADDR_SIZE,                                                                                     \
                          _LOG_ASYNC_ARG_LIST_BUILD(__VA_ARGS__))                                                                        \
    RingBufWrCommit(&Log_RingBuf);                                                                                                       \
  }                                                                                                                                      \
  CORE_EXIT_ATOMIC();                                                                                                                    \
} while (0)

#define _LOG_RING_BUF_WR_POS_INC(cur_printf_arg, cur_wr_pos)           cur_wr_pos                  \
  + PP_IF_ELSE(_LOG_ARG_SPEC_IS_STR(_LOG_ARG_SPEC_EXTRACT(cur_printf_arg)))(                       \
    Str_Len((const CPU_CHAR *)(_LOG_ARG_NAME_EXTRACT(cur_printf_arg))) + 1u + sizeof(_LOG_STR_LEN) \
    )(                                                                                             \
    _LOG_ARG_SIZE_GET(cur_printf_arg)                                                              \
    )

/********************************************************************************************************
 ********************************************************************************************************
 *                               HIERARCHICAL CONFIGURATION GETTER MACRO'S
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       LOGGING LVL & MODE DEFINES
 *******************************************************************************************************/

//                                                                 Must not have the 'u' suffix.
#define  RTOS_CFG_LOG_LVL_DFLT            0
#define  RTOS_CFG_LOG_LVL_OFF             1
#define  RTOS_CFG_LOG_LVL_ERR             2
#define  RTOS_CFG_LOG_LVL_DBG             3
#define  RTOS_CFG_LOG_LVL_VRB             4

#define  RTOS_CFG_LOG_TS_DFLT             0
#define  RTOS_CFG_LOG_TS_TS_EN            1
#define  RTOS_CFG_LOG_TS_TS_DIS           2

#define  RTOS_CFG_LOG_FUNC_NAME_DFLT      0
#define  RTOS_CFG_LOG_FUNC_NAME_FUNC_DIS  1
#define  RTOS_CFG_LOG_FUNC_NAME_FUNC_EN   2

#define  RTOS_CFG_LOG_OUTPUT_MODE_DFLT    0
#define  RTOS_CFG_LOG_OUTPUT_MODE_SYNC    1
#define  RTOS_CFG_LOG_OUTPUT_MODE_ASYNC   2

//                                                                 Configuration parameter index.
#define _LOG_CH_CFG_PARAM_IX_LVL                              1
#define _LOG_CH_CFG_PARAM_IX_OUTPUT_MODE                      2
#define _LOG_CH_CFG_PARAM_IX_FUNC_NAME                        3
#define _LOG_CH_CFG_PARAM_IX_TS                               4
#define _LOG_CH_CFG_PARAM_IX_OUTPUT_FUNC                      5

//                                                                 Default configuration values.
#define _LOG_CH_CFG_PARAM_INIT_VAL(param)                     PP_IF_ELSE(_LOG_CH_CFG_IS_DFLT(ALL, param)) ( \
    PP_CONCAT(_LOG_CH_CFG_PARAM_INIT_VAL_, param)                                                           \
    )(                                                                                                      \
    _LOG_CH_CFG_PARAM_RAW_VAL_GET(ALL, param)                                                               \
    )

#define _LOG_CH_CFG_PARAM_INIT_VAL_LVL                        OFF
#define _LOG_CH_CFG_PARAM_INIT_VAL_OUTPUT_MODE                SYNC
#define _LOG_CH_CFG_PARAM_INIT_VAL_FUNC                       FUNC_DIS
#define _LOG_CH_CFG_PARAM_INIT_VAL_TS                         TS_DIS
#define _LOG_CH_CFG_PARAM_INIT_VAL_OUTPUT_FUNC                DEF_NULL

/********************************************************************************************************
 *                                   _LOG_CH_HIERARCHY_LIST_BUILD()
 *
 * Description :
 *
 * Argument(s) : __VA_ARGS__
 *
 * Expansion   : The parameter's value.
 *
 * Note(s)     : none.
 *******************************************************************************************************/

#define  _LOG_CH_HIERARCHY_LIST_BUILD(...)                          PP_FOR_EACH_WITH_CTXT(_LOG_CH_HIERARCHY_LIST_BUILD_ITER,     \
                                                                                          _LOG_CH_HIERARCHY_LIST_BUILD_CTXT_SET, \
                                                                                          _LOG_CH_HIERARCHY_LIST_BUILD_TERM,     \
                                                                                          /* empty arg */,                       \
                                                                                          __VA_ARGS__)

#define  _LOG_CH_HIERARCHY_LIST_BUILD_ITER(cur_ch, cur_ctxt)        PP_IF_ELSE(PP_HAS_ARGS(cur_ctxt))(PP_CONCAT_UNDERSCORE(cur_ctxt, cur_ch))(cur_ch),
#define  _LOG_CH_HIERARCHY_LIST_BUILD_CTXT_SET(cur_ch, cur_ctxt)    PP_IF_ELSE(PP_HAS_ARGS(cur_ctxt))(PP_CONCAT_UNDERSCORE(cur_ctxt, cur_ch))(cur_ch)
#define  _LOG_CH_HIERARCHY_LIST_BUILD_TERM(cur_ch, cur_ctxt)        PP_IF_ELSE(PP_HAS_ARGS(cur_ctxt))(PP_CONCAT_UNDERSCORE(cur_ctxt, cur_ch))(cur_ch)

/********************************************************************************************************
 *                                       _LOG_CH_CFG_PARAM_VAL_GET()
 *
 * Description : Get log configuration parameter's value.
 *
 * Argument(s) : ch       Channel to get configuration of.
 *
 *               param    Parameter to get value of.
 *
 * Expansion   : The parameter's value.
 *
 * Note(s)     : none.
 *******************************************************************************************************/

#define  _LOG_CH_CFG_PARAM_RAW_VAL_GET(ch, param)                  PP_1D_LIST_ELEM_GET_AT((PP_CONCAT(RTOS_CFG_LOG_, ch)), \
                                                                                          PP_CONCAT(_LOG_CH_CFG_PARAM_IX_, param))

#define _LOG_CH_CFG_PARAM_VAL_GET(ch, param)                        PP_IF_ELSE(PP_NOT(PP_ARE_EQUAL(PP_CONCAT(_LOG_CH_CFG_PARAM_IX_, param), _LOG_CH_CFG_PARAM_IX_OUTPUT_FUNC)))( \
    PP_CONCAT_UNDERSCORE(RTOS_CFG_LOG,                                                                                                                                           \
                         PP_CONCAT_UNDERSCORE(param, _LOG_CH_CFG_PARAM_RAW_VAL_GET(ch, param)))                                                                                  \
    )(                                                                                                                                                                           \
    PP_1D_LIST_ELEM_GET_AT((PP_CONCAT_UNDERSCORE(RTOS_CFG_LOG, ch)),                                                                                                             \
                           PP_CONCAT(_LOG_CH_CFG_PARAM_IX_, param))                                                                                                              \
    )

/********************************************************************************************************
 *                                   _LOG_CH_CFG_EFFECTIVE_VAL_GET()
 *
 * Description : Check if a given log configuration parameter's value is valid.
 *
 * Argument(s) : ch
 *
 * Expansion   : The type specifier
 *
 * Note(s)     : none.
 *******************************************************************************************************/

#define  _LOG_CH_CFG_EFFECTIVE_RAW_VAL_GET(param, ...)                      PP_FOR_EACH_WITH_CTXT(PP_EMPTY,                                   \
                                                                                                  _LOG_CH_CFG_EFFECTIVE_VAL_GET_CTXT_SET,     \
                                                                                                  _LOG_CH_CFG_EFFECTIVE_VAL_GET_TERM,         \
                                                                                                  (param, _LOG_CH_CFG_PARAM_INIT_VAL(param)), \
                                                                                                  _LOG_CH_HIERARCHY_LIST_BUILD(__VA_ARGS__))

#define _LOG_CH_CFG_EFFECTIVE_VAL_GET_CTXT_SET(cur_ch, cur_ctxt)            (PP_1D_LIST_ELEM_GET_AT(cur_ctxt, 1),                                           \
                                                                             PP_IF_ELSE(_LOG_CH_CFG_IS_DFLT(cur_ch, PP_1D_LIST_ELEM_GET_AT(cur_ctxt, 1))) ( \
                                                                               PP_1D_LIST_ELEM_GET_AT(cur_ctxt, 2)                                          \
                                                                               )(                                                                           \
                                                                               _LOG_CH_CFG_PARAM_RAW_VAL_GET(cur_ch, PP_1D_LIST_ELEM_GET_AT(cur_ctxt, 1))   \
                                                                               ))

#define _LOG_CH_CFG_EFFECTIVE_VAL_GET_TERM(cur_ch, cur_ctxt)                PP_IF_ELSE(_LOG_CH_CFG_IS_DFLT(cur_ch, PP_1D_LIST_ELEM_GET_AT(cur_ctxt, 1))) ( \
    PP_1D_LIST_ELEM_GET_AT(cur_ctxt, 2)                                                                                                                    \
    )(                                                                                                                                                     \
    _LOG_CH_CFG_PARAM_RAW_VAL_GET(cur_ch, PP_1D_LIST_ELEM_GET_AT(cur_ctxt, 1))                                                                             \
    )

#define _LOG_CH_CFG_EFFECTIVE_LVL_GET(...)                                  PP_CONCAT(RTOS_CFG_LOG_LVL_, _LOG_CH_CFG_EFFECTIVE_RAW_VAL_GET(LVL, __VA_ARGS__))
#define _LOG_CH_CFG_EFFECTIVE_TS_GET(...)                                   PP_CONCAT(RTOS_CFG_LOG_TS_, _LOG_CH_CFG_EFFECTIVE_RAW_VAL_GET(TS, __VA_ARGS__))
#define _LOG_CH_CFG_EFFECTIVE_FUNC_NAME_GET(...)                            PP_CONCAT(RTOS_CFG_LOG_FUNC_NAME_, _LOG_CH_CFG_EFFECTIVE_RAW_VAL_GET(FUNC_NAME, __VA_ARGS__))
#define _LOG_CH_CFG_EFFECTIVE_OUTPUT_MODE_GET(...)                          PP_CONCAT(RTOS_CFG_LOG_OUTPUT_MODE_, _LOG_CH_CFG_EFFECTIVE_RAW_VAL_GET(OUTPUT_MODE, __VA_ARGS__))
#define _LOG_CH_CFG_EFFECTIVE_OUTPUT_FUNC_GET(...)                          _LOG_CH_CFG_EFFECTIVE_RAW_VAL_GET(OUTPUT_FUNC, __VA_ARGS__)

/********************************************************************************************************
 *                                       _LOG_CH_CFG_IS_VALID()
 *
 * Description : Check if a given log configuration parameter's value is valid.
 *
 * Argument(s) : ch         Channel associated with the configuration to be checked.
 *
 *               param      Parameter to be checked among LVL, OUTPUT_MODE, FUNC, TS, OUTPUT_FUNC.
 *
 * Expansion   : '1', if the parameter's value is valid.
 *               '0', otherwise.
 *
 * Note(s)     : none.
 *******************************************************************************************************/

#define  _LOG_CH_CFG_IS_DFLT(ch, param)                                    PP_NOT(PP_IF_ELSE(PP_HAS_ARGS(PP_CONCAT(RTOS_CFG_LOG_, ch))) (                                                          \
                                                                                    PP_IF_ELSE(PP_IS_LTE(PP_CONCAT(_LOG_CH_CFG_PARAM_IX_, param), PP_NARGS(PP_CONCAT(RTOS_CFG_LOG_, ch)))) (       \
                                                                                      PP_IF_ELSE(PP_OR(PP_IS_DECIMAL_DIGIT(_LOG_CH_CFG_PARAM_VAL_GET(ch, param)),                                  \
                                                                                                       PP_ARE_EQUAL(PP_CONCAT(_LOG_CH_CFG_PARAM_IX_, param), _LOG_CH_CFG_PARAM_IX_OUTPUT_FUNC))) ( \
                                                                                        PP_IF_ELSE(PP_NOT(PP_ARE_EQUAL(0, _LOG_CH_CFG_PARAM_VAL_GET(ch, param)))) (                                \
                                                                                          1                                                                                                        \
                                                                                          )(                                                                                                       \
                                                                                          0                                                                                                        \
                                                                                          )                                                                                                        \
                                                                                        )(                                                                                                         \
                                                                                        0                                                                                                          \
                                                                                        )                                                                                                          \
                                                                                      )(                                                                                                           \
                                                                                      0                                                                                                            \
                                                                                      )                                                                                                            \
                                                                                    )(                                                                                                             \
                                                                                    0                                                                                                              \
                                                                                    ))
#endif

//                                                                 Separate file does cfg checks to avoid putting ...
//                                                                 too much not used stuff in current file.
#include  <common/source/logging/logging_chk_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // End of private logging module include.
