/***************************************************************************//**
 * @file
 * @brief File System - NAND Media Operations
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

#if (defined(RTOS_MODULE_FS_STORAGE_NAND_AVAIL))

#if (!defined(RTOS_MODULE_FS_AVAIL))

#error NAND module requires File System Storage module. Make sure it is part of your project and that \
  RTOS_MODULE_FS_AVAIL is defined in rtos_description.h.

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------------------ FS ------------------------
#include  <fs_storage_cfg.h>
#include  <fs/source/storage/nand/fs_nand_priv.h>
#include  <fs/source/storage/fs_blk_dev_priv.h>

//                                                                 ----------------------- CPU ------------------------
#include  <cpu/include/cpu.h>

//                                                                 ---------------------- COMMON ----------------------
#include  <common/include/rtos_err.h>
#include  <common/include/platform_mgr.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/logging/logging_priv.h>

#include  <em_core.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  RTOS_MODULE_CUR            RTOS_CFG_MODULE_FS
#define  LOG_DFLT_CH                (FS, DRV, NAND)

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static const FS_NAND_HANDLE FS_NAND_NullHandle = { 0 };

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static FS_MEDIA *FS_NAND_MediaAdd(const FS_MEDIA_PM_ITEM *p_pm_item,
                                  RTOS_ERR               *p_err);

static void FS_NAND_MediaRem(FS_MEDIA *p_media,
                             RTOS_ERR *p_err);

static CPU_SIZE_T FS_NAND_MediaAlignReqGet(FS_MEDIA *p_media,
                                           RTOS_ERR *p_err);

static CPU_BOOLEAN FS_NAND_MediaIsConn(FS_MEDIA *p_media);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

const FS_MEDIA_API FS_NAND_MediaApi = {
  .Add = FS_NAND_MediaAdd,
  .Rem = FS_NAND_MediaRem,
  .AlignReqGet = FS_NAND_MediaAlignReqGet,
  .IsConn = FS_NAND_MediaIsConn
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               FS_NAND_Open()
 *
 * @brief    Open a NAND media.
 *
 * @param    media_handle    Handle to a media.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_INIT
 *                               - RTOS_ERR_IO
 *                               - RTOS_ERR_NOT_FOUND
 *
 * @return   Handle to the opened NAND.
 *******************************************************************************************************/
FS_NAND_HANDLE FS_NAND_Open(FS_MEDIA_HANDLE media_handle,
                            RTOS_ERR        *p_err)
{
  FS_NAND        *p_nand;
  FS_NAND_HANDLE nand_handle;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, FS_NAND_NullHandle);
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  //                                                               Check that Storage layer is initialized.
  CORE_ENTER_ATOMIC();
  if (!FSBlkDev_Data.IsInit) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_INIT);
    CORE_EXIT_ATOMIC();
    return (FS_NAND_NullHandle);
  }
  CORE_EXIT_ATOMIC();

  p_nand = (FS_NAND *)media_handle.MediaPtr;

  RTOS_ASSERT_DBG_ERR_SET(!p_nand->IsOpen, *p_err,
                          RTOS_ERR_INVALID_STATE, FS_NAND_NullHandle);

  p_nand->CtrlrPtr->CtrlrApiPtr->Open(p_nand->CtrlrPtr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (FS_NAND_NullHandle);
  }

  p_nand->IsOpen = DEF_YES;

  nand_handle.MediaHandle = media_handle;

  return (nand_handle);
}

/****************************************************************************************************//**
 *                                               FS_NAND_Close()
 *
 * @brief    Close a NAND media.
 *
 * @param    nand_handle     Handle to a NAND.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *******************************************************************************************************/
void FS_NAND_Close(FS_NAND_HANDLE nand_handle,
                   RTOS_ERR       *p_err)
{
  FS_NAND *p_nand;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  p_nand = (FS_NAND *)nand_handle.MediaHandle.MediaPtr;

  RTOS_ASSERT_DBG_ERR_SET(p_nand->IsOpen, *p_err, RTOS_ERR_INVALID_STATE,; );

  p_nand->CtrlrPtr->CtrlrApiPtr->Close(p_nand->CtrlrPtr);
  p_nand->IsOpen = DEF_NO;
}

/****************************************************************************************************//**
 *                                           FS_NAND_BlkErase()
 *
 * @brief    Erase a physical block.
 *
 * @param    nand_handle     Handle to a NAND instance.
 *
 * @param    blk_ix          Index of the block to erase.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_IO
 *******************************************************************************************************/
void FS_NAND_BlkErase(FS_NAND_HANDLE  nand_handle,
                      FS_NAND_BLK_QTY blk_ix,
                      RTOS_ERR        *p_err)
{
  FS_NAND *p_nand;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  p_nand = (FS_NAND *)nand_handle.MediaHandle.MediaPtr;

  RTOS_ASSERT_DBG_ERR_SET(p_nand->IsOpen, *p_err, RTOS_ERR_INVALID_STATE,; );

  FS_NAND_BlkEraseInternal(p_nand, blk_ix, p_err);
}

