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
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include <rtos_description.h>

#if (defined(RTOS_MODULE_FS_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ----------------------- CFG ------------------------
#include  <fs_core_cfg.h>

#if (FS_CORE_CFG_THREAD_SAFETY_EN == DEF_ENABLED)
//                                                                 ------------------------ FS ------------------------
#include  <fs/source/core/fs_core_op_priv.h>
#include  <common/source/rtos/rtos_utils_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH         (FS, CORE, OP)
#define  RTOS_MODULE_CUR     RTOS_CFG_MODULE_FS

/********************************************************************************************************
 *                                   OP LOCK DESCRIPTOR DEFINITIONS
 *******************************************************************************************************/

#define  FS_VOL_OP_FILE_WR_MASK                 (DEF_BIT_00)
#define  FS_VOL_OP_FILE_RD_MASK                 (DEF_BIT_01 | DEF_BIT_02)
#define  FS_VOL_OP_ENTRY_QUERY_MASK             (DEF_BIT_03 | DEF_BIT_04)
#define  FS_VOL_OP_ENTRY_OPEN_MASK              (DEF_BIT_05)
#define  FS_VOL_OP_ENTRY_CLOSE_MASK             (DEF_BIT_06 | DEF_BIT_07)
#define  FS_VOL_OP_ENTRY_CREATE_DEL_MASK        (DEF_BIT_08)
#define  FS_VOL_OP_DATA_ACCESS_MASK             (DEF_BIT_09)

#define  FS_VOL_OP_FILE_WR_POLICY               (FS_VOL_OP_ENTRY_CREATE_DEL_MASK \
                                                 | FS_VOL_OP_FILE_RD_MASK        \
                                                 | FS_VOL_OP_FILE_WR_MASK)
#define  FS_VOL_OP_FILE_RD_POLICY               (FS_VOL_OP_ENTRY_CREATE_DEL_MASK \
                                                 | FS_VOL_OP_FILE_WR_MASK)
#define  FS_VOL_OP_ENTRY_QUERY_POLICY           (FS_VOL_OP_ENTRY_CREATE_DEL_MASK)
#define  FS_VOL_OP_ENTRY_OPEN_POLICY            (FS_VOL_OP_ENTRY_CREATE_DEL_MASK \
                                                 | FS_VOL_OP_ENTRY_CLOSE_MASK    \
                                                 | FS_VOL_OP_ENTRY_OPEN_MASK)
#define  FS_VOL_OP_ENTRY_CLOSE_POLICY           (FS_VOL_OP_ENTRY_CREATE_DEL_MASK \
                                                 | FS_VOL_OP_ENTRY_OPEN_MASK)
#define  FS_VOL_OP_ENTRY_CREATE_DEL_POLICY      (FS_VOL_OP_ENTRY_CREATE_DEL_MASK \
                                                 | FS_VOL_OP_ENTRY_CLOSE_MASK    \
                                                 | FS_VOL_OP_ENTRY_OPEN_MASK)
#define  FS_VOL_OP_DATA_ACCESS_POLICY           (FS_VOL_OP_DATA_ACCESS_MASK)

/********************************************************************************************************
 *                                   GLOBAL OPERATIONS LOCKS DEFINES
 *******************************************************************************************************/

#define  FS_GLOBAL_OP_OPEN_VOL_LIST_RD_MASK         (DEF_BIT_00   \
                                                     | DEF_BIT_01 \
                                                     | DEF_BIT_02 \
                                                     | DEF_BIT_03)
#define  FS_GLOBAL_OP_OPEN_VOL_LIST_WR_MASK         (DEF_BIT_04)
#define  FS_GLOBAL_OP_OPEN_ENTRY_LIST_WR_MASK       (DEF_BIT_05)
#define  FS_GLOBAL_OP_OPEN_ENTRY_LIST_RD_MASK       (DEF_BIT_06 | DEF_BIT_07)

#define  FS_GLOBAL_OP_OPEN_VOL_LIST_RD_POLICY       (FS_GLOBAL_OP_OPEN_VOL_LIST_WR_MASK)
#define  FS_GLOBAL_OP_OPEN_VOL_LIST_WR_POLICY       (FS_GLOBAL_OP_OPEN_VOL_LIST_WR_MASK \
                                                     | FS_GLOBAL_OP_OPEN_VOL_LIST_RD_MASK)
#define  FS_GLOBAL_OP_OPEN_ENTRY_LIST_WR_POLICY     (FS_GLOBAL_OP_OPEN_ENTRY_LIST_WR_MASK \
                                                     | FS_GLOBAL_OP_OPEN_ENTRY_LIST_RD_MASK)
