/***************************************************************************//**
 * @file
 * @brief File System - Core Partition Operations
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
#include  <fs_core_cfg.h>
#include  <fs/source/core/fs_core_partition_priv.h>
#include  <fs/source/core/fs_core_priv.h>
#include  <fs/source/core/fs_core_vol_priv.h>
#include  <fs/source/storage/fs_blk_dev_priv.h>
#include  <fs/source/core/fs_core_cache_priv.h>
#include  <fs/source/shared/fs_utils_priv.h>
#include  <fs/source/core/fs_core_op_priv.h>
#include  <fs/source/shared/cleanup/cleanup_mgmt_priv.h>

//                                                                 ----------------------- EXT ------------------------
#include  <common/include/lib_mem.h>
#include  <common/include/rtos_err.h>
#include  <cpu/include/cpu.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH       (FS, CORE, PARTITION)
#define  RTOS_MODULE_CUR    RTOS_CFG_MODULE_FS

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef struct fs_partition {
  FS_LB_NBR        LastMbrPos;
  FS_LB_NBR        CurBootRecordSec;
  FS_PARTITION_NBR CurBootRecordPos;
  FS_LB_NBR        StartSec;
  FS_LB_QTY        SecCnt;
  FS_LB_NBR        PrimExtPartitionStart;
  FS_LB_QTY        PrimExtPartitionSize;
} FS_PARTITION;

typedef enum fs_partition_rd_status {
  FS_PARTITION_RD_STATUS_NONE,
  FS_PARTITION_RD_STATUS_VALID,
  FS_PARTITION_RD_STATUS_EOT,
  FS_PARTITION_RD_STATUS_INVALID
} FS_PARTITION_RD_STATUS;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (FS_CORE_CFG_PARTITION_EN == DEF_ENABLED)
static const FS_PARTITION FSPartition_Null = { .CurBootRecordSec = 0u,
                                               .CurBootRecordPos = (FS_PARTITION_NBR)-1,
                                               .PrimExtPartitionStart = 0u,
                                               .PrimExtPartitionSize = 0u,
                                               .SecCnt = 0u,
                                               .StartSec = 0u,
                                               .LastMbrPos = 0u };
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (FS_CORE_CFG_PARTITION_EN == DEF_ENABLED)

static CPU_BOOLEAN FSPartition_FindInternal(FS_BLK_DEV_HANDLE blk_dev_handle,
                                            FS_PARTITION      *p_partition,
                                            FS_PARTITION_NBR  partition_nbr,
                                            FS_PARTITION_INFO *p_partition_info,
                                            RTOS_ERR          *p_err);

static FS_PARTITION_RD_STATUS FSPartition_Rd(FS_BLK_DEV_HANDLE blk_dev_handle,
                                             FS_PARTITION      *p_partition,
                                             FS_PARTITION_INFO *p_partition_info,
                                             RTOS_ERR          *p_err);

static void FSPartition_RdEntry(FS_BLK_DEV_HANDLE blk_dev_handle,
                                FS_PARTITION_INFO *p_partition_info,
                                FS_LB_NBR         mbr_sec,
                                FS_PARTITION_NBR  mbr_pos,
                                RTOS_ERR          *p_err);

static FS_PARTITION_NBR FSPartition_CntGetInternal(FS_BLK_DEV_HANDLE blk_dev_handle,
                                                   RTOS_ERR          *p_err);

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FSPartition_InitInternal(FS_BLK_DEV_HANDLE blk_dev_handle,
                                     FS_LB_QTY         lb_cnt,
                                     RTOS_ERR          *p_err);

static void FSPartition_Wr(FS_BLK_DEV_HANDLE blk_dev_handle,
                           FS_PARTITION      *p_partition,
                           FS_PARTITION_INFO *p_partition_info,
                           RTOS_ERR          *p_err);

static void FSPartition_WrEntry(FS_BLK_DEV_HANDLE blk_dev_handle,
                                FS_PARTITION_INFO *p_partition_info,
                                FS_LB_NBR         mbr_lb,
                                FS_PARTITION_NBR  mbr_pos,
                                RTOS_ERR          *p_err);

static void FSPartition_CalcCHS(CPU_INT08U *p_buf,
                                FS_LB_NBR  lba);

#endif

#if LOG_DBG_IS_EN()
static const CPU_CHAR *FSPartition_GetTypeName(CPU_INT08U type);
#endif

#else
static CPU_BOOLEAN FSPartition_FindSimple(FS_BLK_DEV_HANDLE blk_dev_handle,
                                          FS_PARTITION_INFO *p_partition_entry,
                                          RTOS_ERR          *p_err);
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               FSPartition_Add()
 *
 * @brief    Add a partition to a device.
 *
 * @param    blk_dev_handle  Handle to a block device.
 *
 * @param    lb_cnt          Size (in logical blocks) of the partition to add.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_SIZE_INVALID
 *                               - RTOS_ERR_PARTITION_MAX_EXCEEDED
 *                               - RTOS_ERR_BLK_DEV_CLOSED
 *
 * @return   The index of the created partition.
 *
 * @note     (1) The first partition on the device has index 1.
 *
 * @note     (2) If there is no valid partition on the device, the function will automatically
 *               initialize a MBR (Master Boot Record) sector and create the first partition.
 *******************************************************************************************************/

