/***************************************************************************//**
 * @file
 * @brief Common - Operation Locks (Oplocks)
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

#include  <common/source/op_lock/op_lock_priv.h>
#include  <common/include/lib_utils.h>
#include  <common/source/rtos/rtos_utils_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  OP_LOCK_OPS_QUEUED_NONE            DEF_BIT_NONE
#define  OP_LOCK_OPS_QUEUED_IS_ACQUIRE      DEF_GET_U_MAX_VAL(OP_LOCK_BITMAP)

#define  OP_LOCK_DBG_EN                     DEF_DISABLED

#define  LOG_DFLT_CH                       (COMMON, OP_LOCK)
#define  RTOS_MODULE_CUR                    RTOS_CFG_MODULE_COMMON

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef struct op_lock_data {                                   // ------------------- OP LOCK DATA -------------------
  OP_LOCK_OP_DESC *OpDescPtr;                                   // Ptr to op desc op ptr.
  OP_LOCK_BITMAP  OpsQd;                                        // Variable containing statuses for a single eval.
} OP_LOCK_DATA;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static KAL_MON_RES OpLockEnter(void *p_mon_data,
                               void *p_data);

static KAL_MON_RES OpLockExit(void *p_mon_data,
                              void *p_data);

static KAL_MON_RES OpLockEval(void *p_mon_data,
                              void *p_eval_op_data,
                              void *p_scan_op_data);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               OpLockCreate()
 *
 * @brief    Initialize an operation lock.
 *
 * @param    p_op_lock   Pointer to operation lock structure to initialize.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                           - RTOS_ERR_POOL_EMPTY
 *                           - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                           - RTOS_ERR_SEG_OVF
 *******************************************************************************************************/
