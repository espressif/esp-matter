/***************************************************************************//**
 * @file
 * @brief File System - NOR Media Operations
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

#if (defined(RTOS_MODULE_FS_STORAGE_NOR_AVAIL))

#if (!defined(RTOS_MODULE_FS_AVAIL))

#error NOR module requires File System Storage module. Make sure it is part of your project and that \
  RTOS_MODULE_FS_AVAIL is defined in rtos_description.h.

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------------------ FS ------------------------
#include  <fs/source/shared/fs_utils_priv.h>
#include  <fs/source/storage/nor/fs_nor_priv.h>
#include  <fs/source/shared/cleanup/cleanup_mgmt_priv.h>

//                                                                 ----------------------- EXT ------------------------
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <cpu/include/cpu.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH          (FS, DRV, NOR)
#define  RTOS_MODULE_CUR      RTOS_CFG_MODULE_FS

/********************************************************************************************************
 ********************************************************************************************************
 *                                               VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

const FS_NOR_HANDLE FS_NOR_NullHandle = { 0 };

/********************************************************************************************************
 ********************************************************************************************************
 *                                       MEDIA API FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static FS_MEDIA *FS_NOR_MediaAdd(const FS_MEDIA_PM_ITEM *p_media_pm_item,
                                 RTOS_ERR               *p_err);

static void FS_NOR_MediaRem(FS_MEDIA *p_media,
                            RTOS_ERR *p_err);

static CPU_BOOLEAN FS_NOR_MediaIsConn(FS_MEDIA *p_media);

static CPU_SIZE_T FS_NOR_MediaAlignReqGet(FS_MEDIA *p_media,
                                          RTOS_ERR *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

const FS_MEDIA_API FS_NOR_MediaApi = {
  .Add = FS_NOR_MediaAdd,
  .Rem = FS_NOR_MediaRem,
  .AlignReqGet = FS_NOR_MediaAlignReqGet,
  .IsConn = FS_NOR_MediaIsConn,
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               FS_NOR_Open()
 *
 * @brief    Open a NOR for raw access.
 *
 * @param    media_handle    Handle to a media.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_INIT
 *                               - RTOS_ERR_NOT_FOUND
 *                               - RTOS_ERR_MEDIA_CLOSED
 *                               - RTOS_ERR_IO
 *
 * @return   Handle to the opened NOR.
 *******************************************************************************************************/
FS_NOR_HANDLE FS_NOR_Open(FS_MEDIA_HANDLE media_handle,
                          RTOS_ERR        *p_err)
{
  FS_NOR        *p_nor;
  FS_NOR_PHY    *p_nor_phy;
  FS_NOR_HANDLE nor_handle;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, FS_NOR_NullHandle);
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  //                                                               Check that Storage layer is initialized.
  CORE_ENTER_ATOMIC();
  if (!FSBlkDev_Data.IsInit) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_INIT);
    CORE_EXIT_ATOMIC();
    return (FS_NOR_NullHandle);
  }
  CORE_EXIT_ATOMIC();

  nor_handle = FS_NOR_NullHandle;
  p_nor = (FS_NOR *)media_handle.MediaPtr;
  p_nor_phy = p_nor->PhyPtr;

  //                                                               Open PHY.
  p_nor_phy->PhyApiPtr->Open(p_nor_phy, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (FS_NOR_NullHandle);
  }

  p_nor->IsOpen = DEF_YES;

  nor_handle.MediaHandle = media_handle;

  return (nor_handle);
}

/****************************************************************************************************//**
 *                                           FS_NOR_Close()
 *
 * @brief   Close a NOR.
 *
 * @param   nor_handle          Handle to a NOR.
 *
 * @param   p_err               Pointer to variable that will receive the return error code(s) from this
 *                              function:
 *                                  - RTOS_ERR_NONE
 *                                  - RTOS_ERR_IO
 *******************************************************************************************************/