#if (FS_CORE_CFG_PARTITION_EN == DEF_ENABLED)
#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
FS_PARTITION_NBR FSPartition_Add(FS_BLK_DEV_HANDLE blk_dev_handle,
                                 FS_LB_QTY         lb_cnt,
                                 RTOS_ERR          *p_err)
{
  CPU_INT08U             *p_buf;
  FS_CACHE_BLK_DEV_DATA  *p_cache_blk_dev_data;
  FS_PARTITION           partition;
  FS_PARTITION_INFO      partition_entry;
  FS_PARTITION_NBR       partition_nbr = 0u;
  FS_LB_SIZE             lb_size;
  FS_PARTITION_RD_STATUS rd_status;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NULL);
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    BREAK_ON_ERR(lb_size = FSBlkDev_LbSizeGet(blk_dev_handle, p_err));

    partition = FSPartition_Null;

    //                                                             ------------ ITERATE THROUGH PARTITIONS ------------
    BREAK_ON_ERR(rd_status = FSPartition_Rd(blk_dev_handle,     // Rd first partition.
                                            &partition,
                                            &partition_entry,
                                            p_err));

    partition_nbr = 1u;                                         // Partition nbr is 1-based.
    if (rd_status == FS_PARTITION_RD_STATUS_INVALID) {          // If no partition, init partition struct.
                                                                // WARNING: a MBR is created.
      BREAK_ON_ERR(FSPartition_InitInternal(blk_dev_handle,
                                            lb_cnt,
                                            p_err));
    } else {
      //                                                           While another partition exists ...
      BREAK_ON_ERR(while) (rd_status == FS_PARTITION_RD_STATUS_VALID) {
        //                                                         ... rd next partition.
        BREAK_ON_ERR(rd_status = FSPartition_Rd(blk_dev_handle,
                                                &partition,
                                                &partition_entry,
                                                p_err));

        ASSERT_BREAK(rd_status != FS_PARTITION_RD_STATUS_EOT, RTOS_ERR_PARTITION_MAX_EXCEEDED);

        partition_nbr++;
      }

      //                                                           ----------------- CREATE PARTITION -----------------
      partition_entry.StartSec = partition.StartSec + partition.SecCnt;
      partition_entry.SecCnt = lb_cnt;
      partition_entry.Type = 0u;

      //                                                           ------------------- FMT & WR MBR -------------------
      //                                                           Clear the Partition start sector to allow the format
      //                                                           to write the correct FAT info.
      p_cache_blk_dev_data = FSCache_BlkDevDataGet(blk_dev_handle);
      ASSERT_BREAK_LOG((p_cache_blk_dev_data != DEF_NULL), RTOS_ERR_NULL_PTR,
                       ("Blk dev must be assigned to a cache instance"));

      BREAK_ON_ERR(FS_CACHE_BLK_WR)(p_cache_blk_dev_data,
                                    partition_entry.StartSec,
                                    FS_LB_TYPE_MBR,
                                    JobSched_VoidJobHandle,
                                    &p_buf,
                                    DEF_NULL,
                                    p_err) {
        Mem_Clr((void *)p_buf, lb_size);
      }

      BREAK_ON_ERR(FSPartition_Wr(blk_dev_handle,
                                  &partition,
                                  &partition_entry,
                                  p_err));
    }
  } WITH_SCOPE_END

  return (partition_nbr);
}
#endif
#endif

/****************************************************************************************************//**
 *                                           FSPartition_Init()
 *
 * @brief    Initialize the partition structure on a block device (see Note #1).
 *
 * @param    blk_dev_handle  Handle to a block device.
 *
 * @param    lb_cnt          Size (in logical blocks) of first partition. 0 if the partition will
 *                           occupy the entire device (see Note #2).
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_SIZE_INVALID
 *                               - RTOS_ERR_VOL_OPENED
 *                               - RTOS_ERR_BLK_DEV_CLOSED
 *                               - RTOS_ERR_BLK_DEV_CORRUPTED
 *                               - RTOS_ERR_IO
 *
 * @note     (1) This function creates the first partition with index 1. Any subsequent partition is
 *               created using the function FSPartition_Add().
 *
 * @note     (2) If 0 is passed as the number of logical blocks composing the first partition, no MBR
 *               (Master Boot Record) sector is created. If non-zero is passed, a MBR sector is
 *               created allowing the creation of four partitions.
 *
 * @note     (3) Function returns an error if a volume is open on the device.  All volumes (& files)
 *               MUST be closed prior to initializing the partition structure, since it will
 *               obliterate any existing file system.
 *******************************************************************************************************/

#if (FS_CORE_CFG_PARTITION_EN == DEF_ENABLED)
#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FSPartition_Init(FS_BLK_DEV_HANDLE blk_dev_handle,
                      FS_LB_QTY         lb_cnt,
                      RTOS_ERR          *p_err)
{
  FS_VOL    *p_cur_vol;
  FS_LB_QTY dev_lb_cnt;
  FS_LB_QTY lb_cnt_effective;

  WITH_SCOPE_BEGIN(p_err) {
    //                                                             --------------------- ARG CHK ----------------------
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    BREAK_ON_ERR(dev_lb_cnt = FSBlkDev_LbCntGet(blk_dev_handle, p_err));

    FS_OP_LOCK_WITH(&FSCore_Data.OpLock, FS_GLOBAL_OP_OPEN_VOL_LIST_RD) {
      BREAK_ON_ERR(SLIST_FOR_EACH_ENTRY) (FSVol_OpenListHeadPtr, p_cur_vol, FS_VOL, OpenListMember) {
        if (FS_BLK_DEV_HANDLE_ARE_EQUAL(p_cur_vol->BlkDevHandle, blk_dev_handle)) {
          BREAK_ERR_SET(RTOS_ERR_VOL_OPENED);
        }
      }
    }

    ASSERT_BREAK(lb_cnt <= dev_lb_cnt, RTOS_ERR_SIZE_INVALID);

    lb_cnt_effective = (lb_cnt > 0u) ? lb_cnt : dev_lb_cnt;

    BREAK_ON_ERR(FSPartition_InitInternal(blk_dev_handle,
                                          lb_cnt_effective,
                                          p_err));
  } WITH_SCOPE_END
}
#endif
#endif

/****************************************************************************************************//**
 *                                           FSPartition_CntGet()
 *
 * @brief    Get number of partitions on a block device.
 *
 * @param    blk_dev_handle  Handle to a block device.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_BLK_DEV_CLOSED
 *                               - RTOS_ERR_BLK_DEV_CORRUPTED
 *                               - RTOS_ERR_IO
 *
 * @return   Number of partitions, if NO errors.
 *           0,                    otherwise.
 *
 * @note     (1) If no Master Boot Record is present on the media's sector #0, a valid formatted
 *               partition can still be present. In that case, FSPartition_CntGet() will return 1
 *               partition as the number of partitions since no partition table exists to describe
 *               other partitions.
 *******************************************************************************************************/

