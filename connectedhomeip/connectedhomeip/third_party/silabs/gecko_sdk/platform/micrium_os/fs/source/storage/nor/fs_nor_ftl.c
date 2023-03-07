/***************************************************************************//**
 * @file
 * @brief File System NOR FTL
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

//                                                                 ----------------------- CFG ------------------------
#include  <fs_storage_cfg.h>

//                                                                 ----------------------- EXT ------------------------
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/logging/logging_priv.h>
#include  <common/include/lib_ascii.h>
#include  <common/include/lib_mem.h>
#include  <common/include/lib_str.h>

//                                                                 ------------------------ FS ------------------------
#include  <fs/source/shared/cleanup/cleanup_mgmt_priv.h>
#include  <fs/source/shared/fs_utils_priv.h>
#include  <fs/source/storage/nor/fs_nor_ftl_priv.h>
#include  <fs/source/storage/nor/fs_nor_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 *
 * Note(s) : (1) Default logical sector size used in case NOR configuration is not defined by the user.
 *               The logical sector size has an importance when creating the logical-to-physical mapping
 *               table. The lower the logical sector size is, the higher the logical-to-physical mapping
 *               table size will be. A default value of 4K allows a reasonable logical-to-physical mapping
 *               table size and 4K matches the physical subsector size that most NOR flash devices use.
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                        (FS, DRV, NOR)
#define  RTOS_MODULE_CUR                                    RTOS_CFG_MODULE_FS

#define  FS_DEV_NOR_LOG_SEC_SIZE_DFLT                   4096u   // See Note #1.

#define  FS_DEV_NOR_BLK_HDR_LEN                           32u

#define  FS_DEV_NOR_SEC_HDR_LEN                            8u
#define  FS_DEV_NOR_SEC_HDR_LEN_LOG                        3u

//                                                                 --------------- SECTOR STATUS MARKERS --------------
#define  FS_DEV_NOR_STATUS_SEC_ERASED             0xFFFFFFFFuL  // Sector is erased (unused).
#define  FS_DEV_NOR_STATUS_SEC_WRITING            0xFFFFFF00uL  // Sector is in process of being written.
#define  FS_DEV_NOR_STATUS_SEC_VALID              0xFFFF0000uL  // Sector holds valid data.
#define  FS_DEV_NOR_STATUS_SEC_INVALID            0x00000000u   // Sector data is invalid.

//                                                                 ---------------- BLOCK HEADER FIELDS ---------------
#define  FS_DEV_NOR_BLK_HDR_OFFSET_MARK1                   0u   // Marker word 1 offset.
#define  FS_DEV_NOR_BLK_HDR_OFFSET_MARK2                   4u   // Marker word 2 offset.
#define  FS_DEV_NOR_BLK_HDR_OFFSET_ERASE_CNT               8u   // Erase count offset.
#define  FS_DEV_NOR_BLK_HDR_OFFSET_VER                    12u   // Format version offset.
#define  FS_DEV_NOR_BLK_HDR_OFFSET_SEC_SIZE               14u   // Sec size offset.
#define  FS_DEV_NOR_BLK_HDR_OFFSET_BLK_CNT                16u   // Blk cnt  offset.

#define  FS_DEV_NOR_BLK_HDR_MARK_WORD_1           0x4E205346u   // Marker word 1 ("FS N").
#define  FS_DEV_NOR_BLK_HDR_MARK_WORD_2           0x2020524Fu   // Marker word 2 ("OR  ").
#define  FS_DEV_NOR_BLK_HDR_ERASE_CNT_INVALID     0xFFFFFFFFuL  // Erase count invalid.
#define  FS_DEV_NOR_BLK_HDR_VER                       0x0401u   // Format version (4.01).

//                                                                 --------------- SECTOR HEADER FIELDS ---------------
#define  FS_DEV_NOR_SEC_HDR_OFFSET_SEC_NBR                 0u   // Logical sector number offset.
#define  FS_DEV_NOR_SEC_HDR_OFFSET_STATUS                  4u   // Status of sector offset.

#define  FS_DEV_NOR_SEC_NBR_INVALID       DEF_INT_32U_MAX_VAL   // Logical sector number (invalid).

#define  FS_NOR_CFG_DEV_SIZE_AUTO                         0u
#define  FS_NOR_CFG_SEC_SIZE_AUTO                         0u

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef struct fs_nor_ftl {
  FS_BLK_DEV   BlkDev;
  //                                                               ------------------- DEV OPEN INFO ------------------
  FS_LB_QTY    DiskSizeSec;                                     // Size of disk (in secs).
  FS_LB_QTY    SecCntTot;                                       // Tot of secs on dev.
  CPU_INT08U   SecCntTotLog;                                    // Base-2 log of tot of secs on dev.
  FS_LB_QTY    BlkCntUsed;                                      // Cnt of blks in file system.
  FS_LB_NBR    BlkNbrFirst;                                     // Nbr  of first blk in file system.
  FS_LB_QTY    BlkSecCnts;                                      // Cnt of secs in each blk.
  CPU_INT08U   SecSizeLog2;                                     // Base-2 logarithm of the sector size.
  CPU_INT08U   PctRsvd;                                         // Pct of device area rsvd.
  FS_LB_QTY    AB_Cnt;                                          // Active blk cnt.

  //                                                               ------------------- DEV OPEN DATA ------------------
  void         *BufPtr;                                         // Ptr to drv buf.

  //                                                               ------------------ MOUNT DEV INFO ------------------
  void         *BlkSecCntValidTbl;                              // Tbl of cnts of valid secs in blk.
  void         *BlkEraseMap;                                    // Map of blk erase status.
  void         *L2P_Tbl;                                        // Logical to phy addr tbl.
  FS_LB_NBR    *AB_IxTbl;                                       // Active blk ix tbl.
  FS_LB_NBR    *AB_SecNextTbl;                                  // Next sec in active blk tbl.
  CPU_BOOLEAN  BlkWearLevelAvail;                               // Erased wear level blk avail.
  FS_LB_QTY    BlkCntValid;                                     // Cnt of blks with valid data.
  FS_LB_QTY    BlkCntErased;                                    // Cnt of erased blks.
  FS_LB_QTY    BlkCntInvalid;                                   // Cnt of blks with only invalid data.
  FS_LB_QTY    SecCntValid;                                     // Cnt of secs with valid data.
  FS_LB_QTY    SecCntErased;                                    // Cnt of erased secs.
  FS_LB_QTY    SecCntInvalid;                                   // Cnt of secs with invalid data.
  CPU_INT32U   EraseCntMin;                                     // Min erase cnt.
  CPU_INT32U   EraseCntMax;                                     // Max erase cnt.
  CPU_INT16U   EraseCntDiffTh;                                  // Erase count difference threshold.

  CPU_BOOLEAN  IsMounted;

  SLIST_MEMBER ListMember;

#if (FS_STORAGE_CFG_CTR_STAT_EN == DEF_ENABLED)
  CPU_INT32U   StatCopyCtr;                                     // Secs copied.
  CPU_INT32U   StatReleaseCtr;                                  // Secs released.
  CPU_INT32U   StatInvalidBlkCtr;                               // Blks invalidated.
#endif
} FS_NOR_FTL;

/********************************************************************************************************
 ********************************************************************************************************
 *                                               VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

const FS_NOR_FTL_CFG FS_NOR_CfgDflt = {
  .RegionNbr = 0u,
  .StartOffset = 0u,
  .PctRsvd = 10u,
  .EraseCntDiffTh = 20u,
  .DevSize = FS_NOR_CFG_DEV_SIZE_AUTO,
  .SecSize = FS_NOR_CFG_SEC_SIZE_AUTO,
};

static SLIST_MEMBER *FS_NOR_FTL_ListHeadPtr;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                               BLOCK DEVICE INTERFACE FUNCTION PROTOTYPES
 *******************************************************************************************************/

static FS_BLK_DEV *FS_NOR_FTL_Add(FS_MEDIA *p_media,
                                  RTOS_ERR *p_err);

static void FS_NOR_FTL_Rem(FS_BLK_DEV *p_blk_dev,
                           RTOS_ERR   *p_err);

static FS_NOR_FTL *FS_NOR_FTL_Alloc(FS_NOR               *p_nor,
                                    const FS_NOR_FTL_CFG *p_cfg,
                                    RTOS_ERR             *p_err);

static FS_NOR_FTL *FS_NOR_FTL_Get(FS_MEDIA *p_media);

static void FS_NOR_FTL_Open(FS_BLK_DEV *p_blk_dev,
                            RTOS_ERR   *p_err);

static void FS_NOR_FTL_Close(FS_BLK_DEV *p_blk_dev,
                             RTOS_ERR   *p_err);

static void FS_NOR_FTL_Rd(FS_BLK_DEV *p_blk_dev,
                          void       *p_dest,
                          FS_LB_NBR  lb_nbr_start,
                          FS_LB_QTY  lb_cnt,
                          RTOS_ERR   *p_err);

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NOR_FTL_Wr(FS_BLK_DEV *p_blk_dev,
                          void       *p_src,
                          FS_LB_NBR  lb_nbr_start,
                          FS_LB_QTY  lb_cnt,
                          RTOS_ERR   *p_err);

static void FS_NOR_FTL_Trim(FS_BLK_DEV *p_blk_dev,
                            FS_LB_NBR  lb_nbr,
                            RTOS_ERR   *p_err);

static void FS_NOR_FTL_Sync(FS_BLK_DEV *p_blk_dev,
                            RTOS_ERR   *p_err);
#endif

static void FS_NOR_FTL_Query(FS_BLK_DEV      *p_blk_dev,
                             FS_BLK_DEV_INFO *p_info,
                             RTOS_ERR        *p_err);

/********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NOR_FTL_LowFmtInternal(FS_NOR_FTL *p_nor_ftl,
                                      FS_NOR     *p_nor,
                                      RTOS_ERR   *p_err);
#endif

static void FS_NOR_FTL_LowMountInternal(FS_NOR_FTL *p_nor_ftl,
                                        RTOS_ERR   *p_err);

static void FS_NOR_FTL_LowUnmountInternal(FS_NOR_FTL *p_nor_ftl);

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NOR_FTL_LowCompactInternal(FS_NOR_FTL *p_nor_ftl,
                                          RTOS_ERR   *p_err);
#endif

static void FS_NOR_FTL_LogParamCalc(FS_NOR_FTL           *p_nor_ftl,
                                    FS_NOR               *p_nor,
                                    const FS_NOR_FTL_CFG *p_nor_cfg,
                                    RTOS_ERR             *p_err);

static void FS_NOR_FTL_AllocDevData(FS_NOR     *p_nor,
                                    FS_NOR_FTL *p_nor_ftl,
                                    MEM_SEG    *p_seg,
                                    RTOS_ERR   *p_err);

static void FS_NOR_FTL_RdBlkHdr(FS_NOR_FTL *p_nor_ftl,
                                void       *p_dest,
                                FS_LB_NBR  blk_ix,
                                RTOS_ERR   *p_err);

static void FS_NOR_FTL_RdSecLogical(FS_NOR_FTL *p_nor_ftl,
                                    void       *p_dest,
                                    FS_LB_NBR  sec_nbr_logical,
                                    RTOS_ERR   *p_err);

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NOR_FTL_WrSecLogical(FS_NOR_FTL *p_nor_ftl,
                                    void       *p_src,
                                    FS_LB_NBR  sec_nbr_logical,
                                    RTOS_ERR   *p_err);

static void FS_NOR_FTL_WrSecLogicalInternal(FS_NOR_FTL *p_nor_ftl,
                                            void       *p_src,
                                            FS_LB_NBR  sec_nbr_logical,
                                            RTOS_ERR   *p_err);

static void FS_NOR_FTL_InvalidateSec(FS_NOR_FTL *p_nor_ftl,
                                     FS_LB_NBR  sec_nbr_phy,
                                     RTOS_ERR   *p_err);

static void FS_NOR_FTL_EraseBlkPrepare(FS_NOR_FTL *p_nor_ftl,
                                       FS_LB_NBR  blk_ix,
                                       RTOS_ERR   *p_err);

static void FS_NOR_FTL_EraseBlkEmptyFmt(FS_NOR_FTL *p_nor_ftl,
                                        FS_LB_NBR  blk_ix,
                                        RTOS_ERR   *p_err);

static FS_LB_NBR FS_NOR_FTL_FindEraseBlk(FS_NOR_FTL *p_nor_ftl);

static FS_LB_NBR FS_NOR_FTL_FindEraseBlkWear(FS_NOR_FTL *p_nor_ftl);

static FS_LB_NBR FS_NOR_FTL_FindErasedBlk(FS_NOR_FTL *p_nor_ftl);
#endif

static void FS_NOR_FTL_AB_ClrAll(FS_NOR_FTL *p_nor_ftl);

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static FS_LB_QTY FS_NOR_FTL_GetAB_Cnt(FS_NOR_FTL *p_nor_ftl);

static CPU_BOOLEAN FS_NOR_FTL_IsAB(FS_NOR_FTL *p_nor_ftl,
                                   FS_LB_NBR  blk_ix);
#endif

static CPU_BOOLEAN FS_NOR_FTL_AB_Add(FS_NOR_FTL *p_nor_ftl,
                                     FS_LB_NBR  blk_ix,
                                     FS_LB_NBR  sec_ix_next);

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NOR_FTL_AB_RemoveAll(FS_NOR_FTL *p_nor_ftl);

static void FS_NOR_FTL_AB_Remove(FS_NOR_FTL *p_nor_ftl,
                                 FS_LB_NBR  blk_ix);

static FS_LB_QTY FS_NOR_FTL_AB_SecCntTotErased(FS_NOR_FTL *p_nor_ftl);

static FS_LB_QTY FS_NOR_FTL_AB_SecCntErased(FS_NOR_FTL *p_nor_ftl,
                                            FS_LB_NBR  blk_ix);

static FS_LB_NBR FS_NOR_FTL_AB_SecFind(FS_NOR_FTL *p_nor_ftl,
                                       FS_LB_NBR  sec_nbr_logical);
#endif

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NOR_FTL_GetBlkInfo(FS_NOR_FTL  *p_nor_ftl,
                                  FS_LB_NBR   blk_ix,
                                  CPU_BOOLEAN *p_erased,
                                  FS_LB_QTY   *p_sec_cnt_valid);
#endif

static void FS_NOR_FTL_SetBlkErased(FS_NOR_FTL  *p_nor_ftl,
                                    FS_LB_NBR   blk_ix,
                                    CPU_BOOLEAN erased);

static void FS_NOR_FTL_SetBlkSecCntValid(FS_NOR_FTL *p_nor_ftl,
                                         FS_LB_NBR  blk_ix,
                                         FS_LB_QTY  sec_cnt_valid);

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NOR_FTL_DecBlkSecCntValid(FS_NOR_FTL *p_nor_ftl,
                                         FS_LB_NBR  blk_ix);

static void FS_NOR_FTL_IncBlkSecCntValid(FS_NOR_FTL *p_nor_ftl,
                                         FS_LB_NBR  blk_ix);
#endif

static CPU_INT32U FS_NOR_FTL_BlkIxToAddr(FS_NOR_FTL *p_nor_ftl,
                                         FS_LB_NBR  blk_ix,
                                         RTOS_ERR   *p_err);

static CPU_INT32U FS_NOR_FTL_SecNbrPhyToAddr(FS_NOR_FTL *p_nor_ftl,
                                             FS_LB_NBR  sec_nbr_phy,
                                             RTOS_ERR   *p_err);

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static FS_LB_NBR FS_NOR_FTL_SecNbrPhyToBlkIx(FS_NOR_FTL *p_nor_ftl,
                                             FS_LB_NBR  sec_nbr_phy,
                                             RTOS_ERR   *p_err);

static FS_LB_NBR FS_NOR_FTL_BlkIxToSecNbrPhy(FS_NOR_FTL *p_nor_ftl,
                                             FS_LB_NBR  blk_ix);
#endif

static void *FS_NOR_FTL_L2P_Create(FS_NOR_FTL *p_nor_ftl,
                                   MEM_SEG    *p_seg,
                                   RTOS_ERR   *p_err);

static FS_LB_NBR FS_NOR_FTL_L2P_GetEntry(FS_NOR_FTL *p_nor_ftl,
                                         FS_LB_NBR  sec_nbr_logical);

static void FS_NOR_FTL_L2P_SetEntry(FS_NOR_FTL *p_nor_ftl,
                                    FS_LB_NBR  sec_nbr_logical,
                                    FS_LB_NBR  sec_nbr_phy);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

const FS_BLK_DEV_API FS_NOR_BlkDevApi = {
  FS_NOR_FTL_Add,
  FS_NOR_FTL_Rem,
  FS_NOR_FTL_Open,
  FS_NOR_FTL_Close,
  FS_NOR_FTL_Rd,
#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
  FS_NOR_FTL_Wr,
  FS_NOR_FTL_Sync,
  FS_NOR_FTL_Trim,
#endif
  FS_NOR_FTL_Query,
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                  FS_NOR_FTL_ConfigureLowParams()
 *
 * @brief    Configure NOR FTL parameters.
 *
 * @param    media_handle    Handle to a media.
 *
 * @param    p_cfg           Pointer to a NOR FTL configuration structure.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_SEG_OVF
 *
 * @note     (1) FS_NOR_FTL_ConfigureLowParams() must be called prior to FSBlkDev_Open() or
 *               FSMedia_LowFmt(). It will configure a NOR FTL instance using the user specific
 *               parameters and allocate all needed resources by the FTL instance. Then
 *               FSBlkDev_Open() or FSMedia_LowFmt() can be called and the NOR FTL instance can be
 *               obtained from the internal NOR FTL list. Thus avoiding reallocating
 *               resources once more time for the same NOR instance.
 *******************************************************************************************************/
void FS_NOR_FTL_ConfigureLowParams(FS_MEDIA_HANDLE      media_handle,
                                   const FS_NOR_FTL_CFG *p_cfg,
                                   RTOS_ERR             *p_err)
{
  FS_NOR_FTL        *p_nor_ftl;
  FS_NOR            *p_nor;
  FS_BLK_DEV_HANDLE blk_dev_handle;

  //                                                               Assert that NOR blk dev not previously open.
  blk_dev_handle = FSBlkDev_Get(media_handle);
  RTOS_ASSERT_DBG_ERR_SET(FS_BLK_DEV_HANDLE_IS_NULL(blk_dev_handle), *p_err, RTOS_ERR_INVALID_STATE,; );

  FS_MEDIA_WITH_NO_IO(media_handle, p_err) {
    p_nor_ftl = FS_NOR_FTL_Get(media_handle.MediaPtr);
    //                                                             Assert that a FTL obj not previously allocated.
    RTOS_ASSERT_DBG_ERR_SET(p_nor_ftl == DEF_NULL, *p_err, RTOS_ERR_INVALID_STATE,; );

    p_nor = (FS_NOR *)media_handle.MediaPtr;
    p_nor_ftl = FS_NOR_FTL_Alloc(p_nor, p_cfg, p_err);          // Alloc new FTL and cfg it with user params.
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
    p_nor_ftl->BlkDev.MediaPtr = media_handle.MediaPtr;         // Save associated media info into FTL instance...
                                                                // ...Allows other functions to retrieve FTL instance.

    //                                                             Insert NOR FTL instance in list (see Note #1).
    SList_Push(&FS_NOR_FTL_ListHeadPtr, &p_nor_ftl->ListMember);
  }
}