void FS_NOR_Close(FS_NOR_HANDLE nor_handle,
                  RTOS_ERR      *p_err)
{
  FS_NOR     *p_nor_media;
  FS_NOR_PHY *p_nor_phy;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  p_nor_media = (FS_NOR *)nor_handle.MediaHandle.MediaPtr;
  p_nor_phy = p_nor_media->PhyPtr;

  p_nor_phy->PhyApiPtr->Close(p_nor_phy, p_err);
}

/****************************************************************************************************//**
 *                                               FS_NOR_Rd()
 *
 * @brief    Read from a NOR.
 *
 * @param    nor_handle  Handle to a NOR.
 *
 * @param    p_dest      Pointer to destination buffer.
 *
 * @param    start_addr  Start address of read (see Note #1).
 *
 * @param    cnt         Number of octets to read.
 *
 * @param    p_err       Pointer to variable that will receive return the error code(s) from this
 *                       function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_IO
 *
 * @note     (1) Start address represents the flash device physical address from which the read must
 *               start. For instance if the flash device has a size of 32Mb, the physical address
 *               range is 0x00000000-0x003FFFFF. In this example, the start address must be in this
 *               range. Note that the start address is relative to the absolute start of the flash
 *               device. There is no offset to account for.
 *******************************************************************************************************/
void FS_NOR_Rd(FS_NOR_HANDLE nor_handle,
               void          *p_dest,
               CPU_INT32U    start_addr,
               CPU_INT32U    cnt,
               RTOS_ERR      *p_err)
{
  FS_NOR_PHY *p_nor_phy;
  FS_NOR     *p_nor_media;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  FS_MEDIA_WITH_NO_IO(nor_handle.MediaHandle, p_err) {
    p_nor_media = (FS_NOR *)nor_handle.MediaHandle.MediaPtr;
    p_nor_phy = p_nor_media->PhyPtr;

    p_nor_phy->PhyApiPtr->Rd(p_nor_phy,
                             p_dest,
                             start_addr,
                             cnt,
                             p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      FS_BLK_DEV_CTR_ERR_INC(p_nor_media->ErrRdCtr);
      return;
    }

    FS_BLK_DEV_CTR_STAT_ADD(p_nor_media->StatRdOctetCtr, cnt);
  }
}