#if (FS_CORE_CFG_PARTITION_EN == DEF_ENABLED)
FS_PARTITION_NBR FSPartition_CntGet(FS_BLK_DEV_HANDLE blk_dev_handle,
                                    RTOS_ERR          *p_err)
{
  FS_PARTITION_NBR partition_cnt;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NULL);
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    BREAK_ON_ERR(partition_cnt = FSPartition_CntGetInternal(blk_dev_handle, p_err));
  } WITH_SCOPE_END

  return (partition_cnt);
}
#endif

/****************************************************************************************************//**
 *                                           FSPartition_Query()
 *
 * @brief    Query partition information.
 *
 * @param    blk_dev_handle      Handle to a block device.
 *
 * @param    partition_nbr       Number of the partition to query (first partition is number 1).
 *
 * @param    p_partition_info    Pointer to variable that will receive the partition information.
 *
 * @param    p_err               Pointer to variable that will receive the return error code(s) from this
 *                               function:
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_NOT_FOUND
 *                                   - RTOS_ERR_BLK_DEV_CLOSED
 *                                   - RTOS_ERR_BLK_DEV_CORRUPTED
 *                                   - RTOS_ERR_IO
 *******************************************************************************************************/
void FSPartition_Query(FS_BLK_DEV_HANDLE blk_dev_handle,
                       FS_PARTITION_NBR  partition_nbr,
                       FS_PARTITION_INFO *p_partition_info,
                       RTOS_ERR          *p_err)
{
  CPU_BOOLEAN partition_found;

  WITH_SCOPE_BEGIN(p_err) {
    //                                                             --------------------- ARG CHK ----------------------
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    RTOS_ASSERT_DBG_ERR_SET(p_partition_info != DEF_NULL, *p_err, RTOS_ERR_NULL_PTR,; );
    RTOS_ASSERT_DBG_ERR_SET(partition_nbr > 0u, *p_err, RTOS_ERR_INVALID_ARG,; );

    BREAK_ON_ERR(partition_found = FSPartition_Find(blk_dev_handle,
                                                    partition_nbr,
                                                    p_partition_info,
                                                    p_err));

    ASSERT_BREAK(partition_found, RTOS_ERR_NOT_FOUND);
  } WITH_SCOPE_END
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           FSPartition_Find()
 *
 * @brief    Find a partition.
 *
 * @param    blk_dev_handle      Handle to a block device.
 *
 * @param    partition_nbr       Number of the partition to be found.
 *
 * @param    p_partition_info    Pointer to structure that will receive the partition information.
 *
 * @param    p_err               Error pointer.
 *
 * @return   DEF_YES, if the partition is found.
 *           DEF_NO, otherwise.
 *******************************************************************************************************/
CPU_BOOLEAN FSPartition_Find(FS_BLK_DEV_HANDLE blk_dev_handle,
                             FS_PARTITION_NBR  partition_nbr,
                             FS_PARTITION_INFO *p_partition_info,
                             RTOS_ERR          *p_err)
{
  CPU_BOOLEAN partition_found;

#if (FS_CORE_CFG_PARTITION_EN == DEF_ENABLED)
  {
    FS_PARTITION partition = FSPartition_Null;
    partition_found = FSPartition_FindInternal(blk_dev_handle,
                                               &partition,
                                               partition_nbr,
                                               p_partition_info,
                                               p_err);
  }
#else
  RTOS_ASSERT_DBG(partition_nbr == 1u, RTOS_ERR_INVALID_ARG, DEF_NO);
  partition_found = FSPartition_FindSimple(blk_dev_handle,
                                           p_partition_info,
                                           p_err);
#endif

  return (partition_found);
}

/****************************************************************************************************//**
 *                                           FSPartition_Update()
 *
 * @brief    Update partition on a device.
 *
 * @param    blk_dev_handle  Handle to a block device.
 *
 * @param    partition_nbr   Index of the partition to update.
 *
 * @param    partition_type  New type of the partition (specific to file system driver).
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/

#if (FS_CORE_CFG_PARTITION_EN == DEF_ENABLED)
#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FSPartition_Update(FS_BLK_DEV_HANDLE blk_dev_handle,
                        FS_PARTITION_NBR  partition_nbr,
                        CPU_INT08U        partition_type,
                        RTOS_ERR          *p_err)
{
  FS_PARTITION      partition;
  FS_PARTITION_INFO partition_entry;
  CPU_BOOLEAN       partition_found;

  WITH_SCOPE_BEGIN(p_err) {
    partition = FSPartition_Null;
    BREAK_ON_ERR(partition_found = FSPartition_FindInternal(blk_dev_handle,
                                                            &partition,
                                                            partition_nbr,
                                                            &partition_entry,
                                                            p_err));
    ASSERT_BREAK(partition_found, RTOS_ERR_NOT_FOUND);

    partition_entry.Type = partition_type;

    BREAK_ON_ERR(FSPartition_WrEntry(blk_dev_handle,            // Wr updated entry.
                                     &partition_entry,
                                     partition.CurBootRecordSec,
                                     partition.CurBootRecordPos,
                                     p_err));
  } WITH_SCOPE_END
}
#endif
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       FSPartition_GetNbrPartitions()
 *
 * @brief    Get the number of partitions on a device.
 *
 * @param    blk_dev_handle  Handle to a block device.
 *
 * @param    p_err           Error pointer.
 *
 * @return   Number of partitions on device, if NO errors.
 *           0,                              otherwise.
 *******************************************************************************************************/

#if (FS_CORE_CFG_PARTITION_EN == DEF_ENABLED)
static FS_PARTITION_NBR FSPartition_CntGetInternal(FS_BLK_DEV_HANDLE blk_dev_handle,
                                                   RTOS_ERR          *p_err)
{
  FS_PARTITION           partition;
  FS_PARTITION_INFO      partition_entry;
  FS_PARTITION_NBR       partition_cnt;
  FS_PARTITION_RD_STATUS rd_status;

  partition = FSPartition_Null;
  //                                                               ------------ ITERATE THROUGH PARTITIONS ------------
  rd_status = FSPartition_Rd(blk_dev_handle,                    // Rd first partition.
                             &partition,
                             &partition_entry,
                             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }
  if (rd_status == FS_PARTITION_RD_STATUS_INVALID) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_PARTITION_INVALID);
    return (0u);
  }

  partition_cnt = 1u;                                           // While another partition exists ...
  while ((rd_status != FS_PARTITION_RD_STATUS_INVALID)
         && (rd_status != FS_PARTITION_RD_STATUS_EOT)) {
    LOG_DBG(("Partition found: ", (u)partition_entry.StartSec, " + ", (u)partition_entry.SecCnt,
             "type = ", (s)FSPartition_GetTypeName(partition_entry.Type), "."));

    rd_status = FSPartition_Rd(blk_dev_handle,                  // ... rd next partition.
                               &partition,
                               &partition_entry,
                               p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (partition_cnt);
    }

    if ((rd_status != FS_PARTITION_RD_STATUS_INVALID)
        && (rd_status != FS_PARTITION_RD_STATUS_EOT)) {
      partition_cnt += 1u;
    }
  }

  return (partition_cnt);
}
#endif

