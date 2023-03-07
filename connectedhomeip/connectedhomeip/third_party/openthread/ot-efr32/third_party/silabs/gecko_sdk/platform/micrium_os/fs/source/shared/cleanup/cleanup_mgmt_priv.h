/***************************************************************************//**
 * @file
 * @brief File System - Cleanup Management
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.
 * The software is governed by the sections of the MSLA applicable to Micrium
 * Software.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  CLEANUP_MGMT_PRIV_H_
#define  CLEANUP_MGMT_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <common/source/preprocessor/preprocessor_priv.h>
#include  <cpu/include/cpu.h>
#include  <common/source/rtos/rtos_utils_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                                   DEFINES
 *
 *  The following set of macros may be used to alleviate the clean-up process that is commonly triggered
 *  whenever an error occurs. A basic usage example is given bellow. For more complex examples and a
 *  detailed discussion on the implementation details, performance aspects and common pitfalls, please
 *  refer to the online documentation at https://doc.micrium.com.
 *
 *       CPU_BOOLEAN  Foo(RTOS_ERR  p_err)
 *       {
 *           OBJ  *p_obj;
 *
 *           WITH_SCOPE_BEGIN(p_err) {                                      (1)
 *
 *               p_obj = ObjAlloc();
 *               ASSERT_BREAK(p_obj != DEF_NULL, FS_ERR_MEM_ALLOC);         (2)
 *               ON_EXIT {                                                  (3a)(3b)(3c)
 *                   ObjFree(p_obj);
 *               } WITH {                                                   (4)
 *                   BREAK_ON_ERR(Func(p_obj, p_err));                      (5)
 *                   LOG_DBG("Func() successfull.");
 *               }
 *
 *           } WITH_SCOPE_END                                               (1)
 *
 *           if (*p_err != FS_ERR_NONE) {
 *               return (DEF_FAIL);
 *           }
 *           return (DEF_OK);
 *       }
 *
 *  Notes: (1) A clean-up management macro invocation must be performed inside a WITH_SCOPE_BEGIN/
 *               WITH_SCOPE_END delimited compound statement. The execution will jump after the
 *               WITH_SCOPE_END delimiter whenever a break statement is executed inside this compound
 *               statement (but not inside a nested break'able scope, i.e. while, do-while, for, switch,
 *               WITH).
 *
 *           (2) The ASSERT_BREAK(cond, err) macro sets the error (*p_err in the above example) to 'err'
 *               and executes a break statement whenever the 'cond' condition is not fulfilled.
 *
 *           (3) (a) The ON_EXIT block is executed after the WITH block or whenever a break statement is
 *                   executed inside the WITH block (but not inside a nested break'able scope). If the
 *                   ON_EXIT block's end is reached with a null error (0), the execution continues after
 *                   the associated WITH block. Otherwise, the execution jumps out of the surrounding
 *                   break'able scope (the WITH_SCOPE_BEGIN/WITH_SCOPE_END delimited compound statement
 *                   in the above example).
 *
 *               (b) The ON_BREAK block may be paired with a WITH block (much like the ON_EXIT block) to
 *                   provide an error-only execution path. The ON_BREAK block is executed only if a break
 *                   statement is executed inside the associated WITH block. Once the ON_BREAK block's end
 *                   is reached the execution automatically jumps out of the surrounding break'able scope.
 *
 *               (c) The ON_BREAK and ON_EXIT blocks may not be used along with the same WITH block.
 *
 *           (4) The WITH block is executed before the associated ON_EXIT/ON_BREAK block. It may be exited
 *               at any time by executing a break statement. In the above example, the execution will
 *               jump to the ON_EXIT block after the Func() call if Func() fails or after the trace
 *               function call if Func() succeeds.
 *
 *           (5) The BREAK_ON_ERR(statement) macro will make the execution jump out of the surrounding
 *               break'able scope if the error is non-zero.
 ********************************************************************************************************
 *******************************************************************************************************/