/****************************************************************************************************//**
 *                                               FS_NOR_Wr()
 *
 * @brief    Write to a NOR.
 *
 * @param    nor_handle  Handle to a NOR.
 *
 * @param    p_src       Pointer to source buffer.
 *
 * @param    start_addr          Start address of write (see Note #1).
 *
 * @param    cnt         Number of octets to write.
 *
 * @param    p_err       Pointer to variable that will receive return the error code(s) from this
 *                       function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_IO
 *
 * @note     (1) Start address represents the flash device physical address from which the write must
 *               start. For instance if the flash device has a size of 32Mb, the physical address
 *               range is 0x00000000-0x003FFFFF. In this example, the start address must be in this
 *               range. Note that the start address is relative to the absolute start of the flash
 *               device. There is no offset to account for.
 *
 * @note     (2) Care should be taken if this function is used while a file system exists on the
 *               device, or if the device is low-level formatted.  The octet location(s) modified
 *               are NOT verified as being outside any existing file system or low-level format
 *               information.
 *
 * @note     (3) During a program operation, only '1' bits can be changed; a '0' bit cannot be changed
 *               to a '1'.  The application MUST know that the octets being programmed have not
 *               already been programmed.
 *
 * @note     (4) The success of high-level write operations can be checked by volume write check
 *               functionality.  The check here covers write operations for low-level data (e.g.
 *               block & sector headers).
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FS_NOR_Wr(FS_NOR_HANDLE nor_handle,
               void          *p_src,
               CPU_INT32U    start_addr,
               CPU_INT32U    cnt,
               RTOS_ERR      *p_err)
{
  FS_NOR     *p_nor;
  FS_NOR_PHY *p_nor_phy;
#if (FS_NOR_CFG_WR_CHK_EN == DEF_ENABLED)
  CPU_INT08U data_rd[2];
  CPU_INT08U data_wr_01;
  CPU_INT08U data_wr_02;
  CPU_INT08U *p_src_08;
  CPU_INT32U addr;
  CPU_INT32U cnt_rem;
#endif

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  FS_MEDIA_WITH_NO_IO(nor_handle.MediaHandle, p_err) {
    p_nor = (FS_NOR *)nor_handle.MediaHandle.MediaPtr;
    p_nor_phy = p_nor->PhyPtr;

#if (FS_NOR_CFG_WR_CHK_EN == DEF_ENABLED)                       // ------------------ CHK PGM VALID -------------------
    p_src_08 = (CPU_INT08U *)p_src;
    addr = start_addr;
    cnt_rem = cnt;
    while (cnt_rem >= 2u) {
      FS_NOR_Rd(nor_handle,                                     // Rd cur data.
                &data_rd[0],
                addr,
                2u,
                p_err);

      data_wr_01 = *p_src_08++;
      data_wr_02 = *p_src_08++;
      //                                                           Chk bits (see Note #1).
      if ((((CPU_INT08U)(data_wr_01 ^ data_rd[0]) & (CPU_INT08U) ~data_rd[0]) != 0x00u)
          || (((CPU_INT08U)(data_wr_02 ^ data_rd[1]) & (CPU_INT08U) ~data_rd[1]) != 0x00u)) {
        FS_BLK_DEV_CTR_ERR_INC(p_nor->ErrWrFailCtr);
        LOG_ERR(("Trying to write ", (X)data_wr_01, (X)data_wr_02, " over ", (X)data_rd[0], (X)data_rd[1], "."));
        RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
        return;
      }

      addr += 2u;
      cnt_rem -= 2u;
    }
#endif

    //                                                             --------------------- WR DATA ----------------------
    p_nor_phy->PhyApiPtr->Wr(p_nor_phy,
                             p_src,
                             start_addr,
                             cnt,
                             p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      FS_BLK_DEV_CTR_ERR_INC(p_nor->ErrWrCtr);
      return;
    }

#if (FS_NOR_CFG_WR_CHK_EN == DEF_ENABLED)                       // ----------------- CHK PGM SUCCESS ------------------
    p_src_08 = (CPU_INT08U *)p_src;
    addr = start_addr;
    cnt_rem = cnt;
    while (cnt_rem >= 2u) {
      FS_NOR_Rd(nor_handle,                                     // Rd new data.
                &data_rd[0],
                addr,
                2u,
                p_err);

      data_wr_01 = *p_src_08++;
      data_wr_02 = *p_src_08++;

      if ((data_wr_01 != data_rd[0])                            // Cmp to intended data.
          || (data_wr_02 != data_rd[1])) {
        FS_BLK_DEV_CTR_ERR_INC(p_nor->ErrWrFailCtr);
        LOG_ERR(("Data compare failed ", (X)data_wr_01, (X)data_wr_02, " != ", (X)data_rd[0], (X)data_rd[1], "."));
        RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
        return;
      }

      addr += 2u;
      cnt_rem -= 2u;
    }
#endif

    FS_BLK_DEV_CTR_STAT_ADD(p_nor->StatWrOctetCtr, cnt);
  }
}
#endif

/****************************************************************************************************//**
 *                                           FS_NOR_BlkErase()
 *
 * @brief    Erase a block on a NOR.
 *
 * @param    nor_handle  Handle to a NOR.
 *
 * @param    blk_ix      Index of the block to be erased.
 *
 * @param    p_err       Pointer to variable that will receive return the error code(s) from this
 *                       function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_IO
 *
 * @note     (1) Care should be taken if this function is used while a file system exists on the
 *               device, or if the device is low-level formatted.  The erased block is NOT verified
 *               as being outside any existing file system or low-level format information.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FS_NOR_BlkErase(FS_NOR_HANDLE nor_handle,
                     CPU_INT32U    blk_ix,
                     RTOS_ERR      *p_err)
{
  FS_NOR *p_nor_media;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  p_nor_media = (FS_NOR *)nor_handle.MediaHandle.MediaPtr;

  FS_NOR_PHY_BlkEraseInternal(p_nor_media,
                              blk_ix,
                              p_err);
}
#endif

/****************************************************************************************************//**
 *                                           FS_NOR_ChipErase()
 *
 * @brief    Erase an entire NOR.
 *
 * @param    nor_handle  Handle to a NOR.
 *
 * @param    p_err       Pointer to variable that will receive return the error code(s) from this
 *                       function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_IO
 *
 * @note     (1) This function should NOT be used while a file system exists on the device, or if the
 *               device is low-level formatted, unless the intent is to destroy all existing information.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FS_NOR_ChipErase(FS_NOR_HANDLE nor_handle,
                      RTOS_ERR      *p_err)
{
  FS_NOR     *p_nor_media;
  FS_NOR_PHY *p_nor_phy;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  p_nor_media = (FS_NOR *)nor_handle.MediaHandle.MediaPtr;
  p_nor_phy = p_nor_media->PhyPtr;

  p_nor_phy->PhyApiPtr->ChipErase(p_nor_phy, p_err);
}
#endif

/****************************************************************************************************//**
 *                                           FS_NOR_XIP_Cfg()
 *
 * @brief   Configure NOR flash and (Quad) SPI controller in XIP (eXecute-In-Place) mode.
 *
 * @param   nor_handle  Handle to a NOR.
 *
 * @param   xip_en      XIP mode enable/disable flag.
 *
 * @param   p_err       Pointer to variable that will receive return the error code(s) from this
 *                      function:
 *                          - RTOS_ERR_NONE
 *                          - RTOS_ERR_IO
 *******************************************************************************************************/