/****************************************************************************************************//**
 *                                           FS_NAND_ChipErase()
 *
 * @brief    Erase an entire NAND chip. Erase counts for each block will also be erased, affecting
 *           wear leveling mechanism.
 *
 * @param    nand_handle     Handle to a NAND handle.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from
 *                           this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_IO
 *
 * @note     (1) WARNING: this function will reset erase counts, affecting wear leveling algorithms.
 *               Use this function only in very specific cases when a low-level format does not
 *               suffice.
 *******************************************************************************************************/
void FS_NAND_ChipErase(FS_NAND_HANDLE nand_handle,
                       RTOS_ERR       *p_err)
{
  FS_NAND            *p_nand;
  FS_NAND_PART_PARAM *p_info;
  FS_NAND_BLK_QTY    blk_ix;
  RTOS_ERR           err_rtn;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  RTOS_ERR_SET(err_rtn, RTOS_ERR_NONE);

  p_nand = (FS_NAND *)nand_handle.MediaHandle.MediaPtr;

  RTOS_ASSERT_DBG_ERR_SET(p_nand->IsOpen, *p_err, RTOS_ERR_INVALID_STATE,; );

  p_info = p_nand->CtrlrPtr->CtrlrApiPtr->PartInfoGet(p_nand->CtrlrPtr);

  for (blk_ix = 0u; blk_ix < p_info->BlkCnt; blk_ix++) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    FS_NAND_BlkEraseInternal(p_nand,
                             blk_ix,
                             p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      err_rtn = *p_err;
    }
  }

  *p_err = err_rtn;
}

/****************************************************************************************************//**
 *                                               FS_NAND_Dump()
 *
 * @brief    Dumps the entire raw NAND device in multiple data chunks through a user-supplied callback
 *           function.
 *
 * @param    nand_handle     Handle to a NAND.
 *
 * @param    dump_fnct       Callback that will return the read raw NAND data in chunks.
 *
 * @param    p_buf           Pointer to a buffer long enough to contain a NAND page.
 *
 * @param    first_blk_ix    Index of the block to start dumping at.
 *
 * @param    blk_cnt         Number of blocks to dump.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_IO
 *******************************************************************************************************/

