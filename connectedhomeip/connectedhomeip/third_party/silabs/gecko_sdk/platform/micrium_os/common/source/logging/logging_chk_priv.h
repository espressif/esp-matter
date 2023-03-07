/***************************************************************************//**
 * @file
 * @brief Common - Logging Checks
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

#ifndef  _LOGGING_CHK_PRIV_H_
#define  _LOGGING_CHK_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                 HIERARCHICAL CONFIGURATION CHK MACRO'S
 ********************************************************************************************************
 *******************************************************************************************************/

#if (RTOS_CFG_LOG_EN == DEF_ENABLED \
     && defined(PP_C_STD_VERSION_C99_PRESENT))

#define  _LOG_CH_CFG_PARAM_CNT_CHK_CTXT_SET(cur_ch, ctxt)    PP_IF_ELSE(ctxt) ( \
    PP_IS_LTE(PP_NARGS(PP_CONCAT(RTOS_CFG_LOG_, cur_ch)), 5)                    \
    )(                                                                          \
    0                                                                           \
    )

#define  _LOG_CH_CFG_PARAM_CNT_CHK_TERM(cur_ch, ctxt)        _LOG_CH_CFG_PARAM_CNT_CHK_CTXT_SET(cur_ch, ctxt)

#define  _LOG_CH_CFG_PARAM_CNT_CHK_IMPL(...)                 PP_FOR_EACH_WITH_CTXT(PP_EMPTY,                           \
                                                                                   _LOG_CH_CFG_PARAM_CNT_CHK_CTXT_SET, \
                                                                                   _LOG_CH_CFG_PARAM_CNT_CHK_TERM,     \
                                                                                   1,                                  \
                                                                                   __VA_ARGS__)

#define  _LOG_CH_CFG_PARAM_CNT_CHK(ch)                       _LOG_CH_CFG_PARAM_CNT_CHK_IMPL(_LOG_CH_HIERARCHY_LIST_BUILD(PP_SCAN ch))

#define  _LOG_CH_CFG_PARAM_IS_VALID_LVL_ERR                       ~, ~
#define  _LOG_CH_CFG_PARAM_IS_VALID_LVL_DBG                       ~, ~
#define  _LOG_CH_CFG_PARAM_IS_VALID_LVL_VRB                       ~, ~
#define  _LOG_CH_CFG_PARAM_IS_VALID_LVL_DFLT                      ~, ~
#define  _LOG_CH_CFG_PARAM_IS_VALID_LVL_OFF                       ~, ~
#define  _LOG_CH_CFG_PARAM_IS_VALID_OUTPUT_MODE_SYNC              ~, ~
#define  _LOG_CH_CFG_PARAM_IS_VALID_OUTPUT_MODE_ASYNC             ~, ~
#define  _LOG_CH_CFG_PARAM_IS_VALID_OUTPUT_MODE_DFLT              ~, ~
#define  _LOG_CH_CFG_PARAM_IS_VALID_FUNC_NAME_FUNC_EN             ~, ~
#define  _LOG_CH_CFG_PARAM_IS_VALID_FUNC_NAME_FUNC_DIS            ~, ~
#define  _LOG_CH_CFG_PARAM_IS_VALID_FUNC_NAME_DFLT                ~, ~
#define  _LOG_CH_CFG_PARAM_IS_VALID_TS_TS_EN                      ~, ~
#define  _LOG_CH_CFG_PARAM_IS_VALID_TS_TS_DIS                     ~, ~
#define  _LOG_CH_CFG_PARAM_IS_VALID_TS_DFLT                       ~, ~

#define  _LOG_CH_CFG_PARAM_IS_VALID_ITER_IMPL(ch, param)                 PP_ARE_EQUAL(PP_NARGS(PP_CONCAT_UNDERSCORE(PP_CONCAT(_LOG_CH_CFG_PARAM_IS_VALID_, param), _LOG_CH_CFG_PARAM_RAW_VAL_GET(ch, param))), 2)

#define  _LOG_CH_CFG_PARAM_IS_VALID(ch_list, n, param)                   PP_IF_ELSE(PP_IS_LTE(PP_CONCAT(_LOG_CH_CFG_PARAM_IX_, param), PP_NARGS(PP_CONCAT(RTOS_CFG_LOG_, PP_1D_LIST_ELEM_GET_AT((_LOG_CH_HIERARCHY_LIST_BUILD ch_list), n)))))( \
    _LOG_CH_CFG_PARAM_IS_VALID_ITER_IMPL(PP_1D_LIST_ELEM_GET_AT((_LOG_CH_HIERARCHY_LIST_BUILD ch_list), n), param)                                                                                                                              \
    )(                                                                                                                                                                                                                                          \
    1                                                                                                                                                                                                                                           \
    )

#define  _LOG_CH_CFG_IS_DEFINED_INTERNAL_IMPL(ch)     defined(RTOS_CFG_LOG_##ch)

#define  _LOG_CH_CFG_IS_DEFINED_INTERNAL(ch)          _LOG_CH_CFG_IS_DEFINED_INTERNAL_IMPL(ch)

