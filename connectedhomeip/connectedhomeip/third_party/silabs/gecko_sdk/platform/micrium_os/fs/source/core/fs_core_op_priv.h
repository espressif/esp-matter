/***************************************************************************//**
 * @file
 * @brief File System - Core Locking Operations
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

#ifndef  FS_CORE_OP_PRIV_H
#define  FS_CORE_OP_PRIV_H

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ----------------------- EXT ------------------------
#include  <cpu/include/cpu.h>
#include  <common/include/kal.h>
#include  <common/source/collections/slist_priv.h>
#include  <common/include/rtos_err.h>
#include  <common/source/op_lock/op_lock_priv.h>
#include  <common/include/rtos_err.h>

//                                                                 ------------------------ FS ------------------------
#include  <fs/source/shared/fs_utils_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               EXTERNS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           VOLUME OPERATIONS
 *******************************************************************************************************/

extern const OP_LOCK_OP_DESC  FS_VOL_OP_FILE_WR;
extern const OP_LOCK_OP_DESC  FS_VOL_OP_FILE_RD;
extern const OP_LOCK_OP_DESC  FS_VOL_OP_ENTRY_QUERY;
extern const OP_LOCK_OP_DESC  FS_VOL_OP_ENTRY_OPEN;
extern const OP_LOCK_OP_DESC  FS_VOL_OP_ENTRY_CLOSE;
extern const OP_LOCK_OP_DESC  FS_VOL_OP_ENTRY_CREATE_DEL;
extern const OP_LOCK_OP_DESC  FS_VOL_OP_DATA_ACCESS;

/********************************************************************************************************
 *                                           GLOBAL OPERATIONS
 *******************************************************************************************************/

extern const OP_LOCK_OP_DESC FS_GLOBAL_OP_OPEN_VOL_LIST_RD;
extern const OP_LOCK_OP_DESC FS_GLOBAL_OP_OPEN_VOL_LIST_WR;
extern const OP_LOCK_OP_DESC FS_GLOBAL_OP_OPEN_ENTRY_LIST_WR;
extern const OP_LOCK_OP_DESC FS_GLOBAL_OP_OPEN_ENTRY_LIST_RD;

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MACRO'S
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                          OPERATIONS LOCKS
 *******************************************************************************************************/

#if (FS_CORE_CFG_THREAD_SAFETY_EN == DEF_ENABLED)
#define  FS_OP_LOCK_WITH(p_op_lock, op_desc)      \
  if (1) {                                        \
    FSOp_LockAcquire(p_op_lock, &(op_desc));      \
    goto PP_UNIQUE_LABEL(fs_op_lock_with_enter);  \
  } else PP_UNIQUE_LABEL(fs_op_lock_with_enter) : \
    ON_EXIT {                                     \
    FSOp_LockRelease(p_op_lock, &(op_desc));      \
  } WITH

#define  FS_OP_LOCK_ACQUIRE(p_op_lock, op_desc)         FSOp_LockAcquire(p_op_lock, &(op_desc))

#define  FS_OP_LOCK_RELEASE(p_op_lock, op_desc)         FSOp_LockRelease(p_op_lock, &(op_desc))

#else
#define  FS_OP_LOCK_WITH(p_op_lock, op_desc)

#define  FS_OP_LOCK_ACQUIRE(p_op_lock, op_desc)

#define  FS_OP_LOCK_RELEASE(p_op_lock, op_desc)
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void  FSOp_LockAcquire(OP_LOCK          *p_op_lock,
                       const OP_LOCK_OP_DESC  *p_op_lock_desc);

void  FSOp_LockRelease(OP_LOCK          *p_op_lock,
                       const OP_LOCK_OP_DESC  *p_op_lock_desc);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/
#endif
