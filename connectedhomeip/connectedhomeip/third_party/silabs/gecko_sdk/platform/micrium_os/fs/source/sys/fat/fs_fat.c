/***************************************************************************//**
 * @file
 * @brief File System - Fat Operations
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

#include  <fs_core_cfg.h>

#if (FS_CORE_CFG_FAT_EN == DEF_ENABLED)

//                                                                 ----------------------- CORE -----------------------
#include  <fs/source/core/fs_core_vol_priv.h>
#include  <fs/source/sys/fs_sys_priv.h>
#include  <fs/source/core/fs_core_cache_priv.h>
#include  <fs/source/core/fs_core_unicode_priv.h>
#include  <common/include/rtos_err.h>

#include  <fs/source/shared/cleanup/cleanup_mgmt_priv.h>

#include  <common/source/logging/logging_priv.h>
#include  <common/source/rtos/rtos_utils_priv.h>

//                                                                 ----------------------- FAT ------------------------
#include  <fs/source/sys/fat/fs_fat_priv.h>
#include  <fs/source/sys/fat/fs_fat_file_priv.h>
#include  <fs/source/sys/fat/fs_fat_dir_priv.h>
#include  <fs/source/sys/fat/fs_fat_vol_priv.h>
#include  <fs/source/sys/fat/fs_fat_xfn_priv.h>
#include  <fs/source/sys/fat/fs_fat_fatxx_priv.h>
#include  <fs/source/sys/fat/fs_fat_journal_priv.h>

//                                                                 ----------------------- EXT ------------------------
#include  <cpu/include/cpu.h>
#include  <common/include/lib_mem.h>

#include "sl_sleeptimer.h"

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  RTOS_MODULE_CUR    RTOS_CFG_MODULE_FS
#define  LOG_DFLT_CH        (FS, FAT, UTILS)

#define  FS_FAT_BOOT_SEC_FMT_OEM_NAME_OFFSET           0x03
#define  FS_FAT_BOOT_SEC_FMT_FAT12_16_SYS_STR_OFFSET   0x36
#define  FS_FAT_BOOT_SEC_FMT_FAT32_SYS_STR_OFFSET      0x52

/********************************************************************************************************
 ********************************************************************************************************
 *                                         GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

FS_FAT_DATA FS_FAT_Data = { .IsInit = DEF_NO };

//                                                                 ------------------- VOID FAT POS -------------------
const FS_FAT_SEC_BYTE_POS FS_FAT_VoidSecBytePos = { (FS_FAT_SEC_NBR)-1, (FS_FAT_SEC_SIZE)-1 };

const FS_FAT_SEC_BYTE_POS FS_FAT_NullSecBytePos = { 0 };

const FS_SYS_POS FS_FAT_VoidEntrySysPos = (FS_SYS_POS)-1;

//                                                                 ------------------- FAT SYS API --------------------
const FS_SYS_API FS_FAT_API = { FS_FAT_BootSecChk,
                                FS_FAT_VolAlloc,
                                FS_FAT_VolFree,
                                FS_FAT_FileDescAlloc,
                                FS_FAT_FileNodeAlloc,
                                FS_FAT_FileDescFree,
                                FS_FAT_FileNodeFree,
#if (FS_CORE_CFG_DIR_EN == DEF_ENABLED)
                                FS_FAT_DirDescAlloc,
                                FS_FAT_DirNodeAlloc,
                                FS_FAT_DirDescFree,
                                FS_FAT_DirNodeFree,
#endif
                                FS_FAT_VolOpen,
                                FS_FAT_VolClose,
#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
                                FS_FAT_VolSync,
                                FS_FAT_VolFmt,
#endif
                                FS_FAT_VolLabelGet,
#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
                                FS_FAT_VolLabelSet,
#endif
                                FS_FAT_VolQuery,
                                FS_FAT_EntryLookup,
                                FS_FAT_FileNodeInit,
                                FS_FAT_FileDescInit,
#if (FS_CORE_CFG_DIR_EN == DEF_ENABLED)
                                FS_FAT_DirNodeInit,
                                FS_FAT_DirDescInit,
#endif
                                FS_FAT_EntryQuery,
#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
                                FS_FAT_EntryCreate,
                                FS_FAT_EntryDel,
                                FS_FAT_EntryRename,
                                FS_FAT_EntryAttribSet,
                                FS_FAT_EntryTimeSet,
#endif

                                FS_FAT_FileRd,
#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
                                FS_FAT_FileWr,
                                FS_FAT_FileTruncate,
                                FS_FAT_FileExtend,
                                FS_FAT_FileSync,
#endif
#if (FS_CORE_CFG_DIR_EN == DEF_ENABLED)
                                FS_FAT_DirRd,
#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
                                FS_FAT_DirChkEmpty,
#endif
#endif
                                FS_FAT_EntryFirstLbGet,
                                FS_FAT_NextLbGet,

                                (FS_SYS_POS)-1,
                                (FS_SYS_POS)0,
                                FS_FAT_VOID_DATA_LB_NBR };

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL MACRO'S
 ********************************************************************************************************
 *******************************************************************************************************/

#define  FS_FAT_DIRENT_DATE_GET_DAY(date)      ((CPU_INT08U) ((CPU_INT08U)((date) >>  0) & 0x1Fu))
#define  FS_FAT_DIRENT_DATE_GET_MONTH(date)    ((CPU_INT08U) ((CPU_INT08U)((date) >>  5) & 0x0Fu))
#define  FS_FAT_DIRENT_DATE_GET_YEAR(date)     ((CPU_INT16U)(((CPU_INT16U)((date) >>  9) & 0x7Fu) + 1980u))

#define  FS_FAT_DIRENT_TIME_GET_SEC(time)      (((CPU_INT08U)((time) >>  0) & 0x1Fu) * 2u)
#define  FS_FAT_DIRENT_TIME_GET_MIN(time)       ((CPU_INT08U)((time) >>  5) & 0x3Fu)
#define  FS_FAT_DIRENT_TIME_GET_HOUR(time)      ((CPU_INT08U)((time) >> 11) & 0x1Fu)

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static FS_FAT_CLUS_NBR FS_FAT_ClusFreeFind(FS_FAT_VOL *p_fat_vol,
                                           RTOS_ERR   *p_err);

