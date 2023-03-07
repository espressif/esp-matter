/***************************************************************************//**
 * @file
 * @brief File System - Storage Operations
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

//                                                                 ------------------------ FS ------------------------
#include  <fs/source/storage/fs_media_priv.h>
#include  <fs/source/storage/fs_blk_dev_priv.h>
#include  <fs/source/storage/fs_storage_priv.h>

//                                                                 ----------------------- EXT ------------------------
#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define    RTOS_MODULE_CUR    RTOS_CFG_MODULE_FS
#define    LOG_DFLT_CH        (FS, STORAGE)

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_ENABLED)
extern const FS_STORAGE_INIT_CFG FSStorage_InitCfg;
#else
const FS_STORAGE_INIT_CFG FSStorage_InitCfgDflt = {
                                                            #if (FS_STORAGE_CFG_MEDIA_POLL_TASK_EN == DEF_ENABLED)
  .PollTaskStkSizeElements = 512u,
  .PollTaskStkPtr = DEF_NULL,
                                                            #endif
  .OnConn = DEF_NULL,
  .OnDisconn = DEF_NULL,
  .MemSegPtr = DEF_NULL,
  .MaxSCSILuCnt = LIB_MEM_BLK_QTY_UNLIMITED
};

FS_STORAGE_INIT_CFG FSStorage_InitCfg = {
                                                            #if (FS_STORAGE_CFG_MEDIA_POLL_TASK_EN == DEF_ENABLED)
  .PollTaskStkSizeElements = 512u,
  .PollTaskStkPtr = DEF_NULL,
                                                            #endif
  .OnConn = DEF_NULL,
  .OnDisconn = DEF_NULL,
  .MemSegPtr = DEF_NULL,
  .MaxSCSILuCnt = LIB_MEM_BLK_QTY_UNLIMITED
};
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       FSStorage_ConfigureMemSeg()
 *
 * @brief    Configure memory segment for the storage module.
 *
 * @param    p_seg   Pointer to a user-defined memory segment.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void FSStorage_ConfigureMemSeg(MEM_SEG *p_seg)
{
  RTOS_ASSERT_DBG(!FSMedia_Data.IsInit, RTOS_ERR_ALREADY_INIT,; );

  FSStorage_InitCfg.MemSegPtr = p_seg;
}
#endif

/****************************************************************************************************//**
 *                                   FSStorage_ConfigureMediaConnCallback()
 *
 * @brief    Set the media connection and disconnection callbacks.
 *
 * @param    on_conn     Callback that will be called whenever a removable media (SD or SCSI) is
 *                       connected.
 *                       [Content MUST be persistent]
 *
 * @param    on_disconn  Callback that will be called whenever a removable media (SD or SCSI) is
 *                       disconnected.
 *                       [Content MUST be persistent]
 *
 * @note     (1) If the connection callback is NOT provided, the poll task will not be started.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void FSStorage_ConfigureMediaConnCallback(FS_MEDIA_CONN_CB on_conn,
                                          FS_MEDIA_CONN_CB on_disconn)
{
  RTOS_ASSERT_DBG(!FSMedia_Data.IsInit, RTOS_ERR_ALREADY_INIT,; );

  FSStorage_InitCfg.OnConn = on_conn;
  FSStorage_InitCfg.OnDisconn = on_disconn;
}
#endif

/****************************************************************************************************//**
 *                                   FSStorage_ConfigureMediaPollTaskStk()
 *
 * @brief    Set the media polling task stack address and stack size.
 *
 * @param    p_stk               Pointer to the start of the stack memory region.
 *
 * @param    stk_size_elements   Size of the stack in stack size elements.
 *******************************************************************************************************/

#if ((RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED) \
  && (FS_STORAGE_CFG_MEDIA_POLL_TASK_EN == DEF_ENABLED))
void FSStorage_ConfigureMediaPollTaskStk(void       *p_stk,
                                         CPU_INT32U stk_size_elements)
{
  RTOS_ASSERT_DBG(!FSMedia_Data.IsInit, RTOS_ERR_ALREADY_INIT,; );

  FSStorage_InitCfg.PollTaskStkPtr = p_stk;
  FSStorage_InitCfg.PollTaskStkSizeElements = stk_size_elements;
}
#endif

/****************************************************************************************************//**
 *                                   FSStorage_ConfigureSCSILogicalUnitCnt()
 *
 * @brief    Configure the (fixed) quantity of SCSI logical units available.
 *
 * @param    max_cnt     Maximum SCSI logical unit available quantity.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void FSStorage_ConfigureMaxSCSILogicalUnitCnt(CPU_SIZE_T max_cnt)
{
  RTOS_ASSERT_DBG(!FSMedia_Data.IsInit, RTOS_ERR_ALREADY_INIT,; );

  FSStorage_InitCfg.MaxSCSILuCnt = max_cnt;
}
#endif

/****************************************************************************************************//**
 *                                               FSStorage_Init()
 *
 * @brief    Initialize the file system storage sub-module.
 *
 * @param    p_err   Pointer to variable that will receive the return error code(s) from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_SEG_OVF
 *******************************************************************************************************/
void FSStorage_Init(RTOS_ERR *p_err)
{
  FSObj_Init(p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  FSMedia_Init(&FSStorage_InitCfg, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  FSBlkDev_Init(p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
}

/****************************************************************************************************//**
 *                                       FSStorage_PollTaskPeriodSet()
 *
 * @brief    Set the media poll task period.
 *
 * @param    period_ms   Interval of time (in milliseconds) between two polling.
 *
 * @param    p_err       Pointer to variable that will receive the return error code(s) from this
 *                       function:
 *                           - RTOS_ERR_NONE
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_MEDIA_POLL_TASK_EN == DEF_ENABLED)
void FSStorage_PollTaskPeriodSet(CPU_INT32U period_ms,
                                 RTOS_ERR   *p_err)
{
  FSMedia_PollTaskPeriodSet(period_ms, p_err);
}
#endif

/****************************************************************************************************//**
 *                                       FSStorage_PollTaskPrioSet()
 *
 * @brief    Set the priority of the media poll task.
 *
 * @param    prio    New priority for the media poll task.
 *
 * @param    p_err   Pointer to variable that will receive the return error code(s) from this function:
 *                       - RTOS_ERR_NONE
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_MEDIA_POLL_TASK_EN == DEF_ENABLED)
void FSStorage_PollTaskPrioSet(RTOS_TASK_PRIO prio,
                               RTOS_ERR       *p_err)
{
  FSMedia_PollTaskPrioSet(prio, p_err);
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_FS_AVAIL
