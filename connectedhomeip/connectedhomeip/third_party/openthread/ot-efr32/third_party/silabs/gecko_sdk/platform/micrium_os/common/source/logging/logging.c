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
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>

#include  <common/include/logging.h>
#include  <common/include/lib_mem.h>
#include  <common/include/lib_str.h>

#include  <common/source/lib/lib_str_priv.h>
#include  <common/source/common/common_priv.h>
#include  <common/source/logging/logging_priv.h>
#include  <common/source/ring_buf/ring_buf_priv.h>
#include  <common/source/rtos/rtos_utils_priv.h>

#include  <rtos_cfg.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (CPU_CFG_ADDR_SIZE == CPU_WORD_SIZE_16)
#define   MEM_ADDR_GET(addr)      MEM_VAL_GET_INT16U(addr)
#elif (CPU_CFG_ADDR_SIZE == CPU_WORD_SIZE_32)
#define   MEM_ADDR_GET(addr)      MEM_VAL_GET_INT32U(addr)
#elif (CPU_CFG_ADDR_SIZE == CPU_WORD_SIZE_64)
#define   MEM_ADDR_GET(addr)      MEM_VAL_GET_INT64U(addr)
#else
#error "Logging: unsupported architecture."
#endif

#define  RTOS_MODULE_CUR          RTOS_CFG_MODULE_COMMON

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (RTOS_CFG_LOG_EN == DEF_ENABLED)
RING_BUF Log_RingBuf = RING_BUF_INIT_NULL();
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (RTOS_CFG_LOG_EN == DEF_ENABLED)
static void Log_PrintfTypeSpecGen(CPU_CHAR   *p_type_spec,
                                  CPU_CHAR   spec,
                                  CPU_INT08U len);
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               Log_DataIsAvail()
 *
 * @brief    Indicates if there is data present to be logged or not.
 *
 * @return   DEF_YES,  if there is data that is available to be logged,
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/

#if (RTOS_CFG_LOG_EN == DEF_ENABLED)
CPU_BOOLEAN Log_DataIsAvail(void)
{
  CPU_BOOLEAN ret_val;

  ret_val = RingBufDataIsAvail(&Log_RingBuf);

  return (ret_val);
}
#endif

/****************************************************************************************************//**
 *                                               Log_Output()
 *
 * @brief    Outputs data accumulated in the ring buffer.
 *******************************************************************************************************/

#if (RTOS_CFG_LOG_EN == DEF_ENABLED)
void Log_Output(void)
{
  void                        *p_arg;
  void                        *p_fmt_data_addr;
  CPU_ADDR                    fmt_data_addr;
  const LOG_ASYNC_FMT_DATA    *p_fmt_data;
  LOGGING_OUTPUT_FUNC_CB_DATA *p_out_func_data;
  const CPU_CHAR              *p_fmt_str;
  const CPU_CHAR              *p_cur_char;
  const CPU_INT08U            *p_cur_arg_len;
  CPU_CHAR                    printf_type_spec[5];

  if (RING_BUF_IS_NULL(&Log_RingBuf) == DEF_YES) {              // If ring buf is not init'd, early return.
    return;
  }

  while (RingBufRdStart(&Log_RingBuf)) {
    p_fmt_data_addr = RingBufRd(&Log_RingBuf, CPU_CFG_ADDR_SIZE);
    fmt_data_addr = MEM_ADDR_GET((CPU_ADDR)p_fmt_data_addr);
    p_fmt_data = (LOG_ASYNC_FMT_DATA *)fmt_data_addr;
    p_out_func_data = (LOGGING_OUTPUT_FUNC_CB_DATA *)p_fmt_data->OutputFuncArgPtr;
    p_fmt_str = p_fmt_data->FmtStr;
    p_cur_arg_len = p_fmt_data->ArgLenTbl;

    while (*p_fmt_str != '\0') {
      p_cur_char = p_fmt_str;
      while ((*p_cur_char != '%') && (*p_cur_char != '\0')) {
        p_fmt_data->OutputFunc(*p_cur_char, p_out_func_data);
        p_cur_char++;
      }

      if (*p_cur_char == '%') {
        p_cur_char++;
        p_arg = RingBufRd(&Log_RingBuf, *p_cur_arg_len);
        Log_PrintfTypeSpecGen(printf_type_spec, *p_cur_char, *p_cur_arg_len);
        switch (*p_cur_char) {
          case 'p':
            (void)Str_Printf(p_fmt_data->OutputFunc, p_out_func_data, printf_type_spec, *((void **)p_arg));
            break;
          case 's':
            p_arg = RingBufRd(&Log_RingBuf, *((_LOG_STR_LEN *)p_arg));
            (void)Str_Printf(p_fmt_data->OutputFunc, p_out_func_data, printf_type_spec, (CPU_CHAR *)p_arg);
            break;
          case 'd':
            switch (*p_cur_arg_len) {
              case 1:
              {
                CPU_INT08S tmp = 0u;
                Mem_Copy(&tmp, p_arg, 1u);
                (void)Str_Printf(p_fmt_data->OutputFunc, p_out_func_data, "%d", tmp);
              }
              break;
              case 2:
              {
                CPU_INT16S tmp = 0u;
                Mem_Copy(&tmp, p_arg, 2u);
                (void)Str_Printf(p_fmt_data->OutputFunc, p_out_func_data, "%d", tmp);
              }
              break;
              case 4:
              {
                CPU_INT32S tmp = 0u;
                Mem_Copy(&tmp, p_arg, 4u);
                (void)Str_Printf(p_fmt_data->OutputFunc, p_out_func_data, "%ld", tmp);
              }
              break;
              case 8:
              {
                CPU_INT64S tmp = 0u;
                Mem_Copy(&tmp, p_arg, 8u);
                (void)Str_Printf(p_fmt_data->OutputFunc, p_out_func_data, "%lld", tmp);
              }
              break;
              default:
                RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
            }
            break;
          case 'u':
          case 'x':
          case 'X':
            switch (*p_cur_arg_len) {
              case 1:
              {
                CPU_INT08U tmp = 0u;
                Mem_Copy(&tmp, p_arg, 1u);
                (void)Str_Printf(p_fmt_data->OutputFunc, p_out_func_data, "%u", tmp);
              }
              break;
              case 2:
              {
                CPU_INT16U tmp = 0u;
                Mem_Copy(&tmp, p_arg, 2u);
                (void)Str_Printf(p_fmt_data->OutputFunc, p_out_func_data, "%u", tmp);
              }
              break;
              case 4:
              {
                CPU_INT32U tmp = 0u;
                Mem_Copy(&tmp, p_arg, 4u);
                (void)Str_Printf(p_fmt_data->OutputFunc, p_out_func_data, "%lu", tmp);
              }
              break;
              case 8:
              {
                CPU_INT64U tmp = 0u;
                Mem_Copy(&tmp, p_arg, 8u);
                (void)Str_Printf(p_fmt_data->OutputFunc, p_out_func_data, "%llu", tmp);
              }
              break;
              default:
                RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
            }
            break;
          default:
            RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
        }
        p_cur_char++;
        p_cur_arg_len++;
      }
      p_fmt_str = p_cur_char;
    }

    RingBufRdEnd(&Log_RingBuf);
  }
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           PRIVATE FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               Log_Init()
 *
 * @brief    Initializes the logging module.
 *
 * @param    p_cfg   Pointer to the logging configuration structure. If AsyncBufSize is 0, the
 *                   asynchronous buffer will not be allocated, which prevents the use of asynchronous
 *                   logs.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s) from
 *                   this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_SEG_OVF
 *******************************************************************************************************/