void FS_NOR_XIP_Cfg(FS_NOR_HANDLE nor_handle,
                    CPU_BOOLEAN   xip_en,
                    RTOS_ERR      *p_err)
{
  FS_NOR     *p_nor_media;
  FS_NOR_PHY *p_nor_phy;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  p_nor_media = (FS_NOR *)nor_handle.MediaHandle.MediaPtr;
  p_nor_phy = p_nor_media->PhyPtr;

  if (p_nor_phy->PhyApiPtr->XipCfg != DEF_NULL) {
    p_nor_phy->PhyApiPtr->XipCfg(p_nor_phy,
                                 xip_en,
                                 p_err);
  }
}

/****************************************************************************************************//**
 *                                               FS_NOR_Get()
 *
 * @brief    Get an NOR handle from a given media handle.
 *
 * @param    media_handle    Handle to a media.
 *
 * @return   Handle to a NOR.
 *******************************************************************************************************/
FS_NOR_HANDLE FS_NOR_Get(FS_MEDIA_HANDLE media_handle)
{
  FS_NOR_HANDLE nor_handle;

  nor_handle.MediaHandle = media_handle;

  return (nor_handle);
}

/****************************************************************************************************//**
 *                                           FS_NOR_BlkSizeLog2Get()
 *
 * @brief    Get the base-2 logarithm of a NOR block size.
 *
 * @param    nor_handle  Handle to a NOR.
 *
 * @param    p_err       Pointer to variable that will receive the return error code(s) from this
 *                       function:
 *                           - RTOS_ERR_NONE
 *
 * @return   Base-2 logarithm of the block size.
 *******************************************************************************************************/
CPU_INT08U FS_NOR_BlkSizeLog2Get(FS_NOR_HANDLE nor_handle,
                                 RTOS_ERR      *p_err)
{
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  FS_NOR *p_nor = (FS_NOR *)nor_handle.MediaHandle.MediaPtr;

  return (p_nor->PhyPtr->BlkSizeLog2);
}

/****************************************************************************************************//**
 *                                           FS_NOR_BlkCntGet()
 *
 * @brief    Get the number of blocks on a NOR.
 *
 * @param    nor_handle  Handle to a NOR.
 *
 * @param    p_err       Pointer to variable that will receive the return error code(s) from this
 *                       function:
 *                           - RTOS_ERR_NONE
 *
 * @return   The number of blocks.
 *******************************************************************************************************/