void OpLockCreate(OP_LOCK  *p_op_lock,
                  RTOS_ERR *p_err)
{
  RTOS_ASSERT_DBG_ERR_SET((p_op_lock != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  p_op_lock->OpCnt = DEF_BIT_NONE;

  p_op_lock->Mon = KAL_MonCreate("Op-Lock Mon",
                                 DEF_NULL,
                                 p_op_lock,
                                 DEF_NULL,
                                 p_err);
}

/****************************************************************************************************//**
 *                                               OpLockAcquire()
 *
 * @brief    Acquire an operation lock for a given operation.
 *
 * @param    p_op_lock   Pointer to operation lock structure.
 *
 * @param    p_op_desc   Pointer to operation lock operation description structure.
 *
 * @param    timeout_ms  TImeout, in milliseconds, to pend if lock is not available.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_OS_OBJ_DEL
 *                           - RTOS_ERR_ABORT
 *                           - RTOS_ERR_TIMEOUT
 *******************************************************************************************************/
void OpLockAcquire(OP_LOCK         *p_op_lock,
                   OP_LOCK_OP_DESC *p_op_desc,
                   CPU_INT32U      timeout_ms,
                   RTOS_ERR        *p_err)
{
  OP_LOCK_DATA op_lock_data;

  RTOS_ASSERT_DBG_ERR_SET((p_op_lock != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  op_lock_data.OpDescPtr = p_op_desc;
  op_lock_data.OpsQd = OP_LOCK_OPS_QUEUED_IS_ACQUIRE;

  KAL_MonOp(p_op_lock->Mon,
            (void *)&op_lock_data,
            OpLockEnter,
            OpLockEval,
            KAL_OPT_NONE,
            timeout_ms,
            p_err);
}

/****************************************************************************************************//**
 *                                               OpLockRelease()
 *
 * @brief    Release an operation lock for a given operation.
 *
 * @param    p_op_lock   Pointer to operation lock structure.
 *
 * @param    p_op_desc   Pointer to operation lock operation description structure. It should be
 *                       the same as for the call to Common_OpLockAcquire().
 *
 * @note     (1) This variable is used throughout the evaluation function to accumulate pending
 *               operations that are at a higher priority than the one being evaluated.
 *******************************************************************************************************/
void OpLockRelease(OP_LOCK         *p_op_lock,
                   OP_LOCK_OP_DESC *p_op_desc)
{
  OP_LOCK_DATA op_lock_data;
  RTOS_ERR     err;

  RTOS_ASSERT_DBG((p_op_lock != DEF_NULL), RTOS_ERR_NULL_PTR,; );

  op_lock_data.OpDescPtr = p_op_desc;
  op_lock_data.OpsQd = OP_LOCK_OPS_QUEUED_NONE;                 // Init var to accumulate q'd ops. See Note #1.

  KAL_MonOp(p_op_lock->Mon,
            (void *)&op_lock_data,
            OpLockExit,
            DEF_NULL,
            KAL_OPT_NONE,
            0u,
            &err);
  //                                                               No error can be obtained, in this context.
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}

/****************************************************************************************************//**
 *                                               OpLockDel()
 *
 * @brief    Delete/free an operation lock.
 *
 * @param    p_op_lock   Pointer to operation lock structure to delete/free.
 *
 * @return   DEF_OK,   if lock was deleted/freed correctly,
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
void OpLockDel(OP_LOCK *p_op_lock)
{
  RTOS_ASSERT_DBG((p_op_lock != DEF_NULL), RTOS_ERR_NULL_PTR,; );

  KAL_MonDel(p_op_lock->Mon);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               OpLockEnter()
 *
 * @brief    On enter callback when acquiring an operation lock.
 *
 * @param    p_mon_data  Pointer to data passed to KAL_MonCreate(). In this case, a pointer to the
 *                       OP_LOCK structure used.
 *
 * @param    p_data      Pointer to data passed to KAL_MonOp(). In this case, the
 *                       OP_LOCK_DATA on the caller's stack.
 *
 * @return   KAL_MON_RES_BLOCK,     if task must     be blocked,
 *           KAL_MON_RES_STOP_EVAL, if task must not be blocked and the evaluations must be stopped.
 *
 * @note     (1) If any operation is currently holding op lock, operation must be initially blocked,
 *               so that they are added in the OS's pending tasks list. This allows for priority
 *               accounting since the pending list is sorted by priority (see OpLockEval() notes).
 *******************************************************************************************************/
static KAL_MON_RES OpLockEnter(void *p_mon_data,
                               void *p_data)
{
  OP_LOCK        *p_op_lock = (OP_LOCK *)p_mon_data;
  OP_LOCK_BITMAP op_desc_mask = ((OP_LOCK_DATA *)p_data)->OpDescPtr->Mask;
  KAL_MON_RES    res;

  if (p_op_lock->OpCnt == 0u) {
    //                                                             If no other op is cur holding lock,  do not ...
    //                                                             ... block and no need to eval.

    //                                                             Inc op cnt.
    p_op_lock->OpCnt += ((~op_desc_mask + 1u) & op_desc_mask);

    res = KAL_MON_RES_STOP_EVAL;
  } else {                                                      // Else block cur task and start eval to be added ...
                                                                // ... to pend list.
    res = KAL_MON_RES_BLOCK;
  }

  return (res);
}

/****************************************************************************************************//**
 *                                               OpLockExit()
 *
 * @brief    On enter callback when releasing an operation lock.
 *
 * @param    p_mon_data  Pointer to data passed to KAL_MonCreate(). In this case, a pointer to the
 *                       OP_LOCK structure used.
 *
 * @param    p_data      Pointer to data passed to KAL_MonOp(). In this case, the
 *                       OP_LOCK_DATA on the caller's stack.
 *
 * @return   KAL_MON_RES_ALLOW,     if the task must run and the evaluation must be continued.
 *           KAL_MON_RES_STOP_EVAL, if the task must run and the evaluation must be stopped.
 *
 * @note     (1) No need to reevaluate predicates if the operation count is not zero (i.e. no
 *               remaining operations of the same nature is ongoing) or max - 1u (i.e. op cnt was
 *               at max value but has decreased, potentially allowing another op of the same nature to
 *               execute).
 *******************************************************************************************************/
static KAL_MON_RES OpLockExit(void *p_mon_data,
                              void *p_data)
{
  OP_LOCK        *p_op_lock = (OP_LOCK *)p_mon_data;
  OP_LOCK_BITMAP op_desc_mask = ((OP_LOCK_DATA *)p_data)->OpDescPtr->Mask;
  KAL_MON_RES    res;

  p_op_lock->OpCnt -= ((~op_desc_mask + 1u) & op_desc_mask);    // Dec op cnt.

  //                                                               Chk if predicates need to be evaluated (See Note #1)
  if ((DEF_BIT_IS_SET_ANY(p_op_lock->OpCnt, op_desc_mask) == DEF_NO)
      || ((p_op_lock->OpCnt) == (op_desc_mask - ((~op_desc_mask + 1u) & op_desc_mask)))) {
    res = KAL_MON_RES_ALLOW;                                    // Do not block cur task, it exits. Need to eval ...
                                                                // ... other pending tasks.
  } else {
    res = KAL_MON_RES_STOP_EVAL;                                // Else do not block cur task, but no need to eval.
  }

  return (res);
}

/****************************************************************************************************//**
 *                                               OpLockEval()
 *
 * @brief    On evaluation callback when evaluation if an operation lock can be allowed to execute.
 *
 * @param    p_mon_data      Pointer to data passed to KAL_MonCreate(). In this case, a pointer to the
 *                           OP_LOCK structure used.
 *
 * @param    p_eval_op_data  Pointer to data passed to KAL_MonOp(). In this case, the
 *                           OP_LOCK_DATA on one of the previous caller's stack.
 *
 * @param    p_scan_op_data  Pointer to data passed to KAL_MonOp(). In this case, the
 *                           OP_LOCK_DATA on the caller's stack.
 *
 * @return   A combination of these flags:
 *               - KAL_MON_RES_ALLOW,     if task must be allowed to run.
 *               - KAL_MON_RES_BLOCK,     if task must be blocked.
 *               - KAL_MON_RES_STOP_EVAL, if evaluation must be stopped.
 *******************************************************************************************************/
static KAL_MON_RES OpLockEval(void *p_mon_data,
                              void *p_eval_op_data,
                              void *p_scan_op_data)
{
  OP_LOCK         *p_op_lock = (OP_LOCK *)p_mon_data;
  OP_LOCK_OP_DESC *p_scan_op_desc = ((OP_LOCK_DATA *)p_scan_op_data)->OpDescPtr;
  OP_LOCK_OP_DESC *p_eval_op_desc = ((OP_LOCK_DATA *)p_eval_op_data)->OpDescPtr;
  OP_LOCK_BITMAP  *p_scan_ops_qd = &((OP_LOCK_DATA *)p_scan_op_data)->OpsQd;
  KAL_MON_RES     res;

  if (*p_scan_ops_qd == OP_LOCK_OPS_QUEUED_IS_ACQUIRE) {
    //                                                             Caller is acquiring lock.
    if (p_eval_op_data == p_scan_op_data) {                     // Cur evaluating caller task.
      if ((DEF_BIT_IS_SET_ANY(p_op_lock->OpCnt, p_scan_op_desc->Policy) == DEF_NO)
          && (DEF_BIT_IS_CLR_ANY(p_op_lock->OpCnt, p_scan_op_desc->Mask) == DEF_YES)) {
        //                                                         If OpCnt allows exec of cur task oper's policy ...
        //                                                         ... and the mask allows one more oper of this type.

        //                                                         Inc OpCnt for this op.
        p_op_lock->OpCnt += ((~p_scan_op_desc->Mask + 1u) & p_scan_op_desc->Mask);
        //                                                         And un-block cur task. No need to cont eval.
        res = KAL_MON_RES_STOP_EVAL;
      } else {                                                  // Op is not allowed or is already max.
                                                                // Block cur task and stop eval.
        res = (KAL_MON_RES_STOP_EVAL | KAL_MON_RES_BLOCK);
      }
    } else if (DEF_BIT_IS_SET_ANY(p_scan_op_desc->Policy, p_eval_op_desc->Mask) == DEF_YES) {
      //                                                           If other task with higher prio than caller ...
      //                                                           ... (since we are in this if and not in the one ...
      //                                                           ... above) conflicts with caller op, stop eval ...
      //                                                           ... continue blocking evaluated task.
      res = (KAL_MON_RES_STOP_EVAL | KAL_MON_RES_BLOCK);
    } else {                                                    // If other task with higher prio than caller       ...
                                                                // ... (since we are in this if and not in the one  ...
                                                                // ... above) does not conflict with caller op,     ...
                                                                // ... continue eval and continue blocking eval'd task.
      res = KAL_MON_RES_BLOCK;
    }
  } else {                                                      // Caller is releasing lock.
    if ((DEF_BIT_IS_SET_ANY(p_op_lock->OpCnt, p_eval_op_desc->Policy) == DEF_NO)
        && (DEF_BIT_IS_CLR_ANY(p_op_lock->OpCnt, p_eval_op_desc->Mask) == DEF_YES)
        && (DEF_BIT_IS_SET_ANY(*p_scan_ops_qd, p_eval_op_desc->Policy) == DEF_NO)) {
      //                                                           If OpCnt allows exec of cur task oper's policy,  ...
      //                                                           ... that the mask allows one more oper of this   ...
      //                                                           ... type and that no ops previously eval'd for   ...
      //                                                           ... this release call conflict with policy of    ...
      //                                                           ... oper of task being eval'd, op is allowed.
      //                                                           Inc OpCnt for this op.
      p_op_lock->OpCnt += ((~p_eval_op_desc->Mask + 1u) & p_eval_op_desc->Mask);

      res = KAL_MON_RES_ALLOW;
    } else {                                                    // Op is still not allowed.
                                                                // Add oper mask of evaluated task to the bitmap of ...
                                                                // ... q'd ops associated to the whole eval for     ...
                                                                // ... this release call.
      *p_scan_ops_qd |= p_eval_op_desc->Mask;
      //                                                           Block cur eval task but continue eval. Other ...
      //                                                           ... tasks could be able to run.
      res = KAL_MON_RES_BLOCK;
    }
  }

  return (res);
}
