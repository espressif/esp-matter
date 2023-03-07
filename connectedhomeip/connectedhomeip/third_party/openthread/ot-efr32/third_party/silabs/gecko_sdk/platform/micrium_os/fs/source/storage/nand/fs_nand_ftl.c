/***************************************************************************//**
 * @file
 * @brief File System - NAND FTL Operations
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

/****************************************************************************************************//**
 * @note     (1) Supports NAND-type Flash memory devices, including :
 *             - (a) Parallel NAND Flash.
 *                 - (1) Small-page (512-B) SLC Devices.
 *                 - (2) Large-page (2048-, 4096-, 8096-B) SLC Devices.
 *                 - (3) Some MLC Devices.
 *
 * @note     (2) Supported media MUST have the following characteristics :
 *             - (a) Medium organized into units (called blocks) which are the smallest erasable unit.
 *             - (b) When erased, all bits are set to 1.
 *             - (c) Only an erase operation can change a bit from 0 to 1.
 *             - (d) Each block divided into smaller units (called pages) which are the smallest
 *                   writable unit (unless partial-page programming is supported): each page has a
 *                   data area as well as a spare area. Spare area requirement are determined
 *                   at initialization time depending on different parameters. If the spare area
 *                   is insufficient for the parameters, an error code will be returned to the application
 *                   opening a NAND block device.
 *
 * @note     (3) Supported media TYPICALLY have the following characteristics :
 *             - (a) A  program operation takes much longer than a read    operation.
 *             - (b) An erase   operation takes much longer than a program operation.
 *             - (c) The number of erase operations per block is limited.
 *
 * @note     (4) Logical block indexes are mapped to data blocks but also to update and metadata
 *               blocks.
 *             - (a) Logical indexes 0 to (NUMBER OF LOGICAL DATA BLOCKS - 1) are mapped to logical data
 *                   blocks 0 to (NUMBER OF LOGICAL DATA BLOCKS).
 *             - (b) Logical indexes (NUMBER OF LOGICAL DATA BLOCKS) to (NUMBER OF LOGICAL DATA BLOCKS
 *                   + NUMBER OF UPDATE BLOCKS - 1) are mapped to update blocks 0 to (NUMBER OF UPDATE
 *                   BLOCKS - 1)
 *             - (c) Logical index (NUMBER OF LOGICAL DATA BLOCKS + NUMBER OF UPDATE BLOCK) is mapped
 *                   to active metadata block.
 *******************************************************************************************************/

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

#include  <common/include/rtos_path.h>
#include  <fs_storage_cfg.h>

#include  <cpu/include/cpu.h>

#include  <fs/source/storage/nand/fs_nand_ftl_priv.h>
#include  <fs/source/shared/cleanup/cleanup_mgmt_priv.h>

#include  <fs/source/shared/fs_utils_priv.h>

#include  <fs/source/shared/crc/crc_utils.h>
#include  <common/include/lib_ascii.h>
#include  <common/include/lib_mem.h>
#include  <common/include/lib_str.h>
#include  <common/source/logging/logging_priv.h>
#include  <common/source/collections/bitmap_priv.h>

#include  <common/include/rtos_err.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                    (FS, DRV, NAND)
#define  RTOS_MODULE_CUR                                RTOS_CFG_MODULE_FS

#define  FS_NAND_DRV_NAME_LEN                          4u

#define  FS_NAND_HDR_BLK_NBR                           1u
#define  FS_NAND_VALID_META_BLK_NBR                    1u

#define  FS_NAND_META_SEQ_QTY_HALF_RANGE              (DEF_GET_U_MAX_VAL(FS_NAND_META_ID) >> 1u)

#define  FS_NAND_MAP_SRCH_SIZE_OCTETS                  4u
#define  FS_NAND_MAP_SRCH_SIZE_BITS                   (DEF_OCTET_NBR_BITS * FS_NAND_MAP_SRCH_SIZE_OCTETS)

//                                                                 NAND data types 'undefined' val.
#define  FS_NAND_BLK_IX_INVALID                        DEF_GET_U_MAX_VAL(FS_NAND_BLK_QTY)
#define  FS_NAND_UB_IX_INVALID                         DEF_GET_U_MAX_VAL(FS_NAND_UB_QTY)
#define  FS_NAND_SEC_OFFSET_IX_INVALID                 DEF_GET_U_MAX_VAL(FS_NAND_SEC_PER_BLK_QTY)
#define  FS_NAND_ERASE_QTY_INVALID                     DEF_GET_U_MAX_VAL(FS_NAND_ERASE_QTY)
#define  FS_NAND_ASSOC_BLK_IX_INVALID                  DEF_GET_U_MAX_VAL(FS_NAND_ASSOC_BLK_QTY)

#define  FS_NAND_META_ID_STALE_THRESH                 (DEF_GET_U_MAX_VAL(FS_NAND_META_ID) / 4u)

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef CPU_INT32U FS_NAND_ERASE_QTY;
typedef CPU_INT08U FS_NAND_META_SEC_QTY;
typedef CPU_INT16U FS_NAND_META_ID;
typedef CPU_INT08U FS_NAND_META_SEQ_STATUS_STO;

typedef struct fs_nand_avail_blk_entry {
  FS_NAND_BLK_QTY   BlkIxPhy;
  FS_NAND_ERASE_QTY EraseCnt;
} FS_NAND_AVAIL_BLK_ENTRY;

typedef struct fs_nand_ub_data {
  FS_NAND_BLK_QTY BlkIxPhy;
  CPU_INT08U      *SecValidBitMap;
} FS_NAND_UB_DATA;

/********************************************************************************************************
 *                                           UB EXTRA DATA TYPE
 *
 * Note(s) : (1) Contains data related to update blocks that is not stored in metadata sectors. Recreated
 *               on mount using stored metadata and actual update block sector out-of-sector data.
 *******************************************************************************************************/

typedef struct fs_nand_ub_extra_data {
  FS_NAND_BLK_QTY         *AssocLogicalBlksTbl;                 // Logical blks associated with UB.
  CPU_INT08U              *LogicalToPhySecMap;                  // Map linking logical sec to phy sec.
  FS_NAND_ASSOC_BLK_QTY   AssocLvl;                             // Assoc lvl, nbr of associated logical blks.
  FS_NAND_SEC_PER_BLK_QTY NextSecIx;                            // Ix of next empty sec of UB.
  CPU_INT16U              ActivityCtr;                          // Ctr updated on wr. Allows idleness calc.
#if (FS_NAND_CFG_UB_META_CACHE_EN == DEF_ENABLED)
  CPU_INT08U              *MetaCachePtr;                        // Optional cache of meta.
#endif
} FS_NAND_UB_EXTRA_DATA;

/********************************************************************************************************
 *                                           UB SEC DATA TYPE
 *
 * Note(s) : (1) Contains data on location of a specific logical sector in an update block.
 *******************************************************************************************************/

typedef struct fs_nand_ub_sec_data {
  FS_NAND_UB_QTY          UB_Ix;                                // UB ix.
  FS_NAND_ASSOC_BLK_QTY   AssocLogicalBlksTblIx;                // Logical blk's location in assoc logical blks tbl.
  FS_NAND_SEC_PER_BLK_QTY SecOffsetPhy;                         // Phy offset of sec in UB.
} FS_NAND_UB_SEC_DATA;

/********************************************************************************************************
 *                                       NAND DEVICE DATA DATA TYPE
 *******************************************************************************************************/

typedef struct fs_nand_ftl {
  //                                                               --------------------- BLK DEV ----------------------
  FS_BLK_DEV              BlkDev;                               // Generic block device.

  //                                                               -------------------- SIZE INFO ---------------------
  FS_NAND_BLK_QTY         BlkCnt;                               // Total blk cnt.
  FS_NAND_BLK_QTY         BlkIxFirst;                           // Ix of first blk to be used.
  CPU_SIZE_T              SecSize;                              // Sec size in octets.
  CPU_SIZE_T              SecCnt;                               // Logical sec cnt.
  FS_NAND_SEC_PER_BLK_QTY NbrSecPerBlk;                         // Nbr of sec per blk.
  CPU_INT08U              NbrSecPerBlkLog2;                     // Log2 of nbr of sec per blk.
  FS_NAND_BLK_QTY         LogicalDataBlkCnt;                    // Nbr of logical data blks.
  CPU_INT08U              UsedMarkSize;                         // Size of used mark in octets.

  //                                                               -------------------- STATE INFO --------------------
  CPU_INT16U              ActivityCtr;                          // Activity ctr.

  //                                                               ---------------------- METADATA --------------------
  CPU_INT08U              *MetaCache;                           // Cached copy of metadata stored on dev.
  CPU_INT16U              MetaSize;                             // Total size in octets of meta.
  CPU_INT08U              MetaSecCnt;                           // Total size in sec    of meta.
  FS_NAND_META_ID         MetaBlkID;                            // Sequential ID of current metadata block.

  CPU_INT16U              MetaOffsetBadBlkTbl;                  // Offset in octets of bad   blk tbl in meta.
  CPU_INT16U              MetaOffsetAvailBlkTbl;                // Offset in octets of avail blk tbl in meta.
  CPU_INT16U              MetaOffsetDirtyBitmap;                // Offset in octets of dirty bitmap  in meta.
  CPU_INT16U              MetaOffsetUB_Tbl;                     // Offset in octets of UB tbl        in meta.

  //                                                               ----------------- METADATA BLK INFO ----------------
  FS_NAND_BLK_QTY         MetaBlkIxPhy;                         // Phy blk ix of meta blk.
  CPU_BOOLEAN             MetaBlkFoldNeeded;                    // Flag to indicate needed meta blk fold is needed.
  CPU_INT08U              *MetaBlkInvalidSecMap;                // Bitmap of invalid sec in meta blk.
  FS_NAND_SEC_PER_BLK_QTY MetaBlkNextSecIx;                     // Ix of next sec to use in meta blk.

  //                                                               ------------------- DIRTY BITMAP -------------------
  FS_NAND_BLK_QTY         DirtyBitmapSrchPos;                   // Cur srch pos in dirty bitmap.

  //                                                               ---------------- DIRTY BITMAP CACHE ----------------
#if (FS_NAND_CFG_DIRTY_MAP_CACHE_EN == DEF_ENABLED)
  CPU_INT08U *DirtyBitmapCache;                                 // Intact copy of dirty bitmap stored on dev.
  CPU_INT16U DirtyBitmapSize;                                   // Total size in octets for dirty bitmap.
#endif

  //                                                               ---------------- AVAIL BLK TBL INFO ----------------
  CPU_BOOLEAN           AvailBlkTblInvalidated;                 // Flag actived when avail blk tbl changes.
  CPU_INT08U            *AvailBlkTblCommitMap;                  // Bitmap indicating commit state of avail blks.
  FS_NAND_BLK_QTY       AvailBlkTblEntryCntMax;                 // Nbr of entries in avail blk tbl.
  CPU_INT08U            *AvailBlkMetaMap;                       // Bitmap indicating blks that contain metadata.
  FS_NAND_META_ID       *AvailBlkMetaID_Tbl;                    // Available metadata block ID table.

  //                                                               ---------------- UNPACKED METADATA -----------------
  FS_NAND_BLK_QTY       *LogicalToPhyBlkMap;                    // Logical to phy blk ix map.

  //                                                               --------------------- UB info ----------------------
  FS_NAND_UB_QTY        UB_CntMax;                              // Max nbr of UBs.
  CPU_INT08U            UB_SecMapNbrBits;                       // Resolution in bits of UB sec mapping.
  FS_NAND_UB_EXTRA_DATA *UB_ExtraDataTbl;                       // UB extra data tbl.
  FS_NAND_UB_QTY        SUB_Cnt;                                // Nbr of SUBs.
  FS_NAND_UB_QTY        SUB_CntMax;                             // Max nbr of SUBs.
  FS_NAND_ASSOC_BLK_QTY RUB_MaxAssoc;                           // Max assoc of RUBs.
  CPU_INT08U            RUB_MaxAssocLog2;                       // Log2 of max assoc of RUBs.

  //                                                               ------------------ UB TH PARAMS --------------------
  //                                                               See FS_NAND_SecWrInUB() note #2.
  FS_NAND_SEC_PER_BLK_QTY ThSecWrCnt_MergeRUBStartSUB;          // Threshold to start SUB when RUB exists.
  FS_NAND_SEC_PER_BLK_QTY ThSecRemCnt_ConvertSUBToRUB;          // Threshold to convert a SUB to a RUB.
  FS_NAND_SEC_PER_BLK_QTY ThSecGapCnt_PadSUB;                   // Threshold to pad a SUB.

  //                                                               See FS_NAND_UB_Alloc() note #2.
  FS_NAND_SEC_PER_BLK_QTY ThSecRemCnt_MergeSUB;                 // Threshold to merge SUB instead of RUB.

  //                                                               ---------------------- BUFFERS ---------------------
  void                    *BufPtr;                              // Buffer for sec data.
  void                    *OOS_BufPtr;                          // Buffer for OOS data.

  CPU_BOOLEAN             IsMounted;

#if ((FS_STORAGE_CFG_CTR_STAT_EN == DEF_ENABLED) \
  || (FS_STORAGE_CFG_CTR_ERR_EN == DEF_ENABLED))
  FS_NAND_CTRS Ctrs;                                            // Stat and err ctrs.
#endif

  SLIST_MEMBER ListMember;
} FS_NAND_FTL;

/********************************************************************************************************
 *                                           NAND HEADER DATA TYPE
 *******************************************************************************************************/

typedef CPU_INT32U FS_NAND_HDR_MARK_1_TYPE;
typedef CPU_INT32U FS_NAND_HDR_MARK_2_TYPE;
typedef CPU_INT16U FS_NAND_HDR_VER_TYPE;
typedef FS_LB_SIZE FS_NAND_HDR_SEC_SIZE_TYPE;
typedef CPU_INT32U FS_NAND_HDR_BLK_CNT_TYPE;
typedef CPU_INT32U FS_NAND_HDR_BLK_NBR_FIRST_TYPE;
typedef CPU_INT32U FS_NAND_HDR_UB_CNT_TYPE;
typedef CPU_INT32U FS_NAND_HDR_MAX_ASSOC_TYPE;
typedef CPU_INT32U FS_NAND_HDR_AVAIL_BLK_TBL_SIZE_TYPE;
typedef CPU_SIZE_T FS_NAND_HDR_OOS_SIZE_TYPE;
typedef CPU_INT32U FS_NAND_HDR_MAX_BAD_BLK_CNT_TYPE;

/********************************************************************************************************
 *                                           NAND OOS DATA TYPES
 *
 * Note(s) : (1) FS_NAND_SEC_TYPE_STO must be able to store all the enum values from FS_NAND_SEC_TYPE.
 *
 *               (a) FS_NAND_SEC_TYPE_STO contains the mark that will be written on the device. To save
 *                   space on device, the type chosen must be as small as possible.
 *
 *               (b) FS_NAND_SEC_TYPE uses enum values to facilitate debugging.
 *******************************************************************************************************/

typedef enum fs_nand_sec_type {                                 // Sec types. See note #1.
  FS_NAND_SEC_TYPE_INVALID = 0x00u,
  FS_NAND_SEC_TYPE_UNUSED = 0xFFu,
  FS_NAND_SEC_TYPE_STORAGE = 0xF0u,
  FS_NAND_SEC_TYPE_METADATA = 0x0Fu,
  FS_NAND_SEC_TYPE_HDR = 0xAAu,
  FS_NAND_SEC_TYPE_DUMMY = 0x55u
} FS_NAND_SEC_TYPE;

typedef CPU_INT08U FS_NAND_SEC_TYPE_STO;

//                                                                 Seq statuses.
typedef enum fs_nand_meta_seq_status {
  FS_NAND_META_SEQ_NEW = 0xAAu,
  FS_NAND_META_SEQ_UNFINISHED = 0xFFu,
  FS_NAND_META_SEQ_AVAIL_BLK_TBL_ONLY = 0xF0u,
  FS_NAND_META_SEQ_FINISHED = 0x00u
} FS_NAND_META_SEQ_STATUS;

/********************************************************************************************************
 *                                       NAND OOS USAGE ENUM
 *******************************************************************************************************/

enum fs_nand_oos_structure {
  //                                                               Common to all blk types.
  FS_NAND_OOS_SEC_TYPE_OFFSET             =  0u,
  FS_NAND_OOS_ERASE_CNT_OFFSET            =  FS_NAND_OOS_SEC_TYPE_OFFSET           + sizeof(FS_NAND_SEC_TYPE_STO),

  //                                                               Specific to sto blk.
  FS_NAND_OOS_STO_LOGICAL_BLK_IX_OFFSET   =  FS_NAND_OOS_ERASE_CNT_OFFSET          + sizeof(FS_NAND_ERASE_QTY),
  FS_NAND_OOS_STO_BLK_SEC_IX_OFFSET       =  FS_NAND_OOS_STO_LOGICAL_BLK_IX_OFFSET + sizeof(FS_NAND_BLK_QTY),
  FS_NAND_OOS_STO_SEC_SIZE_REQD           =  FS_NAND_OOS_STO_BLK_SEC_IX_OFFSET     + sizeof(FS_NAND_SEC_PER_BLK_QTY),

  //                                                               Specific to metadata blk.
  FS_NAND_OOS_META_SEC_IX_OFFSET          =  FS_NAND_OOS_ERASE_CNT_OFFSET          + sizeof(FS_NAND_ERASE_QTY),
  FS_NAND_OOS_META_ID_OFFSET              =  FS_NAND_OOS_META_SEC_IX_OFFSET        + sizeof(FS_NAND_META_SEC_QTY),
  FS_NAND_OOS_META_SEQ_STATUS_OFFSET      =  FS_NAND_OOS_META_ID_OFFSET            + sizeof(FS_NAND_META_ID),
  FS_NAND_OOS_META_SEC_SIZE_REQD          =  FS_NAND_OOS_META_SEQ_STATUS_OFFSET    + sizeof(FS_NAND_META_SEQ_STATUS_STO),

  //                                                               Size does not include used mark.
  FS_NAND_OOS_PARTIAL_SIZE_REQD           =  DEF_MAX(FS_NAND_OOS_META_SEC_SIZE_REQD, FS_NAND_OOS_STO_SEC_SIZE_REQD),

  FS_NAND_OOS_SEC_USED_OFFSET             =  FS_NAND_OOS_PARTIAL_SIZE_REQD
};

/********************************************************************************************************
 *                                           NAND HDR ENUM
 *
 * Note(s) : (1) The flash translation layer (FTL) version number is not equal to the filesystem or driver
 *               version number. When the FTL version number is incremented, it means the changes are
 *               different enough for previous formats not to be compatible, and that the device must be
 *               low-level formatted before further use.
 *******************************************************************************************************/

//                                                                 ------------------- HDR OFFSETS --------------------
enum FS_NAND_HDR_STRUCTURE {
  FS_NAND_HDR_MARK_1_OFFSET             = 0u,
  FS_NAND_HDR_MARK_2_OFFSET             = FS_NAND_HDR_MARK_1_OFFSET
                                          + sizeof(FS_NAND_HDR_MARK_1_TYPE),

  FS_NAND_HDR_VER_OFFSET                = FS_NAND_HDR_MARK_2_OFFSET
                                          + sizeof(FS_NAND_HDR_MARK_2_TYPE),

  //                                                               Low lvl cfg.
  FS_NAND_HDR_SEC_SIZE_OFFSET           = FS_NAND_HDR_VER_OFFSET
                                          + sizeof(FS_NAND_HDR_VER_TYPE),

  FS_NAND_HDR_BLK_CNT_OFFSET            = FS_NAND_HDR_SEC_SIZE_OFFSET
                                          + sizeof(FS_NAND_HDR_SEC_SIZE_TYPE),

  FS_NAND_HDR_BLK_NBR_FIRST_OFFSET      = FS_NAND_HDR_BLK_CNT_OFFSET
                                          + sizeof(FS_NAND_HDR_BLK_CNT_TYPE),

  FS_NAND_HDR_UB_CNT_OFFSET             = FS_NAND_HDR_BLK_NBR_FIRST_OFFSET
                                          + sizeof(FS_NAND_HDR_BLK_NBR_FIRST_TYPE),

  FS_NAND_HDR_MAX_ASSOC_OFFSET          = FS_NAND_HDR_UB_CNT_OFFSET
                                          + sizeof(FS_NAND_HDR_UB_CNT_TYPE),

  FS_NAND_HDR_AVAIL_BLK_TBL_SIZE_OFFSET = FS_NAND_HDR_MAX_ASSOC_OFFSET
                                          + sizeof(FS_NAND_HDR_MAX_ASSOC_TYPE),

  //                                                               Part data.
  FS_NAND_HDR_OOS_SIZE_OFFSET           = FS_NAND_HDR_AVAIL_BLK_TBL_SIZE_OFFSET
                                          + sizeof(FS_NAND_HDR_AVAIL_BLK_TBL_SIZE_TYPE),

  FS_NAND_HDR_MAX_BAD_BLK_CNT_OFFSET    = FS_NAND_HDR_OOS_SIZE_OFFSET
                                          + sizeof(FS_NAND_HDR_OOS_SIZE_TYPE),

  //                                                               Total size.
  FS_NAND_HDR_TOTAL_SIZE                = FS_NAND_HDR_MAX_BAD_BLK_CNT_OFFSET
                                          + sizeof(FS_NAND_HDR_MAX_BAD_BLK_CNT_TYPE)
};

//                                                                 ------------------- HDR CONSTS ---------------------
enum FS_NAND_HDR_CONSTS {
  FS_NAND_HDR_MARK_WORD1               = 0x534643E6u,           // Marker word 1 ("  CFS")
  FS_NAND_HDR_MARK_WORD2               = 0x444E414Eu,           // Marker word 2 ("NAND")
  FS_NAND_HDR_VER                      =     0x0001u            // FTL version (1) (See Note #1).
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static SLIST_MEMBER *FS_NAND_FTL_ListHeadPtr;

/********************************************************************************************************
 *                                   NAND DEFAULT CONFIGURATION STRUCTURE
 *******************************************************************************************************/

const FS_NAND_FTL_CFG FS_NAND_CfgDflt = {
  .SecSize = FS_NAND_CFG_SEC_SIZE_AUTO,
  .BlkCnt = FS_NAND_CFG_BLK_CNT_AUTO,
  .BlkIxFirst = 0u,
  .UB_CntMax = 10u,
  .RUB_MaxAssoc = 2u,
  .AvailBlkTblEntryCntMax = DEF_MAX(10u, FS_NAND_CFG_RSVD_AVAIL_BLK_CNT + 1u)
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL MACRO'S
 *
 * Note(s) : (1) FS_NAND_BLK_IX_TO_SEC_IX(), FS_NAND_SEC_IX_TO_BLK_IX() & FS_NAND_UB_IX_TO_LOG_BLK_IX
 *               require p_nand_ftl pointer to be not NULL.
 ********************************************************************************************************
 *******************************************************************************************************/

#define  FS_NAND_BLK_IX_TO_SEC_IX(p_nand_ftl, blk_ix)    (FS_LB_QTY)(((FS_LB_QTY)blk_ix) << (p_nand_ftl)->NbrSecPerBlkLog2)
#define  FS_NAND_SEC_IX_TO_BLK_IX(p_nand_ftl, sec_ix)    (FS_NAND_BLK_QTY)((sec_ix) >> (p_nand_ftl)->NbrSecPerBlkLog2)

#define  FS_NAND_UB_IX_TO_LOG_BLK_IX(p_nand_ftl, ub_ix)     (FS_NAND_BLK_QTY)((ub_ix)   + (p_nand_ftl)->LogicalDataBlkCnt)

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                               BLOCK DEVICE INTERFACE FUNCTION PROTOTYPES
 *******************************************************************************************************/

static FS_BLK_DEV *FS_NAND_FTL_Add(FS_MEDIA *p_media,
                                   RTOS_ERR *p_err);

static void FS_NAND_FTL_Rem(FS_BLK_DEV *p_blk_dev,
                            RTOS_ERR   *p_err);

static void FS_NAND_FTL_Open(FS_BLK_DEV *p_blk_dev,
                             RTOS_ERR   *p_err);

static void FS_NAND_FTL_Close(FS_BLK_DEV *p_blk_dev,
                              RTOS_ERR   *p_err);

static void FS_NAND_FTL_Rd(FS_BLK_DEV *p_blk_dev,
                           void       *p_dest,
                           FS_LB_NBR  sec_start,
                           FS_LB_QTY  sec_cnt,
                           RTOS_ERR   *p_err);

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NAND_FTL_Wr(FS_BLK_DEV *p_blk_dev,
                           void       *p_src,
                           FS_LB_NBR  sec_start,
                           FS_LB_QTY  sec_cnt,
                           RTOS_ERR   *p_err);

static void FS_NAND_FTL_Sync(FS_BLK_DEV *p_blk_dev,
                             RTOS_ERR   *p_err);

static void FS_NAND_FTL_Trim(FS_BLK_DEV *p_blk_dev,
                             FS_LB_NBR  lb_nbr,
                             RTOS_ERR   *p_err);
#endif

static void FS_NAND_FTL_Query(FS_BLK_DEV      *p_blk_dev,
                              FS_BLK_DEV_INFO *p_info,
                              RTOS_ERR        *p_err);

/********************************************************************************************************
 *                                           LOCAL FUNCTION PROTOTYPES
 *******************************************************************************************************/

static FS_NAND_FTL *FS_NAND_FTL_Alloc(FS_NAND               *p_nand,
                                      const FS_NAND_FTL_CFG *p_nand_cfg,
                                      RTOS_ERR              *p_err);

static FS_NAND_FTL *FS_NAND_FTL_Get(FS_MEDIA *p_media);

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NAND_FTL_LowFmtInternal(FS_NAND_FTL *p_nand_ftl,
                                       FS_NAND     *p_nand,
                                       RTOS_ERR    *p_err);
#endif

static void FS_NAND_FTL_LowMountInternal(FS_NAND_FTL *p_nand_ftl,
                                         RTOS_ERR    *p_err);

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NAND_FTL_LowUnmountInternal(FS_NAND_FTL *p_nand_ftl,
                                           RTOS_ERR    *p_err);
#endif
static void FS_NAND_FTL_SecRdHandler(FS_NAND_FTL             *p_nand_ftl,
                                     void                    *p_dest,
                                     void                    *p_dest_oos,
                                     FS_NAND_BLK_QTY         blk_ix_logical,
                                     FS_NAND_SEC_PER_BLK_QTY sec_offset_phy,
                                     RTOS_ERR                *p_err);

static void FS_NAND_FTL_SecRdPhyNoRefresh(FS_NAND_FTL             *p_nand_ftl,
                                          void                    *p_dest,
                                          void                    *p_dest_oos,
                                          FS_NAND_BLK_QTY         blk_ix_phy,
                                          FS_NAND_SEC_PER_BLK_QTY sec_offset_phy,
                                          RTOS_ERR                *p_err);

static FS_LB_QTY FS_NAND_FTL_SecRd(FS_NAND_FTL *p_nand_ftl,
                                   void        *p_dest,
                                   FS_LB_QTY   sec_ix_logical,
                                   FS_LB_QTY   sec_cnt,
                                   RTOS_ERR    *p_err);

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NAND_FTL_SecWrHandler(FS_NAND_FTL             *p_nand_ftl,
                                     void                    *p_src,
                                     void                    *p_src_oos,
                                     FS_NAND_BLK_QTY         blk_ix_logical,
                                     FS_NAND_SEC_PER_BLK_QTY sec_offset_phy,
                                     RTOS_ERR                *p_err);

static void FS_NAND_FTL_MetaSecWrHandler(FS_NAND_FTL             *p_nand_ftl,
                                         void                    *p_src,
                                         void                    *p_src_oos,
                                         FS_NAND_SEC_PER_BLK_QTY sec_offset_phy,
                                         RTOS_ERR                *p_err);

static FS_LB_QTY FS_NAND_FTL_SecWr(FS_NAND_FTL *p_nand_ftl,
                                   void        *p_src,
                                   FS_LB_QTY   sec_ix_logical,
                                   FS_LB_QTY   sec_cnt,
                                   RTOS_ERR    *p_err);

static FS_LB_QTY FS_NAND_FTL_SecWrInUB(FS_NAND_FTL         *p_nand_ftl,
                                       void                *p_src,
                                       FS_LB_QTY           sec_ix_logical,
                                       FS_LB_QTY           sec_cnt,
                                       FS_NAND_UB_SEC_DATA ub_sec_data,
                                       RTOS_ERR            *p_err);

static FS_LB_QTY FS_NAND_FTL_SecWrInRUB(FS_NAND_FTL    *p_nand_ftl,
                                        void           *p_src,
                                        FS_LB_QTY      sec_ix_logical,
                                        FS_LB_QTY      sec_cnt,
                                        FS_NAND_UB_QTY ub_ix,
                                        RTOS_ERR       *p_err);

static FS_LB_QTY FS_NAND_FTL_SecWrInSUB(FS_NAND_FTL    *p_nand_ftl,
                                        void           *p_src,
                                        FS_LB_QTY      sec_ix_logical,
                                        FS_LB_QTY      sec_cnt,
                                        FS_NAND_UB_QTY ub_ix,
                                        RTOS_ERR       *p_err);

static void FS_NAND_FTL_OOSGenSto(FS_NAND_FTL             *p_nand_ftl,
                                  void                    *p_oos_buf_v,
                                  FS_NAND_BLK_QTY         blk_ix_logical_data,
                                  FS_NAND_BLK_QTY         blk_ix_phy,
                                  FS_NAND_SEC_PER_BLK_QTY sec_offset_logical,
                                  FS_NAND_SEC_PER_BLK_QTY sec_offset_phy,
                                  RTOS_ERR                *p_err);

static void FS_NAND_FTL_HdrWr(FS_NAND_FTL     *p_nand_ftl,
                              FS_NAND_BLK_QTY blk_ix_hdr,
                              RTOS_ERR        *p_err);
#endif

static FS_NAND_BLK_QTY FS_NAND_FTL_HdrRd(FS_NAND_FTL *p_nand_ftl,
                                         RTOS_ERR    *p_err);

static void FS_NAND_FTL_HdrParamsValidate(FS_NAND_FTL *p_nand_ftl,
                                          CPU_INT08U  *p_hdr_data,
                                          RTOS_ERR    *p_err);

static FS_NAND_BLK_QTY FS_NAND_FTL_BlkIxPhyGet(FS_NAND_FTL     *p_nand_ftl,
                                               FS_NAND_BLK_QTY blk_ix_logical);

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NAND_FTL_BlkRefresh(FS_NAND_FTL     *p_nand_ftl,
                                   FS_NAND_BLK_QTY blk_ix_phy,
                                   RTOS_ERR        *p_err);

static void FS_NAND_FTL_BlkMarkBad(FS_NAND_FTL     *p_nand_ftl,
                                   FS_NAND_BLK_QTY blk_ix_phy,
                                   RTOS_ERR        *p_err);

static void FS_NAND_FTL_BlkMarkDirty(FS_NAND_FTL     *p_nand_ftl,
                                     FS_NAND_BLK_QTY blk_ix_phy);

static void FS_NAND_FTL_BlkUnmap(FS_NAND_FTL     *p_nand_ftl,
                                 FS_NAND_BLK_QTY blk_ix_phy);

static void FS_NAND_FTL_BlkAddToAvail(FS_NAND_FTL     *p_nand_ftl,
                                      FS_NAND_BLK_QTY blk_ix_phy,
                                      RTOS_ERR        *p_err);

static FS_NAND_ERASE_QTY FS_NAND_FTL_BlkRemFromAvail(FS_NAND_FTL     *p_nand_ftl,
                                                     FS_NAND_BLK_QTY blk_ix_phy);

static FS_NAND_BLK_QTY FS_NAND_FTL_BlkGetAvailFromTbl(FS_NAND_FTL *p_nand_ftl,
                                                      CPU_BOOLEAN access_rsvd);

static FS_NAND_BLK_QTY FS_NAND_FTL_BlkGetDirty(FS_NAND_FTL *p_nand_ftl,
                                               CPU_BOOLEAN pending_dirty_chk_en,
                                               RTOS_ERR    *p_err);

static FS_NAND_BLK_QTY FS_NAND_FTL_BlkGetErased(FS_NAND_FTL *p_nand_ftl,
                                                RTOS_ERR    *p_err);

static void FS_NAND_FTL_BlkEnsureErased(FS_NAND_FTL     *p_nand_ftl,
                                        FS_NAND_BLK_QTY blk_ix_phy,
                                        RTOS_ERR        *p_err);
#endif

static CPU_BOOLEAN FS_NAND_FTL_BlkIsBad(FS_NAND_FTL     *p_nand_ftl,
                                        FS_NAND_BLK_QTY blk_ix_phy);

static CPU_BOOLEAN FS_NAND_FTL_BlkIsDirty(FS_NAND_FTL     *p_nand_ftl,
                                          FS_NAND_BLK_QTY blk_ix_phy);

static CPU_BOOLEAN FS_NAND_FTL_BlkIsAvail(FS_NAND_FTL     *p_nand_ftl,
                                          FS_NAND_BLK_QTY blk_ix_phy);

static CPU_BOOLEAN FS_NAND_FTL_BlkIsUB(FS_NAND_FTL     *p_nand_ftl,
                                       FS_NAND_BLK_QTY blk_ix_phy);

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static CPU_BOOLEAN FS_NAND_FTL_BlkIsFactoryDefect(FS_NAND_FTL     *p_nand_ftl,
                                                  FS_NAND_BLK_QTY blk_ix_phy,
                                                  RTOS_ERR        *p_err);
#endif

static void FS_NAND_FTL_MetaBlkFind(FS_NAND_FTL     *p_nand_ftl,
                                    FS_NAND_BLK_QTY blk_ix_hdr,
                                    RTOS_ERR        *p_err);

static FS_NAND_BLK_QTY FS_NAND_FTL_MetaBlkFindID(FS_NAND_FTL     *p_nand_ftl,
                                                 FS_NAND_META_ID meta_blk_id,
                                                 RTOS_ERR        *p_err);

static void FS_NAND_FTL_MetaBlkParse(FS_NAND_FTL *p_nand_ftl,
                                     RTOS_ERR    *p_err);

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NAND_FTL_MetaBlkAvailParse(FS_NAND_FTL *p_nand_ftl,
                                          RTOS_ERR    *p_err);

static void FS_NAND_FTL_MetaBlkFold(FS_NAND_FTL *p_nand_ftl,
                                    RTOS_ERR    *p_err);
#endif

#if (FS_NAND_CFG_DIRTY_MAP_CACHE_EN != DEF_ENABLED)
static FS_NAND_SEC_PER_BLK_QTY FS_NAND_FTL_MetaSecFind(FS_NAND_FTL          *p_nand_ftl,
                                                       FS_NAND_META_SEC_QTY meta_sec_ix,
                                                       RTOS_ERR             *p_err);
#endif

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NAND_FTL_MetaSecRangeInvalidate(FS_NAND_FTL             *p_nand_ftl,
                                               FS_NAND_SEC_PER_BLK_QTY sec_ix_first,
                                               FS_NAND_SEC_PER_BLK_QTY sec_ix_last);

static void FS_NAND_FTL_MetaCommit(FS_NAND_FTL *p_nand_ftl,
                                   CPU_BOOLEAN avail_tbl_only,
                                   RTOS_ERR    *p_err);

static void FS_NAND_FTL_MetaSecCommit(FS_NAND_FTL             *p_nand_ftl,
                                      FS_NAND_META_SEC_QTY    meta_sec_ix,
                                      FS_NAND_META_SEQ_STATUS seq_status,
                                      RTOS_ERR                *p_err);

static void FS_NAND_FTL_MetaSecGatherData(FS_NAND_FTL          *p_nand_ftl,
                                          FS_NAND_META_SEC_QTY meta_sec_ix,
                                          CPU_INT08U           *p_buf);

static void FS_NAND_FTL_AvailBlkTblTmpCommit(FS_NAND_FTL *p_nand_ftl,
                                             RTOS_ERR    *p_err);
#endif

static FS_NAND_AVAIL_BLK_ENTRY FS_NAND_FTL_AvailBlkTblEntryRd(FS_NAND_FTL     *p_nand_ftl,
                                                              FS_NAND_BLK_QTY tbl_ix);

#if  (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NAND_FTL_AvailBlkTblEntryWr(FS_NAND_FTL             *p_nand_ftl,
                                           FS_NAND_BLK_QTY         tbl_ix,
                                           FS_NAND_AVAIL_BLK_ENTRY entry);

static FS_NAND_BLK_QTY FS_NAND_FTL_AvailBlkTblFill(FS_NAND_FTL     *p_nand_ftl,
                                                   FS_NAND_BLK_QTY nbr_entries_min,
                                                   CPU_BOOLEAN     pending_dirty_chk_en,
                                                   RTOS_ERR        *p_err);

static FS_NAND_BLK_QTY FS_NAND_FTL_AvailBlkTblEntryCnt(FS_NAND_FTL *p_nand_ftl);
#endif

#if  (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NAND_FTL_AvailBlkTblInvalidate(FS_NAND_FTL *p_nand_ftl);
#endif

static FS_NAND_UB_DATA FS_NAND_FTL_UB_TblEntryRd(FS_NAND_FTL    *p_nand_ftl,
                                                 FS_NAND_UB_QTY tbl_ix);

#if  (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NAND_FTL_UB_TblEntryWr(FS_NAND_FTL     *p_nand_ftl,
                                      FS_NAND_UB_QTY  tbl_ix,
                                      FS_NAND_BLK_QTY blk_ix_phy);

static void FS_NAND_FTL_UB_TblInvalidate(FS_NAND_FTL *p_nand_ftl);
#endif

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NAND_FTL_UB_Create(FS_NAND_FTL    *p_nand_ftl,
                                  FS_NAND_UB_QTY ub_ix,
                                  RTOS_ERR       *p_err);
#endif

static void FS_NAND_FTL_UB_Load(FS_NAND_FTL     *p_nand_ftl,
                                FS_NAND_BLK_QTY blk_ix_phy,
                                RTOS_ERR        *p_err);

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NAND_FTL_UB_Clr(FS_NAND_FTL    *p_nand_ftl,
                               FS_NAND_UB_QTY ub_ix);
#endif

static FS_NAND_UB_SEC_DATA FS_NAND_FTL_UB_Find(FS_NAND_FTL     *p_nand_ftl,
                                               FS_NAND_BLK_QTY blk_ix_logical);

static FS_NAND_UB_SEC_DATA FS_NAND_FTL_UB_SecFind(FS_NAND_FTL             *p_nand_ftl,
                                                  FS_NAND_UB_SEC_DATA     ub_sec_data,
                                                  FS_NAND_SEC_PER_BLK_QTY sec_offset_logical,
                                                  RTOS_ERR                *p_err);

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NAND_FTL_UB_IncAssoc(FS_NAND_FTL     *p_nand_ftl,
                                    FS_NAND_UB_QTY  ub_ix,
                                    FS_NAND_BLK_QTY blk_ix_logical);
#endif

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static FS_NAND_UB_QTY FS_NAND_FTL_UB_Alloc(FS_NAND_FTL *p_nand_ftl,
                                           CPU_BOOLEAN sequential,
                                           RTOS_ERR    *p_err);

static FS_NAND_UB_QTY FS_NAND_FTL_RUB_Alloc(FS_NAND_FTL     *p_nand_ftl,
                                            FS_NAND_BLK_QTY blk_ix_logical,
                                            RTOS_ERR        *p_err);

static void FS_NAND_FTL_RUB_Merge(FS_NAND_FTL    *p_nand_ftl,
                                  FS_NAND_UB_QTY ub_ix,
                                  RTOS_ERR       *p_err);

static void FS_NAND_FTL_RUB_PartialMerge(FS_NAND_FTL     *p_nand_ftl,
                                         FS_NAND_UB_QTY  ub_ix,
                                         FS_NAND_BLK_QTY data_blk_ix_logical,
                                         RTOS_ERR        *p_err);

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static FS_NAND_ASSOC_BLK_QTY FS_NAND_FTL_RUB_AssocBlkIxGet(FS_NAND_FTL     *p_nand_ftl,
                                                           FS_NAND_UB_QTY  ub_ix,
                                                           FS_NAND_BLK_QTY blk_ix_logical);
#endif

static FS_NAND_UB_QTY FS_NAND_FTL_SUB_Alloc(FS_NAND_FTL     *p_nand_ftl,
                                            FS_NAND_BLK_QTY blk_ix_logical,
                                            RTOS_ERR        *p_err);

static void FS_NAND_FTL_SUB_Merge(FS_NAND_FTL    *p_nand_ftl,
                                  FS_NAND_UB_QTY sub_ix,
                                  RTOS_ERR       *p_err);

static void FS_NAND_FTL_SUB_MergeUntil(FS_NAND_FTL             *p_nand_ftl,
                                       FS_NAND_UB_QTY          sub_ix,
                                       FS_NAND_SEC_PER_BLK_QTY sec_end,
                                       RTOS_ERR                *p_err);

static void FS_NAND_FTL_DirtyMapInvalidate(FS_NAND_FTL *p_nand_ftl);

static void FS_NAND_FTL_BadBlkTblInvalidate(FS_NAND_FTL *p_nand_ftl);
#endif

static FS_NAND_SEC_TYPE FS_NAND_FTL_SecTypeParse(CPU_INT08U *p_oos_buf);

static CPU_BOOLEAN FS_NAND_FTL_SecIsUsed(FS_NAND_FTL *p_nand_ftl,
                                         FS_LB_NBR   sec_ix_phy,
                                         RTOS_ERR    *p_err);

static void FS_NAND_FTL_CalcDevInfo(FS_NAND     *p_nand,
                                    FS_NAND_FTL *p_nand_ftl,
                                    RTOS_ERR    *p_err);

static void FS_NAND_FTL_AllocDevData(FS_NAND     *p_nand,
                                     FS_NAND_FTL *p_nand_ftl,
                                     MEM_SEG     *p_seg,
                                     RTOS_ERR    *p_err);

static void FS_NAND_FTL_InitDevData(FS_NAND_FTL *p_nand_ftl);

/********************************************************************************************************
 *                                       NAND DRIVER API STRUCTURE
 *******************************************************************************************************/

const FS_BLK_DEV_API FS_NAND_BlkDevApi = { FS_NAND_FTL_Add,
                                           FS_NAND_FTL_Rem,
                                           FS_NAND_FTL_Open,
                                           FS_NAND_FTL_Close,
                                           FS_NAND_FTL_Rd,
                                            #if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
                                           FS_NAND_FTL_Wr,
                                           FS_NAND_FTL_Sync,
                                           FS_NAND_FTL_Trim,
                                            #endif
                                           FS_NAND_FTL_Query };

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL CONFIGURATION ERRORS
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  FS_NAND_CFG_AUTO_SYNC_EN
#error  "FS_NAND_CFG_AUTO_SYNC_EN not #define's in 'fs_storage_cfg.h'. MUST be DEF_ENABLED or DEF_DISABLED."
#elif  ((FS_NAND_CFG_AUTO_SYNC_EN != DEF_DISABLED) \
  && (FS_NAND_CFG_AUTO_SYNC_EN != DEF_ENABLED))
#error  "FS_NAND_CFG_AUTO_SYNC_EN illegally #define'd in 'fs_storage_cfg.h'. MUST be DEF_ENABLED or DEF_DISABLED."
#endif

#ifndef  FS_NAND_CFG_UB_META_CACHE_EN
#error  "FS_NAND_CFG_UB_META_CACHE_EN not #define'd in 'fs_storage_cfg.h'. MUST be DEF_ENABLED or DEF_DISABLED."

#elif  ((FS_NAND_CFG_UB_META_CACHE_EN != DEF_DISABLED) \
  && (FS_NAND_CFG_UB_META_CACHE_EN != DEF_ENABLED))
#error  "FS_NAND_CFG_UB_META_CACHE_EN illegally #define'd in 'fs_storage_cfg.h'. MUST be DEF_ENABLED or DEF_DISABLED."
#endif

#ifndef  FS_NAND_CFG_DIRTY_MAP_CACHE_EN
#error  "FS_NAND_CFG_DIRTY_MAP_CACHE_EN not #define'd in 'fs_storage_cfg.h'. MUST be DEF_ENABLED or DEF_DISABLED."

#elif  ((FS_NAND_CFG_DIRTY_MAP_CACHE_EN != DEF_DISABLED) \
  && (FS_NAND_CFG_DIRTY_MAP_CACHE_EN != DEF_ENABLED))
#error  "FS_NAND_CFG_DIRTY_MAP_CACHE_EN illegally #define'd in 'fs_storage_cfg.h'. MUST be DEF_ENABLED or DEF_DISABLED."
#endif

#ifndef  FS_NAND_CFG_UB_TBL_SUBSET_SIZE
#error  "FS_NAND_CFG_UB_TBL_SUBSET_SIZE not #define'd in 'fs_storage_cfg.h'. MUST be DEF_ENABLED or DEF_DISABLED."

#elif ((FS_NAND_CFG_UB_TBL_SUBSET_SIZE != 0u) && (FS_UTIL_IS_PWR2(FS_NAND_CFG_UB_TBL_SUBSET_SIZE) == DEF_NO))
#error "FS_NAND_CFG_UB_TBL_SUBSET_SIZE must be a power of 2, or 0."
#endif

#ifndef FS_NAND_CFG_RSVD_AVAIL_BLK_CNT
#error "FS_NAND_CFG_RSVD_AVAIL_BLK_CNT must be #define'd in fs_storage_cfg.h. Must be a positive integer."
#elif  (FS_NAND_CFG_RSVD_AVAIL_BLK_CNT < 0u)
#error "FS_NAND_CFG_RSVD_AVAIL_BLK_CNT must be positive"
#endif

#ifndef FS_NAND_CFG_MAX_RD_RETRIES
#error "FS_NAND_CFG_MAX_RD_RETRIES must be #define'd in fs_storage_cfg.h"
#elif  (FS_NAND_CFG_MAX_RD_RETRIES < 2u)
#error "FS_NAND_CFG_MAX_RD_RETRIES must be at least 2"
#endif

#ifndef FS_NAND_CFG_MAX_SUB_PCT
#error "FS_NAND_CFG_MAX_SUB_PCT must be #define'd in fs_storage_cfg.h"
#elif  (FS_NAND_CFG_MAX_SUB_PCT < 0u)
#error "FS_NAND_CFG_MAX_SUB_PCT must be positive"
#elif  (FS_NAND_CFG_MAX_SUB_PCT > 100u)
#error "FS_NAND_CFG_MAX_SUB_PCT must not be larger than 100"
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       FS_NAND_FTL_ConfigureLowParams()
 *
 * @brief    Configure NAND FTL parameters.
 *
 * @param    media_handle    Handle to a media.
 *
 * @param    p_nand_cfg      Pointer to a NAND FTL configuration structure.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_INVALID_STATE
 *                               - RTOS_ERR_SEG_OVF
 *                               - RTOS_ERR_INVALID_CFG
 *
 * @note     (1) FS_NAND_FTL_ConfigureLowParams() must be called prior to FSBlkDev_Open() or
 *               FSMedia_LowFmt(). It will configure a NAND FTL instance using the user specific
 *               parameters and allocate all needed resources by the FTL instance. Then
 *               FSBlkDev_Open() or FSMedia_LowFmt() can be called and the NAND FTL instance can be
 *               obtained from the internal NAND FTL list. Thus avoiding to reallocate
 *               resources once more time for the same NAND instance.
 *******************************************************************************************************/
void FS_NAND_FTL_ConfigureLowParams(FS_MEDIA_HANDLE       media_handle,
                                    const FS_NAND_FTL_CFG *p_nand_cfg,
                                    RTOS_ERR              *p_err)
{
  FS_NAND_FTL       *p_nand_ftl;
  FS_NAND           *p_nand;
  FS_BLK_DEV_HANDLE blk_dev_handle;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  //                                                               Assert that NAND blk dev not previously open.
  blk_dev_handle = FSBlkDev_Get(media_handle);
  RTOS_ASSERT_DBG_ERR_SET(FS_BLK_DEV_HANDLE_IS_NULL(blk_dev_handle), *p_err, RTOS_ERR_INVALID_STATE,; );

  FS_MEDIA_WITH_NO_IO(media_handle, p_err) {
    p_nand_ftl = (FS_NAND_FTL *)FS_NAND_FTL_Get(media_handle.MediaPtr);
    //                                                             Assert that a FTL obj not previously allocated.
    RTOS_ASSERT_DBG_ERR_SET(p_nand_ftl == DEF_NULL, *p_err, RTOS_ERR_INVALID_STATE,; );

    p_nand = (FS_NAND *)media_handle.MediaPtr;
    //                                                             Alloc new FTL and cfg it with user params.
    p_nand_ftl = FS_NAND_FTL_Alloc(p_nand, p_nand_cfg, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
    p_nand_ftl->BlkDev.MediaPtr = media_handle.MediaPtr;        // Save associated media info into FTL instance...
                                                                // ...Allows other functions to retrieve FTL instance.

    //                                                             Insert NAND FTL instance in list (see Note #1).
    SList_Push(&FS_NAND_FTL_ListHeadPtr, &p_nand_ftl->ListMember);
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               FS_NAND_Init()
 *
 * @brief    Initialize the driver.
 *
 * @param    p_err   Pointer to variable that will receive the return error code(s) from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_SEG_OVF
 *******************************************************************************************************/
void FS_NAND_FTL_Init(RTOS_ERR *p_err)
{
  PP_UNUSED_PARAM(p_err);

  SList_Init(&FS_NAND_FTL_ListHeadPtr);
}

/****************************************************************************************************//**
 *                                               FS_NAND_LowFmt()
 *
 * @brief    Low-level format a NAND device.
 *
 * @param    media_handle    Handle to a NAND media.
 *
 * @param    p_err           Error pointer.
 *
 * @note     (1) A NAND medium MUST be low-level formatted with this driver prior to access by the
 *               high-level file system, a requirement which the device module enforces.
 *
 * @note     (2) Pushing NAND FTL instance in the internal NAND FTL list allows other functions to
 *               use it and avoid reallocating resources once more time for the same NAND instance.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FS_NAND_FTL_LowFmt(FS_MEDIA_HANDLE media_handle,
                        RTOS_ERR        *p_err)
{
  FS_NAND_FTL       *p_nand_ftl;
  FS_NAND           *p_nand;
  CPU_BOOLEAN       is_open;
  FS_NAND_HANDLE    nand_handle;
  FS_BLK_DEV_HANDLE blk_dev_handle;

  //                                                               Assert that NAND blk dev not previously open.
  blk_dev_handle = FSBlkDev_Get(media_handle);
  RTOS_ASSERT_DBG_ERR_SET(FS_BLK_DEV_HANDLE_IS_NULL(blk_dev_handle), *p_err, RTOS_ERR_INVALID_STATE,; );

  p_nand = (FS_NAND *)media_handle.MediaPtr;
  is_open = p_nand->IsOpen;
  if (!is_open) {
    nand_handle = FS_NAND_Open(media_handle, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
  }

  p_nand_ftl = FS_NAND_FTL_Get(media_handle.MediaPtr);
  if (p_nand_ftl == DEF_NULL) {
    p_nand_ftl = FS_NAND_FTL_Alloc(p_nand, DEF_NULL, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
    p_nand_ftl->BlkDev.MediaPtr = media_handle.MediaPtr;        // Save associated media info into FTL instance...
                                                                // ...Allows other functions to retrieve FTL instance.
  }

  if (p_nand_ftl->IsMounted) {
    FS_NAND_FTL_LowUnmountInternal(p_nand_ftl, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
  }

  FS_NAND_FTL_LowFmtInternal(p_nand_ftl, p_nand, p_err);

  if (!is_open) {
    FS_NAND_Close(nand_handle, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
  }
  //                                                               Insert NAND FTL instance in list (see Note #2).
  SList_Push(&FS_NAND_FTL_ListHeadPtr, &p_nand_ftl->ListMember);
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       DRIVER INTERFACE FUNCTIONS
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           FS_NAND_BlkDevOpen()
 *
 * @brief    Open a NAND block device instance.
 *
 * @param    p_media     Pointer to a NAND media instance.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static FS_BLK_DEV *FS_NAND_FTL_Add(FS_MEDIA *p_media,
                                   RTOS_ERR *p_err)
{
  FS_NAND_FTL *p_nand_ftl;
  FS_NAND     *p_nand;

  p_nand = (FS_NAND *)p_media;

  //                                                               Add block device data if needed.
  p_nand_ftl = FS_NAND_FTL_Get(p_media);
  if (p_nand_ftl == DEF_NULL) {
    p_nand_ftl = FS_NAND_FTL_Alloc(p_nand, DEF_NULL, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (DEF_NULL);
    }
  }

  return (&p_nand_ftl->BlkDev);
}

/****************************************************************************************************//**
 *                                               FS_NAND_Close()
 *
 * @brief    Close a NAND block device instance.
 *
 * @param    p_blk_dev   Pointer to a block device.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_NAND_FTL_Rem(FS_BLK_DEV *p_blk_dev,
                            RTOS_ERR   *p_err)
{
  FS_NAND_FTL *p_nand_ftl;

  PP_UNUSED_PARAM(p_err);

  p_nand_ftl = (FS_NAND_FTL *)p_blk_dev;
  //                                                               Push back NAND FTL instance to be reused at nxt open.
  SList_Push(&FS_NAND_FTL_ListHeadPtr, &p_nand_ftl->ListMember);
}

/****************************************************************************************************//**
 *                                           FS_NAND_FTL_Open()
 *
 * @brief    Open a NAND FTL instance.
 *
 * @param    p_blk_dev   Pointer to a block device.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_NAND_FTL_Open(FS_BLK_DEV *p_blk_dev,
                             RTOS_ERR   *p_err)
{
  FS_NAND_FTL *p_nand_ftl;
  FS_NAND     *p_nand;

  p_nand_ftl = (FS_NAND_FTL *)p_blk_dev;
  p_nand = (FS_NAND *)p_blk_dev->MediaPtr;

  //                                                               Open controller.
  p_nand->CtrlrPtr->CtrlrApiPtr->Open(p_nand->CtrlrPtr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               If block device is mounted, nothing to do.
  if (p_nand_ftl->IsMounted) {
    return;
  }

  FS_NAND_FTL_LowMountInternal(p_nand_ftl, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
    FS_NAND_BLK_QTY last_blk_ix;

    last_blk_ix = p_nand_ftl->BlkIxFirst + p_nand_ftl->BlkCnt - 1u;

    if ((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_BLK_DEV_CORRUPTED)
        && (p_nand_ftl->MetaBlkIxPhy == last_blk_ix)) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

      FS_NAND_BlkEraseInternal(p_nand, last_blk_ix, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return;
      }

      LOG_DBG(("Unable to mount with metadata blk ", (u)last_blk_ix, "."));
      LOG_DBG(("Discarding metadata blk %d and remounting ", (u)last_blk_ix, "."));

      FS_NAND_FTL_LowUnmountInternal(p_nand_ftl, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return;
      }

      FS_NAND_FTL_LowMountInternal(p_nand_ftl, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return;
      }
    } else
#endif
    {
      return;
    }
  }
}

/****************************************************************************************************//**
 *                                           FS_NAND_FTL_Close()
 *
 * @brief    Close a NAND block device instance.
 *
 * @param    p_blk_dev   Pointer to a block device.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_NAND_FTL_Close(FS_BLK_DEV *p_blk_dev,
                              RTOS_ERR   *p_err)
{
  FS_NAND *p_nand;

  PP_UNUSED_PARAM(p_err);

  p_nand = (FS_NAND *)p_blk_dev->MediaPtr;
  p_nand->CtrlrPtr->CtrlrApiPtr->Close(p_nand->CtrlrPtr);
}

/****************************************************************************************************//**
 *                                               FS_NAND_FTL_Rd()
 *
 * @brief    Read from a device & store data in buffer.
 *
 * @param    p_blk_dev   Pointer to block device to read from.
 *
 * @param    p_dest      Pointer to destination buffer.
 *
 * @param    sec_start   Start sector of read.
 *
 * @param    sec_cnt     Number of sectors to read.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_NAND_FTL_Rd(FS_BLK_DEV *p_blk_dev,
                           void       *p_dest,
                           FS_LB_NBR  sec_start,
                           FS_LB_QTY  sec_cnt,
                           RTOS_ERR   *p_err)
{
  FS_NAND_FTL *p_nand_ftl;
  CPU_SIZE_T  rd_cnt_iter;
  CPU_SIZE_T  rd_cnt_total;
  FS_LB_QTY   sec_ix_logical;

  p_nand_ftl = (FS_NAND_FTL *)p_blk_dev;

  sec_ix_logical = sec_start;
  rd_cnt_total = 0u;

  while (rd_cnt_total < sec_cnt) {
    //                                                             Rd 1 or more sec.
    rd_cnt_iter = FS_NAND_FTL_SecRd(p_nand_ftl,
                                    p_dest,
                                    sec_ix_logical,
                                    sec_cnt,
                                    p_err);

    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      LOG_ERR(("Error reading ", (u)sec_cnt, " sectors from index ", (u)sec_start, "."));
      return;
    }

    rd_cnt_total += rd_cnt_iter;
    sec_ix_logical += rd_cnt_iter;

    //                                                             Update dest data ptr.
    p_dest = (void *)((CPU_INT08U *)p_dest + (p_nand_ftl->SecSize * rd_cnt_iter));
  }

  //                                                               ----- COMMIT METADATA (IN CASE OF REFRESH) -----
#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
#if (FS_NAND_CFG_AUTO_SYNC_EN == DEF_ENABLED)
  do {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    FS_NAND_FTL_MetaCommit(p_nand_ftl,
                           DEF_NO,
                           p_err);
  } while ((RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE)
           && (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_BLK_DEV_CORRUPTED));

  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Error committing metadata."));

    return;
  }
#endif
#endif
}

/****************************************************************************************************//**
 *                                               FS_NAND_FTL_Wr()
 *
 * @brief    Write data to a device from a buffer.
 *
 * @param    p_blk_dev   Pointer to block device to write to.
 *
 * @param    p_src       Pointer to source buffer.
 *
 * @param    sec_start   Start sector of write.
 *
 * @param    sec_cnt     Number of sectors to write.
 *
 * @param    p_err       Error pointer.
 *
 * @note     (1) Tracking whether a device is open is not necessary, because this should ONLY be
 *               called when a device is open.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NAND_FTL_Wr(FS_BLK_DEV *p_blk_dev,
                           void       *p_src,
                           FS_LB_NBR  sec_start,
                           FS_LB_QTY  sec_cnt,
                           RTOS_ERR   *p_err)
{
  FS_NAND_FTL *p_nand_ftl;
  FS_LB_QTY   sec_wr_cnt_total;
  FS_LB_QTY   sec_wr_cnt_iter;
  FS_LB_QTY   sec_ix_logical;

  LOG_VRB(("start=", (u)sec_cnt, ", cnt=", (u)sec_cnt, "."));

  p_nand_ftl = (FS_NAND_FTL *)p_blk_dev;

  sec_ix_logical = sec_start;
  sec_wr_cnt_total = 0u;

  while ((sec_wr_cnt_total < sec_cnt)
         && (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE)  ) {
    //                                                             Wr 1 or more sec.
    sec_wr_cnt_iter = FS_NAND_FTL_SecWr(p_nand_ftl,
                                        p_src,
                                        sec_ix_logical,
                                        sec_cnt - sec_wr_cnt_total,
                                        p_err);

    sec_wr_cnt_total += sec_wr_cnt_iter;
    sec_ix_logical += sec_wr_cnt_iter;

    //                                                             Update src data ptr.
    p_src = (void *)((CPU_INT08U *)p_src + (p_nand_ftl->SecSize * sec_wr_cnt_iter));
  }

  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Error writing sectors on device."));
    return;
  }

#if (FS_NAND_CFG_AUTO_SYNC_EN == DEF_ENABLED)
  //                                                               ----------------- COMMIT METADATA ------------------
  do {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    FS_NAND_FTL_MetaCommit(p_nand_ftl,
                           DEF_NO,
                           p_err);
  } while ((RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE)
           && (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_BLK_DEV_CORRUPTED));

  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Error committing metadata."));
    return;
  }
#endif
}
#endif

/****************************************************************************************************//**
 *                                           FS_NAND_FTL_Sync()
 *
 * @brief    Sync NAND device.
 *
 * @param    p_blk_dev   Pointer to a NAND block device instance.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NAND_FTL_Sync(FS_BLK_DEV *p_blk_dev,
                             RTOS_ERR   *p_err)
{
  FS_NAND_FTL *p_nand_ftl;

  p_nand_ftl = (FS_NAND_FTL *)p_blk_dev;
  FS_NAND_FTL_MetaCommit(p_nand_ftl,
                         DEF_NO,
                         p_err);
}
#endif

/****************************************************************************************************//**
 *                                           FS_NAND_FTL_Trim()
 *
 * @brief    Trim NAND device.
 *
 * @param    p_blk_dev   Pointer to a NAND block device instance.
 *
 * @param    lb_nbr      Logical block number.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NAND_FTL_Trim(FS_BLK_DEV *p_blk_dev,
                             FS_LB_NBR  lb_nbr,
                             RTOS_ERR   *p_err)
{
  PP_UNUSED_PARAM(p_blk_dev);
  PP_UNUSED_PARAM(lb_nbr);
  PP_UNUSED_PARAM(p_err);
}
#endif

/****************************************************************************************************//**
 *                                           FS_NAND_FTL_Query()
 *
 * @brief    Get information about a device.
 *
 * @param    p_blk_dev   Pointer to block device to query.
 *
 * @param    p_info      Pointer to structure that will receive device information.
 *
 * @param    p_err       Error pointer.
 *
 * @note     (1) Tracking whether a device is open is not necessary, because this should ONLY be
 *               called when a device is open.
 *******************************************************************************************************/
static void FS_NAND_FTL_Query(FS_BLK_DEV      *p_blk_dev,
                              FS_BLK_DEV_INFO *p_info,
                              RTOS_ERR        *p_err)
{
  FS_NAND_FTL *p_nand_ftl;

  PP_UNUSED_PARAM(p_err);

  p_nand_ftl = (FS_NAND_FTL *)p_blk_dev;

  p_info->LbSizeLog2 = FSUtil_Log2(p_nand_ftl->SecSize);
  p_info->LbCnt = p_nand_ftl->SecCnt;
  p_info->Fixed = DEF_YES;
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           FS_NAND_FTL_HdrWr()
 *
 * @brief    Creates the NAND device header in specified block.
 *
 * @param    p_nand_ftl  Pointer to NAND FTL.
 *
 * @param    blk_ix_hdr  Block index of new header block.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NAND_FTL_HdrWr(FS_NAND_FTL     *p_nand_ftl,
                              FS_NAND_BLK_QTY blk_ix_hdr,
                              RTOS_ERR        *p_err)
{
  FS_NAND                             *p_nand;
  FS_NAND_CTRLR                       *p_ctrlr;
  FS_NAND_PART_PARAM                  *p_part_info;
  CPU_INT08U                          *p_hdr_data;
  CPU_INT08U                          *p_oos_data;
  FS_NAND_HDR_MARK_1_TYPE             mark1;
  FS_NAND_HDR_MARK_2_TYPE             mark2;
  FS_NAND_HDR_VER_TYPE                ver;
  FS_NAND_HDR_SEC_SIZE_TYPE           sec_size_hdr;
  FS_NAND_HDR_BLK_CNT_TYPE            blk_cnt_hdr;
  FS_NAND_HDR_BLK_NBR_FIRST_TYPE      blk_nbr_first_hdr;
  FS_NAND_HDR_UB_CNT_TYPE             ub_cnt_hdr;
  FS_NAND_HDR_MAX_ASSOC_TYPE          max_assoc_hdr;
  FS_NAND_HDR_AVAIL_BLK_TBL_SIZE_TYPE avail_blk_tbl_size_hdr;
  FS_NAND_HDR_OOS_SIZE_TYPE           oos_size_hdr;
  FS_NAND_HDR_MAX_BAD_BLK_CNT_TYPE    max_bad_blk_cnt_hdr;
  FS_NAND_SEC_TYPE_STO                sec_type;
  FS_NAND_ERASE_QTY                   erase_cnt;
  FS_LB_QTY                           sec_ix_phy;

  LOG_VRB(("Creating device header at block ", (u)blk_ix_hdr, "."));

  p_nand = (FS_NAND *)p_nand_ftl->BlkDev.MediaPtr;
  p_ctrlr = p_nand->CtrlrPtr;
  p_part_info = p_ctrlr->CtrlrApiPtr->PartInfoGet(p_ctrlr);
  p_hdr_data = (CPU_INT08U *)p_nand_ftl->BufPtr;
  p_oos_data = (CPU_INT08U *)p_nand_ftl->OOS_BufPtr;

  //                                                               ----------------- GATHER HDR DATA ------------------
  //                                                               Hdr marks.
  mark1 = FS_NAND_HDR_MARK_WORD1;
  MEM_VAL_COPY_SET_INTU_LITTLE(&p_hdr_data[FS_NAND_HDR_MARK_1_OFFSET],
                               &mark1,
                               sizeof(FS_NAND_HDR_MARK_1_TYPE));

  mark2 = FS_NAND_HDR_MARK_WORD2;
  MEM_VAL_COPY_SET_INTU_LITTLE(&p_hdr_data[FS_NAND_HDR_MARK_2_OFFSET],
                               &mark2,
                               sizeof(FS_NAND_HDR_MARK_2_TYPE));

  //                                                               Insert FS_NAND_HDR_VER.
  ver = FS_NAND_HDR_VER;
  MEM_VAL_COPY_SET_INTU_LITTLE(&p_hdr_data[FS_NAND_HDR_VER_OFFSET],
                               &ver,
                               sizeof(FS_NAND_HDR_VER_TYPE));

  //                                                               Insert FS_NAND_HDR_SEC_SIZE.
  sec_size_hdr = p_nand_ftl->SecSize;
  MEM_VAL_COPY_SET_INTU_LITTLE(&p_hdr_data[FS_NAND_HDR_SEC_SIZE_OFFSET],
                               &sec_size_hdr,
                               sizeof(FS_NAND_HDR_SEC_SIZE_TYPE));

  //                                                               Insert FS_NAND_HDR_BLK_CNT.
  blk_cnt_hdr = p_nand_ftl->BlkCnt;
  MEM_VAL_COPY_SET_INTU_LITTLE(&p_hdr_data[FS_NAND_HDR_BLK_CNT_OFFSET],
                               &blk_cnt_hdr,
                               sizeof(FS_NAND_HDR_BLK_CNT_TYPE));

  //                                                               Insert FS_NAND_HDR_BLK_NBR_FIRST.
  blk_nbr_first_hdr = p_nand_ftl->BlkIxFirst;
  MEM_VAL_COPY_SET_INTU_LITTLE(&p_hdr_data[FS_NAND_HDR_BLK_NBR_FIRST_OFFSET],
                               &blk_nbr_first_hdr,
                               sizeof(FS_NAND_HDR_BLK_NBR_FIRST_TYPE));

  //                                                               Insert FS_NAND_HDR_UB_CNT.
  ub_cnt_hdr = p_nand_ftl->UB_CntMax;
  MEM_VAL_COPY_SET_INTU_LITTLE(&p_hdr_data[FS_NAND_HDR_UB_CNT_OFFSET],
                               &ub_cnt_hdr,
                               sizeof(FS_NAND_HDR_UB_CNT_TYPE));

  //                                                               Insert FS_NAND_HDR_MAX_ASSOC.
  max_assoc_hdr = p_nand_ftl->RUB_MaxAssoc;
  MEM_VAL_COPY_SET_INTU_LITTLE(&p_hdr_data[FS_NAND_HDR_MAX_ASSOC_OFFSET],
                               &max_assoc_hdr,
                               sizeof(FS_NAND_HDR_MAX_ASSOC_TYPE));

  //                                                               Insert FS_NAND_HDR_AVAIL_BLK_TBL_SIZE.
  avail_blk_tbl_size_hdr = p_nand_ftl->AvailBlkTblEntryCntMax;
  MEM_VAL_COPY_SET_INTU_LITTLE(&p_hdr_data[FS_NAND_HDR_AVAIL_BLK_TBL_SIZE_OFFSET],
                               &avail_blk_tbl_size_hdr,
                               sizeof(FS_NAND_HDR_AVAIL_BLK_TBL_SIZE_TYPE));

  //                                                               Insert FS_NAND_HDR_OOS_SIZE.
  oos_size_hdr = FS_NAND_OOS_PARTIAL_SIZE_REQD;
  MEM_VAL_COPY_SET_INTU_LITTLE(&p_hdr_data[FS_NAND_HDR_OOS_SIZE_OFFSET],
                               &oos_size_hdr,
                               sizeof(FS_NAND_HDR_OOS_SIZE_TYPE));

  //                                                               Insert FS_NAND_HDR_MAX_BAD_BLK_CNT.
  max_bad_blk_cnt_hdr = p_part_info->MaxBadBlkCnt;
  MEM_VAL_COPY_SET_INTU_LITTLE(&p_hdr_data[FS_NAND_HDR_MAX_BAD_BLK_CNT_OFFSET],
                               &max_bad_blk_cnt_hdr,
                               sizeof(FS_NAND_HDR_MAX_BAD_BLK_CNT_TYPE));

  //                                                               ------------------ CALC OOS DATA -------------------
  Mem_Set(&p_oos_data[FS_NAND_OOS_SEC_USED_OFFSET],             // Insert sec used mark.
          0x00u,
          p_nand_ftl->UsedMarkSize);

  //                                                               Insert sec type.
  sec_type = FS_NAND_SEC_TYPE_HDR;
  MEM_VAL_COPY_SET_INTU_LITTLE(&p_oos_data[FS_NAND_OOS_SEC_TYPE_OFFSET],
                               &sec_type,
                               sizeof(FS_NAND_SEC_TYPE_STO));

  //                                                               Insert erase cnt.
  //                                                               Can't get true erase cnt; dev not low-lvl format'd see FS_NAND_LowFmtHandler() note #5.
  erase_cnt = 1u;

  MEM_VAL_COPY_SET_INTU_LITTLE(&p_oos_data[FS_NAND_OOS_ERASE_CNT_OFFSET],
                               &erase_cnt,
                               sizeof(FS_NAND_ERASE_QTY));

  //                                                               --------------- WR DATA TO FIRST SEC ---------------
  sec_ix_phy = FS_NAND_BLK_IX_TO_SEC_IX(p_nand_ftl, blk_ix_hdr);
  p_ctrlr->CtrlrApiPtr->SecWr(p_ctrlr,
                              p_hdr_data,
                              p_oos_data,
                              sec_ix_phy,
                              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Unable to write header block."));
    return;
  }
}
#endif

/****************************************************************************************************//**
 *                                           FS_NAND_FTL_HdrRd()
 *
 * @brief    Finds and reads low-level device header.
 *
 * @param    p_nand_ftl  Pointer to NAND FTL.
 *
 * @param    p_err       Error pointer.
 *
 * @return   Block index of header          , if an header was found,
 *           Block index of first good block, otherwise.
 *
 * @note     (1) Device header is always located at first good block in specified block range.
 *
 * @note     (2) The search for the device header should not stop if an ECC error is returned by the
 *               SecRd() function. Since the search might need to look in bad blocks, no action should
 *               be taken when an error occurs: the error code is overwritten because of this reason.
 *******************************************************************************************************/
static FS_NAND_BLK_QTY FS_NAND_FTL_HdrRd(FS_NAND_FTL *p_nand_ftl,
                                         RTOS_ERR    *p_err)
{
  FS_NAND_BLK_QTY         blk_ix_phy;
  FS_NAND_BLK_QTY         last_blk_ix;
  CPU_BOOLEAN             hdr_found;
  FS_NAND_SEC_TYPE        blk_type;
  CPU_INT08U              *p_hdr_data;
  FS_NAND_HDR_MARK_1_TYPE mark1;
  FS_NAND_HDR_MARK_2_TYPE mark2;

  last_blk_ix = p_nand_ftl->BlkIxFirst + p_nand_ftl->BlkCnt - 1u;
  blk_ix_phy = p_nand_ftl->BlkIxFirst;

  //                                                               ------------------- FIND HDR BLK -------------------
  hdr_found = DEF_NO;
  while ((blk_ix_phy <= last_blk_ix)
         && (hdr_found == DEF_NO)        ) {
    FS_NAND_FTL_SecRdPhyNoRefresh(p_nand_ftl,                   // Rd first blk sec.
                                  p_nand_ftl->BufPtr,
                                  p_nand_ftl->OOS_BufPtr,
                                  blk_ix_phy,
                                  0u,
                                  p_err);
    switch (RTOS_ERR_CODE_GET(*p_err)) {                        // Chk type only if sec rd'able.
      case RTOS_ERR_ECC_UNCORR:
        blk_ix_phy++;
        break;

      case RTOS_ERR_NONE:
        blk_type = FS_NAND_FTL_SecTypeParse((CPU_INT08U *)(p_nand_ftl->OOS_BufPtr));

        if (blk_type == FS_NAND_SEC_TYPE_HDR) {
          hdr_found = DEF_YES;
        } else {
          blk_ix_phy++;
        }
        break;

      default:
        return (FS_NAND_BLK_IX_INVALID);                        // Prevent 'break NOT reachable' compiler warning.
    }
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);                        // Overwrite err code to continue srch (see Note #2).
  }

  if (hdr_found == DEF_NO) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_BLK_DEV_FMT_INVALID);
    return (FS_NAND_BLK_IX_INVALID);
  }

  //                                                               -------------------- PARSE HDR ---------------------
  p_hdr_data = (CPU_INT08U *)p_nand_ftl->BufPtr;
  MEM_VAL_COPY_GET_INTU_LITTLE(&mark1,
                               &p_hdr_data[FS_NAND_HDR_MARK_1_OFFSET],
                               sizeof(FS_NAND_HDR_MARK_1_TYPE));
  MEM_VAL_COPY_GET_INTU_LITTLE(&mark2,
                               &p_hdr_data[FS_NAND_HDR_MARK_2_OFFSET],
                               sizeof(FS_NAND_HDR_MARK_2_TYPE));

  if ((mark1 == FS_NAND_HDR_MARK_WORD1)                         // If hdr mark corresponds ...
      || (mark2 == FS_NAND_HDR_MARK_WORD2)   ) {
    //                                                             ... validate low lvl params against hdr.
    FS_NAND_FTL_HdrParamsValidate(p_nand_ftl, p_hdr_data, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (blk_ix_phy);
    }
  } else {                                                      // ... else require low lvl fmt.
    RTOS_ERR_SET(*p_err, RTOS_ERR_BLK_DEV_FMT_INVALID);
    return (blk_ix_phy);
  }

  //                                                               Hdr was successfully rd.
  return (blk_ix_phy);
}

/****************************************************************************************************//**
 *                                       FS_NAND_FTL_HdrParamsValidate()
 *
 * @brief    Validate low-level parameters against those stored in device header.
 *
 * @param    p_nand_ftl  Pointer to NAND FTL.
 *
 * @param    p_hdr_data  Pointer to header data.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_NAND_FTL_HdrParamsValidate(FS_NAND_FTL *p_nand_ftl,
                                          CPU_INT08U  *p_hdr_data,
                                          RTOS_ERR    *p_err)
{
  FS_NAND                             *p_nand;
  FS_NAND_PART_PARAM                  *p_part_info;
  FS_NAND_HDR_VER_TYPE                ver;
  FS_NAND_HDR_SEC_SIZE_TYPE           sec_size;
  FS_NAND_HDR_BLK_CNT_TYPE            blk_cnt;
  FS_NAND_HDR_BLK_NBR_FIRST_TYPE      blk_nbr_first;
  FS_NAND_HDR_UB_CNT_TYPE             ub_cnt;
  FS_NAND_HDR_MAX_ASSOC_TYPE          max_assoc;
  FS_NAND_HDR_AVAIL_BLK_TBL_SIZE_TYPE avail_blk_tbl_size;
  FS_NAND_HDR_OOS_SIZE_TYPE           oos_size;
  FS_NAND_HDR_MAX_BAD_BLK_CNT_TYPE    max_bad_blk_cnt;

  ver = 0u;
  sec_size = 0u;
  blk_cnt = 0u;
  blk_nbr_first = 0u;
  ub_cnt = 0u;
  max_assoc = 0u;
  avail_blk_tbl_size = 0u;
  oos_size = 0u;
  max_bad_blk_cnt = 0u;

  p_nand = (FS_NAND *)p_nand_ftl->BlkDev.MediaPtr;

  p_part_info = p_nand->CtrlrPtr->CtrlrApiPtr->PartInfoGet(p_nand->CtrlrPtr);

  //                                                               Validate FS_NAND_HDR_VER.
  MEM_VAL_COPY_GET_INTU_LITTLE(&ver,
                               &p_hdr_data[FS_NAND_HDR_VER_OFFSET],
                               sizeof(FS_NAND_HDR_VER_TYPE));
  if (ver != FS_NAND_HDR_VER) {
    LOG_ERR(("Version doesn't match: device=", (x)ver, "software=", (x)FS_NAND_HDR_VER, "."));
    RTOS_ERR_SET(*p_err, RTOS_ERR_BLK_DEV_FMT_INCOMPATIBLE);
    return;
  }

  //                                                               Validate FS_NAND_HDR_SEC_SIZE.
  MEM_VAL_COPY_GET_INTU_LITTLE(&sec_size,
                               &p_hdr_data[FS_NAND_HDR_SEC_SIZE_OFFSET],
                               sizeof(FS_NAND_HDR_SEC_SIZE_TYPE));
  if (sec_size != p_nand_ftl->SecSize) {
    LOG_ERR(("Sec size doesn't match low lvl hdr: actual=", (u)p_nand_ftl->SecSize, ", hdr=", (u)sec_size));
    RTOS_ERR_SET(*p_err, RTOS_ERR_BLK_DEV_FMT_INCOMPATIBLE);
    return;
  }

  //                                                               Validate FS_NAND_HDR_BLK_CNT.
  MEM_VAL_COPY_GET_INTU_LITTLE(&blk_cnt,
                               &p_hdr_data[FS_NAND_HDR_BLK_CNT_OFFSET],
                               sizeof(FS_NAND_HDR_BLK_CNT_TYPE));
  if (blk_cnt != p_nand_ftl->BlkCnt) {
    LOG_ERR(("Blk cnt doesn't match low lvl hdr: actual=", (u)p_nand_ftl->BlkCnt, ", hdr=", (u)blk_cnt, "."));
    RTOS_ERR_SET(*p_err, RTOS_ERR_BLK_DEV_FMT_INCOMPATIBLE);
    return;
  }

  //                                                               Validate FS_NAND_HDR_BLK_NBR_FIRST.
  MEM_VAL_COPY_GET_INTU_LITTLE(&blk_nbr_first,
                               &p_hdr_data[FS_NAND_HDR_BLK_NBR_FIRST_OFFSET],
                               sizeof(FS_NAND_HDR_BLK_NBR_FIRST_TYPE));
  if (blk_nbr_first != p_nand_ftl->BlkIxFirst) {
    LOG_ERR(("First blk nbr doesn't match low lvl hdr: actual=", (u)p_nand_ftl->BlkIxFirst, ", hdr=", (u)blk_nbr_first));
    RTOS_ERR_SET(*p_err, RTOS_ERR_BLK_DEV_FMT_INCOMPATIBLE);
    return;
  }

  //                                                               Validate FS_NAND_HDR_UB_CNT.
  MEM_VAL_COPY_GET_INTU_LITTLE(&ub_cnt,
                               &p_hdr_data[FS_NAND_HDR_UB_CNT_OFFSET],
                               sizeof(FS_NAND_HDR_UB_CNT_TYPE));
  if (ub_cnt != p_nand_ftl->UB_CntMax) {
    LOG_ERR(("Update blk cnt doesn't match low lvl hdr: actual=", (u)p_nand_ftl->UB_CntMax, ", hdr=", (u)ub_cnt));
    RTOS_ERR_SET(*p_err, RTOS_ERR_BLK_DEV_FMT_INCOMPATIBLE);
    return;
  }

  //                                                               Validate FS_NAND_HDR_MAX_ASSOC.
  MEM_VAL_COPY_GET_INTU_LITTLE(&max_assoc,
                               &p_hdr_data[FS_NAND_HDR_MAX_ASSOC_OFFSET],
                               sizeof(FS_NAND_HDR_MAX_ASSOC_TYPE));
  if (max_assoc != p_nand_ftl->RUB_MaxAssoc) {
    LOG_ERR(("Max associativity doesn't match low lvl hdr: actual=", (u)p_nand_ftl->RUB_MaxAssoc, ", hdr=", (u)max_assoc));
    RTOS_ERR_SET(*p_err, RTOS_ERR_BLK_DEV_FMT_INCOMPATIBLE);
    return;
  }

  //                                                               Validate FS_NAND_HDR_AVAIL_BLK_TBL_SIZE.
  MEM_VAL_COPY_GET_INTU_LITTLE(&avail_blk_tbl_size,
                               &p_hdr_data[FS_NAND_HDR_AVAIL_BLK_TBL_SIZE_OFFSET],
                               sizeof(FS_NAND_HDR_AVAIL_BLK_TBL_SIZE_TYPE));
  if (avail_blk_tbl_size != p_nand_ftl->AvailBlkTblEntryCntMax) {
    LOG_ERR(("Avail blk tbl size doesn't match low lvl hdr: actual=", (u)p_nand_ftl->AvailBlkTblEntryCntMax,
             ", hdr=", (u)avail_blk_tbl_size));
    RTOS_ERR_SET(*p_err, RTOS_ERR_BLK_DEV_FMT_INCOMPATIBLE);
  }

  //                                                               Validate FS_NAND_HDR_OOS_SIZE.
  MEM_VAL_COPY_GET_INTU_LITTLE(&oos_size,
                               &p_hdr_data[FS_NAND_HDR_OOS_SIZE_OFFSET],
                               sizeof(FS_NAND_HDR_OOS_SIZE_TYPE));
  if (oos_size != FS_NAND_OOS_PARTIAL_SIZE_REQD) {
    LOG_ERR(("Out of sector data size doesn't match low lvl hdr: actual=", (u)FS_NAND_OOS_PARTIAL_SIZE_REQD,
             ", hdr=", (u)oos_size));
    RTOS_ERR_SET(*p_err, RTOS_ERR_BLK_DEV_FMT_INCOMPATIBLE);
    return;
  }

  //                                                               Validate FS_NAND_HDR_MAX_BAD_BLK_CNT.
  MEM_VAL_COPY_GET_INTU_LITTLE(&max_bad_blk_cnt,
                               &p_hdr_data[FS_NAND_HDR_MAX_BAD_BLK_CNT_OFFSET],
                               sizeof(FS_NAND_HDR_MAX_BAD_BLK_CNT_TYPE));
  if (max_bad_blk_cnt != p_part_info->MaxBadBlkCnt) {
    LOG_ERR(("Max bad block count doesn't match low level header: actual=", (u)p_part_info->MaxBadBlkCnt,
             ", hdr=", (u)max_bad_blk_cnt));
    RTOS_ERR_SET(*p_err, RTOS_ERR_BLK_DEV_FMT_INCOMPATIBLE);
    return;
  }
}

/****************************************************************************************************//**
 *                                           FS_NAND_FTL_BlkIxPhyGet()
 *
 * @brief    Get physical index associated with specified logical block index.
 *
 * @param    p_nand_ftl      Pointer to NAND FTL.
 *
 * @param    blk_ix_logical  Logical block index.
 *
 * @return   Physical block index,   if logical index specified is valid,
 *           FS_NAND_BLK_IX_INVALID, otherwise.
 *
 * @note     (1) Logical block mapping is described in 'fs_dev_nand.c Note #4'.
 *******************************************************************************************************/
static FS_NAND_BLK_QTY FS_NAND_FTL_BlkIxPhyGet(FS_NAND_FTL     *p_nand_ftl,
                                               FS_NAND_BLK_QTY blk_ix_logical)
{
  FS_NAND_BLK_QTY blk_ix_phy;
  FS_NAND_UB_DATA ub_tbl_entry;

  if (blk_ix_logical < p_nand_ftl->LogicalDataBlkCnt) {
    //                                                             Data blks (see Note #1).
    blk_ix_phy = p_nand_ftl->LogicalToPhyBlkMap[blk_ix_logical];
  } else if (blk_ix_logical < (p_nand_ftl->LogicalDataBlkCnt + p_nand_ftl->UB_CntMax)) {
    //                                                             UBs (see Note #1).
    ub_tbl_entry = FS_NAND_FTL_UB_TblEntryRd(p_nand_ftl, blk_ix_logical - p_nand_ftl->LogicalDataBlkCnt);
    blk_ix_phy = ub_tbl_entry.BlkIxPhy;
  } else if (blk_ix_logical == p_nand_ftl->LogicalDataBlkCnt + p_nand_ftl->UB_CntMax) {
    //                                                             Active meta blk (see Note #1).
    blk_ix_phy = p_nand_ftl->MetaBlkIxPhy;
  } else {
    return (FS_NAND_BLK_IX_INVALID);
  }

  return (blk_ix_phy);
}

/****************************************************************************************************//**
 *                                           FS_NAND_FTL_BlkRefresh()
 *
 * @brief    Copy (backup) all sectors from the target block (blk_ix_phy) to a valid block from the
 *           available block table to cope with degraded cell condition.
 *
 * @param    p_nand_ftl  Pointer to NAND FTL.
 *
 * @param    blk_ix_phy  Block's physical index.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NAND_FTL_BlkRefresh(FS_NAND_FTL     *p_nand_ftl,
                                   FS_NAND_BLK_QTY blk_ix_phy,
                                   RTOS_ERR        *p_err)
{
  FS_NAND                 *p_nand;
  FS_NAND_CTRLR           *p_ctrlr;
  CPU_INT08U              *p_oos_buf;
  FS_LB_NBR               blk_sec_ix_phy;
  FS_NAND_SEC_PER_BLK_QTY sec_offset_phy;
  FS_LB_NBR               new_sec_ix_phy;
  FS_NAND_BLK_QTY         new_blk_ix_phy = 0u;
  FS_NAND_UB_DATA         ub_tbl_entry;
  FS_NAND_ERASE_QTY       erase_cnt;
  CPU_BOOLEAN             uncorrectable_err_occured = DEF_NO;
  CPU_BOOLEAN             done;
  CPU_BOOLEAN             fail;
  CPU_BOOLEAN             found;
  CPU_BOOLEAN             sec_used;
  CPU_INT08U              retry_cnt;
  CPU_SIZE_T              i;

  LOG_VRB(("Refreshing phy blk ", (u)blk_ix_phy));

  p_nand = (FS_NAND *)p_nand_ftl->BlkDev.MediaPtr;
  p_ctrlr = p_nand->CtrlrPtr;
  p_oos_buf = (CPU_INT08U *)p_nand_ftl->OOS_BufPtr;

  done = DEF_NO;
  while (!done) {
    //                                                             ------------------ GET ERASED BLK ------------------
    new_blk_ix_phy = FS_NAND_FTL_BlkGetErased(p_nand_ftl, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      LOG_ERR(("Unable to get an erased block."));
      return;
    }

    //                                                             ------------------- COPY SECTORS -------------------
    sec_offset_phy = 0u;
    blk_sec_ix_phy = FS_NAND_BLK_IX_TO_SEC_IX(p_nand_ftl,       // Calc old sec ix.
                                              blk_ix_phy);
    new_sec_ix_phy = FS_NAND_BLK_IX_TO_SEC_IX(p_nand_ftl,       // Calc new sec ix.
                                              new_blk_ix_phy);

    fail = DEF_NO;
    while ((sec_offset_phy < p_nand_ftl->NbrSecPerBlk)          // For all sec, unless some op fails.
           && (fail == DEF_NO)                      ) {
      retry_cnt = 0u;
      sec_used = DEF_YES;
      do {                                                      // Rd sec, retry if neccessary.
        FS_NAND_FTL_SecRdPhyNoRefresh((FS_NAND_FTL *)p_nand_ftl,
                                      p_nand_ftl->BufPtr,
                                      p_oos_buf,
                                      blk_ix_phy,
                                      sec_offset_phy,
                                      p_err);

        if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_ECC_UNCORR) {
          sec_used = FS_NAND_FTL_SecIsUsed(p_nand_ftl,
                                           blk_sec_ix_phy + sec_offset_phy,
                                           p_err);
        }
        retry_cnt++;
      } while ((retry_cnt <= FS_NAND_CFG_MAX_RD_RETRIES)
               && (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE)
               && (sec_used == DEF_YES));

      switch (RTOS_ERR_CODE_GET(*p_err)) {
        case RTOS_ERR_ECC_UNCORR:
          if (sec_used == DEF_YES) {
            uncorrectable_err_occured = DEF_YES;                // Data corruption occurred.
            FS_BLK_DEV_CTR_ERR_INC(p_nand_ftl->Ctrs.ErrRefreshDataLoss);
          }
          RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
          break;

        case RTOS_ERR_NONE:
          break;

        default:
          return;
      }

      if (sec_offset_phy == 0u) {
        erase_cnt = FS_NAND_FTL_BlkRemFromAvail(p_nand_ftl, new_blk_ix_phy);

        MEM_VAL_COPY_SET_INTU_LITTLE(&p_oos_buf[FS_NAND_OOS_ERASE_CNT_OFFSET],
                                     &erase_cnt,
                                     sizeof(FS_NAND_ERASE_QTY));
#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
        if (sec_used != DEF_YES) {
          LOG_ERR(("Fatal error, blk ", (u)blk_ix_phy, " has unwritten sector 0."));
          RTOS_DBG_FAIL_EXEC(RTOS_ERR_BLK_DEV_CORRUPTED,; );
        }
#endif
      }

      if (sec_used == DEF_YES) {
        p_ctrlr->CtrlrApiPtr->SecWr(p_ctrlr,                    // Wr sec in new blk.
                                    p_nand_ftl->BufPtr,
                                    p_oos_buf,
                                    new_sec_ix_phy,
                                    p_err);
        switch (RTOS_ERR_CODE_GET(*p_err)) {
          case RTOS_ERR_IO:                                     // If pgrm op failed ...
            LOG_ERR(("Sector programming error at sec ", (u)new_sec_ix_phy, ", marking block ", (u)new_blk_ix_phy, " as bad."));

            FS_NAND_FTL_BlkMarkBad(p_nand_ftl,                  // ... mark blk as bad ...
                                   new_blk_ix_phy,
                                   p_err);
            fail = DEF_YES;                                     // ... and try with another blk.
            break;

          case RTOS_ERR_NONE:
            break;

          default:
            return;
        }
      }

      sec_offset_phy++;                                         // Inc sec offsets.
      new_sec_ix_phy++;
    }

    if (fail == DEF_NO) {
      done = DEF_YES;
    }
  }

  //                                                               ---------------- UPDATE METADATA -------------------
  found = DEF_NO;
  i = 0u;
  while ((i < p_nand_ftl->LogicalDataBlkCnt)                    // Scan data blk.
         && (found == DEF_NO)                      ) {
    if (p_nand_ftl->LogicalToPhyBlkMap[i] == blk_ix_phy) {      // If blk was a logical data blk, ...
                                                                // ... update logical to phy map.
      p_nand_ftl->LogicalToPhyBlkMap[i] = new_blk_ix_phy;
      found = DEF_YES;
    }

    i++;
  }

  i = 0u;
  while ((found == DEF_NO)
         && (i < p_nand_ftl->UB_CntMax)) {
    ub_tbl_entry = FS_NAND_FTL_UB_TblEntryRd(p_nand_ftl, i);

    if (ub_tbl_entry.BlkIxPhy == blk_ix_phy) {                  // If blk was an UB, ...
                                                                // ... update tbl.
      FS_NAND_FTL_UB_TblEntryWr(p_nand_ftl, i, new_blk_ix_phy);
      found = DEF_YES;
    }

    i++;
  }

  if (found == DEF_NO) {
    if (blk_ix_phy == p_nand_ftl->MetaBlkIxPhy) {               // If blk was the metadata blk ...
      p_nand_ftl->MetaBlkIxPhy = new_blk_ix_phy;                // ... update p_nand_data.
    } else {                                                    // ... else blk is unaccounted for.
      LOG_ERR(("Could not find metadata associated with block ", (u)blk_ix_phy, "."));
      RTOS_ERR_SET(*p_err, RTOS_ERR_BLK_DEV_CORRUPTED);
      return;
    }
  }

  LOG_ERR(("Refresh operation ", (s)uncorrectable_err_occured == DEF_NO ? "succeeded" : "failed",
           ": new phy blk ix is ", (u)new_blk_ix_phy, "."));

  if (uncorrectable_err_occured == DEF_YES) {                   // If data loss occurred, ...
    RTOS_ERR_SET(*p_err, RTOS_ERR_ECC_UNCORR);
  }

  FS_BLK_DEV_CTR_STAT_INC(p_nand_ftl->Ctrs.StatBlkRefreshCtr);

  return;
}
#endif

/****************************************************************************************************//**
 *                                           FS_NAND_FTL_BlkMarkBad()
 *
 * @brief    Add block to bad block table.
 *
 * @param    p_nand_ftl  Pointer to NAND FTL.
 *
 * @param    blk_ix_phy  Block's physical index.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NAND_FTL_BlkMarkBad(FS_NAND_FTL     *p_nand_ftl,
                                   FS_NAND_BLK_QTY blk_ix_phy,
                                   RTOS_ERR        *p_err)
{
  FS_NAND            *p_nand;
  FS_NAND_BLK_QTY    *p_bad_blk_tbl;
  FS_NAND_PART_PARAM *p_part_info;
  FS_NAND_BLK_QTY    tbl_ix;
  FS_NAND_BLK_QTY    tbl_entry;
  CPU_BOOLEAN        found_slot;

  p_bad_blk_tbl = (FS_NAND_BLK_QTY *)&p_nand_ftl->MetaCache[p_nand_ftl->MetaOffsetBadBlkTbl];
  p_nand = (FS_NAND *)p_nand_ftl->BlkDev.MediaPtr;

  p_part_info = p_nand->CtrlrPtr->CtrlrApiPtr->PartInfoGet(p_nand->CtrlrPtr);

  //                                                               ------------ LOOK FOR AVAIL SLOT IN TBL ------------
  tbl_ix = 0u;
  found_slot = DEF_NO;
  while ((tbl_ix < p_part_info->MaxBadBlkCnt)
         && (found_slot == DEF_NO)) {
    MEM_VAL_COPY_GET_INTU_LITTLE(&tbl_entry,
                                 &p_bad_blk_tbl[tbl_ix],
                                 sizeof(FS_NAND_BLK_QTY));

    if (tbl_entry == blk_ix_phy) {
      LOG_DBG(("Block ", (u)blk_ix_phy, " is already in bad blk tbl."));
      return;
    } else if (tbl_entry != FS_NAND_BLK_IX_INVALID) {
      tbl_ix++;
    } else {
      found_slot = DEF_YES;
    }
  }

  if (found_slot != DEF_YES) {                                  // No slot found.
    LOG_ERR(("Unable to add entry; bad block table is full."));
    RTOS_ERR_SET(*p_err, RTOS_ERR_WOULD_OVF);
    return;
  }

  //                                                               Add blk to bad blk tbl.
  MEM_VAL_COPY_SET_INTU_LITTLE(&p_bad_blk_tbl[tbl_ix],
                               &blk_ix_phy,
                               sizeof(FS_NAND_BLK_QTY));

  FS_NAND_FTL_BlkUnmap(p_nand_ftl,                              // Unmap blk.
                       blk_ix_phy);

  FS_NAND_FTL_BadBlkTblInvalidate(p_nand_ftl);
}
#endif

/****************************************************************************************************//**
 *                                       FS_NAND_FTL_BlkMarkDirty()
 *
 * @brief    Add block to dirty block bitmap.
 *
 * @param    p_nand_ftl  Pointer to NAND FTL.
 *
 * @param    blk_ix_phy  Block's physical index.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NAND_FTL_BlkMarkDirty(FS_NAND_FTL     *p_nand_ftl,
                                     FS_NAND_BLK_QTY blk_ix_phy)
{
  CPU_SIZE_T offset_octet;
  CPU_INT08U offset_bit;
  CPU_INT08U *p_dirty_map;

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)                        // ------------------ ARG VALIDATION ------------------
  if ((blk_ix_phy < p_nand_ftl->BlkIxFirst)                     // Validate blk_ix_phy.
      || (blk_ix_phy >= p_nand_ftl->BlkCnt + p_nand_ftl->BlkIxFirst)) {
    RTOS_DBG_FAIL_EXEC(RTOS_ERR_INVALID_ARG,; );
  }
#endif

  LOG_VRB(("Marking blk ", (u)blk_ix_phy, " dirty."));

  //                                                               Calc bit pos.
  blk_ix_phy -= p_nand_ftl->BlkIxFirst;
  offset_octet = blk_ix_phy  / DEF_OCTET_NBR_BITS;
  offset_bit = blk_ix_phy  % DEF_OCTET_NBR_BITS;

  //                                                               Set bit.
  p_dirty_map = &p_nand_ftl->MetaCache[p_nand_ftl->MetaOffsetDirtyBitmap];

  p_dirty_map[offset_octet] |= DEF_BIT(offset_bit);

  FS_NAND_FTL_DirtyMapInvalidate(p_nand_ftl);                   // Invalidate bitmap.
}
#endif

/****************************************************************************************************//**
 *                                           FS_NAND_FTL_BlkUnmap()
 *
 * @brief    Unmaps a block, removing it from
 *               - (1) Available blocks table,
 *               - (2) Update blocks table,
 *               - (3) Dirty blocks bitmap,
 *               - (4) Logical to physical blocks map.
 *
 * @param    p_nand_ftl  Pointer to NAND FTL.
 *
 * @param    blk_ix_phy  Block's physical index.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NAND_FTL_BlkUnmap(FS_NAND_FTL     *p_nand_ftl,
                                 FS_NAND_BLK_QTY blk_ix_phy)
{
  CPU_INT08U              *p_metadata;
  CPU_SIZE_T              i;
  FS_NAND_BLK_QTY         blk_ix;
  CPU_SIZE_T              offset_octet;
  CPU_INT08U              offset_bit;
  FS_NAND_UB_DATA         ub_tbl_entry;
  FS_NAND_AVAIL_BLK_ENTRY avail_blk_tbl_entry;
  CPU_BOOLEAN             srch_done;

  //                                                               ---------------- UNMAP FROM DATA BLK ---------------
  i = 0u;
  srch_done = DEF_NO;
  while ((i < p_nand_ftl->LogicalDataBlkCnt) && (srch_done != DEF_YES)) {
    if (p_nand_ftl->LogicalToPhyBlkMap[i] == blk_ix_phy) {
      p_nand_ftl->LogicalToPhyBlkMap[i] = FS_NAND_BLK_IX_INVALID;
      srch_done = DEF_YES;
    }
    i++;
  }

  //                                                               ----------------- UNMAP FROM UB TBL ----------------
  i = 0u;
  srch_done = DEF_NO;
  p_metadata = &p_nand_ftl->MetaCache[p_nand_ftl->MetaOffsetUB_Tbl];
  while ((i < p_nand_ftl->UB_CntMax) && (srch_done != DEF_YES)) {
    ub_tbl_entry = FS_NAND_FTL_UB_TblEntryRd(p_nand_ftl, i);
    if (ub_tbl_entry.BlkIxPhy == blk_ix_phy) {
      FS_NAND_FTL_UB_TblEntryWr(p_nand_ftl, i, FS_NAND_BLK_IX_INVALID);
      srch_done = DEF_YES;
    }
    i++;
  }
  //                                                               ------------- UNMAP FROM AVAIL BLK TBL -------------
  i = 0u;
  srch_done = DEF_NO;
  while ((i < p_nand_ftl->AvailBlkTblEntryCntMax) && (srch_done != DEF_YES)) {
    avail_blk_tbl_entry = FS_NAND_FTL_AvailBlkTblEntryRd(p_nand_ftl, i);
    if (avail_blk_tbl_entry.BlkIxPhy == blk_ix_phy) {
      avail_blk_tbl_entry.BlkIxPhy = FS_NAND_BLK_IX_INVALID;

      LOG_VRB(("Removing blk ", (u)blk_ix_phy, " from avail blk tbl at ix ", (u)i, "."));

      FS_NAND_FTL_AvailBlkTblEntryWr(p_nand_ftl, i, avail_blk_tbl_entry);
      FS_NAND_FTL_AvailBlkTblInvalidate(p_nand_ftl);
      srch_done = DEF_YES;
    }
    i++;
  }

  //                                                               ----------- UNMAP FROM DIRTY BLK BITMAP ------------
  p_metadata = &p_nand_ftl->MetaCache[p_nand_ftl->MetaOffsetDirtyBitmap];

  blk_ix = blk_ix_phy - p_nand_ftl->BlkIxFirst;
  offset_octet = blk_ix     / DEF_OCTET_NBR_BITS;
  offset_bit = blk_ix     % DEF_OCTET_NBR_BITS;

  p_metadata[offset_octet] &= ~DEF_BIT(offset_bit);
  FS_NAND_FTL_DirtyMapInvalidate(p_nand_ftl);
}
#endif

/****************************************************************************************************//**
 *                                       FS_NAND_FTL_BlkAddToAvail()
 *
 * @brief    Add specified block to available blocks table.
 *
 * @param    p_nand_ftl  Pointer to NAND FTL.
 *
 * @param    blk_ix_phy  Block physical index.
 *
 * @param    p_err       Error pointer.
 *
 * @note     (1) If a block erase count can't be read, the erase count is reset to 0. This case will
 *               mostly not happen. However, if it happens, it is preferable to ignore the error. The
 *               block will still be usable. If the block is bad, following operations on it will
 *               soon detect it and mark it bad.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NAND_FTL_BlkAddToAvail(FS_NAND_FTL     *p_nand_ftl,
                                      FS_NAND_BLK_QTY blk_ix_phy,
                                      RTOS_ERR        *p_err)
{
  CPU_INT08U              *p_oos_buf;
  FS_NAND_AVAIL_BLK_ENTRY entry;
  FS_NAND_BLK_QTY         avail_blk_tbl_ix;
  FS_NAND_BLK_QTY         avail_blk_tbl_ix_free;
  FS_NAND_SEC_TYPE        blk_type;
  CPU_BOOLEAN             is_meta;
  RTOS_ERR                err;

  p_oos_buf = (CPU_INT08U *)p_nand_ftl->OOS_BufPtr;
  RTOS_ERR_SET(err, RTOS_ERR_NONE);

  //                                                               --------------------- SCAN TBL ---------------------
  avail_blk_tbl_ix_free = FS_NAND_BLK_IX_INVALID;
  avail_blk_tbl_ix = 0u;
  while ((avail_blk_tbl_ix_free == FS_NAND_BLK_IX_INVALID)
         && (avail_blk_tbl_ix < p_nand_ftl->AvailBlkTblEntryCntMax)) {
    entry = FS_NAND_FTL_AvailBlkTblEntryRd(p_nand_ftl, avail_blk_tbl_ix);

    if (entry.BlkIxPhy == blk_ix_phy) {                         // Entry already in tbl.
      return;
    } else if (entry.BlkIxPhy == FS_NAND_BLK_IX_INVALID) {
      avail_blk_tbl_ix_free = avail_blk_tbl_ix;                 // Found free entry in avail blk tbl.
    } else {
      avail_blk_tbl_ix++;
    }
  }

  if (avail_blk_tbl_ix_free == FS_NAND_BLK_IX_INVALID) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_ABORT);
    LOG_ERR(("No more space in available block table."));
    return;
  }

  LOG_VRB(("Adding blk ", (u)blk_ix_phy, " to avail blk tbl at ix ", (u)avail_blk_tbl_ix_free, "."));

  //                                                               ------------------ GET ERASE CNT -------------------
  FS_NAND_FTL_SecRdPhyNoRefresh(p_nand_ftl,
                                p_nand_ftl->BufPtr,
                                p_nand_ftl->OOS_BufPtr,
                                blk_ix_phy,
                                0u,
                                &err);
  if (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE) {                // If sec is rd'able ...
                                                                // ... read erase cnt from sec.
    MEM_VAL_COPY_GET_INTU_LITTLE(&entry.EraseCnt,
                                 &p_oos_buf[FS_NAND_OOS_ERASE_CNT_OFFSET],
                                 sizeof(FS_NAND_ERASE_QTY));

    blk_type = FS_NAND_SEC_TYPE_INVALID;
    MEM_VAL_COPY_GET_INTU_LITTLE(&blk_type,
                                 &p_oos_buf[FS_NAND_OOS_SEC_TYPE_OFFSET],
                                 sizeof(FS_NAND_SEC_TYPE_STO));

    is_meta = (blk_type == FS_NAND_SEC_TYPE_METADATA);

    //                                                             Overwrite invalid erase cnt.
    if (entry.EraseCnt == FS_NAND_ERASE_QTY_INVALID) {
      entry.EraseCnt = 0u;
    }
  } else if (RTOS_ERR_CODE_GET(err) == RTOS_ERR_ECC_UNCORR) {   // ... else if uncorrectable ecc err ...
    entry.EraseCnt = 0;                                         // ... assume blk is brand new (see Note #1).
    is_meta = DEF_NO;
  } else {
    RTOS_ERR_SET(*p_err, RTOS_ERR_ABORT);
    LOG_ERR(("Unexpected error reading sector 0 in block ", (u)blk_ix_phy, "."));
    return;
  }

  //                                                               ----------------- ADD ENTRY TO TBL -----------------
  //                                                               Add entry to tbl.
  entry.BlkIxPhy = blk_ix_phy;

  if (is_meta) {                                                // Indicate whether the added block is a metadata block.
    Bitmap_BitSet(p_nand_ftl->AvailBlkMetaMap, avail_blk_tbl_ix_free);
    MEM_VAL_COPY_GET_INTU_LITTLE(&p_nand_ftl->AvailBlkMetaID_Tbl[avail_blk_tbl_ix_free],
                                 &p_oos_buf[FS_NAND_OOS_META_ID_OFFSET],
                                 sizeof(FS_NAND_META_ID));
  } else {
    Bitmap_BitClr(p_nand_ftl->AvailBlkMetaMap, avail_blk_tbl_ix_free);
  }
  //                                                               Add avail tbl entry.
  FS_NAND_FTL_AvailBlkTblEntryWr(p_nand_ftl, avail_blk_tbl_ix_free, entry);

  FS_NAND_FTL_AvailBlkTblInvalidate(p_nand_ftl);                // Invalidate tbl.
}
#endif

/****************************************************************************************************//**
 *                                       FS_NAND_FTL_BlkRemFromAvail()
 *
 * @brief    Remove specified block from available blocks table and return its stored erase count.
 *
 * @param    p_nand_ftl  Pointer to NAND FTL.
 *
 * @param    blk_ix_phy  Block physical index.
 *
 * @return   erase count of specified block, if the block was found in the table;
 *           FS_NAND_ERASE_QTY_INVALID     , otherwise.
 *******************************************************************************************************/
#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static FS_NAND_ERASE_QTY FS_NAND_FTL_BlkRemFromAvail(FS_NAND_FTL     *p_nand_ftl,
                                                     FS_NAND_BLK_QTY blk_ix_phy)
{
  FS_NAND_BLK_QTY         avail_blk_tbl_ix;
  FS_NAND_AVAIL_BLK_ENTRY avail_blk;
  FS_NAND_ERASE_QTY       erase_cnt;

  erase_cnt = FS_NAND_ERASE_QTY_INVALID;
  avail_blk_tbl_ix = 0u;
  //                                                               Find entry in tbl.
  while ((avail_blk_tbl_ix < p_nand_ftl->AvailBlkTblEntryCntMax)
         && (erase_cnt == FS_NAND_ERASE_QTY_INVALID)) {
    avail_blk = FS_NAND_FTL_AvailBlkTblEntryRd(p_nand_ftl, avail_blk_tbl_ix);

    if (avail_blk.BlkIxPhy == blk_ix_phy) {
      //                                                           Get erase cnt.
      erase_cnt = avail_blk.EraseCnt + 1u;

      LOG_VRB(("Removing blk ", (u)blk_ix_phy, " from avail blk tbl at ix ", (u)avail_blk_tbl_ix, "."));

      //                                                           Rem entry from tbl.
      avail_blk.BlkIxPhy = FS_NAND_BLK_IX_INVALID;
      FS_NAND_FTL_AvailBlkTblEntryWr(p_nand_ftl, avail_blk_tbl_ix, avail_blk);
      FS_NAND_FTL_AvailBlkTblInvalidate(p_nand_ftl);
    }

    avail_blk_tbl_ix++;
  }

  if (erase_cnt == FS_NAND_ERASE_QTY_INVALID) {
    LOG_DBG(("Unable to find available blocks table entry."));
  }

  return (erase_cnt);
}
#endif

/****************************************************************************************************//**
 *                                       FS_NAND_FTL_BlkGetAvailFromTbl()
 *
 * @brief    Get an available block from the available block table.
 *
 * @param    p_nand_ftl      Pointer to NAND FTL.
 *
 * @param    access_rsvd     DEF_YES, if access to reserved blocks is allowed,
 *                           DEF_NO , otherwise.
 *
 * @return   Block index of available block, if a block was found,
 *           FS_NAND_BLK_IX_INVALID,         otherwise.
 *
 * @note     (1) An available block might not be erased. Use FS_NAND_BlkEnsureErased() to make sure it's
 *               erased.
 *
 * @note     (2) Some available blocks are reserved for metadata blocks folding. Those entries will only
 *               be returned if access_rsvd has value DEF_YES. Only FS_NAND_MetaBlkFold() should
 *               call this function with access_rsvd set to DEF_YES.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static FS_NAND_BLK_QTY FS_NAND_FTL_BlkGetAvailFromTbl(FS_NAND_FTL *p_nand_ftl,
                                                      CPU_BOOLEAN access_rsvd)
{
  FS_NAND_BLK_QTY         tbl_ix;
  FS_NAND_BLK_QTY         tbl_ix_committed;
  FS_NAND_BLK_QTY         blk_ix_phy;
  FS_NAND_BLK_QTY         blk_ix_phy_not_committed;
  FS_NAND_BLK_QTY         blk_ix_phy_committed;
  FS_NAND_BLK_QTY         nbr_entries;
  FS_NAND_BLK_QTY         nbr_entries_committed;
  FS_NAND_AVAIL_BLK_ENTRY tbl_entry;
  FS_NAND_ERASE_QTY       min_erase_cnt;
  FS_NAND_ERASE_QTY       min_erase_cnt_committed;
  CPU_BOOLEAN             is_entry_committed;
  CPU_BOOLEAN             is_meta;
  CPU_BOOLEAN             is_stale_meta;
  FS_NAND_META_ID         meta_id_delta;

  min_erase_cnt = DEF_GET_U_MAX_VAL(min_erase_cnt);
  min_erase_cnt_committed = DEF_GET_U_MAX_VAL(min_erase_cnt_committed);
  nbr_entries = 0u;
  nbr_entries_committed = 0u;
  blk_ix_phy = FS_NAND_BLK_IX_INVALID;
  blk_ix_phy_not_committed = FS_NAND_BLK_IX_INVALID;
  blk_ix_phy_committed = FS_NAND_BLK_IX_INVALID;
  tbl_ix_committed = FS_NAND_BLK_IX_INVALID;

  //                                                               -------------------- FIND BLKS ---------------------
  for (tbl_ix = 0u; tbl_ix < p_nand_ftl->AvailBlkTblEntryCntMax; tbl_ix++) {
    tbl_entry = FS_NAND_FTL_AvailBlkTblEntryRd(p_nand_ftl, tbl_ix);

    if (tbl_entry.BlkIxPhy != FS_NAND_BLK_IX_INVALID) {
      nbr_entries++;
      //                                                           Chk if entry is committed.
      is_entry_committed = Bitmap_BitIsSet(p_nand_ftl->AvailBlkTblCommitMap, tbl_ix);
      is_meta = Bitmap_BitIsSet(p_nand_ftl->AvailBlkMetaMap, tbl_ix);
      is_stale_meta = DEF_NO;

      if (is_meta) {
        meta_id_delta = p_nand_ftl->MetaBlkID - p_nand_ftl->AvailBlkMetaID_Tbl[tbl_ix];
        is_stale_meta = (meta_id_delta > FS_NAND_META_ID_STALE_THRESH);
      }

      if (is_entry_committed) {
        nbr_entries_committed++;
        //                                                         Find committed avail blk with lowest erase cnt.

        if (is_stale_meta) {
          min_erase_cnt_committed = 0u;
          blk_ix_phy_committed = tbl_entry.BlkIxPhy;
        } else if (tbl_entry.EraseCnt < min_erase_cnt_committed) {
          min_erase_cnt_committed = tbl_entry.EraseCnt;
          blk_ix_phy_committed = tbl_entry.BlkIxPhy;
          tbl_ix_committed = tbl_ix;
        }
      } else {
        //                                                         Find avail blk with lowest erase cnt.

        if (is_stale_meta) {
          min_erase_cnt = 0u;
          blk_ix_phy_not_committed = tbl_entry.BlkIxPhy;
        } else if (tbl_entry.EraseCnt < min_erase_cnt) {
          min_erase_cnt = tbl_entry.EraseCnt;
          blk_ix_phy_not_committed = tbl_entry.BlkIxPhy;
        }
      }
    }
  }

  //                                                               --------------- DETERMINE BLK TO USE ---------------
  if (access_rsvd == DEF_YES) {
    blk_ix_phy = blk_ix_phy_committed;
    //                                                             Prevent entry from being used again.
    Bitmap_BitClr(p_nand_ftl->AvailBlkTblCommitMap, tbl_ix_committed);
  } else if (nbr_entries_committed > FS_NAND_CFG_RSVD_AVAIL_BLK_CNT) {
    if (min_erase_cnt_committed < min_erase_cnt) {
      blk_ix_phy = blk_ix_phy_committed;
      //                                                           Prevent entry from being used again.
      Bitmap_BitClr(p_nand_ftl->AvailBlkTblCommitMap, tbl_ix_committed);
    } else {
      blk_ix_phy = blk_ix_phy_not_committed;
    }
  } else {
    blk_ix_phy = blk_ix_phy_not_committed;
    LOG_DBG(("Warning -- unable to get a committed available block table entry -- using an uncommitted entry."));
  }

  if (blk_ix_phy == FS_NAND_BLK_IX_INVALID) {                   // If no blk was found ...
    return (FS_NAND_BLK_IX_INVALID);                            // ... return with err.
  } else {
    return (blk_ix_phy);                                        // Found accessible blk.
  }
}
#endif

/****************************************************************************************************//**
 *                                           FS_NAND_FTL_BlkGetDirty()
 *
 * @brief    Get a dirty block from dirty blocks bitmap.
 *
 * @param    p_nand_ftl              Pointer to NAND FTL.
 *
 * @param    pending_dirty_chk_en    Enable/disable pending check of dirty blocks. (see Note #1a).
 *
 * @param    p_err                   Error pointer.
 *
 * @return   Physical block index of dirty block, if a block was found,
 *           FS_NAND_BLK_IX_INVALID,              otherwise.
 *
 * @note     (1) Care must be taken to ensure that the choosen dirty block is not a "pending dirty"
 *               block, i.e. that the block has been marked dirty but the change hasn't yet been
 *               committed to the metadata block. Taking such a block would result in potential data
 *               loss on unexpected power loss.
 *               - (a) The argument 'pending_dirty_chk_en' should be always be set to DEF_ENABLED to
 *                     enable the proper check of "pending dirty blocks". However, when low-level
 *                     formatting, the argument must be set to DEF_DISABLED, because all blocks
 *                     are dirty but none have been committed to the device yet.
 *
 * @note     (2) Dirty block selection is based on round-robin like method. This method could be changed
 *               to select the dirty block with minimum erase count, but it would affect performance.
 *
 * @note     (3) If the metadata is corrupted and cannot be read from the device, we must ensure that
 *               it is committed (rewritten on device) before the next shutdown or power loss occur.
 *               For this behavior to happen, we must complete the operation successfully by clearing
 *               the error code.
 *******************************************************************************************************/
#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static FS_NAND_BLK_QTY FS_NAND_FTL_BlkGetDirty(FS_NAND_FTL *p_nand_ftl,
                                               CPU_BOOLEAN pending_dirty_chk_en,
                                               RTOS_ERR    *p_err)
{
  CPU_INT08U      *p_dirty_map;
  FS_NAND_BLK_QTY blk_ix_phy;
  CPU_SIZE_T      word_ix;
  CPU_SIZE_T      word_ix_wrap;
  CPU_SIZE_T      word_ix_start;
  CPU_INT32U      word;
  CPU_INT32U      mask;
  CPU_INT08U      word_offset;
  CPU_INT08U      bit_pos;
  CPU_INT08U      mask_end_pos;
  CPU_BOOLEAN     dirty_found;
  CPU_BOOLEAN     meta_found;
  CPU_SIZE_T      meta_offset;
  CPU_INT08U      meta_byte = 0u;
#if (FS_NAND_CFG_DIRTY_MAP_CACHE_EN != DEF_ENABLED)
  FS_NAND_META_SEC_QTY    meta_sec_ix;
  CPU_INT08U              *p_buf;
  FS_NAND_SEC_PER_BLK_QTY sec_offset_phy;
#endif

  p_dirty_map = &p_nand_ftl->MetaCache[p_nand_ftl->MetaOffsetDirtyBitmap];

  //                                                               ------------- SCAN TBL FOR A DIRTY BLK -------------
  dirty_found = DEF_NO;
  meta_found = DEF_NO;
  word_ix_start = p_nand_ftl->DirtyBitmapSrchPos >> 5u;
  word_ix_wrap = (p_nand_ftl->BlkCnt - 1u)      >> 5u;
  word_ix = word_ix_start;
  word_offset = p_nand_ftl->DirtyBitmapSrchPos &  DEF_BIT_FIELD(5u, 0u);
  do {
    bit_pos = word_offset;

    //                                                             Get a 32 bits word from bitmap.
    MEM_VAL_COPY_GET_INT32U_LITTLE(&word, &p_dirty_map[word_ix * 4u]);

    if (word_ix == word_ix_wrap) {                              // If last word  ...
                                                                // ... adjust mask to cover only what remains.
      mask_end_pos = p_nand_ftl->BlkCnt & DEF_BIT_FIELD(5u, 0u);
      if (mask_end_pos == 0u) {
        mask_end_pos = 32u;
      }
    } else {
      mask_end_pos = 32u;                                       // Don't care if the end pos goes beyond srch start ix.
    }

    //                                                             Calc and apply mask.
    mask = DEF_BIT_FIELD((CPU_INT08U)(mask_end_pos - word_offset), word_offset);
    word &= mask;

    while ((word != 0u)                                         // If not null, there is a dirty blk in range.
           && (dirty_found == DEF_NO)) {
      bit_pos = CPU_CntTrailZeros32(word);                      // Find least significant bit set in word ...
                                                                // ... See CPU_CntTrailZeros32() Note #2.

      if (pending_dirty_chk_en == DEF_DISABLED) {               // If pending dirty chk is disabled (see Note #1a) ...
        dirty_found = DEF_YES;
      } else {
        //                                                         ------- CHK THAT BLK IS NOT "PENDING" DIRTY --------
        //                                                         Chk if dirty status was committed.
        //                                                         Calc offset in metadata info.
#if (FS_NAND_CFG_DIRTY_MAP_CACHE_EN != DEF_ENABLED)
        meta_offset = p_nand_ftl->MetaOffsetDirtyBitmap;
#else
        meta_offset = 0u;
#endif
        meta_offset += word_ix * FS_NAND_MAP_SRCH_SIZE_OCTETS;
        meta_offset += bit_pos >> DEF_OCTET_TO_BIT_SHIFT;
        meta_offset %= p_nand_ftl->SecSize;                     // Calc octet offset in metadata sec.

#if (FS_NAND_CFG_DIRTY_MAP_CACHE_EN != DEF_ENABLED)
        //                                                         Calc sec offset in metadata info.
        meta_sec_ix = meta_offset / p_nand_ftl->SecSize;
        //                                                         Find metadata sec.
        sec_offset_phy = FS_NAND_FTL_MetaSecFind(p_nand_ftl,
                                                 meta_sec_ix,
                                                 p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          return (FS_NAND_BLK_IX_INVALID);
        } else if (sec_offset_phy == FS_NAND_SEC_OFFSET_IX_INVALID) {
          LOG_ERR(("FS_NAND_BlkGetDirty(): Unable to find metadata sector ", (u)meta_sec_ix,
                   ". Data loss may occur if a power loss occurs during brief period following this warning.\r\n"));
          RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
        }

        p_buf = (CPU_INT08U *)p_nand_ftl->BufPtr;

        FS_NAND_FTL_SecRdPhyNoRefresh(p_nand_ftl,               // Rd sec in meta blk.
                                      p_buf,
                                      p_nand_ftl->OOS_BufPtr,
                                      p_nand_ftl->MetaBlkIxPhy,
                                      sec_offset_phy,
                                      p_err);
        if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_ECC_UNCORR) {
          //                                                       Meta is unreadable (see note #3).
          LOG_ERR(("FS_NAND_BlkGetDirty(): Unable to find metadata sector ", (u)meta_sec_ix,
                   ". Filesystem could be corrupted if a power loss occurs during the metadata is committed.\r\n"));
          RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
          dirty_found = DEF_YES;
        } else if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          return (FS_NAND_BLK_IX_INVALID);
        } else {
          meta_byte = p_buf[meta_offset];                       // Rd dirty bitmap byte from dev.
          meta_found = DEF_YES;
        }
#else //                                                           Rd dirty bitmap byte from cache.
        meta_byte = p_nand_ftl->DirtyBitmapCache[meta_offset];
        meta_found = DEF_YES;
#endif

        if (meta_found == DEF_YES) {
          //                                                       If bit is set ...
          if (DEF_BIT_IS_SET(meta_byte, DEF_BIT(bit_pos & DEF_OCTET_TO_BIT_MASK))) {
            dirty_found = DEF_YES;                              // ... we have found a real dirty blk.
          } else {
            word &= ~DEF_BIT(bit_pos);                          // ... else, clr bit at tested pos.
          }
        }
      }
    }

    if (dirty_found == DEF_NO) {
      word_ix = (word_ix == word_ix_wrap) ? 0u : word_ix + 1u;
      word_offset = 0u;
    }
  } while ((word_ix != word_ix_start)                           // Loop until whole bitmap has been srch'd or blk found.
           && (dirty_found == DEF_NO)          );

  p_nand_ftl->DirtyBitmapSrchPos = (word_ix << 5u) + bit_pos + 1u;
  if (p_nand_ftl->DirtyBitmapSrchPos == p_nand_ftl->BlkCnt) {
    p_nand_ftl->DirtyBitmapSrchPos = 0u;
  }

  if (dirty_found == DEF_NO) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_BLK_DEV_CORRUPTED);
    return (FS_NAND_BLK_IX_INVALID);
  }
  //                                                               Convert pos in dirty blk bitmap to blk phy ix.
  blk_ix_phy = (word_ix * FS_NAND_MAP_SRCH_SIZE_BITS) + bit_pos;
  blk_ix_phy += p_nand_ftl->BlkIxFirst;

  if (blk_ix_phy >= p_nand_ftl->BlkIxFirst + p_nand_ftl->BlkCnt) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_BLK_DEV_CORRUPTED);
    return (FS_NAND_BLK_IX_INVALID);
  }

  return (blk_ix_phy);
}
#endif

/****************************************************************************************************//**
 *                                       FS_NAND_FTL_BlkGetErased()
 *
 * @brief    Get a block from available block table. Erase it if not already erased.
 *
 * @param    p_nand_ftl  Pointer to NAND FTL.
 *
 * @param    p_err       Error pointer.
 *
 * @return   Block index in available blocks table, if a block was found;
 *           FS_NAND_BLK_IX_INVALID,                otherwise.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static FS_NAND_BLK_QTY FS_NAND_FTL_BlkGetErased(FS_NAND_FTL *p_nand_ftl,
                                                RTOS_ERR    *p_err)
{
  FS_NAND_BLK_QTY blk_ix_phy = 0u;
  FS_NAND_BLK_QTY avail_blk_cnt;
  CPU_BOOLEAN     meta_committed;
  CPU_BOOLEAN     done;

  meta_committed = DEF_NO;
  done = DEF_NO;
  do {                                                          // Until blk can be erased successfully.
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    //                                                             Fill avail blk tbl.
    avail_blk_cnt = FS_NAND_FTL_AvailBlkTblFill(p_nand_ftl,
                                                FS_NAND_CFG_RSVD_AVAIL_BLK_CNT + 1u,
                                                DEF_ENABLED,
                                                p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      LOG_ERR(("Error filling available block table."));
      return (FS_NAND_BLK_IX_INVALID);
    }

    if (avail_blk_cnt != FS_NAND_CFG_RSVD_AVAIL_BLK_CNT + 1u) {
      if (meta_committed == DEF_NO) {
        //                                                         Commit metadata: switch 'pending' dirty to dirty.
        FS_NAND_FTL_MetaCommit(p_nand_ftl, DEF_NO, p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          LOG_ERR(("Fatal error committing metadata."));
          return (FS_NAND_BLK_IX_INVALID);
        }
        meta_committed = DEF_YES;
      } else {                                                  // Fails since a meta commit already has been tried.
        LOG_DBG(("Unable to fill available block table; device is full."));
        return (FS_NAND_BLK_IX_INVALID);
      }
    }

    //                                                             Commit avail blk tbl on dev if invalid.
    //                                                             This might cause the new avail blk to be used.
    if (p_nand_ftl->AvailBlkTblInvalidated == DEF_YES) {
      FS_NAND_FTL_AvailBlkTblTmpCommit(p_nand_ftl, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        LOG_ERR(("Error committing available block table to device."));
        return (FS_NAND_BLK_IX_INVALID);
      }
    }

    //                                                             Chk if we still have enough avail blks.
    avail_blk_cnt = FS_NAND_FTL_AvailBlkTblEntryCnt(p_nand_ftl);
    if (avail_blk_cnt >= FS_NAND_CFG_RSVD_AVAIL_BLK_CNT + 1u) {
      //                                                           Get a new avail blk.
      blk_ix_phy = FS_NAND_FTL_BlkGetAvailFromTbl(p_nand_ftl, DEF_NO);
      if (blk_ix_phy == FS_NAND_BLK_IX_INVALID) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_BLK_DEV_CORRUPTED);
        return (FS_NAND_BLK_IX_INVALID);
      }

      //                                                           Make sure blk is erased.
      FS_NAND_FTL_BlkEnsureErased(p_nand_ftl, blk_ix_phy, p_err);
      switch (RTOS_ERR_CODE_GET(*p_err)) {
        case RTOS_ERR_NONE:
          done = DEF_YES;
          break;

        case RTOS_ERR_IO:
          break;                                                // Go through another loop iter.

        default:
          LOG_ERR(("Error ensuring blk ", (u)blk_ix_phy, " is erased."));
          return (FS_NAND_BLK_IX_INVALID);                      // Prevent 'break NOT reachable' compiler warning.
      }
    }
  } while ((done != DEF_YES));

  return (blk_ix_phy);
}
#endif

/****************************************************************************************************//**
 *                                       FS_NAND_FTL_BlkEnsureErased()
 *
 * @brief    Make sure a block from the available block table is really erased. Erase it if it isn't.
 *
 * @param    p_nand_ftl  Pointer to NAND FTL.
 *
 * @param    blk_ix_phy  Physical block index.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NAND_FTL_BlkEnsureErased(FS_NAND_FTL     *p_nand_ftl,
                                        FS_NAND_BLK_QTY blk_ix_phy,
                                        RTOS_ERR        *p_err)
{
  FS_NAND     *p_nand;
  CPU_BOOLEAN is_used;
  FS_LB_NBR   sec_ix_phy;

  p_nand = (FS_NAND *)p_nand_ftl->BlkDev.MediaPtr;

  sec_ix_phy = FS_NAND_BLK_IX_TO_SEC_IX(p_nand_ftl, blk_ix_phy);

  is_used = FS_NAND_FTL_SecIsUsed(p_nand_ftl,                   // Chk if used mark is present.
                                  sec_ix_phy,
                                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("No need to erase block ", (u)blk_ix_phy, " (err)."));
    return;
  }

  if (is_used == DEF_NO) {                                      // If the sec is unused ...
    LOG_VRB(("FS_NAND_BlkEnsureErased(): No need to erase block ", (u)blk_ix_phy, " (not used)."));
    return;                                                     // ... nothing to do ...
  } else {                                                      // ... else, erase sec.
    FS_NAND_BlkEraseInternal(p_nand,
                             blk_ix_phy,
                             p_err);
    switch (RTOS_ERR_CODE_GET(*p_err)) {
      case RTOS_ERR_IO:
        LOG_ERR(("Error erasing block ", (u)blk_ix_phy, ". Marking it bad."));

        RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
        FS_NAND_FTL_BlkMarkBad(p_nand_ftl, blk_ix_phy, p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          LOG_ERR(("Unable to mark block bad."));
        }

        RTOS_ERR_SET(*p_err, RTOS_ERR_IO);                      // Return original err code.
        return;

      case RTOS_ERR_NONE:
        return;

      default:
        break;
    }
  }
}
#endif

/****************************************************************************************************//**
 *                                           FS_NAND_FTL_BlkIsBad()
 *
 * @brief    Determine if block is bad according to bad block table.
 *
 * @param    p_nand_ftl  Pointer to NAND FTL.
 *
 * @param    blk_ix_phy  Index of block to check.
 *
 * @return   DEF_YES, if block is bad,
 *           DEF_NO , otherwise.
 *
 * @note     (1) Only checks bad block table; factory defect blocks not having been marked as bad will
 *               not cause returned value to be DEF_YES.
 *******************************************************************************************************/
static CPU_BOOLEAN FS_NAND_FTL_BlkIsBad(FS_NAND_FTL     *p_nand_ftl,
                                        FS_NAND_BLK_QTY blk_ix_phy)
{
  FS_NAND            *p_nand;
  FS_NAND_PART_PARAM *p_part_info;
  FS_NAND_BLK_QTY    *p_bad_blk_tbl;
  FS_NAND_BLK_QTY    ix;
  FS_NAND_BLK_QTY    blk_ix_phy_entry;

  p_nand = (FS_NAND *)p_nand_ftl->BlkDev.MediaPtr;

  p_bad_blk_tbl = (FS_NAND_BLK_QTY *)&p_nand_ftl->MetaCache[p_nand_ftl->MetaOffsetBadBlkTbl];

  p_part_info = p_nand->CtrlrPtr->CtrlrApiPtr->PartInfoGet(p_nand->CtrlrPtr);

  for (ix = 0u; ix < p_part_info->MaxBadBlkCnt; ix++) {
    MEM_VAL_COPY_GET_INTU_LITTLE(&blk_ix_phy_entry,             // Get entry's blk ix.
                                 &p_bad_blk_tbl[ix],
                                 sizeof(FS_NAND_BLK_QTY));
    if (blk_ix_phy_entry == blk_ix_phy) {
      return (DEF_YES);
    }
  }

  return (DEF_NO);
}

/****************************************************************************************************//**
 *                                           FS_NAND_FTL_BlkIsDirty()
 *
 * @brief    Determine if block is dirty according to dirty bitmap.
 *
 * @param    p_nand_ftl  Pointer to NAND FTL.
 *
 * @param    blk_ix_phy  Index of block to check.
 *
 * @return   DEF_YES, if block is dirty,
 *           DEF_NO , otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN FS_NAND_FTL_BlkIsDirty(FS_NAND_FTL     *p_nand_ftl,
                                          FS_NAND_BLK_QTY blk_ix_phy)
{
  CPU_SIZE_T  offset_octet;
  CPU_INT08U  offset_bit;
  CPU_INT08U  *p_dirty_map;
  CPU_BOOLEAN bit_set;

  p_dirty_map = &p_nand_ftl->MetaCache[p_nand_ftl->MetaOffsetDirtyBitmap];

  blk_ix_phy -= p_nand_ftl->BlkIxFirst;                         // Remove offset of first blk.

  offset_octet = blk_ix_phy / DEF_OCTET_NBR_BITS;
  offset_bit = blk_ix_phy % DEF_OCTET_NBR_BITS;

  //                                                               Get bit's value.
  bit_set = DEF_BIT_IS_SET(p_dirty_map[offset_octet], DEF_BIT(offset_bit));

  return (bit_set);
}

/****************************************************************************************************//**
 *                                           FS_NAND_FTL_BlkIsAvail()
 *
 * @brief    Determine if designated block is in available blocks table.
 *
 * @param    p_nand_ftl  Pointer to NAND FTL.
 *
 * @param    blk_ix_phy  Index of block to check.
 *
 * @return   DEF_YES, if block is in available blocks table,
 *           DEF_NO , otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN FS_NAND_FTL_BlkIsAvail(FS_NAND_FTL     *p_nand_ftl,
                                          FS_NAND_BLK_QTY blk_ix_phy)
{
  FS_NAND_AVAIL_BLK_ENTRY entry;
  FS_NAND_BLK_QTY         ix;

  for (ix = 0u; ix < p_nand_ftl->AvailBlkTblEntryCntMax; ix++) {
    entry = FS_NAND_FTL_AvailBlkTblEntryRd(p_nand_ftl, ix);
    if (entry.BlkIxPhy == blk_ix_phy) {
      return (DEF_YES);
    }
  }

  return (DEF_NO);
}

/****************************************************************************************************//**
 *                                           FS_NAND_FTL_BlkIsUB()
 *
 * @brief    Determine if designated block is an update block, according to the update block table.
 *
 * @param    p_nand_ftl  Pointer to NAND FTL.
 *
 * @param    blk_ix_phy  Index of block to check.
 *
 * @return   DEF_YES, if block is an update block,
 *           DEF_NO , otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN FS_NAND_FTL_BlkIsUB(FS_NAND_FTL     *p_nand_ftl,
                                       FS_NAND_BLK_QTY blk_ix_phy)
{
  FS_NAND_UB_DATA entry;
  FS_NAND_UB_QTY  ix;

  for (ix = 0u; ix < p_nand_ftl->UB_CntMax; ix++) {
    entry = FS_NAND_FTL_UB_TblEntryRd(p_nand_ftl, ix);          // Rd update blk tbl entry.
    if (entry.BlkIxPhy == blk_ix_phy) {
      return (DEF_YES);
    }
  }

  return (DEF_NO);
}

/****************************************************************************************************//**
 *                                       FS_NAND_FTL_BlkIsFactoryDefect()
 *
 * @brief    Check if block is a factory defect block.
 *
 * @param    p_nand_ftl  Pointer to NAND FTL.
 *
 * @param    blk_ix_phy  Index of physical block.
 *
 * @param    p_err       Error pointer.
 *
 * @return   DEF_YES, if block is a factory defect block,
 *           DEF_NO,  otherwise.
 *
 * @note     (1) ONFI spec states that factory defect are marked as 0x00 or 0x0000(16-bit access). Bit
 *               errors in defect mark are not tolerated, even if ECC-correctable.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static CPU_BOOLEAN FS_NAND_FTL_BlkIsFactoryDefect(FS_NAND_FTL     *p_nand_ftl,
                                                  FS_NAND_BLK_QTY blk_ix_phy,
                                                  RTOS_ERR        *p_err)
{
  FS_NAND            *p_nand;
  FS_NAND_PART_PARAM *p_part_info;
  FS_NAND_CTRLR      *p_ctrlr;
  CPU_INT08U         *p_buf;
  FS_LB_QTY          pg_sec_ix_phy;
  CPU_INT08U         pg_chk_iter;
  CPU_SIZE_T         i;

  p_nand = (FS_NAND *)p_nand_ftl->BlkDev.MediaPtr;
  p_ctrlr = p_nand->CtrlrPtr;
  p_part_info = p_ctrlr->CtrlrApiPtr->PartInfoGet(p_ctrlr);

  p_buf = (CPU_INT08U *)p_nand_ftl->BufPtr;                     // Sec buf used since spare size > OOS size.

  for (pg_chk_iter = 0u; pg_chk_iter <= 1u; pg_chk_iter++) {
    if (pg_chk_iter == 0u) {
      switch (p_part_info->DefectMarkType) {
        case DEFECT_SPARE_ANY_PG_1_OR_N_ALL_0:
        case DEFECT_SPARE_B_1_6_W_1_IN_PG_1:
        case DEFECT_SPARE_B_6_W_1_PG_1_OR_2:
        case DEFECT_SPARE_L_1_PG_1_OR_2:
        case DEFECT_SPARE_L_1_PG_1_OR_N_ALL_0:
          pg_sec_ix_phy = blk_ix_phy * p_part_info->PgPerBlk;
          break;

        case DEFECT_PG_L_1_OR_N_PG_1_OR_2:
          pg_sec_ix_phy = blk_ix_phy * p_nand_ftl->NbrSecPerBlk;
          break;

        default:
          RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_CFG);
          return (DEF_YES);                                     // Prevent 'break NOT reachable' compiler warning.
      }
    } else {
      switch (p_part_info->DefectMarkType) {
        case DEFECT_SPARE_ANY_PG_1_OR_N_ALL_0:
        case DEFECT_SPARE_L_1_PG_1_OR_N_ALL_0:
          pg_sec_ix_phy = (blk_ix_phy + 1) * p_part_info->PgPerBlk - 1u;
          break;

        case DEFECT_SPARE_B_6_W_1_PG_1_OR_2:
        case DEFECT_SPARE_L_1_PG_1_OR_2:
          pg_sec_ix_phy = blk_ix_phy * p_part_info->PgPerBlk + 1u;
          break;

        case DEFECT_PG_L_1_OR_N_PG_1_OR_2:
          //                                                       Only loc 1 checked.
          pg_sec_ix_phy = blk_ix_phy * p_nand_ftl->NbrSecPerBlk + 1u;
          break;

        case DEFECT_SPARE_B_1_6_W_1_IN_PG_1:
          return (DEF_NO);                                      // Prevent 'break NOT reachable' compiler warning.

        default:
          RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_CFG);
          return (DEF_YES);                                     // Prevent 'break NOT reachable' compiler warning.
      }
    }

    switch (p_part_info->DefectMarkType) {
      case DEFECT_SPARE_L_1_PG_1_OR_N_ALL_0:
      case DEFECT_SPARE_ANY_PG_1_OR_N_ALL_0:
      case DEFECT_SPARE_B_6_W_1_PG_1_OR_2:
      case DEFECT_SPARE_L_1_PG_1_OR_2:
      case DEFECT_SPARE_B_1_6_W_1_IN_PG_1:
        p_ctrlr->CtrlrApiPtr->SpareRdRaw(p_ctrlr,
                                         p_buf,
                                         pg_sec_ix_phy,
                                         0u,
                                         p_part_info->SpareSize,
                                         p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          return (DEF_YES);
        }
        break;

      case DEFECT_PG_L_1_OR_N_PG_1_OR_2:
        p_ctrlr->CtrlrApiPtr->SecRd(p_ctrlr,
                                    p_buf,
                                    p_nand_ftl->OOS_BufPtr,
                                    pg_sec_ix_phy,
                                    p_err);
        if ((RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE)
            && (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_ECC_CORR)
            && (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_ECC_CRITICAL_CORR)
            && (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_ECC_UNCORR)) {
          return (DEF_YES);
        }
        RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
        break;

      default:
        RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_CFG);
        return (DEF_YES);
    }

    switch (p_part_info->DefectMarkType) {
      case DEFECT_SPARE_L_1_PG_1_OR_2:
      case DEFECT_PG_L_1_OR_N_PG_1_OR_2:
        if ( (p_buf[0] != 0xFFu)
             || ((p_buf[1] != 0xFFu) && (p_part_info->BusWidth == 16u)) ) {
          return (DEF_YES);
        }
        break;

      case DEFECT_SPARE_L_1_PG_1_OR_N_ALL_0:
        if ( (p_buf[0] == 0x00u)
             && ((p_buf[1] == 0x00u) || (p_part_info->BusWidth == 8u)) ) {
          return (DEF_YES);
        }
        break;

      case DEFECT_SPARE_ANY_PG_1_OR_N_ALL_0:
        for (i = 0; i < p_part_info->SpareSize; i++) {
          if (p_buf[i] == 0x00u) {
            return (DEF_YES);
          }
        }
        break;

      case DEFECT_SPARE_B_1_6_W_1_IN_PG_1:
        if (p_part_info->BusWidth == 16u) {
          if ((p_buf[0] != 0xFFu) || (p_buf[1] != 0xFFu)) {
            return (DEF_YES);
          }
        } else {
          if ((p_buf[0] != 0xFFu) || (p_buf[5] != 0xFFu)) {
            return (DEF_YES);
          }
        }
        break;

      case DEFECT_SPARE_B_6_W_1_PG_1_OR_2:
        if (p_part_info->BusWidth == 16u) {
          if ((p_buf[0] != 0xFFu) || (p_buf[1] != 0xFFu)) {
            return (DEF_YES);
          }
        } else {
          if (p_buf[5] != 0xFFu) {
            return (DEF_YES);
          }
        }
        break;

      default:
        RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_CFG);
        return (DEF_YES);                                       // Prevent 'break NOT reachable' compiler warning.
    }
  }

  return (DEF_NO);
}
#endif

/****************************************************************************************************//**
 *                                           FS_NAND_FTL_MetaBlkFind()
 *
 * @brief    Find the latest metadata block on NAND device.
 *
 * @param    p_nand_ftl  Pointer to NAND FTL.
 *
 * @param    blk_ix_hdr  Index of device header block.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_NAND_FTL_MetaBlkFind(FS_NAND_FTL     *p_nand_ftl,
                                    FS_NAND_BLK_QTY blk_ix_hdr,
                                    RTOS_ERR        *p_err)
{
  CPU_INT08U           *p_oos_buf;
  FS_NAND_BLK_QTY      blk_ix_phy;
  FS_NAND_BLK_QTY      last_blk_ix;
  FS_NAND_SEC_TYPE_STO blk_type;
  FS_NAND_META_ID      min_meta_blk_id;
  FS_NAND_META_ID      max_meta_blk_id;
  FS_NAND_META_ID      max_meta_blk_id_lwr;
  FS_NAND_META_ID      min_meta_blk_id_higher;
  FS_NAND_META_ID      meta_blk_id_range;
  FS_NAND_META_ID      seq_nbr;
  FS_NAND_BLK_QTY      max_seq_blk_ix;
  FS_NAND_BLK_QTY      min_seq_blk_ix;
  FS_NAND_BLK_QTY      max_seq_blk_ix_lwr;
  FS_NAND_BLK_QTY      min_seq_blk_ix_higher;

  p_oos_buf = (CPU_INT08U *)p_nand_ftl->OOS_BufPtr;

  //                                                               ------------------ FIND META BLK -------------------
  min_meta_blk_id = DEF_GET_U_MAX_VAL(min_meta_blk_id);
  max_meta_blk_id = 0u;
  max_meta_blk_id_lwr = 0u;
  min_meta_blk_id_higher = DEF_GET_U_MAX_VAL(min_meta_blk_id_higher);
  max_seq_blk_ix = FS_NAND_BLK_IX_INVALID;
  min_seq_blk_ix = FS_NAND_BLK_IX_INVALID;
  max_seq_blk_ix_lwr = FS_NAND_BLK_IX_INVALID;
  min_seq_blk_ix_higher = FS_NAND_BLK_IX_INVALID;

  last_blk_ix = p_nand_ftl->BlkIxFirst + p_nand_ftl->BlkCnt - 1u;
  for (blk_ix_phy = blk_ix_hdr; blk_ix_phy <= last_blk_ix; blk_ix_phy++) {
    blk_type = FS_NAND_SEC_TYPE_UNUSED;

    FS_NAND_FTL_SecRdPhyNoRefresh(p_nand_ftl,                   // Rd sec to get blk type.
                                  p_nand_ftl->BufPtr,
                                  p_nand_ftl->OOS_BufPtr,
                                  blk_ix_phy,
                                  0u,
                                  p_err);
    switch (RTOS_ERR_CODE_GET(*p_err)) {
      case RTOS_ERR_ECC_UNCORR:
        LOG_DBG(("Uncorr ECC error while reading first page of physical blk ", (u)blk_ix_phy));
        RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
        break;

      case RTOS_ERR_NONE:
        RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
        blk_type = FS_NAND_SEC_TYPE_INVALID;                    // Rd blk type.
        MEM_VAL_COPY_GET_INTU_LITTLE(&blk_type,
                                     &p_oos_buf[FS_NAND_OOS_SEC_TYPE_OFFSET],
                                     sizeof(FS_NAND_SEC_TYPE_STO));
        break;

      default:
        return;                                                 // Return err.
    }

    if (blk_type == FS_NAND_SEC_TYPE_METADATA) {
      //                                                           Determine if meta blk is complete.
      MEM_VAL_COPY_GET_INTU_LITTLE(&seq_nbr,                    // Rd blk ID.
                                   &p_oos_buf[FS_NAND_OOS_META_ID_OFFSET],
                                   sizeof(FS_NAND_META_ID));

#if LOG_VRB_IS_EN()
      {
        FS_NAND_ERASE_QTY erase_cnt;

        MEM_VAL_COPY_GET_INTU_LITTLE(&erase_cnt,
                                     &p_oos_buf[FS_NAND_OOS_ERASE_CNT_OFFSET],
                                     sizeof(FS_NAND_ERASE_QTY));

        LOG_VRB(("Found meta block at physical block ", (u)blk_ix_phy, " with ID ", (u)seq_nbr, " and erase count ", (u)erase_cnt, "."));
      }
#endif

      //                                                           Update min and max meta blk seq nbr.
      if (seq_nbr <= min_meta_blk_id) {
        min_meta_blk_id = seq_nbr;
        min_seq_blk_ix = blk_ix_phy;
      }
      if (seq_nbr >= max_meta_blk_id) {
        max_meta_blk_id = seq_nbr;
        max_seq_blk_ix = blk_ix_phy;
      }

      if ((seq_nbr >= max_meta_blk_id_lwr)                      // Lower half seq range.
          && (seq_nbr < FS_NAND_META_SEQ_QTY_HALF_RANGE)) {
        max_meta_blk_id_lwr = seq_nbr;
        max_seq_blk_ix_lwr = blk_ix_phy;
      }

      //                                                           Higher half seq range.
      if ((seq_nbr > FS_NAND_META_SEQ_QTY_HALF_RANGE)
          && (seq_nbr <= min_meta_blk_id_higher)) {
        min_meta_blk_id_higher = seq_nbr;
        min_seq_blk_ix_higher = blk_ix_phy;
      }
    }
  }

  //                                                               ----------------- FIND LATEST SEQ ------------------
  meta_blk_id_range = max_meta_blk_id - min_meta_blk_id;
  if (meta_blk_id_range > FS_NAND_META_SEQ_QTY_HALF_RANGE) {    // If wrap around, discard seq nbr > half range ...
    p_nand_ftl->MetaBlkIxPhy = max_seq_blk_ix_lwr;
    p_nand_ftl->DirtyBitmapSrchPos = min_seq_blk_ix_higher - p_nand_ftl->BlkIxFirst;
  } else {                                                      // ... otherwise, take absolute max.
    p_nand_ftl->MetaBlkIxPhy = max_seq_blk_ix;
    p_nand_ftl->DirtyBitmapSrchPos = min_seq_blk_ix - p_nand_ftl->BlkIxFirst;
  }

  if (p_nand_ftl->MetaBlkIxPhy == FS_NAND_BLK_IX_INVALID) {
    LOG_ERR(("No valid meta block found."));
    RTOS_ERR_SET(*p_err, RTOS_ERR_BLK_DEV_FMT_INVALID);
    return;
  }

  LOG_VRB(("Found metadata block at block index ", (u)p_nand_ftl->MetaBlkIxPhy, "."));
}

/****************************************************************************************************//**
 *                                       FS_NAND_FTL_MetaBlkFindID()
 *
 * @brief    Find a sector of the specific metadata sequence on NAND device, and return the index of
 *           the physical block on which it is written.
 *
 * @param    p_nand_ftl      Pointer to NAND FTL.
 *
 * @param    meta_blk_id     ID of metadata block to find.
 *
 * @param    p_err           Error pointer.
 *
 * @return   Physical index of block containing sectors with specified sequence number, if successful;
 *           FS_NAND_BLK_IX_INVALID,                                                    otherwise.
 *******************************************************************************************************/
static FS_NAND_BLK_QTY FS_NAND_FTL_MetaBlkFindID(FS_NAND_FTL     *p_nand_ftl,
                                                 FS_NAND_META_ID meta_blk_id,
                                                 RTOS_ERR        *p_err)
{
  CPU_INT08U           *p_oos_buf;
  FS_NAND_BLK_QTY      blk_ix_phy;
  FS_NAND_BLK_QTY      last_blk_ix;
  CPU_INT08U           rd_retries;
  FS_NAND_SEC_TYPE_STO sec_type;
  FS_NAND_META_ID      meta_blk_id_rd;

  p_oos_buf = (CPU_INT08U *)p_nand_ftl->OOS_BufPtr;
  blk_ix_phy = p_nand_ftl->BlkIxFirst;
  last_blk_ix = blk_ix_phy + p_nand_ftl->BlkCnt - 1u;

  while (blk_ix_phy <= last_blk_ix) {
    //                                                             ---------------------- RD SEC ----------------------
    rd_retries = 0u;
    do {
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

      FS_NAND_FTL_SecRdPhyNoRefresh(p_nand_ftl,
                                    p_nand_ftl->BufPtr,
                                    p_oos_buf,
                                    blk_ix_phy,
                                    0u,
                                    p_err);
      rd_retries++;
    } while ((rd_retries <= FS_NAND_CFG_MAX_RD_RETRIES)
             && (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_ECC_UNCORR));

    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (FS_NAND_BLK_IX_INVALID);
    }

    MEM_VAL_COPY_GET_INTU_LITTLE(&sec_type,                     // Get sec type.
                                 &p_oos_buf[FS_NAND_OOS_SEC_TYPE_OFFSET],
                                 sizeof(FS_NAND_SEC_TYPE_STO));

    if (sec_type == FS_NAND_SEC_TYPE_METADATA) {                // If it's a meta blk ...
      MEM_VAL_COPY_GET_INTU_LITTLE(&meta_blk_id_rd,             // Get blk ID.
                                   &p_oos_buf[FS_NAND_OOS_META_ID_OFFSET],
                                   sizeof(FS_NAND_META_ID));

      if (meta_blk_id_rd == meta_blk_id) {
        return (blk_ix_phy);                                    // Meta blk     found.
      }
    }

    blk_ix_phy++;
  }

  //                                                               Meta blk not found.
  LOG_ERR(("Unable to find metadata block for sequence ", (u)meta_blk_id, "."));
  RTOS_ERR_SET(*p_err, RTOS_ERR_BLK_DEV_CORRUPTED);

  return (FS_NAND_BLK_IX_INVALID);
}

/****************************************************************************************************//**
 *                                       FS_NAND_FTL_MetaBlkParse()
 *
 * @brief    Load metadata block contents into RAM.
 *
 * @param    p_nand_ftl  Pointer to NAND FTL.
 *
 * @param    p_err       Error pointer.
 *
 * @note     (1) Combined to the fact that all blocks located in the available block table at mount
 *               time are considered as potential metadata blocks (see FS_NAND_InitDevData()), this
 *               guarantees that all blocks in the available block table will be used after remounting,
 *               thus preventing stale metadata blocks.
 *******************************************************************************************************/
static void FS_NAND_FTL_MetaBlkParse(FS_NAND_FTL *p_nand_ftl,
                                     RTOS_ERR    *p_err)
{
  CPU_INT08U                  *p_sec_buf;
  CPU_INT08U                  *p_oos_buf;
  CPU_SIZE_T                  map_size;
  CPU_SIZE_T                  offset_octet;
  CPU_INT08U                  offset_bit;
  FS_NAND_SEC_PER_BLK_QTY     sec_ix;
  FS_NAND_BLK_QTY             meta_blk_ix;
  FS_NAND_META_ID             meta_blk_id = 0u;
  FS_NAND_META_SEC_QTY        meta_sec_ix;
  FS_NAND_META_SEQ_STATUS_STO seq_status;
  FS_LB_QTY                   sec_base;
  CPU_SIZE_T                  copy_size;
  CPU_INT08U                  rd_retries;
  CPU_INT08U                  meta_map[32];
  CPU_BOOLEAN                 seq_last_sec;
  CPU_BOOLEAN                 last_seq_found;
  CPU_BOOLEAN                 is_sec_used;
  CPU_BOOLEAN                 status_finished;
  CPU_BOOLEAN                 done;
  CPU_DATA                    ix;

  p_sec_buf = (CPU_INT08U *)p_nand_ftl->BufPtr;
  p_oos_buf = (CPU_INT08U *)p_nand_ftl->OOS_BufPtr;

  //                                                               ------------- INIT META SEC VALID MAP --------------
  map_size = p_nand_ftl->MetaSecCnt / DEF_OCTET_NBR_BITS;
  map_size += p_nand_ftl->MetaSecCnt % DEF_OCTET_NBR_BITS == 0u ? 0u : 1u;

  //                                                               Init map to all zeroes.
  for (ix = 0; ix < map_size; ix++) {
    p_nand_ftl->MetaBlkInvalidSecMap[ix] = 0x00u;
  }

  //                                                               Mark each sector invalid.
  for (sec_ix = 0u; sec_ix < p_nand_ftl->MetaSecCnt; sec_ix++) {
    offset_octet = sec_ix / DEF_OCTET_NBR_BITS;
    offset_bit = sec_ix % DEF_OCTET_NBR_BITS;

    p_nand_ftl->MetaBlkInvalidSecMap[offset_octet] |= (1u << offset_bit);
  }

  //                                                               --------------------- RD META ----------------------
  done = DEF_NO;
  meta_blk_ix = p_nand_ftl->MetaBlkIxPhy;
  p_nand_ftl->MetaBlkNextSecIx = 0u;
  p_nand_ftl->MetaBlkFoldNeeded = DEF_NO;
  sec_ix = p_nand_ftl->NbrSecPerBlk - 1u;
  status_finished = DEF_NO;
  seq_last_sec = DEF_NO;
  last_seq_found = DEF_NO;

  LOG_VRB(("Parsing meta block ", (u)meta_blk_ix));

  while (done != DEF_YES) {                                     // Until all sec have been found.
    sec_base = FS_NAND_BLK_IX_TO_SEC_IX(p_nand_ftl, meta_blk_ix);

    //                                                             Find first written sec.
    is_sec_used = DEF_NO;
    while (is_sec_used != DEF_YES) {
      is_sec_used = FS_NAND_FTL_SecIsUsed(p_nand_ftl,
                                          sec_base + sec_ix,
                                          p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return;
      }
      //                                                           Dec sec ix if not used.
      if (is_sec_used != DEF_YES) {
        if (sec_ix == 0u) {
          LOG_ERR(("Fatal error; no metadata sectors found in block."));
          RTOS_ERR_SET(*p_err, RTOS_ERR_BLK_DEV_CORRUPTED);
          return;
        }
        sec_ix--;
      }
    }

    //                                                             Update next meta sec ix if latest meta blk.
    if ((p_nand_ftl->MetaBlkNextSecIx == 0u)
        && (meta_blk_ix == p_nand_ftl->MetaBlkIxPhy)
        && (p_nand_ftl->MetaBlkFoldNeeded == DEF_NO)   ) {
      p_nand_ftl->MetaBlkNextSecIx = sec_ix + 1u;

      //                                                           If ix wraps, blk is full and a fold is needed.
      if (p_nand_ftl->MetaBlkNextSecIx == 0u) {
        p_nand_ftl->MetaBlkFoldNeeded = DEF_YES;
      } else {
        p_nand_ftl->MetaBlkFoldNeeded = DEF_NO;
      }
    }

    sec_ix++;
    while ((done != DEF_YES) && (sec_ix > 0)) {                 // For each sec in blk, or until we're done ...
      sec_ix--;

      rd_retries = 0u;
      do {                                                      // Rd sec, retrying if ECC err.
        FS_NAND_FTL_SecRdPhyNoRefresh(p_nand_ftl,
                                      p_sec_buf,
                                      p_oos_buf,
                                      meta_blk_ix,
                                      sec_ix,
                                      p_err);
        rd_retries++;
      } while ((rd_retries <= FS_NAND_CFG_MAX_RD_RETRIES)
               && (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_ECC_UNCORR));

      if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_ECC_UNCORR) {
        LOG_DBG(("ECC error reading metadata sector ", (u)sec_base + sec_ix, ". Ignoring sector."));
        RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      } else if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        LOG_ERR(("Fatal error reading metadata sector ", (u)sec_base + sec_ix, "."));
        return;
      } else {
        MEM_VAL_COPY_GET_INTU_LITTLE(&seq_status,               // Rd seq status.
                                     &p_oos_buf[FS_NAND_OOS_META_SEQ_STATUS_OFFSET],
                                     sizeof(FS_NAND_META_SEQ_STATUS_STO));

        MEM_VAL_COPY_GET_INTU_LITTLE(&meta_blk_id,
                                     &p_oos_buf[FS_NAND_OOS_META_ID_OFFSET],
                                     sizeof(FS_NAND_META_ID));

        if (last_seq_found == DEF_NO) {
          p_nand_ftl->MetaBlkID = meta_blk_id;
          last_seq_found = DEF_YES;
        }

        switch (seq_status) {                                   // Chk for seq finished mark.
          case FS_NAND_META_SEQ_FINISHED:
            status_finished = DEF_YES;
            break;

          case FS_NAND_META_SEQ_AVAIL_BLK_TBL_ONLY:
            status_finished = DEF_NO;
            break;

          default:
            if (seq_last_sec == DEF_YES) {
              status_finished = DEF_NO;
            }
            break;
        }

        if (seq_status == FS_NAND_META_SEQ_NEW) {               // Determine if this is the last sec of the seq.
          seq_last_sec = DEF_YES;
        } else {
          seq_last_sec = DEF_NO;
        }

        if (status_finished == DEF_YES) {                       // Seq finished; we can consider sec.
          MEM_VAL_COPY_GET_INTU_LITTLE(&meta_sec_ix,            // Rd meta sec ix.
                                       &p_oos_buf[FS_NAND_OOS_META_SEC_IX_OFFSET],
                                       sizeof(FS_NAND_META_SEC_QTY));

          offset_octet = meta_sec_ix / DEF_OCTET_NBR_BITS;
          offset_bit = meta_sec_ix % DEF_OCTET_NBR_BITS;

          //                                                       Check if meta sec has already been cached.
          if ((p_nand_ftl->MetaBlkInvalidSecMap[offset_octet] & DEF_BIT(offset_bit)) != 0u) {
            //                                                     Calc size to copy.
            if (meta_sec_ix == p_nand_ftl->MetaSecCnt - 1) {
              //                                                   Last meta sec, copy remaining size.
              copy_size = p_nand_ftl->MetaSize;
              copy_size -= (p_nand_ftl->MetaSecCnt - 1) * p_nand_ftl->SecSize;
            } else {
              copy_size = p_nand_ftl->SecSize;
            }

            //                                                     Copy metadata.
            Mem_Copy(&p_nand_ftl->MetaCache[meta_sec_ix * p_nand_ftl->SecSize],
                     &p_sec_buf[0],
                     copy_size);

            //                                                     Mark meta sec as up-to-date.
            p_nand_ftl->MetaBlkInvalidSecMap[offset_octet] ^= DEF_BIT(offset_bit);

            //                                                     Chk if all sec have been rd.
            done = DEF_YES;
            ix = 0u;
            while ((done == DEF_YES)
                   && (ix < map_size)   ) {
              if (p_nand_ftl->MetaBlkInvalidSecMap[ix] != 0x00u) {
                done = DEF_NO;
              }
              ix++;
            }
          }
        }
      }
    }

    if (meta_blk_ix == p_nand_ftl->MetaBlkIxPhy) {              // If this is the latest meta blk, preserve meta map.
      Mem_Copy(&meta_map[0],
               &p_nand_ftl->MetaBlkInvalidSecMap[0],
               map_size);
    }

    if (done != DEF_YES) {                                      // Blk finished, but not all needed data was found.
      meta_blk_id--;                                            // Wrap if 0.
      meta_blk_ix = FS_NAND_FTL_MetaBlkFindID(p_nand_ftl,       // Find previous meta blk.
                                              meta_blk_id,
                                              p_err);
      LOG_DBG(("Failed parsing meta block. Falling back to previous meta block (", (u)meta_blk_ix, ")"));
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        LOG_ERR(("Fatal error; could not find metadata block for sequence ", (u)meta_blk_id, "."));
        return;
      }
    }
  }

  //                                                               See Note # 1.
  for (ix = 0u; ix < p_nand_ftl->AvailBlkTblEntryCntMax; ix++) {
    p_nand_ftl->AvailBlkMetaID_Tbl[ix] = p_nand_ftl->MetaBlkID - FS_NAND_META_ID_STALE_THRESH - 1u;
  }

  //                                                               ----------- RESTORE META INVALID SEC MAP -----------
  Mem_Copy(&p_nand_ftl->MetaBlkInvalidSecMap[0],
           &meta_map[0],
           map_size);
}

/****************************************************************************************************//**
 *                                       FS_NAND_FTL_MetaBlkAvailParse()
 *
 * @brief    Parse available block table commits following the last complete metadata sequence committed.
 *
 * @param    p_nand_ftl  Pointer to NAND FTL.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NAND_FTL_MetaBlkAvailParse(FS_NAND_FTL *p_nand_ftl,
                                          RTOS_ERR    *p_err)
{
  FS_NAND                     *p_nand;
  CPU_INT08U                  *p_buf;
  CPU_INT08U                  *p_oos_buf;
  FS_NAND_SEC_PER_BLK_QTY     meta_sec_offset;
  FS_NAND_META_SEQ_STATUS_STO meta_seq_status_sto;
  FS_NAND_BLK_QTY             avail_tbl_ix;
  FS_NAND_BLK_QTY             tmp_avail_tbl_ix;
  FS_NAND_AVAIL_BLK_ENTRY     avail_blk;
  FS_NAND_AVAIL_BLK_ENTRY     tmp_avail_blk;
  FS_NAND_SEC_TYPE_STO        sec_type_sto;
  FS_NAND_SEC_TYPE            sec_type;
  FS_LB_QTY                   sec_ix_phy;
  CPU_SIZE_T                  meta_offset;
  CPU_SIZE_T                  avail_blk_map_size;
  CPU_SIZE_T                  ix;
  CPU_BOOLEAN                 in_avail_tbl;
  CPU_BOOLEAN                 blk_is_dirty;
  CPU_BOOLEAN                 entry_added;
  CPU_BOOLEAN                 sec_scan_complete;
  CPU_BOOLEAN                 create_dummy_blk;

  p_buf = (CPU_INT08U *)p_nand_ftl->BufPtr;
  p_oos_buf = (CPU_INT08U *)p_nand_ftl->OOS_BufPtr;

  p_nand = (FS_NAND *)p_nand_ftl->BlkDev.MediaPtr;

  meta_sec_offset = p_nand_ftl->MetaBlkNextSecIx - 1u;

  //                                                               ------ SCAN META SECS AFTER LAST FULL COMMIT -------
  sec_scan_complete = DEF_NO;
  while ((sec_scan_complete == DEF_NO) && (meta_sec_offset > 0)) {
    FS_NAND_FTL_SecRdPhyNoRefresh(p_nand_ftl,
                                  p_buf,
                                  p_oos_buf,
                                  p_nand_ftl->MetaBlkIxPhy,
                                  meta_sec_offset,
                                  p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      LOG_DBG(("Unable to read sector ", (u)meta_sec_offset, " of metadata block. "
                                                             "Some erase counts might be lost."));
    } else {
      MEM_VAL_COPY_GET_INTU_LITTLE(&meta_seq_status_sto,
                                   &p_oos_buf[FS_NAND_OOS_META_SEQ_STATUS_OFFSET],
                                   sizeof(FS_NAND_META_SEQ_STATUS_STO));

      switch (meta_seq_status_sto) {
        case FS_NAND_META_SEQ_FINISHED:
          sec_scan_complete = DEF_YES;                          // Seq finished: done processing avail blk tbl commits.
          break;

        case FS_NAND_META_SEQ_UNFINISHED:
          break;

        case FS_NAND_META_SEQ_AVAIL_BLK_TBL_ONLY:
          //                                                       ---------- SCAN AVAIL TBL FOR NEW ENTRIES ----------
          for (tmp_avail_tbl_ix = 0u; tmp_avail_tbl_ix < p_nand_ftl->AvailBlkTblEntryCntMax; tmp_avail_tbl_ix++) {
            //                                                     Rd entry.
            meta_offset = tmp_avail_tbl_ix * (sizeof(FS_NAND_BLK_QTY) + sizeof(FS_NAND_ERASE_QTY));
            MEM_VAL_COPY_GET_INTU_LITTLE(&tmp_avail_blk.BlkIxPhy,
                                         &p_buf[meta_offset],
                                         sizeof(FS_NAND_BLK_QTY));

            if (tmp_avail_blk.BlkIxPhy != FS_NAND_BLK_IX_INVALID) {
              //                                                   Chk if already in tbl.
              in_avail_tbl = FS_NAND_FTL_BlkIsAvail(p_nand_ftl, tmp_avail_blk.BlkIxPhy);
              if (in_avail_tbl == DEF_NO) {
                LOG_VRB(("Found available block ", (u)tmp_avail_blk.BlkIxPhy,
                         " from a temporary available block table commit."));

                meta_offset += sizeof(FS_NAND_BLK_QTY);
                MEM_VAL_COPY_GET_INTU_LITTLE(&tmp_avail_blk.EraseCnt,
                                             &p_buf[meta_offset],
                                             sizeof(FS_NAND_ERASE_QTY));

                //                                                 Make sure blk is dirty.
                blk_is_dirty = FS_NAND_FTL_BlkIsDirty(p_nand_ftl, tmp_avail_blk.BlkIxPhy);
                if (blk_is_dirty != DEF_YES) {
                  LOG_ERR(("New available block ", (u)tmp_avail_blk.BlkIxPhy,
                           " wasn't dirty in last full commit. Volume might be corrupted."));
                  RTOS_ERR_SET(*p_err, RTOS_ERR_BLK_DEV_CORRUPTED);
                  return;
                }

                //                                                 ----------- PROCESS NEW AVAIL TBL ENTRY ------------
                avail_tbl_ix = 0u;
                entry_added = DEF_NO;
                while ((entry_added == DEF_NO)
                       && (avail_tbl_ix < p_nand_ftl->AvailBlkTblEntryCntMax)) {
                  avail_blk = FS_NAND_FTL_AvailBlkTblEntryRd(p_nand_ftl, avail_tbl_ix);
                  if (avail_blk.BlkIxPhy == FS_NAND_BLK_IX_INVALID) {
                    LOG_VRB(("Adding block ", (u)tmp_avail_blk.BlkIxPhy, " to available block table."));

                    //                                             Unmap blk.
                    FS_NAND_FTL_BlkUnmap(p_nand_ftl, tmp_avail_blk.BlkIxPhy);

                    //                                             Insert avail entry in empty slot.
                    avail_blk.BlkIxPhy = tmp_avail_blk.BlkIxPhy;
                    avail_blk.EraseCnt = tmp_avail_blk.EraseCnt;

                    FS_NAND_FTL_AvailBlkTblEntryWr(p_nand_ftl, avail_tbl_ix, avail_blk);
                    FS_NAND_FTL_AvailBlkTblInvalidate(p_nand_ftl);

                    entry_added = DEF_YES;
                  }

                  avail_tbl_ix++;
                }

                //                                                 No space left: chk if a dummy blk is needed.
                if (entry_added == DEF_NO) {
                  FS_NAND_FTL_SecRdPhyNoRefresh(p_nand_ftl,
                                                p_buf,
                                                p_oos_buf,
                                                tmp_avail_blk.BlkIxPhy,
                                                0u,
                                                p_err);
                  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
                    sec_type = FS_NAND_FTL_SecTypeParse(p_oos_buf);

                    //                                             If sec type isn't unused, no need for dummy blk.
                    //                                             (erase cnt will be in OOS in those cases.)
                    if (sec_type == FS_NAND_SEC_TYPE_UNUSED) {
                      create_dummy_blk = DEF_YES;
                    } else {
                      create_dummy_blk = DEF_NO;
                    }
                  } else {
                    create_dummy_blk = DEF_YES;
                  }

                  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
                } else {
                  create_dummy_blk = DEF_NO;
                }

                //                                                 Create dummy blk to hold erase cnt.
                if (create_dummy_blk == DEF_YES) {
                  FS_NAND_BlkEraseInternal(p_nand,
                                           tmp_avail_blk.BlkIxPhy,
                                           p_err);
                  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
                    LOG_ERR(("Unable to erase block ", (u)tmp_avail_blk.BlkIxPhy, ". Marking it bad."));

                    FS_NAND_FTL_BlkMarkBad(p_nand_ftl, tmp_avail_blk.BlkIxPhy, p_err);
                    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
                      LOG_ERR(("Unable to mark block ", (u)tmp_avail_blk.BlkIxPhy, "bad."));
                      return;
                    }
                  }

                  //                                               Set sec type in OOS.
                  sec_type = FS_NAND_SEC_TYPE_DUMMY;
                  sec_type_sto = sec_type;

                  MEM_VAL_COPY_SET_INTU_LITTLE(&p_oos_buf[FS_NAND_OOS_SEC_TYPE_OFFSET],
                                               &sec_type_sto,
                                               sizeof(FS_NAND_SEC_TYPE_STO));

                  //                                               Set erase cnt in OOS.
                  MEM_VAL_COPY_SET_INTU_LITTLE(&p_oos_buf[FS_NAND_OOS_ERASE_CNT_OFFSET],
                                               &tmp_avail_blk.EraseCnt,
                                               sizeof(FS_NAND_ERASE_QTY));

                  //                                               Wr first sec with erase cnt. Don't care about data.
                  sec_ix_phy = FS_NAND_BLK_IX_TO_SEC_IX(p_nand_ftl, tmp_avail_blk.BlkIxPhy);
                  p_nand->CtrlrPtr->CtrlrApiPtr->SecWr(p_nand->CtrlrPtr,
                                                       p_buf,
                                                       p_oos_buf,
                                                       sec_ix_phy,
                                                       p_err);
                  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
                    LOG_ERR(("Unable to write first sector of block ",
                             (u)tmp_avail_blk.BlkIxPhy, ". Marking block bad."));

                    FS_NAND_FTL_BlkMarkBad(p_nand_ftl, tmp_avail_blk.BlkIxPhy, p_err);
                    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
                      LOG_ERR(("Unable to mark block ", (u)tmp_avail_blk.BlkIxPhy, " bad."));
                      return;
                    }
                  } else {
                    //                                             Mark erased blk dirty.
                    FS_NAND_FTL_BlkMarkDirty(p_nand_ftl, tmp_avail_blk.BlkIxPhy);
                  }
                }

                //                                                 Re-rd avail blk tbl since p_buf has been used.
                if (entry_added == DEF_NO) {
                  FS_NAND_FTL_SecRdPhyNoRefresh(p_nand_ftl,
                                                p_buf,
                                                p_oos_buf,
                                                p_nand_ftl->MetaBlkIxPhy,
                                                meta_sec_offset,
                                                p_err);
                  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
                    LOG_ERR(("Fatal error -- unable to read sector ", (u)meta_sec_offset, " of metadata block. "
                                                                                          "Sector was previously readable."));
                    return;
                  }
                }
              }
            }
          }

          break;

        case FS_NAND_META_SEQ_NEW:
        default:
          break;
      }
    }

    meta_sec_offset--;
  }

  //                                                               Set avail blk tbl commit map bits.
  avail_blk_map_size = FS_UTIL_BIT_NBR_TO_OCTET_NBR(p_nand_ftl->AvailBlkTblEntryCntMax);
  for (ix = 0; ix < avail_blk_map_size; ix++) {
    *p_nand_ftl->AvailBlkTblCommitMap = 0xFF;
  }
}
#endif

/****************************************************************************************************//**
 *                                           FS_NAND_FTL_MetaBlkFold()
 *
 * @brief    - (1) Creates a new metadata block.
 *           - (2) Copies valid metadata sectors from the old to the new metadata block.
 *           - (3) Mark the old metadata block dirty.
 *
 * @param    p_nand_ftl  Pointer to NAND FTL.
 *
 * @param    p_err       Error pointer.
 *
 * @note     (1) When folding a metadata block it is impossible to add a new block to the available
 *               block table (since there is no space left in the metadata block being folded). To make
 *               sure blocks are available, a portion of the available blocks table is reserved for
 *               FS_NAND_MetaBlkFold. This reserved space must always be filled, and not used by other
 *               functions than this one.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NAND_FTL_MetaBlkFold(FS_NAND_FTL *p_nand_ftl,
                                    RTOS_ERR    *p_err)
{
  FS_NAND_BLK_QTY blk_ix_phy_new;
  FS_NAND_BLK_QTY avail_blk_entry_cnt;
  FS_NAND_BLK_QTY cnt;
  CPU_SIZE_T      inv_sec_map_size;
  CPU_SIZE_T      ix;

  LOG_VRB(("Fold metadata block: old=", (u)p_nand_ftl->MetaBlkIxPhy));

  //                                                               ----------- CNT ENTRIES IN AVAIL BLK TBL -----------
  avail_blk_entry_cnt = FS_NAND_FTL_AvailBlkTblEntryCnt(p_nand_ftl);

  p_nand_ftl->MetaBlkID++;

  //                                                               ------------------- GET NEW BLK --------------------
  do {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    //                                                             Find avail blk.
    blk_ix_phy_new = FS_NAND_FTL_BlkGetAvailFromTbl(p_nand_ftl, DEF_YES);
    if (blk_ix_phy_new == FS_NAND_BLK_IX_INVALID) {
      LOG_ERR(("Fatal exception. Unable to get a new metadata block."));
      RTOS_ERR_SET(*p_err, RTOS_ERR_BLK_DEV_CORRUPTED);
      return;
    }

    FS_NAND_FTL_BlkEnsureErased(p_nand_ftl,                     // Make sure the blk is erased.
                                blk_ix_phy_new,
                                p_err);

    avail_blk_entry_cnt++;
  } while (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE);

  //                                                               --------------- FILL AVAIL BLK TBL -----------------
  //                                                               Limit refill to max cnt.
  if (avail_blk_entry_cnt > p_nand_ftl->AvailBlkTblEntryCntMax) {
    avail_blk_entry_cnt = p_nand_ftl->AvailBlkTblEntryCntMax;
  }

  cnt = FS_NAND_FTL_AvailBlkTblFill(p_nand_ftl,
                                    avail_blk_entry_cnt,        // Refill to same level to make sure callers still have
                                    DEF_ENABLED,                // enough avail blks committed.
                                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if (cnt != avail_blk_entry_cnt) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_BLK_DEV_CORRUPTED);
    return;
  }

  //                                                               ------------------- UPDATE META --------------------
  FS_NAND_FTL_BlkMarkDirty(p_nand_ftl,                          // Mark old meta blk dirty.
                           p_nand_ftl->MetaBlkIxPhy);

  p_nand_ftl->MetaBlkIxPhy = blk_ix_phy_new;
  p_nand_ftl->MetaBlkNextSecIx = 0u;

  p_nand_ftl->MetaBlkFoldNeeded = DEF_NO;

  //                                                               ----------------- INVALIDATE META ------------------
  inv_sec_map_size = (p_nand_ftl->MetaSecCnt + 1u) / DEF_OCTET_NBR_BITS;
  inv_sec_map_size += (p_nand_ftl->MetaSecCnt + 1u) % DEF_OCTET_NBR_BITS == 0u ? 0u : 1u;

  for (ix = 0; ix < inv_sec_map_size; ix++) {
    p_nand_ftl->MetaBlkInvalidSecMap[ix] = 0xFFu;
  }

  LOG_VRB(("Fold metadata block: new= ", (u)p_nand_ftl->MetaBlkIxPhy));
}
#endif

/****************************************************************************************************//**
 *                                           FS_NAND_FTL_MetaSecFind()
 *
 * @brief    Find the physical sector index corresponding to specified metadata sector index.
 *
 * @param    p_nand_ftl      Pointer to NAND FTL.
 *
 * @param    meta_sec_ix     Metadata sector index.
 *
 * @param    p_err           Error pointer.
 *
 * @return   Physical sector offset of metadata sector in metadata block, if found,
 *           FS_NAND_SEC_OFFSET_IX_INVALID                              , otherwise.
 *
 * @note     (1) Each time metadata sectors are committed, the last one will be written with a status
 *               equal to FS_NAND_META_SEQ_FINISHED. This method prevents considering metadata sectors
 *               from an uncompleted sequence. Doing this, integrity is assured, even after an
 *               unexpected power loss that would otherwise cause corruption.
 *******************************************************************************************************/

#if (FS_NAND_CFG_DIRTY_MAP_CACHE_EN != DEF_ENABLED)
static FS_NAND_SEC_PER_BLK_QTY FS_NAND_FTL_MetaSecFind(FS_NAND_FTL          *p_nand_ftl,
                                                       FS_NAND_META_SEC_QTY meta_sec_ix,
                                                       RTOS_ERR             *p_err)
{
  CPU_INT08U                  *p_oos_buf;
  FS_NAND_SEC_PER_BLK_QTY     sec_offset_phy;
  FS_NAND_SEC_PER_BLK_QTY     sec_rem;
  FS_NAND_META_SEC_QTY        meta_sec_ix_rd;
  FS_NAND_META_SEQ_STATUS_STO meta_seq_status;
  CPU_BOOLEAN                 meta_seq_valid;
  CPU_BOOLEAN                 meta_seq_last_sec;

  p_oos_buf = (CPU_INT08U *)p_nand_ftl->OOS_BufPtr;

  //                                                               Calc range to srch.
  if (p_nand_ftl->MetaBlkNextSecIx != 0u) {
    sec_offset_phy = p_nand_ftl->MetaBlkNextSecIx - 1u;
    sec_rem = p_nand_ftl->MetaBlkNextSecIx;
  } else {
    sec_offset_phy = ((FS_NAND_META_SEC_QTY)-1);                // MetaBlkNextSecIx == 0 implies that meta blk is full.
    sec_rem = ((FS_NAND_META_SEC_QTY)-1) + 1u;
  }

  meta_seq_last_sec = DEF_NO;
  meta_seq_valid = DEF_NO;
  while (sec_rem != 0u) {
    FS_NAND_FTL_SecRdPhyNoRefresh(p_nand_ftl,                   // Rd sec in meta blk.
                                  p_nand_ftl->BufPtr,
                                  p_nand_ftl->OOS_BufPtr,
                                  p_nand_ftl->MetaBlkIxPhy,
                                  sec_offset_phy,
                                  p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (FS_NAND_SEC_OFFSET_IX_INVALID);
    } else {                                                    // Sec successfully rd.
      MEM_VAL_COPY_GET_INTU_LITTLE(&meta_sec_ix_rd,             // Get sec ix.
                                   &p_oos_buf[FS_NAND_OOS_META_SEC_IX_OFFSET],
                                   sizeof(FS_NAND_META_SEC_QTY));

      MEM_VAL_COPY_GET_INTU_LITTLE(&meta_seq_status,            // Get seq status.
                                   &p_oos_buf[FS_NAND_OOS_META_SEQ_STATUS_OFFSET],
                                   sizeof(FS_NAND_META_SEQ_STATUS_STO));

      if (meta_seq_status == FS_NAND_META_SEQ_FINISHED) {
        meta_seq_valid = DEF_YES;
      } else if (meta_seq_status == FS_NAND_META_SEQ_AVAIL_BLK_TBL_ONLY) {
        meta_seq_valid = DEF_NO;
      } else if (meta_seq_last_sec == DEF_YES) {
        meta_seq_valid = DEF_NO;
      }

      if (meta_seq_status == FS_NAND_META_SEQ_NEW) {
        meta_seq_last_sec = DEF_YES;
      } else {
        meta_seq_last_sec = DEF_NO;
      }

      //                                                           Verify seq is complete (see Note #1).
      if ((meta_seq_valid == DEF_YES)
          && (meta_sec_ix_rd == meta_sec_ix)) {
        return (sec_offset_phy);                                // Found latest meta sec.
      }
    }
    sec_rem--;                                                  // Prepare for next iteration.
    sec_offset_phy--;
  }

  return (FS_NAND_SEC_OFFSET_IX_INVALID);
}
#endif

/****************************************************************************************************//**
 *                                   FS_NAND_FTL_MetaSecRangeInvalidate()
 *
 * @brief    Invalidate a range of metadata sectors. They will be updated on next metadata commit.
 *
 * @param    p_nand_ftl      Pointer to NAND FTL.
 *
 * @param    sec_ix_first    First sector of the range to invalidate.
 *
 * @param    sec_ix_last     Last sector of the range to invalidate.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NAND_FTL_MetaSecRangeInvalidate(FS_NAND_FTL             *p_nand_ftl,
                                               FS_NAND_SEC_PER_BLK_QTY sec_ix_first,
                                               FS_NAND_SEC_PER_BLK_QTY sec_ix_last)
{
  FS_NAND_SEC_PER_BLK_QTY sec_ix;
  FS_NAND_SEC_PER_BLK_QTY offset_octet;
  CPU_INT08U              offset_bit;

  for (sec_ix = sec_ix_first; sec_ix <= sec_ix_last; sec_ix++) {
    offset_octet = sec_ix / DEF_OCTET_NBR_BITS;
    offset_bit = sec_ix % DEF_OCTET_NBR_BITS;

    p_nand_ftl->MetaBlkInvalidSecMap[offset_octet] |= DEF_BIT(offset_bit);
  }
}
#endif

/****************************************************************************************************//**
 *                                           FS_NAND_FTL_MetaCommit()
 *
 * @brief    Commit metadata to device. After this operation, device's state will be consistent with
 *           ram-stored structures. If 'avail_tbl_only' is set to DEF_YES, only the available block
 *           table will be commited to the device and consistent with the cached table.
 *
 * @param    p_nand_ftl      Pointer to NAND FTL.
 *
 * @param    avail_tbl_only  Indicates if only the available blocks table must be committed.
 *
 * @param    p_err           Error pointer.
 *
 * @note     (1) Not finalizing a metadata commit operation may be useful when erasing a block since
 *               the erase count of an erased block must be committed before actually erasing a block.
 *
 * @note     (2) This function should be retried if an error code is returned, unless the error code
 *               RTOS_ERR_DEV_NAND_NO_AVAIL_BLK is returned, which means the error is fatal.
 *
 * @note     (3) If only the available blocks table is commited, and that the current metadata block
 *               is full, the last valid metadata sequence must be commited on the new metadata
 *               block before commiting the available block table.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NAND_FTL_MetaCommit(FS_NAND_FTL *p_nand_ftl,
                                   CPU_BOOLEAN avail_tbl_only,
                                   RTOS_ERR    *p_err)
{
  FS_NAND_META_SEC_QTY    meta_sec_ix;
  FS_NAND_META_SEC_QTY    meta_sec_ix_first;
  FS_NAND_META_SEC_QTY    meta_sec_ix_last;
  CPU_SIZE_T              offset_octet;
  CPU_INT08U              offset_bit;
  CPU_INT08U              sec_needs_update;
  CPU_INT08U              sec_update_cnt;
  FS_NAND_SEC_PER_BLK_QTY sec_rem;
  FS_NAND_META_SEQ_STATUS seq_status;
  CPU_DATA                avail_blk_tbl_size;
  CPU_DATA                ix;
  CPU_BOOLEAN             done;

  done = DEF_NO;
  while (done != DEF_YES) {
    //                                                             -------------- FIND ALL SEC TO COMMIT --------------
    meta_sec_ix_first = (FS_NAND_META_SEC_QTY) -1;
    meta_sec_ix_last = (FS_NAND_META_SEC_QTY) -1;
    if (avail_tbl_only == DEF_YES) {
      sec_update_cnt = 1u;                                      // Tbl fits 1 sec (see FS_NAND_FTL_AllocDevData Note #1a)
      meta_sec_ix_first = 0u;
      meta_sec_ix_last = 0u;
    } else {
      sec_update_cnt = 0;                                       // Scan metadata sector invalid map.
      for (meta_sec_ix = 0u; meta_sec_ix < p_nand_ftl->MetaSecCnt; meta_sec_ix++) {
        offset_octet = meta_sec_ix / DEF_OCTET_NBR_BITS;
        offset_bit = meta_sec_ix % DEF_OCTET_NBR_BITS;

        sec_needs_update = p_nand_ftl->MetaBlkInvalidSecMap[offset_octet] & DEF_BIT08(offset_bit);

        if (sec_needs_update != 0u) {
          sec_update_cnt++;

          meta_sec_ix_last = meta_sec_ix;
          if (meta_sec_ix_first == (FS_NAND_META_SEC_QTY) -1) {
            meta_sec_ix_first = meta_sec_ix;
          }
        }
      }
    }

    if (meta_sec_ix_first == (FS_NAND_META_SEC_QTY) -1) {       // No meta sec are invalidated.
      return;
    }

    //                                                             -------- PERFORM FOLD OF META BLK IF NEEDED --------

    //                                                             Chk if meta fits actual meta blk ...
    sec_rem = p_nand_ftl->NbrSecPerBlk - p_nand_ftl->MetaBlkNextSecIx;
    if (sec_update_cnt > sec_rem) {
      p_nand_ftl->MetaBlkFoldNeeded = DEF_YES;                  // ... fold if not enough sec rem.

      sec_update_cnt = p_nand_ftl->MetaSecCnt;
      meta_sec_ix_first = 0u;
      meta_sec_ix_last = p_nand_ftl->MetaSecCnt - 1u;
      avail_tbl_only = DEF_NO;
      FS_NAND_FTL_MetaBlkFold(p_nand_ftl, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return;
      }
    }

    //                                                             ----------------- COMMIT META SEC ------------------
    if (avail_tbl_only == DEF_YES) {
      seq_status = FS_NAND_META_SEQ_AVAIL_BLK_TBL_ONLY;
    } else {
      seq_status = FS_NAND_META_SEQ_NEW;
    }

    meta_sec_ix = meta_sec_ix_first;
    while ((meta_sec_ix <= meta_sec_ix_last)
           && (p_nand_ftl->MetaBlkFoldNeeded == DEF_NO)) {
      offset_octet = 0u;
      offset_bit = 0u;

      if (avail_tbl_only == DEF_YES) {
        sec_needs_update = 1;
      } else {
        offset_octet = meta_sec_ix / DEF_OCTET_NBR_BITS;
        offset_bit = meta_sec_ix % DEF_OCTET_NBR_BITS;
        sec_needs_update = p_nand_ftl->MetaBlkInvalidSecMap[offset_octet] & DEF_BIT08(offset_bit);
      }

      if (sec_needs_update != 0u) {
        if (sec_update_cnt == 1u) {                             // If last sec to commit, chng seq status to finished.
          if (seq_status != FS_NAND_META_SEQ_AVAIL_BLK_TBL_ONLY) {
            seq_status = FS_NAND_META_SEQ_FINISHED;
          }
        }

        //                                                         Commit sector.
        FS_NAND_FTL_MetaSecCommit(p_nand_ftl,
                                  meta_sec_ix,
                                  seq_status,
                                  p_err);
        if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_ABORT) {
          //                                                       Trigger fold and complete meta commit.
          avail_tbl_only = DEF_NO;
          p_nand_ftl->MetaBlkFoldNeeded = DEF_YES;
        } else if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          LOG_ERR(("Error committing sector ", (u)meta_sec_ix));
          return;
        } else {
          if (avail_tbl_only != DEF_YES) {                      // Remove from invalid map.
            p_nand_ftl->MetaBlkInvalidSecMap[offset_octet] ^= DEF_BIT(offset_bit);
          }

          sec_update_cnt--;
          seq_status = FS_NAND_META_SEQ_UNFINISHED;             // Chng seq status to unfinished for all next secs.
        }
      }

      meta_sec_ix++;
    }

    if (p_nand_ftl->MetaBlkFoldNeeded == DEF_NO) {              // If this point is reached without fold needed ...
      done = DEF_YES;                                           // ... operation is done.
    }
  }

  //                                                               Set avail blk tbl commit map bits.
  avail_blk_tbl_size = FS_UTIL_BIT_NBR_TO_OCTET_NBR(p_nand_ftl->AvailBlkTblEntryCntMax);
  for (ix = 0; ix < avail_blk_tbl_size; ix++) {
    *p_nand_ftl->AvailBlkTblCommitMap = 0xFF;
  }

  //                                                               -------------- UPDATE METADATA CACHE ---------------
#if (FS_NAND_CFG_DIRTY_MAP_CACHE_EN == DEF_ENABLED)             // Copy metadata in DirtyBitmapCache.
  if (avail_tbl_only == DEF_NO) {
    Mem_Copy(&p_nand_ftl->DirtyBitmapCache[0u],
             &p_nand_ftl->MetaCache[p_nand_ftl->MetaOffsetDirtyBitmap],
             p_nand_ftl->DirtyBitmapSize);
  }
#endif
}
#endif

/****************************************************************************************************//**
 *                                       FS_NAND_FTL_MetaSecCommit()
 *
 * @brief    Commit a single metadata sector to the metadata block on device.
 *
 * @param    p_nand_ftl      Pointer to NAND FTL.
 *
 * @param    meta_sec_ix     Metadata sector's index.
 *
 * @param    seq_status      Sector's status to write.
 *
 * @param    p_err           Error pointer.
 *
 * @note     (1) This function should never be called by another function than FS_NAND_MetaCommit to
 *               avoid inconsistency in cache objects which are only updated in FS_NAND_MetaCommit,
 *               after a successful operation is completed.
 *
 * @note     (2) The field MetaBlkFoldNeeded of the p_nand_data structure can be set to 0 to force
 *               a fold of the metadata block. The fold will be performed after the function returns
 *               to FS_NAND_MetaCommit(), or the next time it will be called.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NAND_FTL_MetaSecCommit(FS_NAND_FTL             *p_nand_ftl,
                                      FS_NAND_META_SEC_QTY    meta_sec_ix,
                                      FS_NAND_META_SEQ_STATUS seq_status,
                                      RTOS_ERR                *p_err)
{
  CPU_INT08U                  *p_oos_buf;
  FS_NAND_SEC_TYPE_STO        sec_type;
  FS_NAND_META_ID             meta_blk_id;
  FS_NAND_META_SEQ_STATUS_STO seq_status_sto;
  FS_NAND_ERASE_QTY           erase_cnt;

  FS_BLK_DEV_CTR_STAT_INC(p_nand_ftl->Ctrs.StatMetaSecCommitCtr);

  LOG_VRB(("Metadata sector ", (u)meta_sec_ix, " commit at offset ", (u)p_nand_ftl->MetaBlkNextSecIx,
           " of block ", (u)p_nand_ftl->MetaBlkIxPhy, " (seq ", (u)p_nand_ftl->MetaBlkID, ")."));

  //                                                               ------------- REM AVAIL BLK TBL ENTRY --------------
  if (p_nand_ftl->MetaBlkNextSecIx == 0u) {
    erase_cnt = FS_NAND_FTL_BlkRemFromAvail(p_nand_ftl,
                                            p_nand_ftl->MetaBlkIxPhy);
  } else {
    erase_cnt = FS_NAND_ERASE_QTY_INVALID;
  }

  //                                                               ------------------- GATHER META --------------------
  FS_NAND_FTL_MetaSecGatherData(p_nand_ftl,
                                meta_sec_ix,
                                (CPU_INT08U *)p_nand_ftl->BufPtr);

  //                                                               -------------------- FILL OOS ----------------------
  p_oos_buf = (CPU_INT08U *)p_nand_ftl->OOS_BufPtr;

  Mem_Set(&p_oos_buf[FS_NAND_OOS_SEC_USED_OFFSET],              // Sec used mark.
          0x00u,
          p_nand_ftl->UsedMarkSize);

  sec_type = FS_NAND_SEC_TYPE_METADATA;                         // Sec type.
  MEM_VAL_COPY_SET_INTU_LITTLE(&p_oos_buf[FS_NAND_OOS_SEC_TYPE_OFFSET],
                               &sec_type,
                               sizeof(FS_NAND_SEC_TYPE_STO));

  //                                                               Erase cnt: Only valid if first sec of blk.
  MEM_VAL_COPY_SET_INTU_LITTLE(&p_oos_buf[FS_NAND_OOS_ERASE_CNT_OFFSET],
                               &erase_cnt,
                               sizeof(FS_NAND_ERASE_QTY));

  //                                                               Meta sec ix.
  MEM_VAL_COPY_SET_INTU_LITTLE(&p_oos_buf[FS_NAND_OOS_META_SEC_IX_OFFSET],
                               &meta_sec_ix,
                               sizeof(FS_NAND_META_SEC_QTY));

  meta_blk_id = p_nand_ftl->MetaBlkID;                          // Meta blk ID.
  MEM_VAL_COPY_SET_INTU_LITTLE(&p_oos_buf[FS_NAND_OOS_META_ID_OFFSET],
                               &meta_blk_id,
                               sizeof(FS_NAND_META_ID));

  seq_status_sto = seq_status;                                  // Meta seq status.
  MEM_VAL_COPY_SET_INTU_LITTLE(&p_oos_buf[FS_NAND_OOS_META_SEQ_STATUS_OFFSET],
                               &seq_status_sto,
                               sizeof(FS_NAND_META_SEQ_STATUS_STO));

  //                                                               ---------------------- WR SEC ----------------------
  FS_NAND_FTL_MetaSecWrHandler(p_nand_ftl,
                               p_nand_ftl->BufPtr,
                               p_nand_ftl->OOS_BufPtr,
                               p_nand_ftl->MetaBlkNextSecIx,
                               p_err);
  switch (RTOS_ERR_CODE_GET(*p_err)) {
    case RTOS_ERR_NONE:
      p_nand_ftl->MetaBlkNextSecIx++;
      if (p_nand_ftl->MetaBlkNextSecIx >= p_nand_ftl->NbrSecPerBlk) {
        p_nand_ftl->MetaBlkFoldNeeded = DEF_YES;                // Force folding meta blk (see Note #2).
      }
      break;

    case RTOS_ERR_ABORT:
      break;

    default:
      p_nand_ftl->MetaBlkNextSecIx++;
      if (p_nand_ftl->MetaBlkNextSecIx >= p_nand_ftl->NbrSecPerBlk) {
        p_nand_ftl->MetaBlkFoldNeeded = DEF_YES;                // Force folding meta blk (see Note #2).
      }
      break;
  }
}
#endif

/****************************************************************************************************//**
 *                                       FS_NAND_FTL_MetaSecGatherData()
 *
 * @brief    Gather metadata sector data for specified sector.
 *
 * @param    p_nand_ftl      Pointer to NAND FTL.
 *
 * @param    meta_sec_ix     Metadata sector's index.
 *
 * @param    p_buf           Pointer to buffer that will receive metadata sector data.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NAND_FTL_MetaSecGatherData(FS_NAND_FTL          *p_nand_ftl,
                                          FS_NAND_META_SEC_QTY meta_sec_ix,
                                          CPU_INT08U           *p_buf)
{
  CPU_SIZE_T offset_octet;
  CPU_SIZE_T size;
  CPU_INT08U *p_metadata;

  p_metadata = (CPU_INT08U *)p_nand_ftl->MetaCache;

  offset_octet = meta_sec_ix * p_nand_ftl->SecSize;
  size = p_nand_ftl->MetaSize - offset_octet;

  if (size > p_nand_ftl->SecSize) {
    size = p_nand_ftl->SecSize;                                 // Limit data size to sector size.
  }

  Mem_Copy(p_buf, &p_metadata[offset_octet], size);             // Copy data.
}
#endif

/****************************************************************************************************//**
 *                                       FS_NAND_FTL_AvailBlkTmpCommit()
 *
 * @brief    Commits metadata to sector to device immediately.
 *
 * @param    p_nand_ftl  Pointer to NAND FTL.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NAND_FTL_AvailBlkTblTmpCommit(FS_NAND_FTL *p_nand_ftl,
                                             RTOS_ERR    *p_err)
{
  do {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    FS_NAND_FTL_MetaCommit(p_nand_ftl,
                           DEF_YES,
                           p_err);
  } while ((RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE)
           && (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_BLK_DEV_CORRUPTED));
}
#endif

/****************************************************************************************************//**
 *                                       FS_NAND_FTL_AvailBlkTblEntryRd()
 *
 * @brief    Reads an entry from available blocks table.
 *
 * @param    p_nand_ftl  Pointer to NAND FTL.
 *
 * @param    tbl_ix      Index of entry to read.
 *
 * @return   Structure containing available block's physical index and its erase count.
 *******************************************************************************************************/
static FS_NAND_AVAIL_BLK_ENTRY FS_NAND_FTL_AvailBlkTblEntryRd(FS_NAND_FTL     *p_nand_ftl,
                                                              FS_NAND_BLK_QTY tbl_ix)
{
  FS_NAND_AVAIL_BLK_ENTRY entry;
  CPU_SIZE_T              metadata_offset;

  metadata_offset = tbl_ix * (sizeof(FS_NAND_BLK_QTY) + sizeof(FS_NAND_ERASE_QTY));
  metadata_offset += p_nand_ftl->MetaOffsetAvailBlkTbl;

  MEM_VAL_COPY_GET_INTU_LITTLE(&entry.BlkIxPhy,
                               &p_nand_ftl->MetaCache[metadata_offset],
                               sizeof(FS_NAND_BLK_QTY));

  metadata_offset += sizeof(FS_NAND_BLK_QTY);
  MEM_VAL_COPY_GET_INTU_LITTLE(&entry.EraseCnt,
                               &p_nand_ftl->MetaCache[metadata_offset],
                               sizeof(FS_NAND_ERASE_QTY));

  return (entry);
}

/****************************************************************************************************//**
 *                                       FS_NAND_FTL_AvailBlkTblEntryWr()
 *
 * @brief    Writes an entry to available blocks table.
 *
 * @param    p_nand_ftl  Pointer to NAND FTL.
 *
 * @param    tbl_ix      Index of entry to write.
 *
 * @param    entry       Entry to write in available blocks table.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NAND_FTL_AvailBlkTblEntryWr(FS_NAND_FTL             *p_nand_ftl,
                                           FS_NAND_BLK_QTY         tbl_ix,
                                           FS_NAND_AVAIL_BLK_ENTRY entry)
{
  CPU_SIZE_T metadata_offset;

  metadata_offset = tbl_ix * (sizeof(FS_NAND_BLK_QTY) + sizeof(FS_NAND_ERASE_QTY));
  metadata_offset += p_nand_ftl->MetaOffsetAvailBlkTbl;

  MEM_VAL_COPY_SET_INTU_LITTLE(&p_nand_ftl->MetaCache[metadata_offset],
                               &entry.BlkIxPhy,
                               sizeof(FS_NAND_BLK_QTY));

  metadata_offset += sizeof(FS_NAND_BLK_QTY);
  MEM_VAL_COPY_SET_INTU_LITTLE(&p_nand_ftl->MetaCache[metadata_offset],
                               &entry.EraseCnt,
                               sizeof(FS_NAND_ERASE_QTY));

  //                                                               Clear entry bit in avail blk tbl commit map.
  Bitmap_BitClr(p_nand_ftl->AvailBlkTblCommitMap, tbl_ix);
}
#endif

/****************************************************************************************************//**
 *                                       FS_NAND_FTL_AvailBlkTblFill()
 *
 * @brief    Fill the available block table until it contains at least min_nbr_entries.
 *
 * @param    p_nand_ftl              Pointer to NAND FTL.
 *
 * @param    nbr_entries_min         Minimum number of entries in the table after it's been filled.
 *
 * @param    pending_dirty_chk_en    Enable/disable pending check of dirty blocks.
 *
 * @param    p_err                   Error pointer.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static FS_NAND_BLK_QTY FS_NAND_FTL_AvailBlkTblFill(FS_NAND_FTL     *p_nand_ftl,
                                                   FS_NAND_BLK_QTY nbr_entries_min,
                                                   CPU_BOOLEAN     pending_dirty_chk_en,
                                                   RTOS_ERR        *p_err)
{
  FS_NAND_AVAIL_BLK_ENTRY entry;
  FS_NAND_BLK_QTY         entry_cnt;

  RTOS_ASSERT_CRITICAL_ERR_SET(nbr_entries_min <= p_nand_ftl->AvailBlkTblEntryCntMax,
                               *p_err, RTOS_ERR_ASSERT_CRITICAL_FAIL, 0u);

  //                                                               ----------- CNT ENTRIES IN AVAIL BLK TBL -----------
  entry_cnt = FS_NAND_FTL_AvailBlkTblEntryCnt(p_nand_ftl);

  //                                                               --------------------- FILL TBL ---------------------
  while (entry_cnt < nbr_entries_min) {
    //                                                             Get dirty blk.
    entry.BlkIxPhy = FS_NAND_FTL_BlkGetDirty(p_nand_ftl, pending_dirty_chk_en, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (entry_cnt);
    }

    //                                                             Add blk to avail blk tbl.
    FS_NAND_FTL_BlkUnmap(p_nand_ftl, entry.BlkIxPhy);

    FS_NAND_FTL_BlkAddToAvail(p_nand_ftl, entry.BlkIxPhy, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      LOG_ERR(("Could not add dirty block to available blocks table. "
               "This block's erase count might be lost in the event of a power-loss."));
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    }

    entry_cnt++;
  }

  return (entry_cnt);
}
#endif

/****************************************************************************************************//**
 *                                       FS_NAND_FTL_AvailBlkTblEntryCnt()
 *
 * @brief    Counts the number of entries in the available blocks table.
 *
 * @param    p_nand_ftl  Pointer to NAND FTL.
 *
 * @return   number of available blocks in the available blocks table.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static FS_NAND_BLK_QTY FS_NAND_FTL_AvailBlkTblEntryCnt(FS_NAND_FTL *p_nand_ftl)
{
  FS_NAND_AVAIL_BLK_ENTRY entry;
  FS_NAND_BLK_QTY         tbl_ix;
  FS_NAND_BLK_QTY         entry_cnt;

  //                                                               ----------- CNT ENTRIES IN AVAIL BLK TBL -----------
  entry_cnt = 0u;
  for (tbl_ix = 0; tbl_ix < p_nand_ftl->AvailBlkTblEntryCntMax; tbl_ix++) {
    entry = FS_NAND_FTL_AvailBlkTblEntryRd(p_nand_ftl, tbl_ix);

    if (entry.BlkIxPhy != FS_NAND_BLK_IX_INVALID) {
      entry_cnt++;
    }
  }

  return (entry_cnt);
}
#endif

/****************************************************************************************************//**
 *                                       FS_NAND_FTL_UB_TblEntryRd()
 *
 * @brief    Reads an entry from update block table.
 *
 * @param    p_nand_ftl  Pointer to NAND FTL.
 *
 * @param    tbl_ix      Index of entry to read.
 *
 * @return   Structure containing update block's physical index and its associated valid sector map.
 *******************************************************************************************************/
static FS_NAND_UB_DATA FS_NAND_FTL_UB_TblEntryRd(FS_NAND_FTL    *p_nand_ftl,
                                                 FS_NAND_UB_QTY tbl_ix)
{
  FS_NAND_UB_DATA entry;
  CPU_SIZE_T      metadata_offset;

  metadata_offset = FS_UTIL_BIT_NBR_TO_OCTET_NBR(p_nand_ftl->NbrSecPerBlk);
  metadata_offset += sizeof(FS_NAND_BLK_QTY);
  metadata_offset *= tbl_ix;
  metadata_offset += p_nand_ftl->MetaOffsetUB_Tbl;

  MEM_VAL_COPY_GET_INTU_LITTLE(&entry.BlkIxPhy,
                               &p_nand_ftl->MetaCache[metadata_offset],
                               sizeof(FS_NAND_BLK_QTY));

  metadata_offset += sizeof(FS_NAND_BLK_QTY);
  entry.SecValidBitMap = &p_nand_ftl->MetaCache[metadata_offset];

  return (entry);
}

/****************************************************************************************************//**
 *                                       FS_NAND_FTL_UB_TblEntryWr()
 *
 * @brief    Writes an entry to update block table.
 *
 * @param    p_nand_ftl  Pointer to NAND FTL.
 *
 * @param    tbl_ix      Index of entry to read.
 *
 * @param    blk_ix_phy  Physical block index of new update block.
 *
 * @note     (1) The sector valid map may be modified by using FS_NAND_UB_TblEntryRd() and
 *               modifying data pointed by returned field sec_valid_map.
 *
 * @note     (2) After modifying the UB (update block) table, the function must invalidate it.
 *               See FS_NAND_UB_TblInvalidate note #1.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NAND_FTL_UB_TblEntryWr(FS_NAND_FTL     *p_nand_ftl,
                                      FS_NAND_UB_QTY  tbl_ix,
                                      FS_NAND_BLK_QTY blk_ix_phy)
{
  CPU_SIZE_T metadata_offset;

  metadata_offset = FS_UTIL_BIT_NBR_TO_OCTET_NBR(p_nand_ftl->NbrSecPerBlk);
  metadata_offset += sizeof(FS_NAND_BLK_QTY);
  metadata_offset *= tbl_ix;
  metadata_offset += p_nand_ftl->MetaOffsetUB_Tbl;

  MEM_VAL_COPY_SET_INTU_LITTLE(&p_nand_ftl->MetaCache[metadata_offset],
                               &blk_ix_phy,
                               sizeof(FS_NAND_BLK_QTY));

  FS_NAND_FTL_UB_TblInvalidate(p_nand_ftl);                     // Invalidate tbl (See note #2).
}
#endif

/****************************************************************************************************//**
 *                                           FS_NAND_FTL_UB_Create()
 *
 * @brief    Create an update block.
 *
 * @param    p_nand_ftl  Pointer to NAND FTL.
 *
 * @param    ub_ix       Index of new update block.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NAND_FTL_UB_Create(FS_NAND_FTL    *p_nand_ftl,
                                  FS_NAND_UB_QTY ub_ix,
                                  RTOS_ERR       *p_err)
{
  FS_NAND_BLK_QTY blk_ix_phy;

  //                                                               ------------------ GET ERASED BLK ------------------
  blk_ix_phy = FS_NAND_FTL_BlkGetErased(p_nand_ftl,
                                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Unable to get an erased block."));
    return;
  }

  //                                                               ----------------- UPDATE METADATA ------------------
  FS_NAND_FTL_UB_TblEntryWr(p_nand_ftl,
                            ub_ix,
                            blk_ix_phy);

  LOG_VRB(("Create UB ", (u)ub_ix, " at phy ix ", (u)blk_ix_phy));
}
#endif

/****************************************************************************************************//**
 *                                           FS_NAND_FTL_UB_Load()
 *
 * @brief    Loads metadata structures associated with specified update block.
 *
 * @param    p_nand_ftl  Pointer to NAND FTL.
 *
 * @param    blk_ix_phy  Physical block index of new update block.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_NAND_FTL_UB_Load(FS_NAND_FTL     *p_nand_ftl,
                                FS_NAND_BLK_QTY blk_ix_phy,
                                RTOS_ERR        *p_err)
{
  CPU_INT08U              *p_oos_buf;
  FS_NAND_UB_QTY          ub_ix;
  FS_NAND_UB_DATA         entry;
  FS_NAND_UB_EXTRA_DATA   *p_entry_extra = DEF_NULL;
  FS_NAND_SEC_PER_BLK_QTY sec_offset_phy;
  FS_NAND_BLK_QTY         blk_ix_logical;
  FS_NAND_SEC_PER_BLK_QTY sec_offset_logical;
  CPU_BOOLEAN             sec_valid;
  CPU_BOOLEAN             sec_is_used;
  FS_LB_QTY               sec_ix_base;
  CPU_INT08U              associated_blk_tbl_ix;
  CPU_SIZE_T              ub_sec_map_loc_octet_array;
#if (FS_NAND_CFG_UB_TBL_SUBSET_SIZE != 0u)
  CPU_SIZE_T ub_sec_map_pos_bit_array;
#endif
  CPU_DATA ub_sec_map_loc_bit_octet;
#if (FS_NAND_CFG_UB_META_CACHE_EN == DEF_ENABLED)
  CPU_SIZE_T ub_meta_pos_bit_array;
  CPU_SIZE_T ub_meta_loc_octet_array = 0u;
  CPU_DATA   ub_meta_loc_bit_octet = 0u;
#endif
  CPU_INT08U  rd_retries;
  CPU_BOOLEAN blk_is_sequential;
  CPU_BOOLEAN entry_found;
#if (FS_NAND_CFG_UB_TBL_SUBSET_SIZE != 0u)
  FS_NAND_SEC_PER_BLK_QTY blk_sec_subset_ix;
#endif

  p_oos_buf = (CPU_INT08U *)p_nand_ftl->OOS_BufPtr;

  //                                                               ---------------- FIND ENTRY IN TBL -----------------
  entry_found = DEF_NO;
  ub_ix = 0u;
  while ((entry_found != DEF_YES)
         && (ub_ix < p_nand_ftl->UB_CntMax)) {
    //                                                             Rd UB tbl entry.
    entry = FS_NAND_FTL_UB_TblEntryRd(p_nand_ftl, ub_ix);

    if (entry.BlkIxPhy == blk_ix_phy) {
      //                                                           Rd extra data tbl.
      p_entry_extra = &p_nand_ftl->UB_ExtraDataTbl[ub_ix];
      entry_found = DEF_YES;
    } else {
      ub_ix++;
    }
  }

  if (entry_found != DEF_YES) {                                 // Blk wasn't found.
    RTOS_ERR_SET(*p_err, RTOS_ERR_BLK_DEV_CORRUPTED);
    return;
  }

  //                                                               Inits ptrs for UB meta info.
  ub_sec_map_loc_octet_array = 0u;
  ub_sec_map_loc_bit_octet = 0u;

  sec_ix_base = FS_NAND_BLK_IX_TO_SEC_IX(p_nand_ftl, blk_ix_phy);
  blk_is_sequential = DEF_YES;

  //                                                               ------------ SCAN TO FIND LAST SEC USED ------------
  sec_offset_phy = p_nand_ftl->NbrSecPerBlk;
  while ((p_entry_extra->NextSecIx == 0u)
         && (sec_offset_phy != 0u)) {
    sec_offset_phy--;

    sec_is_used = FS_NAND_FTL_SecIsUsed(p_nand_ftl, sec_ix_base + sec_offset_phy, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      LOG_ERR(("Fatal error determining if sector ", (u)sec_ix_base + sec_offset_phy, " is used."));
      return;
    }

    if (sec_is_used == DEF_YES) {
      p_entry_extra->NextSecIx = sec_offset_phy + 1u;
    }
  }

  //                                                               --------------- SCAN SEC TO GET INFO ---------------
  for (sec_offset_phy = 0u; sec_offset_phy < p_nand_ftl->NbrSecPerBlk; sec_offset_phy++) {
    sec_valid = Bitmap_BitIsSet(entry.SecValidBitMap, sec_offset_phy);
    //                                                             If sec is valid ...
    if (sec_valid == DEF_YES) {
      //                                                           ... rd sec & retry if it fails.
      rd_retries = 0u;

      do {
        RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

        FS_NAND_FTL_SecRdPhyNoRefresh(p_nand_ftl,
                                      p_nand_ftl->BufPtr,
                                      p_oos_buf,
                                      blk_ix_phy,
                                      sec_offset_phy,
                                      p_err);

        rd_retries++;
      } while ((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_ECC_UNCORR)
               && (rd_retries < FS_NAND_CFG_MAX_RD_RETRIES));

      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return;
      }

      MEM_VAL_COPY_GET_INTU_LITTLE(&blk_ix_logical,             // Get blk ix logical for sec.
                                   &p_oos_buf[FS_NAND_OOS_STO_LOGICAL_BLK_IX_OFFSET],
                                   sizeof(FS_NAND_BLK_QTY));

      MEM_VAL_COPY_GET_INTU_LITTLE(&sec_offset_logical,         // Get sec ix logical for sec.
                                   &p_oos_buf[FS_NAND_OOS_STO_BLK_SEC_IX_OFFSET],
                                   sizeof(FS_NAND_SEC_PER_BLK_QTY));

      //                                                           Find logical blk ix in associated blk tbl.
      associated_blk_tbl_ix = 0u;
      while ((p_entry_extra->AssocLogicalBlksTbl[associated_blk_tbl_ix] != blk_ix_logical)
             && (associated_blk_tbl_ix < p_nand_ftl->RUB_MaxAssoc)                              ) {
        //                                                         If space is left, insert it.
        if (p_entry_extra->AssocLogicalBlksTbl[associated_blk_tbl_ix] == FS_NAND_BLK_IX_INVALID) {
          p_entry_extra->AssocLogicalBlksTbl[associated_blk_tbl_ix] = blk_ix_logical;
          p_entry_extra->AssocLvl++;
        } else {
          associated_blk_tbl_ix++;
        }
      }

      //                                                           Handle tbl overflow.
      if (associated_blk_tbl_ix > p_nand_ftl->RUB_MaxAssoc) {
        LOG_ERR(("Update block ", (u)blk_ix_phy, "associativity exceeds maximum value."));
        RTOS_ERR_SET(*p_err, RTOS_ERR_BLK_DEV_CORRUPTED);
        return;
      }

      //                                                           Update sec mapping tbl.
#if (FS_NAND_CFG_UB_TBL_SUBSET_SIZE != 0u)
      blk_sec_subset_ix = sec_offset_phy / FS_NAND_CFG_UB_TBL_SUBSET_SIZE;

      //                                                           Calc pos in tbl.
      ub_sec_map_pos_bit_array = (associated_blk_tbl_ix * p_nand_ftl->NbrSecPerBlk) + sec_offset_logical;
      ub_sec_map_pos_bit_array *= p_nand_ftl->UB_SecMapNbrBits;

      FS_UTIL_BITMAP_LOC_GET(ub_sec_map_pos_bit_array, ub_sec_map_loc_octet_array, ub_sec_map_loc_bit_octet);

      if (sec_offset_logical != FS_NAND_SEC_OFFSET_IX_INVALID) {
        FSUtil_ValPack32(&p_entry_extra->LogicalToPhySecMap[0],
                         &ub_sec_map_loc_octet_array,
                         &ub_sec_map_loc_bit_octet,
                         blk_sec_subset_ix,
                         p_nand_ftl->UB_SecMapNbrBits);
      } else {
        LOG_ERR(("Sector with physical offset ", (u)sec_offset_phy,
                 "of update block ", (u)ub_ix, " has invalid logical block sector index."));
      }
#endif
      //                                                           Update UB meta cache.
#if (FS_NAND_CFG_UB_META_CACHE_EN == DEF_ENABLED)

      FSUtil_ValPack32(&p_entry_extra->MetaCachePtr[0],
                       &ub_meta_loc_octet_array,
                       &ub_meta_loc_bit_octet,
                       sec_offset_logical,
                       p_nand_ftl->NbrSecPerBlkLog2);

      FSUtil_ValPack32(&p_entry_extra->MetaCachePtr[0],
                       &ub_meta_loc_octet_array,
                       &ub_meta_loc_bit_octet,
                       associated_blk_tbl_ix,
                       p_nand_ftl->RUB_MaxAssocLog2);

#endif

      if (sec_offset_logical != sec_offset_phy) {
        blk_is_sequential = DEF_NO;
      }
    } else {
      blk_is_sequential = DEF_NO;                               // SUB can't have any invalid sec.

#if (FS_NAND_CFG_UB_META_CACHE_EN == DEF_ENABLED)
      //                                                           Update UB meta cache ptrs.
      ub_meta_pos_bit_array = ub_meta_loc_octet_array << DEF_OCTET_TO_BIT_SHIFT;
      ub_meta_pos_bit_array += ub_meta_loc_bit_octet;
      ub_meta_pos_bit_array += (p_nand_ftl->NbrSecPerBlkLog2 + p_nand_ftl->RUB_MaxAssocLog2);

      FS_UTIL_BITMAP_LOC_GET(ub_meta_pos_bit_array, ub_meta_loc_octet_array, ub_meta_loc_bit_octet);
#endif
    }
  }

  p_entry_extra->ActivityCtr = 0u;

  //                                                               ---------------- CHK IF UB IS A SUB ----------------
  if ((blk_is_sequential == DEF_YES)
      && (p_entry_extra->AssocLvl == 1u)) {
    p_entry_extra->AssocLvl = 0u;                               // Make it a SUB.

    p_nand_ftl->SUB_Cnt++;
  }
}

/****************************************************************************************************//**
 *                                           FS_NAND_FTL_UB_Clr()
 *
 * @brief    Removes specified update block from update block tables and clears associated data
 *           structures.
 *
 * @param    p_nand_ftl  Pointer to NAND FTL.
 *
 * @param    ub_ix       Index of update block.
 *
 * @note     (1) Not all fields of the update block extra data (FS_NAND_UB_EXTRA_DATA) must
 *               be cleared. Pointer to buffers need to be maintained. However, the associated
 *               logical blocks table (AssocLogicalBlkTbl) entries must be set to their maximum value,
 *               which means they are invalid. The driver must check for those entries if the data
 *               is valid or not.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NAND_FTL_UB_Clr(FS_NAND_FTL    *p_nand_ftl,
                               FS_NAND_UB_QTY ub_ix)
{
  FS_NAND_UB_DATA       ub_data;
  FS_NAND_UB_EXTRA_DATA *p_ub_extra_data;
  CPU_SIZE_T            size;
  CPU_DATA              ix;

  LOG_VRB(("Clearing update blk ", (u)ub_ix));

  //                                                               --------------------- UB DATA ----------------------
  ub_data = FS_NAND_FTL_UB_TblEntryRd(p_nand_ftl, ub_ix);

  size = FS_UTIL_BIT_NBR_TO_OCTET_NBR(p_nand_ftl->NbrSecPerBlk);
  Mem_Clr(&ub_data.SecValidBitMap[0], size);

  FS_NAND_FTL_UB_TblEntryWr(p_nand_ftl, ub_ix, FS_NAND_BLK_IX_INVALID);

  //                                                               ------------------ UB EXTRA DATA -------------------
  p_ub_extra_data = &p_nand_ftl->UB_ExtraDataTbl[ub_ix];

  p_ub_extra_data->AssocLvl = 0u;                               // Not all fields must be cleared (see #note #1).
  p_ub_extra_data->NextSecIx = 0u;
  p_ub_extra_data->ActivityCtr = 0u;

  for (ix = 0; ix < p_nand_ftl->RUB_MaxAssoc; ix++) {
    p_ub_extra_data->AssocLogicalBlksTbl[ix] = FS_NAND_BLK_IX_INVALID;
  }
}
#endif

/****************************************************************************************************//**
 *                                           FS_NAND_FTL_UB_Find()
 *
 * @brief    Find update block associated with specified logical block index.
 *
 * @param    p_nand_ftl      Pointer to NAND FTL.
 *
 * @param    blk_ix_logical  Index of logical block.
 *
 * @return   Index of update block associated with blk_ix_logical,   if found,
 *           FS_NAND_BLK_IX_INVALID,                                 otherwise.
 *******************************************************************************************************/
static FS_NAND_UB_SEC_DATA FS_NAND_FTL_UB_Find(FS_NAND_FTL     *p_nand_ftl,
                                               FS_NAND_BLK_QTY blk_ix_logical)
{
  FS_NAND_UB_QTY        ub_ix;
  FS_NAND_UB_EXTRA_DATA ub_extra_data;
  FS_NAND_UB_SEC_DATA   ub_sec_data;
  FS_NAND_ASSOC_BLK_QTY assoc_blk_ix;

  for (ub_ix = 0u; ub_ix < p_nand_ftl->UB_CntMax; ub_ix++) {
    ub_extra_data = p_nand_ftl->UB_ExtraDataTbl[ub_ix];

    for (assoc_blk_ix = 0u; assoc_blk_ix < p_nand_ftl->RUB_MaxAssoc; assoc_blk_ix++) {
      if (ub_extra_data.AssocLogicalBlksTbl[assoc_blk_ix] == blk_ix_logical) {
        ub_sec_data.AssocLogicalBlksTblIx = assoc_blk_ix;
        ub_sec_data.UB_Ix = ub_ix;
        return (ub_sec_data);
      }
    }
  }

  ub_sec_data.UB_Ix = FS_NAND_UB_IX_INVALID;
  return (ub_sec_data);
}

/****************************************************************************************************//**
 *                                           FS_NAND_FTL_UB_SecFind()
 *
 * @brief    Finds latest version of specified sector in update block.
 *
 * @param    p_nand_ftl          Pointer to NAND FTL.
 *
 * @param    ub_sec_data         Struct containing index of update block and associated block index.
 *
 * @param    sec_offset_logical  Sector offset relative to logical block.
 *
 * @param    p_err               Error pointer.
 *
 * @return   ub_sec_data copy with .SecOffsetPhy set to: Offset of sector in update block, if found,
 *                                                       FS_NAND_SEC_OFFSET_IX_INVALID,    otherwise.
 *
 * @note     (1) Even if the sector was not found, p_err will not be set. Caller has to check return
 *               value.
 *
 * @note     (2) This function assumes that 'ub_sec_data' contains a valid index of update block
 *               and a valid index of the logical block in the associated blocks table. The function
 *               will then find where the logical sector in located, and set it in the return
 *               FS_NAND_UB_SEC_DATA structure.
 *******************************************************************************************************/
static FS_NAND_UB_SEC_DATA FS_NAND_FTL_UB_SecFind(FS_NAND_FTL             *p_nand_ftl,
                                                  FS_NAND_UB_SEC_DATA     ub_sec_data,
                                                  FS_NAND_SEC_PER_BLK_QTY sec_offset_logical,
                                                  RTOS_ERR                *p_err)
{
  FS_NAND_UB_EXTRA_DATA   ub_extra_data;
  FS_NAND_UB_DATA         ub_data;
  FS_NAND_SEC_PER_BLK_QTY range_begin;
  FS_NAND_SEC_PER_BLK_QTY range_end;
  FS_NAND_SEC_PER_BLK_QTY sec_offset_phy;
  FS_NAND_SEC_PER_BLK_QTY sec_offset_rd;
  CPU_SIZE_T              loc_octet_array;
  CPU_DATA                loc_bit_octet;
  CPU_BOOLEAN             sec_valid;
#if (FS_NAND_CFG_UB_TBL_SUBSET_SIZE != 0)
  FS_NAND_SEC_PER_BLK_QTY subset_ix;
#endif
#if ((FS_NAND_CFG_UB_META_CACHE_EN == DEF_ENABLED) || (FS_NAND_CFG_UB_TBL_SUBSET_SIZE != 0))
  CPU_SIZE_T pos_bit_array;
#endif
#if (FS_NAND_CFG_UB_META_CACHE_EN == DEF_ENABLED)
  FS_NAND_ASSOC_BLK_QTY assoc_blk_ix_rd;
#else
  FS_NAND_BLK_QTY blk_ix_logical_rd;
  FS_NAND_BLK_QTY blk_ix_logical;
  CPU_INT08U      *p_oos_buf;
#endif

#if (FS_NAND_CFG_UB_META_CACHE_EN != DEF_ENABLED)
  p_oos_buf = (CPU_INT08U *)p_nand_ftl->OOS_BufPtr;
#else
  PP_UNUSED_PARAM(p_err);
#endif

  ub_extra_data = p_nand_ftl->UB_ExtraDataTbl[ub_sec_data.UB_Ix];
  ub_data = FS_NAND_FTL_UB_TblEntryRd(p_nand_ftl, ub_sec_data.UB_Ix);

  if (ub_extra_data.AssocLvl == 0) {                            // ----------------------- SUB ------------------------
    sec_offset_phy = sec_offset_logical;                        // In a SUB, logical offset is equal to phy offset.
                                                                // Chk if SUB sec has been wr'en.
    if (sec_offset_phy < ub_extra_data.NextSecIx) {
      FS_UTIL_BITMAP_LOC_GET(sec_offset_phy, loc_octet_array, loc_bit_octet);
      //                                                           Check if sec is still valid.
      if (DEF_BIT_IS_SET(ub_data.SecValidBitMap[loc_octet_array], DEF_BIT(loc_bit_octet)) == DEF_YES) {
        ub_sec_data.SecOffsetPhy = sec_offset_phy;
        return (ub_sec_data);
      } else {
        ub_sec_data.SecOffsetPhy = FS_NAND_SEC_OFFSET_IX_INVALID;
        return (ub_sec_data);
      }
    } else {
      ub_sec_data.SecOffsetPhy = FS_NAND_SEC_OFFSET_IX_INVALID;
      return (ub_sec_data);
    }
  } else {                                                      // ----------------------- RUB ------------------------
                                                                // ------ RUB: DETERMINE SRCH RANGE FROM SUBSET -------
                                                                // See ' NAND FTL PARAMETERS DEFINES' Note #2.
#if (FS_NAND_CFG_UB_TBL_SUBSET_SIZE != 0)
    pos_bit_array = ub_sec_data.AssocLogicalBlksTblIx;
    pos_bit_array *= p_nand_ftl->NbrSecPerBlk;
    pos_bit_array *= p_nand_ftl->UB_SecMapNbrBits;
    pos_bit_array += (CPU_INT32U)sec_offset_logical * p_nand_ftl->UB_SecMapNbrBits;

    FS_UTIL_BITMAP_LOC_GET(pos_bit_array, loc_octet_array, loc_bit_octet);

    //                                                             Unpack subset ix from map.
    subset_ix = FSUtil_ValUnpack32(ub_extra_data.LogicalToPhySecMap,
                                   &loc_octet_array,
                                   &loc_bit_octet,
                                   p_nand_ftl->UB_SecMapNbrBits);

    //                                                             Calc range from subset ix.
    range_begin = subset_ix   * FS_NAND_CFG_UB_TBL_SUBSET_SIZE;
    range_end = range_begin + FS_NAND_CFG_UB_TBL_SUBSET_SIZE - 1u;
#else
    range_begin = 0u;
    range_end = p_nand_ftl->NbrSecPerBlk - 1u;
#endif
    //                                                             Limit range to wr'en sec in RUB.
    if (ub_extra_data.NextSecIx == 0u) {
      range_end = 0u;
    } else {
      if (range_end >= ub_extra_data.NextSecIx) {
        range_end = ub_extra_data.NextSecIx - 1u;
      }
    }

    //                                                             -------------- RUB: SRCH SEC IN RANGE --------------
    sec_offset_phy = range_begin;
    while (sec_offset_phy <= range_end) {
      //                                                           Chk if sec is valid.
      sec_valid = Bitmap_BitIsSet(ub_data.SecValidBitMap, sec_offset_phy);

      if (sec_valid == DEF_YES) {                               // Sec is valid, may contain logical sec.
#if (FS_NAND_CFG_UB_META_CACHE_EN == DEF_ENABLED)               // Srch in meta cache.
        pos_bit_array = sec_offset_phy  * (p_nand_ftl->RUB_MaxAssocLog2 + p_nand_ftl->NbrSecPerBlkLog2);

        FS_UTIL_BITMAP_LOC_GET(pos_bit_array, loc_octet_array, loc_bit_octet);

        //                                                         Unpack sec offset.
        sec_offset_rd = FSUtil_ValUnpack32(ub_extra_data.MetaCachePtr,
                                           &loc_octet_array,
                                           &loc_bit_octet,
                                           p_nand_ftl->NbrSecPerBlkLog2);

        //                                                         Unpack assoc blk ix.
        assoc_blk_ix_rd = FSUtil_ValUnpack32(ub_extra_data.MetaCachePtr,
                                             &loc_octet_array,
                                             &loc_bit_octet,
                                             p_nand_ftl->RUB_MaxAssocLog2);

        //                                                         Compare against srch'ed values.
        if ((sec_offset_rd == sec_offset_logical)
            && (assoc_blk_ix_rd == ub_sec_data.AssocLogicalBlksTblIx)) {
          ub_sec_data.SecOffsetPhy = sec_offset_phy;
          return (ub_sec_data);
        }
#else //                                                           Srch directly on device.
        blk_ix_logical = ub_extra_data.AssocLogicalBlksTbl[ub_sec_data.AssocLogicalBlksTblIx];

        FS_NAND_FTL_SecRdHandler(p_nand_ftl,                    // Rd sec.
                                 p_nand_ftl->BufPtr,
                                 p_nand_ftl->OOS_BufPtr,
                                 FS_NAND_UB_IX_TO_LOG_BLK_IX(p_nand_ftl, ub_sec_data.UB_Ix),
                                 sec_offset_phy,
                                 p_err);

        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          LOG_ERR(("FS_NAND_UB_SecFind: Unable to read sec ", (u)sec_offset_phy,
                   " in update block ", (u)ub_sec_data.UB_Ix, ".\r\n"));
          ub_sec_data.SecOffsetPhy = FS_NAND_SEC_OFFSET_IX_INVALID;
          return (ub_sec_data);
        }
        //                                                         Get blk sec ix.
        MEM_VAL_COPY_GET_INTU_LITTLE(&sec_offset_rd,
                                     &p_oos_buf[FS_NAND_OOS_STO_BLK_SEC_IX_OFFSET],
                                     sizeof(FS_NAND_SEC_PER_BLK_QTY));

        //                                                         Get blk ix logical.
        MEM_VAL_COPY_GET_INTU_LITTLE(&blk_ix_logical_rd,
                                     &p_oos_buf[FS_NAND_OOS_STO_LOGICAL_BLK_IX_OFFSET],
                                     sizeof(FS_NAND_BLK_QTY));

        //                                                         Compare against srch'ed values.
        if ((sec_offset_rd == sec_offset_logical)
            && (blk_ix_logical_rd == blk_ix_logical)) {
          ub_sec_data.SecOffsetPhy = sec_offset_phy;
          return (ub_sec_data);
        }
#endif
      }

      sec_offset_phy++;
    }
  }

  ub_sec_data.SecOffsetPhy = FS_NAND_SEC_OFFSET_IX_INVALID;     // Could not find sec.
  return (ub_sec_data);
}

/****************************************************************************************************//**
 *                                           FS_NAND_FTL_UB_IncAssoc()
 *
 * @brief    Associates update block with specified logical block.
 *
 * @param    p_nand_ftl      Pointer to NAND FTL.
 *
 * @param    ub_ix           Index of update block to associate with logical block.
 *
 * @param    blk_ix_logical  Index of logical block.
 *
 * @note     (1) Caller must ensure associativity value of update block is not maxed prior to call.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NAND_FTL_UB_IncAssoc(FS_NAND_FTL     *p_nand_ftl,
                                    FS_NAND_UB_QTY  ub_ix,
                                    FS_NAND_BLK_QTY blk_ix_logical)
{
  FS_NAND_UB_EXTRA_DATA *p_ub_extra_data;
  FS_NAND_ASSOC_BLK_QTY assoc_blk_ix;
  CPU_BOOLEAN           added;

  LOG_VRB(("Associate update blk ", (u)ub_ix, " with blk ix logical ", (u)blk_ix_logical));

  p_ub_extra_data = &p_nand_ftl->UB_ExtraDataTbl[ub_ix];

  //                                                               Ensure that assoc can be increased.
  if (p_ub_extra_data->AssocLvl >= p_nand_ftl->RUB_MaxAssoc) {
    LOG_ERR(("Fatal error. Can't increase associativity for update block ", (u)ub_ix, "."));
  } else {                                                      // Assoc can be increased.
    added = DEF_NO;
    assoc_blk_ix = 0u;
    while ((assoc_blk_ix < p_nand_ftl->RUB_MaxAssoc)
           && (added == DEF_NO)) {
      if (p_ub_extra_data->AssocLogicalBlksTbl[assoc_blk_ix] == FS_NAND_BLK_IX_INVALID) {
        p_ub_extra_data->AssocLogicalBlksTbl[assoc_blk_ix] = blk_ix_logical;
        p_ub_extra_data->AssocLvl++;
        added = DEF_YES;
      } else {
        if (p_ub_extra_data->AssocLogicalBlksTbl[assoc_blk_ix] == blk_ix_logical) {
          p_ub_extra_data->AssocLvl++;
          added = DEF_YES;                                      // Logical blk already assoc'd.
        }
      }

      assoc_blk_ix++;
    }

    if (added == DEF_NO) {
      LOG_ERR(("Fatal error: unable to associate logical block ", (u)blk_ix_logical, " with update block ", (u)ub_ix, "."));
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
    }
  }
}
#endif

/********************************************************************************************************
 *                                       FS_NAND_FTL_UB_Alloc()
 *
 * @brief        Allocates an update block.
 *
 * @param        p_nand_ftl      Pointer to NAND FTL.
 *
 * @param        sequential      DEF_YES, if update block will be a sequential update block,
 *                               DEF_NO , otherwise.
 *
 * @param        p_err           Error pointer.
 *
 * @return       Index of allocated update block,    if operation succeeded,
 *               FS_NAND_BLK_IX_INVALID,             otherwise.
 *
 * @note         (1) Flowchart for function is shown below. Block labels are shown in section comments.
 *               @verbatim
 *                                           +--------+
 *                                           |Alloc UB|
 *                                           +----+---+
 *                                               |
 *                                               |                    (A)
 *                                           /-----+-----\      +-----------------+
 *                                           |A full SUB |______| Merge SUB in DB |
 *                                           |  exists?  |Yes   |    Create UB    |
 *                                           \-----+-----/      +-----------------+
 *                                               |No
 *                                       /-------+------\     /--------------\              (F)
 *                                       |              |     |  sequential  |      +-----------------+
 *                                       |Empty UB Slot |-----|      ==      |---+--|    Create UB    |
 *                                       |              |Yes  |    DEF_NO    |Yes|  +-----------------+
 *                                       \-------+------/     \-------+------/   |
 *                                               |No                  |No        |
 *                                               |            /-------+------\   |
 *                                               |            | SUB cnt < max|---+
 *                                               |            \-------+------/Yes
 *                                               |                    |No
 *                                               +--------------------+
 *                                               |
 *                                               |                                          (B)
 *                                       /-------+------\     /----------------\    +-----------------+
 *                                       |  sequential  |     |  a RUB exists  |    |  Choose RUB to  |
 *                                       |      ==      |-----|   with k < K   |----|    associate    |
 *                                       |    DEF_NO    |Yes  |       ?        |Yes |     with LB     |
 *                                       \-------+------/     \----------------/    +-----------------+
 *                                               |No                  |No                   (C)
 *                                               |            /-------+--------\    +-------------------+
 *                                               |            |   a SUB with   |    |    Find SUB and   |
 *                                               |            |large free space|----|   Convert to RUB  |
 *                                               |            |    is idle ?   |Yes |see Notes #2a & #2b|
 *                                               |            \-------+--------/    +-------------------+
 *                                               |                    |No
 *                                               +--------------------+
 *                                               |
 *                       (E)                     |No                  (D)
 *       +----------------------+     /---------+---------\     +-----------------+
 *       | Find RUB w/ highest  |_____| SUB with few free |_____|    Find  SUB    |
 *       |      merge prio      |No   |  sec or no RUB?   |Yes  |    Merge SUB    |
 *       | Merge RUB, Create UB |     |   see Note #2c    |     |    Create UB    |
 *       +----------------------+     \-------------------/     +-----------------+
 *               @endverbatim
 * @note         (2) Various thresholds must be set. These thresholds can however be set according to
 *                   application specifics to obtain best performance. Indications are mentioned for every
 *                   threshold (in configuration file fs_nand_ftl_priv.h). These indications are guidelines
 *                   and specific cases could lead to different behaviors than what is expected.
 *                   - (a) FS_NAND_TH_SUB_MIN_IDLE_TO_FOLD (see C)
 *                         This threshold indicates the minimum idle time (specified as the number of
 *                         driver accesses since the last access that has written to the SUB) for a
 *                         sequential update blocks (SUB) to be converted to a random update block (RUB).
 *                         This threshold must be set so that 'hot' SUBs are not converted to RUBs.
 *                         This threshold can be set (in driver write operations) in fs_nand_ftl_priv.h :
 *                               @verbatim
 *                                 #define FS_NAND_TH_SUB_MIN_IDLE_TO_FOLD
 *                               @endverbatim
 *                   - (b) ThSecRemCnt_ConvertSUBToRUB (see C).
 *                         This threshold indicates the minimum size (in sectors) of free space needed in a
 *                         sequential update block (SUB) to convert it to a random update block (RUB). RUBs
 *                         have more flexible write rules, at the expense of a longer merge time. If the
 *                         SUB is near full (few free sectors remaining), the SUB will me merged and a new
 *                         RUB will be started, instead of performing the conversion from SUB to RUB.
 *                         This threshold can be set as a percentage  (relative to number of sectors per
 *                         block) in fs_nand_ftl_priv.h :
 *                             @verbatim
 *                             #define FS_NAND_TH_PCT_CONVERT_SUB_TO_RUB
 *                             @endverbatim
 *                         Set higher than default -> Better overall write speed
 *                         Set lower  than default -> Better overall wear leveling
 *
 *                   - (c) ThSecRemCnt_MergeSUB (see D)
 *                         This threshold indicates the maximum size (in sectors) of free space needed in a
 *                         sequential update block (SUB) to merge it to allocate another update block. If
 *                         the threshold is exceeded, a random update block (RUB) will be merged instead.
 *                         This threshold must be set so that SUBs with significant free space are not
 *                         merged. Merging SUBs early will generate additional wear.
 *
 *                         This threshold can be set as a percentage  (relative to number of sectors per
 *                         block) in fs_nand_ftl_priv.h :
 *                             @verbatim
 *                             #define FS_NAND_TH_PCT_MERGE_SUB
 *                             @endverbatim
 * @note         (3) When a random update block (RUB) must be merged, we choose the RUB with the highest
 *                   merge priority. Merge priority is calculated based on 2 criterias :
 *                   - (a) How full is the block. This is determinated by the next sector index (which
 *                                                points to the first writable sector of the block).
 *                   - (b) How idle is the block. This is determinated by the number of write cycles during
 *                                                which this update block was not used. To make it relative,
 *                                                it is divided by the total number of update blocks.
 *                   If the block is completely full, it will be granted the highest priority. If it is
 *                   not completely full, the priority will be calculated with this formula :
 *                           @verbatim
 *                           prio = next sector index + idle factor
 *                           @endverbatim
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static FS_NAND_UB_QTY FS_NAND_FTL_UB_Alloc(FS_NAND_FTL *p_nand_ftl,
                                           CPU_BOOLEAN sequential,
                                           RTOS_ERR    *p_err)
{
  FS_NAND_UB_QTY          ix_sub_full;
  FS_NAND_UB_QTY          ix_sub_idlest;
  FS_NAND_UB_QTY          ix_sub_fullest;
  FS_NAND_UB_QTY          ix_rub_priority;
  FS_NAND_UB_QTY          ix_blk_lowest_assoc;
  FS_NAND_UB_QTY          ix_blk_erased;
  FS_NAND_UB_QTY          ub_ix;
  FS_NAND_UB_EXTRA_DATA   ub_extra_data;
  CPU_BOOLEAN             early_exit;
  FS_NAND_SEC_PER_BLK_QTY min_free_sec_cnt;
  CPU_INT16U              max_idle_val;
  CPU_INT16U              idle_val;
  CPU_INT08U              min_assoc;
  FS_NAND_SEC_PER_BLK_QTY min_assoc_free_sec_cnt;
  FS_NAND_SEC_PER_BLK_QTY free_sec_cnt;
  CPU_INT32U              rub_merge_prio;
  CPU_INT32U              max_rub_merge_prio;

  //                                                               -------------------- INIT VARs ---------------------
  //                                                               Init max_idle_val to the threshold (see Note #2).
  max_idle_val = FS_NAND_TH_SUB_MIN_IDLE_TO_FOLD;
  min_free_sec_cnt = FS_NAND_SEC_OFFSET_IX_INVALID;
  ix_sub_full = FS_NAND_UB_IX_INVALID;
  ix_blk_lowest_assoc = FS_NAND_UB_IX_INVALID;
  ix_sub_idlest = FS_NAND_UB_IX_INVALID;
  ix_sub_fullest = FS_NAND_UB_IX_INVALID;
  ub_ix = FS_NAND_UB_IX_INVALID;
  ix_blk_erased = FS_NAND_UB_IX_INVALID;
  ix_rub_priority = FS_NAND_UB_IX_INVALID;
  min_assoc = p_nand_ftl->RUB_MaxAssoc;
  min_assoc_free_sec_cnt = FS_NAND_SEC_OFFSET_IX_INVALID;
  max_rub_merge_prio = 0u;

  //                                                               ------------------- SCAN ALL UBs -------------------
  ub_ix = 0u;
  early_exit = DEF_NO;
  while ((ub_ix < p_nand_ftl->UB_CntMax)
         && (early_exit == DEF_NO)) {
    ub_extra_data = p_nand_ftl->UB_ExtraDataTbl[ub_ix];

    if (ub_extra_data.NextSecIx != 0u) {                        // Used blks.
                                                                // Determine idle cnt.
      if (ub_extra_data.ActivityCtr > p_nand_ftl->ActivityCtr) {
        //                                                         Wrapped around.
        idle_val = DEF_GET_U_MAX_VAL(idle_val) - ub_extra_data.ActivityCtr;
        idle_val += p_nand_ftl->ActivityCtr;
      } else {
        //                                                         No wrap around.
        idle_val = p_nand_ftl->ActivityCtr - ub_extra_data.ActivityCtr;
      }

      if (ub_extra_data.AssocLvl == 0u) {                       // Blk is a SUB.
                                                                // ----------------- SUB: CHK IF FULL -----------------
        if (ub_extra_data.NextSecIx == p_nand_ftl->NbrSecPerBlk) {
          ix_sub_full = ub_ix;
          early_exit = DEF_YES;                                 // No need to continue scanning blks.
        }

        //                                                         ---------------- SUB: FREE SEC CNT -----------------
        free_sec_cnt = p_nand_ftl->NbrSecPerBlk - ub_extra_data.NextSecIx;
        if (free_sec_cnt < min_free_sec_cnt) {
          min_free_sec_cnt = free_sec_cnt;
          ix_sub_fullest = ub_ix;
        }

        //                                                         ------------------ SUB: IDLE CNT -------------------

        //                                                         Consider only spacious blks.
        if (free_sec_cnt < p_nand_ftl->ThSecRemCnt_ConvertSUBToRUB) {
          if (idle_val > max_idle_val) {
            //                                                     New idlest SUB.
            max_idle_val = idle_val;
            ix_sub_idlest = ub_ix;
          }
        }
      } else {                                                  // Blk is a RUB.
        free_sec_cnt = p_nand_ftl->NbrSecPerBlk - ub_extra_data.NextSecIx;

        //                                                         -------------------- RUB: ASSOC --------------------
        if (free_sec_cnt != 0u) {
          if ((ub_extra_data.AssocLvl < min_assoc)) {
            min_assoc_free_sec_cnt = p_nand_ftl->NbrSecPerBlk - ub_extra_data.NextSecIx;
            ix_blk_lowest_assoc = ub_ix;
          } else {
            if ((ub_extra_data.AssocLvl == min_assoc)
                && (ub_extra_data.AssocLvl < p_nand_ftl->RUB_MaxAssoc)) {
              //                                                   Chk if more sec are free in this UB.
              if (free_sec_cnt > min_assoc_free_sec_cnt) {
                min_assoc_free_sec_cnt = free_sec_cnt;
                ix_blk_lowest_assoc = ub_ix;
              }
            }
          }
        }

        //                                                         ----------------- RUB: MERGE PRIO ------------------
        //                                                         If RUB full ...
        if (ub_extra_data.NextSecIx - 1 >= p_nand_ftl->NbrSecPerBlk) {
          //                                                       ... set to highest merge prio ...
          rub_merge_prio = DEF_GET_U_MAX_VAL(rub_merge_prio);
        } else {
          //                                                       ... else, calc merge prio (see Note #3).
          rub_merge_prio = idle_val / p_nand_ftl->UB_CntMax;
          rub_merge_prio += ub_extra_data.NextSecIx;
        }

        if (rub_merge_prio > max_rub_merge_prio) {              // Get ix of RUB with highest prio.
          max_rub_merge_prio = rub_merge_prio;
          ix_rub_priority = ub_ix;
        }
      }
    } else {
      if ((sequential == DEF_NO)                                // Unused blk, unless SUB over max.
          || (p_nand_ftl->SUB_Cnt < p_nand_ftl->SUB_CntMax)) {
        ix_blk_erased = ub_ix;
        early_exit = DEF_YES;
      }
    }

    ub_ix++;
  }

  //                                                               ---------------- FULL SUB AVAIL (A) ----------------
  if (ix_sub_full != FS_NAND_UB_IX_INVALID) {
    //                                                             Convert SUB to DB.
    FS_NAND_FTL_SUB_Merge(p_nand_ftl,
                          ix_sub_full,
                          p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      LOG_ERR(("Error merging sequential update blk ", (u)ix_sub_full, "."));
      return (FS_NAND_UB_IX_INVALID);
    }

    //                                                             Create new UB.
    FS_NAND_FTL_UB_Create(p_nand_ftl,
                          ix_sub_full,
                          p_err);

    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      LOG_ERR(("Error creating random update block."));
      return (FS_NAND_UB_IX_INVALID);
    }

    return (ix_sub_full);
  }

  //                                                               ------------------ EMPTY SLOT (F) ------------------
  if (ix_blk_erased != FS_NAND_UB_IX_INVALID) {
    if ((sequential == DEF_NO)                                  // If alloc'ing RUB or (SUB with free SUB slots) ...
        || (p_nand_ftl->SUB_Cnt < p_nand_ftl->SUB_CntMax)) {
      //                                                           ... create new UB.
      FS_NAND_FTL_UB_Create(p_nand_ftl,
                            ix_blk_erased,
                            p_err);

      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        LOG_ERR(("Error creating random update block."));
        return (FS_NAND_UB_IX_INVALID);
      }

      return (ix_blk_erased);
    }
  }

  if (sequential == DEF_NO) {
    //                                                             -------------- RUB WITH k<K AVAIL (B) --------------
    if (ix_blk_lowest_assoc != FS_NAND_UB_IX_INVALID) {
      return (ix_blk_lowest_assoc);
    }

    //                                                             --------- IDLE AND SPACIOUS SUB AVAIL (C) ----------
    if (ix_sub_idlest != FS_NAND_UB_IX_INVALID) {
      ub_extra_data = p_nand_ftl->UB_ExtraDataTbl[ix_sub_idlest];

      FS_NAND_FTL_UB_IncAssoc(p_nand_ftl,                       // Convert SUB to RUB with k=1.
                              ix_sub_idlest,
                              ub_extra_data.AssocLogicalBlksTbl[0]);

      p_nand_ftl->SUB_Cnt--;

      return (ix_sub_idlest);
    }
  }

  //                                                               ---- SUB WITH FEW FREE SECS AVAIL OR NO RUB (D) ----
  if ((min_free_sec_cnt < p_nand_ftl->ThSecRemCnt_MergeSUB)
      || (ix_rub_priority == FS_NAND_UB_IX_INVALID)) {
    //                                                             Merge SUB.
    FS_NAND_FTL_SUB_Merge(p_nand_ftl,
                          ix_sub_fullest,
                          p_err);

    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      LOG_ERR(("Error during sequential block ", (u)ix_sub_fullest, " merge."));
      return (FS_NAND_UB_IX_INVALID);
    }

    //                                                             Create new UB.
    FS_NAND_FTL_UB_Create(p_nand_ftl,
                          ix_sub_fullest,
                          p_err);

    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      LOG_ERR(("Error creating update block ", (u)ix_sub_fullest, "."));
      return (FS_NAND_UB_IX_INVALID);
    }

    return (ix_sub_fullest);
  }

  //                                                               ------- NO SUB WITH FEW FREE SECS AVAIL (E) --------

  //                                                               Merge RUB.
  FS_NAND_FTL_RUB_Merge(p_nand_ftl,
                        ix_rub_priority,
                        p_err);

  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Error during update block ", (u)ix_rub_priority, " merge."));
    return (FS_NAND_UB_IX_INVALID);
  }

  //                                                               Create new UB.
  FS_NAND_FTL_UB_Create(p_nand_ftl,
                        ix_rub_priority,
                        p_err);

  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Error creating new update block."));
    return (FS_NAND_UB_IX_INVALID);
  }

  return (ix_rub_priority);
}
#endif

/****************************************************************************************************//**
 *                                           FS_NAND_FTL_RUB_Alloc()
 *
 * @brief    Allocates a random update block and associates it with specified logical block.
 *
 * @param    p_nand_ftl      Pointer to NAND FTL.
 *
 * @param    blk_ix_logical  Index of logical block.
 *
 * @param    p_err           Error pointer.
 *
 * @return   Index of allocated update block, if operation succeeded,
 *           FS_NAND_BLK_IX_INVALID,          otherwise.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static FS_NAND_UB_QTY FS_NAND_FTL_RUB_Alloc(FS_NAND_FTL     *p_nand_ftl,
                                            FS_NAND_BLK_QTY blk_ix_logical,
                                            RTOS_ERR        *p_err)
{
  FS_NAND_UB_QTY ub_ix;

  //                                                               Alloc new UB.
  ub_ix = FS_NAND_FTL_UB_Alloc(p_nand_ftl,
                               DEF_NO,
                               p_err);

  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (FS_NAND_UB_IX_INVALID);
  }

  //                                                               Associate UB with logical blk.
  FS_NAND_FTL_UB_IncAssoc(p_nand_ftl,
                          ub_ix,
                          blk_ix_logical);

  return (ub_ix);
}
#endif

/****************************************************************************************************//**
 *                                           FS_NAND_FTL_RUB_Merge()
 *
 * @brief    Perform a full merge of specified random update block with associated data block.
 *
 * @param    p_nand_ftl  Pointer to NAND FTL.
 *
 * @param    ub_ix       Index of logical block.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NAND_FTL_RUB_Merge(FS_NAND_FTL    *p_nand_ftl,
                                  FS_NAND_UB_QTY ub_ix,
                                  RTOS_ERR       *p_err)
{
  FS_NAND_UB_EXTRA_DATA ub_extra_data;
  FS_NAND_BLK_QTY       blk_ix_logical;
  CPU_INT08U            assoc_ix;

  FS_BLK_DEV_CTR_STAT_INC(p_nand_ftl->Ctrs.StatRUB_MergeCtr);

  LOG_VRB(("Full merge of RUB ", (u)ub_ix, "."));

  ub_extra_data = p_nand_ftl->UB_ExtraDataTbl[ub_ix];

  //                                                               ------------------ PERFORM MERGE -------------------

  //                                                               Merge 1 blk at a time until full merge is complete.
  for (assoc_ix = 0u; assoc_ix < p_nand_ftl->RUB_MaxAssoc; assoc_ix++) {
    blk_ix_logical = ub_extra_data.AssocLogicalBlksTbl[assoc_ix];

    if (blk_ix_logical != FS_NAND_BLK_IX_INVALID) {
      FS_NAND_FTL_RUB_PartialMerge(p_nand_ftl,
                                   ub_ix,
                                   ub_extra_data.AssocLogicalBlksTbl[assoc_ix],
                                   p_err);

      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        LOG_ERR(("Error during partial merge of associated block ", (u)assoc_ix, "."));
        return;
      }
    }
  }
}
#endif

/****************************************************************************************************//**
 *                                       FS_NAND_FTL_RUB_PartialMerge()
 *
 * @brief    Perform a merge of the specified data block associated with the random update block (RUB).
 *           See Note #1.
 *
 * @param    p_nand_ftl              Pointer to NAND FTL.
 *
 * @param    ub_ix                   Index of update block.
 *                                   Argument validated by caller (see Note #4).
 *
 * @param    data_blk_ix_logical     Index of associated logical block in associated block table.
 *
 * @param    p_err                   Error pointer.
 *
 * @note     (1) A random update block (RUB) is often associated with multiple logical data blocks.
 *               A partial merge consists to only merge the sectors belonging to one data block with
 *               the data block itself. This will also decrease the associativity level of the RUB.
 *
 * @note     (2) The new data block (data_blk_ix_phy_new) will be created from the merged content of
 *               2 blocks :
 *               - the old data block (data_blk_ix_phy_old);
 *               - the update   block (ub_ix).
 *               This function will evaluate, for each sector, which of the 2 blocks contains the
 *               latest valid version of the sector.
 *
 * @note     (3) To revert the operation, this function assumes that 'data_blk_ix_phy_old' has not
 *               been changed since its first and only assignation.
 *
 * @note     (4) Caller must validate 'ub_ix' to make sure it is a random update block (RUB),
 *               and not a sequential update block (SUB).
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NAND_FTL_RUB_PartialMerge(FS_NAND_FTL     *p_nand_ftl,
                                         FS_NAND_UB_QTY  ub_ix,
                                         FS_NAND_BLK_QTY data_blk_ix_logical,
                                         RTOS_ERR        *p_err)
{
  FS_NAND_UB_DATA         ub_data;
  FS_NAND_UB_EXTRA_DATA   *p_ub_extra_data;
  FS_NAND_UB_SEC_DATA     ub_sec_data;
  FS_NAND_SEC_PER_BLK_QTY sec_offset_logical;
  FS_NAND_SEC_PER_BLK_QTY sec_offset_logical_oos;
  FS_NAND_SEC_PER_BLK_QTY sec_offset_phy;
  FS_NAND_BLK_QTY         data_blk_ix_phy_old;
  FS_NAND_BLK_QTY         data_blk_ix_phy_new;
  FS_NAND_BLK_QTY         blk_ix_logical_src;
  FS_NAND_BLK_QTY         blk_ix_phy_src;
  FS_NAND_ASSOC_BLK_QTY   assoc_blk_ix;
  FS_LB_QTY               sec_ix_phy;
  CPU_BOOLEAN             is_sec_used;
  CPU_BOOLEAN             sec_found;
  CPU_INT08U              *p_oos_buf;

  FS_BLK_DEV_CTR_STAT_INC(p_nand_ftl->Ctrs.StatRUB_PartialMergeCtr);

  p_oos_buf = (CPU_INT08U *)p_nand_ftl->OOS_BufPtr;

  ub_data = FS_NAND_FTL_UB_TblEntryRd(p_nand_ftl, ub_ix);
  p_ub_extra_data = &p_nand_ftl->UB_ExtraDataTbl[ub_ix];
  data_blk_ix_phy_old = p_nand_ftl->LogicalToPhyBlkMap[data_blk_ix_logical];
  assoc_blk_ix = FS_NAND_FTL_RUB_AssocBlkIxGet(p_nand_ftl, ub_ix, data_blk_ix_logical);

  LOG_VRB(("Partial merge of assoc blk ", (u)assoc_blk_ix, "from RUB ", (u)ub_ix));

  if (p_ub_extra_data->AssocLvl == 0u) {
    LOG_ERR(("Fatal error: associativity level is 0, suggesting a SUB."));
    RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }

  //                                                               ------------- GET BLK FOR NEW DATA BLK -------------
  data_blk_ix_phy_new = FS_NAND_FTL_BlkGetErased(p_nand_ftl,
                                                 p_err);

  LOG_VRB(("Logical blk ", (u)data_blk_ix_logical, "will be merged to data blk ", (u)data_blk_ix_phy_new));

  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Unable to get an erased block for new data block."));
    return;
  }

  //                                                               --------- COPY ALL VALID SEC SEQUENTIALLY ----------
  ub_sec_data.UB_Ix = ub_ix;
  ub_sec_data.AssocLogicalBlksTblIx = assoc_blk_ix;
  for (sec_offset_logical = 0u; sec_offset_logical < p_nand_ftl->NbrSecPerBlk; sec_offset_logical++) {
    sec_found = DEF_NO;
    sec_offset_phy = FS_NAND_SEC_OFFSET_IX_INVALID;
    blk_ix_logical_src = FS_NAND_BLK_IX_INVALID;

    //                                                             Locate sec in UB.
    ub_sec_data = FS_NAND_FTL_UB_SecFind(p_nand_ftl,
                                         ub_sec_data,
                                         sec_offset_logical,
                                         p_err);

    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      LOG_ERR(("Error looking for updated sector ", (u)sec_offset_logical, " of logical block ", (u)data_blk_ix_logical, "."));
      return;
    }

    //                                                             Chk if sec was found in UB (see Note #2).
    if (ub_sec_data.SecOffsetPhy != FS_NAND_SEC_OFFSET_IX_INVALID) {
      //                                                           Set src blk to UB.
      blk_ix_logical_src = FS_NAND_UB_IX_TO_LOG_BLK_IX(p_nand_ftl, ub_ix);
      sec_offset_phy = ub_sec_data.SecOffsetPhy;
      sec_found = DEF_YES;

      //                                                           Remove from valid sec map.
      Bitmap_BitClr(ub_data.SecValidBitMap, sec_offset_phy);
    } else {
      if (data_blk_ix_phy_old != FS_NAND_BLK_IX_INVALID) {
        //                                                         Chk is sec is wr'en in data blk (see Note #2).
        sec_ix_phy = FS_NAND_BLK_IX_TO_SEC_IX(p_nand_ftl, data_blk_ix_phy_old);
        sec_ix_phy += sec_offset_logical;

        is_sec_used = FS_NAND_FTL_SecIsUsed(p_nand_ftl, sec_ix_phy, p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          LOG_ERR(("Error determining if data block sector ", (u)sec_ix_phy, " is used."));
          return;
        }

        if (is_sec_used == DEF_YES) {
          blk_ix_logical_src = data_blk_ix_logical;             // Set src blk to data blk.
          sec_offset_phy = sec_offset_logical;                  // Offset logical equal to offset phy for a data blk.
          sec_found = DEF_YES;
        }
      }
    }

    do {                                                        // Until sec has been successfully wr'en ...
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

      //                                                           If sec can be located, wr it in new data blk.
      if (sec_found == DEF_YES) {
        LOG_VRB(((u)blk_ix_logical_src, ":", (u)sec_offset_phy, "->", (u)sec_offset_logical, "."));

        //                                                         Rd sec from src blk.
        blk_ix_phy_src = FS_NAND_FTL_BlkIxPhyGet(p_nand_ftl, blk_ix_logical_src);
        FS_NAND_FTL_SecRdPhyNoRefresh(p_nand_ftl,
                                      p_nand_ftl->BufPtr,
                                      p_nand_ftl->OOS_BufPtr,
                                      blk_ix_phy_src,
                                      sec_offset_phy,
                                      p_err);

        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          LOG_ERR(("Error reading updated sector ", (u)sec_offset_logical, " for logical block ", (u)data_blk_ix_logical));
        }
      }

      if (sec_offset_logical == 0u) {                           // First sec, so recreate OOS (for erase cnt).
        if (sec_found == DEF_YES) {                             // Prev wr'en sec; preserve blk sec ix.
          MEM_VAL_COPY_GET_INTU_LITTLE(&sec_offset_logical_oos,
                                       &p_oos_buf[FS_NAND_OOS_STO_BLK_SEC_IX_OFFSET],
                                       sizeof(FS_NAND_SEC_PER_BLK_QTY));
        } else {                                                // Dummy sec.
          sec_offset_logical_oos = FS_NAND_SEC_OFFSET_IX_INVALID;
        }

        FS_NAND_FTL_OOSGenSto(p_nand_ftl,
                              p_nand_ftl->OOS_BufPtr,
                              data_blk_ix_logical,
                              data_blk_ix_phy_new,
                              sec_offset_logical_oos,
                              sec_offset_logical,
                              p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          return;
        }
        sec_found = DEF_YES;                                    // Even if sec not wr'en, create dummy sec.
      }

      if (sec_found == DEF_YES) {
        //                                                         Manipulate LogicalToPhyBlkMap to wr data to new  ...
        //                                                         ... data blk before it is added to the tbl.
        p_nand_ftl->LogicalToPhyBlkMap[data_blk_ix_logical] = data_blk_ix_phy_new;

        FS_NAND_FTL_SecWrHandler(p_nand_ftl,                    // Wr sec in new data blk.
                                 p_nand_ftl->BufPtr,
                                 p_nand_ftl->OOS_BufPtr,
                                 data_blk_ix_logical,
                                 sec_offset_logical,
                                 p_err);
        //                                                         Revert change in LogicalToPhyBlkMap (see Note #3).
        p_nand_ftl->LogicalToPhyBlkMap[data_blk_ix_logical] = data_blk_ix_phy_old;
      }
    } while (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_ABORT);

    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      LOG_ERR(("Error writing sec ", (u)sec_offset_logical, " to new data block."));
      return;
    }
  }

  //                                                               ------------------ UPDATE UB DATA ------------------
  p_ub_extra_data->AssocLogicalBlksTbl[assoc_blk_ix] = FS_NAND_BLK_IX_INVALID;
  p_ub_extra_data->AssocLvl--;

  if (p_ub_extra_data->AssocLvl == 0u) {                        // If UB becomes empty, ...
                                                                // ... mark UB dirty.
    FS_NAND_FTL_BlkMarkDirty(p_nand_ftl, ub_data.BlkIxPhy);

    FS_NAND_FTL_UB_Clr(p_nand_ftl, ub_ix);                      // Clr UB data.
  }

  //                                                               ----------------- UPDATE METADATA ------------------
  if (data_blk_ix_phy_old != FS_NAND_BLK_IX_INVALID) {
    FS_NAND_FTL_BlkMarkDirty(p_nand_ftl, data_blk_ix_phy_old);  // Mark old data blk dirty.
  }

  //                                                               Update logical to phy blk tbl.
  p_nand_ftl->LogicalToPhyBlkMap[data_blk_ix_logical] = data_blk_ix_phy_new;

  LOG_VRB(("Partial merge of assoc blk ", (u)assoc_blk_ix, " from RUB ", (u)ub_ix, " done."));
}
#endif

/****************************************************************************************************//**
 *                                       FS_NAND_FTL_RUB_AssocBlkIxGet()
 *
 * @brief    Get associated block index for specified logical block index in specified update block.
 *
 * @param    p_nand_ftl      Pointer to NAND FTL.
 *
 * @param    ub_ix           Index of update block.
 *
 * @param    blk_ix_logical  Index of logical block.
 *
 * @return   Index of allocated block,     if found;
 *           FS_NAND_ASSOC_BLK_IX_INVALID, otherwise.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static FS_NAND_ASSOC_BLK_QTY FS_NAND_FTL_RUB_AssocBlkIxGet(FS_NAND_FTL     *p_nand_ftl,
                                                           FS_NAND_UB_QTY  ub_ix,
                                                           FS_NAND_BLK_QTY blk_ix_logical)
{
  FS_NAND_ASSOC_BLK_QTY assoc_blk_ix;
  FS_NAND_UB_EXTRA_DATA ub_extra_data;

  assoc_blk_ix = 0u;
  ub_extra_data = p_nand_ftl->UB_ExtraDataTbl[ub_ix];
  while (assoc_blk_ix < p_nand_ftl->RUB_MaxAssoc) {
    if (ub_extra_data.AssocLogicalBlksTbl[assoc_blk_ix] == blk_ix_logical) {
      return (assoc_blk_ix);
    }

    assoc_blk_ix++;
  }

  //                                                               Logical blk not associated with UB.
  LOG_ERR(("Logical block index ", (u)blk_ix_logical, " is not associated with update block ", (u)ub_ix));
  return (FS_NAND_ASSOC_BLK_IX_INVALID);
}
#endif

/****************************************************************************************************//**
 *                                           FS_NAND_FTL_SUB_Alloc()
 *
 * @brief    Allocates a sequential update block and associate it with specified logical block.
 *
 * @param    p_nand_ftl      Pointer to NAND FTL.
 *
 * @param    blk_ix_logical  Index of logical block.
 *
 * @param    p_err           Error pointer.
 *
 * @return   Index of allocated update block,    if operation succeeded,
 *           FS_NAND_BLK_IX_INVALID,             otherwise.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static FS_NAND_UB_QTY FS_NAND_FTL_SUB_Alloc(FS_NAND_FTL     *p_nand_ftl,
                                            FS_NAND_BLK_QTY blk_ix_logical,
                                            RTOS_ERR        *p_err)
{
  FS_NAND_UB_QTY        ub_ix;
  FS_NAND_UB_EXTRA_DATA *p_extra_data;

  //                                                               Alloc new UB.
  ub_ix = FS_NAND_FTL_UB_Alloc(p_nand_ftl,
                               DEF_YES,
                               p_err);

  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (FS_NAND_UB_IX_INVALID);
  }

  //                                                               Associate UB with logical blk.
  FS_NAND_FTL_UB_IncAssoc(p_nand_ftl,
                          ub_ix,
                          blk_ix_logical);

  //                                                               Correct assoc for SUB.
  p_extra_data = &p_nand_ftl->UB_ExtraDataTbl[ub_ix];
  p_extra_data->AssocLvl = 0u;

  p_nand_ftl->SUB_Cnt++;                                        // Update SUB cnt.

  return (ub_ix);
}
#endif

/****************************************************************************************************//**
 *                                           FS_NAND_FTL_SUB_Merge()
 *
 * @brief    Merge specified sequential update block with associated data block.
 *
 * @param    p_nand_ftl  Pointer to NAND FTL.
 *
 * @param    sub_ix      Index of logical block.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NAND_FTL_SUB_Merge(FS_NAND_FTL    *p_nand_ftl,
                                  FS_NAND_UB_QTY sub_ix,
                                  RTOS_ERR       *p_err)
{
  FS_NAND_UB_EXTRA_DATA ub_extra_data;
  FS_NAND_UB_DATA       ub_data;
  FS_NAND_BLK_QTY       data_blk_ix_phy;
  FS_NAND_BLK_QTY       data_blk_ix_logical;

  FS_BLK_DEV_CTR_STAT_INC(p_nand_ftl->Ctrs.StatSUB_MergeCtr);

  LOG_VRB(("Merge SUB ", (u)sub_ix, "."));

  ub_data = FS_NAND_FTL_UB_TblEntryRd(p_nand_ftl, sub_ix);
  ub_extra_data = p_nand_ftl->UB_ExtraDataTbl[sub_ix];
  //                                                               Find associated logical blk.
  data_blk_ix_logical = ub_extra_data.AssocLogicalBlksTbl[0];

  if (ub_extra_data.AssocLvl != 0u) {                           // Validate that assoc lvl is 0.
    LOG_ERR(("Fatal error: associativity level is not 0."));
    RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }

  if (ub_extra_data.NextSecIx == 0u) {                          // Validate that sec 0 is wr'en in SUB.
    LOG_ERR(("Fatal error: sector 0 must be wr'en in SUB."));
    RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }

  data_blk_ix_phy = FS_NAND_FTL_BlkIxPhyGet(p_nand_ftl, data_blk_ix_logical);
  if (data_blk_ix_phy != FS_NAND_BLK_IX_INVALID) {
    FS_NAND_FTL_SUB_MergeUntil(p_nand_ftl,                      // Perform copy of data blk sec until end of blk.
                               sub_ix,
                               p_nand_ftl->NbrSecPerBlk - 1u,
                               p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
    //                                                             ----------------- UPDATE METADATA ------------------

    //                                                             Mark old data blk dirty if there was one.
    FS_NAND_FTL_BlkMarkDirty(p_nand_ftl, data_blk_ix_phy);
  }

  //                                                               Update logical to physical blk tbl.
  p_nand_ftl->LogicalToPhyBlkMap[data_blk_ix_logical] = ub_data.BlkIxPhy;

  //                                                               Clear old SUB data structures.
  FS_NAND_FTL_UB_Clr(p_nand_ftl,
                     sub_ix);

  p_nand_ftl->SUB_Cnt--;
}
#endif

/****************************************************************************************************//**
 *                                       FS_NAND_FTL_SUB_MergeUntil()
 *
 * @brief    Copy data block sectors corresponding to specified sequential update block from current
 *           sequential update block sector to specified sector.
 *
 * @param    p_nand_ftl  Pointer to NAND FTL.
 *
 * @param    sub_ix      Index of logical block.
 *
 * @param    sec_end     Offset of last sector to merge with sequential update block.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NAND_FTL_SUB_MergeUntil(FS_NAND_FTL             *p_nand_ftl,
                                       FS_NAND_UB_QTY          sub_ix,
                                       FS_NAND_SEC_PER_BLK_QTY sec_end,
                                       RTOS_ERR                *p_err)
{
  FS_NAND_UB_EXTRA_DATA   ub_extra_data;
  FS_NAND_BLK_QTY         data_blk_ix_phy;
  FS_NAND_BLK_QTY         data_blk_ix_logical;
  FS_NAND_BLK_QTY         ub_ix_logical;
  FS_NAND_SEC_PER_BLK_QTY sec_offset_phy;
  FS_LB_NBR               sec_ix_phy_base;
  CPU_BOOLEAN             is_sec_used;

  ub_extra_data = p_nand_ftl->UB_ExtraDataTbl[sub_ix];
  //                                                               Find associated logical blk.
  data_blk_ix_logical = ub_extra_data.AssocLogicalBlksTbl[0];
  ub_ix_logical = FS_NAND_UB_IX_TO_LOG_BLK_IX(p_nand_ftl, sub_ix);

  data_blk_ix_phy = FS_NAND_FTL_BlkIxPhyGet(p_nand_ftl, data_blk_ix_logical);
  if (data_blk_ix_phy != FS_NAND_BLK_IX_INVALID) {
    sec_ix_phy_base = FS_NAND_BLK_IX_TO_SEC_IX(p_nand_ftl, data_blk_ix_phy);

    LOG_VRB(("Copying data sectors of old data block ", (u)data_blk_ix_phy,
             " (log ", (u)data_blk_ix_logical, ") from offset ",
             (u)ub_extra_data.NextSecIx, " to offset ", (u)sec_end));

    //                                                             --------------- COPY ALL MISSING SEC ---------------
    while (ub_extra_data.NextSecIx <= sec_end) {
      sec_offset_phy = ub_extra_data.NextSecIx;

      //                                                           Chk if sec is used.
      is_sec_used = FS_NAND_FTL_SecIsUsed(p_nand_ftl, sec_ix_phy_base + sec_offset_phy, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        is_sec_used = DEF_NO;                                   // If err, assume sec isn't used.
      }
      if (is_sec_used == DEF_YES) {
        do {                                                    // Do until wr is not aborted.
          RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

          //                                                       Rd data sec.
          FS_NAND_FTL_SecRdPhyNoRefresh(p_nand_ftl,
                                        p_nand_ftl->BufPtr,
                                        p_nand_ftl->OOS_BufPtr,
                                        data_blk_ix_phy,
                                        sec_offset_phy,
                                        p_err);

          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            LOG_ERR(("Fatal error reading sector ", (u)sec_offset_phy,
                     " of logical block ", (u)data_blk_ix_logical));
            return;
          }

          //                                                       Wr sec in SUB.
          FS_NAND_FTL_SecWrHandler(p_nand_ftl,
                                   p_nand_ftl->BufPtr,
                                   p_nand_ftl->OOS_BufPtr,
                                   ub_ix_logical,
                                   sec_offset_phy,
                                   p_err);
        } while (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_ABORT);

        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {       // SecWrHandler err code chk.
          LOG_ERR(("Fatal error writing sector ", (u)sec_offset_phy, " of logical block ", (u)ub_ix_logical));
          return;
        }
      }

      ub_extra_data.NextSecIx++;
    }
  }
}
#endif

/****************************************************************************************************//**
 *                                   FS_NAND_FTL_AvailBlkTblInvalidate()
 *
 * @brief    Invalidate available block table contents on device. It will be updated on next metadata
 *           commit.
 *
 * @param    p_nand_ftl  Pointer to NAND FTL.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NAND_FTL_AvailBlkTblInvalidate(FS_NAND_FTL *p_nand_ftl)
{
  FS_NAND_SEC_PER_BLK_QTY sec_ix_start;
  FS_NAND_SEC_PER_BLK_QTY sec_ix_end;
  CPU_SIZE_T              avail_blk_tbl_end;

  sec_ix_start = p_nand_ftl->MetaOffsetAvailBlkTbl / p_nand_ftl->SecSize;
  avail_blk_tbl_end = sizeof(FS_NAND_BLK_QTY) + sizeof(FS_NAND_ERASE_QTY);
  avail_blk_tbl_end *= p_nand_ftl->AvailBlkTblEntryCntMax;
  avail_blk_tbl_end += p_nand_ftl->MetaOffsetAvailBlkTbl - 1u;
  sec_ix_end = avail_blk_tbl_end / p_nand_ftl->SecSize;

  FS_NAND_FTL_MetaSecRangeInvalidate(p_nand_ftl, sec_ix_start, sec_ix_end);

  p_nand_ftl->AvailBlkTblInvalidated = DEF_YES;
}
#endif

/****************************************************************************************************//**
 *                                       FS_NAND_FTL_DirtyMapInvalidate()
 *
 * @brief    Invalidate dirty block bitmap contents on device. It will be updated on next metadata commit.
 *
 * @param    p_nand_ftl  Pointer to NAND FTL.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NAND_FTL_DirtyMapInvalidate(FS_NAND_FTL *p_nand_ftl)
{
  FS_NAND_SEC_PER_BLK_QTY sec_ix_start;
  FS_NAND_SEC_PER_BLK_QTY sec_ix_end;
  CPU_SIZE_T              dirty_map_end;

  dirty_map_end = p_nand_ftl->BlkCnt / DEF_OCTET_NBR_BITS;
  dirty_map_end += (p_nand_ftl->BlkCnt % DEF_OCTET_NBR_BITS) == 0u ? 0u : 1u;
  dirty_map_end += p_nand_ftl->MetaOffsetDirtyBitmap - 1u;

  sec_ix_start = p_nand_ftl->MetaOffsetDirtyBitmap / p_nand_ftl->SecSize;
  sec_ix_end = dirty_map_end                     / p_nand_ftl->SecSize;

  FS_NAND_FTL_MetaSecRangeInvalidate(p_nand_ftl, sec_ix_start, sec_ix_end);
}
#endif

/****************************************************************************************************//**
 *                                       FS_NAND_FTL_UB_TblInvalidate()
 *
 * @brief    Invalidate update block table contents on device. The invalid state means that the
 *           UB (update block) table has been modified in volatile memory, but was not committed to
 *           the device already. The valid state will be restored after the next metadata commit.
 *
 * @param    p_nand_ftl  Pointer to NAND FTL.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NAND_FTL_UB_TblInvalidate(FS_NAND_FTL *p_nand_ftl)
{
  FS_NAND_SEC_PER_BLK_QTY sec_ix_start;
  FS_NAND_SEC_PER_BLK_QTY sec_ix_end;
  CPU_SIZE_T              ub_tbl_end;

  ub_tbl_end = sizeof(FS_NAND_BLK_QTY);
  ub_tbl_end += p_nand_ftl->NbrSecPerBlk / DEF_OCTET_NBR_BITS;
  ub_tbl_end += p_nand_ftl->NbrSecPerBlk % DEF_OCTET_NBR_BITS == 0u ? 0u : 1u;
  ub_tbl_end *= p_nand_ftl->UB_CntMax;
  ub_tbl_end += p_nand_ftl->MetaOffsetUB_Tbl - 1u;

  sec_ix_start = p_nand_ftl->MetaOffsetUB_Tbl / p_nand_ftl->SecSize;
  sec_ix_end = ub_tbl_end                   / p_nand_ftl->SecSize;

  FS_NAND_FTL_MetaSecRangeInvalidate(p_nand_ftl, sec_ix_start, sec_ix_end);
}
#endif

/****************************************************************************************************//**
 *                                       FS_NAND_FTL_BadBlkTblInvalidate()
 *
 * @brief    Invalidate update block table contents on device. It will be updated on next metadata commit.
 *
 * @param    p_nand_ftl  Pointer to NAND FTL.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NAND_FTL_BadBlkTblInvalidate(FS_NAND_FTL *p_nand_ftl)
{
  FS_NAND                 *p_nand;
  FS_NAND_PART_PARAM      *p_part_info;
  FS_NAND_SEC_PER_BLK_QTY sec_ix_start;
  FS_NAND_SEC_PER_BLK_QTY sec_ix_end;
  CPU_SIZE_T              bad_blk_tbl_end;

  p_nand = (FS_NAND *)p_nand_ftl->BlkDev.MediaPtr;

  p_part_info = p_nand->CtrlrPtr->CtrlrApiPtr->PartInfoGet(p_nand->CtrlrPtr);

  sec_ix_start = p_nand_ftl->MetaOffsetBadBlkTbl / p_nand_ftl->SecSize;

  bad_blk_tbl_end = p_nand_ftl->MetaOffsetBadBlkTbl;
  bad_blk_tbl_end += p_part_info->MaxBadBlkCnt * sizeof(FS_NAND_BLK_QTY);
  sec_ix_end = bad_blk_tbl_end / p_nand_ftl->SecSize;

  FS_NAND_FTL_MetaSecRangeInvalidate(p_nand_ftl, sec_ix_start, sec_ix_end);
}
#endif

/****************************************************************************************************//**
 *                                       FS_NAND_FTL_SecTypeParse()
 *
 * @brief    Parse sector type from out of sector (OOS) data.
 *
 * @param    p_oos_buf   Pointer to out of sector (OOS) data buffer. Must be populated by caller.
 *
 * @return   Sector type.
 *******************************************************************************************************/
static FS_NAND_SEC_TYPE FS_NAND_FTL_SecTypeParse(CPU_INT08U *p_oos_buf)
{
  FS_NAND_SEC_TYPE_STO sec_type_sto;
  FS_NAND_SEC_TYPE     sec_type;

  MEM_VAL_COPY_GET_INTU_LITTLE(&sec_type_sto,
                               &p_oos_buf[FS_NAND_OOS_SEC_TYPE_OFFSET],
                               sizeof(FS_NAND_SEC_TYPE_STO));

  switch (sec_type_sto) {
    case FS_NAND_SEC_TYPE_UNUSED:
      sec_type = FS_NAND_SEC_TYPE_UNUSED;
      break;

    case FS_NAND_SEC_TYPE_STORAGE:
      sec_type = FS_NAND_SEC_TYPE_STORAGE;
      break;

    case FS_NAND_SEC_TYPE_METADATA:
      sec_type = FS_NAND_SEC_TYPE_METADATA;
      break;

    case FS_NAND_SEC_TYPE_HDR:
      sec_type = FS_NAND_SEC_TYPE_HDR;
      break;

    case FS_NAND_SEC_TYPE_DUMMY:
      sec_type = FS_NAND_SEC_TYPE_DUMMY;
      break;

    case FS_NAND_SEC_TYPE_INVALID:
    default:
      sec_type = FS_NAND_SEC_TYPE_INVALID;
      break;
  }

  return (sec_type);
}

/****************************************************************************************************//**
 *                                       FS_NAND_FTL_LowFmtInternal()
 *
 * @brief    Low-level format NAND.
 *
 * @param    p_nand_ftl  Pointer to NAND FTL.
 *
 * @param    p_nand      _ftl      Pointer to NAND FTL.
 *
 * @param    p_err       Error pointer.
 *
 * @note     (1) Device header is always on first good block.
 *
 * @note     (2) The NAND device is marked unformatted when formatting commences; it will not be marked
 *               formatted until the low-level mount completes.
 *
 * @note     (3) If low level parameters change, the device must be reformatted. Low level format
 *               conversion is not yet implemented.
 *
 * @note     (4) If an existing metadata block exists on the device, the initial ID number is
 *               changed in order to make sure FS_NAND_MetaBlkFind() will choose the newly created
 *               metadata block.
 *
 * @note     (5) Device should never be low-level formatted by more than one filesystem/driver.
 *               As soon as a block is erased, the erase count and the bad block mark will be lost.
 *               This vital information might affect the performance of the driver. However, when
 *               low-level formatting a device with this driver, care will be taken to keep the erase
 *               counts and the bad block marks.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NAND_FTL_LowFmtInternal(FS_NAND_FTL *p_nand_ftl,
                                       FS_NAND     *p_nand,
                                       RTOS_ERR    *p_err)
{
  CPU_INT08U      *p_oos_buf;
  FS_NAND_BLK_QTY blk_ix_phy;
  FS_NAND_BLK_QTY blk_ix_hdr;
  FS_NAND_BLK_QTY last_blk_ix_phy;
  FS_NAND_BLK_QTY avail_blk_cnt;
  CPU_BOOLEAN     blk_is_bad;
  CPU_BOOLEAN     done;

  p_oos_buf = (CPU_INT08U *)p_nand_ftl->OOS_BufPtr;

  blk_ix_hdr = FS_NAND_BLK_IX_INVALID;
  p_nand_ftl->MetaBlkIxPhy = FS_NAND_BLK_IX_INVALID;
  p_nand_ftl->MetaBlkID = 0u;

  //                                                               ------------- CHK FOR EXISTING LOW FMT -------------
  blk_ix_hdr = FS_NAND_FTL_HdrRd(p_nand_ftl, p_err);
  if ((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE)
      || (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_BLK_DEV_CORRUPTED)
      || (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_BLK_DEV_FMT_INCOMPATIBLE)) {
    LOG_ERR(("An existing low-level format exists; some block erase count may "
             "be lost during the low-level format operation."));

    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    FS_NAND_FTL_MetaBlkFind(p_nand_ftl, blk_ix_hdr, p_err);     // Chk for old meta blk.
    if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
      //                                                           An old meta blk was found.
      FS_NAND_FTL_SecRdPhyNoRefresh(p_nand_ftl,
                                    p_nand_ftl->BufPtr,
                                    p_nand_ftl->OOS_BufPtr,
                                    p_nand_ftl->MetaBlkIxPhy,
                                    0u,
                                    p_err);
      if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
        //                                                         Start ID numbering after old blk ID.
        MEM_VAL_COPY_GET_INTU_LITTLE(&p_nand_ftl->MetaBlkID,
                                     &p_oos_buf[FS_NAND_OOS_META_ID_OFFSET],
                                     sizeof(FS_NAND_META_ID));

        p_nand_ftl->MetaBlkID += 1u;                            // See Note 4.
      }
    }
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ------------ FIND GOOD BLK FOR DEV HDR -------------
  blk_ix_hdr = p_nand_ftl->BlkIxFirst;
  last_blk_ix_phy = p_nand_ftl->BlkIxFirst + p_nand_ftl->BlkCnt - 1u;
  done = DEF_NO;
  do {
    blk_is_bad = FS_NAND_FTL_BlkIsFactoryDefect(p_nand_ftl,
                                                blk_ix_hdr,
                                                p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      LOG_ERR(("Fatal error determining if block ", (u)blk_ix_hdr, " is bad."));
      return;
    }

    if (blk_is_bad == DEF_NO) {
      //                                                           -------------------- ERASE BLK ---------------------
      FS_NAND_BlkEraseInternal(p_nand,
                               blk_ix_hdr,
                               p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);                    // Ignore the err and go through another loop iter.
      } else {
        //                                                         -------------------- WR DEV HDR --------------------
        FS_NAND_FTL_HdrWr(p_nand_ftl,
                          blk_ix_hdr,
                          p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          LOG_ERR(("Error writing header block on device."));
          RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
        } else {
          done = DEF_YES;
        }
      }
    }

    if (done == DEF_NO) {
      blk_ix_hdr++;
    }
  } while ((blk_ix_hdr <= last_blk_ix_phy)
           && (done == DEF_NO));

  if (blk_is_bad == DEF_YES) {
    LOG_ERR(("Could not find a good block to host device header."));
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return;
  }

  //                                                               ----- SCAN ALL BLKS TO MARK DIRTY AND BAD BLKS -----
  for (blk_ix_phy = p_nand_ftl->BlkIxFirst; blk_ix_phy <= last_blk_ix_phy; blk_ix_phy++) {
    if ((blk_ix_phy != p_nand_ftl->MetaBlkIxPhy)                // Ignore meta and hdr blks.
        && (blk_ix_phy != blk_ix_hdr)              ) {
      blk_is_bad = FS_NAND_FTL_BlkIsFactoryDefect(p_nand_ftl,
                                                  blk_ix_phy,
                                                  p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
        //                                                         If defect mark can't be rd, assume it's bad.
        FS_NAND_FTL_BlkMarkBad(p_nand_ftl, blk_ix_phy, p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          return;
        }
      } else {
        if (blk_is_bad == DEF_YES) {                            // If defect, mark as bad ...
          FS_NAND_FTL_BlkMarkBad(p_nand_ftl, blk_ix_phy, p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            return;
          }
        } else {                                                // ... else, mark it as dirty.
          FS_NAND_FTL_BlkMarkDirty(p_nand_ftl, blk_ix_phy);
        }
      }
    }
  }

  //                                                               ---------------- FILL UP AVAIL TBL -----------------
  avail_blk_cnt = FS_NAND_FTL_AvailBlkTblFill(p_nand_ftl,
                                              FS_NAND_CFG_RSVD_AVAIL_BLK_CNT + 1,
                                              DEF_DISABLED,
                                              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Fatal error; unable to fill available block table."));
    return;
  }

  if (avail_blk_cnt != FS_NAND_CFG_RSVD_AVAIL_BLK_CNT + 1) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_BLK_DEV_CORRUPTED);
    return;
  }

  //                                                               -------- FIND BLK FOR META IN AVAIL BLK TBL --------
  p_nand_ftl->MetaBlkIxPhy = FS_NAND_FTL_BlkGetAvailFromTbl(p_nand_ftl, DEF_NO);
  if (p_nand_ftl->MetaBlkIxPhy == FS_NAND_BLK_IX_INVALID) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_BLK_DEV_CORRUPTED);
    return;
  }

  FS_NAND_FTL_BlkEnsureErased(p_nand_ftl, p_nand_ftl->MetaBlkIxPhy, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Fatal error; unable to erase initial metadata block."));
    return;
  }

  //                                                               ------------------- COMMIT META --------------------
  p_nand_ftl->MetaBlkFoldNeeded = DEF_NO;
  p_nand_ftl->MetaBlkNextSecIx = 0u;

  FS_NAND_FTL_MetaSecRangeInvalidate(p_nand_ftl,
                                     0u,
                                     p_nand_ftl->MetaSecCnt);

  FS_NAND_FTL_MetaCommit(p_nand_ftl,
                         DEF_NO,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Fatal error; unable to commit metadata to device."));
    return;
  }

  //                                                               --------------------- MOUNT DEV --------------------
  LOG_DBG(("Low-level formatting complete."));
}
#endif

/****************************************************************************************************//**
 *                                       FS_NAND_FTL_LowMountInternal()
 *
 * @brief    Low-level mount NAND.
 *
 * @param    p_nand_ftl  Pointer to NAND FTL.
 *
 * @param    p_err       Error pointer.
 *
 * @note     (1) Low-level mounting parses the on-device structure, detecting the presence of a valid
 *               low-level format.  If 'RTOS_ERR_DEV_INVALID_LOW_FMT' is returned, the device is NOT
 *               low-level formatted (see 'FS_NAND_LowFmtHandler()).
 *
 * @note     (2) If an existing on-device low-level format is found but doesn't match the format
 *               prompted by specified device configuration, 'RTOS_ERR_INCOMPATIBLE_LOW_PARAMS' will
 *               be returned. A low-level format is required.
 *
 * @note     (3) If an existing and compatible on-device low-level format is found, but is not
 *               usable because of some metadata corruption, 'RTOS_ERR_CORRUPT_LOW_FMT' will be.
 *               A chip erase and/or low-level format is required.
 *******************************************************************************************************/
static void FS_NAND_FTL_LowMountInternal(FS_NAND_FTL *p_nand_ftl,
                                         RTOS_ERR    *p_err)
{
  CPU_INT08U       *p_sec_buf;
  CPU_INT08U       *p_oos_buf;
  FS_NAND_BLK_QTY  blk_ix_phy_hdr;
  FS_NAND_BLK_QTY  blk_ix_phy;
  FS_NAND_BLK_QTY  blk_ix_logical;
  FS_NAND_BLK_QTY  last_blk_ix;
  FS_NAND_SEC_TYPE blk_type;
  CPU_BOOLEAN      blk_is_bad;
  CPU_BOOLEAN      blk_is_dirty;
  CPU_BOOLEAN      blk_is_ub;
  CPU_BOOLEAN      blk_is_avail;
  FS_LB_QTY        sec_ix_phy;
  CPU_BOOLEAN      sec_is_used;
  CPU_INT08U       rd_retries;

  p_sec_buf = (CPU_INT08U *)p_nand_ftl->BufPtr;
  p_oos_buf = (CPU_INT08U *)p_nand_ftl->OOS_BufPtr;

  //                                                               ------------------- FIND HDR BLK -------------------
  blk_ix_phy_hdr = FS_NAND_FTL_HdrRd(p_nand_ftl, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Can't read device header."));
    return;
  }

  //                                                               ------------------ FIND MAPPING BLK ----------------
  FS_NAND_FTL_MetaBlkFind(p_nand_ftl, blk_ix_phy_hdr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_BLK_DEV_CORRUPTED);
    return;
  }

  //                                                               ------------------- PARSE META BLK -----------------
  FS_NAND_FTL_MetaBlkParse(p_nand_ftl, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_BLK_DEV_CORRUPTED);
    return;
  }

  //                                                               ------------- LOAD DIRTY BITMAP CACHE --------------
#if (FS_NAND_CFG_DIRTY_MAP_CACHE_EN == DEF_ENABLED)
  Mem_Copy(&p_nand_ftl->DirtyBitmapCache[0u],
           &p_nand_ftl->MetaCache[p_nand_ftl->MetaOffsetDirtyBitmap],
           p_nand_ftl->DirtyBitmapSize);
#endif

  //                                                               -------------- SCAN BLKS TO FILL META --------------
  last_blk_ix = p_nand_ftl->BlkIxFirst + p_nand_ftl->BlkCnt - 1u;
  //                                                               Ix pre-incremented to avoid scanning hdr blk.
  for (blk_ix_phy = blk_ix_phy_hdr + 1u; blk_ix_phy <= last_blk_ix; blk_ix_phy++) {
    //                                                             Chk that blk isn't bad.
    blk_is_bad = FS_NAND_FTL_BlkIsBad(p_nand_ftl, blk_ix_phy);
    if (blk_is_bad == DEF_NO) {
      blk_is_dirty = FS_NAND_FTL_BlkIsDirty(p_nand_ftl, blk_ix_phy);
      blk_is_avail = FS_NAND_FTL_BlkIsAvail(p_nand_ftl, blk_ix_phy);

      if ((blk_is_dirty == DEF_YES)                             // If blk is both dirty and avail ...
          && (blk_is_avail == DEF_YES)) {
        LOG_DBG(("Block ", (u)blk_ix_phy, " is dirty and available. Unmarking dirty."));
        //                                                         ... unmark dirty.
        Bitmap_BitClr(&p_nand_ftl->MetaCache[p_nand_ftl->MetaOffsetDirtyBitmap], blk_ix_phy);
      } else {
        if ((blk_is_dirty == DEF_NO)                            // Nothing to do for dirty or avail blks.
            && (blk_is_avail == DEF_NO)   ) {
          //                                                       Get 1st sect ix from this blk.
          sec_ix_phy = FS_NAND_BLK_IX_TO_SEC_IX(p_nand_ftl, blk_ix_phy);

          blk_is_ub = FS_NAND_FTL_BlkIsUB(p_nand_ftl, blk_ix_phy);
          sec_is_used = FS_NAND_FTL_SecIsUsed(p_nand_ftl, sec_ix_phy, p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            LOG_ERR(("Unable to determine if sector ", (u)blk_ix_phy, " is used."));
            RTOS_ERR_SET(*p_err, RTOS_ERR_BLK_DEV_CORRUPTED);
            return;
          }

          if (blk_is_ub == DEF_YES) {                           // Blk is an UB.
            LOG_DBG(("Loading update block ", (u)blk_ix_phy));
            //                                                     Load blk.
            FS_NAND_FTL_UB_Load(p_nand_ftl, blk_ix_phy, p_err);
            switch (RTOS_ERR_CODE_GET(*p_err)) {
              case RTOS_ERR_NONE:
                break;

              default:
                LOG_ERR(("Could not load update block at ix=", (u)blk_ix_phy, "."));
                RTOS_ERR_SET(*p_err, RTOS_ERR_BLK_DEV_CORRUPTED);
                return;
            }
          } else {
            if (sec_is_used == DEF_YES) {                       // Blk is either a meta or a data blk.
                                                                // Rd sec to determine if blk is meta or data.
              rd_retries = 0u;
              do {
                FS_NAND_FTL_SecRdPhyNoRefresh(p_nand_ftl,
                                              p_sec_buf,
                                              p_oos_buf,
                                              blk_ix_phy,
                                              0u,
                                              p_err);
                rd_retries++;
              } while ((rd_retries <= FS_NAND_CFG_MAX_RD_RETRIES)
                       && (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_ECC_UNCORR));

              switch (RTOS_ERR_CODE_GET(*p_err)) {
                case RTOS_ERR_ECC_UNCORR:
                  LOG_ERR(("Fatal ECC error reading block ", (u)blk_ix_phy, "."));
                  RTOS_ERR_SET(*p_err, RTOS_ERR_BLK_DEV_CORRUPTED);
                  return;

                case RTOS_ERR_NONE:
                  break;

                default:
                  return;
              }

              //                                                   Parse sec type from OOS meta.
              blk_type = FS_NAND_FTL_SecTypeParse(p_oos_buf);

              if (blk_type == FS_NAND_SEC_TYPE_STORAGE) {
                //                                                 Blk is a data blk.

                //                                                 Get assoc logical blk ix.
                MEM_VAL_COPY_GET_INTU_LITTLE(&blk_ix_logical,
                                             &p_oos_buf[FS_NAND_OOS_STO_LOGICAL_BLK_IX_OFFSET],
                                             sizeof(FS_NAND_BLK_QTY));

                //                                                 Validate that the mapping entry is inexistant.
                if (p_nand_ftl->LogicalToPhyBlkMap[blk_ix_logical] != FS_NAND_BLK_IX_INVALID) {
#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
                  if (blk_ix_phy == last_blk_ix) {
                    LOG_DBG(("Logical block ", (u)blk_ix_logical, " to last physical block mapping conflict."));
                    LOG_DBG(("Discarding last physical block."));
                    FS_NAND_FTL_BlkMarkDirty(p_nand_ftl, last_blk_ix);
                  } else
#endif
                  {
                    LOG_ERR(("Fatal error: logical block ", (u)blk_ix_logical, " is already mapped "
                                                                               "to physical block ", (u)p_nand_ftl->LogicalToPhyBlkMap[blk_ix_logical],
                             "; cannot map it to physical block ", (u)blk_ix_phy));
                    RTOS_ERR_SET(*p_err, RTOS_ERR_BLK_DEV_CORRUPTED);
                    return;
                  }
                } else {
                  //                                               Put in tbl.
                  LOG_VRB(("Storage block ", (u)blk_ix_phy, " is mapped to logical block ", (u)blk_ix_logical));
                  p_nand_ftl->LogicalToPhyBlkMap[blk_ix_logical] = blk_ix_phy;
                }
              } else if (blk_type == FS_NAND_SEC_TYPE_METADATA) {
                //                                                 Blk is a metadata blk.
                //                                                 Make sure every old meta blk is marked as dirty.
                if (blk_ix_phy != p_nand_ftl->MetaBlkIxPhy) {
                  LOG_VRB(("Found old metadata block (phy ix = ", (u)blk_ix_phy, ")."));
#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
                  FS_NAND_FTL_BlkMarkDirty(p_nand_ftl, blk_ix_phy);
#endif
                }
              } else {
                LOG_ERR(("Invalid block type for block ", (u)blk_ix_phy, "."));
                RTOS_ERR_SET(*p_err, RTOS_ERR_BLK_DEV_CORRUPTED);
                return;
              }
            }
          }
        }
      }
    }
  }

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
  //                                                               --------- PARSE AVAIL BLK TBL TMP COMMITS ----------
  FS_NAND_FTL_MetaBlkAvailParse(p_nand_ftl, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_BLK_DEV_CORRUPTED);
    LOG_ERR(("Error parsing available table temporary commits."));
    return;
  }
#endif

  p_nand_ftl->IsMounted = DEF_YES;

  LOG_DBG(("Low level mount succeeded."));
}

/****************************************************************************************************//**
 *                                       FS_NAND_FTL_LowUnmountInternal()
 *
 * @brief    Low-level unmount NAND.
 *
 * @param    p_nand_ftl  Pointer to NAND FTL.
 *
 * @param    p_err       Error pointer.
 *
 * @note     (1) Low-level unmounting clears software knowledge of the on-disk structures, forcing
 *               the device to again be low-level mounted or formatted prior to further use.
 *******************************************************************************************************/
#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NAND_FTL_LowUnmountInternal(FS_NAND_FTL *p_nand_ftl,
                                           RTOS_ERR    *p_err)
{
  PP_UNUSED_PARAM(p_err);
  FS_NAND_FTL_InitDevData(p_nand_ftl);

  p_nand_ftl->IsMounted = DEF_NO;
}
#endif

/****************************************************************************************************//**
 *                                       FS_NAND_FTL_SecRdHandler()
 *
 * @brief    - (1) Read sector (using logical block index) from a NAND device & store data in buffer.
 *           - (2) Handles some errors that may occur during read.
 *           - (3) May refresh an unreadable block.
 *
 * @param    p_nand_ftl      Pointer to NAND FTL.
 *
 * @param    p_dest          Pointer to destination buffer.
 *
 * @param    p_dest_oos      Pointer to destination spare-area buffer.
 *
 * @param    blk_ix_logical  Logical index of the block to read from.
 *
 * @param    sec_offset_phy  Physical offset of the sector to read.
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/
static void FS_NAND_FTL_SecRdHandler(FS_NAND_FTL             *p_nand_ftl,
                                     void                    *p_dest,
                                     void                    *p_dest_oos,
                                     FS_NAND_BLK_QTY         blk_ix_logical,
                                     FS_NAND_SEC_PER_BLK_QTY sec_offset_phy,
                                     RTOS_ERR                *p_err)
{
  FS_NAND         *p_nand;
  FS_NAND_CTRLR   *p_ctrlr;
  FS_NAND_BLK_QTY blk_ix_phy;
  FS_LB_QTY       sec_ix_phy;

  p_nand = (FS_NAND *)p_nand_ftl->BlkDev.MediaPtr;
  p_ctrlr = p_nand->CtrlrPtr;

  while (DEF_YES) {                                             // Loop until rd success or fatal err.
                                                                // Calc sec ix phy.
    blk_ix_phy = FS_NAND_FTL_BlkIxPhyGet(p_nand_ftl, blk_ix_logical);
    if (blk_ix_phy == FS_NAND_BLK_IX_INVALID) {
      LOG_ERR(("Index of physical block (blk_ix_phy) is invalid."));
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
      return;
    }

    sec_ix_phy = FS_NAND_BLK_IX_TO_SEC_IX(p_nand_ftl, blk_ix_phy);
    sec_ix_phy += sec_offset_phy;

    p_ctrlr->CtrlrApiPtr->SecRd(p_ctrlr,                        // Rd sec.
                                p_dest,
                                p_dest_oos,
                                sec_ix_phy,
                                p_err);

    if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_ECC_CORR) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      return;
    } else if ((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_ECC_UNCORR)
               || (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_ECC_CRITICAL_CORR)) {
#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      FS_NAND_FTL_BlkRefresh(p_nand_ftl,                        // Refresh blk.
                             blk_ix_phy,
                             p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {         // If refresh failed ...
        LOG_ERR(("Uncorrectable read err in blk ", (u)blk_ix_phy, "marking it as bad."));

        RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
        FS_NAND_FTL_BlkMarkBad(p_nand_ftl,                      // ... mark blk as bad.
                               blk_ix_phy,
                               p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          LOG_ERR(("Could not mark failing blk ", (u)blk_ix_phy, " bad."));
        }
        RTOS_ERR_SET(*p_err, RTOS_ERR_ECC_UNCORR);
        return;
      } else {                                                  // ... else, mark blk as dirty.
        FS_NAND_FTL_BlkMarkDirty(p_nand_ftl,
                                 blk_ix_phy);
      }

      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
#else
      return;                                                   // Ret err to caller.
#endif
    } else if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {    // Unhandled err.
      return;
    } else {                                                    // Success.
      return;
    }
  }
}

/****************************************************************************************************//**
 *                                       FS_NAND_FTL_SecRdPhyNoRefresh()
 *
 * @brief    Read a sector in a physical block from a NAND device & store data in buffer. Handles
 *           some errors that may occur during read. May NOT refresh an unreadable block.
 *
 * @param    p_nand_ftl      Pointer to NAND FTL.
 *
 * @param    p_dest          Pointer to destination buffer.
 *
 * @param    p_dest_oos      Pointer to destination spare-area buffer.
 *
 * @param    blk_ix_phy      Physical index of block containing sector to read.
 *
 * @param    sec_offset_phy  Physical offset of the sector to read.
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/
static void FS_NAND_FTL_SecRdPhyNoRefresh(FS_NAND_FTL             *p_nand_ftl,
                                          void                    *p_dest,
                                          void                    *p_dest_oos,
                                          FS_NAND_BLK_QTY         blk_ix_phy,
                                          FS_NAND_SEC_PER_BLK_QTY sec_offset_phy,
                                          RTOS_ERR                *p_err)
{
  FS_NAND       *p_nand;
  FS_NAND_CTRLR *p_ctrlr;
  FS_LB_QTY     sec_ix_phy;
  CPU_INT08U    rd_retries;

  if (blk_ix_phy == FS_NAND_BLK_IX_INVALID) {
    LOG_ERR(("Index of physical block (blk_ix_phy) is invalid.\r\n"));
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return;
  }

  p_nand = (FS_NAND *)p_nand_ftl->BlkDev.MediaPtr;
  p_ctrlr = p_nand->CtrlrPtr;

  rd_retries = 0u;
  while (rd_retries < FS_NAND_CFG_MAX_RD_RETRIES) {             // Loop until rd success or retry cnt exceeded.
                                                                // Calc sec ix phy.
    sec_ix_phy = FS_NAND_BLK_IX_TO_SEC_IX(p_nand_ftl, blk_ix_phy);
    sec_ix_phy += sec_offset_phy;

    p_ctrlr->CtrlrApiPtr->SecRd(p_ctrlr,                        // Rd sec.
                                p_dest,
                                p_dest_oos,
                                sec_ix_phy,
                                p_err);

    if ((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_ECC_CORR)
        || (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_ECC_CRITICAL_CORR)) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      return;
      //                                                           Retry on uncorrectable ecc err.
    } else if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_ECC_UNCORR) {
      CPU_INT32U sec_used_mark;                                 // Chk used mark.
      CPU_INT08U set_bit_cnt;

      sec_used_mark = 0u;
      MEM_VAL_COPY_GET_INTU_LITTLE(&sec_used_mark,
                                   (CPU_CHAR *)p_dest_oos + FS_NAND_OOS_SEC_USED_OFFSET,
                                   p_nand_ftl->UsedMarkSize);

      set_bit_cnt = CRCUtil_PopCnt_32(sec_used_mark);

      if (set_bit_cnt < (p_nand_ftl->UsedMarkSize * DEF_INT_08_NBR_BITS / 2u)) {
        rd_retries++;                                           // Sec is used, retry.
        RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      } else {
        return;                                                 // Sec isn't used, no need to retry.
      }
      //                                                           Unhandled err.
    } else if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    } else {                                                    // Success.
      return;
    }
  }

  //                                                               Max retry cnt reached.
  RTOS_ERR_SET(*p_err, RTOS_ERR_ECC_UNCORR);
  return;
}

/****************************************************************************************************//**
 *                                           FS_NAND_FTL_SecRd()
 *
 * @brief    Read one sector from a NAND device & store data in buffer.
 *
 * @param    p_nand_ftl      Pointer to NAND FTL.
 *
 * @param    p_dest          Pointer to destination buffer.
 *
 * @param    sec_ix_logical  Logical index of the sector to read.
 *
 * @param    sec_cnt         Number of sectors to read (unused).
 *
 * @param    p_err           Error pointer.
 *
 * @return   Number of sectors read from device.
 *
 * @note     (1) The function must search for the sector(s) in the UB (update blocks) before searching
 *               data blocks to make sure to get the latest version of the sector written on the device.
 *
 * @note     (2) For the moment, the parameter 'sec_cnt' is not used, and only one sector is read for
 *               each call. However, the parameter is left for future implementation.
 *
 * @note     (3) At this stage, the searched sector has:
 *               - Either the associated block index that does not exist in the blocks table
 *               - Either the sector exists in the data block but the spare area indicates that this
 *                 sector is not used.
 *               In both cases, the sector read is considered as an empty read with no error
 *               (FS v4 implementation).
 *
 * @note     (4) At this stage, the sector has been found in update block or data block, has been read
 *               from the NAND device but the spare area indicates that this sector is invalid (that
 *               is) a dummy sector. In that case, the sector is considered as an empty read with no
 *               error (FS v4 implementation).
 *******************************************************************************************************/
static FS_LB_QTY FS_NAND_FTL_SecRd(FS_NAND_FTL *p_nand_ftl,
                                   void        *p_dest,
                                   FS_LB_QTY   sec_ix_logical,
                                   FS_LB_QTY   sec_cnt,
                                   RTOS_ERR    *p_err)
{
  FS_NAND_BLK_QTY         blk_ix_logical;
  FS_NAND_SEC_PER_BLK_QTY sec_offset_logical;
  FS_NAND_SEC_PER_BLK_QTY sec_offset_logical_rd;
  FS_NAND_SEC_PER_BLK_QTY sec_offset_phy = 0u;
  FS_NAND_UB_SEC_DATA     ub_sec_data;
  FS_NAND_UB_DATA         ub_data;
  FS_LB_QTY               sec_ix_phy;
  FS_NAND_BLK_QTY         blk_ix_phy;
  CPU_BOOLEAN             is_sec_in_ub;
  CPU_BOOLEAN             is_sec_used;
  CPU_BOOLEAN             *p_dest_oos;

  PP_UNUSED_PARAM(sec_cnt);
  p_dest_oos = (CPU_INT08U *)p_nand_ftl->OOS_BufPtr;

  blk_ix_logical = FS_NAND_SEC_IX_TO_BLK_IX(p_nand_ftl, sec_ix_logical);
  sec_offset_logical = sec_ix_logical - FS_NAND_BLK_IX_TO_SEC_IX(p_nand_ftl, blk_ix_logical);

  //                                                               --------------- FIND VALID SEC IN UB ---------------
  is_sec_in_ub = DEF_NO;

  ub_sec_data = FS_NAND_FTL_UB_Find(p_nand_ftl,
                                    blk_ix_logical);

  if (ub_sec_data.UB_Ix != FS_NAND_UB_IX_INVALID) {             // If sector exists in Update Block, ...
                                                                // ...find sec in UB.
    ub_sec_data = FS_NAND_FTL_UB_SecFind(p_nand_ftl,
                                         ub_sec_data,
                                         sec_offset_logical,
                                         p_err);

    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      LOG_ERR(("Error looking for sector ", (u)sec_offset_logical, " in update block ", (u)ub_sec_data.UB_Ix));

      return (0u);
    }

    if (ub_sec_data.SecOffsetPhy != FS_NAND_SEC_OFFSET_IX_INVALID) {
      sec_offset_phy = ub_sec_data.SecOffsetPhy;

      //                                                           Chk is sec is valid.
      ub_data = FS_NAND_FTL_UB_TblEntryRd(p_nand_ftl,
                                          ub_sec_data.UB_Ix);

      is_sec_in_ub = Bitmap_BitIsSet(ub_data.SecValidBitMap, sec_offset_phy);

      if (is_sec_in_ub == DEF_YES) {
        blk_ix_logical = FS_NAND_UB_IX_TO_LOG_BLK_IX(p_nand_ftl, ub_sec_data.UB_Ix);
      }
    }
  }

  //                                                               --------------- FIND SEC ON DATA BLK ---------------
  if (is_sec_in_ub == DEF_NO) {
    //                                                             Chk if sec is wr'en.
    blk_ix_phy = FS_NAND_FTL_BlkIxPhyGet(p_nand_ftl, blk_ix_logical);

    if (blk_ix_phy == FS_NAND_BLK_IX_INVALID) {
      is_sec_used = DEF_NO;                                     // No data blk assoc with logical blk.
    } else {
      sec_ix_phy = FS_NAND_BLK_IX_TO_SEC_IX(p_nand_ftl, blk_ix_phy);
      sec_ix_phy += sec_offset_logical;

      is_sec_used = FS_NAND_FTL_SecIsUsed(p_nand_ftl, sec_ix_phy, p_err);

      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        LOG_ERR(("Error determining if sector ", (u)sec_ix_phy, " is used."));

        return (0u);
      }
    }

    if (is_sec_used == DEF_YES) {
      sec_offset_phy = sec_offset_logical;
    } else {
      LOG_DBG(("Logical sector ", (u)sec_ix_logical, " does not exist on device."));
      return (1u);                                              // See Note #3.
    }
  }

  //                                                               ------------------ PERFORM SEC RD ------------------
  FS_NAND_FTL_SecRdHandler(p_nand_ftl,
                           p_dest,
                           p_nand_ftl->OOS_BufPtr,
                           blk_ix_logical,
                           sec_offset_phy,
                           p_err);

  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Error reading physical sector ", (u)sec_offset_phy, " in logical block ", (u)blk_ix_logical));
    return (0u);
  }

  //                                                               Chk if sec is dummy.
  MEM_VAL_COPY_GET_INTU_LITTLE(&sec_offset_logical_rd,
                               &p_dest_oos[FS_NAND_OOS_STO_BLK_SEC_IX_OFFSET],
                               sizeof(FS_NAND_SEC_PER_BLK_QTY));

  if (sec_offset_logical_rd == FS_NAND_SEC_OFFSET_IX_INVALID) {
    LOG_ERR(("Tried to read an invalid, dummy sector (sec ", (u)sec_ix_logical, ")."));
    return (1u);                                                // See Note #4.
  }

  return (1u);
}

/****************************************************************************************************//**
 *                                       FS_NAND_FTL_SecWrHandler()
 *
 * @brief    Write data sector and associated out of sector data to a NAND device.
 *
 * @param    p_nand_ftl      Pointer to NAND FTL.
 *
 * @param    p_src           Pointer to source buffer.
 *
 * @param    p_src_oos       Pointer to source spare-area buffer.
 *
 * @param    blk_ix_logical  Block's logical index.
 *
 * @param    sec_offset_phy  Physical offset of the sector to write.
 *
 * @param    p_err           Error pointer.
 *
 * @note     (1) When a programming error occurs, the block is refreshed and marked bad. Sector write
 *               operation is aborted. The caller is responsible for retrying the operation. If p_dest
 *               and p_dest_oos are pointers to internal buffers (p_nand_data->BufPtr and p_nand_data->
 *               OOS_BufPtr), the caller MUST refresh their content prior to retrying the operation
 *               since it might have been overwritten by FS_NAND_BlkRefresh() to backup sectors.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NAND_FTL_SecWrHandler(FS_NAND_FTL             *p_nand_ftl,
                                     void                    *p_src,
                                     void                    *p_src_oos,
                                     FS_NAND_BLK_QTY         blk_ix_logical,
                                     FS_NAND_SEC_PER_BLK_QTY sec_offset_phy,
                                     RTOS_ERR                *p_err)
{
  FS_NAND         *p_nand;
  FS_NAND_CTRLR   *p_ctrlr;
  FS_NAND_BLK_QTY blk_ix_phy;
  FS_LB_QTY       sec_ix_phy;

  p_nand = (FS_NAND *)p_nand_ftl->BlkDev.MediaPtr;
  p_ctrlr = p_nand->CtrlrPtr;

  //                                                               ----------------- CALC SEC PHY IX ------------------
  blk_ix_phy = FS_NAND_FTL_BlkIxPhyGet(p_nand_ftl, blk_ix_logical);
  if (blk_ix_phy == FS_NAND_BLK_IX_INVALID) {
    LOG_ERR(("Index of physical block (blk_ix_phy) is invalid."));
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return;
  }

  sec_ix_phy = FS_NAND_BLK_IX_TO_SEC_IX(p_nand_ftl, blk_ix_phy);
  sec_ix_phy += sec_offset_phy;

  //                                                               ------------------ WR SEC TO DEV -------------------
  p_ctrlr->CtrlrApiPtr->SecWr(p_ctrlr,
                              p_src,
                              p_src_oos,
                              sec_ix_phy,
                              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_IO) {               // ------------------- HANDLE ERRS --------------------
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    FS_NAND_FTL_BlkRefresh(p_nand_ftl, blk_ix_phy, p_err);      // Refresh blk.

    switch (RTOS_ERR_CODE_GET(*p_err)) {
      case RTOS_ERR_ECC_UNCORR:                                 // Ignore uncorrectable ECC err.
        RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
        break;

      case RTOS_ERR_NONE:
        break;

      default:
        return;
    }

    FS_NAND_FTL_BlkMarkBad(p_nand_ftl, blk_ix_phy, p_err);      // Mark blk bad.
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      LOG_ERR(("Failed to mark failing block ", (u)blk_ix_phy, " as bad."));
      return;
    }

    RTOS_ERR_SET(*p_err, RTOS_ERR_ABORT);                       // Notify caller op has failed.
  }
}
#endif

/****************************************************************************************************//**
 *                                       FS_NAND_FTL_MetaSecWrHandler()
 *
 * @brief    Write metadata sector and associated out of sector data to a NAND device.
 *
 * @param    p_nand_ftl      Pointer to NAND FTL.
 *
 * @param    p_src           Pointer to source buffer.
 *
 * @param    p_src_oos       Pointer to source spare-area buffer.
 *
 * @param    sec_offset_phy  Physical offset of the sector to write.
 *
 * @param    p_err           Error pointer.
 *
 * @note     (1) When a programming error occurs, a new metadata block is started. Metadata is
 *               committed to that new block. Operation aborts and must be restarted by caller.
 *               If p_dest and p_dest_oos are pointers to buffers p_nand_data->BufPtr and
 *               p_nand_data->OOS_BufPtr, the buffers might be overwritten and thus must be repopulated
 *               by the caller.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NAND_FTL_MetaSecWrHandler(FS_NAND_FTL             *p_nand_ftl,
                                         void                    *p_src,
                                         void                    *p_src_oos,
                                         FS_NAND_SEC_PER_BLK_QTY sec_offset_phy,
                                         RTOS_ERR                *p_err)
{
  FS_NAND         *p_nand;
  FS_NAND_CTRLR   *p_ctrlr;
  FS_NAND_BLK_QTY blk_ix_logical;
  FS_NAND_BLK_QTY blk_ix_phy;
  FS_LB_QTY       sec_ix_phy;

  p_nand = (FS_NAND *)p_nand_ftl->BlkDev.MediaPtr;
  p_ctrlr = p_nand->CtrlrPtr;
  //                                                               Set blk_ix_logical to wr to meta blk.
  blk_ix_logical = p_nand_ftl->LogicalDataBlkCnt + p_nand_ftl->UB_CntMax;

  //                                                               ----------------- CALC SEC PHY IX ------------------
  blk_ix_phy = FS_NAND_FTL_BlkIxPhyGet(p_nand_ftl, blk_ix_logical);
  if (blk_ix_phy == FS_NAND_BLK_IX_INVALID) {
    LOG_ERR(("Index of physical block (blk_ix_phy) is invalid."));
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return;
  }

  sec_ix_phy = FS_NAND_BLK_IX_TO_SEC_IX(p_nand_ftl, blk_ix_phy);
  sec_ix_phy += sec_offset_phy;

  //                                                               ------------------ WR SEC TO DEV -------------------
  p_ctrlr->CtrlrApiPtr->SecWr(p_ctrlr,
                              p_src,
                              p_src_oos,
                              sec_ix_phy,
                              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_IO) {               // If meta blk cannot be wr'en ...
    p_nand_ftl->MetaBlkFoldNeeded = DEF_YES;                    // ... trigger a folding op.
    LOG_ERR(("Meta block on device is not accessible. Corruption could occur."));
    RTOS_ERR_SET(*p_err, RTOS_ERR_ABORT);
  }
}
#endif

/****************************************************************************************************//**
 *                                           FS_NAND_FTL_SecWr()
 *
 * @brief    Write one logical sector on NAND device.
 *
 * @param    p_nand_ftl      Pointer to NAND FTL.
 *
 * @param    p_src           Pointer to source buffer.
 *
 * @param    sec_ix_logical  Sector's logical index.
 *
 * @param    sec_cnt         Number of consecutive sectors to write (unused).
 *
 * @param    p_err           Error pointer.
 *
 * @return   Number of sectors written. May not be equal to cnt.
 *
 * @note     (1) For the moment, the parameter 'sec_cnt' is not used, and only one sector is write for
 *               each call. However, the parameter is left for future implementation.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static FS_LB_QTY FS_NAND_FTL_SecWr(FS_NAND_FTL *p_nand_ftl,
                                   void        *p_src,
                                   FS_LB_QTY   sec_ix_logical,
                                   FS_LB_QTY   sec_cnt,
                                   RTOS_ERR    *p_err)
{
  FS_NAND_BLK_QTY     blk_ix_logical;
  FS_NAND_UB_SEC_DATA ub_sec_data;
  FS_LB_QTY           sec_wr_cnt;

  blk_ix_logical = FS_NAND_SEC_IX_TO_BLK_IX(p_nand_ftl, sec_ix_logical);

  ub_sec_data.UB_Ix = FS_NAND_UB_IX_INVALID;
  ub_sec_data.SecOffsetPhy = FS_NAND_SEC_OFFSET_IX_INVALID;

  //                                                               --------- CHK IF UB EXISTS FOR LOGICAL BLK ---------
  ub_sec_data = FS_NAND_FTL_UB_Find(p_nand_ftl,                 // Find associated UB.
                                    blk_ix_logical);

  //                                                               --------------------- WR IN UB ---------------------
  sec_wr_cnt = FS_NAND_FTL_SecWrInUB(p_nand_ftl,                // Wr sec in UB.
                                     p_src,
                                     sec_ix_logical,
                                     sec_cnt,
                                     ub_sec_data,
                                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Error writing sector ", (u)sec_ix_logical, " in an update block."));
    return (sec_wr_cnt);
  }

  return (sec_wr_cnt);
}
#endif

/****************************************************************************************************//**
 *                                           FS_NAND_FTL_SecWrInUB()
 *
 * @brief    Write 1 or more logical sectors in an update block.
 *
 * @param    p_nand_ftl      Pointer to NAND FTL.
 *
 * @param    p_src           Pointer to source buffer.
 *
 * @param    sec_ix_logical  Sector's logical index.
 *
 * @param    sec_cnt         Number of consecutive sectors to write.
 *
 * @param    ub_sec_data     Structure containing data on sector's latest update.
 *
 * @param    p_err           Error pointer.
 *
 * @return   Number of sectors written. Might not be equal to sec_cnt.
 *
 * @note     (1) Flowchart is shown below. Labels in parenthesis are written in appropriate
 *               section comments.
 *          @verbatim
 *                                             +-----------+
 *                                             | Wr in UB  |
 *           1d)             (A1b)             +-----+-----+
 *           --+   +-------------+                   |
 *             |   |PartMerge RUB|            /------+------\
 *           --+   |  Start SUB  |          +-|Sec 0 of LB? |--+
 *                 +------+------+          | \-------------/  |
 *           1a)          |No (A1)          |Yes (A)           |No  (B)        (B1)
 *         --\   /------+------\   /------+------\    /------'------\   /-------------\  +---------+
 *           |___| Write count |___| RUB exists  |    | RUB exists  |___|  RUB full?  |__|Wr in RUB|
 *           |Yes|< threshold? |Yes|   for LB?   |    |   for LB?   |Yes|             |No|         |
 *           |   | (Note #2a)  |   |             |    |             |   |             |  |         |
 *         --/   \-------------/   \------+------/    \------+------/   \------+------/  +---------+
 *           (A1c)                          |No                |No               |Yes
 *           ----+            (A3)          |                  |         +-------+--------+
 *               | +-------------+   /------+------\           |         |   Merge RUB    |
 *            LB | |  Start SUB  |___| SUB exists  |           |         |Alloc RUB for LB|
 *               | |  Wr in SUB  | No|   for LB?   |           |         |   Wr in RUB    |
 *           ----+ +-------------+   \------+------/           |         +----------------+
 *                                          |Yes               |
 *                           (A2b)          |   (A2)           |   (B3)
 *               +-------------+   /------+------\    /------+------\  +----------------+
 *               |Merge old SUB|___|Nbr free sec |    | SUB exists  |__|Alloc RUB for LB|
 *               |Start new SUB| No|> threshold? |    |   for LB?   |No|   Wr in RUB    |
 *               |             |   | (Note #2b)  |    |             |  |                |
 *               +-------------+   \------+------/    \------+------/  +----------------+
 *                                           |Yes               |Yes
 *                                           |  (A2a)           |   (B2)
 *                                   +------+------+    /------+------\
 *                                   |  SUB->RUB   |  __| Sec already |____
 *                                   |  Wr in RUB  |  | |   wr'en?    |   |
 *                                   +-------------+  | \-------------/   |
 *                                                    |No                 |Yes
 *                                    (B2c)           |  (B2b)            |  (B2a)            (B2a2)
 *                            +-------------+   /------+------\     /------+------\   +-------------+
 *                            |   Pad SUB   |___|  Gap below  |     |Nbr free sec |___|  SUB->RUB   |
 *                            |  Wr in SUB  |Yes| threshold?  |     |> threshold? |Yes|  Wr in RUB  |
 *                            |             |   | (Note #2c)  |     | (Note #2b)  |   |             |
 *                            +-------------+   \------+------/     \------+------/   +-------------+
 *                                                     |No                 |No
 *                                     (B2d2)          |  (B2d)            |   (B2a1)
 *                            +-------------+   /------+------\    +-------+--------+
 *                            |             |___|Nbr free sec |    |   Merge SUB    |
 *                            |  Wr in SUB  | No|> threshold? |    |Alloc RUB for LB|
 *                            |             |   | (Note #2b)  |    |   Wr in RUB    |
 *                            +-------------+   \------+------/    +----------------+
 *                                                     |Yes
 *                                                     | (B2d1)
 *                                              +------+------+
 *                                              |  SUB->RUB   |
 *                                              |  Wr in RUB  |
 *                                              +-------------+
 *          @endverbatim
 * @note     (2) Various thresholds must be set, relatively to the number of sectors per block.
 *               These thresholds can be set according to application specifics to obtain best
 *               performance. Indications are mentioned for every threshold (in the configuration
 *               file, fs_nand_ftl_priv.h). These indications are guidelines and specific cases
 *               could lead to different behaviors than what is expected.
 *               - (a) ThSecWrCnt_MergeRUBStartSUB (see A1).
 *                     This threshold indicates the minimum size (in sectors) of the write operation
 *                     needed to create a sequential update block (SUB) when a random update block (RUB)
 *                     already exists. SUBs offer a substantial gain in merge speed when a large
 *                     quantity of sectors are written sequentially (within a single or multiple write
 *                     operations). However, if many SUBs are created and merged early, the device will
 *                     wear faster (less sectors written between block erase operations).
 *                     This threshold can be set as a percentage  (relative to number of sectors per
 *                     block) in fs_nand_ftl_priv.h :
 *                       @verbatim
 *                       #define FS_NAND_TH_PCT_MERGE_RUB_START_SUB
 *                       @endverbatim
 *                     Set higher than default -> Better overall wear leveling
 *                     Set lower  than default -> Better overall write speed
 *           - (b) ThSecRemCnt_ConvertSUBToRUB (see A2, B2a and B2d).
 *                 This threshold indicates the minimum size (in sectors) of free space needed in a
 *                 sequential update block (SUB) to convert it to a random update block (RUB). RUBs
 *                 have more flexible write rules, at the expense of a longer merge time. If the
 *                 SUB is near full (few free sectors remaining), the SUB will me merged and a new
 *                 RUB will be started, instead of performing the conversion from SUB to RUB.
 *                 This threshold can be set as a percentage  (relative to number of sectors per
 *                 block) in fs_nand_ftl_priv.h :
 *                     @verbatim
 *                     #define FS_NAND_TH_PCT_CONVERT_SUB_TO_RUB
 *                     @endverbatim
 *                     Set higher than default -> Better overall write speed
 *                     Set lower  than default -> Better overall wear leveling
 *                 To take advantage of this threshold in flowchart's B2d, it must be set higher
 *                 than the value of ThSecGapCnt_PadSUB (see Note #2d). Otherwise, this threshold
 *                 won't have any effect.
 *           - (c) ThSecGapCnt_PadSUB (see B2b).
 *                 This threshold indicates the maximum size (in sectors) that can be skipped in a
 *                 sequential update block (SUB). Since each sector of a SUB must be written at a
 *                 single location (sector physical index == sector logical index), it is possible
 *                 to allow small gaps in the sequence. Larger gaps are more flexible, and can
 *                 improve the overall merge speed, at the cost of faster wear, since some sectors
 *                 are left empty between erase operations.
 *
 *                 This threshold can be set as a percentage  (relative to number of sectors per
 *                 block) in fs_nand_ftl_priv.h :
 *                     @verbatim
 *                     #define FS_NAND_TH_PCT_PAD_SUB
 *                     @endverbatim
 *                 Set higher than default -> Better overall write speed
 *                 Set lower  than default -> Better overall wear leveling
 *               - (1) Comparison with this threshold can assume that the gap will always be positive.
 *                     Since we are in a sequential update block (SUB), and that the sector has not
 *                     been written yet (see flowchart in note #1), the sector to write will
 *                     systematically have an offset greater or equal than the first free sector in
 *                     sequential update block (SUB).
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static FS_LB_QTY FS_NAND_FTL_SecWrInUB(FS_NAND_FTL         *p_nand_ftl,
                                       void                *p_src,
                                       FS_LB_QTY           sec_ix_logical,
                                       FS_LB_QTY           sec_cnt,
                                       FS_NAND_UB_SEC_DATA ub_sec_data,
                                       RTOS_ERR            *p_err)
{
  FS_LB_QTY               sec_base;
  FS_NAND_SEC_PER_BLK_QTY sec_offset_logical;
  FS_NAND_SEC_PER_BLK_QTY sec_cnt_in_ub;
  FS_NAND_UB_EXTRA_DATA   ub_extra_data;
  FS_NAND_BLK_QTY         blk_ix_logical;
  FS_NAND_UB_QTY          ub_ix;
  FS_LB_QTY               wr_cnt;

  sec_base = FS_NAND_SEC_IX_TO_BLK_IX(p_nand_ftl, sec_ix_logical);
  sec_base = FS_NAND_BLK_IX_TO_SEC_IX(p_nand_ftl, sec_base);
  sec_offset_logical = sec_ix_logical - sec_base;

  ub_extra_data = p_nand_ftl->UB_ExtraDataTbl[ub_sec_data.UB_Ix];
  blk_ix_logical = FS_NAND_SEC_IX_TO_BLK_IX(p_nand_ftl, sec_ix_logical);

  if (sec_offset_logical == 0u) {
    //                                                             ----------- FIRST SEC OF LOGICAL BLK (A) -----------
    //                                                             Chk for associated UB.
    if (ub_sec_data.UB_Ix != FS_NAND_UB_IX_INVALID) {
      //                                                           -------------- FIRST SEC + UB EXISTS ---------------
      if (ub_extra_data.AssocLvl != 0u) {
        //                                                         ----------- FIRST SEC + RUB EXISTS (A1) ------------
        //                                                         Chk wr cnt to determine if a SUB should be started.
        if (sec_cnt < p_nand_ftl->ThSecWrCnt_MergeRUBStartSUB) {
          //                                                       -------- FIRST SEC + RUB + LO WR CNT (A1a) ---------
          //                                                       Chk if space is left in RUB.
          if (ub_extra_data.NextSecIx < p_nand_ftl->NbrSecPerBlk) {
            //                                                     ---- FIRST SEC + RUB NOT FULL + LO WR CNT (A1d) ----
            //                                                     Wr sec in RUB.
            wr_cnt = FS_NAND_FTL_SecWrInRUB(p_nand_ftl,
                                            p_src,
                                            sec_ix_logical,
                                            sec_cnt,
                                            ub_sec_data.UB_Ix,
                                            p_err);

            if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
              LOG_ERR(("Error writing sector in update block ", (u)ub_sec_data.UB_Ix, "."));
              return (wr_cnt);
            }

            return (wr_cnt);
          } else {
            //                                                     ------ FIRST SEC + RUB FULL + LO WR CNT (A1c) ------
            //                                                     Merge RUB.
            FS_NAND_FTL_RUB_Merge(p_nand_ftl, ub_sec_data.UB_Ix, p_err);

            if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
              LOG_ERR(("Error performing full merge of update block ", (u)ub_sec_data.UB_Ix, "."));
              return (0u);
            }

            //                                                     Alloc SUB for LB.
            ub_ix = FS_NAND_FTL_SUB_Alloc(p_nand_ftl, blk_ix_logical, p_err);

            if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
              LOG_ERR(("Error allocating new sequential update block."));
              return (0u);
            }

            //                                                     Wr in SUB.
            wr_cnt = FS_NAND_FTL_SecWrInSUB(p_nand_ftl,
                                            p_src,
                                            sec_ix_logical,
                                            sec_cnt,
                                            ub_ix,
                                            p_err);

            if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
              LOG_ERR(("Error writing in sequential update block ", (u)ub_ix, "."));
              return (wr_cnt);
            }
            return (wr_cnt);
          }
        } else {
          //                                                       -------- FIRST SEC + RUB + HI WR CNT (A1b) ---------
          //                                                       Partial merge RUB.
          FS_NAND_FTL_RUB_PartialMerge(p_nand_ftl,
                                       ub_sec_data.UB_Ix,
                                       blk_ix_logical,
                                       p_err);

          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            LOG_ERR(("Error performing partial merge of update block ", (u)ub_sec_data.UB_Ix,
                     " (Assoc blk ix ", (u)ub_sec_data.AssocLogicalBlksTblIx, ")."));
            return (0u);
          }

          //                                                       Create SUB.
          ub_ix = FS_NAND_FTL_SUB_Alloc(p_nand_ftl, blk_ix_logical, p_err);

          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            LOG_ERR(("Error allocating new sequential update block."));
            return (0u);
          }

          //                                                       Wr sec in SUB.
          wr_cnt = FS_NAND_FTL_SecWrInSUB(p_nand_ftl,
                                          p_src,
                                          sec_ix_logical,
                                          sec_cnt,
                                          ub_ix,
                                          p_err);

          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            LOG_ERR(("Error writing sector in sequential update block ", (u)ub_ix));

            return (wr_cnt);
          }

          return (wr_cnt);
        }
      } else {
        //                                                         ----------- FIRST SEC + SUB EXISTS (A2) ------------
        sec_cnt_in_ub = p_nand_ftl->NbrSecPerBlk - ub_extra_data.NextSecIx;
        if (sec_cnt_in_ub > p_nand_ftl->ThSecRemCnt_ConvertSUBToRUB) {
          //                                                       ------- FIRST SEC + SUB + HI FREE SEC (A2a) --------
          FS_NAND_FTL_UB_IncAssoc(p_nand_ftl,                   // Convert SUB to RUB with k=1.
                                  ub_sec_data.UB_Ix,
                                  ub_extra_data.AssocLogicalBlksTbl[0]);

          p_nand_ftl->SUB_Cnt--;

          //                                                       Wr sec in RUB.
          wr_cnt = FS_NAND_FTL_SecWrInRUB(p_nand_ftl,
                                          p_src,
                                          sec_ix_logical,
                                          sec_cnt,
                                          ub_sec_data.UB_Ix,
                                          p_err);

          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            LOG_ERR(("Error writing sector in random update block ", (u)ub_sec_data.UB_Ix));
            return (wr_cnt);
          }

          return (wr_cnt);
        } else {
          //                                                       ----- FIRST SEC + SUB + HI WR CNT OR FULL(A2b) -----
          if (sec_cnt < ub_extra_data.NextSecIx) {              // Do not merge SUB if all sec will be overwritten.
            FS_NAND_FTL_SUB_Merge(p_nand_ftl,
                                  ub_sec_data.UB_Ix,
                                  p_err);

            if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
              LOG_ERR(("Error performing full merge of update block ", (u)ub_sec_data.UB_Ix, "."));
              return (0u);
            }
          }

          //                                                       Alloc SUB.
          ub_ix = FS_NAND_FTL_SUB_Alloc(p_nand_ftl,
                                        blk_ix_logical,
                                        p_err);

          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            LOG_ERR(("Unable to allocate sequential update block."));
            return (0u);
          }

          //                                                       Wr sec in SUB.
          wr_cnt = FS_NAND_FTL_SecWrInSUB(p_nand_ftl,
                                          p_src,
                                          sec_ix_logical,
                                          sec_cnt,
                                          ub_ix,
                                          p_err);

          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            LOG_ERR(("Error writing sector in sequential update block ", (u)ub_ix));
            return (wr_cnt);
          }

          return (wr_cnt);
        }
      }
    } else {
      //                                                           -------------- FIRST SEC + NO UB (A3) --------------
      //                                                           Alloc SUB.
      ub_ix = FS_NAND_FTL_SUB_Alloc(p_nand_ftl,
                                    blk_ix_logical,
                                    p_err);

      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        LOG_ERR(("Unable to allocate sequential update block."));
        return (0u);
      }

      //                                                           Wr sec in SUB.
      wr_cnt = FS_NAND_FTL_SecWrInSUB(p_nand_ftl,
                                      p_src,
                                      sec_ix_logical,
                                      sec_cnt,
                                      ub_ix,
                                      p_err);

      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        LOG_ERR(("Error writing sector in sequential update block ", (u)ub_ix, "."));
        return (wr_cnt);
      }

      return (wr_cnt);
    }
  } else {
    //                                                             --------- NOT FIRST SEC OF LOGICAL BLK (B)----------
    //                                                             Chk for associated UB.
    if (ub_sec_data.UB_Ix != FS_NAND_UB_IX_INVALID) {
      //                                                           ----------------- NFS + UB EXISTS ------------------
      if (ub_extra_data.AssocLvl > 0u) {
        //                                                         -------------- NFS + RUB EXISTS (B1) ---------------
        if (ub_extra_data.NextSecIx < p_nand_ftl->NbrSecPerBlk) {
          //                                                       ----------- NFS + RUB EXISTS + FREE SEC ------------
          //                                                       Wr sec in RUB.
          wr_cnt = FS_NAND_FTL_SecWrInRUB(p_nand_ftl,
                                          p_src,
                                          sec_ix_logical,
                                          sec_cnt,
                                          ub_sec_data.UB_Ix,
                                          p_err);

          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            LOG_ERR(("Error writing sector in random update block ", (u)ub_sec_data.UB_Ix));
            return (wr_cnt);
          }

          return (wr_cnt);
        } else {
          //                                                       ------------- NFS + RUB EXISTS + FULL --------------
          //                                                       Merge RUB.
          FS_NAND_FTL_RUB_Merge(p_nand_ftl,
                                ub_sec_data.UB_Ix,
                                p_err);

          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            LOG_ERR(("Error performing full merge of update block ", (u)ub_sec_data.UB_Ix));
            return (0u);
          }

          //                                                       Alloc RUB for logical blk.
          ub_ix = FS_NAND_FTL_RUB_Alloc(p_nand_ftl,
                                        blk_ix_logical,
                                        p_err);

          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            LOG_ERR(("Unable to allocate new random update block.")); \
            return (0u);
          }

          //                                                       Wr in RUB.
          ub_extra_data = p_nand_ftl->UB_ExtraDataTbl[ub_ix];

          wr_cnt = FS_NAND_FTL_SecWrInRUB(p_nand_ftl,
                                          p_src,
                                          sec_ix_logical,
                                          sec_cnt,
                                          ub_ix,
                                          p_err);

          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            LOG_ERR(("Error writing sector in random update block ", (u)ub_ix, "."));

            return (wr_cnt);
          }

          return (wr_cnt);
        }
      } else {
        //                                                         -------------- NFS + SUB ALLOC'D (B2) --------------
        //                                                         Chk if sec is wr'en.
        if (sec_offset_logical < ub_extra_data.NextSecIx) {
          //                                                       ----------- NFS + SUB + SEC WR'EN (B2a) ------------
          //                                                       Compare free sec cnt with threshold.
          sec_cnt_in_ub = p_nand_ftl->NbrSecPerBlk - ub_extra_data.NextSecIx;
          if (sec_cnt_in_ub > p_nand_ftl->ThSecRemCnt_ConvertSUBToRUB) {
            //                                                     ---- NFS + SUB + SEC WR'EN + LO FREE SEC (B2a2) ----
            //                                                     Change SUB in RUB with k=1.
            FS_NAND_FTL_UB_IncAssoc(p_nand_ftl,
                                    ub_sec_data.UB_Ix,
                                    blk_ix_logical);

            p_nand_ftl->SUB_Cnt--;

            //                                                     Wr in RUB.
            wr_cnt = FS_NAND_FTL_SecWrInRUB(p_nand_ftl,
                                            p_src,
                                            sec_ix_logical,
                                            sec_cnt,
                                            ub_sec_data.UB_Ix,
                                            p_err);

            if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
              LOG_ERR(("Error writing sector in random update blk ", (u)ub_sec_data.UB_Ix, "."));
              return (wr_cnt);
            }

            return (wr_cnt);
          } else {
            //                                                     ---- NFS + SUB + SEC WR'EN + HI FREE SEC (B2a1) ----
            //                                                     Merge SUB.
            FS_NAND_FTL_SUB_Merge(p_nand_ftl,
                                  ub_sec_data.UB_Ix,
                                  p_err);

            if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
              LOG_ERR(("Error performing merge of sequential update block ", (u)ub_sec_data.UB_Ix, "."));

              return (0u);
            }

            //                                                     Alloc RUB for logical blk.
            ub_ix = FS_NAND_FTL_RUB_Alloc(p_nand_ftl,
                                          blk_ix_logical,
                                          p_err);

            if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
              LOG_ERR(("Unable to allocate new random update block."));

              return (0u);
            }

            //                                                     Wr sec in RUB.
            ub_extra_data = p_nand_ftl->UB_ExtraDataTbl[ub_ix];

            wr_cnt = FS_NAND_FTL_SecWrInRUB(p_nand_ftl,
                                            p_src,
                                            sec_ix_logical,
                                            sec_cnt,
                                            ub_ix,
                                            p_err);

            if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
              LOG_ERR(("Error writing to random update block ", (u)ub_ix, "."));

              return (wr_cnt);
            }

            return (wr_cnt);
          }
        } else {
          //                                                       ------------ NFS + SUB + SEC FREE (B2b) ------------
          //                                                       Chk gap size.
          sec_cnt_in_ub = sec_offset_logical - ub_extra_data.NextSecIx;
          //                                                       sec_cnt can't be negative (see note #2d1).
          if (sec_cnt_in_ub < p_nand_ftl->ThSecGapCnt_PadSUB) {
            //                                                     ------ NFS + SUB + SEC FREE + SMALL GAP (B2c) ------
            //                                                     Pad SUB until sec with logical blk data.
            FS_NAND_FTL_SUB_MergeUntil(p_nand_ftl,
                                       ub_sec_data.UB_Ix,
                                       sec_offset_logical - 1u,
                                       p_err);
            if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
              LOG_ERR(("Error performing merge of sequential update block ", (u)ub_sec_data.UB_Ix, "."));
              return (0u);
            }

            //                                                     Wr sec in SUB.
            wr_cnt = FS_NAND_FTL_SecWrInSUB(p_nand_ftl,
                                            p_src,
                                            sec_ix_logical,
                                            sec_cnt,
                                            ub_sec_data.UB_Ix,
                                            p_err);

            if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
              LOG_ERR(("Error writing sector in random update block", (u)ub_sec_data.UB_Ix, "."));
              return (wr_cnt);
            }

            return (wr_cnt);
          } else {
            //                                                     ------ NFS + SUB + SEC FREE + LARGE GAP (B2d) ------
            sec_cnt_in_ub = p_nand_ftl->NbrSecPerBlk - ub_extra_data.NextSecIx;

#if (FS_NAND_TH_PCT_CONVERT_SUB_TO_RUB > FS_NAND_TH_PCT_PAD_SUB)
            if (sec_cnt_in_ub > p_nand_ftl->ThSecRemCnt_ConvertSUBToRUB) {
#endif

            //                                                     ---------- [B2d] + HI FREE SEC CNT (B2d1) ----------
            //                                                     Change SUB in RUB with k=1.
            FS_NAND_FTL_UB_IncAssoc(p_nand_ftl,
                                    ub_sec_data.UB_Ix,
                                    blk_ix_logical);

            p_nand_ftl->SUB_Cnt--;

            //                                                     Wr sec in RUB.
            wr_cnt = FS_NAND_FTL_SecWrInRUB(p_nand_ftl,
                                            p_src,
                                            sec_ix_logical,
                                            sec_cnt,
                                            ub_sec_data.UB_Ix,
                                            p_err);

            if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
              LOG_ERR(("Error writing sector in random update block ", (u)ub_sec_data.UB_Ix, "."));
              return (wr_cnt);
            }

            return (wr_cnt);

#if (FS_NAND_TH_PCT_CONVERT_SUB_TO_RUB > FS_NAND_TH_PCT_PAD_SUB)
          } else {
            //                                                     ---------- [B2d] + LO FREE SEC CNT (B2d2) ----------
            //                                                     Wr sec in SUB.
            wr_cnt = FS_NAND_FTL_SecWrInSUB(p_nand_ftl,
                                            p_src,
                                            sec_ix_logical,
                                            sec_cnt,
                                            ub_sec_data.UB_Ix,
                                            p_err);

            if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
              LOG_ERR(("Error writing sector in sequential update block ", (u)ub_sec_data.UB_Ix, "."));
              return (wr_cnt);
            }

            return (wr_cnt);
          }
#endif
          }
        }
      }
    } else {
      //                                                           ------------- NFS + NO UB EXISTS (B3) --------------
      //                                                           Alloc RUB for logical blk.
      ub_ix = FS_NAND_FTL_RUB_Alloc(p_nand_ftl,
                                    blk_ix_logical,
                                    p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        LOG_ERR(("Unable to alloc random update block for logical block index ", (u)blk_ix_logical));
        return (0u);
      }

      //                                                           Wr sec in RUB.
      ub_extra_data = p_nand_ftl->UB_ExtraDataTbl[ub_ix];

      wr_cnt = FS_NAND_FTL_SecWrInRUB(p_nand_ftl,
                                      p_src,
                                      sec_ix_logical,
                                      sec_cnt,
                                      ub_ix,
                                      p_err);

      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        LOG_ERR(("Unable to write sector ", (u)sec_ix_logical, " in update block."));
        return (wr_cnt);
      }

      return (wr_cnt);
    }
  }
}
#endif

/****************************************************************************************************//**
 *                                           FS_NAND_FTL_SecWrInRUB()
 *
 * @brief    Write one logical sector in a random update block.
 *
 * @param    p_nand_ftl      Pointer to NAND FTL.
 *
 * @param    p_src           Pointer to source buffer.
 *
 * @param    sec_ix_logical  Sector's logical index.
 *
 * @param    sec_cnt         Number of consecutive sectors to write (unused).
 *
 * @param    ub_ix           Update block's index.
 *
 * @param    p_err           Error pointer.
 *
 * @return   Number of sectors written. Might not be equal to sec_cnt.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static FS_LB_QTY FS_NAND_FTL_SecWrInRUB(FS_NAND_FTL    *p_nand_ftl,
                                        void           *p_src,
                                        FS_LB_QTY      sec_ix_logical,
                                        FS_LB_QTY      sec_cnt,
                                        FS_NAND_UB_QTY ub_ix,
                                        RTOS_ERR       *p_err)
{
  FS_NAND_BLK_QTY blk_ix_logical;
  FS_NAND_SEC_PER_BLK_QTY sec_offset_logical;
  FS_NAND_UB_DATA p_data;
  FS_NAND_UB_EXTRA_DATA *p_extra_data;
  FS_NAND_UB_SEC_DATA ub_sec_data;
  FS_NAND_BLK_QTY blk_ix_phy;
  FS_NAND_BLK_QTY blk_ix_log;
#if (FS_NAND_CFG_UB_TBL_SUBSET_SIZE != 0)
  CPU_SIZE_T loc_octet_array;
  CPU_SIZE_T pos_bit_array;
  CPU_DATA loc_bit_octet;
  FS_NAND_SEC_PER_BLK_QTY sec_subset_ix;
#endif

  PP_UNUSED_PARAM(sec_cnt);
  blk_ix_logical = FS_NAND_SEC_IX_TO_BLK_IX(p_nand_ftl, sec_ix_logical);
  sec_offset_logical = sec_ix_logical - FS_NAND_BLK_IX_TO_SEC_IX(p_nand_ftl, blk_ix_logical);
  p_extra_data = &p_nand_ftl->UB_ExtraDataTbl[ub_ix];

  LOG_VRB(("Wr sector ", (u)sec_ix_logical, " in RUB ", (u)ub_ix, " at sec offset ", (u)p_extra_data->NextSecIx));

  if (p_extra_data->NextSecIx >= p_nand_ftl->NbrSecPerBlk) {
    LOG_ERR(("Next empty sector index (", (u)p_extra_data->NextSecIx, ") has overflowed the number of available sectors (", (u)p_nand_ftl->NbrSecPerBlk, ") within a block."));
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return (0u);
  }

  //                                                               ----------------- FIND OLD UB SEC ------------------
  ub_sec_data.AssocLogicalBlksTblIx = FS_NAND_FTL_RUB_AssocBlkIxGet(p_nand_ftl, ub_ix, blk_ix_logical);

  if (ub_sec_data.AssocLogicalBlksTblIx == FS_NAND_ASSOC_BLK_IX_INVALID) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_BLK_DEV_CORRUPTED);
    return (0u);
  }

  ub_sec_data.UB_Ix = ub_ix;

  ub_sec_data = FS_NAND_FTL_UB_SecFind(p_nand_ftl,
                                       ub_sec_data,
                                       sec_offset_logical,
                                       p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Error searching for sector with logical offset ", (u)sec_offset_logical, " in update blk ", (u)ub_sec_data.UB_Ix));
    return (0u);
  }

  do {                                                          // Until sec wr'en successfully.
                                                                // ------------------ CALC OOS DATA -------------------
    blk_ix_log = FS_NAND_UB_IX_TO_LOG_BLK_IX(p_nand_ftl, ub_ix);
    blk_ix_phy = FS_NAND_FTL_BlkIxPhyGet(p_nand_ftl, blk_ix_log);
    if (blk_ix_phy == FS_NAND_BLK_IX_INVALID) {
      LOG_ERR(("Index of physical block (blk_ix_phy) is invalid."));
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
      return (0u);
    }

    FS_NAND_FTL_OOSGenSto(p_nand_ftl,
                          p_nand_ftl->OOS_BufPtr,
                          blk_ix_logical,
                          blk_ix_phy,
                          sec_offset_logical,
                          p_extra_data->NextSecIx,
                          p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (0u);
    }

    //                                                             ---------------------- WR SEC ----------------------
    blk_ix_log = FS_NAND_UB_IX_TO_LOG_BLK_IX(p_nand_ftl, ub_ix);
    FS_NAND_FTL_SecWrHandler(p_nand_ftl,
                             p_src,
                             p_nand_ftl->OOS_BufPtr,
                             blk_ix_log,
                             p_extra_data->NextSecIx,
                             p_err);
  } while (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_ABORT);

  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  //                                                               ----------------- UPDATE METADATA ------------------
  //                                                               Update sec valid map.
  p_data = FS_NAND_FTL_UB_TblEntryRd(p_nand_ftl,
                                     ub_ix);

  Bitmap_BitSet(p_data.SecValidBitMap, p_extra_data->NextSecIx);

  //                                                               Invalidate old UB sector.
  if (ub_sec_data.SecOffsetPhy != FS_NAND_SEC_OFFSET_IX_INVALID) {
    Bitmap_BitClr(p_data.SecValidBitMap, ub_sec_data.SecOffsetPhy);
  }

  FS_NAND_FTL_UB_TblInvalidate(p_nand_ftl);

#if (FS_NAND_CFG_UB_TBL_SUBSET_SIZE != 0)
  //                                                               Update UB mapping tbl.
  pos_bit_array = ub_sec_data.AssocLogicalBlksTblIx * p_nand_ftl->NbrSecPerBlk * p_nand_ftl->UB_SecMapNbrBits;
  pos_bit_array += sec_offset_logical * p_nand_ftl->UB_SecMapNbrBits;

  FS_UTIL_BITMAP_LOC_GET(pos_bit_array, loc_octet_array, loc_bit_octet);

  sec_subset_ix = p_extra_data->NextSecIx / FS_NAND_CFG_UB_TBL_SUBSET_SIZE;

  FSUtil_ValPack32(p_extra_data->LogicalToPhySecMap,
                   &loc_octet_array,
                   &loc_bit_octet,
                   sec_subset_ix,
                   p_nand_ftl->UB_SecMapNbrBits);
#endif

#if (FS_NAND_CFG_UB_META_CACHE_EN == DEF_ENABLED)
  //                                                               Update UB meta cache.
  pos_bit_array = p_extra_data->NextSecIx * (p_nand_ftl->RUB_MaxAssocLog2 + p_nand_ftl->NbrSecPerBlkLog2);

  FS_UTIL_BITMAP_LOC_GET(pos_bit_array, loc_octet_array, loc_bit_octet);

  FSUtil_ValPack32(p_extra_data->MetaCachePtr,
                   &loc_octet_array,
                   &loc_bit_octet,
                   sec_offset_logical,
                   p_nand_ftl->NbrSecPerBlkLog2);

  FSUtil_ValPack32(p_extra_data->MetaCachePtr,
                   &loc_octet_array,
                   &loc_bit_octet,
                   ub_sec_data.AssocLogicalBlksTblIx,
                   p_nand_ftl->RUB_MaxAssocLog2);
#endif

  p_extra_data->NextSecIx++;

  p_extra_data->ActivityCtr = p_nand_ftl->ActivityCtr;          // Assign current activity ctr to UB.

  p_nand_ftl->ActivityCtr++;                                    // Inc global activity ctr.

  return (1u);
}
#endif

/****************************************************************************************************//**
 *                                           FS_NAND_FTL_SecWrInSUB()
 *
 * @brief    Write 1 or more logical sectors in a sequential update block (SUB).
 *
 * @param    p_nand_ftl      Pointer to NAND FTL.
 *
 * @param    p_src           Pointer to source buffer.
 *
 * @param    sec_ix_logical  Sector's logical index.
 *
 * @param    sec_cnt         Number of consecutive sectors to write.
 *
 * @param    ub_ix           Update block's index.
 *
 * @param    p_err           Error pointer.
 *
 * @return   Number of sectors written. Might not be equal to sec_cnt.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static FS_LB_QTY FS_NAND_FTL_SecWrInSUB(FS_NAND_FTL    *p_nand_ftl,
                                        void           *p_src,
                                        FS_LB_QTY      sec_ix_logical,
                                        FS_LB_QTY      sec_cnt,
                                        FS_NAND_UB_QTY ub_ix,
                                        RTOS_ERR       *p_err)
{
  FS_NAND_BLK_QTY data_blk_ix_logical;
  FS_NAND_BLK_QTY ub_ix_logical;
  FS_NAND_SEC_PER_BLK_QTY sec_offset_phy;
  FS_LB_QTY wr_cnt;
  FS_NAND_UB_DATA p_data;
  FS_NAND_UB_EXTRA_DATA *p_extra_data;
  FS_NAND_BLK_QTY blk_ix_phy;
#if (FS_NAND_CFG_UB_TBL_SUBSET_SIZE != 0)
  CPU_SIZE_T loc_octet_array;
  CPU_SIZE_T pos_bit_array;
  CPU_DATA loc_bit_octet;
  FS_NAND_SEC_PER_BLK_QTY sec_subset_ix;
#endif

  data_blk_ix_logical = FS_NAND_SEC_IX_TO_BLK_IX(p_nand_ftl, sec_ix_logical);
  sec_offset_phy = sec_ix_logical - FS_NAND_BLK_IX_TO_SEC_IX(p_nand_ftl, data_blk_ix_logical);
  wr_cnt = 0u;
  p_extra_data = &p_nand_ftl->UB_ExtraDataTbl[ub_ix];

  while ((wr_cnt < sec_cnt)                                     // Until all sec are wr'en or blk is full.
         && (sec_offset_phy < p_nand_ftl->NbrSecPerBlk)) {
    LOG_VRB(("Wr sector ", (u)sec_ix_logical + wr_cnt, " in SUB ", (u)ub_ix, " at sec offset ", (u)sec_offset_phy));

    do {                                                        // Until sec wr'en successfully.
                                                                // ------------------ CALC OOS DATA -------------------
      blk_ix_phy = FS_NAND_UB_IX_TO_LOG_BLK_IX(p_nand_ftl, ub_ix);
      blk_ix_phy = FS_NAND_FTL_BlkIxPhyGet(p_nand_ftl, blk_ix_phy);
      if (blk_ix_phy == FS_NAND_BLK_IX_INVALID) {
        LOG_ERR(("Index of physical block (blk_ix_phy) is invalid."));
        RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
        return (0u);
      }

      FS_NAND_FTL_OOSGenSto(p_nand_ftl,
                            p_nand_ftl->OOS_BufPtr,
                            data_blk_ix_logical,
                            blk_ix_phy,
                            sec_offset_phy,
                            sec_offset_phy,
                            p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return (0u);
      }

      //                                                           ---------------------- WR SEC ----------------------
      ub_ix_logical = FS_NAND_UB_IX_TO_LOG_BLK_IX(p_nand_ftl, ub_ix);
      FS_NAND_FTL_SecWrHandler(p_nand_ftl,
                               p_src,
                               p_nand_ftl->OOS_BufPtr,
                               ub_ix_logical,
                               sec_offset_phy,
                               p_err);
    } while (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_ABORT);

    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (wr_cnt);
    }
    //                                                             ----------------- UPDATE METADATA ------------------
    //                                                             Update sec valid map.
    p_data = FS_NAND_FTL_UB_TblEntryRd(p_nand_ftl,
                                       ub_ix);

    Bitmap_BitSet(p_data.SecValidBitMap, sec_offset_phy);

    FS_NAND_FTL_UB_TblInvalidate(p_nand_ftl);

#if (FS_NAND_CFG_UB_TBL_SUBSET_SIZE != 0)
    //                                                             Update UB mapping tbl.
    pos_bit_array = sec_offset_phy * p_nand_ftl->UB_SecMapNbrBits;

    FS_UTIL_BITMAP_LOC_GET(pos_bit_array, loc_octet_array, loc_bit_octet);

    sec_subset_ix = sec_offset_phy / FS_NAND_CFG_UB_TBL_SUBSET_SIZE;

    FSUtil_ValPack32(p_extra_data->LogicalToPhySecMap,
                     &loc_octet_array,
                     &loc_bit_octet,
                     sec_subset_ix,
                     p_nand_ftl->UB_SecMapNbrBits);
#endif

#if (FS_NAND_CFG_UB_META_CACHE_EN == DEF_ENABLED)
    //                                                             Update UB meta cache.
    pos_bit_array = sec_offset_phy * (p_nand_ftl->RUB_MaxAssocLog2 + p_nand_ftl->NbrSecPerBlkLog2);

    FS_UTIL_BITMAP_LOC_GET(pos_bit_array, loc_octet_array, loc_bit_octet);

    FSUtil_ValPack32(p_extra_data->MetaCachePtr,
                     &loc_octet_array,
                     &loc_bit_octet,
                     sec_offset_phy,
                     p_nand_ftl->NbrSecPerBlkLog2);

    FSUtil_ValPack32(p_extra_data->MetaCachePtr,
                     &loc_octet_array,
                     &loc_bit_octet,
                     0u,
                     p_nand_ftl->RUB_MaxAssocLog2);
#endif

    sec_offset_phy++;
    p_extra_data->NextSecIx = sec_offset_phy;

    wr_cnt++;
    p_src = (void *)((CPU_INT08U *)p_src + p_nand_ftl->SecSize);
  }

  p_extra_data->ActivityCtr = p_nand_ftl->ActivityCtr;          // Assign current activity ctr to UB.

  p_nand_ftl->ActivityCtr++;                                    // Inc global activity ctr.

  return (wr_cnt);
}
#endif

/****************************************************************************************************//**
 *                                           FS_NAND_FTL_OOSGenSto()
 *
 * @brief    Generate out of sector data for storage sector.
 *
 * @param    p_nand_ftl              Pointer to NAND FTL.
 *
 * @param    p_oos_buf_v             Pointer to buffer that will receive OOS data.
 *
 * @param    blk_ix_logical_data     Logical block index associated with data sector.
 *
 * @param    blk_ix_phy              Physical block index of block that will store the data sector.
 *
 * @param    sec_offset_logical      Sector offset relative to logical block.
 *
 * @param    sec_offset_phy          Sector offset relative to physical block.
 *
 * @param    p_err                   Error pointer.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_NAND_FTL_OOSGenSto(FS_NAND_FTL             *p_nand_ftl,
                                  void                    *p_oos_buf_v,
                                  FS_NAND_BLK_QTY         blk_ix_logical_data,
                                  FS_NAND_BLK_QTY         blk_ix_phy,
                                  FS_NAND_SEC_PER_BLK_QTY sec_offset_logical,
                                  FS_NAND_SEC_PER_BLK_QTY sec_offset_phy,
                                  RTOS_ERR                *p_err)
{
  CPU_INT08U *p_oos_buf_08;
  FS_NAND_SEC_TYPE_STO sec_type;
  FS_NAND_ERASE_QTY erase_cnt;

  PP_UNUSED_PARAM(p_err);
  p_oos_buf_08 = (CPU_INT08U *)p_oos_buf_v;

  Mem_Set(&p_oos_buf_08[FS_NAND_OOS_SEC_USED_OFFSET],           // Sec used mark.
          0x00u,
          p_nand_ftl->UsedMarkSize);

  //                                                               Sec type.
  sec_type = FS_NAND_SEC_TYPE_STORAGE;
  MEM_VAL_COPY_SET_INTU_LITTLE(&p_oos_buf_08[FS_NAND_OOS_SEC_TYPE_OFFSET],
                               &sec_type,
                               sizeof(FS_NAND_SEC_TYPE_STO));

  //                                                               Blk ix logical.
  MEM_VAL_COPY_SET_INTU_LITTLE(&p_oos_buf_08[FS_NAND_OOS_STO_LOGICAL_BLK_IX_OFFSET],
                               &blk_ix_logical_data,
                               sizeof(FS_NAND_BLK_QTY));

  //                                                               Blk sec ix.
  MEM_VAL_COPY_SET_INTU_LITTLE(&p_oos_buf_08[FS_NAND_OOS_STO_BLK_SEC_IX_OFFSET],
                               &sec_offset_logical,
                               sizeof(FS_NAND_SEC_PER_BLK_QTY));

  //                                                               Erase cnt.
  if (sec_offset_phy == 0u) {
    //                                                             Find entry in avail blk tbl.
    erase_cnt = FS_NAND_FTL_BlkRemFromAvail(p_nand_ftl, blk_ix_phy);

    //                                                             Wr erase cnt in OOS.
    MEM_VAL_COPY_SET_INTU_LITTLE(&p_oos_buf_08[FS_NAND_OOS_ERASE_CNT_OFFSET],
                                 &erase_cnt,
                                 sizeof(FS_NAND_ERASE_QTY));
  }
}
#endif

/****************************************************************************************************//**
 *                                           FS_NAND_FTL_SecIsUsed()
 *
 * @brief    Determine if specified sector is used.
 *
 * @param    p_nand_ftl  Pointer to NAND FTL.
 *
 * @param    sec_ix_phy  Physical sector index.
 *
 * @param    p_err       Error pointer.
 *
 * @return   DEF_YES, if sector is used,
 *           DEF_NO , otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN FS_NAND_FTL_SecIsUsed(FS_NAND_FTL *p_nand_ftl,
                                         FS_LB_NBR   sec_ix_phy,
                                         RTOS_ERR    *p_err)
{
  FS_NAND *p_nand;
  FS_NAND_CTRLR *p_ctrlr;
  CPU_INT32U sec_used_mark;
  CPU_INT08U set_bit_cnt;

  p_nand = (FS_NAND *)p_nand_ftl->BlkDev.MediaPtr;
  p_ctrlr = p_nand->CtrlrPtr;

  sec_used_mark = 0;
  p_ctrlr->CtrlrApiPtr->OOSRdRaw(p_ctrlr,
                                 &sec_used_mark,
                                 sec_ix_phy,
                                 FS_NAND_OOS_SEC_USED_OFFSET,
                                 p_nand_ftl->UsedMarkSize,
                                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NO);
  }

  //                                                               Chk used mark.
  set_bit_cnt = CRCUtil_PopCnt_32(sec_used_mark);
  if (set_bit_cnt < (p_nand_ftl->UsedMarkSize * DEF_INT_08_NBR_BITS / 2u)) {
    return (DEF_YES);
  } else {
    return (DEF_NO);
  }
}

/****************************************************************************************************//**
 *                                           FS_NAND_FTL_CalcDevInfo()
 *
 * @brief    Calculate NAND device data.
 *
 * @param    p_nand      Pointer to a NAND instance.
 *
 * @param    p_nand_ftl  Pointer to NAND FTL.
 *
 * @param    p_err       Error pointer.
 *
 * @note     (1) Block & block region information supplied by the phy driver are strictly checked :
 *               - (a) At least one block must exist.
 *               - (b) The last block number should not exceed the maximum blks number.
 *               - (c) The block size must NOT be smaller than the page   size.
 *               - (d) The page  size must NOT be smaller than the sector size.
 *               - (e) The block size MUST be a multiple of the    page   size.
 *               - (f) The page  size MUST be a multiple of the    sector size.
 *******************************************************************************************************/
static void FS_NAND_FTL_CalcDevInfo(FS_NAND     *p_nand,
                                    FS_NAND_FTL *p_nand_ftl,
                                    RTOS_ERR    *p_err)
{
  FS_NAND_CTRLR *p_ctrlr;
  FS_NAND_PART_PARAM *p_part_info;
  FS_LB_QTY nbr_sec;
  CPU_INT32S nbr_blk;
  CPU_INT08U sec_per_pg;
  FS_LB_QTY sec_per_blk;

  p_ctrlr = p_nand->CtrlrPtr;
  p_part_info = p_ctrlr->CtrlrApiPtr->PartInfoGet(p_ctrlr);

  //                                                               --------- CALC NBR OF LOGICAL BLK TO REPORT --------
  nbr_blk = p_nand_ftl->BlkCnt;
  nbr_blk -= p_part_info->MaxBadBlkCnt;                         // Potential bad blks.
  nbr_blk -= p_nand_ftl->UB_CntMax;                             // UBs.
  nbr_blk -= FS_NAND_HDR_BLK_NBR;                               // Hdr blk.
  nbr_blk -= FS_NAND_VALID_META_BLK_NBR;                        // Meta blk.
  nbr_blk -= FS_NAND_CFG_RSVD_AVAIL_BLK_CNT;                    // Rsvd avail blks.
  nbr_blk -= p_nand_ftl->RUB_MaxAssoc;                          // Blks needed for largest merge ops.

  if (nbr_blk <= 0) {
    LOG_ERR(("There are not enough blks accessible by uC/FS."));
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_CFG);
    return;
  }

  sec_per_pg = (CPU_INT08U)(p_part_info->PgSize / p_nand_ftl->SecSize);
  sec_per_blk = sec_per_pg * p_part_info->PgPerBlk;
  nbr_sec = (CPU_INT32U)nbr_blk    * sec_per_blk;

  p_nand_ftl->SecCnt = nbr_sec;
}

/****************************************************************************************************//**
 *                                       FS_NAND_FTL_AllocDevData()
 *
 * @brief    Allocate NAND device data.
 *
 * @param    p_nand          Pointer to a NAND instance.
 *
 * @param    p_nand_ftl      Pointer to NAND FTL.
 *
 * @param    p_seg           Pointer to a memory segment.
 *
 * @param    p_err           Error pointer.
 *
 * @note    (1) The size of the available blocks table must be inferior or equal to the size of
 *              one sector. This restriction is necessary to insure that committing the available
 *              blocks table on device is an atomic operation that could not be half-way through
 *              if an unexpected power-loss occurs. The user can either set sector size to a
 *              higher value, or reduce the number of entries in the available blocks table.
 *              - (a) The available blocks table must be located at the beginning of the metadata and
 *                    its size restricted to one sector to make the search for it trivial : it will
 *                    always be contained in the first sector of metadata.
 *******************************************************************************************************/
static void FS_NAND_FTL_AllocDevData(FS_NAND     *p_nand,
                                     FS_NAND_FTL *p_nand_ftl,
                                     MEM_SEG     *p_seg,
                                     RTOS_ERR    *p_err)
{
  FS_NAND_CTRLR *p_ctrlr;
  FS_NAND_PART_PARAM *p_part_info;
  FS_NAND_UB_EXTRA_DATA *p_tbl_entry;
  CPU_SIZE_T align_req;
  CPU_SIZE_T octets_reqd;
  CPU_INT16U meta_invalid_map_size;
  CPU_INT16U blk_bitmap_size;
  CPU_INT16U bad_blk_tbl_size;
  CPU_INT16U avail_blk_tbl_size;
  CPU_INT08U avail_blk_tbl_entry_size;
  CPU_INT16U ub_bitmap_size;
  CPU_INT16U total_metadata_size;
  CPU_INT16U blk_sec_bitmap_size;
  FS_NAND_BLK_QTY tbl_ix;
  CPU_SIZE_T ub_sec_map_size;
  CPU_SIZE_T ub_meta_cache_size;

  p_ctrlr = p_nand->CtrlrPtr;

  //                                                               ------------------- ALLOC L2P TBL ------------------
  p_nand_ftl->LogicalToPhyBlkMap = (FS_NAND_BLK_QTY *)Mem_SegAlloc("FS - NAND log to phy blk map",
                                                                   p_seg,
                                                                   sizeof(FS_NAND_BLK_QTY) * p_nand_ftl->LogicalDataBlkCnt,
                                                                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               ------------- DETERMINE METADATA INFO --------------
  //                                                               Avail blk tbl at beginning of meta (see Note #1b).
  avail_blk_tbl_entry_size = sizeof(FS_NAND_BLK_QTY) + sizeof(FS_NAND_ERASE_QTY);
  avail_blk_tbl_size = p_nand_ftl->AvailBlkTblEntryCntMax * avail_blk_tbl_entry_size;
  p_nand_ftl->MetaOffsetAvailBlkTbl = 0u;

  //                                                               Check avail blk tbl size (see Note #1a).
  RTOS_ASSERT_DBG(avail_blk_tbl_size <= p_nand_ftl->SecSize, RTOS_ERR_INVALID_CFG,; );

  p_part_info = p_ctrlr->CtrlrApiPtr->PartInfoGet(p_ctrlr);

  //                                                               Bad blk tbl.
  bad_blk_tbl_size = p_part_info->MaxBadBlkCnt * sizeof(FS_NAND_BLK_QTY);
  p_nand_ftl->MetaOffsetBadBlkTbl = avail_blk_tbl_size;

  //                                                               Dirty blk bitmap.
  blk_bitmap_size = FS_UTIL_BIT_NBR_TO_OCTET_NBR(p_nand_ftl->BlkCnt);

#if (FS_NAND_CFG_DIRTY_MAP_CACHE_EN == DEF_ENABLED)
  p_nand_ftl->DirtyBitmapSize = blk_bitmap_size;
#endif

  p_nand_ftl->MetaOffsetDirtyBitmap = p_nand_ftl->MetaOffsetBadBlkTbl + bad_blk_tbl_size;

  //                                                               Update blk sec bitmaps.
  blk_sec_bitmap_size = FS_UTIL_BIT_NBR_TO_OCTET_NBR(p_nand_ftl->NbrSecPerBlk);

  ub_bitmap_size = blk_sec_bitmap_size + sizeof(FS_NAND_BLK_QTY);
  ub_bitmap_size *= p_nand_ftl->UB_CntMax;
  p_nand_ftl->MetaOffsetUB_Tbl = p_nand_ftl->MetaOffsetDirtyBitmap + blk_bitmap_size;

  //                                                               Check total size needed
  total_metadata_size = bad_blk_tbl_size
                        + avail_blk_tbl_size
                        + blk_bitmap_size
                        + ub_bitmap_size;

  p_nand_ftl->MetaSize = total_metadata_size;

  p_nand_ftl->MetaSecCnt = total_metadata_size / p_nand_ftl->SecSize;
  p_nand_ftl->MetaSecCnt += (total_metadata_size % p_nand_ftl->SecSize == 0u) ? 0u : 1u;

  //                                                               -------------- ALLOC METADATA CACHE ----------------
  p_nand_ftl->MetaCache = (CPU_INT08U *)Mem_SegAlloc("FS - NAND meta cache",
                                                     p_seg,
                                                     total_metadata_size,
                                                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               ------------ ALLOC DIRTY BITMAP CACHE --------------
#if (FS_NAND_CFG_DIRTY_MAP_CACHE_EN == DEF_ENABLED)
  p_nand_ftl->DirtyBitmapCache = (CPU_INT08U *)Mem_SegAlloc("FS - NAND dirty bitmap cache",
                                                            p_seg,
                                                            blk_bitmap_size,
                                                            p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
#endif

  //                                                               ---------- ALLOC AVAIL BLK TBL COMMIT MAP ----------
  p_nand_ftl->AvailBlkTblCommitMap = (CPU_INT08U *)Mem_SegAlloc("FS - NAND avail blk commit map",
                                                                p_seg,
                                                                FS_UTIL_BIT_NBR_TO_OCTET_NBR(p_nand_ftl->AvailBlkTblEntryCntMax),
                                                                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_nand_ftl->AvailBlkMetaMap = (CPU_INT08U *)Mem_SegAlloc("FS - NAND avail meta blk map",
                                                           p_seg,
                                                           FS_UTIL_BIT_NBR_TO_OCTET_NBR(p_nand_ftl->AvailBlkTblEntryCntMax),
                                                           p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_nand_ftl->AvailBlkMetaID_Tbl = (FS_NAND_META_ID *)Mem_SegAlloc("FS - Avail meta blk ID tbl",
                                                                   p_seg,
                                                                   sizeof(FS_NAND_META_ID) * p_nand_ftl->AvailBlkTblEntryCntMax,
                                                                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               ------------ ALLOC METADATA INVALID MAP ------------
  meta_invalid_map_size = p_nand_ftl->MetaSecCnt / DEF_OCTET_NBR_BITS;
  meta_invalid_map_size += (p_nand_ftl->MetaSecCnt % DEF_OCTET_NBR_BITS) == 0u ? 0u : 1u;
  p_nand_ftl->MetaBlkInvalidSecMap = (CPU_INT08U *)Mem_SegAlloc("FS - NAND meta blk invalid sec map",
                                                                p_seg,
                                                                sizeof(CPU_INT08U) * meta_invalid_map_size,
                                                                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               ---------------- ALLOC SECTOR BUF ------------------
  align_req = p_nand->Media.PmItemPtr->MediaApiPtr->AlignReqGet(&p_nand->Media, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_nand_ftl->BufPtr = Mem_SegAllocExt("FS - NAND buffer",
                                       p_seg,
                                       sizeof(CPU_INT08U) * p_nand_ftl->SecSize,
                                       align_req,
                                       &octets_reqd,
                                       p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Could not alloc mem for data buf: ", (u)octets_reqd, " octets req'd."));
    return;
  }

  //                                                               ------------- ALLOC UB EXTRA DATA TBL --------------
  p_nand_ftl->UB_ExtraDataTbl = (FS_NAND_UB_EXTRA_DATA *)Mem_SegAlloc("FS - NAND UB extra data tbl",
                                                                      p_seg,
                                                                      sizeof(FS_NAND_UB_EXTRA_DATA) * p_nand_ftl->UB_CntMax,
                                                                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  ub_sec_map_size = p_nand_ftl->NbrSecPerBlk * p_nand_ftl->UB_SecMapNbrBits;
  ub_sec_map_size *= p_nand_ftl->RUB_MaxAssoc;
  ub_sec_map_size = FS_UTIL_BIT_NBR_TO_OCTET_NBR(ub_sec_map_size);

  ub_meta_cache_size = p_nand_ftl->NbrSecPerBlkLog2 + p_nand_ftl->RUB_MaxAssocLog2;
  ub_meta_cache_size *= p_nand_ftl->NbrSecPerBlk;
  ub_meta_cache_size = FS_UTIL_BIT_NBR_TO_OCTET_NBR(ub_meta_cache_size);

  //                                                               For each entry...
  for (tbl_ix = 0u; tbl_ix < p_nand_ftl->UB_CntMax; tbl_ix++) {
    p_tbl_entry = &p_nand_ftl->UB_ExtraDataTbl[tbl_ix];

    //                                                             Alloc associated blk tbl.
    p_tbl_entry->AssocLogicalBlksTbl = (FS_NAND_BLK_QTY *)Mem_SegAlloc("FS - NAND assoc log blk tbl",
                                                                       p_seg,
                                                                       sizeof(FS_NAND_BLK_QTY) * p_nand_ftl->RUB_MaxAssoc,
                                                                       p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      LOG_ERR(("Could not alloc mem for UB ", (u)tbl_ix, "'s associated blocks table: ", (u)octets_reqd, " octets req'd."));
      return;
    }

    //                                                             Init associated blk tbl.
    Mem_Set(&p_tbl_entry->AssocLogicalBlksTbl[0], 0xFFu, sizeof(FS_NAND_BLK_QTY) * p_nand_ftl->RUB_MaxAssoc);

    if (ub_sec_map_size != 0u) {                                // If size if non-null ...
                                                                // Alloc sec mapping tbl.
      p_tbl_entry->LogicalToPhySecMap = (CPU_INT08U *)Mem_SegAlloc("FS - NAND log to phy sec map",
                                                                   p_seg,
                                                                   sizeof(CPU_INT08U) * ub_sec_map_size,
                                                                   p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return;
      }

      //                                                           Init sec mapping tbl.
      Mem_Set(&p_tbl_entry->LogicalToPhySecMap[0], 0x00u, sizeof(CPU_INT08U) * ub_sec_map_size);
    } else {
      p_tbl_entry->LogicalToPhySecMap = DEF_NULL;
    }

    //                                                             Alloc meta cache.
#if  (FS_NAND_CFG_UB_META_CACHE_EN == DEF_ENABLED)
    p_tbl_entry->MetaCachePtr = (CPU_INT08U *)Mem_SegAlloc("FS - NAND meta cache",
                                                           p_seg,
                                                           sizeof(CPU_INT08U) * ub_meta_cache_size,
                                                           p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
#endif
  }

  //                                                               ------------------ INIT DEV DATA -------------------
  FS_NAND_FTL_InitDevData(p_nand_ftl);
}

/****************************************************************************************************//**
 *                                           FS_NAND_FTL_InitDevData()
 *
 * @brief    Initialize NAND device data.
 *
 * @param    p_nand_ftl  Pointer to NAND FTL.
 *
 * @note     (1) All blocks located in the available block table at mount time are considered as
 *               potential stale metadata blocks (see also FS_NAND_MetaBlkParse()) to avoid metadata
 *               block ID overflow issues.
 *******************************************************************************************************/
static void FS_NAND_FTL_InitDevData(FS_NAND_FTL *p_nand_ftl)
{
  CPU_INT08U *p_buf;
  FS_NAND_UB_QTY tbl_ix;
  FS_NAND_UB_EXTRA_DATA *p_tbl_entry;
  CPU_INT16U meta_invalid_map_size;
  CPU_INT16U blk_sec_bitmap_size;
  CPU_INT16U ub_sec_map_size;
  CPU_INT16U ub_meta_cache_size;

  //                                                               ---------------- INIT NAND DEV DATA ----------------
  p_nand_ftl->AvailBlkTblInvalidated = DEF_NO;
  p_nand_ftl->SUB_Cnt = 0u;
  p_nand_ftl->ActivityCtr = 0u;

  //                                                               ------------------- INIT L2P TBL -------------------
  Mem_Set((void *)p_nand_ftl->LogicalToPhyBlkMap, 0xff, sizeof(FS_NAND_BLK_QTY) * p_nand_ftl->LogicalDataBlkCnt);

  //                                                               ---------------- INIT AVAIL BLK TBL ----------------
  p_buf = &p_nand_ftl->MetaCache[p_nand_ftl->MetaOffsetAvailBlkTbl];
  Mem_Set(&p_buf[0], 0xFFu, p_nand_ftl->MetaOffsetBadBlkTbl - 0u);

  //                                                               ----------------- INIT BAD BLK TBL -----------------
  p_buf = &p_nand_ftl->MetaCache[p_nand_ftl->MetaOffsetBadBlkTbl];
  Mem_Set(&p_buf[0], 0xFFu, p_nand_ftl->MetaOffsetDirtyBitmap - p_nand_ftl->MetaOffsetBadBlkTbl);

  //                                                               ---------------- INIT DIRTY BITMAP -----------------
  p_buf = &p_nand_ftl->MetaCache[p_nand_ftl->MetaOffsetDirtyBitmap];
  Mem_Set(&p_buf[0], 0x00u, p_nand_ftl->MetaOffsetUB_Tbl - p_nand_ftl->MetaOffsetDirtyBitmap);

#if (FS_NAND_CFG_DIRTY_MAP_CACHE_EN == DEF_ENABLED)
  p_buf = &p_nand_ftl->DirtyBitmapCache[0u];
  Mem_Set(&p_buf[0], 0x00u, p_nand_ftl->MetaOffsetUB_Tbl - p_nand_ftl->MetaOffsetDirtyBitmap);
#endif

  //                                                               ------------------- INIT UB TBL --------------------
  blk_sec_bitmap_size = p_nand_ftl->NbrSecPerBlk / 8u;
  blk_sec_bitmap_size += (p_nand_ftl->NbrSecPerBlk % 8u) == 0u ? 0u : 1u;
  p_buf = &p_nand_ftl->MetaCache[p_nand_ftl->MetaOffsetUB_Tbl];
  for (tbl_ix = 0u; tbl_ix < p_nand_ftl->UB_CntMax; tbl_ix++) {
    Mem_Set(&p_buf[tbl_ix * (blk_sec_bitmap_size + sizeof(FS_NAND_BLK_QTY))],
            0xFFu,
            sizeof(FS_NAND_BLK_QTY));
    Mem_Set(&p_buf[tbl_ix * (blk_sec_bitmap_size + sizeof(FS_NAND_BLK_QTY)) + sizeof(FS_NAND_BLK_QTY)],
            0x00u,
            blk_sec_bitmap_size);
  }

  //                                                               -------------- INIT META INVALID MAP ---------------
  meta_invalid_map_size = p_nand_ftl->MetaSecCnt / DEF_OCTET_NBR_BITS;
  meta_invalid_map_size += (p_nand_ftl->MetaSecCnt % DEF_OCTET_NBR_BITS) == 0u ? 0u : 1u;

  Mem_Clr((void *)p_nand_ftl->MetaBlkInvalidSecMap, meta_invalid_map_size);

  //                                                               -------------- INIT UB EXTRA DATA TBL --------------

  ub_sec_map_size = p_nand_ftl->NbrSecPerBlk * p_nand_ftl->UB_SecMapNbrBits;
  ub_sec_map_size *= p_nand_ftl->RUB_MaxAssoc;
  ub_sec_map_size = FS_UTIL_BIT_NBR_TO_OCTET_NBR(ub_sec_map_size);

  ub_meta_cache_size = p_nand_ftl->NbrSecPerBlkLog2 + p_nand_ftl->RUB_MaxAssocLog2;
  ub_meta_cache_size *= p_nand_ftl->NbrSecPerBlk;
  ub_meta_cache_size = FS_UTIL_BIT_NBR_TO_OCTET_NBR(ub_meta_cache_size);

  //                                                               For each entry...
  for (tbl_ix = 0u; tbl_ix < p_nand_ftl->UB_CntMax; tbl_ix++) {
    p_tbl_entry = &p_nand_ftl->UB_ExtraDataTbl[tbl_ix];

    p_tbl_entry->AssocLvl = 0u;
    p_tbl_entry->NextSecIx = 0u;
    p_tbl_entry->ActivityCtr = 0u;

    //                                                             Init associated blk tbl.
    Mem_Set(&p_tbl_entry->AssocLogicalBlksTbl[0], 0xFFu, sizeof(FS_NAND_BLK_QTY) * p_nand_ftl->RUB_MaxAssoc);

    if (ub_sec_map_size != 0u) {                                // If size if non-null ...
                                                                // Init sec mapping tbl.
      Mem_Set(&p_tbl_entry->LogicalToPhySecMap[0], 0x00u, sizeof(CPU_INT08U) * ub_sec_map_size);
    } else {
      p_tbl_entry->LogicalToPhySecMap = DEF_NULL;
    }
  }

  //                                                               See Note #1.
  Mem_Set(p_nand_ftl->AvailBlkMetaMap, 0xFF, FS_UTIL_BIT_NBR_TO_OCTET_NBR(p_nand_ftl->AvailBlkTblEntryCntMax));
}

/****************************************************************************************************//**
 *                                           FS_NAND_FTL_Alloc()
 *
 * @brief    Allocate resources needed by a NAND instance.
 *
 * @param    p_nand      Pointer to a NAND instance.
 *
 * @param    p_nand_cfg  Pointer to a NAND FTL configuration structure.
 *
 * @param    p_err       Error pointer.
 *
 * @return   Handle to a NAND FTL instance.
 *******************************************************************************************************/
static FS_NAND_FTL *FS_NAND_FTL_Alloc(FS_NAND               *p_nand,
                                      const FS_NAND_FTL_CFG *p_nand_cfg,
                                      RTOS_ERR              *p_err)
{
  FS_NAND_FTL *p_nand_ftl;
  FS_NAND_PART_PARAM *p_part_info;
  FS_NAND_CTRLR *p_ctrlr;
  CPU_SIZE_T align_req;
  FS_LB_SIZE min_sec_size;
  CPU_INT08U nbr_sec_per_pg;
  FS_NAND_OOS_INFO OOS_info;
  FS_NAND_PG_SIZE oos_size_free;
  FS_NAND_PG_SIZE needed_oos_size;
  FS_NAND_BLK_QTY last_blk_ix;
  CPU_INT16U used_mark_size_bits;

  //                                                               -------------- NAND DRV STRUCT ALLOC ---------------
  p_nand_ftl = (FS_NAND_FTL *)Mem_SegAlloc("FS - NAND FTL",
                                           FSStorage_InitCfg.MemSegPtr,
                                           sizeof(FS_NAND_FTL),
                                           p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }
  //                                                               --------------------- CLR DATA ---------------------

#if (FS_STORAGE_CFG_CTR_STAT_EN == DEF_ENABLED)                 // Clr stat ctrs.
  p_nand_ftl->Ctrs.StatMetaSecCommitCtr = 0u;
  p_nand_ftl->Ctrs.StatSUB_MergeCtr = 0u;
  p_nand_ftl->Ctrs.StatRUB_MergeCtr = 0u;
  p_nand_ftl->Ctrs.StatRUB_PartialMergeCtr = 0u;

  p_nand_ftl->Ctrs.StatBlkRefreshCtr = 0u;
#endif

#if (FS_STORAGE_CFG_CTR_ERR_EN == DEF_ENABLED)                  // Clr err ctrs.
  p_nand_ftl->Ctrs.ErrRefreshDataLoss = 0u;
#endif

  //                                                               Retrieve part data.
  p_ctrlr = p_nand->CtrlrPtr;
  p_part_info = p_ctrlr->CtrlrApiPtr->PartInfoGet(p_ctrlr);
  if (p_part_info == DEF_NULL) {
    LOG_ERR(("Error retrieving nand part data."));
    RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
    return (DEF_NULL);
  }

  if (p_nand_cfg == DEF_NULL) {
    p_nand_cfg = &FS_NAND_CfgDflt;
  }

  if (p_nand_cfg->BlkCnt == FS_NAND_CFG_BLK_CNT_AUTO) {         // Validate blk cnt.
    LOG_VRB(("Using default blk cnt (all blocks): ", (u)p_part_info->BlkCnt - p_nand_cfg->BlkIxFirst, "."));
    p_nand_ftl->BlkCnt = p_part_info->BlkCnt - p_nand_cfg->BlkIxFirst;
  } else {
    p_nand_ftl->BlkCnt = p_nand_cfg->BlkCnt;
  }

  //                                                               Validate UB cnt.
  RTOS_ASSERT_DBG_ERR_SET(p_nand_cfg->UB_CntMax < p_nand_ftl->BlkCnt,
                          *p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);

  p_nand_ftl->UB_CntMax = p_nand_cfg->UB_CntMax;
  p_nand_ftl->RUB_MaxAssoc = p_nand_cfg->RUB_MaxAssoc;

  //                                                               Validate avail blk tbl nbr of entries.
  RTOS_ASSERT_DBG_ERR_SET(p_nand_cfg->AvailBlkTblEntryCntMax >= FS_NAND_CFG_RSVD_AVAIL_BLK_CNT + 1u,
                          *p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);

  p_nand_ftl->AvailBlkTblEntryCntMax = p_nand_cfg->AvailBlkTblEntryCntMax;

  RTOS_ASSERT_DBG_ERR_SET(p_nand_cfg->AvailBlkTblEntryCntMax <= p_nand_ftl->BlkCnt,
                          *p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);

  //                                                               Validate index of first blk to use.
  RTOS_ASSERT_DBG_ERR_SET(p_nand_cfg->BlkIxFirst < p_nand_ftl->BlkCnt,
                          *p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);

  switch (p_nand_cfg->SecSize) {                                // Validate sec size.
    case  512u:
    case 1024u:
    case 2048u:
    case 4096u:
      p_nand_ftl->SecSize = p_nand_cfg->SecSize;
      break;

    case FS_NAND_CFG_SEC_SIZE_AUTO:
      p_nand_ftl->SecSize = p_part_info->PgSize;
      break;

    default:
      LOG_ERR(("Invalid NAND sec size: ", (u)p_nand_cfg->SecSize, "."));
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);
  }

  //                                                               ---------- VALIDATE CFG AGAINST PART DATA ----------
  //                                                               Validate sector size.
  min_sec_size = p_part_info->PgSize / p_part_info->NbrPgmPerPg;
  RTOS_ASSERT_DBG((p_nand_ftl->SecSize >= min_sec_size)
                  && (p_nand_ftl->SecSize <= p_part_info->PgSize),
                  RTOS_ERR_INVALID_CFG, DEF_NULL);

  //                                                               Validate blk cnt.
  p_nand_ftl->BlkIxFirst = p_nand_cfg->BlkIxFirst;
  last_blk_ix = p_nand_ftl->BlkIxFirst + p_nand_ftl->BlkCnt - 1u;
  RTOS_ASSERT_DBG(last_blk_ix < p_part_info->BlkCnt, RTOS_ERR_INVALID_CFG, DEF_NULL);

  //                                                               Validate UB cnt.
  RTOS_ASSERT_DBG(p_nand_ftl->UB_CntMax < p_nand_ftl->BlkCnt, RTOS_ERR_INVALID_CFG, DEF_NULL);

  //                                                               ------------ VALIDATE AGAINST HDR SIZE -------------
  RTOS_ASSERT_DBG(FS_NAND_HDR_TOTAL_SIZE <= p_nand_ftl->SecSize, RTOS_ERR_INVALID_CFG, DEF_NULL);

  //                                                               ------------------- SETUP CTRLR --------------------
  align_req = p_nand->Media.PmItemPtr->MediaApiPtr->AlignReqGet(&p_nand->Media, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  OOS_info = p_ctrlr->CtrlrApiPtr->Setup(p_ctrlr,
                                         FSStorage_InitCfg.MemSegPtr,
                                         align_req,
                                         p_nand_ftl->SecSize,
                                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  oos_size_free = OOS_info.Size;
  p_nand_ftl->OOS_BufPtr = OOS_info.BufPtr;

  //                                                               ----------------- CALC FTL PARAMS ------------------
  //                                                               Determine nbr of sec per blk.
  nbr_sec_per_pg = p_part_info->PgSize / p_nand_ftl->SecSize;
  p_nand_ftl->NbrSecPerBlk = p_part_info->PgPerBlk * nbr_sec_per_pg;
  p_nand_ftl->NbrSecPerBlkLog2 = FSUtil_Log2(p_nand_ftl->NbrSecPerBlk);

  p_nand_ftl->UB_SecMapNbrBits = p_nand_ftl->NbrSecPerBlkLog2 - FSUtil_Log2(FS_NAND_CFG_UB_TBL_SUBSET_SIZE);

  p_nand_ftl->RUB_MaxAssocLog2 = FSUtil_Log2(p_nand_ftl->RUB_MaxAssoc);

  used_mark_size_bits = p_part_info->ECC_NbrCorrBits * 2u;
  p_nand_ftl->UsedMarkSize = FS_UTIL_BIT_NBR_TO_OCTET_NBR(used_mark_size_bits);

  RTOS_ASSERT_DBG_ERR_SET(p_nand_ftl->UsedMarkSize <= 4u, *p_err,
                          RTOS_ERR_INVALID_CFG, DEF_NULL);

  //                                                               Calc limit on nbr of SUBs.
  p_nand_ftl->SUB_CntMax = p_nand_ftl->UB_CntMax * FS_NAND_CFG_MAX_SUB_PCT / 100;
  if (p_nand_ftl->SUB_CntMax == 0u) {
    p_nand_ftl->SUB_CntMax = 1u;
  }

  //                                                               ------------------- CALC FTL TH --------------------
  //                                                               See FS_NAND_SecWrInUB() note #2.
  //                                                               See FS_NAND_UB_Alloc() note #2.
  p_nand_ftl->ThSecWrCnt_MergeRUBStartSUB = p_nand_ftl->NbrSecPerBlk * FS_NAND_TH_PCT_MERGE_RUB_START_SUB / 100u + 1u;
  p_nand_ftl->ThSecRemCnt_ConvertSUBToRUB = p_nand_ftl->NbrSecPerBlk * FS_NAND_TH_PCT_CONVERT_SUB_TO_RUB  / 100u + 1u;
  p_nand_ftl->ThSecGapCnt_PadSUB = p_nand_ftl->NbrSecPerBlk * FS_NAND_TH_PCT_PAD_SUB             / 100u + 1u;
  p_nand_ftl->ThSecRemCnt_MergeSUB = p_nand_ftl->NbrSecPerBlk * FS_NAND_TH_PCT_MERGE_SUB           / 100u + 1u;

  //                                                               --------------- DETERMINE OOS USAGE ----------------
  needed_oos_size = FS_NAND_OOS_PARTIAL_SIZE_REQD + p_nand_ftl->UsedMarkSize;
  RTOS_ASSERT_DBG(needed_oos_size <= oos_size_free, RTOS_ERR_INVALID_CFG, DEF_NULL);

  //                                                               ------------------ CALC NAND INFO ------------------
  FS_NAND_FTL_CalcDevInfo(p_nand, p_nand_ftl, p_err);           // Calc dev info.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  p_nand_ftl->LogicalDataBlkCnt = p_nand_ftl->SecCnt / p_nand_ftl->NbrSecPerBlk;

  //                                                               Alloc dev data.
  FS_NAND_FTL_AllocDevData(p_nand, p_nand_ftl, FSStorage_InitCfg.MemSegPtr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  //                                                               --------------- CHECK METADATA SIZE ----------------
  RTOS_ASSERT_DBG(p_nand_ftl->MetaSecCnt <= p_nand_ftl->NbrSecPerBlk,
                  RTOS_ERR_INVALID_CFG, DEF_NULL)

  p_nand_ftl->IsMounted = DEF_NO;

  LOG_DBG(("NAND FLASH FOUND: Sec Size   : ", (u)p_nand_ftl->SecSize, " bytes"));
  LOG_DBG(("                  Size       : ", (u)p_nand_ftl->SecCnt, " sectors"));
  LOG_DBG(("                  Update blks: ", (u)p_nand_ftl->UB_CntMax));

  return (p_nand_ftl);
}

/****************************************************************************************************//**
 *                                               FS_NAND_FTL_Get()
 *
 * @brief    Allocate resources needed by a NAND instance.
 *
 * @param    p_media     Pointer to media.
 *
 * @return   Handle to a NAND FTL instance.
 *******************************************************************************************************/
static FS_NAND_FTL *FS_NAND_FTL_Get(FS_MEDIA *p_media)
{
  FS_NAND_FTL *p_nand_ftl;
  FS_NAND_FTL *p_cur_nand_ftl;

  p_nand_ftl = DEF_NULL;
  SLIST_FOR_EACH_ENTRY(FS_NAND_FTL_ListHeadPtr, p_cur_nand_ftl, FS_NAND_FTL, ListMember) {
    if (p_cur_nand_ftl->BlkDev.MediaPtr == p_media) {
      p_nand_ftl = p_cur_nand_ftl;
      break;
    }
  }

  if (p_nand_ftl != DEF_NULL) {
    SList_Rem(&FS_NAND_FTL_ListHeadPtr, &p_nand_ftl->ListMember);
  }

  return (p_nand_ftl);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_FS_STORAGE_NAND_AVAIL