#define  PP_LIST_ELEM_GET_AT_CHK(list, n)             PP_IF(PP_IS_LTE(n, PP_NARGS list)) (PP_1D_LIST_ELEM_GET_AT(list, n))

#define  _LOG_CH_CFG_IS_DEFINED(ch, n)                _LOG_CH_CFG_IS_DEFINED_INTERNAL(PP_LIST_ELEM_GET_AT_CHK((_LOG_CH_HIERARCHY_LIST_BUILD ch), n))

#ifdef  RTOS_CFG_LOG_ALL
#if (_LOG_CH_CFG_PARAM_CNT_CHK((ALL)) != 1)
#error "Log channel configuration takes at most 5 comma-separated parameters."
#endif

#if (_LOG_CH_CFG_PARAM_IS_VALID((ALL), 1, LVL) == DEF_NO)
#error "Log level (1st parameter of log channel cfg) must either be ERR, DBG, VRB or DFLT."
#endif

#if (_LOG_CH_CFG_PARAM_IS_VALID((ALL), 1, OUTPUT_MODE) == DEF_NO)
#error "Log output mode (2nd parameter of log channel cfg) must either be SYNC, ASYNC or DFLT."
#endif

#if (_LOG_CH_CFG_PARAM_IS_VALID((ALL), 1, FUNC_NAME) == DEF_NO)
#error "Log function name en/dis (3rd parameter of log channel cfg) must either be FUNC_EN, FUNC_DIS or DFLT."
#endif

#if (_LOG_CH_CFG_PARAM_IS_VALID((ALL), 1, TS) == DEF_NO)
#error "Log timestamp en/dis (4th parameter of log channel cfg) must either be TS_EN, TS_DIS or DFLT."
#endif
#endif

#ifdef  LOG_DFLT_CH
#if _LOG_CH_CFG_IS_DEFINED(LOG_DFLT_CH, 1)
#if (_LOG_CH_CFG_PARAM_CNT_CHK(LOG_DFLT_CH) != 1)
#error "Log channel configuration takes at most 5 comma-separated parameters."
#endif

#if (_LOG_CH_CFG_PARAM_IS_VALID(LOG_DFLT_CH, 1, LVL) == DEF_NO)
#error "Log level (1st parameter of log channel cfg) must either be ERR, DBG, VRB or DFLT"
#endif

#if (_LOG_CH_CFG_PARAM_IS_VALID(LOG_DFLT_CH, 1, OUTPUT_MODE) == DEF_NO)
#error "Log output mode (2nd parameter of log channel cfg) must either be SYNC, ASYNC or DFLT"
#endif

#if (_LOG_CH_CFG_PARAM_IS_VALID(LOG_DFLT_CH, 1, FUNC_NAME) == DEF_NO)
#error "Log function name en/dis (3rd parameter of log channel cfg) must either be FUNC_EN, FUNC_DIS or DFLT"
#endif

#if (_LOG_CH_CFG_PARAM_IS_VALID(LOG_DFLT_CH, 1, TS) == DEF_NO)
#error "Log timestamp en/dis (4th parameter of log channel cfg) must either be TS_EN, TS_DIS or DFLT"
#endif
#endif

#if _LOG_CH_CFG_IS_DEFINED(LOG_DFLT_CH, 2)
#if (_LOG_CH_CFG_PARAM_CNT_CHK(LOG_DFLT_CH) != 1)
#error "Log channel configuration takes at most 5 comma-separated parameters."
#endif

#if (_LOG_CH_CFG_PARAM_IS_VALID(LOG_DFLT_CH, 2, LVL) == DEF_NO)
#error "Log level (1st parameter of log channel cfg) must either be ERR, DBG, VRB or DFLT"
#endif

#if (_LOG_CH_CFG_PARAM_IS_VALID(LOG_DFLT_CH, 2, OUTPUT_MODE) == DEF_NO)
#error "Log output mode (2nd parameter of log channel cfg) must either be SYNC, ASYNC or DFLT"
#endif

#if (_LOG_CH_CFG_PARAM_IS_VALID(LOG_DFLT_CH, 2, FUNC_NAME) == DEF_NO)
#error "Log function name en/dis (3rd parameter of log channel cfg) must either be FUNC_EN, FUNC_DIS or DFLT"
#endif

#if (_LOG_CH_CFG_PARAM_IS_VALID(LOG_DFLT_CH, 2, TS) == DEF_NO)
#error "Log timestamp en/dis (4th parameter of log channel cfg) must either be TS_EN, TS_DIS or DFLT"
#endif
#endif

#if _LOG_CH_CFG_IS_DEFINED(LOG_DFLT_CH, 3)
#if (_LOG_CH_CFG_PARAM_CNT_CHK(LOG_DFLT_CH) != 1)
#error "Log channel configuration takes at most 5 comma-separated parameters."
#endif

#if (_LOG_CH_CFG_PARAM_IS_VALID(LOG_DFLT_CH, 3, LVL) == DEF_NO)
#error "Log level (1st parameter of log channel cfg) must either be ERR, DBG, VRB or DFLT"
#endif