/********************************************************************************************************
 *                                       FSPartition_FindSimple()
 *
 * Description : Find first partition on a device.
 *
 * Argument(s) : blk_dev_handle      Handle to a block device.
 *
 *               p_partition_info    Pointer to structure that will receive the partition information.
 *
 *               p_err               Error pointer.
 *
 * Note(s)     : none.
 *******************************************************************************************************/

#if (FS_CORE_CFG_PARTITION_EN == DEF_DISABLED)
static CPU_BOOLEAN FSPartition_FindSimple(FS_BLK_DEV_HANDLE blk_dev_handle,
                                          FS_PARTITION_INFO *p_partition_info,
                                          RTOS_ERR          *p_err)
{
  CPU_INT08U            *p_buf;
  FS_CACHE_BLK_DEV_DATA *p_cache_blk_dev_data;
  CPU_INT16U            sig_val;
  FS_LB_QTY             dev_lb_cnt;
  CPU_BOOLEAN           partition_found = DEF_NO;

  WITH_SCOPE_BEGIN(p_err) {
    BREAK_ON_ERR(dev_lb_cnt = FSBlkDev_LbCntGet(blk_dev_handle, p_err));
    BREAK_ON_ERR(p_cache_blk_dev_data = FSCache_BlkDevDataGet(blk_dev_handle));

    //                                                             --------------------- READ MBR ---------------------
    partition_found = DEF_NO;
    BREAK_ON_ERR(FS_CACHE_BLK_RD)(p_cache_blk_dev_data, 0u, FS_LB_TYPE_MBR, &p_buf, p_err) {
      sig_val = MEM_VAL_GET_INT16U_LITTLE(p_buf + 510u);        // Check sec sig.
      ASSERT_BREAK(sig_val == 0xAA55u, RTOS_ERR_PARTITION_INVALID);

      //                                                           ------------------ READ MBR ENTRY ------------------
      //                                                           Rd start sec.
      p_partition_info->StartSec = MEM_VAL_GET_INT32U_LITTLE(p_buf + FS_PARTITION_DOS_OFF_START_LBA_1      + (0u * FS_PARTITION_DOS_TBL_ENTRY_SIZE));
      //                                                           Rd partition size (see Note #2).
      p_partition_info->SecCnt = MEM_VAL_GET_INT32U_LITTLE(p_buf + FS_PARTITION_DOS_OFF_SIZE_1           + (0u * FS_PARTITION_DOS_TBL_ENTRY_SIZE));
      //                                                           Rd partition type.
      p_partition_info->Type = MEM_VAL_GET_INT08U_LITTLE(p_buf + FS_PARTITION_DOS_OFF_PARTITION_TYPE_1 + (0u * FS_PARTITION_DOS_TBL_ENTRY_SIZE));
    }

    if ((p_partition_info->StartSec + p_partition_info->SecCnt <= dev_lb_cnt)
        && (p_partition_info->SecCnt > 1u)) {
      partition_found = DEF_YES;
    }
  } WITH_SCOPE_END

  return (partition_found);
}
#endif

/****************************************************************************************************//**
 *                                       FSPartition_InitInternal()
 *
 * @brief    Initialize the partition structure on the device and creates the first partition.
 *
 * @param    blk_dev_handle  Handle to a block device.
 *
 * @param    lb_cnt          Size (in logical blocks) of the first partition.
 *
 * @param    p_err           Error pointer.
 *
 * @note     (1) The partition begins in the sector immediately after the MBR.
 *
 * @note     (3) The file system type is not set in the MBR record.  This can only be done once the
 *               file system type had been determined.
 *             - (a) #### Provide mechanism for setting partition type.
 *
 * @note     (4) The CHS addresses are set as a backup addressing system, in case the disk is used on
 *               a system unaware of LBA addressing.
 *             - (a) CHS addresses are always calculated assuming a geometry of 255 heads & 63 sectors
 *                     per track.
 *             - (b) LBA & CHS address are related by :
 *                   @verbatim
 *                       LBA = (((CYLINDER * heads_per_cylinder + HEAD) * sectors_per_track) + SECTOR - 1
 *
 *                   so
 *
 *                       SECTOR   =   (LBA + 1) % sectors_per_track
 *                       HEAD     =  ((LBA + 1 - SECTOR) / sectors_per_track) % heads_per_cylinder
 *                       CYLINDER = (((LBA + 1 - SECTOR) / sectors_per_track) - HEAD) / heads_per_cylinder
 *                   @endverbatim
 * @note     (5) The first sector of the new partition is cleared, so that any data from a previous
 *               file system will not be used to incorrectly initialize the volume.
 *
 * @note     (6) Avoid 'Excessive shift value' or 'Constant expression evaluates to 0' warning.
 *******************************************************************************************************/