//                                                                 Open a new with scope.
#define  WITH_SCOPE_BEGIN(p_err)       do { RTOS_ERR *_p_err = p_err; PP_UNUSED_PARAM(_p_err);

#define  WITH_SCOPE_END                 } while (0);            // Close a with scope.

#define  ON_BREAK                       ON_BREAK_IMPL(; )       // Exec'd after a break in assoc'd WITH blk.

//                                                                 For meta macro creation.
#define  ON_BREAK_IMPL(after_code)                       \
  if (1) {                                               \
    goto PP_UNIQUE_LABEL(on_brk_enter);                  \
    PP_UNIQUE_LABEL(on_brk_brkout) : break;              \
  } else                                                 \
  while (1)                                              \
  if (1) {                                               \
    goto PP_UNIQUE_LABEL(on_brk_cleanup);                \
    PP_UNIQUE_LABEL(on_brk_brkafter) : after_code break; \
  } else                                                 \
  while (1)                                              \
  if (1) {                                               \
    goto PP_UNIQUE_LABEL(on_brk_brkafter);               \
  } else PP_UNIQUE_LABEL(on_brk_enter) :                 \
    if (0) {                                             \
    while (1) {                                          \
      goto PP_UNIQUE_LABEL(on_brk_brkout);               \
      do {                                               \
        PP_UNIQUE_LABEL(on_brk_cleanup) :

//                                                                 Exec'd after assoc'd WITH blk.
#define  ON_EXIT                            \
  if (1) {                                  \
    goto PP_UNIQUE_LABEL(on_exit_enter);    \
    PP_UNIQUE_LABEL(on_exit_brkout) :;      \
  } else                                    \
  while (1)                                 \
  if (1) {                                  \
    goto PP_UNIQUE_LABEL(on_exit_cleanup);  \
  } else                                    \
  while (1)                                 \
  if (1) {                                  \
    goto PP_UNIQUE_LABEL(on_exit_cleanup);  \
  } else PP_UNIQUE_LABEL(on_exit_enter) :   \
    if (0) {                                \
    while (1) {                             \
      goto PP_UNIQUE_LABEL(on_exit_brkout); \
      do {                                  \
        PP_UNIQUE_LABEL(on_exit_cleanup) :

#define  WITH \
}while (0);   \
}             \
}else

//                                                                 Break if err non-zero after statement exec.
#define BREAK_ON_ERR(statement)              \
  if (1) {                                   \
    goto PP_UNIQUE_LABEL(brk_on_err_exec);   \
    PP_UNIQUE_LABEL(brk_on_err_brkout) :     \
      break;                                 \
  } else                                     \
  while (RTOS_ERR_CODE_GET(*_p_err) != 0u)   \
  if (1) {                                   \
    goto PP_UNIQUE_LABEL(brk_on_err_brkout); \
  } else PP_UNIQUE_LABEL(brk_on_err_exec) :  \
    statement

//                                                                 Set err and break.
#define BREAK_ERR_SET(err)      \
  if (1) {                      \
    RTOS_ERR_SET(*_p_err, err); \
    break;                      \
  } else do {} while (0)

#define BREAK_ERR_SET_LOG(err, msg) \
  if (1) {                          \
    LOG_ERR(msg);                   \
    RTOS_ERR_SET(*_p_err, err);     \
    break;                          \
  } else do {} while (0)

//                                                                 Set err and break if cond is false.
#define ASSERT_BREAK(cond, err) \
  if (!(cond)) {                \
    RTOS_ERR_SET(*_p_err, err); \
    break;                      \
  } else do {} while (0)

#define ASSERT_BREAK_LOG(cond, err, msg) \
  if (!(cond)) {                         \
    LOG_ERR(msg);                        \
    RTOS_ERR_SET(*_p_err, err);          \
    break;                               \
  } else do {} while (0)

/********************************************************************************************************
 ********************************************************************************************************
 *                                           MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
