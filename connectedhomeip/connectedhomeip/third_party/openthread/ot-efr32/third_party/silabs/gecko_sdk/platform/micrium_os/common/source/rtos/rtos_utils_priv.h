/***************************************************************************//**
 * @file
 * @brief Common - Rtos Utilities
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
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _RTOS_UTILS_PRIV_H_
#define  _RTOS_UTILS_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <common/include/rtos_err.h>

#include  <common/include/lib_utils.h>
#include  <common/include/rtos_utils.h>

#include  <common/include/rtos_opt_def.h>

#include  <common/source/logging/logging_priv.h>
#include  <common/source/kal/kal_priv.h>

#include  <common/include/rtos_path.h>
#include  <rtos_cfg.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                                   DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                               ARG CHK DEFINES
 *******************************************************************************************************/

#define  RTOS_ARG_CHK_EXT_EN                       (DEF_BIT_IS_SET_ANY(RTOS_CFG_ASSERT_DBG_ARG_CHK_EXT_MASK, (RTOS_MODULE_CUR)))

/********************************************************************************************************
 *                                               EXT ERR DEFINES
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       RTOS_ERR_CODE_LOG_ARG_GET()
 *
 * @brief    Returns logging argument corresponding to the given error code. The logging argument is
 *           either a string or an integer depending on whether the error strings are enabled or not.
 *
 * @param    err_code    Error code to generate logging argument from.
 *
 * @return   Log argument for error code.
 *******************************************************************************************************/

#if (RTOS_ERR_CFG_STR_EN == DEF_ENABLED)
#define  RTOS_ERR_CODE_LOG_ARG_GET(err_code)       (s)RTOS_ERR_STR_GET((err_code))
#else
#define  RTOS_ERR_CODE_LOG_ARG_GET(err_code)       (u)(err_code)
#endif

/****************************************************************************************************//**
 *                                           RTOS_ERR_LOG_ARG_GET()
 *
 * @brief    Returns logging argument corresponding to the given error (extended or not depending on
 *           extended error configuration). The logging argument is either a string or an integer
 *           depending on whether the error strings are enabled or not.
 *
 * @param    err_var     Error (of type RTOS_ERR) to generate logging argument from.
 *
 * @return   Log argument for error variable.
 *******************************************************************************************************/

#define  RTOS_ERR_LOG_ARG_GET(err_var)              RTOS_ERR_CODE_LOG_ARG_GET(RTOS_ERR_CODE_GET((err_var)))

/****************************************************************************************************//**
 *                           RTOS_ERR_SET_AND_LOG_ERR() / RTOS_ERR_SET_AND_LOG_DBG()
 *
 * @brief    Sets a given error variable to an error value. Also sets the extended error content, if
 *           it is enabled and logs the error string in the default channel either at ERR level or at
 *           DBG level, if enabled.
 *
 * @param    err_var         Error variable to set to 'err'.
 *
 * @param    err_code        Error code to which to set 'err_var' / *p_err.
 *
 * @param    __VA_ARGS__     Variable arguments that will be passed to the logging function. See LOG_ERR()
 *                           for more details.
 *******************************************************************************************************/

#define  RTOS_ERR_SET_AND_LOG_ERR(err_var, err_code, args)  RTOS_ERR_SET((err_var), (err_code)); \
  LOG_ERR(args)

#define  RTOS_ERR_SET_AND_LOG_DBG(err_var, err_code, args)  RTOS_ERR_SET((err_var), (err_code)); \
  LOG_DBG(args)

/********************************************************************************************************
 *                                           ASSERT LOGGING DEFINES
 ********************************************************************************************************
 * Note(s) : (1) The assert logging is handled differently than the others, since it MUST output its
 *               information directly and the event CANNOT be posted in queue, since a CPU_SW_EXCEPTION
 *               will be triggered right after the log. It also includes additional info (file, line,
 *               function) that may not be required by all other logging events.
 *******************************************************************************************************/

#define  RTOS_ASSERT_ERR_CODE_FAIL_LOG(expr, err_code)      LOG_SYNC(_LOG_CH_CFG_EFFECTIVE_OUTPUT_FUNC_GET(PP_SCAN LOG_DFLT_CH), \
                                                                     ("Assert ",                #expr,                           \
                                                                      " failed in file ", __FILE__,                              \
                                                                      " at line ", (d)__LINE__,                                  \
                                                                      " in function ", (s) & __func__[0],                        \
                                                                      ". Reported error is : ", RTOS_ERR_CODE_LOG_ARG_GET((err_code))));

#if (RTOS_ERR_CFG_EXT_EN == DEF_DISABLED)

    #define  RTOS_ASSERT_ERR_VAR_FAIL_LOG(expr, err_var)    RTOS_ASSERT_ERR_CODE_FAIL_LOG(expr, RTOS_ERR_CODE_GET((err_var)))