#if (FS_CORE_CFG_PARTITION_EN == DEF_ENABLED)
#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FSPartition_InitInternal(FS_BLK_DEV_HANDLE blk_dev_handle,
                                     FS_LB_QTY         lb_cnt,
                                     RTOS_ERR          *p_err)
{
  CPU_INT08U            *p_buf;
  FS_CACHE_BLK_DEV_DATA *p_cache_blk_dev_data;
  CPU_INT32U            start_lba;
  FS_LB_SIZE            lb_size;

  WITH_SCOPE_BEGIN(p_err) {
    p_cache_blk_dev_data = FSCache_BlkDevDataGet(blk_dev_handle);
    ASSERT_BREAK_LOG((p_cache_blk_dev_data != DEF_NULL), RTOS_ERR_NULL_PTR,
                     ("Blk dev must be assigned to a cache instance"));

    BREAK_ON_ERR(lb_size = FSBlkDev_LbSizeGet(blk_dev_handle, p_err));

    //                                                             ------------------- FMT & WR MBR -------------------
    BREAK_ON_ERR(FS_CACHE_BLK_WR)(p_cache_blk_dev_data,
                                  1u,
                                  FS_LB_TYPE_MBR,
                                  JobSched_VoidJobHandle,
                                  &p_buf,
                                  DEF_NULL,
                                  p_err) {
      Mem_Clr((void *)p_buf, lb_size);
    }

    BREAK_ON_ERR(FS_CACHE_BLK_WR)(p_cache_blk_dev_data,
                                  0u,
                                  FS_LB_TYPE_MBR,
                                  JobSched_VoidJobHandle,
                                  &p_buf,
                                  DEF_NULL,
                                  p_err) {
      Mem_Clr((void *)p_buf, lb_size);

      MEM_VAL_SET_INT08U_LITTLE((void *)(p_buf + FS_PARTITION_DOS_OFF_BOOT_FLAG_1), FS_PARTITION_DOS_BOOT_FLAG);

      FSPartition_CalcCHS(p_buf + FS_PARTITION_DOS_OFF_START_CHS_ADDR_1, (FS_LB_NBR)1);

      MEM_VAL_SET_INT08U_LITTLE((void *)(p_buf + FS_PARTITION_DOS_OFF_PARTITION_TYPE_1), FS_PARTITION_TYPE_FAT32_LBA);

      FSPartition_CalcCHS(p_buf + FS_PARTITION_DOS_OFF_END_CHS_ADDR_1, lb_cnt - 1u);

      start_lba = 1u;                                           // See Note #6.
      MEM_VAL_SET_INT32U_LITTLE((void *)(p_buf + FS_PARTITION_DOS_OFF_START_LBA_1), start_lba);
      MEM_VAL_SET_INT32U_LITTLE((void *)(p_buf + FS_PARTITION_DOS_OFF_SIZE_1), lb_cnt - 1u);
      MEM_VAL_SET_INT16U_LITTLE((void *)(p_buf + FS_PARTITION_DOS_OFF_SIG), 0xAA55u);
    }
  } WITH_SCOPE_END
}
#endif
#endif

/****************************************************************************************************//**
 *                                       FSPartition_FindInternal()
 *
 * @brief    Find a partition.
 *
 * @param    blk_dev_handle      Handle to a block device.
 *
 * @param    p_partition         Pointer to a partition (caller is responsible for init).
 *
 * @param    partition_nbr       Number of the partition to find.
 *
 * @param    p_partition_info    Pointer to a structure that will receive partition information.
 *
 * @param    p_err               Error pointer.
 *
 * @return   DEF_YES, if the partition is found.
 *           DEF_NO, otherwise.
 *******************************************************************************************************/

#if (FS_CORE_CFG_PARTITION_EN == DEF_ENABLED)
static CPU_BOOLEAN FSPartition_FindInternal(FS_BLK_DEV_HANDLE blk_dev_handle,
                                            FS_PARTITION      *p_partition,
                                            FS_PARTITION_NBR  partition_nbr,
                                            FS_PARTITION_INFO *p_partition_info,
                                            RTOS_ERR          *p_err)
{
  FS_PARTITION_NBR       partition_cnt;
  FS_PARTITION_RD_STATUS rd_status;
  CPU_BOOLEAN            partition_found;

  WITH_SCOPE_BEGIN(p_err) {
    //                                                             ------------ ITERATE THROUGH PARTITIONS ------------
    partition_found = DEF_NO;
    partition_cnt = 0u;

    //                                                             *INDENT-OFF*
    BREAK_ON_ERR(do ) {
                                                                // Rd next partition entry.
      BREAK_ON_ERR(rd_status = FSPartition_Rd(blk_dev_handle,
                                              p_partition,
                                              p_partition_info,
                                              p_err));
      partition_cnt++;
    } while ((rd_status != FS_PARTITION_RD_STATUS_EOT)
            && (partition_cnt < partition_nbr));
    //                                                             *INDENT-ON*

    partition_found = ((partition_cnt == partition_nbr)
                       && (rd_status == FS_PARTITION_RD_STATUS_VALID));
  } WITH_SCOPE_END

  return (partition_found);
}
#endif

/****************************************************************************************************//**
 *                                           FSPartition_CalcCHS()
 *
 * @brief    Calculate CHS address.
 *
 * @param    p_buf   Pointer to buffer in which CHS address will be stored.
 *
 * @param    lba     LBA address.
 *******************************************************************************************************/

#if ((FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED) \
  && (FS_CORE_CFG_PARTITION_EN == DEF_ENABLED))
static void FSPartition_CalcCHS(CPU_INT08U *p_buf,
                                FS_LB_NBR  lba)
{
  CPU_INT16U cylinder;
  CPU_INT08U head;
  CPU_INT08U sector;

  sector = (CPU_INT08U)(  (lba + 1u)          % FS_PARTITION_DOS_CHS_SECTORS_PER_TRK);
  head = (CPU_INT08U)( ((lba + 1u - sector) / FS_PARTITION_DOS_CHS_SECTORS_PER_TRK)         % FS_PARTITION_DOS_CSH_HEADS_PER_CYLINDER);
  cylinder = (CPU_INT16U)((((lba + 1u - sector) / FS_PARTITION_DOS_CHS_SECTORS_PER_TRK) - head) / FS_PARTITION_DOS_CSH_HEADS_PER_CYLINDER);
  *p_buf = head;
  p_buf++;
  *p_buf = sector | (CPU_INT08U)((cylinder & 0x300u) >> 8);
  p_buf++;
  *p_buf = (CPU_INT08U)cylinder & DEF_INT_08_MASK;
}
#endif

/****************************************************************************************************//**
 *                                           FSPartition_GetTypeName()
 *
 * @brief    Get name of partition type.
 *
 * @param    type    Partition type.
 *
 * @return   Pointer to partition name string.
 *******************************************************************************************************/