/****************************************************************************************************//**
 *                                           FS_NOR_FTL_LowCompact()
 *
 * @brief    Low-level compact a NOR device.
 *
 * @param    blk_dev_handle  Handle to a NOR-based block device.
 *
 * @param    p_err           Pointer to variable that will receive return the error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_BLK_DEV_CLOSED
 *                               - RTOS_ERR_BLK_DEV_CORRUPTED
 *                               - RTOS_ERR_IO
 *
 * @note     (1) Compacting groups sectors containing high-level data into as few blocks as possible.
 *               If an image of a file system is to be formed for deployment, to be burned into chips
 *               for production, then it should be compacted after all files & directories are created.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FS_NOR_FTL_LowCompact(FS_BLK_DEV_HANDLE blk_dev_handle,
                           RTOS_ERR          *p_err)
{
  FS_BLK_DEV *p_blk_dev;
  FS_MEDIA   *p_media;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    BREAK_ON_ERR(FS_BLK_DEV_WITH) (blk_dev_handle, p_err) {
      p_blk_dev = blk_dev_handle.BlkDevPtr;
      p_media = p_blk_dev->MediaPtr;

      RTOS_ASSERT_DBG(p_media->PmItemPtr->PmItem.Type == PLATFORM_MGR_ITEM_TYPE_HW_INFO_FS_NOR,
                      RTOS_ERR_INVALID_TYPE,; );

      BREAK_ON_ERR(FS_MEDIA_LOCK_WITH) (p_media) {
        ASSERT_BREAK(p_blk_dev->Id == blk_dev_handle.BlkDevId,
                     RTOS_ERR_BLK_DEV_CLOSED);

        FS_NOR_FTL_LowCompactInternal((FS_NOR_FTL *)p_blk_dev, p_err);
      }
    }
  } WITH_SCOPE_END
}
#endif

/****************************************************************************************************//**
 *                                           FS_NOR_FTL_LowDefrag()
 *
 * @brief    Low-level defragment a NOR device.
 *
 * @param    blk_dev_handle  Handle to a NOR block device.
 *
 * @param    p_err           Pointer to variable that will receive return the error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_BLK_DEV_CLOSED
 *                               - RTOS_ERR_BLK_DEV_CORRUPTED
 *                               - RTOS_ERR_IO
 *
 * @note     (1) Defragmentation groups sectors containing high-level data into as few blocks as
 *               possible, in order of logical sector.  A defragmented file system should have near-
 *               optimal access speeds in a read-only environment.
 *               See also 'FS_NOR_FTL_LowCompact() Note #1'.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FS_NOR_FTL_LowDefrag(FS_BLK_DEV_HANDLE blk_dev_handle,
                          RTOS_ERR          *p_err)
{
  FS_BLK_DEV *p_blk_dev;
  FS_MEDIA   *p_media;
  FS_NOR_FTL *p_nor_ftl;
  FS_LB_NBR  sec_nbr_logical;
  FS_LB_NBR  sec_nbr_phy;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    BREAK_ON_ERR(FS_BLK_DEV_WITH) (blk_dev_handle, p_err) {
      p_blk_dev = blk_dev_handle.BlkDevPtr;
      p_media = p_blk_dev->MediaPtr;
      p_nor_ftl = (FS_NOR_FTL *)p_blk_dev;

      RTOS_ASSERT_DBG(p_media->PmItemPtr->PmItem.Type == PLATFORM_MGR_ITEM_TYPE_HW_INFO_FS_NOR,
                      RTOS_ERR_INVALID_TYPE,; );

      BREAK_ON_ERR(FS_MEDIA_LOCK_WITH) (p_media) {
        FS_NOR_FTL_LowCompactInternal(p_nor_ftl, p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          return;
        }

        FS_NOR_FTL_AB_RemoveAll(p_nor_ftl);                     // Remove all active blks.

        for (sec_nbr_logical = 0u; sec_nbr_logical < p_nor_ftl->DiskSizeSec; sec_nbr_logical++) {
          sec_nbr_phy = FS_NOR_FTL_L2P_GetEntry(p_nor_ftl, sec_nbr_logical);
          if (sec_nbr_phy != FS_DEV_NOR_SEC_NBR_INVALID) {
            FS_NOR_FTL_RdSecLogical(p_nor_ftl,                  // Rd sec.
                                    p_nor_ftl->BufPtr,
                                    sec_nbr_logical,
                                    p_err);
            if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
              return;
            }

            FS_NOR_FTL_WrSecLogical(p_nor_ftl,                  // Wr sec to new location.
                                    p_nor_ftl->BufPtr,
                                    sec_nbr_logical,
                                    p_err);
            if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
              return;
            }
          }
        }
      }
    }
  } WITH_SCOPE_END
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               FS_NOR_FTL_Init()
 *
 * @brief    Initialize the NOR FTL.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/
void FS_NOR_FTL_Init(RTOS_ERR *p_err)
{
  PP_UNUSED_PARAM(p_err);

  SList_Init(&FS_NOR_FTL_ListHeadPtr);
}

/****************************************************************************************************//**
 *                                           FS_NOR_FTL_LowFmt()
 *
 * @brief    Low-level format a NOR media instance.
 *
 * @param    p_media     Pointer to a media instance.
 *
 * @param    p_err       Error pointer.
 *
 * @note     (1) A NOR medium MUST be low-level formatted with this driver prior to access by the
 *               high-level file system, a requirement which the device module enforces.
 *
 * @note     (2) Pushing NOR FTL instance in the internal NOR FTL list allows other functions to use
 *               it and avoid reallocating resources one more time for the same NOR instance.
 *******************************************************************************************************/