#else

    #define  RTOS_ASSERT_ERR_VAR_FAIL_LOG(expr, err_var)    LOG_SYNC(_LOG_CH_CFG_EFFECTIVE_OUTPUT_FUNC_GET(PP_SCAN LOG_DFLT_CH), \
                                                                     ("Assert ",              #expr,                             \
                                                                      " failed due to err: ", RTOS_ERR_LOG_ARG_GET((err_var)),   \
                                                                      ", coming from file ", (s)err_var.FileName,                \
                                                                      " at line ", (d)err_var.LineNbr,                           \
                                                                      " in function ", (s)err_var.FnctName));

#endif

/********************************************************************************************************
 *                                               ASSERT DEFINES
 ********************************************************************************************************
 * Note(s) : (1) The macros have been done so that they could be overridden with as much flexibility as
 *               possible. The default behavior is the same for every level of assertion, the notes below
 *               indicate how to override for each level.
 *
 *               (a) Overriding the RTOS_ASSERT_{DBG|CRITICAL} macro will have an impact for that level
 *                   (either DBG or CRITICAL) only.
 *
 *           (2) The macro RTOS_ASSERT_DBG_ERR_PTR_VALIDATE has a different behavior, since no error can
 *               be reported if error pointer passed to function is NULL. In that case, a CPU_SW_EXCEPTION
 *               is ALWAYS executed.
 *
 *           (3) The 'ret_val' parameter cannot have parentheses added to it when receiving it as a
 *               parameter, since in the case of a void return value, ';' is given as parameter, to be
 *               directly added after a potential 'return' call.
 *******************************************************************************************************/

/********************************************************************************************************
 *                                   RTOS_ASSERT_CRITICAL / RTOS_ASSERT_DBG
 *
 * Description : Assert given expression. In case of failure, log error, and call corresponding
 *               RTOS_ASSERT_xxx_FAILED_END_CALL(ret_val).
 *
 * Argument(s) : expr        Expression to assert. If expression is false, the error will be logged, and
 *                           the error caught call made.
 *
 *               err_code    Error code associated with assert.
 *
 *               ret_val     Value that would be returned from the function, ';' if void.
 *
 * Return(s)   : none.
 *
 * Note(s)     : (1) Usage of assert is as follows:
 *                       RTOS_ASSERT_CRITICAL((p_buf != DEF_NULL), RTOS_ERR_NULL_PTR, DEF_NULL);
 *                       RTOS_ASSERT_DBG((p_buf != DEF_NULL), RTOS_ERR_NULL_PTR, DEF_FAIL);
 *******************************************************************************************************/

#define  RTOS_ASSERT_CRITICAL(expr, err_code, ret_val)          RTOS_ASSERT_CRITICAL_EXEC(expr, ret_val,                                   \
                                                                                          RTOS_ASSERT_ERR_CODE_FAIL_LOG(expr, (err_code)); \
                                                                                          )

#define  RTOS_ASSERT_DBG(expr, err_code, ret_val)               RTOS_ASSERT_DBG_EXEC(expr, ret_val,                                   \
                                                                                     RTOS_ASSERT_ERR_CODE_FAIL_LOG(expr, (err_code)); \
                                                                                     )

/****************************************************************************************************//**
 *                                   RTOS_ASSERT_DBG_ERR_PTR_VALIDATE()
 *
 * @brief    Validate an error pointer received, to see if it can be used to report an error.
 *
 * @param    p_err       Error pointer to validate.
 *
 * @param    ret_val     Value that would be returned from the function, ';' if void.
 *
 * @note     (1) Since no error can be reported if the error pointer is NULL, a CPU_SW_EXCEPTION is
 *               ALWAYS executed if this assert fails. See ASSERT DEFINES's Note #2.
 *
 * @note     (2) Usage of RTOS_ASSERT_DBG_ERR_PTR_VALIDATE is as follows:
 *                   RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, ;);         The error will be RTOS_ERR_ASSERT_ERR_PTR_NULL.
 *                   RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NULL);  The error will be RTOS_ERR_ASSERT_ERR_PTR_NULL.
 *******************************************************************************************************/

#define  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, ret_val)           if (RTOS_ARG_CHK_EXT_EN) { \
    if ((p_err) == DEF_NULL) {                                                                 \
      RTOS_ASSERT_ERR_CODE_FAIL_LOG((p_err == DEF_NULL), RTOS_ERR_ASSERT_ERR_PTR_NULL);        \
      CPU_SW_EXCEPTION(ret_val);                                                               \
    }                                                                                          \
}