#if (FS_CORE_CFG_PARTITION_EN == DEF_ENABLED)
#if LOG_DBG_IS_EN()
static const CPU_CHAR *FSPartition_GetTypeName(CPU_INT08U type)
{
  const CPU_CHAR *p_type_name;

  switch (type) {
    case FS_PARTITION_TYPE_FAT12_CHS:
    case FS_PARTITION_TYPE_HID_FAT12_CHS:
      p_type_name = FS_PARTITION_DOS_NAME_FAT12;
      break;

    case FS_PARTITION_TYPE_FAT16_16_32MB:
    case FS_PARTITION_TYPE_FAT16_CHS_32MB_2GB:
    case FS_PARTITION_TYPE_FAT16_LBA_32MB_2GB:
    case FS_PARTITION_TYPE_HID_FAT16_16_32MB_CHS:
    case FS_PARTITION_TYPE_HID_FAT16_CHS_32MB_2GB:
    case FS_PARTITION_TYPE_HID_FAT16_LBA_32MB_2GB:
      p_type_name = FS_PARTITION_DOS_NAME_FAT16;
      break;

    case FS_PARTITION_TYPE_FAT32_CHS:
    case FS_PARTITION_TYPE_FAT32_LBA:
    case FS_PARTITION_TYPE_HID_CHS_FAT32:
    case FS_PARTITION_TYPE_HID_LBA_FAT32:
      p_type_name = FS_PARTITION_DOS_NAME_FAT32;
      break;

    case FS_PARTITION_TYPE_CHS_MICROSOFT_EXT:
    case FS_PARTITION_TYPE_LBA_MICROSOFT_EXT:
    case FS_PARTITION_TYPE_LINUX_EXT:
      p_type_name = FS_PARTITION_DOS_NAME_EXT;
      break;

    default:
      p_type_name = FS_PARTITION_DOS_NAME_OTHER;
      break;
  }

  return (p_type_name);
}
#endif
#endif

/****************************************************************************************************//**
 *                                               FSPartition_Rd()
 *
 * @brief    Read a partition entry.
 *
 * @param    blk_dev_handle      Handle to a block device.
 *
 * @param    p_partition         Pointer to partition.
 *
 * @param    p_partition_info    Pointer to a structure that will receive the partition information.
 *
 * @param    p_err               Error pointer.
 *
 * @note     (1) The start sector of a partition entry may be relative to one of three device
 *               locations :
 *               - (a) The start sector for a primary partition or for the primary extended partition is
 *                       relative to the start of the device (sector 0).  The entries for this partition
 *                       are located in the "primary" MBR.
 *               - (b) The start sector for a secondary extended partition is relative to the start of
 *                       the primary extended partition.
 *               - (c) The start sector for a secondary file system partition is relative to the start
 *                       of the partition.
 *
 * @note     (2) The "primary" MBR may hold as many as 4 partition entries.
 *               "Secondary" MBRs may hold as many as 2 partition entries.  If two entries are
 *               present, the first will specify a file system & the second will point to the next
 *               extended partition.
 *
 * @note     (3) If "p_partition_info" is already valid, this part of the code should not make any
 *               change to the structure. If "p_partition_info" is not valid, the loop should test every
 *               file systems. If a valid file system boot sector is found, a valid "p_partition_info"
 *               is returned.
 *******************************************************************************************************/

#if (FS_CORE_CFG_PARTITION_EN == DEF_ENABLED)
static FS_PARTITION_RD_STATUS FSPartition_Rd(FS_BLK_DEV_HANDLE blk_dev_handle,
                                             FS_PARTITION      *p_partition,
                                             FS_PARTITION_INFO *p_partition_info,
                                             RTOS_ERR          *p_err)
{
  FS_LB_QTY   lb_cnt;
  CPU_BOOLEAN partition_found;
  CPU_BOOLEAN check;
  CPU_INT08U  type_idx;

  p_partition->CurBootRecordPos++;

  //                                                               --------------------- RD ENTRY ---------------------
  do {
    if (p_partition->CurBootRecordPos == 4u) {
      return (FS_PARTITION_RD_STATUS_EOT);
    }

    FSPartition_RdEntry(blk_dev_handle,
                        p_partition_info,
                        p_partition->CurBootRecordSec,
                        p_partition->CurBootRecordPos,
                        p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (FS_PARTITION_RD_STATUS_NONE);
    }

    //                                                             ---------------- EXTENDED PARTITION ----------------
    if ((p_partition_info->Type == FS_PARTITION_TYPE_CHS_MICROSOFT_EXT)
        || (p_partition_info->Type == FS_PARTITION_TYPE_LBA_MICROSOFT_EXT)
        || (p_partition_info->Type == FS_PARTITION_TYPE_LINUX_EXT)) {
      //                                                           Update partition struct with info from valid...
      //                                                           ...partition entry.
      if (p_partition->CurBootRecordSec == 0u) {                // If browsing Master Boot Record.
        p_partition->LastMbrPos = p_partition->CurBootRecordPos;
        p_partition->CurBootRecordSec = p_partition_info->StartSec;
        p_partition->CurBootRecordPos = 0u;
        p_partition->PrimExtPartitionStart = p_partition_info->StartSec;
        p_partition->PrimExtPartitionSize = p_partition_info->SecCnt;
      } else {                                                  // If browsing Extended Boot Record.
        p_partition->CurBootRecordSec = p_partition->PrimExtPartitionStart + p_partition_info->StartSec;
        p_partition->CurBootRecordPos = 0u;
      }

      partition_found = DEF_NO;

      //                                                           ----------- PRIMARY OR LOGICAL PARTITION -----------
    } else {
      type_idx = 0u;
      //                                                           See Note #3.
      while (FSSys_API_Tbl[type_idx] != DEF_NULL) {             // Iterate through all the table.
                                                                // Check boot sector for a partition.
        check = FSSys_API_Tbl[type_idx]->BootSecChk(blk_dev_handle,
                                                    p_partition_info,
                                                    p_partition->CurBootRecordSec,
                                                    p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          return (FS_PARTITION_RD_STATUS_NONE);
        }

        if (check == DEF_TRUE) {
          break;                                                // A valid boot sector is found.
        }

        type_idx++;
      }
      //                                                           Verify validity of partition entry.
      if (p_partition->CurBootRecordSec == 0u) {                // If browsing Master Boot Record.
        lb_cnt = FSBlkDev_LbCntGet(blk_dev_handle, p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          return (FS_PARTITION_RD_STATUS_NONE);
        }

        if ((p_partition_info->StartSec < p_partition->StartSec + p_partition->SecCnt)
            || (p_partition_info->StartSec + p_partition_info->SecCnt > lb_cnt)
            || (p_partition_info->SecCnt == 0u)) {
          return (FS_PARTITION_RD_STATUS_INVALID);
        }

        partition_found = DEF_YES;
      } else {                                                  // If browsing Extended Boot Record.
        if ((p_partition_info->StartSec <= p_partition->StartSec + p_partition->SecCnt)
            || (p_partition_info->StartSec + p_partition_info->SecCnt - 1u > p_partition->PrimExtPartitionStart + p_partition->PrimExtPartitionSize - 1)
            || (p_partition_info->SecCnt <= 1u)
            || (p_partition_info->StartSec <= 1u)) {
          p_partition->CurBootRecordSec = 0u;
          p_partition->CurBootRecordPos = p_partition->LastMbrPos + 1u;
          p_partition->StartSec = p_partition->PrimExtPartitionStart;
          p_partition->SecCnt = p_partition->PrimExtPartitionSize;
          partition_found = DEF_NO;
        } else {
          partition_found = DEF_YES;
        }
      }
    }
  } while (!partition_found);

  p_partition->SecCnt = p_partition_info->SecCnt;
  p_partition->StartSec = p_partition_info->StartSec;

  return (FS_PARTITION_RD_STATUS_VALID);
}
#endif