#define  FS_GLOBAL_OP_OPEN_ENTRY_LIST_RD_POLICY     (FS_GLOBAL_OP_OPEN_ENTRY_LIST_WR_MASK)

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       VOLUME OPERATIONS LOCKS
 *******************************************************************************************************/

const OP_LOCK_OP_DESC FS_VOL_OP_FILE_WR =
{
  .Policy = FS_VOL_OP_FILE_WR_POLICY,
  .Mask   = FS_VOL_OP_FILE_WR_MASK
};
const OP_LOCK_OP_DESC FS_VOL_OP_FILE_RD =
{
  .Policy = FS_VOL_OP_FILE_RD_POLICY,
  .Mask   = FS_VOL_OP_FILE_RD_MASK
};
const OP_LOCK_OP_DESC FS_VOL_OP_ENTRY_QUERY =
{
  .Policy = FS_VOL_OP_ENTRY_QUERY_POLICY,
  .Mask   = FS_VOL_OP_ENTRY_QUERY_MASK
};
const OP_LOCK_OP_DESC FS_VOL_OP_ENTRY_OPEN =
{
  .Policy = FS_VOL_OP_ENTRY_OPEN_POLICY,
  .Mask   = FS_VOL_OP_ENTRY_OPEN_MASK
};
const OP_LOCK_OP_DESC FS_VOL_OP_ENTRY_CLOSE =
{
  .Policy = FS_VOL_OP_ENTRY_CLOSE_POLICY,
  .Mask   = FS_VOL_OP_ENTRY_CLOSE_MASK
};
const OP_LOCK_OP_DESC FS_VOL_OP_ENTRY_CREATE_DEL =
{
  .Policy = FS_VOL_OP_ENTRY_CREATE_DEL_POLICY,
  .Mask   = FS_VOL_OP_ENTRY_CREATE_DEL_MASK
};
const OP_LOCK_OP_DESC FS_VOL_OP_DATA_ACCESS =
{
  .Policy = FS_VOL_OP_DATA_ACCESS_POLICY,
  .Mask   = FS_VOL_OP_DATA_ACCESS_MASK
};

/********************************************************************************************************
 *                                       GLOBAL OPERATIONS LOCKS
 *******************************************************************************************************/

const OP_LOCK_OP_DESC FS_GLOBAL_OP_OPEN_VOL_LIST_RD =
{
  .Policy = FS_GLOBAL_OP_OPEN_VOL_LIST_RD_POLICY,
  .Mask   = FS_GLOBAL_OP_OPEN_VOL_LIST_RD_MASK
};

const OP_LOCK_OP_DESC FS_GLOBAL_OP_OPEN_VOL_LIST_WR =
{
  .Policy = FS_GLOBAL_OP_OPEN_VOL_LIST_WR_POLICY,
  .Mask   = FS_GLOBAL_OP_OPEN_VOL_LIST_WR_MASK
};

const OP_LOCK_OP_DESC FS_GLOBAL_OP_OPEN_ENTRY_LIST_WR =
{
  .Policy = FS_GLOBAL_OP_OPEN_ENTRY_LIST_WR_POLICY,
  .Mask   = FS_GLOBAL_OP_OPEN_ENTRY_LIST_WR_MASK
};

const OP_LOCK_OP_DESC FS_GLOBAL_OP_OPEN_ENTRY_LIST_RD =
{
  .Policy = FS_GLOBAL_OP_OPEN_ENTRY_LIST_RD_POLICY,
  .Mask   = FS_GLOBAL_OP_OPEN_ENTRY_LIST_RD_MASK
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           FSOp_LockAcquire()
 *
 * @brief    Acquire a core operation lock.
 *
 * @param    p_op_lock       Pointer to a core operation lock instance.
 *
 * @param    p_op_lock_desc  Pointer to a core operation lock descriptor.
 *******************************************************************************************************/
void FSOp_LockAcquire(OP_LOCK               *p_op_lock,
                      const OP_LOCK_OP_DESC *p_op_lock_desc)
{
  RTOS_ERR err;

  RTOS_ERR_SET(err, RTOS_ERR_NONE);

  OpLockAcquire(p_op_lock, (OP_LOCK_OP_DESC *)p_op_lock_desc, 0u, &err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}

/****************************************************************************************************//**
 *                                           FSOp_LockRelease()
 *
 * @brief    Release a core operation lock.
 *
 * @param    p_op_lock       Pointer to a core operation lock instance.
 *
 * @param    p_op_lock_desc  Pointer to a core operation lock descriptor.
 *******************************************************************************************************/
void FSOp_LockRelease(OP_LOCK               *p_op_lock,
                      const OP_LOCK_OP_DESC *p_op_lock_desc)
{
  OpLockRelease(p_op_lock, (OP_LOCK_OP_DESC *)p_op_lock_desc);
}

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_FS_AVAIL