/****************************************************************************************************//**
 *                       RTOS_ASSERT_CRITICAL_ERR_SET() / RTOS_ASSERT_DBG_ERR_SET()
 *
 * @brief    Assert given expression. In case of failure, log, set error and call
 *           RTOS_ASSERT_FAILED_END_CALL(ret_val).
 *
 * @param    expr        Expression to assert. If expression is false, the error will be logged, set
 *                       and the error caught call made.
 *
 * @param    err_var     Error variable to set.
 *
 * @param    err_code    Error code to log and set 'err_var' to.
 *
 * @param    ret_val     Value that would be returned from the function, ';' if void.
 *
 * @note     (1) Usage of RTOS_ASSERT_CRITICAL_ERR_SET / RTOS_ASSERT_DBG_ERR_SET /
 *               RTOS_ASSERT_CRITICAL_ERR_PTR_SET / RTOS_ASSERT_DBG_ERR_PTR_SETis as follows:
 *                   RTOS_ASSERT_CRITICAL_ERR_SET((p_buf != DEF_NULL), local_err, RTOS_ERR_NULL_PTR, ;);
 *                   RTOS_ASSERT_DBG_ERR_SET((p_arg != DEF_NULL), local_err, RTOS_ERR_NULL_PTR, DEF_NULL);
 *******************************************************************************************************/

#define  RTOS_ASSERT_CRITICAL_ERR_SET(expr, err_var, err_code, ret_val)         RTOS_ASSERT_CRITICAL_EXEC(expr, ret_val,                                   \
                                                                                                          RTOS_ASSERT_ERR_CODE_FAIL_LOG(expr, (err_code)); \
                                                                                                          RTOS_ERR_SET((err_var), (err_code));             \
                                                                                                          )

#define  RTOS_ASSERT_DBG_ERR_SET(expr, err_var, err_code, ret_val)              RTOS_ASSERT_DBG_EXEC(expr, ret_val,                                   \
                                                                                                     RTOS_ASSERT_ERR_CODE_FAIL_LOG(expr, (err_code)); \
                                                                                                     RTOS_ERR_SET((err_var), (err_code));             \
                                                                                                     )

/********************************************************************************************************
 *                                           RTOS_DBG_FAIL_EXEC
 *                                           RTOS_CRITICAL_FAIL_EXEC
 *
 * Description : Checks if assert is enabled for current module, logs error and calls END_CALL as
 *               configured by user.
 *
 * Argument(s) : err_code    Error code to log.
 *
 *               ret_val     Value that would be returned from the function, ';' if void.
 *
 * Return(s)   : none.
 *
 * Note(s)     : none.
 *******************************************************************************************************/

#define  RTOS_DBG_FAIL_EXEC(err_code, ret_val)                  RTOS_ASSERT_DBG_EXEC(0, ret_val,                                                \
                                                                                     RTOS_ASSERT_ERR_CODE_FAIL_LOG((no condition), (err_code)); \
                                                                                     )

#define  RTOS_CRITICAL_FAIL_EXEC(err_code, ret_val)             RTOS_ASSERT_ERR_CODE_FAIL_LOG((no condition), (err_code)); \
  RTOS_ASSERT_CRITICAL_FAILED_END_CALL(ret_val);                                                                           \


/********************************************************************************************************
 *                                           RTOS_DBG_FAIL_EXEC_ERR
 *
 * Description : Checks if assert is enabled for current module, logs and sets error and calls END_CALL as
 *               configured by user.
 *
 * Argument(s) : err_var     Error variable to set.
 *
 *               err_code    Error code to log and set 'err_var' to.
 *
 *               ret_val     Value that would be returned from the function, ';' if void.
 *
 * Return(s)   : none.
 *
 * Note(s)     : none.
 *******************************************************************************************************/

#define  RTOS_DBG_FAIL_EXEC_ERR(err_var, err_code, ret_val)     RTOS_ASSERT_DBG_EXEC(0, ret_val,                                                \
                                                                                     RTOS_ASSERT_ERR_CODE_FAIL_LOG((no condition), (err_code)); \
                                                                                     RTOS_ERR_SET((err_var), (err_code));                       \
                                                                                     )

/********************************************************************************************************
 *                           RTOS_ASSERT_CRITICAL_EXEC / RTOS_ASSERT_DBG_EXEC
 *
 * Description : Checks if assert is enabled for current module, tests the assertion, executes the failure
 *               operation(s) if assert failed and calls END_CALL as configured by user.
 *
 * Argument(s) : expr        Expression to assert. If expression is false, the error will be logged, set
 *                           and the error caught call made.
 *
 *               ret_val     Value that would be returned from the function, ';' if void.
 *
 *               fail_oper   Operation(s) to execute in case of failure.
 *
 * Return(s)   : none.
 *
 * Note(s)     : (1) These macros MUST not be used by anything else than higher-level assert macros.
 *******************************************************************************************************/

#define  RTOS_ASSERT_CRITICAL_EXEC(expr, ret_val, fail_oper)    if ((expr) == 0u) { \
    fail_oper                                                                       \
    RTOS_ASSERT_CRITICAL_FAILED_END_CALL(ret_val);                                  \
}

#define  RTOS_ASSERT_DBG_EXEC(expr, ret_val, fail_oper)         if (RTOS_ARG_CHK_EXT_EN) { \
    if ((expr) == 0u) {                                                                    \
      fail_oper                                                                            \
      RTOS_ASSERT_DBG_FAILED_END_CALL(ret_val);                                            \
    }                                                                                      \
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // End of rtos utils priv module include.