/****************************************************************************************************//**
 *                                           FSPartition_RdEntry()
 *
 * @brief    Read a partition entry in the partition table at specified position (see Note #1).
 *
 * @param    blk_dev_handle      Handle to a block device.
 *
 * @param    p_partition_info    Pointer to a structure that will receive the partition information.
 *
 * @param    mbr_sec             MBR sector.
 *
 * @param    mbr_pos             Position in MBR sector.
 *
 * @param    p_err               Error pointer.
 *
 * @note     (1) The partition table has four entries, each of which can describe a DOS partition.
 *               Each entry has the following fields:
 *               - Starting CHS address
 *               - Ending CHS address
 *               - Starting LBA address
 *               - Number of sectors in partition
 *               - Type of partition
 *******************************************************************************************************/

#if (FS_CORE_CFG_PARTITION_EN == DEF_ENABLED)
static void FSPartition_RdEntry(FS_BLK_DEV_HANDLE blk_dev_handle,
                                FS_PARTITION_INFO *p_partition_info,
                                FS_LB_NBR         mbr_sec,
                                FS_PARTITION_NBR  mbr_pos,
                                RTOS_ERR          *p_err)
{
  CPU_INT08U            *p_buf;
  FS_CACHE_BLK_DEV_DATA *p_cache_blk_dev_data;
  CPU_INT16U            sig_val;

  p_cache_blk_dev_data = FSCache_BlkDevDataGet(blk_dev_handle);
  if (p_cache_blk_dev_data == DEF_NULL) {
    LOG_ERR(("Blk dev must be assigned to a cache instance"));
    RTOS_ERR_SET(*p_err, RTOS_ERR_NULL_PTR);
    return;
  }

  //                                                               ---------------------- RD MBR ----------------------
  FS_CACHE_BLK_RD(p_cache_blk_dev_data, mbr_sec, FS_LB_TYPE_MBR, &p_buf, p_err) {
    //                                                             Check sec sig.
    sig_val = MEM_VAL_GET_INT16U_LITTLE((void *)(p_buf + 510u));
    if (sig_val != 0xAA55u) {                                   // MBR has always signature 0xAA55 at sec end.
      LOG_DBG(("Invalid partition sig: ", (X)sig_val, " != 0xAA55."));
      RTOS_ERR_SET(*p_err, RTOS_ERR_PARTITION_INVALID);
      break;
    }

    //                                                             Rd partition entry and keep 3 infos.
    p_partition_info->StartSec = MEM_VAL_GET_INT32U_LITTLE((void *)(p_buf + FS_PARTITION_DOS_OFF_START_LBA_1      + (mbr_pos * FS_PARTITION_DOS_TBL_ENTRY_SIZE)));
    p_partition_info->SecCnt = MEM_VAL_GET_INT32U_LITTLE((void *)(p_buf + FS_PARTITION_DOS_OFF_SIZE_1           + (mbr_pos * FS_PARTITION_DOS_TBL_ENTRY_SIZE)));
    p_partition_info->Type = MEM_VAL_GET_INT08U_LITTLE((void *)(p_buf + FS_PARTITION_DOS_OFF_PARTITION_TYPE_1 + (mbr_pos * FS_PARTITION_DOS_TBL_ENTRY_SIZE)));

    LOG_DBG(("Found possible partition: Start: ", (u)p_partition_info->StartSec, " sector"));
    LOG_DBG(("                          Size : ", (u)p_partition_info->SecCnt, " sectors"));
    LOG_DBG(("                          Type : ", (X)p_partition_info->Type));
  }
}
#endif