#if (FS_NAND_CFG_DUMP_SUPPORT_EN == DEF_ENABLED)
void FS_NAND_Dump(FS_NAND_HANDLE    nand_handle,
                  FS_NAND_DUMP_FUNC dump_func,
                  void              *p_buf,
                  FS_NAND_BLK_QTY   first_blk_ix,
                  FS_NAND_BLK_QTY   blk_cnt,
                  RTOS_ERR          *p_err)
{
  FS_NAND                *p_nand;
  FS_NAND_CTRLR          *p_ctrlr;
  FS_NAND_PART_PARAM     *p_part_param;
  FS_NAND_BLK_QTY        blk_ix;
  FS_NAND_PG_PER_BLK_QTY pg_ix;
  CPU_BOOLEAN            is_open;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  p_nand = (FS_NAND *)nand_handle.MediaHandle.MediaPtr;
  p_ctrlr = p_nand->CtrlrPtr;

  is_open = p_nand->IsOpen;
  if (!is_open) {
    p_ctrlr->CtrlrApiPtr->Open(p_ctrlr, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
  }

  p_part_param = p_ctrlr->CtrlrApiPtr->PartInfoGet(p_ctrlr);

  for (blk_ix = first_blk_ix; blk_ix < (first_blk_ix + blk_cnt); blk_ix++) {
    LOG_VRB(("Dumping block ", (u)blk_ix));
    for (pg_ix = 0u; pg_ix < p_part_param->PgPerBlk; pg_ix++) {
      //                                                           ------------------- DUMP PG AREA -------------------
      p_ctrlr->CtrlrApiPtr->PgRdRaw(p_ctrlr,
                                    p_buf,
                                    blk_ix * p_part_param->PgPerBlk + pg_ix,
                                    0u,
                                    p_part_param->PgSize,
                                    p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        LOG_ERR(("Error ", RTOS_ERR_LOG_ARG_GET(*p_err), " at blk ", (u)blk_ix, ", reading page ", (u)pg_ix));
        return;
      }

      dump_func(p_buf,
                p_part_param->PgSize,
                p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        LOG_ERR(("Call to 'dump_fnct' failed w/err ", RTOS_ERR_LOG_ARG_GET(*p_err), "."));
        return;
      }

      //                                                           ----------------- DUMP SPARE AREA ------------------
      p_ctrlr->CtrlrApiPtr->SpareRdRaw(p_ctrlr,
                                       p_buf,
                                       blk_ix * p_part_param->PgPerBlk + pg_ix,
                                       0u,
                                       p_part_param->SpareSize,
                                       p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        LOG_ERR(("Error ", RTOS_ERR_LOG_ARG_GET(*p_err), " at blk ", (u)blk_ix, ", reading page ", (u)pg_ix, " (spare)."));
        return;
      }

      dump_func(p_buf,
                p_part_param->SpareSize,
                p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        LOG_ERR(("Call to 'dump_fnct' failed w/err ", RTOS_ERR_LOG_ARG_GET(*p_err), "."));
        return;
      }
    }
  }

  if (!is_open) {
    p_ctrlr->CtrlrApiPtr->Close(p_ctrlr);
  }
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       FS_NAND_BlkEraseInternal()
 *
 * @brief    Erase a block from device.
 *
 * @param    p_ctrlr     Pointer to a NAND controller instance.
 *
 * @param    blk_ix_phy  Index of the block to be erased.
 *
 * @param    p_err       Error pointer.
 *
 * @note     (1) Erased blocks must have their erase count saved in the available blocks table prior
 *               to calling this function.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FS_NAND_BlkEraseInternal(FS_NAND         *p_nand,
                              FS_NAND_BLK_QTY blk_ix_phy,
                              RTOS_ERR        *p_err)
{
  LOG_VRB(("Erase block ", (u)blk_ix_phy));

  p_nand->CtrlrPtr->CtrlrApiPtr->BlkErase(p_nand->CtrlrPtr,
                                          blk_ix_phy,
                                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Error erasing block ", (u)blk_ix_phy));
    return;
  }
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           FS_NAND_AddInternal()
 *
 * @brief    Add a NAND instance.
 *
 * @param    p_pm_item   Pointer to a media platform manager item.
 *
 * @param    p_err       Error pointer.
 *
 * @return   Pointer to the allocated media (FS_NAND).
 *******************************************************************************************************/
static FS_MEDIA *FS_NAND_MediaAdd(const FS_MEDIA_PM_ITEM *p_pm_item,
                                  RTOS_ERR               *p_err)
{
  FS_NAND         *p_nand;
  FS_NAND_PM_ITEM *p_nand_pm_item;

  if (p_pm_item->PmItem.Type != PLATFORM_MGR_ITEM_TYPE_HW_INFO_FS_NAND) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_TYPE);
    return (DEF_NULL);
  }

  p_nand_pm_item = (FS_NAND_PM_ITEM *)p_pm_item;

  p_nand = (FS_NAND *)Mem_SegAlloc("FS - NAND instance",
                                   FSMedia_InitCfgPtr->MemSegPtr,
                                   sizeof(FS_NAND),
                                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  p_nand->CtrlrPtr = p_nand_pm_item->HwInfoPtr->CtrlrHwInfoPtr->CtrlrApiPtr->Add(p_nand_pm_item,
                                                                                 FSMedia_InitCfgPtr->MemSegPtr,
                                                                                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  p_nand->IsOpen = DEF_NO;

  return (&p_nand->Media);
}

/****************************************************************************************************//**
 *                                               FS_NAND_Rem()
 *
 * @brief    Remove a NAND instance (not implemented).
 *
 * @param    p_media     Pointer to a media.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_NAND_MediaRem(FS_MEDIA *p_media,
                             RTOS_ERR *p_err)
{
  PP_UNUSED_PARAM(p_media);
  PP_UNUSED_PARAM(p_err);

  RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_NOT_SUPPORTED,; );
}

/****************************************************************************************************//**
 *                                       FS_NAND_MediaAlignReqGet()
 *
 * @brief    Get the alignment requirement for a media instance.
 *
 * @param    p_media     Pointer to a NAND media instance.
 *
 * @param    p_err       Error pointer.
 *
 * @return   Alignment requirement.
 *******************************************************************************************************/
static CPU_SIZE_T FS_NAND_MediaAlignReqGet(FS_MEDIA *p_media,
                                           RTOS_ERR *p_err)
{
  FS_NAND_PM_ITEM *p_nand_pm_item;

  PP_UNUSED_PARAM(p_err);

  p_nand_pm_item = (FS_NAND_PM_ITEM *)p_media->PmItemPtr;

  return (p_nand_pm_item->HwInfoPtr->CtrlrHwInfoPtr->AlignReq);
}

/****************************************************************************************************//**
 *                                           FS_NAND_MediaIsConn()
 *
 * @brief    Verify whether a NAND media is connected.
 *
 * @param    p_media     Pointer to a media instance.
 *
 * @return   DEF_YES (since it's a non-removable media).
 *******************************************************************************************************/
static CPU_BOOLEAN FS_NAND_MediaIsConn(FS_MEDIA *p_media)
{
  PP_UNUSED_PARAM(p_media);

  return (DEF_YES);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_FS_STORAGE_NAND_AVAIL