#if (_LOG_CH_CFG_PARAM_IS_VALID(LOG_DFLT_CH, 3, OUTPUT_MODE) == DEF_NO)
#error "Log output mode (2nd parameter of log channel cfg) must either be SYNC, ASYNC or DFLT"
#endif

#if (_LOG_CH_CFG_PARAM_IS_VALID(LOG_DFLT_CH, 3, FUNC_NAME) == DEF_NO)
#error "Log function name en/dis (3rd parameter of log channel cfg) must either be FUNC_EN, FUNC_DIS or DFLT"
#endif

#if (_LOG_CH_CFG_PARAM_IS_VALID(LOG_DFLT_CH, 3, TS) == DEF_NO)
#error "Log timestamp en/dis (4th parameter of log channel cfg) must either be TS_EN, TS_DIS or DFLT"
#endif
#endif

#if (_LOG_CH_CFG_IS_DEFINED(LOG_DFLT_CH, 4))
#if (_LOG_CH_CFG_PARAM_CNT_CHK(LOG_DFLT_CH) != 1)
#error "Log channel configuration takes at most 5 comma-separated parameters."
#endif

#if (_LOG_CH_CFG_PARAM_IS_VALID(LOG_DFLT_CH, 4, LVL) == DEF_NO)
#error "Log level (1st parameter of log channel cfg) must either be ERR, DBG, VRB or DFLT"
#endif

#if (_LOG_CH_CFG_PARAM_IS_VALID(LOG_DFLT_CH, 4, OUTPUT_MODE) == DEF_NO)
#error "Log output mode (2nd parameter of log channel cfg) must either be SYNC, ASYNC or DFLT"
#endif

#if (_LOG_CH_CFG_PARAM_IS_VALID(LOG_DFLT_CH, 4, FUNC_NAME) == DEF_NO)
#error "Log function name en/dis (3rd parameter of log channel cfg) must either be FUNC_EN, FUNC_DIS or DFLT"
#endif

#if (_LOG_CH_CFG_PARAM_IS_VALID(LOG_DFLT_CH, 4, TS) == DEF_NO)
#error "Log timestamp en/dis (4th parameter of log channel cfg) must either be TS_EN, TS_DIS or DFLT"
#endif
#endif

#if (_LOG_CH_CFG_IS_DEFINED(LOG_DFLT_CH, 5))
#if (_LOG_CH_CFG_PARAM_CNT_CHK(LOG_DFLT_CH) != 1)
#error "Log channel configuration takes at most 5 comma-separated parameters."
#endif

#if (_LOG_CH_CFG_PARAM_IS_VALID(LOG_DFLT_CH, 5, LVL) == DEF_NO)
#error "Log level (1st parameter of log channel cfg) must either be ERR, DBG, VRB or DFLT"
#endif

#if (_LOG_CH_CFG_PARAM_IS_VALID(LOG_DFLT_CH, 5, OUTPUT_MODE) == DEF_NO)
#error "Log output mode (2nd parameter of log channel cfg) must either be SYNC, ASYNC or DFLT"
#endif

#if (_LOG_CH_CFG_PARAM_IS_VALID(LOG_DFLT_CH, 5, FUNC_NAME) == DEF_NO)
#error "Log function name en/dis (3rd parameter of log channel cfg) must either be FUNC_EN, FUNC_DIS or DFLT"
#endif

#if (_LOG_CH_CFG_PARAM_IS_VALID(LOG_DFLT_CH, 5, TS) == DEF_NO)
#error "Log timestamp en/dis (4th parameter of log channel cfg) must either be TS_EN, TS_DIS or DFLT"
#endif
#endif

#if (_LOG_CH_CFG_IS_DEFINED(LOG_DFLT_CH, 6))
#if (_LOG_CH_CFG_PARAM_CNT_CHK(LOG_DFLT_CH) != 1)
#error "Log channel configuration takes at most 5 comma-separated parameters."
#endif

#if (_LOG_CH_CFG_PARAM_IS_VALID(LOG_DFLT_CH, 6, LVL) == DEF_NO)
#error "Log level (1st parameter of log channel cfg) must either be ERR, DBG, VRB or DFLT"
#endif

#if (_LOG_CH_CFG_PARAM_IS_VALID(LOG_DFLT_CH, 6, OUTPUT_MODE) == DEF_NO)
#error "Log output mode (2nd parameter of log channel cfg) must either be SYNC, ASYNC or DFLT"
#endif

#if (_LOG_CH_CFG_PARAM_IS_VALID(LOG_DFLT_CH, 6, FUNC_NAME) == DEF_NO)
#error "Log function name en/dis (3rd parameter of log channel cfg) must either be FUNC_EN, FUNC_DIS or DFLT"
#endif

#if (_LOG_CH_CFG_PARAM_IS_VALID(LOG_DFLT_CH, 6, TS) == DEF_NO)
#error "Log timestamp en/dis (4th parameter of log channel cfg) must either be TS_EN, TS_DIS or DFLT"
#endif
#endif

#endif

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // End of private logging module check include.