#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FS_NOR_FTL_LowFmt(FS_MEDIA_HANDLE media_handle,
                       RTOS_ERR        *p_err)
{
  FS_NOR_FTL        *p_nor_ftl;
  FS_NOR            *p_nor;
  FS_NOR_PHY        *p_nor_phy;
  CPU_BOOLEAN       is_open;
  FS_BLK_DEV_HANDLE blk_dev_handle;

  //                                                               Assert that NOR blk dev not previously open.
  blk_dev_handle = FSBlkDev_Get(media_handle);
  RTOS_ASSERT_DBG_ERR_SET(FS_BLK_DEV_HANDLE_IS_NULL(blk_dev_handle), *p_err, RTOS_ERR_INVALID_STATE,; );

  p_nor = (FS_NOR *)media_handle.MediaPtr;
  p_nor_phy = p_nor->PhyPtr;

  is_open = p_nor->IsOpen;
  if (!is_open) {
    p_nor_phy->PhyApiPtr->Open(p_nor_phy, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
  }

  p_nor_ftl = FS_NOR_FTL_Get(media_handle.MediaPtr);
  if (p_nor_ftl == DEF_NULL) {
    p_nor_ftl = FS_NOR_FTL_Alloc(p_nor, DEF_NULL, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
    p_nor_ftl->BlkDev.MediaPtr = media_handle.MediaPtr;         // Save associated media info into FTL instance...
                                                                // ...Allows other functions to retrieve FTL instance.
  }

  if (p_nor_ftl->IsMounted) {
    FS_NOR_FTL_LowUnmountInternal(p_nor_ftl);
  }

  FS_NOR_FTL_LowFmtInternal(p_nor_ftl, p_nor, p_err);

  if (!is_open) {
    p_nor_phy->PhyApiPtr->Close(p_nor_phy, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
  }
  //                                                               Insert NOR FTL instance in list (see Note #2).
  SList_Push(&FS_NOR_FTL_ListHeadPtr, &p_nor_ftl->ListMember);
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       BLOCK DEVICE API FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               FS_NOR_FTL_Add()
 *
 * @brief    Add a NOR FTL instance.
 *
 * @param    p_media     Pointer to a NOR media instance.
 *
 * @param    p_err       Error pointer.
 *
 * @return   Added FTL instance (i.e. block device).
 *******************************************************************************************************/
static FS_BLK_DEV *FS_NOR_FTL_Add(FS_MEDIA *p_media,
                                  RTOS_ERR *p_err)
{
  FS_NOR     *p_nor;
  FS_NOR_FTL *p_nor_ftl;

  p_nor = (FS_NOR *)p_media;
  p_nor_ftl = FS_NOR_FTL_Get(p_media);
  if (p_nor_ftl == DEF_NULL) {
    p_nor_ftl = FS_NOR_FTL_Alloc(p_nor, DEF_NULL, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (DEF_NULL);
    }
  }

  return (&p_nor_ftl->BlkDev);
}

/****************************************************************************************************//**
 *                                               FS_NOR_FTL_Rem()
 *
 * @brief    Remove a NOR FTL instance (dummy function).
 *
 * @param    p_blk_dev   Pointer to a block device.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_NOR_FTL_Rem(FS_BLK_DEV *p_blk_dev,
                           RTOS_ERR   *p_err)
{
  FS_NOR_FTL *p_nor_ftl;

  PP_UNUSED_PARAM(p_err);

  p_nor_ftl = (FS_NOR_FTL *)p_blk_dev;
  //                                                               Push back NOR FTL instance to be reused at nxt open.
  SList_Push(&FS_NOR_FTL_ListHeadPtr, &p_nor_ftl->ListMember);
}

/****************************************************************************************************//**
 *                                               FS_NOR_FTL_Open()
 *
 * @brief    Open a NOR instance.
 *
 * @param    p_media     Pointer to the media (NOR) to be opened.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_NOR_FTL_Open(FS_BLK_DEV *p_blk_dev,
                            RTOS_ERR   *p_err)
{
  FS_NOR     *p_nor;
  FS_NOR_PHY *p_nor_phy;
  FS_NOR_FTL *p_nor_ftl;

  p_nor = (FS_NOR *)p_blk_dev->MediaPtr;
  p_nor_ftl = (FS_NOR_FTL *)p_blk_dev;
  p_nor_phy = (FS_NOR_PHY *)p_nor->PhyPtr;

  p_nor_phy->PhyApiPtr->Open(p_nor_phy, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if (p_nor_ftl->IsMounted) {
    return;
  }

  FS_NOR_FTL_LowMountInternal(p_nor_ftl, p_err);
}

/****************************************************************************************************//**
 *                                           FS_NOR_FTL_Close()
 *
 * @brief    Close a NOR instance.
 *
 * @param    p_blk_dev   Pointer to a block device (NOR) to be closed.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_NOR_FTL_Close(FS_BLK_DEV *p_blk_dev,
                             RTOS_ERR   *p_err)
{
  FS_NOR *p_nor;

  p_nor = (FS_NOR *)p_blk_dev->MediaPtr;

  p_nor->PhyPtr->PhyApiPtr->Close(p_nor->PhyPtr, p_err);
}

/****************************************************************************************************//**
 *                                               FS_NOR_FTL_Rd()
 *
 * @brief    Read from a NOR.
 *
 * @param    p_blk_dev      Pointer to a block device (NOR) to read from.
 *
 * @param    p_dest         Pointer to destination buffer.
 *
 * @param    lb_nbr_start   Sector number to start reading from.
 *
 * @param    lb_cnt         Number of sectors to read.
 *
 * @param    p_err          Error pointer.
 *******************************************************************************************************/
static void FS_NOR_FTL_Rd(FS_BLK_DEV *p_blk_dev,
                          void       *p_dest,
                          FS_LB_NBR  lb_nbr_start,
                          FS_LB_QTY  lb_cnt,
                          RTOS_ERR   *p_err)
{
  FS_NOR_FTL *p_nor_ftl;
  CPU_INT08U *p_dest_08;
  FS_LB_NBR  sec_nbr_logical;
  FS_LB_QTY  cnt_rem;

  p_nor_ftl = (FS_NOR_FTL *)p_blk_dev;
  p_dest_08 = (CPU_INT08U *)p_dest;
  sec_nbr_logical = lb_nbr_start;
  cnt_rem = lb_cnt;

  while (cnt_rem > 0u) {
    FS_NOR_FTL_RdSecLogical(p_nor_ftl,
                            p_dest_08,
                            sec_nbr_logical,
                            p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    cnt_rem--;
    sec_nbr_logical++;
    p_dest_08 += FS_UTIL_PWR2(p_nor_ftl->SecSizeLog2);
  }
}

/****************************************************************************************************//**
 *                                               FS_NOR_FTL_Wr()
 *
 * @brief    Write to a NOR.
 *
 * @param    p_blk_dev      Pointer to a block device (NOR) to write to.
 *
 * @param    p_src          Pointer to source buffer.
 *
 * @param    lb_nbr_start   Sector number to start writing to.
 *
 * @param    lb_cnt         Number of sectors to write.
 *
 * @param    p_err          Error pointer.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NOR_FTL_Wr(FS_BLK_DEV *p_blk_dev,
                          void       *p_src,
                          FS_LB_NBR  lb_nbr_start,
                          FS_LB_QTY  lb_cnt,
                          RTOS_ERR   *p_err)
{
  FS_NOR_FTL *p_nor_ftl;
  CPU_INT08U *p_src_08;
  FS_LB_NBR  sec_nbr_logical;
  FS_LB_QTY  cnt_rem;

  p_nor_ftl = (FS_NOR_FTL *)p_blk_dev;
  p_src_08 = (CPU_INT08U *)p_src;
  sec_nbr_logical = lb_nbr_start;
  cnt_rem = lb_cnt;

  while (cnt_rem > 0u) {
    FS_NOR_FTL_WrSecLogical(p_nor_ftl,
                            p_src_08,
                            sec_nbr_logical,
                            p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    cnt_rem--;
    sec_nbr_logical++;
    p_src_08 += FS_UTIL_PWR2(p_nor_ftl->SecSizeLog2);
  }
}
#endif

/****************************************************************************************************//**
 *                                           FS_NOR_FTL_Query()
 *
 * @brief    Get block device information.
 *
 * @param    p_blk_dev   Pointer to a block device (NOR).
 *
 * @param    p_info      Pointer to structure that will receive device information.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_NOR_FTL_Query(FS_BLK_DEV      *p_blk_dev,
                             FS_BLK_DEV_INFO *p_info,
                             RTOS_ERR        *p_err)
{
  FS_NOR_FTL *p_nor_ftl;

  PP_UNUSED_PARAM(p_err);

  p_nor_ftl = (FS_NOR_FTL *)p_blk_dev;

  p_info->LbSizeLog2 = p_nor_ftl->SecSizeLog2;
  p_info->LbCnt = p_nor_ftl->DiskSizeSec;
  p_info->Fixed = DEF_YES;
}

/****************************************************************************************************//**
 *                                               FS_NOR_FTL_Trim()
 *
 * @brief    Trim unused logical blocks.
 *
 * @param    p_blk_dev   Pointer to a block device (NOR).
 *
 * @param    lb_nbr      Number of the logical block to be trimmed.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NOR_FTL_Trim(FS_BLK_DEV *p_blk_dev,
                            FS_LB_NBR  lb_nbr,
                            RTOS_ERR   *p_err)
{
  FS_NOR_FTL *p_nor_ftl;
  FS_LB_NBR  sec_nbr_phy;

  p_nor_ftl = (FS_NOR_FTL *)p_blk_dev;

  sec_nbr_phy = FS_NOR_FTL_L2P_GetEntry(p_nor_ftl, lb_nbr);
  if (sec_nbr_phy != FS_DEV_NOR_SEC_NBR_INVALID) {
    FS_NOR_FTL_InvalidateSec(p_nor_ftl, sec_nbr_phy, p_err);
    FS_NOR_FTL_L2P_SetEntry(p_nor_ftl,
                            lb_nbr,
                            FS_DEV_NOR_SEC_NBR_INVALID);
    FS_BLK_DEV_CTR_STAT_INC(p_nor_ftl->StatReleaseCtr);
  }
}
#endif

/****************************************************************************************************//**
 *                                               FS_NOR_FTL_Sync()
 *
 * @brief    Sync NOR (dummy function).
 *
 * @param    p_blk_dev   Pointer to a block device (NOR).
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NOR_FTL_Sync(FS_BLK_DEV *p_blk_dev,
                            RTOS_ERR   *p_err)
{
  PP_UNUSED_PARAM(p_blk_dev);
  PP_UNUSED_PARAM(p_err);
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           FS_NOR_FTL_Alloc()
 *
 * @brief    Allocate a NOR FTL instance.
 *
 * @param    p_nor   Pointer to a NOR instance.
 *
 * @param    p_cfg   Pointer to a NOR FTL configuration structure.
 *
 * @param    p_err   Error pointer.
 *
 * @return   Pointer to the allocated FTL instance.
 *******************************************************************************************************/
static FS_NOR_FTL *FS_NOR_FTL_Alloc(FS_NOR               *p_nor,
                                    const FS_NOR_FTL_CFG *p_cfg,
                                    RTOS_ERR             *p_err)
{
  FS_NOR_FTL *p_nor_ftl;

  if (p_cfg == DEF_NULL) {
    p_cfg = &FS_NOR_CfgDflt;
  }

  //                                                               Validate rsvd pct.
  if ((p_cfg->PctRsvd < FS_NOR_FTL_PCT_RSVD_MIN)
      && (p_cfg->PctRsvd > FS_NOR_FTL_PCT_RSVD_MAX)) {
    LOG_ERR(("Rsvd space pct specified invalid."));
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);
  }

  //                                                               Validate erase cnt diff th.
  if ((p_cfg->EraseCntDiffTh < FS_NOR_FTL_ERASE_CNT_DIFF_TH_MIN)
      && (p_cfg->EraseCntDiffTh > FS_NOR_FTL_ERASE_CNT_DIFF_TH_MAX)) {
    LOG_ERR(("Erase cnt diff th specified invalid."));
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);
  }

  //                                                               ------------------ INIT UNIT INFO ------------------
  p_nor_ftl = (FS_NOR_FTL *)Mem_SegAlloc("FS - NOR FTL",
                                         FSStorage_InitCfg.MemSegPtr,
                                         sizeof(FS_NOR_FTL),
                                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

#if (FS_STORAGE_CFG_CTR_STAT_EN == DEF_ENABLED)                 // Clr stat ctrs.
  p_nor_ftl->StatCopyCtr = 0u;
  p_nor_ftl->StatReleaseCtr = 0u;
  p_nor_ftl->StatInvalidBlkCtr = 0u;
#endif

  //                                                               ------------------ CALC NOR INFO -------------------
  //                                                               Calc dev info.
  FS_NOR_FTL_LogParamCalc(p_nor_ftl, p_nor, p_cfg, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  p_nor_ftl->EraseCntDiffTh = p_cfg->EraseCntDiffTh;
  p_nor_ftl->AB_Cnt = 1u;

  //                                                               Alloc dev data.
  FS_NOR_FTL_AllocDevData(p_nor, p_nor_ftl, FSStorage_InitCfg.MemSegPtr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  FS_NOR_FTL_LowUnmountInternal(p_nor_ftl);

  LOG_DBG(("NOR FLASH FOUND: Sec Size   : ", (u)FS_UTIL_PWR2(p_nor_ftl->SecSizeLog2), " bytes"));
  LOG_DBG(("                 Size       : ", (u)p_nor_ftl->DiskSizeSec, " sectors"));
  LOG_DBG(("                 Rsvd       : ", (u)p_nor_ftl->PctRsvd, "(",
           (u)(p_nor_ftl->BlkSecCnts * p_nor_ftl->BlkCntUsed - p_nor_ftl->SecCntTot), " sectors)"));
  LOG_DBG(("                 Active blks: ", (u)p_nor_ftl->AB_Cnt, " sectors"));

  return (p_nor_ftl);
}

/****************************************************************************************************//**
 *                                               FS_NOR_FTL_Get()
 *
 * @brief    Get an FTL instance from the internal FTL list.
 *
 * @param    p_media     Pointer to the media tied to FTL instance to find.
 *
 * @return   Pointer to the found FTL.
 *           DEF_NULL if no FTL is found.
 *******************************************************************************************************/
static FS_NOR_FTL *FS_NOR_FTL_Get(FS_MEDIA *p_media)
{
  FS_NOR_FTL *p_nor_ftl;
  FS_NOR_FTL *p_cur_nor_ftl;

  p_nor_ftl = DEF_NULL;
  SLIST_FOR_EACH_ENTRY(FS_NOR_FTL_ListHeadPtr, p_cur_nor_ftl, FS_NOR_FTL, ListMember) {
    if (p_cur_nor_ftl->BlkDev.MediaPtr == p_media) {
      p_nor_ftl = p_cur_nor_ftl;
      break;
    }
  }

  if (p_nor_ftl != DEF_NULL) {
    SList_Rem(&FS_NOR_FTL_ListHeadPtr, &p_nor_ftl->ListMember);
  }

  return (p_nor_ftl);
}

/****************************************************************************************************//**
 *                                       FS_NOR_FTL_LowFmtInternal()
 *
 * @brief    Low-level format NOR.
 *
 * @param    p_nor_ftl   Pointer to a NOR FTL instance.
 *
 * @param    p_nor       Pointer to a NOR FTL instance.
 *
 * @param    p_err       Error pointer.
 *
 * @note     (1) #### Preserve erase counts from previous format by reading block headers.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NOR_FTL_LowFmtInternal(FS_NOR_FTL *p_nor_ftl,
                                      FS_NOR     *p_nor,
                                      RTOS_ERR   *p_err)
{
  CPU_INT08U blk_hdr[FS_DEV_NOR_BLK_HDR_LEN];
  FS_LB_NBR  blk_ix;
  CPU_INT32U blk_addr;

  Mem_Clr((void *)&blk_hdr[0], FS_DEV_NOR_BLK_HDR_LEN);
  MEM_VAL_SET_INT32U((void *)&blk_hdr[FS_DEV_NOR_BLK_HDR_OFFSET_MARK1], FS_DEV_NOR_BLK_HDR_MARK_WORD_1);
  MEM_VAL_SET_INT32U((void *)&blk_hdr[FS_DEV_NOR_BLK_HDR_OFFSET_MARK2], FS_DEV_NOR_BLK_HDR_MARK_WORD_2);

  //                                                               See Note #1.
  MEM_VAL_SET_INT32U((void *)&blk_hdr[FS_DEV_NOR_BLK_HDR_OFFSET_ERASE_CNT], 0u);
  MEM_VAL_SET_INT16U((void *)&blk_hdr[FS_DEV_NOR_BLK_HDR_OFFSET_VER], FS_DEV_NOR_BLK_HDR_VER);
  MEM_VAL_SET_INT16U((void *)&blk_hdr[FS_DEV_NOR_BLK_HDR_OFFSET_SEC_SIZE], FS_UTIL_PWR2(p_nor_ftl->SecSizeLog2));
  MEM_VAL_SET_INT16U((void *)&blk_hdr[FS_DEV_NOR_BLK_HDR_OFFSET_BLK_CNT], p_nor_ftl->BlkCntUsed);

  for (blk_ix = 0u; blk_ix < p_nor_ftl->BlkCntUsed; blk_ix++) {
    blk_addr = FS_NOR_FTL_BlkIxToAddr(p_nor_ftl, blk_ix, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    //                                                             -------------------- ERASE BLK ---------------------
    FS_NOR_PHY_BlkEraseInternal(p_nor,
                                blk_ix,
                                p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      LOG_ERR(("Failed to erase blk ", (u)p_nor_ftl->BlkNbrFirst + blk_ix, " (", (u)blk_addr, ")."));
      return;
    }

    LOG_DBG(("Erased blk ", (u)p_nor_ftl->BlkNbrFirst + blk_ix, " (", (u)blk_addr, ")."));

    //                                                             -------------------- WR BLK HDR --------------------
    FS_NOR_PHY_WrInternal(p_nor,
                          (void *)&blk_hdr[0],
                          blk_addr,
                          FS_DEV_NOR_BLK_HDR_LEN,
                          p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      LOG_ERR(("Failed to wr blk hdr ", (u)p_nor_ftl->BlkNbrFirst + blk_ix, " (", (u)blk_addr, ")."));
      return;
    }
  }

  LOG_DBG(("Low-level fmt'ing complete."));
}
#endif

/****************************************************************************************************//**
 *                                       FS_NOR_FTL_LowMountInternal()
 *
 * @brief    Low-level mount NOR.
 *
 * @param    p_nor_ftl   Pointer to a NOR instance.
 *
 * @param    p_err       Error pointer.
 *
 * @note     (1) A block may be invalid upon mount if the erase process was interrupted by power loss
 *               after the block was erased, but before the block header was written.
 *               - (a) If more than one block is invalid, then the device is (probably) NOT formatted
 *                     OR it has been corrupted.
 *
 * @note     (2) At most one invalid block should be found (see also Note #2).  This block MUST be
 *               erased & a correct header written prior to other operations which might created
 *               invalid blocks.
 *
 * @note     (3) Two physical sectors may be assigned the same logical sector upon mount if the write
 *               process was interrupted after writing the 'new' sector data & header, but before
 *               invalidating the old sector.  It is impossible to determine which is the 'old' data;
 *               the first physical sector discovered is kept.  Any subsequent sectors will be marked
 *               invalid.
 *
 * @note     (4) There may be no erased blocks upon mount if a block move process was interrupted
 *               before copying all sectors & erasing & re-marking the emptied block.  It will be
 *               possible to resume that move, since the number of erased sectors in the active block
 *               (i.e., the block to which data was being moved) will be less than the number of valid
 *               sectors in the source block.  Additional writes may disturb this, so a block MUST be
 *               erased here.
 *               - (a) The block selected SHOULD be the block with the fewest valid sectors.  The min/
 *                     max erase counts are assigned after block erasure to ensure that wear-leveling
 *                     concerns are not factored into block selection.
 *               - (b) The block selected MUST NOT be the active block.  The 'valid' sectors of the
 *                     active block are the valid written sectors plus the erased sectors.  If this were
 *                     less than the valid sector count of any other block, the move would be impossible.
 *               - (c) The active block MUST have more erased sectors than valid sectors in the selected
 *                     block.
 *
 * @note     (5) There may be multiple blocks found upon mount with valid or invalid sectors AND
 *               erased sectors, if the active block was selected for erasure, but the block move
 *               was interrupted before copying all sectors.  Since only one block will be selected as
 *               the active block, the erased sectors at the end of any other block MUST be classified
 *               as invalid.
 *******************************************************************************************************/
static void FS_NOR_FTL_LowMountInternal(FS_NOR_FTL *p_nor_ftl,
                                        RTOS_ERR   *p_err)
{
  FS_NOR      *p_nor;
  CPU_INT08U  blk_hdr[FS_DEV_NOR_BLK_HDR_LEN];
  CPU_INT08U  sec_hdr[FS_DEV_NOR_SEC_HDR_LEN];
  CPU_BOOLEAN active;
  CPU_INT32U  blk_addr;
  FS_LB_QTY   blk_cnt;
  FS_LB_QTY   blk_cnt_erased;
  FS_LB_QTY   blk_cnt_valid;
  FS_LB_NBR   blk_ix;
  FS_LB_NBR   blk_ix_invalid;
  CPU_BOOLEAN erased;
  CPU_INT32U  erase_cnt;
  CPU_INT32U  erase_cnt_min;
  CPU_INT32U  erase_cnt_max;
  CPU_INT32U  mark1;
  CPU_INT32U  mark2;
  CPU_INT16U  ver;
  FS_LB_QTY   sec_cnt_erased;
  FS_LB_QTY   sec_cnt_invalid;
  FS_LB_QTY   sec_cnt_valid;
  FS_LB_NBR   sec_ix;
  FS_LB_NBR   sec_ix_next;
  CPU_INT32U  sec_nbr_logical;
  FS_LB_NBR   sec_nbr_phy;
  FS_LB_NBR   sec_nbr_phy_old;
  FS_LB_SIZE  sec_size;
  CPU_BOOLEAN sec_valid;
  CPU_INT32U  status;

  p_nor = (FS_NOR *)p_nor_ftl->BlkDev.MediaPtr;

  //                                                               ----------------- VERIFY BLK HDRS ------------------
  blk_ix_invalid = FS_DEV_NOR_SEC_NBR_INVALID;
  blk_cnt_valid = 0u;
  erase_cnt_min = DEF_INT_32U_MAX_VAL;
  erase_cnt_max = 0u;
  for (blk_ix = 0u; blk_ix < p_nor_ftl->BlkCntUsed; blk_ix++) {
    //                                                             Rd blk hdr.
    FS_NOR_FTL_RdBlkHdr(p_nor_ftl, &blk_hdr[0], blk_ix, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    //                                                             Chk marks & ver.
    mark1 = MEM_VAL_GET_INT32U((void *)&blk_hdr[FS_DEV_NOR_BLK_HDR_OFFSET_MARK1]);
    mark2 = MEM_VAL_GET_INT32U((void *)&blk_hdr[FS_DEV_NOR_BLK_HDR_OFFSET_MARK2]);
    erase_cnt = MEM_VAL_GET_INT32U((void *)&blk_hdr[FS_DEV_NOR_BLK_HDR_OFFSET_ERASE_CNT]);
    ver = MEM_VAL_GET_INT16U((void *)&blk_hdr[FS_DEV_NOR_BLK_HDR_OFFSET_VER]);
    sec_size = (FS_LB_SIZE)MEM_VAL_GET_INT16U((void *)&blk_hdr[FS_DEV_NOR_BLK_HDR_OFFSET_SEC_SIZE]);
    blk_cnt = (FS_LB_QTY)MEM_VAL_GET_INT16U((void *)&blk_hdr[FS_DEV_NOR_BLK_HDR_OFFSET_BLK_CNT]);
    if ((mark1 == FS_DEV_NOR_BLK_HDR_MARK_WORD_1)
        && (mark2 == FS_DEV_NOR_BLK_HDR_MARK_WORD_2)
        && (ver == FS_DEV_NOR_BLK_HDR_VER)
        && (sec_size == FS_UTIL_PWR2(p_nor_ftl->SecSizeLog2))
        && (blk_cnt == p_nor_ftl->BlkCntUsed)) {
      blk_cnt_valid++;

      if (erase_cnt_min > erase_cnt) {
        erase_cnt_min = erase_cnt;
      }
      if (erase_cnt_max < erase_cnt) {
        erase_cnt_max = erase_cnt;
      }
    } else {                                                    // Blk is NOT valid (see Note #1).
      blk_ix_invalid = blk_ix;
    }
  }

  if (blk_cnt_valid < p_nor_ftl->BlkCntUsed - 1u) {             // If more than 1 invalid blk (see Note #1a) ...
    LOG_ERR(("Low-level format invalid: ", (u)p_nor_ftl->BlkCntUsed - blk_cnt_valid, " invalid blks found."));
    RTOS_ERR_SET(*p_err, RTOS_ERR_BLK_DEV_FMT_INVALID);         // ... low fmt invalid.
    return;
  }

  //                                                               ---------------- CONSTRUCT L2P TBL -----------------
  p_nor_ftl->SecCntValid = 0u;                                  // Clr sec cnts.
  p_nor_ftl->SecCntErased = 0u;
  p_nor_ftl->SecCntInvalid = 0u;

  sec_nbr_phy = 0u;
  blk_cnt_valid = 0u;
  blk_cnt_erased = 0u;
  for (blk_ix = 0u; blk_ix < p_nor_ftl->BlkCntUsed; blk_ix++) {
#if LOG_DBG_IS_EN()
    FS_NOR_FTL_RdBlkHdr(p_nor_ftl, &blk_hdr[0], blk_ix, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
    erase_cnt = MEM_VAL_GET_INT32U((void *)&blk_hdr[FS_DEV_NOR_BLK_HDR_OFFSET_ERASE_CNT]);
#endif

    if (blk_ix == blk_ix_invalid) {                             // If blk invalid, all secs invalid & should be erased.
      p_nor_ftl->SecCntInvalid += p_nor_ftl->BlkSecCnts;
      sec_nbr_phy += p_nor_ftl->BlkSecCnts;
      LOG_ERR(("Invalid blk found: ", (u)p_nor_ftl->BlkNbrFirst + blk_ix, "."));
      sec_cnt_valid = 0u;

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
      blk_addr = FS_NOR_FTL_BlkIxToAddr(p_nor_ftl, blk_ix, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return;
      }

      Mem_Clr(&blk_hdr[0], FS_DEV_NOR_BLK_HDR_LEN);
      MEM_VAL_SET_INT32U((void *)&blk_hdr[FS_DEV_NOR_BLK_HDR_OFFSET_MARK1], FS_DEV_NOR_BLK_HDR_MARK_WORD_1);
      MEM_VAL_SET_INT32U((void *)&blk_hdr[FS_DEV_NOR_BLK_HDR_OFFSET_MARK2], FS_DEV_NOR_BLK_HDR_MARK_WORD_2);
      MEM_VAL_SET_INT32U((void *)&blk_hdr[FS_DEV_NOR_BLK_HDR_OFFSET_ERASE_CNT], erase_cnt_max);
      MEM_VAL_SET_INT16U((void *)&blk_hdr[FS_DEV_NOR_BLK_HDR_OFFSET_VER], FS_DEV_NOR_BLK_HDR_VER);
      MEM_VAL_SET_INT16U((void *)&blk_hdr[FS_DEV_NOR_BLK_HDR_OFFSET_SEC_SIZE], FS_UTIL_PWR2(p_nor_ftl->SecSizeLog2));
      MEM_VAL_SET_INT16U((void *)&blk_hdr[FS_DEV_NOR_BLK_HDR_OFFSET_BLK_CNT], p_nor_ftl->BlkCntUsed);

      FS_NOR_PHY_BlkEraseInternal(p_nor,                        // Erase blk ...
                                  blk_ix,
                                  p_err);

      FS_NOR_PHY_WrInternal(p_nor,                              // ... & wr hdr (see Note #2).
                            &blk_hdr[0],
                            blk_addr,
                            FS_DEV_NOR_BLK_HDR_LEN,
                            p_err);

      erased = DEF_YES;
#else
      erased = DEF_NO;
#endif
    } else {
      blk_addr = FS_NOR_FTL_BlkIxToAddr(p_nor_ftl, blk_ix, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return;
      }
      blk_addr += FS_DEV_NOR_BLK_HDR_LEN;

      //                                                           Chk hdr of each sec.
      sec_ix = 0u;
      sec_ix_next = 0u;
      sec_valid = DEF_NO;
      sec_cnt_erased = 0u;
      sec_cnt_valid = 0u;
      sec_cnt_invalid = 0u;
      while (sec_ix < p_nor_ftl->BlkSecCnts) {
        FS_NOR_PHY_RdInternal(p_nor,
                              &sec_hdr[0],
                              blk_addr,
                              FS_DEV_NOR_SEC_HDR_LEN,
                              p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          return;
        }
        sec_nbr_logical = MEM_VAL_GET_INT32U((void *)&sec_hdr[FS_DEV_NOR_SEC_HDR_OFFSET_SEC_NBR]);
        status = MEM_VAL_GET_INT32U((void *)&sec_hdr[FS_DEV_NOR_SEC_HDR_OFFSET_STATUS]);

        switch (status) {
          case FS_DEV_NOR_STATUS_SEC_ERASED:
            sec_cnt_erased++;
            break;

          case FS_DEV_NOR_STATUS_SEC_VALID:
            if (sec_nbr_logical < p_nor_ftl->DiskSizeSec) {
              sec_nbr_phy_old = FS_NOR_FTL_L2P_GetEntry(p_nor_ftl,
                                                        sec_nbr_logical);
              if (sec_nbr_phy_old == FS_DEV_NOR_SEC_NBR_INVALID) {
                FS_NOR_FTL_L2P_SetEntry(p_nor_ftl,
                                        sec_nbr_logical,
                                        sec_nbr_phy);
                sec_cnt_valid++;
                sec_valid = DEF_YES;                            // Blk has valid data.
              } else {                                          // Duplicate entry (see Note #3).
#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
                MEM_VAL_SET_INT32U((void *)&sec_hdr[FS_DEV_NOR_SEC_HDR_OFFSET_STATUS], FS_DEV_NOR_STATUS_SEC_INVALID);
                //                                                 Mark sec as 'invalid'.
                FS_NOR_PHY_WrInternal(p_nor,
                                      &sec_hdr[FS_DEV_NOR_SEC_HDR_OFFSET_STATUS],
                                      blk_addr + FS_DEV_NOR_SEC_HDR_OFFSET_STATUS,
                                      4u,
                                      p_err);
#endif

                LOG_ERR(("Logical sec ", (u)sec_nbr_logical, " already assigned to phy sec ", (u)sec_nbr_phy_old, " before ", (u)sec_nbr_phy, "."));
                sec_cnt_invalid++;
              }
            } else {
              sec_cnt_invalid++;
            }
            sec_cnt_invalid += sec_cnt_erased;                  // No erased secs precede valid secs.
            sec_cnt_erased = 0u;
            sec_ix_next = sec_ix + 1u;                          // Set next potentially erased sec.
            break;

          case FS_DEV_NOR_STATUS_SEC_WRITING:                   // No erased secs precede invalid secs.
          case FS_DEV_NOR_STATUS_SEC_INVALID:
          default:
            sec_cnt_invalid += sec_cnt_erased + 1u;
            sec_cnt_erased = 0u;
            sec_ix_next = sec_ix + 1u;                          // Set next potentially erased sec.
            break;
        }

        blk_addr += FS_UTIL_PWR2(p_nor_ftl->SecSizeLog2) + FS_DEV_NOR_SEC_HDR_LEN;
        sec_ix++;
        sec_nbr_phy++;
      }

      if ((sec_ix_next == p_nor_ftl->BlkSecCnts)                // If final sec is invalid     ...
          && (sec_valid == DEF_NO)) {                           // ... & blk has no valid data ...
                                                                // ... add blk to erase q.
        erased = DEF_NO;
        LOG_DBG(("Valid blk found ", (u)p_nor_ftl->BlkNbrFirst + blk_ix, " w/ erase cnt ", (u)erase_cnt, "; blk should be erased."));
      } else if (sec_ix_next == 0u) {
        erased = DEF_YES;
        blk_cnt_erased++;
        LOG_DBG(("Valid blk found ", (u)p_nor_ftl->BlkNbrFirst + blk_ix, " w/ erase cnt ", (u)erase_cnt, "; blk is erased."));
      } else {
        erased = DEF_NO;

        if (sec_cnt_erased > 0u) {
          active = FS_NOR_FTL_AB_Add(p_nor_ftl,                 // Try to add to list of active blks ...
                                     blk_ix,
                                     sec_ix_next);
          if (active != DEF_OK) {                               // ... if it could NOT be added      ...
            LOG_DBG(("Could not make blk ", (u)blk_ix, " active blk; reclassifying ", (u)sec_cnt_erased, " erased secs as invalid."));
            sec_cnt_invalid += sec_cnt_erased;                  // ... reclassify secs as invalid.
            sec_cnt_erased = 0u;
          }
        }

        if ((sec_cnt_erased != 0u) || (sec_cnt_valid != 0u)) {
          blk_cnt_valid++;
          LOG_DBG(("Valid blk found ", (u)p_nor_ftl->BlkNbrFirst + blk_ix, " w/ erase cnt ", (u)erase_cnt, ";"));
          LOG_DBG(("blk has ", (u)sec_cnt_valid, " valid, ", (u)(sec_ix_next == FS_DEV_NOR_SEC_NBR_INVALID) ? 0u : (p_nor_ftl->BlkSecCnts - sec_ix_next), " erased secs."));
        } else {
          LOG_DBG(("Valid blk found ", (u)(p_nor_ftl->BlkNbrFirst + blk_ix), " w/ erase cnt ", (u)erase_cnt, "; blk should be erased."));
        }
      }

      p_nor_ftl->SecCntErased += sec_cnt_erased;
      p_nor_ftl->SecCntValid += sec_cnt_valid;
      p_nor_ftl->SecCntInvalid += sec_cnt_invalid;
    }

    FS_NOR_FTL_SetBlkErased(p_nor_ftl,                          // Update erase map.
                            blk_ix,
                            erased);

    FS_NOR_FTL_SetBlkSecCntValid(p_nor_ftl,                     // Update sec cnt valid tbl.
                                 blk_ix,
                                 sec_cnt_valid);
  }

  //                                                               ------------------- UPDATE INFO --------------------
  p_nor_ftl->BlkWearLevelAvail = DEF_NO;

  p_nor_ftl->BlkCntValid = blk_cnt_valid;
  p_nor_ftl->BlkCntErased = blk_cnt_erased;
  p_nor_ftl->BlkCntInvalid = (p_nor_ftl->BlkCntUsed - blk_cnt_valid) - blk_cnt_erased;

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
  if ((p_nor_ftl->BlkCntErased == 0u)                           // If no erased blks (see Note #4) ...
      && (p_nor_ftl->BlkCntInvalid == 0u)) {
    blk_ix = FS_NOR_FTL_FindEraseBlk(p_nor_ftl);

    if (blk_ix != FS_DEV_NOR_SEC_NBR_INVALID) {
      active = FS_NOR_FTL_IsAB(p_nor_ftl, blk_ix);
      if (active == DEF_YES) {                                  // See Note #4b.
        LOG_ERR(("Cannot mount dev; no erase blk, active blk has fewest valid secs."));
        RTOS_ERR_SET(*p_err, RTOS_ERR_BLK_DEV_FMT_INVALID);
        return;
      }

      FS_NOR_FTL_GetBlkInfo(p_nor_ftl,
                            blk_ix,
                            &erased,
                            &sec_cnt_valid);
      //                                                           See Note #4c.
      sec_cnt_erased = FS_NOR_FTL_AB_SecCntTotErased(p_nor_ftl);
      if (sec_cnt_valid > sec_cnt_erased) {
        LOG_ERR(("Cannot mount dev; no erase blk, active blk has too few erased secs (", (u)sec_cnt_erased, " < ", (u)sec_cnt_valid, ")."));
        RTOS_ERR_SET(*p_err, RTOS_ERR_BLK_DEV_FMT_INVALID);
        return;
      }

      FS_NOR_FTL_EraseBlkPrepare(p_nor_ftl,                     // ... erase 1 blk.
                                 blk_ix,
                                 p_err);
    }
  }
#endif

  p_nor_ftl->EraseCntMin = erase_cnt_min;                       // See Note #4a.
  p_nor_ftl->EraseCntMax = erase_cnt_max;

  p_nor_ftl->IsMounted = DEF_YES;

  LOG_DBG(("NOR FLASH MOUNT:       Blks valid   : ", (u)p_nor_ftl->BlkCntValid));
  LOG_DBG(("                            erased  : ", (u)p_nor_ftl->BlkCntErased));
  LOG_DBG(("                            erase q : ", (u)(p_nor_ftl->BlkCntUsed - p_nor_ftl->BlkCntValid) - p_nor_ftl->BlkCntErased));
  LOG_DBG(("                       Secs valid   : ", (u)p_nor_ftl->SecCntValid));
  LOG_DBG(("                            erased  : ", (u)p_nor_ftl->SecCntErased));
  LOG_DBG(("                            invalid : ", (u)p_nor_ftl->SecCntInvalid));
  LOG_DBG(("                       Erase cnt min: ", (u)p_nor_ftl->EraseCntMin));
  LOG_DBG(("                       Erase cnt max: ", (u)p_nor_ftl->EraseCntMax));
}

/****************************************************************************************************//**
 *                                       FS_NOR_FTL_LowUnmountInternal()
 *
 * @brief    Low-level unmount NOR.
 *
 * @param    p_nor_ftl   Pointer to a NOR FTL instance.
 *******************************************************************************************************/
static void FS_NOR_FTL_LowUnmountInternal(FS_NOR_FTL *p_nor_ftl)
{
  FS_LB_NBR blk_ix;
  FS_LB_NBR sec_nbr_logical;

  //                                                               ------------------- CLR BLK INFO -------------------
  for (blk_ix = 0u; blk_ix < p_nor_ftl->BlkCntUsed; blk_ix++) {
    FS_NOR_FTL_SetBlkErased(p_nor_ftl,                          // Set each entry as NOT erased.
                            blk_ix,
                            DEF_NO);

    FS_NOR_FTL_SetBlkSecCntValid(p_nor_ftl,                     // Set each sec cnt invalid.
                                 blk_ix,
                                 FS_DEV_NOR_SEC_NBR_INVALID);

    blk_ix++;
  }

  //                                                               ------------------- CLR L2P TBL --------------------
  for (sec_nbr_logical = 0u; sec_nbr_logical < p_nor_ftl->DiskSizeSec; sec_nbr_logical++) {
    FS_NOR_FTL_L2P_SetEntry(p_nor_ftl,
                            sec_nbr_logical,
                            FS_DEV_NOR_SEC_NBR_INVALID);
  }

  FS_NOR_FTL_AB_ClrAll(p_nor_ftl);                              // CLR ABs

  //                                                               ------------------- CLR NOR INFO -------------------
  p_nor_ftl->BlkWearLevelAvail = DEF_NO;
  p_nor_ftl->BlkCntValid = 0u;
  p_nor_ftl->BlkCntErased = 0u;
  p_nor_ftl->BlkCntInvalid = 0u;

  p_nor_ftl->SecCntValid = 0u;
  p_nor_ftl->SecCntErased = 0u;
  p_nor_ftl->SecCntInvalid = 0u;

  p_nor_ftl->EraseCntMin = 0u;
  p_nor_ftl->EraseCntMax = 0u;

  p_nor_ftl->IsMounted = DEF_NO;
}

/****************************************************************************************************//**
 *                                       FS_NOR_FTL_LowCompactInternal()
 *
 * @brief    Low-level compact NOR.
 *
 * @param    p_nor_ftl   Pointer to a NOR FTL instance.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NOR_FTL_LowCompactInternal(FS_NOR_FTL *p_nor_ftl,
                                          RTOS_ERR   *p_err)
{
  FS_LB_NBR   blk_ix;
  FS_LB_QTY   sec_cnt_valid;
  CPU_BOOLEAN erased;
  CPU_BOOLEAN active;

  LOG_DBG(("NOR FLASH COMPACT: BEFORE: Blks valid   : ", (u)p_nor_ftl->BlkCntValid));
  LOG_DBG(("                                erased  : ", (u)p_nor_ftl->BlkCntErased));
  LOG_DBG(("                                erase q : ", (u)(p_nor_ftl->BlkCntUsed - p_nor_ftl->BlkCntValid) - p_nor_ftl->BlkCntErased));
  LOG_DBG(("                           Secs valid   : ", (u)p_nor_ftl->SecCntValid));
  LOG_DBG(("                                erased  : ", (u)p_nor_ftl->SecCntErased));
  LOG_DBG(("                                invalid : ", (u)p_nor_ftl->SecCntInvalid));

  for (blk_ix = 0u; blk_ix < p_nor_ftl->BlkCntUsed; blk_ix++) {
    FS_NOR_FTL_GetBlkInfo(p_nor_ftl,                            // Get blk info.
                          blk_ix,
                          &erased,
                          &sec_cnt_valid);

    if (!erased) {                                              // If blk is NOT erased      ...
      if (sec_cnt_valid != p_nor_ftl->BlkSecCnts) {             // ... & blk had invalid secs ...
                                                                // ... move blk data.
        active = FS_NOR_FTL_IsAB(p_nor_ftl, blk_ix);
        if (!active) {
          FS_NOR_FTL_EraseBlkPrepare(p_nor_ftl, blk_ix, p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            return;
          }
        }
      }
    }
  }

  LOG_DBG(("                    AFTER: Blks valid   : ", (u)p_nor_ftl->BlkCntValid));
  LOG_DBG(("                                erased  : ", (u)p_nor_ftl->BlkCntErased));
  LOG_DBG(("                                erase q : ", (u)(p_nor_ftl->BlkCntUsed - p_nor_ftl->BlkCntValid) - p_nor_ftl->BlkCntErased));
  LOG_DBG(("                           Secs valid   : ", (u)p_nor_ftl->SecCntValid));
  LOG_DBG(("                                erased  : ", (u)p_nor_ftl->SecCntErased));
  LOG_DBG(("                                invalid : ", (u)p_nor_ftl->SecCntInvalid));
}
#endif

/****************************************************************************************************//**
 *                                           FS_NOR_FTL_LogParamCalc()
 *
 * @brief    Calculate NOR device info.
 *
 * @param    p_nor_ftl   Pointer to a NOR FTL instance.
 *
 * @param    p_nor       Pointer to a NOR instance.
 *
 * @param    p_nor_cfg   Pointer to a NOR FTL configuration structure.
 *
 * @param    p_err       Error pointer.
 *
 * @note     (1) Block & block region information supplied by the phy driver are strictly checked :
 *               - (a) As least one block must exist.
 *               - (b) All blocks must be large enough for at least one data sector.
 *               - (c) The configured data region start must lie within the block region.
 *
 * @note     (2) At least one block's worth of sectors MUST be reserved.
 *******************************************************************************************************/
static void FS_NOR_FTL_LogParamCalc(FS_NOR_FTL           *p_nor_ftl,
                                    FS_NOR               *p_nor,
                                    const FS_NOR_FTL_CFG *p_nor_cfg,
                                    RTOS_ERR             *p_err)
{
  FS_NOR_PHY *p_nor_phy;
  FS_LB_QTY  ftl_blk_cnt;
  FS_LB_NBR  rsvd_blk_cnt;
  FS_LB_SIZE data_size_blk;
  FS_LB_QTY  sec_cnt_blk;
  FS_LB_QTY  ftl_sec_cnt;
  FS_LB_QTY  sec_cnt_rsvd;
  FS_LB_SIZE sec_size;
  CPU_INT32U blk_size;
  CPU_INT32U blk_cnt;

  p_nor_phy = p_nor->PhyPtr;
  blk_cnt = p_nor_phy->BlkCnt;
  blk_size = FS_UTIL_PWR2(p_nor_phy->BlkSizeLog2);

  //                                                               ------------ CHK PHY INFO (see Note #1) ------------
  //                                                               Validate blk cnt.
  RTOS_ASSERT_DBG_ERR_SET(blk_cnt > 0u, *p_err, RTOS_ERR_INVALID_CFG,; );

  sec_size = p_nor_cfg->SecSize;
  if (sec_size == 0u) {
    sec_size = DEF_MIN(blk_size, FS_DEV_NOR_LOG_SEC_SIZE_DFLT);
  }

  switch (sec_size) {                                           // Validate sec size.
    case 512u:
    case 1024u:
    case 2048u:
    case 4096u:
      break;

    default:
      LOG_ERR(("Invalid sector size."));
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG,; );
  }

  //                                                               Cfg'd info.
  p_nor_ftl->SecSizeLog2 = FSUtil_Log2(sec_size);
  p_nor_ftl->PctRsvd = p_nor_cfg->PctRsvd;

  //                                                               Validate blk size.
  RTOS_ASSERT_DBG_ERR_SET(sec_size + FS_DEV_NOR_BLK_HDR_LEN + FS_DEV_NOR_SEC_HDR_LEN <= blk_size,
                          *p_err, RTOS_ERR_INVALID_CFG,; );

#if 0
  if (p_nor_ftl->PhyPtr->AddrRegionStart < p_nor_ftl->PhyPtr->AddrBase) {
    LOG_ERR(("Phy info invalid: AddrRegionStart = ", (u)p_nor_ftl->PhyPtr->AddrRegionStart, " < ", (u)p_nor_ftl->PhyPtr->AddrBase, "."));
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_LOW_FMT_PARAMS);
    return;
  }
#endif

  //                                                               ------------ FIND 1ST BLK OF FILE SYSTEM -----------
#if 0
  rsvd_blk_cnt = 0u;
  blk_addr = p_nor_ftl->PhyPtr->AddrRegionStart;
  while ((blk_addr < data_start_offset)                         // While addr of blk before start of file system ...
         && (rsvd_blk_cnt < p_nor_ftl->PhyPtr->BlkCnt)) {       // ... & more blocks rem                         ...
                                                                // ... get start addr of next blk.
    blk_addr += (CPU_ADDR)p_nor_ftl->PhyPtr->BlkSize;
    rsvd_blk_cnt++;
  }
#endif

  //                                                               Number of reserved block rounded down.
  rsvd_blk_cnt = FS_UTIL_DIV_PWR2(p_nor_cfg->StartOffset, p_nor_phy->BlkSizeLog2);
  RTOS_ASSERT_DBG_ERR_SET(rsvd_blk_cnt < blk_cnt, *p_err, RTOS_ERR_INVALID_CFG,; );

  //                                                               Calc blk cnt.
  if (p_nor_cfg->DevSize == FS_NOR_CFG_DEV_SIZE_AUTO) {         // Dflt.
    ftl_blk_cnt = blk_cnt - rsvd_blk_cnt;
  } else {
    ftl_blk_cnt = FS_UTIL_DIV_PWR2(p_nor_cfg->DevSize, p_nor_phy->BlkSizeLog2);
    if (ftl_blk_cnt > blk_cnt - rsvd_blk_cnt) {
      ftl_blk_cnt = blk_cnt - rsvd_blk_cnt;
    }
  }

  //                                                               Calc secs/blk for size.
  sec_cnt_blk = FS_UTIL_DIV_PWR2((blk_size - FS_DEV_NOR_BLK_HDR_LEN), p_nor_ftl->SecSizeLog2);
  data_size_blk = FS_DEV_NOR_BLK_HDR_LEN + FS_UTIL_MULT_PWR2(sec_cnt_blk, p_nor_ftl->SecSizeLog2)
                  + FS_UTIL_MULT_PWR2(sec_cnt_blk, FS_DEV_NOR_SEC_HDR_LEN_LOG);
  while (data_size_blk > blk_size) {
    data_size_blk -= sec_size + FS_DEV_NOR_SEC_HDR_LEN;
    sec_cnt_blk--;
  }

  ftl_sec_cnt = sec_cnt_blk * ftl_blk_cnt;

  //                                                               ------------------ STO INFO & RTN ------------------
  p_nor_ftl->BlkCntUsed = ftl_blk_cnt;
  p_nor_ftl->BlkNbrFirst = rsvd_blk_cnt;
  p_nor_ftl->BlkSecCnts = sec_cnt_blk;
  p_nor_ftl->SecCntTot = ftl_sec_cnt;
  p_nor_ftl->SecCntTotLog = FSUtil_Log2(ftl_sec_cnt);

  sec_cnt_rsvd = (ftl_sec_cnt * p_nor_ftl->PctRsvd) / 100u;
  if (sec_cnt_rsvd < sec_cnt_blk) {                             // If sec cnt rsvd less than sec cnt blk ...
    sec_cnt_rsvd = sec_cnt_blk;                                 // ... sec cnt rsvd equals sec cnt blk (see Note #2).
  }

  p_nor_ftl->DiskSizeSec = ftl_sec_cnt - sec_cnt_rsvd;
}

/****************************************************************************************************//**
 *                                           FS_NOR_FTL_AllocDevData()
 *
 * @brief    Allocate NOR FTL internal data structures.
 *
 * @param    p_nor       Pointer to a NOR instance
 *
 * @param    p_nor_ftl   Pointer to a NOR FTL instance.
 *
 * @param    p_seg       Pointer to a memory segment where to allocate the FTL data.
 *
 * @param    p_err       Error pointer.
 *
 * @note     (1) The device data requires memory allocated from the heap :
 *          @verbatim
 *           - (a) AB_IxTbl               : sizeof(FS_LB_NBR ) * AB_Cnt
 *                                           +
 *           - (b) AB_SecNextTbl          : sizeof(FS_LB_QTY ) * AB_Cnt
 *                                           +
 *           - (c) BlkEraseMap            : sizeof(CPU_INT08U) * ceil(BlkCntUsed / 8)
 *                                           +
 *           - (d) SecCntTbl
 *                   If BlkSecCnts < 256  : sizeof(CPU_INT08U) * BlkSecCnts      OR
 *                   Else                   sizeof(CPU_INT16U) * BlkSecCnts
 *                                           +
 *           - (e) L2P_Tbl                : log2(SecCntTot)    * Size
 *                                           +
 *           - (f) Buf                    : sizeof(CPU_INT08U) * SecSize
 *          @endverbatim
 *******************************************************************************************************/
static void FS_NOR_FTL_AllocDevData(FS_NOR     *p_nor,
                                    FS_NOR_FTL *p_nor_ftl,
                                    MEM_SEG    *p_seg,
                                    RTOS_ERR   *p_err)
{
  FS_LB_NBR  *p_ab_ix_tbl;
  FS_LB_NBR  *p_ab_sec_next_tbl;
  void       *p_buf;
  void       *p_erase_map;
  void       *p_l2p_tbl;
  void       *p_sec_cnt_tbl;
  CPU_SIZE_T align_req;
  CPU_SIZE_T erase_map_size;
  CPU_SIZE_T octets_reqd;
  CPU_SIZE_T sec_cnt_tbl_size;

  //                                                               --------------- ALLOC AV SEC NEXT TBL --------------
  p_ab_ix_tbl = (FS_LB_NBR *)Mem_SegAlloc("FS - NOR AB ix tbl",
                                          p_seg,
                                          sizeof(FS_LB_NBR) * (CPU_SIZE_T)p_nor_ftl->AB_Cnt,
                                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
  p_nor_ftl->AB_IxTbl = p_ab_ix_tbl;

  //                                                               -------------- ALLOC AB SEC NEXT TBL ---------------
  p_ab_sec_next_tbl = (FS_LB_QTY *)Mem_SegAlloc("FS - AB sec next tbl",
                                                p_seg,
                                                sizeof(FS_LB_QTY) * (CPU_SIZE_T)p_nor_ftl->AB_Cnt,
                                                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
  p_nor_ftl->AB_SecNextTbl = p_ab_sec_next_tbl;

  //                                                               ---------------- ALLOC BLK ERASE MAP ---------------
  erase_map_size = (((CPU_SIZE_T)p_nor_ftl->BlkCntUsed + 7u) / 8u);
  p_erase_map = (void *)Mem_SegAlloc("FS - NOR erase map",
                                     p_seg,
                                     sizeof(CPU_INT08U) * erase_map_size,
                                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
  p_nor_ftl->BlkEraseMap = p_erase_map;

  //                                                               --------------- ALLOC BLK SEC CNT TBL --------------
  sec_cnt_tbl_size = (p_nor_ftl->BlkSecCnts < DEF_INT_08U_MAX_VAL) ? ((CPU_SIZE_T)p_nor_ftl->BlkCntUsed * sizeof(CPU_INT08U))
                     : ((CPU_SIZE_T)p_nor_ftl->BlkCntUsed * sizeof(CPU_INT16U));
  p_sec_cnt_tbl = (void *)Mem_SegAlloc("FS - NOR sec cnt tbl",
                                       p_seg,
                                       sec_cnt_tbl_size,
                                       p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
  p_nor_ftl->BlkSecCntValidTbl = p_sec_cnt_tbl;

  //                                                               ------------------- ALLOC L2P TBL ------------------
  p_l2p_tbl = FS_NOR_FTL_L2P_Create(p_nor_ftl, p_seg, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
  p_nor_ftl->L2P_Tbl = p_l2p_tbl;

  //                                                               ------------------- ALLOC SEC BUF ------------------
  align_req = p_nor->Media.PmItemPtr->MediaApiPtr->AlignReqGet(&p_nor->Media, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_buf = (void *)Mem_SegAllocExt("FS - NOR buf",
                                  p_seg,
                                  FS_UTIL_PWR2(p_nor_ftl->SecSizeLog2),
                                  align_req,
                                  &octets_reqd,
                                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Could not alloc mem for buf: ", (u)octets_reqd, " octets req'd."));
    return;
  }

  p_nor_ftl->BufPtr = p_buf;
}

/****************************************************************************************************//**
 *                                           FS_NOR_FTL_RdBlkHdr()
 *
 * @brief    Read header of a block.
 *
 * @param    p_nor_ftl   Pointer to a NOR FTL instance.
 *
 * @param    p_dest      Pointer to destination buffer.
 *
 * @param    blk_ix      Block index.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_NOR_FTL_RdBlkHdr(FS_NOR_FTL *p_nor_ftl,
                                void       *p_dest,
                                FS_LB_NBR  blk_ix,
                                RTOS_ERR   *p_err)
{
  FS_NOR     *p_nor;
  CPU_INT32U blk_addr;

  p_nor = (FS_NOR *)p_nor_ftl->BlkDev.MediaPtr;

  //                                                               Get blk addr.
  blk_addr = FS_NOR_FTL_BlkIxToAddr(p_nor_ftl, blk_ix, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  FS_NOR_PHY_RdInternal(p_nor,                                  // Rd blk hdr.
                        p_dest,
                        blk_addr,
                        FS_DEV_NOR_BLK_HDR_LEN,
                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Failed to rd blk header ", (u)blk_ix, " ( ", (X)blk_addr, ")."));
    return;
  }
}

/****************************************************************************************************//**
 *                                           FS_NOR_FTL_RdSecLogical()
 *
 * @brief    Read sector.
 *
 * @param    p_nor_ftl           Pointer to a NOR FTL instance.
 *
 * @param    p_dest              Pointer to destination buffer.
 *
 * @param    sec_nbr_logical     Logical sector number.
 *
 * @param    p_err               Error pointer.
 *
 * @note     (1) To read a sector, this driver must :
 *               - (a) Look up physical sector number in the L2P table.
 *               - (b) Find/calculate location of physical sector.
 *               - (c) Read data.
 *******************************************************************************************************/
static void FS_NOR_FTL_RdSecLogical(FS_NOR_FTL *p_nor_ftl,
                                    void       *p_dest,
                                    FS_LB_NBR  sec_nbr_logical,
                                    RTOS_ERR   *p_err)
{
  FS_NOR     *p_nor;
  CPU_INT32U sec_addr;
  FS_LB_NBR  sec_nbr_phy;
  FS_LB_SIZE sec_size;

  p_nor = (FS_NOR *)p_nor_ftl->BlkDev.MediaPtr;

  sec_size = FS_UTIL_PWR2(p_nor_ftl->SecSizeLog2);
  sec_nbr_phy = FS_NOR_FTL_L2P_GetEntry(p_nor_ftl,              // Look up phy sec nbr (see Note #1a).
                                        sec_nbr_logical);
  if (sec_nbr_phy == FS_DEV_NOR_SEC_NBR_INVALID) {
    Mem_Set(p_dest, 0xFFu, sec_size);
  } else {
    //                                                             Get sec addr (see Note #1b).
    sec_addr = FS_NOR_FTL_SecNbrPhyToAddr(p_nor_ftl, sec_nbr_phy, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    FS_NOR_PHY_RdInternal(p_nor,                                // Rd sec (see Note #1c).
                          p_dest,
                          sec_addr + FS_DEV_NOR_SEC_HDR_LEN,
                          sec_size,
                          p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      LOG_ERR(("Failed to rd sec ", (u)sec_nbr_phy, " (", (X)sec_addr, ")."));
      return;
    }
  }
}

/****************************************************************************************************//**
 *                                           FS_NOR_FTL_WrSecLogical()
 *
 * @brief    Write sector.
 *
 * @param    p_nor_ftl           Pointer to a NOR FTL instance.
 *
 * @param    p_src               Pointer to source buffer.
 *
 * @param    sec_nbr_logical     Logical sector number.
 *
 * @param    p_err               Error pointer.
 *
 * @note     (1) To write a sector, this driver must :
 *               - (a) Check if sufficient free blocks exist in case new block must be allocated.
 *               - (b) Look up physical sector number in the L2P table.
 *                   - (1) If found, find/calculate location of physical sector.
 *                   - (2) Remember as old physical sector location.
 *               - (c) Write new physical sector.
 *                   - (1) Find/calculation location of next available physical sector.
 *                       - (A) Remember as new physical sector location.
 *                   - (2) Mark new physical sector as 'WRITING'.
 *                   - (3) Write data into physical sector.
 *                   - (4) Mark new physical sector as 'VALID'.
 *                   - (5) Update L2P table.
 *               - (d) Mark old physical sector as 'INVALID'.
 *                   - (1) If last valid sector in block :
 *                       - (A) Move block to  erase queue.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NOR_FTL_WrSecLogical(FS_NOR_FTL *p_nor_ftl,
                                    void       *p_src,
                                    FS_LB_NBR  sec_nbr_logical,
                                    RTOS_ERR   *p_err)
{
  FS_LB_NBR sec_nbr_phy_old;
  FS_LB_NBR blk_ix;

  //                                                               FREE BLK (see Note #1a)
  blk_ix = FS_NOR_FTL_FindEraseBlk(p_nor_ftl);                  // Chk if blk must be erased.
  while (blk_ix != FS_DEV_NOR_SEC_NBR_INVALID) {
    FS_NOR_FTL_EraseBlkPrepare(p_nor_ftl, blk_ix, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
    blk_ix = FS_NOR_FTL_FindEraseBlk(p_nor_ftl);
  }

  //                                                               ------------------ WR LOGICAL SEC ------------------
  //                                                               Save old phy sec (see Note #1b).
  sec_nbr_phy_old = FS_NOR_FTL_L2P_GetEntry(p_nor_ftl, sec_nbr_logical);
  FS_NOR_FTL_WrSecLogicalInternal(p_nor_ftl,                    // See Note #1c.
                                  p_src,
                                  sec_nbr_logical,
                                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               ------- INVALIDATE OLD PHY SEC (see Note #1d) ------
  if (sec_nbr_phy_old != FS_DEV_NOR_SEC_NBR_INVALID) {
    FS_NOR_FTL_InvalidateSec(p_nor_ftl, sec_nbr_phy_old, p_err);
  }
}
#endif

/****************************************************************************************************//**
 *                                       FS_NOR_FTL_WrSecLogicalInternal()
 *
 * @brief    Write sector.
 *
 * @param    p_nor_ftl           Pointer to a NOR FTL instance.
 *
 * @param    p_src               Pointer to source buffer.
 *
 * @param    sec_nbr_logical     Logical sector number.
 *
 * @param    p_err               Error pointer.
 *
 * @note     (1) See 'FS_NOR_WrSecLogical()  Note #1c'.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NOR_FTL_WrSecLogicalInternal(FS_NOR_FTL *p_nor_ftl,
                                            void       *p_src,
                                            FS_LB_NBR  sec_nbr_logical,
                                            RTOS_ERR   *p_err)
{
  FS_NOR     *p_nor;
  CPU_INT32U sec_addr;
  FS_LB_NBR  sec_nbr_phy;
  CPU_INT08U sec_hdr[FS_DEV_NOR_SEC_HDR_LEN];

  p_nor = (FS_NOR *)p_nor_ftl->BlkDev.MediaPtr;

  //                                                               ----------------- FIND NEW PHY SEC -----------------
  //                                                               Find active blk sec.
  sec_nbr_phy = FS_NOR_FTL_AB_SecFind(p_nor_ftl, sec_nbr_logical);
  if (sec_nbr_phy == FS_DEV_NOR_SEC_NBR_INVALID) {
    LOG_ERR(("Failed to get sec."));
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return;
  }

  sec_addr = FS_NOR_FTL_SecNbrPhyToAddr(p_nor_ftl, sec_nbr_phy, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               -------------------- WR NEW DATA -------------------
  MEM_VAL_SET_INT32U((void *)&sec_hdr[FS_DEV_NOR_SEC_HDR_OFFSET_STATUS],
                     FS_DEV_NOR_STATUS_SEC_WRITING);

  FS_NOR_PHY_WrInternal(p_nor,                                  // Mark sec as 'writing'.
                        &sec_hdr[FS_DEV_NOR_SEC_HDR_OFFSET_STATUS],
                        (sec_addr + FS_DEV_NOR_SEC_HDR_OFFSET_STATUS),
                        4u,
                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    p_nor_ftl->SecCntInvalid++;
    LOG_ERR(("Failed to wr hdr of sec ", (u)sec_nbr_phy, " (", (X)sec_addr, "."));
    return;
  }

  FS_NOR_PHY_WrInternal(p_nor,                                  // Wr sec.
                        p_src,
                        sec_addr + FS_DEV_NOR_SEC_HDR_LEN,
                        FS_UTIL_PWR2(p_nor_ftl->SecSizeLog2),
                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    p_nor_ftl->SecCntInvalid++;
    LOG_ERR(("Failed to wr sec ", (u)sec_nbr_phy, " (", (X)sec_addr, "."));
    return;
  }

  MEM_VAL_SET_INT32U((void *)&sec_hdr[FS_DEV_NOR_SEC_HDR_OFFSET_SEC_NBR], sec_nbr_logical);
  MEM_VAL_SET_INT32U((void *)&sec_hdr[FS_DEV_NOR_SEC_HDR_OFFSET_STATUS], FS_DEV_NOR_STATUS_SEC_VALID);

  //                                                               --------------- VALIDATE NEW PHY SEC ---------------
  FS_NOR_PHY_WrInternal(p_nor,                                  // Mark sec as 'written'.
                        &sec_hdr[0],
                        sec_addr,
                        FS_DEV_NOR_SEC_HDR_LEN,
                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    p_nor_ftl->SecCntInvalid++;
    LOG_ERR(("Failed to wr hdr of sec ", (u)sec_nbr_phy, " (", (X)sec_addr, "."));
    return;
  }

  FS_NOR_FTL_L2P_SetEntry(p_nor_ftl,                            // Update L2P tbl.
                          sec_nbr_logical,
                          sec_nbr_phy);

  p_nor_ftl->SecCntValid++;                                     // Wr sec is now valid.
}
#endif

/****************************************************************************************************//**
 *                                       FS_NOR_FTL_InvalidateSec()
 *
 * @brief    Invalidate sector.
 *
 * @param    p_nor_ftl       Pointer to a NOR FTL instance.
 *
 * @param    sec_nbr_phy     Physical sector number.
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NOR_FTL_InvalidateSec(FS_NOR_FTL *p_nor_ftl,
                                     FS_LB_NBR  sec_nbr_phy,
                                     RTOS_ERR   *p_err)
{
  FS_NOR      *p_nor;
  CPU_BOOLEAN active;
  FS_LB_NBR   blk_ix;
  CPU_BOOLEAN erased;
  CPU_INT32U  sec_addr;
  FS_LB_QTY   sec_cnt_valid;
  CPU_INT08U  sec_hdr[FS_DEV_NOR_SEC_HDR_LEN];

  p_nor = (FS_NOR *)p_nor_ftl->BlkDev.MediaPtr;
  //                                                               ---------------- MARK SEC AS INVALID ---------------
  sec_addr = FS_NOR_FTL_SecNbrPhyToAddr(p_nor_ftl, sec_nbr_phy, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  MEM_VAL_SET_INT32U((void *)&sec_hdr[FS_DEV_NOR_SEC_HDR_OFFSET_STATUS],
                     FS_DEV_NOR_STATUS_SEC_INVALID);

  FS_NOR_PHY_WrInternal(p_nor,                                  // Mark sec as 'invalid'.
                        &sec_hdr[FS_DEV_NOR_SEC_HDR_OFFSET_STATUS],
                        (sec_addr + FS_DEV_NOR_SEC_HDR_OFFSET_STATUS),
                        4u,
                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Failed to wr hdr of sec ", (u)sec_nbr_phy, " (", (X)sec_addr, "."));
    return;
  }

  //                                                               ----------------- UPDATE BLK INFO ------------------
  blk_ix = FS_NOR_FTL_SecNbrPhyToBlkIx(p_nor_ftl, sec_nbr_phy, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  FS_NOR_FTL_GetBlkInfo(p_nor_ftl,                              // Get blk valid sec cnt.
                        blk_ix,
                        &erased,
                        &sec_cnt_valid);

#if (FS_NOR_CFG_DBG_CHK_EN == DEF_ENABLED)
  if (erased == DEF_YES) {
    LOG_ERR(("L2P sec corrupted: blk ", (u)blk_ix, " erased."));
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return;
  }
  if (sec_cnt_valid == 0u) {
    LOG_ERR(("L2P sec corrupted: blk ", (u)blk_ix, " has no valid secs."));
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return;
  }
#endif

  FS_NOR_FTL_DecBlkSecCntValid(p_nor_ftl, blk_ix);              // Dec blk valid sec cnt.
  sec_cnt_valid--;

  if (sec_cnt_valid == 0u) {                                    // If no more valid secs in blk ...
    active = FS_NOR_FTL_IsAB(p_nor_ftl, blk_ix);
    if (active == DEF_NO) {                                     // ... & blk is NOT active blk  ...
      LOG_DBG(("Blk ", (u)blk_ix, " invalidated."));
      p_nor_ftl->BlkCntInvalid++;
      FS_BLK_DEV_CTR_STAT_INC(p_nor_ftl->StatInvalidBlkCtr);
#if (FS_NOR_CFG_DBG_CHK_EN == DEF_ENABLED)
      if (p_nor_ftl->BlkCntValid == 0u) {
        LOG_ERR(("Valid blk cnt == 0 upon invalidation of blk ", (u)blk_ix, "."));
        RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
        return;
      }
#endif
      p_nor_ftl->BlkCntValid--;                                 // ... erase.
    }
  }

  p_nor_ftl->SecCntValid--;
  p_nor_ftl->SecCntInvalid++;
}
#endif

/****************************************************************************************************//**
 *                                           FS_NOR_EraseBlkPrepare()
 *
 * @brief    Prepare & erase block.
 *
 * @param    p_nor_ftl   Pointer to a NOR FTL instance.
 *
 * @param    blk_ix      Block index.
 *
 * @param    p_err       Error pointer.
 *
 * @note     (1) A block move, if interrupted by power failure, will be resumed by
 *               'FS_NOR_FTL_LowMountInternal()' upon mount if the target block (i.e., the active block)
 *               was the only erased block. See also 'FS_NOR_FTL_LowMountInternal()  Note #4'.
 *
 * @note     (2) If there were NO valid sectors in the source block, it MUST have been considered
 *               invalid already.
 *
 * @note     (3) The driver is responsible for maintaining an erased block to receive data sectors
 *               during this move.  See also 'FS_NOR_FTL_FindEraseBlk()  Note #1a2'.
 *
 * @note     (4) If the erase of the former active block is interrupted, multiple blocks may be found
 *               upon mount with valid or invalid sectors AND erased sectors.  The sectors in all but
 *               one of those will be classified as invalid upon mount (see
 *               'FS_NOR_FTL_LowMountInternal()  Note #5').
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NOR_FTL_EraseBlkPrepare(FS_NOR_FTL *p_nor_ftl,
                                       FS_LB_NBR  blk_ix,
                                       RTOS_ERR   *p_err)
{
  FS_NOR      *p_nor;
  CPU_BOOLEAN active;
  CPU_INT32U  blk_addr;
  CPU_BOOLEAN erased;
  CPU_INT32U  sec_addr;
  FS_LB_QTY   sec_cnt;
  FS_LB_QTY   sec_cnt_valid;
  CPU_INT08U  sec_hdr[FS_DEV_NOR_SEC_HDR_LEN];
  FS_LB_NBR   sec_ix;
  FS_LB_NBR   sec_nbr_logical;
  FS_LB_NBR   sec_nbr_phy;
  CPU_INT32U  status;

  p_nor = (FS_NOR *)p_nor_ftl->BlkDev.MediaPtr;

  //                                                               --------------- INVALIDATE BLK INFO ----------------
  active = FS_NOR_FTL_IsAB(p_nor_ftl, blk_ix);
  if (active) {                                                 // If blk to erase if active blk ... (see Note #4)
    FS_NOR_FTL_AB_Remove(p_nor_ftl, blk_ix);                    // ... invalidate blk.
  }

  //                                                               ----------------- MOVE DATA IN BLK -----------------
  FS_NOR_FTL_GetBlkInfo(p_nor_ftl,
                        blk_ix,
                        &erased,
                        &sec_cnt_valid);

  if (sec_cnt_valid != 0u) {
    LOG_VRB(("Moving from blk ", (u)blk_ix, ", secs (", (u)sec_cnt_valid, ") prior to erase."));

    blk_addr = FS_NOR_FTL_BlkIxToAddr(p_nor_ftl, blk_ix, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    sec_cnt = p_nor_ftl->BlkSecCnts;
    sec_nbr_phy = FS_NOR_FTL_BlkIxToSecNbrPhy(p_nor_ftl, blk_ix);
    sec_addr = blk_addr + FS_DEV_NOR_BLK_HDR_LEN;

    sec_ix = 0u;
    while (sec_ix < sec_cnt) {
      FS_NOR_PHY_RdInternal(p_nor,                              // Rd blk hdr.
                            &sec_hdr[0],
                            sec_addr,
                            FS_DEV_NOR_SEC_HDR_LEN,
                            p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        LOG_ERR(("Failed to rd sec hdr ", (u)sec_nbr_phy, " (", (X)sec_addr, "."));
        return;
      }

      sec_nbr_logical = MEM_VAL_GET_INT32U((void *)&sec_hdr[FS_DEV_NOR_SEC_HDR_OFFSET_SEC_NBR]);
      status = MEM_VAL_GET_INT32U((void *)&sec_hdr[FS_DEV_NOR_SEC_HDR_OFFSET_STATUS]);

      //                                                           ----------------- MOVE DATA IN SEC -----------------
      if ((status == FS_DEV_NOR_STATUS_SEC_VALID)               // If sec valid                ...
          && (sec_nbr_logical < p_nor_ftl->DiskSizeSec)) {      // ... & logical sec nbr valid ...
                                                                // ... rd sec                  ...
        FS_NOR_PHY_RdInternal(p_nor,
                              p_nor_ftl->BufPtr,
                              (sec_addr + FS_DEV_NOR_SEC_HDR_LEN),
                              FS_UTIL_PWR2(p_nor_ftl->SecSizeLog2),
                              p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          LOG_ERR(("Failed to rd sec hdr ", (u)sec_nbr_phy, " (", (X)sec_addr, "."));
          return;
        }
        //                                                         ... & wr to free sec       ... (see Note #3)
        FS_NOR_FTL_WrSecLogicalInternal(p_nor_ftl,
                                        p_nor_ftl->BufPtr,
                                        sec_nbr_logical,
                                        p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          return;
        }

        MEM_VAL_SET_INT32U((void *)&sec_hdr[FS_DEV_NOR_SEC_HDR_OFFSET_STATUS], FS_DEV_NOR_STATUS_SEC_INVALID);

        FS_NOR_PHY_WrInternal(p_nor,                            // Mark sec as 'invalid'.
                              &sec_hdr[FS_DEV_NOR_SEC_HDR_OFFSET_STATUS],
                              sec_addr + FS_DEV_NOR_SEC_HDR_OFFSET_STATUS,
                              4u,
                              p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          LOG_ERR(("Failed to rd sec hdr ", (u)sec_nbr_phy, " (", (X)sec_addr, "."));
          return;
        }

#if (FS_NOR_CFG_DBG_CHK_EN == DEF_ENABLED)
        if (sec_cnt_valid == 0u) {
          FS_NOR_FTL_IncBlkSecCntValid(p_nor_ftl, blk_ix);
          LOG_ERR(("Too many valid secs found in blk ix ", (u)blk_ix, "."));
          RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
          return;
        }
#endif
        FS_NOR_FTL_DecBlkSecCntValid(p_nor_ftl, blk_ix);
        sec_cnt_valid--;
#if (FS_NOR_CFG_DBG_CHK_EN == DEF_ENABLED)
        if (p_nor_ftl->SecCntValid < 1u) {
          LOG_ERR(("Unexpectedly few valid secs ", (u)p_nor_ftl->SecCntValid, " exist in NOR."));
          RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
          return;
        }
#endif
        p_nor_ftl->SecCntValid--;
        p_nor_ftl->SecCntInvalid++;
        FS_BLK_DEV_CTR_STAT_INC(p_nor_ftl->StatCopyCtr);
      }

      sec_addr += FS_UTIL_PWR2(p_nor_ftl->SecSizeLog2) + FS_DEV_NOR_SEC_HDR_LEN;
      sec_ix++;
      sec_nbr_phy++;
    }

#if (FS_NOR_CFG_DBG_CHK_EN == DEF_ENABLED)
    if (sec_cnt_valid != 0u) {
      LOG_ERR(("Too few valid secs (", (u)sec_cnt_valid, " more expected) found in blk ix ", (u)blk_ix));
      if (p_nor_ftl->SecCntValid < sec_cnt_valid) {
        LOG_ERR(("Unexpectedly few valid secs (", (u)p_nor_ftl->SecCntValid, ") exist in NOR."));
      }
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
      return;
    }
#endif

#if (FS_NOR_CFG_DBG_CHK_EN == DEF_ENABLED)
    if (p_nor_ftl->BlkCntValid < 1u) {
      LOG_ERR(("Unexpectedly few valid blks (", (u)p_nor_ftl->BlkCntValid, ") exist in NOR."));
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
      return;
    }
#endif
    p_nor_ftl->BlkCntValid--;                                   // One fewer valid blks (see Note #2).
    p_nor_ftl->BlkCntInvalid++;
  }

  //                                                               -------------------- ERASE BLK ---------------------
  FS_NOR_FTL_EraseBlkEmptyFmt(p_nor_ftl, blk_ix, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
}
#endif

/****************************************************************************************************//**
 *                                       FS_NOR_FTL_EraseBlkEmptyFmt()
 *
 * @brief    Erase empty block.
 *
 * @param    p_nor_ftl   Pointer to a NOR FTL instance.
 *
 * @param    blk_ix      Block index.
 *
 * @param    p_err       Error pointer.
 *
 * @note     (1) To erase a block :
 *               - (a) Find/calculation location of block.
 *               - (b) Erase block.
 *               - (c) Write block header.
 *
 * @note     (2) A block may lose its erase count if the block erase is interrupted, or if the block
 *               header is not completely written.  The block is assumed to have the maximum erase
 *               count upon restart.  Wear-leveling erased block approximately uniformly, so this
 *               should be close to true block count.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NOR_FTL_EraseBlkEmptyFmt(FS_NOR_FTL *p_nor_ftl,
                                        FS_LB_NBR  blk_ix,
                                        RTOS_ERR   *p_err)
{
  FS_NOR     *p_nor;
  CPU_INT32U blk_addr;
  CPU_INT32U erase_cnt;
  CPU_INT08U blk_hdr[FS_DEV_NOR_BLK_HDR_LEN];
  CPU_INT08U erase_cnt_octets[4];

  p_nor = (FS_NOR *)p_nor_ftl->BlkDev.MediaPtr;

  //                                                               -------------------- ERASE BLK ---------------------
  blk_addr = FS_NOR_FTL_BlkIxToAddr(p_nor_ftl, blk_ix, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  FS_NOR_PHY_RdInternal(p_nor,                                  // Rd erase cnt.
                        &erase_cnt_octets[0],
                        (blk_addr + FS_DEV_NOR_BLK_HDR_OFFSET_ERASE_CNT),
                        4u,
                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Failed to rd erase cnts of blk ", (u)blk_ix, " (", (X)blk_addr, ")."));
    return;
  }

  erase_cnt = MEM_VAL_GET_INT32U((void *)&erase_cnt_octets[0]);
  if (erase_cnt == FS_DEV_NOR_BLK_HDR_ERASE_CNT_INVALID) {
    erase_cnt = p_nor_ftl->EraseCntMax;                         // If erase cnt invalid, set to max cnt (see Note #2).
  } else {
    erase_cnt++;
    if (p_nor_ftl->EraseCntMax < erase_cnt) {
      LOG_DBG(("Max erase cnt now ", (u)erase_cnt, " (prev was ", (u)p_nor_ftl->EraseCntMax, ")."));
      p_nor_ftl->EraseCntMax = erase_cnt;
    }
  }

  FS_NOR_PHY_BlkEraseInternal(p_nor,                            // Erase blk.
                              blk_ix,
                              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Failed to erase blk ix ", (u)blk_ix, " (", (X)blk_addr, ")."));
    return;
  }

  LOG_VRB(("Erased blk ", (u)blk_ix, " (", (X)blk_addr, ") w/erase cnt ", (u)erase_cnt, "."));

  //                                                               -------------------- WR BLK HDR --------------------
  MEM_VAL_SET_INT32U((void *)&blk_hdr[FS_DEV_NOR_BLK_HDR_OFFSET_MARK1], FS_DEV_NOR_BLK_HDR_MARK_WORD_1);
  MEM_VAL_SET_INT32U((void *)&blk_hdr[FS_DEV_NOR_BLK_HDR_OFFSET_MARK2], FS_DEV_NOR_BLK_HDR_MARK_WORD_2);
  MEM_VAL_SET_INT32U((void *)&blk_hdr[FS_DEV_NOR_BLK_HDR_OFFSET_ERASE_CNT], erase_cnt);
  MEM_VAL_SET_INT16U((void *)&blk_hdr[FS_DEV_NOR_BLK_HDR_OFFSET_VER], FS_DEV_NOR_BLK_HDR_VER);
  MEM_VAL_SET_INT16U((void *)&blk_hdr[FS_DEV_NOR_BLK_HDR_OFFSET_SEC_SIZE], FS_UTIL_PWR2(p_nor_ftl->SecSizeLog2));
  MEM_VAL_SET_INT16U((void *)&blk_hdr[FS_DEV_NOR_BLK_HDR_OFFSET_BLK_CNT], p_nor_ftl->BlkCntUsed);

  FS_NOR_PHY_WrInternal(p_nor,                                  // Wr hdr.
                        &blk_hdr[0],
                        blk_addr,
                        FS_DEV_NOR_BLK_HDR_LEN,
                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Failed to wr blk hdr ix ", (u)blk_ix, " (", (X)blk_addr, ")."));
    return;
  }

  //                                                               ------------------- UPDATE INFO --------------------
  p_nor_ftl->BlkCntErased++;                                    // Another erased blk.
#if (FS_NOR_CFG_DBG_CHK_EN == DEF_ENABLED)
  if (p_nor_ftl->BlkCntInvalid == 0u) {
    LOG_ERR(("Invalid blk cnt == 0 upon erase of blk ", (u)blk_ix, "."));
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return;
  }
#endif
  p_nor_ftl->BlkCntInvalid--;                                   // One fewer invalid blks.
  p_nor_ftl->SecCntErased += p_nor_ftl->BlkSecCnts;             // More erased secs.
  p_nor_ftl->SecCntInvalid -= p_nor_ftl->BlkSecCnts;            // Fewer invalid secs.

  FS_NOR_FTL_SetBlkErased(p_nor_ftl,                            // Blk is erased.
                          blk_ix,
                          DEF_YES);
}
#endif

/****************************************************************************************************//**
 *                                           FS_NOR_FTL_FindEraseBlk()
 *
 * @brief    Find block that needs to be erased.
 *
 * @param    p_nor_ftl   Pointer to a NOR FTL instance.
 *
 * @return   Block index,                if block found that needs to be erased.
 *           FS_DEV_NOR_SEC_NBR_INVALID, if NO block needs to be erased.
 *
 * @note     (1) Block erasure handling :
 *               - (a) Blocks may be erased under one of two conditions :
 *                   - (1) The erase count difference threshold is exceeded, i.e., a block exists that
 *                         has been erased 'EraseCntDiffTh' fewer times than the most often
 *                         erased block.
 *                   - (2) Fewer block exist than ...
 *                       - (A) ... 1, if there is an active block assigned;
 *                       - (B) ... 2, if there is NO active block assigned.
 *                             This maintains the minimum number erase blocks required by the driver (see also
 *                             'FS_NOR_FTL_EraseBlkPrepare()  Note #3').
 *               - (b) The block chosen for erasure is either :
 *                   - (1) The block with the lowest erase count.
 *                   - (2) The block with the fewest valid sectors.
 *                         The former is chosen if the difference between the block's erase count & the
 *                         maximum is greater than some maximum value; the default maximum is 100 erasures.
 *               - (c) Blocks are erased until no more block such as #1a1 exist, & until enough
 *                     sectors (#1a2) are unoccupied.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static FS_LB_NBR FS_NOR_FTL_FindEraseBlk(FS_NOR_FTL *p_nor_ftl)
{
  CPU_BOOLEAN active;
  FS_LB_QTY   active_blk_cnt;
  FS_LB_NBR   blk_ix;
  FS_LB_NBR   blk_ix_min;
  CPU_BOOLEAN empty;
  CPU_BOOLEAN erased;
  FS_LB_QTY   sec_cnt_valid_min;
  FS_LB_QTY   sec_cnt_valid;

  //                                                               ----------------- CHK FOR ERASE TH -----------------
  //                                                               If enough blks erased                 ...
  active_blk_cnt = FS_NOR_FTL_GetAB_Cnt(p_nor_ftl);
  if (p_nor_ftl->BlkCntErased >= 1u) {
    //                                                             ... & erase cnt diff th satisfied  OR ...
    if ((p_nor_ftl->EraseCntMax < p_nor_ftl->EraseCntMin + p_nor_ftl->EraseCntDiffTh)
        ||                                                      // ...   erased wear-level blk exists OR ...
        (p_nor_ftl->BlkWearLevelAvail == DEF_YES)
        ||                                                      // ...   sufficient blks erased       OR ...
        (p_nor_ftl->BlkCntErased >= 2u)
        ||                                                      // ...   at least one blk active         ...
        (active_blk_cnt > 0u)) {
      return (FS_DEV_NOR_SEC_NBR_INVALID);                      // ... do not erase a blk yet.
    }
  }

  //                                                               ----------------- ACTIVE WEAR-LEVEL ----------------
  blk_ix_min = FS_NOR_FTL_FindEraseBlkWear(p_nor_ftl);          // Find a possible blk with lowest erase cnt.
  if (blk_ix_min == FS_DEV_NOR_SEC_NBR_INVALID) {               // If NO minimally-erased blk    ...
                                                                // ... but if enough blks erased ...
    if ( (p_nor_ftl->BlkCntErased >= 2u)
         || ((p_nor_ftl->BlkCntErased >= 1u) && (active_blk_cnt > 0u))) {
      return (FS_DEV_NOR_SEC_NBR_INVALID);                      // ... do not erase a blk yet.
    }
  } else {                                                      // If active wear level req'd ...
    LOG_VRB(("Wear level performed: Blk ", (u)blk_ix_min, " erased."));
    p_nor_ftl->BlkWearLevelAvail = DEF_YES;
    return (blk_ix_min);                                        // ... rtn blk.
  }

  //                                                               ---------------- CHK FOR PARTIAL BLK ---------------
  //                                                               Find blk with the fewest valid sectors.
  blk_ix = 0u;
  sec_cnt_valid_min = p_nor_ftl->BlkSecCnts;
  blk_ix_min = FS_DEV_NOR_SEC_NBR_INVALID;
  empty = DEF_NO;
  while (!empty && (blk_ix < p_nor_ftl->BlkCntUsed)) {          // Probe each block ...
                                                                // while NO empty blk is found.
    FS_NOR_FTL_GetBlkInfo(p_nor_ftl,                            // Get blk info from internal FTL tables.
                          blk_ix,
                          &erased,
                          &sec_cnt_valid);

    if (erased == DEF_NO) {                                     // Consider only blk not erased.
      active = FS_NOR_FTL_IsAB(p_nor_ftl, blk_ix);
      if (active == DEF_YES) {                                  // If blk is active blk ...
                                                                // ... add erased secs to nbr of valid secs.
        sec_cnt_valid += FS_NOR_FTL_AB_SecCntErased(p_nor_ftl, blk_ix);
      }

      if (sec_cnt_valid_min > sec_cnt_valid) {
        sec_cnt_valid_min = sec_cnt_valid;                      // Update min sec cnt.
        blk_ix_min = blk_ix;                                    // Select this blk.

        if (sec_cnt_valid == 0u) {                              // If blk is empty ...
          empty = DEF_YES;                                      // ... brk from loop.
        }
      }
    }

    blk_ix++;
  }

  return (blk_ix_min);
}
#endif

/****************************************************************************************************//**
 *                                       FS_NOR_FTL_FindEraseBlkWear()
 *
 * @brief    Find block that needs to be erased (for active wear leveling).
 *
 * @param    p_nor_ftl   Pointer to a NOR FTL instance.
 *
 * @return   Block index,                if block found that needs to be erased.
 *           FS_DEV_NOR_SEC_NBR_INVALID, if NO block needs to be erased.
 *
 * @note     (1) Blocks with seldom-changed data will NOT be erased as often as other blocks with pure
 *               passive wear leveling.  Active wear-leveling purposefully copies data from such blocks
 *               to new (& more often erased) blocks. The burden of erasures will subsequently fall
 *               more heavily upon these old blocks, smoothing the erase counts.
 *               - (a) The minimum erase count MAY not be accurate, since the minimally-erased block(s)
 *                     may have been erased; since more than one minimally-erased block may exist, the
 *                     minimum erase count is NEVER updated upon erase.
 *
 * @note     (2) If all minimally-erased blocks are erased, then no block will be found.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static FS_LB_NBR FS_NOR_FTL_FindEraseBlkWear(FS_NOR_FTL *p_nor_ftl)
{
  CPU_BOOLEAN active;
  CPU_INT08U  blk_hdr[FS_DEV_NOR_BLK_HDR_LEN];
  FS_LB_NBR   blk_ix;
  FS_LB_NBR   blk_ix_min;
  CPU_BOOLEAN erased;
  CPU_INT32U  erase_cnt;
  CPU_INT32U  erase_cnt_min;
  FS_LB_QTY   sec_cnt_valid_min;
  FS_LB_QTY   sec_cnt_valid;
  RTOS_ERR    err;

  RTOS_ERR_SET(err, RTOS_ERR_NONE);
  //                                                               ACTIVE WEAR-LEVEL (see Note #1)
  //                                                               If erase cnt th exceeded ...
  if (p_nor_ftl->EraseCntMax >= p_nor_ftl->EraseCntMin + p_nor_ftl->EraseCntDiffTh) {
    //                                                             ... find blk with min erase cnt & valid secs.
    erase_cnt_min = p_nor_ftl->EraseCntMax + 1u;
    sec_cnt_valid_min = p_nor_ftl->BlkSecCnts;
    blk_ix_min = FS_DEV_NOR_SEC_NBR_INVALID;
    //                                                             Scan all blk headers.
    for (blk_ix = 0u; blk_ix < p_nor_ftl->BlkCntUsed; blk_ix++) {
      FS_NOR_FTL_GetBlkInfo(p_nor_ftl,                          // Get blk info from internal FTL tables.
                            blk_ix,
                            &erased,
                            &sec_cnt_valid);

      FS_NOR_FTL_RdBlkHdr(p_nor_ftl,                            // Get blk header (read access to physical blk).
                          &blk_hdr[0],
                          blk_ix,
                          &err);
      if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
        LOG_ERR(("Reading header for blk ", (u)blk_ix, " failed with error ", (u)RTOS_ERR_CODE_GET(err), "."));
        return (FS_DEV_NOR_SEC_NBR_INVALID);                    // #### Add rtn err val.
      }
      //                                                           Get cur blk erase cnt.
      erase_cnt = MEM_VAL_GET_INT32U((void *)&blk_hdr[FS_DEV_NOR_BLK_HDR_OFFSET_ERASE_CNT]);
      if (erase_cnt_min > erase_cnt) {
        erase_cnt_min = erase_cnt;                              // Update cur min erase cnt if needed.
      }

      if (erased == DEF_NO) {                                   // If blk is NOT erased        ...
        active = FS_NOR_FTL_IsAB(p_nor_ftl, blk_ix);
        if (active == DEF_NO) {                                 // ... & blk is NOT active blk ...
                                                                // ... & erase cnt below th    ...
          if (p_nor_ftl->EraseCntMax >= erase_cnt + p_nor_ftl->EraseCntDiffTh) {
            if (sec_cnt_valid_min > sec_cnt_valid) {            // ... & fewer valid secs      ...
              sec_cnt_valid_min = sec_cnt_valid;
              blk_ix_min = blk_ix;                              // ... sel blk.
            }
          }
        }
      }
    }

    p_nor_ftl->EraseCntMin = erase_cnt_min;                     // Update cnt min w/actual erase cnt min.

    if (blk_ix_min == FS_DEV_NOR_SEC_NBR_INVALID) {             // Verify blk ix min (see Note #2).
      return (FS_DEV_NOR_SEC_NBR_INVALID);
    }

#if LOG_VRB_IS_EN()
    if (p_nor_ftl->EraseCntMin < erase_cnt_min) {
      LOG_VRB(("Min erase cnt now ", (u)erase_cnt_min, " (prev was ", (u)p_nor_ftl->EraseCntMin, ")."));
    }
#endif

    //                                                             If erase cnt th still exceeded (see Note #1a) ...
    if (p_nor_ftl->EraseCntMax >= p_nor_ftl->EraseCntMin + p_nor_ftl->EraseCntDiffTh) {
      return (blk_ix_min);                                      // ... erase blk.
    }
  }

  return (FS_DEV_NOR_SEC_NBR_INVALID);
}
#endif

/****************************************************************************************************//**
 *                                       FS_NOR_FTL_FindErasedBlk()
 *
 * @brief    Find block that is erased.
 *
 * @param    p_nor_ftl   Pointer to a NOR FTL instance.
 *
 * @return   Block index,                if erased block found.
 *           FS_DEV_NOR_SEC_NBR_INVALID, if NO erased block found.
 *
 * @note     (1) The erased block with the lowest erase count is selected.
 *               - (a) #### Optimize for cases with one erased blk OR no large erase count differences.
 * @note     (2) There may be no block with erase count as low as 'EraseCntMin' (see
 *               'FS_NOR_FindEraseBlkWear()  Note #1a').
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static FS_LB_NBR FS_NOR_FTL_FindErasedBlk(FS_NOR_FTL *p_nor_ftl)
{
  CPU_INT08U  blk_hdr[FS_DEV_NOR_BLK_HDR_LEN];
  FS_LB_QTY   blk_cnt_erased;
  FS_LB_NBR   blk_ix;
  FS_LB_NBR   blk_ix_erased;
  CPU_BOOLEAN erased;
  CPU_INT32U  erase_cnt;
  CPU_INT32U  erase_cnt_min;
  CPU_BOOLEAN found;
  FS_LB_QTY   sec_cnt_valid;
  RTOS_ERR    err;

  RTOS_ERR_SET(err, RTOS_ERR_NONE);

  blk_cnt_erased = 0u;
  blk_ix = 0u;
  blk_ix_erased = FS_DEV_NOR_SEC_NBR_INVALID;
  erase_cnt_min = p_nor_ftl->EraseCntMax + 1u;
  found = DEF_NO;
  while ((found == DEF_NO)
         && (blk_ix < p_nor_ftl->BlkCntUsed)) {
    FS_NOR_FTL_GetBlkInfo(p_nor_ftl,
                          blk_ix,
                          &erased,
                          &sec_cnt_valid);
    if (erased == DEF_YES) {
      blk_cnt_erased++;

      FS_NOR_FTL_RdBlkHdr(p_nor_ftl,
                          &blk_hdr[0],
                          blk_ix,
                          &err);
      if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {            // If hdr could NOT be rd ...
        return (FS_DEV_NOR_SEC_NBR_INVALID);                    // ... rtn invalid blk ix.
      }

      erase_cnt = MEM_VAL_GET_INT32U((void *)&blk_hdr[FS_DEV_NOR_BLK_HDR_OFFSET_ERASE_CNT]);
      if (erase_cnt_min > erase_cnt) {                          // If lower erase cnt (see Note #1) ...
        erase_cnt_min = erase_cnt;
        blk_ix_erased = blk_ix;                                 // ... sel this blk.

        if (erase_cnt_min == p_nor_ftl->EraseCntMin) {          // If least erased blk (see Note #2) ...
          found = DEF_YES;                                      // ... stop srch.
        }
        if (blk_cnt_erased >= p_nor_ftl->BlkCntErased) {        // If no more erased blks exist ...
          found = DEF_YES;                                      // ... stop srch.
        }
      }
    }

    blk_ix++;
  }

  p_nor_ftl->BlkWearLevelAvail = DEF_NO;

  return (blk_ix_erased);
}
#endif

/****************************************************************************************************//**
 *                                           FS_NOR_FTL_AB_ClrAll()
 *
 * @brief    Clear all active block information.
 *
 * @param    p_nor_ftl   Pointer to a NOR FTL instance.
 *******************************************************************************************************/
static void FS_NOR_FTL_AB_ClrAll(FS_NOR_FTL *p_nor_ftl)
{
  FS_LB_QTY ix;

  for (ix = 0u; ix < p_nor_ftl->AB_Cnt; ix++) {
    p_nor_ftl->AB_IxTbl[ix] = FS_DEV_NOR_SEC_NBR_INVALID;
    p_nor_ftl->AB_SecNextTbl[ix] = 0u;
  }
}

/****************************************************************************************************//**
 *                                           FS_NOR_GetAB_Cnt()
 *
 * @brief    Determine number of active blocks.
 *
 * @param    p_nor_ftl   Pointer to a NOR FTL instance.
 *
 * @return   Number of active blocks.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static FS_LB_QTY FS_NOR_FTL_GetAB_Cnt(FS_NOR_FTL *p_nor_ftl)
{
  FS_LB_QTY cnt;
  FS_LB_QTY ix;

  cnt = 0u;
  for (ix = 0u; ix < p_nor_ftl->AB_Cnt; ix++) {
    if (p_nor_ftl->AB_IxTbl[ix] != FS_DEV_NOR_SEC_NBR_INVALID) {
      cnt++;
    }
  }

  return (cnt);
}
#endif

/****************************************************************************************************//**
 *                                               FS_NOR_FTL_IsAB()
 *
 * @brief    Determine whether block is an active block.
 *
 * @param    p_nor_ftl   Pointer to a NOR FTL instance.
 *
 * @param    blk_ix      Block index.
 *
 * @return   DEF_YES, if block is an active block.
 *           DEF_NO,  otherwise.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static CPU_BOOLEAN FS_NOR_FTL_IsAB(FS_NOR_FTL *p_nor_ftl,
                                   FS_LB_NBR  blk_ix)
{
  FS_LB_NBR   ix;
  CPU_BOOLEAN found;

  found = DEF_NO;
  ix = 0u;
  while (!found && (ix < p_nor_ftl->AB_Cnt)) {
    if (p_nor_ftl->AB_IxTbl[ix] == blk_ix) {
      found = DEF_YES;
    }
    ix++;
  }
  return (found);
}
#endif

/****************************************************************************************************//**
 *                                           FS_NOR_FTL_AB_Add()
 *
 * @brief    Add new active block.
 *
 * @param    p_nor_ftl       Pointer to a NOR FTL instance.
 *
 * @param    blk_ix          Block index.
 *
 * @param    sec_ix_next     Next sector in block to use.
 *
 * @return   DEF_OK,   if block added.
 *           DEF_FAIL, otherwise (see Note #1).
 *
 * @note     (1) If all active block slots are used, another active block CANNOT be added.
 *******************************************************************************************************/
static CPU_BOOLEAN FS_NOR_FTL_AB_Add(FS_NOR_FTL *p_nor_ftl,
                                     FS_LB_NBR  blk_ix,
                                     FS_LB_NBR  sec_ix_next)
{
  FS_LB_NBR   ix;
  CPU_BOOLEAN added;

  added = DEF_FAIL;
  ix = 0u;
  while (!added && (ix < p_nor_ftl->AB_Cnt)) {
    if (p_nor_ftl->AB_IxTbl[ix] == FS_DEV_NOR_SEC_NBR_INVALID) {
      p_nor_ftl->AB_IxTbl[ix] = blk_ix;
      p_nor_ftl->AB_SecNextTbl[ix] = sec_ix_next;
      added = DEF_OK;
    }
    ix++;
  }

  return (added);
}

/****************************************************************************************************//**
 *                                           FS_NOR_FTL_AB_RemoveAll()
 *
 * @brief    Remove all active blocks.
 *
 * @param    p_nor_ftl   Pointer to a NOR FTL instance.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NOR_FTL_AB_RemoveAll(FS_NOR_FTL *p_nor_ftl)
{
  CPU_BOOLEAN erased;
  FS_LB_QTY   sec_cnt_valid;
  FS_LB_QTY   ix;

  ix = 0u;
  while (ix < p_nor_ftl->AB_Cnt) {
    if (p_nor_ftl->AB_IxTbl[ix] != FS_DEV_NOR_SEC_NBR_INVALID) {
      FS_NOR_FTL_GetBlkInfo(p_nor_ftl,
                            p_nor_ftl->AB_IxTbl[ix],
                            &erased,
                            &sec_cnt_valid);
      if (sec_cnt_valid == 0u) {                                // If no valid secs in blk ...
        p_nor_ftl->BlkCntInvalid++;                             // ... blk is now invalid.
        p_nor_ftl->BlkCntValid--;
      }

      p_nor_ftl->SecCntInvalid += p_nor_ftl->BlkSecCnts - p_nor_ftl->AB_SecNextTbl[ix];
      p_nor_ftl->SecCntErased -= p_nor_ftl->BlkSecCnts - p_nor_ftl->AB_SecNextTbl[ix];
      p_nor_ftl->AB_IxTbl[ix] = FS_DEV_NOR_SEC_NBR_INVALID;
      p_nor_ftl->AB_SecNextTbl[ix] = 0u;
    }
    ix++;
  }
}
#endif

/****************************************************************************************************//**
 *                                           FS_NOR_FTL_AB_Remove()
 *
 * @brief    Remove active block.
 *
 * @param    p_nor_ftl   Pointer to a NOR FTL instance.
 *
 * @param    blk_ix      Block index.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NOR_FTL_AB_Remove(FS_NOR_FTL *p_nor_ftl,
                                 FS_LB_NBR  blk_ix)
{
  CPU_BOOLEAN erased;
  CPU_BOOLEAN found;
  FS_LB_QTY   sec_cnt_valid;
  FS_LB_NBR   ix;

  found = DEF_NO;
  ix = 0u;
  while ((ix < p_nor_ftl->AB_Cnt)
         && (found == DEF_NO)) {
    if (p_nor_ftl->AB_IxTbl[ix] == blk_ix) {
      FS_NOR_FTL_GetBlkInfo(p_nor_ftl,
                            p_nor_ftl->AB_IxTbl[ix],
                            &erased,
                            &sec_cnt_valid);
      if (sec_cnt_valid == 0u) {                                // If no valid secs in blk ...
        p_nor_ftl->BlkCntInvalid++;                             // ... blk is now invalid.
        p_nor_ftl->BlkCntValid--;
      }

      p_nor_ftl->SecCntInvalid += p_nor_ftl->BlkSecCnts - p_nor_ftl->AB_SecNextTbl[ix];
      p_nor_ftl->SecCntErased -= p_nor_ftl->BlkSecCnts - p_nor_ftl->AB_SecNextTbl[ix];
      p_nor_ftl->AB_IxTbl[ix] = FS_DEV_NOR_SEC_NBR_INVALID;
      p_nor_ftl->AB_SecNextTbl[ix] = 0u;
      found = DEF_YES;;
    }
    ix++;
  }
}
#endif

/****************************************************************************************************//**
 *                                       FS_NOR_FTL_AB_SecCntTotErased()
 *
 * @brief    Determine total number of erase sectors in active blocks.
 *
 * @param    p_nor_ftl   Pointer to a NOR FTL instance.
 *
 * @return   Erased sector count.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static FS_LB_QTY FS_NOR_FTL_AB_SecCntTotErased(FS_NOR_FTL *p_nor_ftl)
{
  FS_LB_QTY erased_sec_cnt;
  FS_LB_QTY ix;

  erased_sec_cnt = 0u;
  ix = 0u;
  while (ix < p_nor_ftl->AB_Cnt) {
    if (p_nor_ftl->AB_IxTbl[ix] != FS_DEV_NOR_SEC_NBR_INVALID) {
      erased_sec_cnt += p_nor_ftl->BlkSecCnts - p_nor_ftl->AB_SecNextTbl[ix];
    }
    ix++;
  }
  return (erased_sec_cnt);
}
#endif

/****************************************************************************************************//**
 *                                       FS_NOR_FTL_AB_SecCntErased()
 *
 * @brief    Determine number of erase sectors in active block.
 *
 * @param    p_nor_ftl   Pointer to a NOR FTL instance.
 *
 * @param    blk_ix      Block index.
 *
 * @return   Erased sector count.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static FS_LB_QTY FS_NOR_FTL_AB_SecCntErased(FS_NOR_FTL *p_nor_ftl,
                                            FS_LB_NBR  blk_ix)
{
  FS_LB_QTY   erased_sec_cnt;
  CPU_BOOLEAN found;
  FS_LB_NBR   ix;

  erased_sec_cnt = 0u;
  found = DEF_NO;
  ix = 0u;
  while ((ix < p_nor_ftl->AB_Cnt) && !found) {
    if (p_nor_ftl->AB_IxTbl[ix] == blk_ix) {
      erased_sec_cnt = p_nor_ftl->BlkSecCnts - p_nor_ftl->AB_SecNextTbl[ix];
      found = DEF_YES;
    }
    ix++;
  }
  return (erased_sec_cnt);
}
#endif

/****************************************************************************************************//**
 *                                           FS_NOR_FTL_AB_SecFind()
 *
 * @brief    Find active block sector.
 *
 * @param    p_nor_ftl           Pointer to a NOR FTL instance.
 *
 * @param    sec_nbr_logical     Logical sector to be stored in active block.
 *
 * @return   Physical sector index.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static FS_LB_NBR FS_NOR_FTL_AB_SecFind(FS_NOR_FTL *p_nor_ftl,
                                       FS_LB_NBR  sec_nbr_logical)
{
  FS_LB_NBR blk_ix_ab;
  FS_LB_NBR sec_nbr_phy;
  FS_LB_NBR ix;
  FS_LB_NBR ix_sel;

  ix_sel = (sec_nbr_logical * p_nor_ftl->AB_Cnt * 2u) / p_nor_ftl->DiskSizeSec;
  if (ix_sel >= p_nor_ftl->AB_Cnt) {
    ix_sel -= p_nor_ftl->AB_Cnt;
  }
  blk_ix_ab = p_nor_ftl->AB_IxTbl[ix_sel];
  if (blk_ix_ab == FS_DEV_NOR_SEC_NBR_INVALID) {                // If desired blk ix invalid ...
    blk_ix_ab = FS_NOR_FTL_FindErasedBlk(p_nor_ftl);            // ... find erased blk.

    if (blk_ix_ab != FS_DEV_NOR_SEC_NBR_INVALID) {
      p_nor_ftl->AB_IxTbl[ix_sel] = blk_ix_ab;
      p_nor_ftl->AB_SecNextTbl[ix_sel] = 0u;

      FS_NOR_FTL_SetBlkErased(p_nor_ftl,                        // Set blk erased status.
                              blk_ix_ab,
                              DEF_NO);

      FS_NOR_FTL_SetBlkSecCntValid(p_nor_ftl,                   // Set sec cnt valid.
                                   blk_ix_ab,
                                   0u);

#if (FS_NOR_CFG_DBG_CHK_EN == DEF_ENABLED)
      if (p_nor_ftl->BlkCntErased == 0u) {
        LOG_ERR(("Erased blk cnt == 0 upon alloc of blk ", (u)blk_ix_ab, "."));
        return (FS_DEV_NOR_SEC_NBR_INVALID);
      }
#endif

      p_nor_ftl->BlkCntErased--;                                // Erased blk now contains valid data.
      p_nor_ftl->BlkCntValid++;
    }
  }

  if (blk_ix_ab == FS_DEV_NOR_SEC_NBR_INVALID) {                // If still no active blk ...
    ix = 0u;
    while (ix < p_nor_ftl->AB_Cnt) {                            // ... srch through all blks.
      blk_ix_ab = p_nor_ftl->AB_IxTbl[ix];
      if (blk_ix_ab != FS_DEV_NOR_SEC_NBR_INVALID) {
        ix_sel = ix;
        break;
      }
      ix++;
    }
  }

  if (blk_ix_ab == FS_DEV_NOR_SEC_NBR_INVALID) {
    return (FS_DEV_NOR_SEC_NBR_INVALID);
  }

  //                                                               Get start phy sec nbr of blk.
  sec_nbr_phy = FS_NOR_FTL_BlkIxToSecNbrPhy(p_nor_ftl, blk_ix_ab);
#if (FS_NOR_CFG_DBG_CHK_EN == DEF_ENABLED)
  if (sec_nbr_phy == FS_DEV_NOR_SEC_NBR_INVALID) {
    LOG_ERR(("Failed to get addr of blk ", (u)blk_ix_ab, "."));
    return (FS_DEV_NOR_SEC_NBR_INVALID);
  }
#endif
  //                                                               Get phy sec nbr of next free sec in blk.
  sec_nbr_phy += p_nor_ftl->AB_SecNextTbl[ix_sel];

  p_nor_ftl->AB_SecNextTbl[ix_sel]++;
  p_nor_ftl->SecCntErased--;
  FS_NOR_FTL_IncBlkSecCntValid(p_nor_ftl, p_nor_ftl->AB_IxTbl[ix_sel]);

  //                                                               If all secs in active blk used ...
  if (p_nor_ftl->AB_SecNextTbl[ix_sel] == p_nor_ftl->BlkSecCnts) {
    p_nor_ftl->AB_IxTbl[ix_sel] = FS_DEV_NOR_SEC_NBR_INVALID;
    p_nor_ftl->AB_SecNextTbl[ix_sel] = 0u;                      // ... invalidate active blk ix.
  }

  return (sec_nbr_phy);
}
#endif

/****************************************************************************************************//**
 *                                           FS_NOR_FTL_GetBlkInfo()
 *
 * @brief    Get block information.
 *
 * @param    p_nor_ftl           Pointer to a NOR FTL instance.
 *
 * @param    blk_ix              Block index.
 *
 * @param    p_erased            Pointer to variable that will receive the erase status.
 *
 * @param    p_sec_cnt_valid     Pointer to variable that will receive the count of valid sectors.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NOR_FTL_GetBlkInfo(FS_NOR_FTL  *p_nor_ftl,
                                  FS_LB_NBR   blk_ix,
                                  CPU_BOOLEAN *p_erased,
                                  FS_LB_QTY   *p_sec_cnt_valid)
{
  CPU_INT08U  *p_erase_map;
  CPU_INT08U  *p_sec_cnt_tbl_08;
  CPU_INT16U  *p_sec_cnt_tbl_16;
  FS_LB_NBR   blk_ix_bit;
  FS_LB_NBR   blk_ix_byte;
  CPU_BOOLEAN erased;
  FS_LB_QTY   sec_cnt_valid;

  blk_ix_bit = blk_ix % DEF_INT_08_NBR_BITS;
  blk_ix_byte = blk_ix / DEF_INT_08_NBR_BITS;

  p_erase_map = (CPU_INT08U *)p_nor_ftl->BlkEraseMap;
  erased = DEF_BIT_IS_SET(p_erase_map[blk_ix_byte], DEF_BIT(blk_ix_bit));
  *p_erased = erased;

  if (erased == DEF_NO) {
    if (p_nor_ftl->BlkSecCnts < DEF_INT_08U_MAX_VAL) {
      p_sec_cnt_tbl_08 = (CPU_INT08U *)p_nor_ftl->BlkSecCntValidTbl;
      sec_cnt_valid = (FS_LB_QTY)(p_sec_cnt_tbl_08[blk_ix]);
    } else {
      p_sec_cnt_tbl_16 = (CPU_INT16U *)p_nor_ftl->BlkSecCntValidTbl;
      sec_cnt_valid = (FS_LB_QTY)(p_sec_cnt_tbl_16[blk_ix]);
    }
  } else {
    sec_cnt_valid = 0u;
  }

  *p_sec_cnt_valid = sec_cnt_valid;
}
#endif

/****************************************************************************************************//**
 *                                           FS_NOR_FTL_SetBlkErased()
 *
 * @brief    Set block erase status.
 *
 * @param    p_nor_ftl   Pointer to a NOR FTL instance.
 *
 * @param    blk_ix      Block index.
 *
 * @param    erased      Erased status.
 *******************************************************************************************************/
static void FS_NOR_FTL_SetBlkErased(FS_NOR_FTL  *p_nor_ftl,
                                    FS_LB_NBR   blk_ix,
                                    CPU_BOOLEAN erased)
{
  CPU_INT08U *p_erase_map;
  FS_LB_NBR  blk_ix_bit;
  FS_LB_NBR  blk_ix_byte;

  blk_ix_bit = blk_ix % DEF_INT_08_NBR_BITS;
  blk_ix_byte = blk_ix / DEF_INT_08_NBR_BITS;

  p_erase_map = (CPU_INT08U *)p_nor_ftl->BlkEraseMap;
  if (erased == DEF_YES) {
    DEF_BIT_SET(p_erase_map[blk_ix_byte], DEF_BIT(blk_ix_bit));
  } else {
    DEF_BIT_CLR(p_erase_map[blk_ix_byte], DEF_BIT(blk_ix_bit));
  }
}

/****************************************************************************************************//**
 *                                       FS_NOR_FTL_SetBlkSecCntValid()
 *
 * @brief    Set block valid sector count.
 *
 * @param    p_nor_ftl       Pointer to a NOR FTL instance.
 *
 * @param    blk_ix          Block index.
 *
 * @param    sec_cnt_valid   Valid sector count.
 *******************************************************************************************************/
static void FS_NOR_FTL_SetBlkSecCntValid(FS_NOR_FTL *p_nor_ftl,
                                         FS_LB_NBR  blk_ix,
                                         FS_LB_QTY  sec_cnt_valid)
{
  CPU_INT08U *p_sec_cnt_tbl_08;
  CPU_INT16U *p_sec_cnt_tbl_16;

  if (p_nor_ftl->BlkSecCnts < DEF_INT_08U_MAX_VAL) {
    p_sec_cnt_tbl_08 = (CPU_INT08U *)p_nor_ftl->BlkSecCntValidTbl;
    p_sec_cnt_tbl_08[blk_ix] = (CPU_INT08U)sec_cnt_valid;
  } else {
    p_sec_cnt_tbl_16 = (CPU_INT16U *)p_nor_ftl->BlkSecCntValidTbl;
    p_sec_cnt_tbl_16[blk_ix] = (CPU_INT16U)sec_cnt_valid;
  }
}

/****************************************************************************************************//**
 *                                       FS_NOR_FTL_DecBlkSecCntValid()
 *
 * @brief    Decrement block valid sector count.
 *
 * @param    p_nor_ftl   Pointer to a NOR FTL instance.
 *
 * @param    blk_ix      Block index.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NOR_FTL_DecBlkSecCntValid(FS_NOR_FTL *p_nor_ftl,
                                         FS_LB_NBR  blk_ix)
{
  CPU_INT08U *p_sec_cnt_tbl_08;
  CPU_INT16U *p_sec_cnt_tbl_16;
  CPU_INT08U sec_cnt_08;
  CPU_INT16U sec_cnt_16;

  if (p_nor_ftl->BlkSecCnts < DEF_INT_08U_MAX_VAL) {
    p_sec_cnt_tbl_08 = (CPU_INT08U *)p_nor_ftl->BlkSecCntValidTbl;
    sec_cnt_08 = p_sec_cnt_tbl_08[blk_ix];

#if (FS_NOR_CFG_DBG_CHK_EN == DEF_ENABLED)
    if (sec_cnt_08 == 0u) {
      LOG_ERR(("Sec cnt for blk ", (u)blk_ix, " already 0."));
      return;
    }
#endif

    p_sec_cnt_tbl_08[blk_ix] = sec_cnt_08 - 1u;
  } else {
    p_sec_cnt_tbl_16 = (CPU_INT16U *)p_nor_ftl->BlkSecCntValidTbl;
    sec_cnt_16 = p_sec_cnt_tbl_16[blk_ix];

#if (FS_NOR_CFG_DBG_CHK_EN == DEF_ENABLED)
    if (sec_cnt_16 == 0u) {
      LOG_ERR(("Sec cnt for blk ", (u)blk_ix, " already 0."));
      return;
    }
#endif

    p_sec_cnt_tbl_16[blk_ix] = sec_cnt_16 - 1u;
  }
}
#endif

/****************************************************************************************************//**
 *                                       FS_NOR_FTL_IncBlkSecCntValid()
 *
 * @brief    Increment block valid sector count.
 *
 * @param    p_nor_ftl   Pointer to a NOR FTL instance.
 *
 * @param    blk_ix      Block index.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NOR_FTL_IncBlkSecCntValid(FS_NOR_FTL *p_nor_ftl,
                                         FS_LB_NBR  blk_ix)
{
  CPU_INT08U *p_sec_cnt_tbl_08;
  CPU_INT16U *p_sec_cnt_tbl_16;
  CPU_INT08U sec_cnt_08;
  CPU_INT16U sec_cnt_16;

  if (p_nor_ftl->BlkSecCnts < DEF_INT_08U_MAX_VAL) {
    p_sec_cnt_tbl_08 = (CPU_INT08U *)p_nor_ftl->BlkSecCntValidTbl;
    sec_cnt_08 = p_sec_cnt_tbl_08[blk_ix];
    p_sec_cnt_tbl_08[blk_ix] = sec_cnt_08 + 1u;
  } else {
    p_sec_cnt_tbl_16 = (CPU_INT16U *)p_nor_ftl->BlkSecCntValidTbl;
    sec_cnt_16 = p_sec_cnt_tbl_16[blk_ix];
    p_sec_cnt_tbl_16[blk_ix] = sec_cnt_16 + 1u;
  }
}
#endif

/****************************************************************************************************//**
 *                                           FS_NOR_FTL_BlkIxToAddr()
 *
 * @brief    Get address of a block.
 *
 * @param    p_nor_ftl   Pointer to a NOR FTL instance.
 *
 * @param    blk_ix      Block index
 *
 * @param    p_err       Error pointer.
 *
 * @return   Block address, relative start of device.
 *******************************************************************************************************/
static CPU_INT32U FS_NOR_FTL_BlkIxToAddr(FS_NOR_FTL *p_nor_ftl,
                                         FS_LB_NBR  blk_ix,
                                         RTOS_ERR   *p_err)
{
  FS_NOR     *p_nor;
  CPU_INT32U blk_addr;

  p_nor = (FS_NOR *)p_nor_ftl->BlkDev.MediaPtr;
  blk_addr = (p_nor_ftl->BlkNbrFirst + blk_ix) << p_nor->PhyPtr->BlkSizeLog2;
  if (blk_addr == DEF_INT_32U_MAX_VAL) {
    LOG_ERR(("Failed to get addr of blk ix ", (u)blk_ix, "."));
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return (blk_addr);
  }

  return (blk_addr);
}

/****************************************************************************************************//**
 *                                       FS_NOR_FTL_SecNbrPhyToAddr()
 *
 * @brief    Get address of a physical sector.
 *
 * @param    p_nor_ftl       Pointer to a NOR FTL instance.
 *
 * @param    sec_nbr_phy     Physical sector number
 *
 * @param    p_err           Error pointer.
 *
 * @return   Sector address, relative start of device.
 *******************************************************************************************************/
static CPU_INT32U FS_NOR_FTL_SecNbrPhyToAddr(FS_NOR_FTL *p_nor_ftl,
                                             FS_LB_NBR  sec_nbr_phy,
                                             RTOS_ERR   *p_err)
{
  FS_NOR     *p_nor;
  FS_LB_NBR  blk_ix;
  FS_LB_NBR  sec_ix;
  CPU_INT32U sec_addr;

  p_nor = (FS_NOR *)p_nor_ftl->BlkDev.MediaPtr;

  blk_ix = p_nor_ftl->BlkNbrFirst;
  blk_ix += sec_nbr_phy / p_nor_ftl->BlkSecCnts;
  sec_ix = sec_nbr_phy % p_nor_ftl->BlkSecCnts;
  sec_addr = FS_UTIL_MULT_PWR2(blk_ix, p_nor->PhyPtr->BlkSizeLog2) + FS_DEV_NOR_BLK_HDR_LEN
             + FS_UTIL_MULT_PWR2(sec_ix, p_nor_ftl->SecSizeLog2) + FS_UTIL_MULT_PWR2(sec_ix, FS_DEV_NOR_SEC_HDR_LEN_LOG);

  if (sec_addr == DEF_INT_32U_MAX_VAL) {
    LOG_ERR(("Failed to get addr of sec ", (u)sec_nbr_phy, "."));
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return (sec_addr);
  }

  return (sec_addr);
}

/****************************************************************************************************//**
 *                                       FS_NOR_FTL_SecNbrPhyToBlkIx()
 *
 * @brief    Get index of block in which a physical sector is located.
 *
 * @param    p_nor_ftl       Pointer to a NOR FTL instance.
 *
 * @param    sec_nbr_phy     Physical sector number
 *
 * @param    p_err           Error pointer.
 *
 * @return   Block index.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static FS_LB_NBR FS_NOR_FTL_SecNbrPhyToBlkIx(FS_NOR_FTL *p_nor_ftl,
                                             FS_LB_NBR  sec_nbr_phy,
                                             RTOS_ERR   *p_err)
{
  FS_LB_NBR blk_ix;

  blk_ix = sec_nbr_phy / p_nor_ftl->BlkSecCnts;
  if (blk_ix == FS_DEV_NOR_SEC_NBR_INVALID) {
    LOG_ERR(("Failed to get ix of blk containing sec ", (u)sec_nbr_phy, "."));
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return (blk_ix);
  }

  return (blk_ix);
}
#endif

/****************************************************************************************************//**
 *                                       FS_NOR_FTL_BlkIxToSecNbrPhy()
 *
 * @brief    Get sector number of a block.
 *
 * @param    p_nor_ftl   Pointer to a NOR FTL instance.
 *
 * @param    blk_ix      Block index.
 *
 * @return   Sector number.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static FS_LB_NBR FS_NOR_FTL_BlkIxToSecNbrPhy(FS_NOR_FTL *p_nor_ftl,
                                             FS_LB_NBR  blk_ix)
{
  return (blk_ix * p_nor_ftl->BlkSecCnts);
}
#endif

/****************************************************************************************************//**
 *                                           FS_NOR_FTL_L2P_Create()
 *
 * @brief    Allocate logical-to-physical table.
 *
 * @param    p_nor_ftl   Pointer to a NOR FTL instance.
 *
 * @param    p_seg       Pointer to a memory segment where to allocate the logical-to-physical table.
 *
 * @param    p_err       Error pointer.
 *
 * @return   Pointer to L2P table, if NO errors.
 *           Pointer to NULL,      otherwise.
 *******************************************************************************************************/
static void *FS_NOR_FTL_L2P_Create(FS_NOR_FTL *p_nor_ftl,
                                   MEM_SEG    *p_seg,
                                   RTOS_ERR   *p_err)
{
  void       *p_l2p_tbl;
  CPU_SIZE_T l2p_tbl_size;
  CPU_INT32U l2p_tbl_size_bit;

  l2p_tbl_size_bit = (CPU_INT32U)p_nor_ftl->DiskSizeSec * (CPU_INT32U)p_nor_ftl->SecCntTotLog;
  l2p_tbl_size = (CPU_SIZE_T)((l2p_tbl_size_bit + DEF_OCTET_NBR_BITS - 1u) / DEF_OCTET_NBR_BITS);
  p_l2p_tbl = (void *)Mem_SegAlloc("FS - NOR l2p tbl",
                                   p_seg,
                                   l2p_tbl_size,
                                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  return (p_l2p_tbl);
}

/****************************************************************************************************//**
 *                                           FS_NOR_FTL_L2P_GetEntry()
 *
 * @brief    Get logical-to-physical table entry for a logical sector.
 *
 * @param    p_nor_ftl           Pointer to a NOR FTL instance
 *
 * @param    sec_nbr_logical     Logical sector number.
 *
 * @return   Physical sector number,     if table entry     set.
 *           FS_DEV_NOR_SEC_NBR_INVALID, if table entry NOT set (see Note #1).
 *
 * @note     (1) FS_DEV_NOR_SEC_NBR_INVALID must be defined to DEF_INT_32U_MAX_VAL.
 *               - (a) DEF_INT_32U_MAX_VAL cannot be stored in a table entry; consequently, a placeholder
 *                     value, the maximum integer for the entry width, is set instead & must be detected/
 *                     handled upon get.
 *******************************************************************************************************/
static FS_LB_NBR FS_NOR_FTL_L2P_GetEntry(FS_NOR_FTL *p_nor_ftl,
                                         FS_LB_NBR  sec_nbr_logical)
{
  CPU_INT08U *p_l2p_tbl_08;
  CPU_INT08U bits_rd;
  CPU_INT08U bits_rem;
  CPU_INT32U l2p_bit_off;
  CPU_INT32U l2p_octet_off;
  CPU_INT08U l2p_octet_bit_off;
  CPU_INT08U l2p_rd_octet;
  FS_LB_NBR  sec_nbr_phy;

  l2p_bit_off = (CPU_INT32U)sec_nbr_logical * (CPU_INT32U)p_nor_ftl->SecCntTotLog;
  l2p_octet_off = l2p_bit_off / DEF_OCTET_NBR_BITS;
  l2p_octet_bit_off = (CPU_INT08U)(l2p_bit_off % DEF_OCTET_NBR_BITS);
  p_l2p_tbl_08 = (CPU_INT08U *)p_nor_ftl->L2P_Tbl + l2p_octet_off;
  bits_rem = p_nor_ftl->SecCntTotLog;
  sec_nbr_phy = 0u;

  while (bits_rem > 0u) {
    bits_rd = DEF_MIN(DEF_OCTET_NBR_BITS - l2p_octet_bit_off, bits_rem);

    l2p_rd_octet = *p_l2p_tbl_08;                               // Rd tbl octet ...
    l2p_rd_octet >>= l2p_octet_bit_off;                         // ... shift bits to lowest ...
    l2p_rd_octet &= DEF_BIT_FIELD(bits_rd, 0u);                 // ... mask off other bits  ...
                                                                // ... OR w/datum.
    sec_nbr_phy |= (FS_LB_NBR)l2p_rd_octet << (p_nor_ftl->SecCntTotLog - bits_rem);
    p_l2p_tbl_08++;

    bits_rem -= bits_rd;
    l2p_octet_bit_off = 0u;                                     // Next rd will be at start of next octet.
  }

  if (sec_nbr_phy == DEF_BIT_FIELD(p_nor_ftl->SecCntTotLog, 0u)) {
    sec_nbr_phy = DEF_INT_32U_MAX_VAL;
  }

  return (sec_nbr_phy);
}

/****************************************************************************************************//**
 *                                           FS_NOR_FTL_L2P_SetEntry()
 *
 * @brief    Set logical-to-physical table entry for a logical sector.
 *
 * @param    p_nor_ftl           Pointer to a NOR FTL instance.
 *
 * @param    sec_nbr_logical     Logical  sector number.
 *
 * @param    sec_nbr_phy         Physical sector number,     if table entry to be set.
 *                               OR
 *                               FS_DEV_NOR_SEC_NBR_INVALID, if table entry to be cleared (see Note #1).
 *
 * @note     (1) FS_DEV_NOR_SEC_NBR_INVALID must be defined to DEF_INT_32U_MAX_VAL.
 *               - (a) DEF_INT_32U_MAX_VAL cannot be stored in a table entry; consequently, a placeholder
 *                     value, the maximum integer for the entry width, is set instead & must be detected/
 *                     handled upon get.
 *******************************************************************************************************/
static void FS_NOR_FTL_L2P_SetEntry(FS_NOR_FTL *p_nor_ftl,
                                    FS_LB_NBR  sec_nbr_logical,
                                    FS_LB_NBR  sec_nbr_phy)
{
  CPU_INT08U *p_l2p_tbl_08;
  CPU_INT08U bits_rem;
  CPU_INT08U bits_wr;
  CPU_INT32U l2p_bit_off;
  CPU_INT32U l2p_octet_off;
  CPU_INT08U l2p_octet_bit_off;
  CPU_INT08U l2p_rd_octet;
  CPU_INT08U l2p_wr_octet;

  if (sec_nbr_phy == DEF_INT_32U_MAX_VAL) {
    sec_nbr_phy = DEF_BIT_FIELD(p_nor_ftl->SecCntTotLog, 0u);
  }

  l2p_bit_off = (CPU_INT32U)sec_nbr_logical * (CPU_INT32U)p_nor_ftl->SecCntTotLog;
  l2p_octet_off = l2p_bit_off / DEF_OCTET_NBR_BITS;
  l2p_octet_bit_off = (CPU_INT08U)(l2p_bit_off % DEF_OCTET_NBR_BITS);
  p_l2p_tbl_08 = (CPU_INT08U *)p_nor_ftl->L2P_Tbl + l2p_octet_off;
  bits_rem = p_nor_ftl->SecCntTotLog;

  while (bits_rem > 0u) {
    bits_wr = DEF_MIN(DEF_OCTET_NBR_BITS - l2p_octet_bit_off, bits_rem);

    l2p_rd_octet = *p_l2p_tbl_08;                               // Rd current tbl entry.
    l2p_wr_octet = (CPU_INT08U)(sec_nbr_phy << l2p_octet_bit_off) & DEF_OCTET_MASK;
    l2p_rd_octet &= ~DEF_BIT_FIELD(bits_wr, l2p_octet_bit_off);
    l2p_wr_octet &= DEF_BIT_FIELD(bits_wr, l2p_octet_bit_off);
    l2p_wr_octet |= l2p_rd_octet;                               // OR current entry with new bits...
    *p_l2p_tbl_08 = l2p_wr_octet;                               // ... update tbl.
    p_l2p_tbl_08++;

    bits_rem -= bits_wr;
    sec_nbr_phy >>= bits_wr;
    l2p_octet_bit_off = 0u;                                     // Next wr will be at start of next octet.
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_FS_STORAGE_NOR_AVAIL