static void FS_FAT_DataSecClr(FS_FAT_VOL             *p_fat_vol,
                              FS_FAT_SEC_NBR         start,
                              FS_FAT_SEC_NBR         cnt,
                              FS_CACHE_WR_JOB_HANDLE *p_data_wr_job_handle,
                              CPU_INT08U             fat_lb_type,
                              RTOS_ERR               *p_err);
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               FS_FAT_Fmt()
 *
 * @brief    FAT format a partition.
 *
 * @param    blk_dev_handle  Handle to a block device.
 *
 * @param    partition_nbr   Number of the partition to be formatted. The first partition is partition
 *                           number #1. FS_PARTITION_NBR_VOID to erase the MBR and use the whole media.
 *
 * @param    p_fat_vol_cfg   Pointer to a FAT volume configuration structure (optional).
 *                           DEF_NULL for default values (see Note #1).
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_FOUND
 *                               - RTOS_ERR_VOL_OPENED
 *                               - RTOS_ERR_PARTITION_INVALID
 *                               - RTOS_ERR_BLK_DEV_CORRUPTED
 *                               - RTOS_ERR_BLK_DEV_CLOSED
 *                               - RTOS_ERR_IO
 *
 * @note     (1) In general, the best option for the FAT volume configuration structure is DEF_NULL.
 *               In that case, the File System FAT layer will automatically determine the proper FAT
 *               configuration values based on the block device total size. If you want to specify
 *               the values, the FAT configuration structure is composed of:
 *                   - The cluster size expressed in number of sectors.
 *                   - The reserved area size expressed in number of sectors.
 *                   - The number of entries in the root directory table.
 *                   - The type of FAT: FAT 12, 16 or 32.
 *                   - The number of FAT tables.
 *               @n
 *               You may use these typical values for the FAT configuration structure:
 *               @n
 *               Cluster size: 1, 2, 4, 8, 16, 32, 64
 *               Reserved area size:
 *                   - FS_FAT_DFLT_RSVD_SEC_CNT_FAT12
 *                   - FS_FAT_DFLT_RSVD_SEC_CNT_FAT16
 *                   - FS_FAT_DFLT_RSVD_SEC_CNT_FAT32
 *               @n
 *               Number of entries in the root directory table:
 *                   - FS_FAT_DFLT_ROOT_ENT_CNT_FAT12
 *                   - FS_FAT_DFLT_ROOT_ENT_CNT_FAT16
 *                   - FS_FAT_DFLT_ROOT_ENT_CNT_FAT32
 *               @n
 *               Type of FAT:
 *                   - FS_FAT_TYPE_FAT12
 *                   - FS_FAT_TYPE_FAT16
 *                   - FS_FAT_TYPE_FAT32
 *               @n
 *               Number of FAT tables:
 *                   - FS_FAT_DFLT_NBR_FATS_FAT12
 *                   - FS_FAT_DFLT_NBR_FATS_FAT16
 *                   - FS_FAT_DFLT_NBR_FATS_FAT32
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FS_FAT_Fmt(FS_BLK_DEV_HANDLE blk_dev_handle,
                FS_PARTITION_NBR  partition_nbr,
                FS_FAT_VOL_CFG    *p_fat_vol_cfg,
                RTOS_ERR          *p_err)
{
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  RTOS_ASSERT_DBG_ERR_SET(FS_FAT_Data.IsInit, *p_err, RTOS_ERR_NOT_INIT,; );

  FSVol_Fmt(blk_dev_handle,
            partition_nbr,
            FS_SYS_TYPE_FAT,
            (void *)p_fat_vol_cfg,
            p_err);
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               FS_FAT_Init()
 *
 * @brief    Initialize FAT file system driver.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/
void FS_FAT_Init(RTOS_ERR *p_err)
{
  FS_FAT_FileModuleInit(p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

#if (FS_CORE_CFG_DIR_EN == DEF_ENABLED)
  FS_FAT_DirModuleInit(p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
#endif

  FS_FAT_VolModuleInit(p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  FS_FAT_Data.IsInit = DEF_YES;
}

/****************************************************************************************************//**
 *                                               FS_FAT_DateFmt()
 *
 * @brief    Format date for directory entry.
 *
 * @param    p_date_time  Pointer to date/time structure.
 *
 * @return   Formatted    date for directory entry.
 *
 * @note     (1) The FAT date stamp encodes the date as follows :
 *               - (a) Bits 0 -  4 : Day of month,     1-  31 inclusive.
 *               - (b) Bits 5 -  8 : Month of year,    1-  12 inclusive.
 *               - (c) Bits 9 - 15 : Years,         1980-2107 inclusive.
 *
 * @note     (2) According to [Ref #1], unsupported date & time fields should be set to 0.
 *               Consequently, if any illegal parameters are passed, the date is returned as 0.
 *
 * @note     (3) Expect valid data time. The date/time should be validated by the caller(s).
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
FS_FAT_DATE FS_FAT_DateFmt(sl_sleeptimer_date_t *p_date_time)
{
  CPU_INT16U  date_val;

  if ((p_date_time->year >= 1980u) && (p_date_time->year <= 2107u)) {
    date_val = ((CPU_INT16U)((p_date_time->month_day) << 0)
                +  (CPU_INT16U)((p_date_time->month) << 5)
                +  (CPU_INT16U)((p_date_time->year - 1980u) << 9));
  } else {
    date_val = 0u;
  }

  return (date_val);
}
#endif

/****************************************************************************************************//**
 *                                               FS_FAT_TimeFmt()
 *
 * @brief    Format time for directory entry.
 *
 * @param    p_date_time  Pointer to date/time structure.
 *
 * @return   Formatted    time for directory entry.
 *
 * @note     (1) The FAT time stamp encodes the date as follows :
 *               - (a) Bits  0 -  4 : 2-second count, 0-29 inclusive (0-58 seconds).
 *               - (b) Bits  5 - 10 : Minutes,        0-59 inclusive.
 *               - (c) Bits 11 - 15 : Hours,          0-23 inclusive.
 *
 * @note     (2) According to Microsoft ('FS_FAT_VolOpen() Ref #1'), unsupported date & time fields
 *               should be set to 0. Consequently, if any illegal parameters are passed, the time is
 *               returned as 0.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
FS_FAT_TIME FS_FAT_TimeFmt(sl_sleeptimer_date_t *p_date_time)
{
  CPU_INT16U time_val;

  if ((p_date_time->sec < 61u) && (p_date_time->min < 60u) && (p_date_time->hour < 24u)) {
    time_val = (CPU_INT16U)((CPU_INT16U)(p_date_time->sec >> 1)
                            + (CPU_INT16U)(p_date_time->min << 5)
                            + (CPU_INT16U)(p_date_time->hour << 11));
  } else {
    time_val = 0u;
  }

  return (time_val);
}
#endif

/****************************************************************************************************//**
 *                                           FS_FAT_DateTimeParse()
 *
 * @brief    Parse date & time for directory entry.
 *
 * @param    p_ts    Pointer to timestamp.
 *
 * @param    date    FAT-formatted date.
 *
 * @param    time    FAT-formatted time.
 *
 * @note     (1) See 'FS_FAT_DateFmt() Note #1' & 'FS_FAT_TimeFmt() Note #1'.
 *
 * @note     (2) If the date is passed as 0, or if the date is an illegal value, then the date fields
 *               of 'p_time' will be cleared.
 *
 * @note     (3) If the time is passed as 0, or if the time is an illegal value, then the time fields
 *               of 'stime' will be cleared.
 *******************************************************************************************************/
void FS_FAT_DateTimeParse(sl_sleeptimer_timestamp_t  *p_ts,
                          FS_FAT_DATE                 date,
                          FS_FAT_TIME                 time)
{
  sl_sleeptimer_timestamp_t ts;
  sl_sleeptimer_date_t date_time;
  sl_status_t status;

  if (date != 0u) {                                             // See Note #2.
    date_time.month_day = FS_FAT_DIRENT_DATE_GET_DAY(date);
    date_time.month = FS_FAT_DIRENT_DATE_GET_MONTH(date);
    date_time.year = FS_FAT_DIRENT_DATE_GET_YEAR(date);

    if (!((date_time.month_day >= 1) && (date_time.month_day <= 31)
          && (date_time.month >= 1) && (date_time.month <= 12)
          && (date_time.year >= 1970) && (date_time.year <= 2107))) {
      *p_ts = FS_TIME_TS_INVALID;
      return;
    }
  } else {
    *p_ts = FS_TIME_TS_INVALID;
    return;
  }

  date_time.sec = FS_FAT_DIRENT_TIME_GET_SEC(time);
  date_time.min = FS_FAT_DIRENT_TIME_GET_MIN(time);
  date_time.hour = FS_FAT_DIRENT_TIME_GET_HOUR(time);
  date_time.time_zone = 0;

  if (!((date_time.sec < 61u) && (date_time.min < 60u) && (date_time.hour < 24u))) {
    *p_ts = FS_TIME_TS_INVALID;
    return;
  }

  status = sl_sleeptimer_convert_date_to_time(&date_time, &ts);
  if (status != SL_STATUS_OK) {
    *p_ts = FS_TIME_TS_INVALID;
    return;
  }

  *p_ts = ts;
}

/****************************************************************************************************//**
 *                                           FS_FAT_ClusChainAlloc()
 *
 * @brief    Allocate new cluster chain or extend existing one.
 *
 * @param    p_fat_vol                   Pointer to FAT volume object.
 *
 * @param    start_clus                  First cluster of the chain from which allocation/extension
 *                                       will take place.
 *
 * @param    p_last_clus                 Pointer to a variable that will receive the number of the
 *                                       last allocated cluster OR DEF_NULL.
 *
 * @param    nbr_clus                    Number of clusters to allocate.
 *
 * @param    clr                         Indicates whether the newly allocated clusters must be
 *                                       cleared or not:
 *                                       DEF_YES,  if clusters cleared.
 *                                       DEF_NO,   if clusters not cleared.
 *
 * @param    p_data_cache_blk_handle     Pointer to a job handle.
 *
 * @param    fat_lb_type                 Logical block type.
 *                                           - FS_FAT_LB_TYPE_DIRENT       Directory entry.
 *                                           - FS_FAT_LB_TYPE_FAT          FAT.
 *                                           - FS_FAT_LB_TYPE_DATA         Data.
 *                                           - FS_FAT_LB_TYPE_JNL_DIRENT   Journal directory entry.
 *                                           - FS_FAT_LB_TYPE_JNL_DATA     Journal data.
 *                                           - FS_FAT_LB_TYPE_RSVD_AREA    Reserved area.
 *
 * @param    p_err                       Error pointer.
 *
 * @return   Index of the cluster from which the allocation/extension takes place.
 *
 * @note     (1) Uncompleted allocations are rewinded using reverse deletion. By doing so, we make sure
 *               deletion can always be completed after a potential failure (even without journaling).
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
FS_FAT_CLUS_NBR FS_FAT_ClusChainAlloc(FS_FAT_VOL             *p_fat_vol,
                                      FS_FAT_CLUS_NBR        start_clus,
                                      FS_FAT_CLUS_NBR        *p_last_clus,
                                      FS_FAT_CLUS_NBR        nbr_clus,
                                      CPU_BOOLEAN            clr,
                                      FS_CACHE_WR_JOB_HANDLE *p_data_cache_blk_handle,
                                      CPU_INT08U             fat_lb_type,
                                      RTOS_ERR               *p_err)
{
  FS_FAT_TYPE_API *p_fat_api;
  FS_FAT_CLUS_NBR rem_clus;
  FS_FAT_CLUS_NBR cur_clus;
  FS_FAT_CLUS_NBR next_clus;
  FS_FAT_CLUS_NBR first_clus;
  CPU_BOOLEAN     is_new_chain;
#if ((FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED) \
  && (FS_FAT_CFG_JOURNAL_EN == DEF_ENABLED))
  FS_CACHE_WR_JOB_HANDLE stub_job_handle = FS_CACHE_WR_JOB_HANDLE_INIT;
#endif

  RTOS_ASSERT_CRITICAL(nbr_clus > 0u, RTOS_ERR_INVALID_ARG, 0u);

  rem_clus = nbr_clus;
  is_new_chain = DEF_NO;
  first_clus = 0u;

  LOG_VRB(("Allocating a chain of ", (u)nbr_clus, " cluster(s) from cluster ", (u)start_clus, "."));

  WITH_SCOPE_BEGIN(p_err) {
    p_fat_api = FS_FAT_TYPE_API_GET(p_fat_vol);

    //                                                             ----------------- FIND START CLUS ------------------
    if (start_clus == 0u) {                                     // If new chain, find start clus.
      BREAK_ON_ERR(start_clus = FS_FAT_ClusFreeFind(p_fat_vol, p_err));
      ASSERT_BREAK(start_clus != 0u, RTOS_ERR_VOL_FULL);
      first_clus = start_clus;
      if (clr == DEF_YES) {
        BREAK_ON_ERR(FS_FAT_DataSecClr(p_fat_vol,
                                       FS_FAT_CLUS_TO_SEC(p_fat_vol, start_clus),
                                       FS_UTIL_PWR2(p_fat_vol->SecPerClusLog2),
                                       p_data_cache_blk_handle,
                                       fat_lb_type,
                                       p_err));
      }

      is_new_chain = DEF_YES;
      rem_clus--;
      LOG_VRB(("The new chain will start with cluster ", (u)start_clus, "."));
    }
#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
    else {
      BREAK_ON_ERR(next_clus = p_fat_api->ClusValRd(p_fat_vol,
                                                    start_clus,
                                                    p_err));

      RTOS_ASSERT_CRITICAL_ERR_SET(next_clus >= p_fat_api->ClusEOF,
                                   *p_err, RTOS_ERR_VOL_CORRUPTED, 0u);
    }
#endif

    //                                                             ------------------ ENTER JOURNAL -------------------
#if (FS_FAT_CFG_JOURNAL_EN == DEF_ENABLED)
    if (p_fat_vol->IsJournaled) {
      BREAK_ON_ERR(FS_FAT_JournalEnterClusChainAlloc(p_fat_vol,
                                                     start_clus,
                                                     is_new_chain,
                                                     p_err));

#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
      BREAK_ON_ERR(stub_job_handle = FSCache_WrJobAppend(p_fat_vol->Vol.CacheBlkDevDataPtr->CachePtr,
                                                         p_fat_vol->FatWrJobHandle,
                                                         p_err));

      BREAK_ON_ERR(p_fat_vol->FatWrJobHandle = FSCache_WrJobJoin(p_fat_vol->Vol.CacheBlkDevDataPtr->CachePtr,
                                                                 p_fat_vol->JournalData.JnlWrJobHandle,
                                                                 stub_job_handle,
                                                                 p_err));
#endif
    }

#endif

    ON_BREAK {
      RTOS_ERR err_tmp = RTOS_ERR_INIT_CODE(RTOS_ERR_NONE);
      FS_FAT_ClusChainReverseDel(p_fat_vol,                     // Del partially alloc'd clus chain (see Note #1).
                                 start_clus,
                                 is_new_chain,
                                 &err_tmp);
      if (RTOS_ERR_CODE_GET(err_tmp) != RTOS_ERR_NONE) {
        LOG_ERR(("Error ", RTOS_ERR_LOG_ARG_GET(err_tmp), " occured while reverting clus chain alloc."));
      }
      break;
    } WITH {
      //                                                           ----------------- ALLOC CLUS CHAIN -----------------
      cur_clus = start_clus;
      BREAK_ON_ERR(while) (rem_clus > 0u) {
        //                                                         Find next clus in chain.
        BREAK_ON_ERR(next_clus = FS_FAT_ClusFreeFind(p_fat_vol, p_err));

        ASSERT_BREAK((next_clus != 0u) && (next_clus != cur_clus), RTOS_ERR_VOL_FULL);

        if (rem_clus == nbr_clus) {
          first_clus = next_clus;
        }

        LOG_VRB(("Next cluster in chain is ", (u)next_clus, "."));

        if (clr) {
          BREAK_ON_ERR(FS_FAT_DataSecClr(p_fat_vol,
                                         FS_FAT_CLUS_TO_SEC(p_fat_vol, next_clus),
                                         FS_UTIL_PWR2(p_fat_vol->SecPerClusLog2),
                                         p_data_cache_blk_handle,
                                         fat_lb_type,
                                         p_err));
        }

        BREAK_ON_ERR(p_fat_api->ClusValWr(p_fat_vol,            // Update FAT chain.
                                          cur_clus,
                                          next_clus,
                                          p_err));
        cur_clus = next_clus;
        rem_clus--;
      }

      BREAK_ON_ERR(p_fat_api->ClusValWr(p_fat_vol,              // Update FAT chain (EOC).
                                        cur_clus,
                                        p_fat_api->ClusEOF,
                                        p_err));

      if (p_last_clus != DEF_NULL) {
        *p_last_clus = cur_clus;
      }
    }

#if ((FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED) \
    && (FS_FAT_CFG_JOURNAL_EN == DEF_ENABLED))
    if (p_fat_vol->IsJournaled) {
      FSCache_StubWrJobRem(p_fat_vol->Vol.CacheBlkDevDataPtr->CachePtr, stub_job_handle);
    }
#endif

    //                                                             ------------------- UPDATE & RTN -------------------
    LOG_VRB(((u)nbr_clus, " cluster(s) were allocated from cluster ", (u)start_clus, "."));

    FS_CTR_STAT_INC(p_fat_vol->StatAllocClusCtr);

    if (p_fat_vol->QueryInfoValid) {                            // Update query info.
      p_fat_vol->QueryFreeClusCnt -= nbr_clus;
    }
  } WITH_SCOPE_END

  return (first_clus);
}
#endif

/****************************************************************************************************//**
 *                                           FS_FAT_ClusChainDel()
 *
 * @brief    Forward delete FAT cluster chain.
 *
 * @param    p_fat_vol   Pointer to FAT volume object.
 *
 * @param    first_clus  First cluster of the cluster chain to be deleted.
 *
 * @param    del_first   Indicates whether first cluster should be deleted :
 *                       DEF_NO,  if first clus will be marked EOF.
 *                       DEF_YES, if first clus will be marked free.
 *
 * @param    p_err       Error pointer.
 *
 * @return   Number of clusters deleted.
 *
 * @note     (1) All clusters located after 'start_clus' will be deleted, that is, until an end of cluster
 *               chain or an invalid cluster is found. In both cases, no error is returned.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
FS_FAT_CLUS_NBR FS_FAT_ClusChainDel(FS_FAT_VOL      *p_fat_vol,
                                    FS_FAT_CLUS_NBR first_clus,
                                    CPU_BOOLEAN     del_first,
                                    RTOS_ERR        *p_err)
{
  FS_FAT_TYPE_API *p_fat_api;
  FS_FAT_CLUS_NBR cur_clus;
  FS_FAT_CLUS_NBR next_clus;
  FS_FAT_CLUS_NBR clus_cnt;
  FS_FAT_CLUS_NBR rtn_cnt;
  FS_FAT_SEC_NBR  cur_sec;
  FS_FAT_CLUS_NBR new_fat_entry;
  CPU_BOOLEAN     del;

  del = del_first;
  cur_clus = first_clus;
  rtn_cnt = 0u;

  WITH_SCOPE_BEGIN(p_err) {
    //                                                             --------------- VALIDATE START CLUS ----------------
    ASSERT_BREAK(FS_FAT_IS_VALID_CLUS(p_fat_vol, first_clus), RTOS_ERR_INVALID_ARG);

    p_fat_api = FS_FAT_TYPE_API_GET(p_fat_vol);

#if  (FS_FAT_CFG_JOURNAL_EN == DEF_ENABLED)
    {
      //                                                           Cnt clus's.
      FS_FAT_CLUS_NBR last_clus_tbl[3];
      BREAK_ON_ERR(clus_cnt = FS_FAT_ClusChainEndFind(p_fat_vol,
                                                      first_clus,
                                                      last_clus_tbl,
                                                      p_err));

      //                                                           Add journal log.
      BREAK_ON_ERR(FS_FAT_JournalEnterClusChainDel(p_fat_vol,
                                                   first_clus,
                                                   clus_cnt,
                                                   del_first,
                                                   p_err));
    }
#endif

    //                                                             ------------------- FREE CLUS'S --------------------
    clus_cnt = 0u;
    //                                                             *INDENT-OFF*
    BREAK_ON_ERR(do ) {
                                                                // Rd next FAT entry.
      BREAK_ON_ERR(next_clus = p_fat_api->ClusValRd(p_fat_vol,
                                                    cur_clus,
                                                    p_err));

      if (!del) {                                               // If first clus must be preserved ...
        del = DEF_YES;
        new_fat_entry = p_fat_api->ClusEOF;                     // ... set to EOC.
        LOG_VRB(("Cluster marked as EOC: ", (u)cur_clus, "."));
      } else {                                                  // If clus must be del'd ...
        new_fat_entry = p_fat_api->ClusFree;                    // ... set to clus free mark.
        cur_sec = FS_FAT_CLUS_TO_SEC(p_fat_vol, cur_clus);

        BREAK_ON_ERR(FSVol_Trim(&p_fat_vol->Vol,
                                cur_sec,
                                FS_UTIL_PWR2(p_fat_vol->SecPerClusLog2),
                                p_err));

        if (p_fat_vol->QueryInfoValid) {
          p_fat_vol->QueryFreeClusCnt++;
        }

        FS_CTR_STAT_INC(p_fat_vol->StatFreeClusCtr);
        clus_cnt++;                                             // Inc del clus cnt.
        LOG_VRB(("Cluster freed: ", (u)cur_clus, "."));
      }

      BREAK_ON_ERR(p_fat_api->ClusValWr(p_fat_vol,              // Wr FAT entry.
                                        cur_clus,
                                        new_fat_entry,
                                        p_err));

      cur_clus = next_clus;                                     // Update cur clus.
    } while (FS_FAT_IS_VALID_CLUS(p_fat_vol, cur_clus));
    //                                                             *INDENT-ON*

    //                                                             ---------------- ALL CLUS'S FREE'D -----------------
    if (cur_clus >= p_fat_api->ClusEOF) {
      LOG_VRB(((u)clus_cnt, " cluster(s) freed upon reaching cluster chain end."));
    } else {
      LOG_DBG(((u)clus_cnt, " cluster(s) freed upon reaching invalid cluster."));
    }

    rtn_cnt = clus_cnt;
  } WITH_SCOPE_END

  return (rtn_cnt);
}
#endif

/****************************************************************************************************//**
 *                                       FS_FAT_ClusChainReverseDel()
 *
 * @brief    Reverse delete FAT cluster chain.
 *
 * @param    p_fat_vol   Pointer to FAT volume object.
 *
 * @param    first_clus  First cluster of the cluster chain.
 *
 * @param    del_first   Indicates whether first cluster should be deleted :
 *                       DEF_NO,  if first clus will be marked EOF.
 *                       DEF_YES, if first clus will be marked free.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FS_FAT_ClusChainReverseDel(FS_FAT_VOL      *p_fat_vol,
                                FS_FAT_CLUS_NBR first_clus,
                                CPU_BOOLEAN     del_first,
                                RTOS_ERR        *p_err)
{
  FS_FAT_TYPE_API *p_fat_api;
  FS_FAT_CLUS_NBR cur_clus;
  FS_FAT_SEC_NBR  cur_sec;
  FS_FAT_CLUS_NBR new_fat_entry;
  FS_FAT_CLUS_NBR last_clus_tbl[3];
#if LOG_VRB_IS_EN()
  FS_FAT_CLUS_NBR clus_cnt = 0u;
#endif

  WITH_SCOPE_BEGIN(p_err) {
    //                                                             ------------------- FREE CLUS'S --------------------
    p_fat_api = FS_FAT_TYPE_API_GET(p_fat_vol);
    //                                                             *INDENT-OFF*
    BREAK_ON_ERR(do ) {
                                                                // Find chain end.
      BREAK_ON_ERR((void)FS_FAT_ClusChainEndFind(p_fat_vol,
                                                 first_clus,
                                                 last_clus_tbl,
                                                 p_err));

      cur_clus = (last_clus_tbl[0] == p_fat_api->ClusFree) ? last_clus_tbl[2]
                 : last_clus_tbl[1];
      if (cur_clus == 0u) {
        break;
      }

      if ((cur_clus == first_clus) && !del_first) {             // If start clus must be preserved ...
        new_fat_entry = p_fat_api->ClusEOF;                     // ... set to EOC.
      } else {
        new_fat_entry = p_fat_api->ClusFree;                    // If clus must be del'd ...
        LOG_VRB(("Cluster ", (u)cur_clus, " freed."));          // ... set to clus free mark.

        cur_sec = FS_FAT_CLUS_TO_SEC(p_fat_vol, cur_clus);
        BREAK_ON_ERR(FSVol_Trim(&p_fat_vol->Vol,
                                cur_sec,
                                FS_UTIL_PWR2(p_fat_vol->SecPerClusLog2),
                                p_err));

        if (p_fat_vol->QueryInfoValid) {
          p_fat_vol->QueryFreeClusCnt++;
        }

        FS_CTR_STAT_INC(p_fat_vol->StatFreeClusCtr);
#if LOG_VRB_IS_EN()
        clus_cnt++;                                             // Inc clus cnt.
#endif
                     }

                                                                // ------------------- WR FAT ENTRY -------------------
      BREAK_ON_ERR(p_fat_api->ClusValWr(p_fat_vol,
                                        cur_clus,
                                        new_fat_entry,
                                        p_err));
    } while (first_clus != cur_clus);                           // Start over until start clus has been del'd.
    //                                                             *INDENT-ON*

    //                                                             ---------------- ALL CLUS'S FREE'D -----------------
    LOG_VRB(((u)clus_cnt, " cluster(s) freed."));
  } WITH_SCOPE_END
}
#endif

/****************************************************************************************************//**
 *                                           FS_FAT_ClusChainFollow()
 *
 * @brief    Follow FAT cluster chain for a given number of clusters.
 *
 * @param    p_fat_vol           Pointer to FAT volume object.
 *
 * @param    start_clus          Cluster to start following from.
 *
 * @param    len                 Number of clusters to follow.
 *
 * @param    p_last_clus_tbl     Pointer to a table that will receive the number of the last valid
 *                               cluster and the previous/next cluster number surrounding the last
 *                               cluster.
 *
 * @param    p_err               Error pointer.
 *
 * @return   Number of visited clusters.
 *
 * @note     (1) If 'len', the number of clusters to follow, is zero, the start cluster will be
 *               returned.  Otherwise, the cluster chain will be followed 'len' clusters or until chain
 *               ends, starting at cluster 'start_clus'.
 *******************************************************************************************************/
FS_FAT_CLUS_NBR FS_FAT_ClusChainFollow(FS_FAT_VOL      *p_fat_vol,
                                       FS_FAT_CLUS_NBR start_clus,
                                       FS_FAT_CLUS_NBR len,
                                       FS_FAT_CLUS_NBR *p_last_clus_tbl,
                                       RTOS_ERR        *p_err)
{
  FS_FAT_TYPE_API *p_fat_api;
  FS_FAT_CLUS_NBR prev_fat_entry;
  FS_FAT_CLUS_NBR cur_fat_entry;
  FS_FAT_CLUS_NBR next_fat_entry;
  FS_FAT_CLUS_NBR cnt;

  WITH_SCOPE_BEGIN(p_err) {
    cnt = 0u;
    next_fat_entry = start_clus;
    cur_fat_entry = 0u;
    prev_fat_entry = 0u;

    //                                                             --------------- VALIDATE START CLUS ----------------
    ASSERT_BREAK(FS_FAT_IS_VALID_CLUS(p_fat_vol, start_clus), RTOS_ERR_INVALID_ARG);

    p_fat_api = FS_FAT_TYPE_API_GET(p_fat_vol);

    //                                                             ------------------- FOLLOW CHAIN -------------------
    BREAK_ON_ERR(for) (cnt = 0u; cnt < len; cnt++) {
      if (!FS_FAT_IS_VALID_CLUS(p_fat_vol, next_fat_entry)) {
        break;
      }

      prev_fat_entry = cur_fat_entry;
      cur_fat_entry = next_fat_entry;
      BREAK_ON_ERR(next_fat_entry = p_fat_api->ClusValRd(p_fat_vol,
                                                         cur_fat_entry,
                                                         p_err));
    }

    p_last_clus_tbl[0] = next_fat_entry;
    p_last_clus_tbl[1] = cur_fat_entry;
    p_last_clus_tbl[2] = prev_fat_entry;
  } WITH_SCOPE_END

  return (cnt);
}

/****************************************************************************************************//**
 *                                           FS_FAT_ClusChainEndFind()
 *
 * @brief    Follow FAT cluster chain until the end.
 *
 * @param    p_fat_vol           Pointer to a FAT volume object.
 *
 * @param    start_clus          Cluster to start following from.
 *
 * @param    p_last_clus_tbl     Pointer to table that will receive information about clusters followed.
 *
 * @param    p_err               Error pointer.
 *
 * @return   Number of visited clusters.
 *******************************************************************************************************/
FS_FAT_CLUS_NBR FS_FAT_ClusChainEndFind(FS_FAT_VOL      *p_fat_vol,
                                        FS_FAT_CLUS_NBR start_clus,
                                        FS_FAT_CLUS_NBR *p_last_clus_tbl,
                                        RTOS_ERR        *p_err)
{
  FS_FAT_CLUS_NBR clus_cnt;

  clus_cnt = FS_FAT_ClusChainFollow(p_fat_vol,
                                    start_clus,
                                    (FS_FAT_CLUS_NBR) -1,
                                    p_last_clus_tbl,
                                    p_err);
  return (clus_cnt);
}

/****************************************************************************************************//**
 *                                       FS_FAT_ClusChainReverseFollow()
 *
 * @brief    Reverse follow FAT cluster chain.
 *
 * @param    p_fat_vol   Pointer to FAT volume object.
 *
 * @param    start_clus  Cluster to start following from.
 *
 * @param    stop_clus   Cluster to stop following at.
 *
 * @param    p_err       Error pointer.
 *
 * @return   Cluster number of the last valid cluster found.
 *
 * @note     (1) File allocation table is browsed backward, entry by entry, starting at the entry right
 *               before the current known first cluster entry. Since cluster chains are mostly allocated
 *               forward, this allows many clusters to be followed within a single revolution around the
 *               file allocation table.
 *
 * @note     (2) If the entry lookup returns back to its starting point after wrapping around, then no
 *               cluster points to the current target cluster and, therefore, the first cluster of the
 *               chain has been found.
 *
 * @note     (3) No error is returned in case chain following ends before the stop cluster is reached.
 *******************************************************************************************************/
FS_FAT_CLUS_NBR FS_FAT_ClusChainReverseFollow(FS_FAT_VOL      *p_fat_vol,
                                              FS_FAT_CLUS_NBR start_clus,
                                              FS_FAT_CLUS_NBR stop_clus,
                                              RTOS_ERR        *p_err)
{
  FS_FAT_TYPE_API *p_fat_api;
  FS_FAT_CLUS_NBR cur_clus;
  FS_FAT_CLUS_NBR next_clus;
  FS_FAT_CLUS_NBR target_clus;
  FS_FAT_CLUS_NBR rtn_clus;
#if LOG_DBG_IS_EN()
  FS_FAT_CLUS_NBR found_clus_cnt;
#endif

  rtn_clus = 0u;
  target_clus = start_clus;
  cur_clus = start_clus - 1u;
#if LOG_DBG_IS_EN()
  found_clus_cnt = 0u;
#endif

  WITH_SCOPE_BEGIN(p_err) {
    //                                                             ------------ VALIDATE START & STOP CLUS ------------
    ASSERT_BREAK(FS_FAT_IS_VALID_CLUS(p_fat_vol, start_clus), RTOS_ERR_INVALID_ARG);
    ASSERT_BREAK(FS_FAT_IS_VALID_CLUS(p_fat_vol, stop_clus), RTOS_ERR_INVALID_ARG);

    p_fat_api = FS_FAT_TYPE_API_GET(p_fat_vol);

    //                                                             ---------------- PERFORM FAT LOOKUP ----------------
    BREAK_ON_ERR(while) (target_clus != stop_clus) {
      if (cur_clus < FS_FAT_MIN_CLUS_NBR) {                     // Wrap clus nbr.
        cur_clus = FS_FAT_MIN_CLUS_NBR + p_fat_vol->ClusCnt - 1u;
      }
      //                                                           Chk if start has been reached (see Note #2).
      if (cur_clus == target_clus) {
        break;
      }
      //                                                           Rd next FAT entry.
      BREAK_ON_ERR(next_clus = p_fat_api->ClusValRd(p_fat_vol,
                                                    cur_clus,
                                                    p_err));

      //                                                           Update target clus.
      if (next_clus == target_clus) {
        target_clus = cur_clus;
#if LOG_DBG_IS_EN()
        found_clus_cnt++;
#endif
      }
      cur_clus--;
    }

    //                                                             ---------------- STOP CLUS REACHED -----------------
#if LOG_DBG_IS_EN()
    if (cur_clus == target_clus) {
      LOG_DBG(("Reached start of cluster chain after ", (u)found_clus_cnt, " cluster(s)."));
    } else {
      LOG_DBG(("Stop cluster reached after ", (u)found_clus_cnt, " cluster(s)."));
    }
#endif

    rtn_clus = target_clus;
  } WITH_SCOPE_END

  return (rtn_clus);
}

/****************************************************************************************************//**
 *                                           FS_FAT_SecNextGet()
 *
 * @brief    Get next sector in cluster chain.
 *
 * @param    p_fat_vol   Pointer to FAT volume object.
 *
 * @param    start_sec   Current sector number.
 *
 * @param    p_err       Error pointer.
 *
 * @return   Next sector number (relative to partition start), if a next sector exists.
 *           0, if the current sector is the last in the cluster chain.
 *******************************************************************************************************/
FS_FAT_SEC_NBR FS_FAT_SecNextGet(FS_FAT_VOL     *p_fat_vol,
                                 FS_FAT_SEC_NBR start_sec,
                                 RTOS_ERR       *p_err)
{
  FS_FAT_TYPE_API *p_fat_api;
  FS_FAT_SEC_NBR  clus_sec_rem;
  FS_FAT_CLUS_NBR next_clus;
  FS_FAT_SEC_NBR  next_sec;
  FS_FAT_SEC_NBR  root_dir_sec_last;
  FS_FAT_CLUS_NBR start_clus;

  WITH_SCOPE_BEGIN(p_err) {
    p_fat_api = FS_FAT_TYPE_API_GET(p_fat_vol);
    next_sec = FS_FAT_VOID_DATA_LB_NBR;
    //                                                             ----------------- HANDLE ROOT DIR ------------------
    if (p_fat_vol->FAT_Type != FS_FAT_TYPE_FAT32) {
      if (start_sec < p_fat_vol->DataAreaStart) {               // If start sec outside data          ...
        if (start_sec >= p_fat_vol->RootDirStart) {             // ... & sec within root dir          ...
          root_dir_sec_last = p_fat_vol->RootDirSize
                              + p_fat_vol->RootDirStart - 1u;
          if (start_sec < root_dir_sec_last) {                  // ... & sec before last root dir sec ...
            next_sec = start_sec + 1u;                          // ... rtn next sec.
            goto rtn;
          }
        }

        next_sec = FS_FAT_VOID_DATA_LB_NBR;
        goto rtn;
      }
    }

    //                                                             ---------------- HANDLE GENERAL SEC ----------------
    clus_sec_rem = FS_FAT_CLUS_SEC_REM(p_fat_vol, start_sec);

    if (clus_sec_rem > 1u) {                                    // If more secs rem in clus ...
      next_sec = start_sec + 1u;                                // ... rtn next sec ...
      goto rtn;
    }
    //                                                             ... otherwise, find next clus.
    start_clus = FS_FAT_SEC_TO_CLUS(p_fat_vol, start_sec);

    BREAK_ON_ERR(next_clus = p_fat_api->ClusValRd(p_fat_vol,
                                                  start_clus,
                                                  p_err));

    ASSERT_BREAK(FS_FAT_IS_VALID_CLUS(p_fat_vol, next_clus) || (next_clus >= p_fat_api->ClusEOF),
                 RTOS_ERR_VOL_CORRUPTED);

    next_sec = (next_clus >= p_fat_api->ClusEOF) ? FS_FAT_VOID_DATA_LB_NBR
               : FS_FAT_CLUS_TO_SEC(p_fat_vol, next_clus);

rtn:;
  } WITH_SCOPE_END

  return (next_sec);
}

/****************************************************************************************************//**
 *                                           FS_FAT_BootSecChk()
 *
 * @brief    Check boot sector.
 *
 * @param    blk_dev_handle      Block device handle.
 *
 * @param    p_partition_info    Pointer to variable that will receive the partition information.
 *
 * @param    mbr_sec             MBR sector.
 *
 * @param    p_err               Error pointer.
 *
 * @return   DEF_TRUE, if a FAT boot sector is found
 *           DEF_FALSE, if FAT boot sector was not found
 *
 * @note     (1) In a FAT boot sector, bytes from 3 to 10 contains an "eight-character string
 *               called the OEM name that may correspond to what tool was used to make the file system.
 *               For example, a Windows 95 system sets it to "MSWIN4.0", a Windows 98 system
 *               sets it to "MSWIN4.1", and a Windows XP or 2000 system sets it to "MSDOS5.0".
 *               Microsoft recommends "MSWIN4.1" for maximum compatibility.
 *******************************************************************************************************/
CPU_BOOLEAN FS_FAT_BootSecChk(FS_BLK_DEV_HANDLE blk_dev_handle,
                              FS_PARTITION_INFO *p_partition_info,
                              FS_LB_NBR         mbr_sec,
                              RTOS_ERR          *p_err)
{
  CPU_INT08U            *p_buf;
  FS_CACHE_BLK_DEV_DATA *p_cache_blk_dev_data;
  CPU_INT16S            comp_val;
  CPU_BOOLEAN           boot_chk = DEF_FALSE;

  p_cache_blk_dev_data = FSCache_BlkDevDataGet(blk_dev_handle);
  if (p_cache_blk_dev_data == DEF_NULL) {
    LOG_ERR(("Blk dev must be assigned to a cache instance"));
    RTOS_ERR_SET(*p_err, RTOS_ERR_NULL_PTR);
    return (DEF_FALSE);
  }

  //                                                               ---------------------- RD MBR ----------------------
  FS_CACHE_BLK_RD(p_cache_blk_dev_data, mbr_sec, FS_LB_TYPE_MBR, &p_buf, p_err) {
    if ((p_partition_info->StartSec == 0u)
        && (p_partition_info->SecCnt == 0u)
        && (p_partition_info->Type == 0u)) {
      //                                                           Determine if valid FAT boot sec.
      //                                                           Compare string at offset 0x03.
      comp_val = Str_Cmp_N((CPU_CHAR *)&p_buf[FS_FAT_BOOT_SEC_FMT_OEM_NAME_OFFSET],
                           (CPU_CHAR *)"MSWIN4.1",              // See Note #1
                           8u);

      if (comp_val == 0u) {
        //                                                         Compare string at offset 0x36.
        comp_val = Str_Cmp_N((CPU_CHAR *)&p_buf[FS_FAT_BOOT_SEC_FMT_FAT12_16_SYS_STR_OFFSET],
                             (CPU_CHAR *)"FAT",                 // Format can be FAT12, FAT16.
                             3u);
        if (comp_val == 0u) {
          //                                                       One partition on entire volume.
          p_partition_info->SecCnt = FSBlkDev_LbCntGet(blk_dev_handle,
                                                       p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            return (DEF_FALSE);
          }

          if (p_partition_info->SecCnt != 0u) {
            RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
            boot_chk = DEF_TRUE;
          }
        } else {
          //                                                       Compare string at offset 0x52.
          comp_val = Str_Cmp_N((CPU_CHAR *)&p_buf[FS_FAT_BOOT_SEC_FMT_FAT32_SYS_STR_OFFSET],
                               (CPU_CHAR *)"FAT32",             // Format can be FAT32.
                               5u);
          if (comp_val == 0u) {
            //                                                     One partition on entire volume.
            p_partition_info->SecCnt = FSBlkDev_LbCntGet(blk_dev_handle,
                                                         p_err);
            if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
              return (DEF_FALSE);
            }

            if (p_partition_info->SecCnt != 0u) {
              RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
              boot_chk = DEF_TRUE;
            }
          }
        }
      }
    }
  }

  return (boot_chk);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           FS_FAT_ClusFreeFind()
 *
 * @brief    Find free cluster.
 *
 * @param    p_fat_vol   Pointer to FAT volume object.
 *
 * @param    p_err       Error pointer.
 *
 * @return   Cluster number, if free cluster found.
 *           0,              otherwise.
 *
 * @note     (1) In order for journaling to behave as expected, FAT entry updates must be atomic.
 *               To ensure this is the case when using FAT12, cross-boundary FAT entries must be
 *               avoided.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static FS_FAT_CLUS_NBR FS_FAT_ClusFreeFind(FS_FAT_VOL *p_fat_vol,
                                           RTOS_ERR   *p_err)
{
  FS_FAT_TYPE_API *p_fat_api;
  FS_FAT_CLUS_NBR fat_entry;
  FS_FAT_CLUS_NBR rtn_clus;
  FS_FAT_CLUS_NBR next_clus;
  FS_FAT_CLUS_NBR clus_cnt_chkd;
  CPU_BOOLEAN     clus_ignore;

  WITH_SCOPE_BEGIN(p_err) {
    p_fat_api = FS_FAT_TYPE_API_GET(p_fat_vol);
    next_clus = p_fat_vol->NextClusNbr;
    rtn_clus = 0u;
    clus_cnt_chkd = 0u;

    //                                                             ----------------- FREE CLUS LOOKUP -----------------
    BREAK_ON_ERR(while) (clus_cnt_chkd < p_fat_vol->ClusCnt) {
      //                                                           Wrap clus nbr.
      if (next_clus >= FS_FAT_MIN_CLUS_NBR + p_fat_vol->ClusCnt) {
        next_clus = FS_FAT_MIN_CLUS_NBR;
      }

      //                                                           Rd next FAT entry.
      BREAK_ON_ERR(fat_entry = p_fat_api->ClusValRd(p_fat_vol,
                                                    next_clus,
                                                    p_err));

      //                                                           ----------------- FREE CLUS FOUND ------------------
      if (fat_entry == p_fat_api->ClusFree) {                   // Chk if free clus found.
        clus_ignore = DEF_NO;                                   // Clus not ignore'd by dflt.
#if ((FS_FAT_CFG_FAT12_EN == DEF_ENABLED) && (FS_FAT_CFG_JOURNAL_EN == DEF_ENABLED))
        {
          FS_LB_SIZE fat_offset;
          FS_LB_SIZE fat_sec_offset;
          CPU_INT08U lb_size_log2;
          //                                                       If FAT12 and journal started ...
          //                                                       ... avoid sec boundary (see Note #1) ...
          if ((p_fat_vol->FAT_Type == FS_FAT_TYPE_FAT12) && p_fat_vol->IsJournaled) {
            BREAK_ON_ERR(lb_size_log2 = FSBlkDev_LbSizeLog2Get(p_fat_vol->Vol.BlkDevHandle, p_err));
            fat_offset = next_clus + next_clus / 2u;
            fat_sec_offset = fat_offset & (FS_UTIL_PWR2(lb_size_log2) - 1u);
            if (fat_sec_offset == FS_UTIL_PWR2(lb_size_log2) - 1u) {
              LOG_VRB(("Sector boundary cluster avoided: ", (u)next_clus));
              clus_ignore = DEF_YES;
            }
          }
        }
#endif
        if (!clus_ignore) {
          p_fat_vol->NextClusNbr = next_clus + 1u;              // ... else store next clus ...
          rtn_clus = next_clus;                                 // ... and rtn clus.
          break;
        }
      }

      next_clus++;
      clus_cnt_chkd++;
    }
  } WITH_SCOPE_END

  return (rtn_clus);
}
#endif

/****************************************************************************************************//**
 *                                               FS_FAT_SecClr()
 *
 * @brief    Clear contiguous sector(s) on a FAT volume.
 *
 * @param    p_fat_vol               Pointer to a FAT volume.
 *
 * @param    start_sec               Start sector.
 *
 * @param    cnt                     Number of sectors to clear.
 *
 * @param    p_data_wr_job_handle    Pointer to a job handle.
 *
 * @param    fat_lb_type             Logical block type.
 *                                   FS_FAT_LB_TYPE_DIRENT       Directory entry.
 *                                   FS_FAT_LB_TYPE_FAT          FAT.
 *                                   FS_FAT_LB_TYPE_DATA         Data.
 *                                   FS_FAT_LB_TYPE_JNL_DIRENT   Journal directory entry.
 *                                   FS_FAT_LB_TYPE_JNL_DATA     Journal data.
 *                                   FS_FAT_LB_TYPE_RSVD_AREA    Reserved area.
 *
 * @param    p_err                   Error pointer.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_FAT_DataSecClr(FS_FAT_VOL             *p_fat_vol,
                              FS_FAT_SEC_NBR         start_sec,
                              FS_FAT_SEC_NBR         cnt,
                              FS_CACHE_WR_JOB_HANDLE *p_data_wr_job_handle,
                              CPU_INT08U             fat_lb_type,
                              RTOS_ERR               *p_err)
{
  CPU_INT08U     *p_buf;
  FS_FAT_SEC_NBR cnt_rem;
  FS_FAT_SEC_NBR cur_sec;
  CPU_INT08U     lb_size_log2;
#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
  FS_CACHE_WR_JOB_HANDLE sec_clr_wr_job = FS_CACHE_WR_JOB_HANDLE_INIT;
  FS_CACHE_WR_JOB_HANDLE stub_job_handle = FS_CACHE_WR_JOB_HANDLE_INIT;
#endif

#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_DISABLED)
  PP_UNUSED_PARAM(p_data_wr_job_handle);
#endif

  WITH_SCOPE_BEGIN(p_err) {
    BREAK_ON_ERR(lb_size_log2 = FSBlkDev_LbSizeLog2Get(p_fat_vol->Vol.BlkDevHandle, p_err));

#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
    sec_clr_wr_job = (p_data_wr_job_handle != DEF_NULL)
                     ? *p_data_wr_job_handle : FSCache_VoidWrJobHandle;
#endif

    cnt_rem = cnt;
    cur_sec = start_sec;
    BREAK_ON_ERR(while) (cnt_rem > 0u) {
      BREAK_ON_ERR(FS_VOL_CACHE_BLK_WR)(&p_fat_vol->Vol,
                                        cur_sec,
                                        fat_lb_type,
                                        sec_clr_wr_job,
                                        &p_buf,
                                        &sec_clr_wr_job,
                                        p_err) {
        Mem_Clr((void *)p_buf, FS_UTIL_PWR2(lb_size_log2));
      }

#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
      FS_CACHE *p_cache = p_fat_vol->Vol.CacheBlkDevDataPtr->CachePtr;
      BREAK_ON_ERR(stub_job_handle = FSCache_WrJobJoin(p_cache,
                                                       sec_clr_wr_job,
                                                       stub_job_handle,
                                                       p_err));
#endif

      cnt_rem--;
      cur_sec++;
    }

#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
    if (p_data_wr_job_handle != DEF_NULL) {
      *p_data_wr_job_handle = stub_job_handle;
    }
#endif
  } WITH_SCOPE_END
}
#endif

#endif // FS_CORE_CFG_FAT_EN

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_FS_AVAIL