CPU_INT32U FS_NOR_BlkCntGet(FS_NOR_HANDLE nor_handle,
                            RTOS_ERR      *p_err)
{
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  FS_NOR *p_nor = (FS_NOR *)nor_handle.MediaHandle.MediaPtr;

  return (p_nor->PhyPtr->BlkCnt);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           MEDIA API FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           FS_NOR_MediaAdd()
 *
 * @brief    Add a NOR media instance.
 *
 * @param    p_media_pm_item     Pointer to a file system media platform manager item.
 *
 * @param    p_err               Error pointer.
 *******************************************************************************************************/
static FS_MEDIA *FS_NOR_MediaAdd(const FS_MEDIA_PM_ITEM *p_media_pm_item,
                                 RTOS_ERR               *p_err)
{
  FS_NOR_PM_ITEM *p_nor_pm_item;
  FS_NOR_PHY_API *p_phy_api;
  FS_NOR         *p_nor;

  RTOS_ASSERT_DBG_ERR_SET(p_media_pm_item->PmItem.Type == PLATFORM_MGR_ITEM_TYPE_HW_INFO_FS_NOR,
                          *p_err, RTOS_ERR_INVALID_TYPE, DEF_NULL);

  p_nor = (FS_NOR *)Mem_SegAlloc("FS - NOR instance",
                                 FSMedia_InitCfgPtr->MemSegPtr,
                                 sizeof(FS_NOR),
                                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  p_nor_pm_item = (FS_NOR_PM_ITEM *)p_media_pm_item;
  p_phy_api = (FS_NOR_PHY_API *)p_nor_pm_item->PartInfoPtr->PhyApiPtr;
  p_nor->PhyPtr = p_phy_api->Add(p_nor_pm_item,
                                 FSMedia_InitCfgPtr->MemSegPtr,
                                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  p_nor->PmItemPtr = p_nor_pm_item;

#if (FS_STORAGE_CFG_CTR_ERR_EN == DEF_ENABLED)                  // Clr err ctrs.
  p_nor->ErrRdCtr = 0u;
  p_nor->ErrWrCtr = 0u;
  p_nor->ErrEraseCtr = 0u;
  p_nor->ErrWrFailCtr = 0u;
  p_nor->ErrEraseFailCtr = 0u;
#endif

#if (FS_STORAGE_CFG_CTR_STAT_EN == DEF_ENABLED)                 // Clr stat ctrs.
  p_nor->StatRdOctetCtr = 0u;
  p_nor->StatWrOctetCtr = 0u;
  p_nor->StatEraseBlkCtr = 0u;
#endif

  return (&p_nor->Media);
}

/****************************************************************************************************//**
 *                                           FS_NOR_MediaRem()
 *
 * @brief    Remove a NOR media instance (not supported).
 *
 * @param    p_media     Pointer to a media instance.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_NOR_MediaRem(FS_MEDIA *p_media,
                            RTOS_ERR *p_err)
{
  PP_UNUSED_PARAM(p_media);
  PP_UNUSED_PARAM(p_err);

  RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_NOT_SUPPORTED,; );
}

/****************************************************************************************************//**
 *                                           FS_NOR_MediaIsConn()
 *
 * @brief    Verify whether a NOR media is connected.
 *
 * @param    p_media     Pointer to a media instance.
 *
 * @return   DEF_YES (since it's a non-removable media).
 *******************************************************************************************************/
static CPU_BOOLEAN FS_NOR_MediaIsConn(FS_MEDIA *p_media)
{
  PP_UNUSED_PARAM(p_media);

  return (DEF_YES);
}

/****************************************************************************************************//**
 *                                       FS_NOR_MediaAlignReqGet()
 *
 * @brief    Get the alignment requirement for a media instance.
 *
 * @param    p_media     Pointer to a NOR media instance.
 *
 * @param    p_err       Error pointer.
 *
 * @return   Alignment requirement.
 *******************************************************************************************************/
static CPU_SIZE_T FS_NOR_MediaAlignReqGet(FS_MEDIA *p_media,
                                          RTOS_ERR *p_err)
{
  FS_NOR     *p_nor;
  FS_NOR_PHY *p_nor_phy;
  CPU_SIZE_T align_req;

  p_nor = (FS_NOR *)p_media;
  p_nor_phy = p_nor->PhyPtr;

  align_req = p_nor_phy->PhyApiPtr->AlignReqGet(p_nor_phy, p_err);

  return (align_req);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       FS_NOR_PHY_BlkEraseInternal()
 *
 * @brief    Erase a block.
 *
 * @param    p_nor   Pointer to a NOR instance.
 *
 * @param    blk_ix  Index of the block to be erased.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/
void FS_NOR_PHY_BlkEraseInternal(FS_NOR     *p_nor,
                                 CPU_INT32U blk_ix,
                                 RTOS_ERR   *p_err)
{
  FS_NOR_PHY *p_nor_phy;
  CPU_INT32U blk_addr_phy;
  CPU_INT32U blk_size;
#if (FS_NOR_CFG_WR_CHK_EN == DEF_ENABLED)
  CPU_INT32U addr;
  CPU_INT32U cnt_rem;
  CPU_INT08U data_rd[16];
  CPU_INT32U i;
#endif

  p_nor_phy = p_nor->PhyPtr;

  blk_addr_phy = FS_UTIL_MULT_PWR2(blk_ix, p_nor_phy->BlkSizeLog2);
  blk_size = FS_UTIL_PWR2(p_nor_phy->BlkSizeLog2);

  p_nor_phy->PhyApiPtr->BlkErase(p_nor_phy,
                                 blk_addr_phy,
                                 blk_size,
                                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    FS_BLK_DEV_CTR_ERR_INC(p_nor->ErrEraseCtr);
    return;
  }

#if (FS_NOR_CFG_WR_CHK_EN == DEF_ENABLED)
  addr = blk_addr_phy;
  cnt_rem = blk_size;
  while (cnt_rem >= sizeof(data_rd)) {
    FS_NOR_PHY_RdInternal(p_nor,                                // Rd new data.
                          &data_rd[0],
                          addr,
                          sizeof(data_rd),
                          p_err);

    for (i = 0; i < sizeof(data_rd); i++) {
      if (data_rd[i] != 0xFFu) {                                // Cmp to erased data.
        FS_BLK_DEV_CTR_ERR_INC(p_nor->ErrEraseFailCtr);
        LOG_ERR(("Data compare failed ", (X)data_rd[i], " != ", (X)0xFFu, " at ", (X)addr + i, "."));
        RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
        return;
      }
    }

    addr += sizeof(data_rd);
    cnt_rem -= sizeof(data_rd);
  }
#endif

  FS_BLK_DEV_CTR_STAT_INC(p_nor->StatEraseBlkCtr);
}

/****************************************************************************************************//**
 *                                           FS_NOR_PHY_RdInternal()
 *
 * @brief    Read from a NOR.
 *
 * @param    p_nor   Pointer to a NOR instance.
 *
 * @param    p_dest  Pointer to a destination buffer.
 *
 * @param    start   Start address.
 *
 * @param    cnt     Number of octets to be read.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/
void FS_NOR_PHY_RdInternal(FS_NOR     *p_nor,
                           void       *p_dest,
                           CPU_INT32U start,
                           CPU_INT32U cnt,
                           RTOS_ERR   *p_err)
{
  FS_NOR_PHY *p_nor_phy;

  p_nor_phy = p_nor->PhyPtr;

  p_nor_phy->PhyApiPtr->Rd(p_nor_phy,
                           p_dest,
                           start,
                           cnt,
                           p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    FS_BLK_DEV_CTR_ERR_INC(p_nor->ErrRdCtr);
    return;
  }

  FS_BLK_DEV_CTR_STAT_ADD(p_nor->StatRdOctetCtr, cnt);
}

/****************************************************************************************************//**
 *                                           FS_NOR_PHY_WrInternal()
 *
 * @brief    Write to a NOR.
 *
 * @param    p_nor   Pointer to a NOR instance.
 *
 * @param    p_src   Pointer to the source buffer.
 *
 * @param    start   Start address.
 *
 * @param    cnt     Number of octets to be written.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FS_NOR_PHY_WrInternal(FS_NOR     *p_nor,
                           void       *p_src,
                           CPU_INT32U start,
                           CPU_INT32U cnt,
                           RTOS_ERR   *p_err)
{
  FS_NOR_PHY *p_nor_phy;
#if (FS_NOR_CFG_WR_CHK_EN == DEF_ENABLED)
  CPU_INT08U data_rd[2];
  CPU_INT08U data_wr_01;
  CPU_INT08U data_wr_02;
  CPU_INT08U *p_src_08;
  CPU_INT32U addr;
  CPU_INT32U cnt_rem;
#endif

  p_nor_phy = p_nor->PhyPtr;

#if (FS_NOR_CFG_WR_CHK_EN == DEF_ENABLED)                       // ------------------ CHK PGM VALID -------------------
  p_src_08 = (CPU_INT08U *)p_src;
  addr = start;
  cnt_rem = cnt;
  while (cnt_rem >= 2u) {
    FS_NOR_PHY_RdInternal(p_nor,                                // Rd cur data.
                          &data_rd[0],
                          addr,
                          2u,
                          p_err);

    data_wr_01 = *p_src_08++;
    data_wr_02 = *p_src_08++;
    //                                                             Chk bits (see Note #1).
    if ((((CPU_INT08U)(data_wr_01 ^ data_rd[0]) & (CPU_INT08U) ~data_rd[0]) != 0x00u)
        || (((CPU_INT08U)(data_wr_02 ^ data_rd[1]) & (CPU_INT08U) ~data_rd[1]) != 0x00u)) {
      FS_BLK_DEV_CTR_ERR_INC(p_nor->ErrWrFailCtr);
      LOG_ERR(("Trying to write ", (X)data_wr_01, (X)data_wr_02, " over ", (X)data_rd[0], (X)data_rd[1], "."));
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
      return;
    }

    addr += 2u;
    cnt_rem -= 2u;
  }
#endif

  //                                                               ---------------------- WR DATA ---------------------
  p_nor_phy->PhyApiPtr->Wr(p_nor_phy,
                           p_src,
                           start,
                           cnt,
                           p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    FS_BLK_DEV_CTR_ERR_INC(p_nor->ErrWrCtr);
    return;
  }

#if (FS_NOR_CFG_WR_CHK_EN == DEF_ENABLED)                       // ----------------- CHK PGM SUCCESS ------------------
  p_src_08 = (CPU_INT08U *)p_src;
  addr = start;
  cnt_rem = cnt;
  while (cnt_rem >= 2u) {
    FS_NOR_PHY_RdInternal(p_nor,                                // Rd new data.
                          &data_rd[0],
                          addr,
                          2u,
                          p_err);

    data_wr_01 = *p_src_08++;
    data_wr_02 = *p_src_08++;

    if ((data_wr_01 != data_rd[0])                              // Cmp to intended data.
        || (data_wr_02 != data_rd[1])) {
      FS_BLK_DEV_CTR_ERR_INC(p_nor->ErrWrFailCtr);
      LOG_ERR(("Data compare failed ", (X)data_wr_01, (X)data_wr_02, " != ", (X)data_rd[0], (X)data_rd[1], "."));
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
      return;
    }

    addr += 2u;
    cnt_rem -= 2u;
  }
#endif

  FS_BLK_DEV_CTR_STAT_ADD(p_nor->StatWrOctetCtr, cnt);
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_FS_STORAGE_NOR_AVAIL