/****************************************************************************************************//**
 *                                               FSPartition_Wr()
 *
 * @brief    Write a partition entry.
 *
 * @param    blk_dev_handle      Handle to a block device.
 *
 * @param    p_partition         Pointer to partition.
 *
 * @param    p_partition_info    Pointer to partition info structure containing size & type of new
 *                               partition. The start sector & size will be updated, if necessary, to
 *                               reflect the characteristics of the created partition.
 *
 * @param    p_err               Error pointer.
 *
 * @note     (1) See 'FSPartition_Rd().
 *               - (a) If the MBR has the maximum number of partition entries, then no new partition
 *                     entry may be created.
 *               - (b) A partition can only be created after the final valid partition/partition entry.
 *                     In this case, an error will ALWAYS have occurred previously while reading the
 *                     partition structure.
 *
 * @note     (2) (a) If the partition is at the 0th entry of an extended partition, then that
 *                   partition must have an illegal signature, a zero first entry or an invalid
 *                   first entry.  In this case, that extended partition's first sector should be
 *                   fully re-initialized.  The size should be checked against the previous MBR
 *                   sector/entry as well.
 *               - (1) If the previous MBR sector/entry is corrupt, e.g., the start sector/size are
 *                       really invalid, then no partition will be created.
 *           - (b) If the partition is at the 1st entry of an extended partition, then a new
 *                   extended partition will be created (space permitting).
 *               - (1) If the 1st entry is not 'zero', then no partition will be created.
 *           - (c) The case in which the position is past the 1st entry of an extended partition is
 *                   included for completeness.  This should NEVER be reached.
 *           - (d) If the partition is within the primary partition, then a new partition entry
 *                   will be created.
 *               - (1) If this is the 4th entry, then no more partitions can be created on the
 *                       device.
 *               - (2) If the partition size in the partition entry passed to this function is too
 *                       large or zero, it will be set to the largest possible size.
 *               - (3) A partition MUST contain at least one sector.
 *******************************************************************************************************/

#if (FS_CORE_CFG_PARTITION_EN == DEF_ENABLED)
#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FSPartition_Wr(FS_BLK_DEV_HANDLE blk_dev_handle,
                           FS_PARTITION      *p_partition,
                           FS_PARTITION_INFO *p_partition_info,
                           RTOS_ERR          *p_err)
{
  FS_LB_QTY dev_lb_cnt;

  dev_lb_cnt = FSBlkDev_LbCntGet(blk_dev_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if (p_partition_info->SecCnt == 0u) {
    p_partition_info->SecCnt = dev_lb_cnt - p_partition_info->StartSec;
  }

  if (p_partition_info->StartSec + p_partition_info->SecCnt > dev_lb_cnt) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_SIZE_INVALID);
    return;
  }

  //                                                               See Note #2d.
  if (p_partition_info->SecCnt < 2u) {                          // See Note #2d3.
    LOG_DBG(("Cannot create partition. Partition would be too small:"));
    LOG_DBG(("Size  = ", (u)p_partition->SecCnt));
    LOG_DBG(("Start = ", (u)p_partition->StartSec));
    LOG_DBG(("End   = ", (u)p_partition->StartSec + p_partition->SecCnt - 1u));
    RTOS_ERR_SET(*p_err, RTOS_ERR_SIZE_INVALID);
    return;
  }

  FSPartition_WrEntry(blk_dev_handle,
                      p_partition_info,
                      p_partition->CurBootRecordSec,
                      p_partition->CurBootRecordPos,
                      p_err);
}
#endif
#endif

/****************************************************************************************************//**
 *                                           FSPartition_WrEntry()
 *
 * @brief    Write a partition entry.
 *
 * @param    blk_dev_handle      Handle to a block device.
 *
 * @param    p_partition_info    Pointer to structure that contains partition information.
 *
 * @param    mbr_sec             MBR sector.
 *
 * @param    mbr_pos             Position in MBR sector.
 *
 * @param    p_err               Error pointer.
 *******************************************************************************************************/

#if (FS_CORE_CFG_PARTITION_EN == DEF_ENABLED)
#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FSPartition_WrEntry(FS_BLK_DEV_HANDLE blk_dev_handle,
                                FS_PARTITION_INFO *p_partition_info,
                                FS_LB_NBR         mbr_lb,
                                FS_PARTITION_NBR  mbr_pos,
                                RTOS_ERR          *p_err)
{
  CPU_INT08U            *p_buf;
  FS_CACHE_BLK_DEV_DATA *p_cache_blk_dev_data;

  WITH_SCOPE_BEGIN(p_err) {
    p_cache_blk_dev_data = FSCache_BlkDevDataGet(blk_dev_handle);
    ASSERT_BREAK_LOG((p_cache_blk_dev_data != DEF_NULL), RTOS_ERR_NULL_PTR,
                     ("Blk dev must be assigned to a cache instance"));

    //                                                             ---------------------- RD MBR ----------------------
    BREAK_ON_ERR(FS_CACHE_BLK_RW)(p_cache_blk_dev_data,
                                  mbr_lb,
                                  FS_LB_TYPE_MBR,
                                  JobSched_VoidJobHandle,
                                  &p_buf,
                                  DEF_NULL,
                                  p_err) {
      //                                                           Set sec sig.
      MEM_VAL_SET_INT16U_LITTLE((void *)(p_buf + 510u), 0xAA55u);

      //                                                           ------------------- WR MBR ENTRY -------------------
      MEM_VAL_SET_INT08U_LITTLE((void *)(p_buf + FS_PARTITION_DOS_OFF_BOOT_FLAG_1      + (mbr_pos * FS_PARTITION_DOS_TBL_ENTRY_SIZE)), FS_PARTITION_DOS_BOOT_FLAG);

      FSPartition_CalcCHS(p_buf + FS_PARTITION_DOS_OFF_START_CHS_ADDR_1 + (mbr_pos * FS_PARTITION_DOS_TBL_ENTRY_SIZE), p_partition_info->StartSec);

      MEM_VAL_SET_INT08U_LITTLE((void *)(p_buf + FS_PARTITION_DOS_OFF_PARTITION_TYPE_1 + (mbr_pos * FS_PARTITION_DOS_TBL_ENTRY_SIZE)), p_partition_info->Type);

      FSPartition_CalcCHS(p_buf + FS_PARTITION_DOS_OFF_END_CHS_ADDR_1   + (mbr_pos * FS_PARTITION_DOS_TBL_ENTRY_SIZE), p_partition_info->StartSec + p_partition_info->SecCnt - 1u);

      MEM_VAL_SET_INT32U_LITTLE((void *)(p_buf + FS_PARTITION_DOS_OFF_START_LBA_1      + (mbr_pos * FS_PARTITION_DOS_TBL_ENTRY_SIZE)), p_partition_info->StartSec);
      MEM_VAL_SET_INT32U_LITTLE((void *)(p_buf + FS_PARTITION_DOS_OFF_SIZE_1           + (mbr_pos * FS_PARTITION_DOS_TBL_ENTRY_SIZE)), p_partition_info->SecCnt);
    }
  } WITH_SCOPE_END
}
#endif
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_FS_AVAIL