#if (RTOS_CFG_LOG_EN == DEF_ENABLED)
void Log_Init(COMMON_CFG_LOGGING *p_cfg,
              RTOS_ERR           *p_err)
{
  MEM_SEG *p_mem_seg;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((RING_BUF_IS_NULL(&Log_RingBuf) == DEF_YES), *p_err, RTOS_ERR_ALREADY_INIT,; );

  p_mem_seg = Common_MemSegLoggingPtrGet();

  if (p_cfg->AsyncBufSize != 0u) {
    RingBufCreate(&Log_RingBuf,
                  p_mem_seg,
                  p_cfg->AsyncBufSize,
                  p_err);
  } else {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  }
}
#endif

/****************************************************************************************************//**
 *                                           Logging_OutputFunc()
 *
 * @brief    Output function used to print data by the logging module.
 *
 * @param    c       Character to output.
 *
 * @param    p_arg   Callback function that needs to be used to output character.
 *
 * @return   0.
 *******************************************************************************************************/
int Logging_OutputFunc(int  c,
                       void *p_arg)
{
  LOGGING_OUTPUT_FUNC_CB_DATA *p_out_data;

  RTOS_ASSERT_DBG((p_arg != DEF_NULL), RTOS_ERR_NULL_PTR, 0);

  p_out_data = (LOGGING_OUTPUT_FUNC_CB_DATA *)p_arg;

  p_out_data->ByteOutCb(c);

  return (0);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           Log_PrintfTypeSpecGen()
 *
 * @brief    Gets correct type specifier based on 'spec' and 'len' to pass to printf-like functions.
 *
 * @param    p_type_spec     Pointer to start of type specifier buffer.
 *
 * @param    spec            Specifier character.
 *
 * @param    len             Length.
 *******************************************************************************************************/

#if (RTOS_CFG_LOG_EN == DEF_ENABLED)
static void Log_PrintfTypeSpecGen(CPU_CHAR   *p_type_spec,
                                  CPU_CHAR   spec,
                                  CPU_INT08U len)
{
  *p_type_spec = '%';
  p_type_spec++;

  if (spec == 'x' || spec == 'X' || spec == 'p') {
    *p_type_spec = '#';
    p_type_spec++;
  }

  if (spec != 's') {
    RTOS_ASSERT_CRITICAL(((len == 1u)
                          || (len == 2u)
                          || (len == 4u)
                          || (len == 8u)), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

    if ((len == 4u)
        || (len == 8u)) {
      *p_type_spec = 'l';
      p_type_spec++;
    }
    if (len == 8u) {
      *p_type_spec = 'l';
      p_type_spec++;
    }
  }

  if (spec == 'p') {
    *p_type_spec = 'x';
  } else {
    *p_type_spec = spec;
  }

  p_type_spec++;
  *p_type_spec = '\0';
}
#endif
