/***************************************************************************//**
 * @file
 * @brief File System - Fat Entry Operations
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

#include  <common/include/rtos_path.h>
#include  <fs_core_cfg.h>

#if (FS_CORE_CFG_FAT_EN == DEF_ENABLED)

//                                                                 ----------------------- FAT ------------------------
#include  <fs/include/fs_fat.h>
#include  <fs/source/sys/fat/fs_fat_journal_priv.h>
#include  <fs/source/sys/fat/fs_fat_xfn_priv.h>
#include  <fs/source/sys/fat/fs_fat_file_priv.h>
#include  <fs/source/sys/fat/fs_fat_dir_priv.h>
#include  <fs/source/sys/fat/fs_fat_entry_priv.h>
#include  <fs/source/sys/fat/fs_fat_priv.h>
#include  <fs/source/sys/fat/fs_fat_journal_priv.h>
#include  <fs/source/shared/cleanup/cleanup_mgmt_priv.h>

//                                                                 ----------------------- CORE -----------------------
#include  <fs/source/core/fs_core_priv.h>
#include  <fs/source/core/fs_core_op_priv.h>
#include  <fs/source/core/fs_core_entry_priv.h>
#include  <fs/source/core/fs_core_file_priv.h>
#include  <fs/source/core/fs_core_vol_priv.h>
#include  <fs/source/sys/fs_sys_priv.h>
#include  <fs/source/core/fs_core_cache_priv.h>

//                                                                 ----------------------- EXT ------------------------
#include  <common/include/lib_mem.h>
#include  <common/include/lib_utils.h>
#include  <common/source/logging/logging_priv.h>

#include  "sl_sleeptimer.h"

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                    (FS, FAT, ENTRY)
#define  RTOS_MODULE_CUR                RTOS_CFG_MODULE_FS

#define  FS_FAT_LFN_OFF_CHKSUM          13u

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef  struct  fs_fat_sfn_dirent_find_data {
  void        *NamePtr;
  CPU_BOOLEAN EntryFound;
} FS_FAT_SFN_DIRENT_FIND_DATA;

typedef  struct  fs_fat_dte_acquire_cb_data {
  FS_FLAGS                 AcquireMode;
  CPU_BOOLEAN              HasFirstLFN;
  CPU_BOOLEAN              HasLastLFN;
  FS_FAT_DIR_ENTRY_QTY     DteRemCnt;
  CPU_INT08U               ChkSum;
  FS_FAT_ON_DTE_ACQUIRE_CB OnDteAcquireCb;
  void                     *OnDteAcquireCbData;
  FS_CACHE_WR_JOB_HANDLE   *EntryCacheWrJobHandlePtr;
} FS_FAT_DTE_ACQUIRE_CB_DATA;

typedef  struct  fs_fat_dte_erase_cb_data {
  FS_FAT_SEC_BYTE_POS    EndDtePos;
  FS_CACHE_WR_JOB_HANDLE *EntryCacheWrJobHandlePtr;
} FS_FAT_DTE_ERASE_CB_DATA;

typedef  struct  fs_fat_dirent_empty_get_data {
  FS_FAT_SEC_BYTE_POS FirstDirEntryPos;
  FS_FAT_SEC_BYTE_POS LastDirEntryPos;
  CPU_INT08U          DirEntryCnt;
  CPU_INT08U          FoundDirEntryCnt;
} FS_FAT_DIRENT_EMPTY_GET_CB_DATA;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBALS VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (FS_TEST_FAT_JNL_EN == DEF_ENABLED)
extern CPU_INT08U FS_Test_FAT_JournalTestNumber;
#endif

static const CPU_INT32U FS_FAT_NameDot[3] = { 0x2020202Eu,
                                              0x20202020u,
                                              0x00202020u };

static const CPU_INT32U FS_FAT_NameDotDot[3] = { 0x20202E2Eu,
                                                 0x20202020u,
                                                 0x00202020u };

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static FS_FILE_NAME_LEN FS_FAT_SFN_NameParse(void     *p_dir_entry,
                                             CPU_CHAR *p_name_buf);

static void FS_FAT_SFN_NameCreate(CPU_CHAR   *p_name,
                                  CPU_INT32U *p_name_8_3);

static void FS_FAT_SFN_DirEntryParse(CPU_INT08U        *p_dir_entry,
                                     FS_FAT_ENTRY_INFO *p_fat_entry_info);

static FS_FLAGS FS_FAT_SFN_NameChk(CPU_CHAR *p_name);

static FS_FAT_DIRENT_BROWSE_OUTCOME FS_FAT_EntryFindByFirstClusCb(void                *p_dte,
                                                                  FS_FAT_SEC_BYTE_POS *p_dte_pos,
                                                                  void                *p_dte_info,
                                                                  void                *p_arg);

static FS_FAT_DIRENT_BROWSE_OUTCOME FS_FAT_EntryParseCb(void                *p_dte,
                                                        FS_FAT_SEC_BYTE_POS *p_dte_pos,
                                                        void                *p_dte_info,
                                                        void                *p_arg);

static FS_FAT_DIRENT_BROWSE_OUTCOME FS_FAT_EntryFindByNameCb(void                *p_dte,
                                                             FS_FAT_SEC_BYTE_POS *p_dte_pos,
                                                             void                *p_dte_info,
                                                             void                *p_arg);

#if (FS_FAT_CFG_LFN_EN == DEF_ENABLED)
static FS_FAT_DIRENT_BROWSE_OUTCOME FS_FAT_EntryFindBySFNCb(void                *p_dte,
                                                            FS_FAT_SEC_BYTE_POS *p_dte_pos,
                                                            void                *p_dte_info,
                                                            void                *p_arg);
#endif

static void FS_FAT_DotDotEntryResolve(FS_FAT_VOL          *p_fat_vol,
                                      FS_FAT_SEC_BYTE_POS *p_entry_start_pos,
                                      RTOS_ERR            *p_err);

static FS_FAT_DIRENT_BROWSE_OUTCOME FS_FAT_DteAcquireCb(FS_FAT_VOL          *p_fat_vol,
                                                        FS_FAT_SEC_BYTE_POS *p_dte_pos,
                                                        void                *p_arg,
                                                        RTOS_ERR            *p_err);

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_FAT_SFN_DirEntryFmt(void            *p_dir_entry,
                                   CPU_INT32U      *p_name_8_3,
                                   CPU_BOOLEAN     is_dir,
                                   FS_FAT_CLUS_NBR file_first_clus,
                                   CPU_BOOLEAN     name_lower_case,
                                   CPU_BOOLEAN     ext_lower_case);

static FS_FAT_DIRENT_BROWSE_OUTCOME FS_FAT_DirEntriesEmptyFindCb(void                *p_dte,
                                                                 FS_FAT_SEC_BYTE_POS *p_dte_pos,
                                                                 void                *p_dte_info,
                                                                 void                *p_arg);

static FS_FAT_DIRENT_BROWSE_OUTCOME FS_FAT_DirEntriesEraseCb(FS_FAT_VOL          *p_fat_vol,
                                                             FS_FAT_SEC_BYTE_POS *p_cur_pos,
                                                             void                *p_arg,
                                                             RTOS_ERR            *p_err);

static void FS_FAT_EntryTimeWr(FS_FAT_VOL           *p_fat_vol,
                               FS_FAT_SEC_BYTE_POS  *p_dte_end_pos,
                               sl_sleeptimer_date_t *p_time,
                               CPU_INT08U           time_type,
                               RTOS_ERR             *p_err);

static void FS_FAT_DirEntriesEmptyGet(FS_FAT_VOL             *p_vol,
                                      FS_FAT_DIR_ENTRY_QTY   dir_entry_cnt,
                                      FS_FAT_SEC_NBR         first_sec_nbr,
                                      FS_FAT_SEC_BYTE_POS    *p_dir_start_pos,
                                      FS_FAT_SEC_BYTE_POS    *p_dir_end_pos,
                                      FS_CACHE_WR_JOB_HANDLE *p_entry_wr_job_handle,
                                      RTOS_ERR               *p_err);
#endif

static void FS_FAT_VolLabelFind(FS_FAT_VOL          *p_fat_vol,
                                FS_FAT_SEC_BYTE_POS *p_entry_pos,
                                RTOS_ERR            *p_err);

static FS_FAT_DIRENT_BROWSE_OUTCOME FS_FAT_VolLabelFindCb(void                *p_dirent,
                                                          FS_FAT_SEC_BYTE_POS *p_cur_pos,
                                                          void                *p_dte_info,
                                                          void                *p_arg);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           FS_FAT_EntryFirstLbGet()
 *
 * @brief    Get the number of the given entry's first logical block.
 *
 * @param    p_vol       Pointer to a volume.
 *
 * @param    entry_pos   Entry position.
 *
 * @param    p_err       Error pointer.
 *
 * @return   Number of the entry's first logical block.
 *******************************************************************************************************/
FS_LB_NBR FS_FAT_EntryFirstLbGet(FS_VOL     *p_vol,
                                 FS_SYS_POS entry_pos,
                                 RTOS_ERR   *p_err)
{
  FS_FAT_VOL          *p_fat_vol;
  FS_FAT_SEC_BYTE_POS de_start_pos;
  FS_FAT_SEC_BYTE_POS de_end_pos;
  FS_FAT_ENTRY_INFO   fat_entry_info;
  FS_FAT_CLUS_NBR     first_clus_nbr;
  FS_FAT_SEC_NBR      first_sec_nbr;

  WITH_SCOPE_BEGIN(p_err) {
    p_fat_vol = (FS_FAT_VOL *)p_vol;
    first_sec_nbr = 0u;

    FS_FAT_POS_DATA_DECODE(entry_pos, de_start_pos);
    BREAK_ON_ERR(FS_FAT_EntryParse(p_fat_vol,
                                   &de_start_pos,
                                   &de_end_pos,
                                   &fat_entry_info,
                                   DEF_NULL,
                                   DEF_NULL,
                                   0u,
                                   p_err));

    first_clus_nbr = fat_entry_info.FirstClusNbr;
    first_sec_nbr = FS_FAT_CLUS_TO_SEC(p_fat_vol, first_clus_nbr);
  } WITH_SCOPE_END

  return (first_sec_nbr);
}

/****************************************************************************************************//**
 *                                           FS_FAT_NextLbGet()
 *
 * @brief    Get next logical block number given a current logical block number.
 *
 * @param    p_vol       Pointer to a volume.
 *
 * @param    cur_lb_nbr  Current logical block number.
 *
 * @param    p_err       Error pointer.
 *
 * @return   Next logical block number
 *******************************************************************************************************/
FS_LB_NBR FS_FAT_NextLbGet(FS_VOL    *p_vol,
                           FS_LB_NBR cur_lb_nbr,
                           RTOS_ERR  *p_err)
{
  FS_FAT_VOL *p_fat_vol;
  FS_LB_NBR  next_lb_nbr;

  p_fat_vol = (FS_FAT_VOL *)p_vol;

  next_lb_nbr = FS_FAT_SecNextGet(p_fat_vol,
                                  cur_lb_nbr,
                                  p_err);

  return (next_lb_nbr);
}

/****************************************************************************************************//**
 *                                           FS_FAT_EntryLookup()
 *
 * @brief    Look for an entry starting at the given parent directory location.
 *
 * @param    p_vol           Pointer to a volume.
 *
 * @param    start_sys_pos   Encoded lookup start position.
 *
 * @param    p_entry_name    Pointer to the entry name.
 *
 * @param    p_err           Error pointer.
 *
 * @return   Found entry position.
 *******************************************************************************************************/
FS_SYS_POS FS_FAT_EntryLookup(FS_VOL     *p_vol,
                              FS_SYS_POS start_sys_pos,
                              CPU_CHAR   *p_entry_name,
                              RTOS_ERR   *p_err)
{
  FS_FAT_VOL          *p_fat_vol;
  FS_FAT_ENTRY_INFO   entry_info;
  FS_SYS_POS          entry_sys_pos;
  FS_FAT_SEC_BYTE_POS start_pos;
  FS_FAT_SEC_BYTE_POS de_start_pos;
  FS_FAT_SEC_BYTE_POS de_end_pos;
  FS_FAT_SEC_BYTE_POS dt_pos;
  FS_FAT_CLUS_NBR     first_clus_nbr;
  FS_FLAGS            attrib;
  CPU_BOOLEAN         entry_found;

  WITH_SCOPE_BEGIN(p_err) {
    p_fat_vol = (FS_FAT_VOL *)p_vol;
    entry_sys_pos = FS_FAT_VoidEntrySysPos;

    FS_FAT_POS_DATA_DECODE(start_sys_pos, start_pos);

    if ((start_pos.SecNbr == 0u)                                // Root's parent is itself.
        && (start_pos.SecOffset == 0u)
        && FSCore_PathSegIsDotDot(p_entry_name)) {
      FS_FAT_POS_DATA_ENCODE(FS_FAT_NullSecBytePos, entry_sys_pos);
      return (entry_sys_pos);
    }

    de_start_pos = start_pos;                                   // Find start of the directory table.
    BREAK_ON_ERR(FS_FAT_EntryParse(p_fat_vol,
                                   &de_start_pos,
                                   &de_end_pos,
                                   &entry_info,
                                   DEF_NULL,
                                   DEF_NULL,
                                   0u,
                                   p_err));

    first_clus_nbr = entry_info.FirstClusNbr;
    attrib = entry_info.Attrib;

    //                                                             The parent entry must be a directory.
    ASSERT_BREAK(DEF_BIT_IS_CLR(attrib, FS_FAT_DIRENT_ATTR_VOLUME_ID),
                 RTOS_ERR_ENTRY_PARENT_NOT_DIR);

    ASSERT_BREAK(DEF_BIT_IS_SET(attrib, FS_FAT_DIRENT_ATTR_DIRECTORY),
                 RTOS_ERR_ENTRY_PARENT_NOT_DIR);

    if (first_clus_nbr == 0u) {                                 // Get root dir tbl pos.
      dt_pos.SecNbr = p_fat_vol->RootDirStart;
      dt_pos.SecOffset = 0u;
    } else {                                                    // Get regular dir tbl pos.
      ASSERT_BREAK(FS_FAT_IS_VALID_CLUS(p_fat_vol, first_clus_nbr), RTOS_ERR_VOL_CORRUPTED);
      dt_pos.SecNbr = FS_FAT_CLUS_TO_SEC(p_fat_vol, first_clus_nbr);
      dt_pos.SecOffset = 0u;
    }

    //                                                             Find entry.
    de_start_pos = dt_pos;
    BREAK_ON_ERR(entry_found = FS_FAT_EntryFind(p_fat_vol,
                                                p_entry_name,
                                                DEF_NULL,
                                                &de_start_pos,
                                                &de_end_pos,
                                                p_err));

    //                                                             Find the entry corresponding to the dot dot entry.
    if (entry_found && FSCore_PathSegIsDotDot(p_entry_name)) {
      BREAK_ON_ERR(FS_FAT_DotDotEntryResolve(p_fat_vol,
                                             &de_start_pos,
                                             p_err));
    }

    if (entry_found) {                                          // Set return val.
      FS_FAT_POS_DATA_ENCODE(de_start_pos, entry_sys_pos);
    } else {
      FS_FAT_POS_DATA_ENCODE(FS_FAT_VoidSecBytePos, entry_sys_pos);
    }
  } WITH_SCOPE_END

  return (entry_sys_pos);
}

/****************************************************************************************************//**
 *                                           FS_FAT_EntryCreate()
 *
 * @brief    Create a new entry on the disk.
 *
 * @param    p_vol           Pointer to a core volume object.
 *
 * @param    parent_sys_pos  Encoded position of the created entry's parent.
 *
 * @param    p_entry_name    Pointer to the entry name.
 *
 * @param    is_dir          Indicates whether the new entry shall be a directory or a file.
 *
 * @param    p_err           Error pointer.
 *
 * @return   Created entry's encoded position.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
FS_SYS_POS FS_FAT_EntryCreate(FS_VOL      *p_vol,
                              FS_SYS_POS  parent_sys_pos,
                              CPU_CHAR    *p_entry_name,
                              CPU_BOOLEAN is_dir,
                              RTOS_ERR    *p_err)
{
  FS_FAT_VOL             *p_fat_vol;
  FS_FAT_SEC_BYTE_POS    de_start_pos;
  FS_FAT_SEC_BYTE_POS    de_end_pos;
  FS_FAT_SEC_BYTE_POS    parent_entry_pos;
  FS_FAT_SEC_NBR         parent_dt_first_sec_nbr;
  FS_SYS_POS             entry_sys_pos;
  FS_CACHE_WR_JOB_HANDLE wr_job_handle = FS_CACHE_WR_JOB_HANDLE_INIT;

  WITH_SCOPE_BEGIN(p_err) {
    p_fat_vol = (FS_FAT_VOL *)p_vol;
    entry_sys_pos = FS_FAT_VoidEntrySysPos;

#if (FS_FAT_CFG_JOURNAL_EN == DEF_ENABLED)
    {
      CPU_SIZE_T log_size;

      log_size = is_dir ? FS_FAT_JOURNAL_DIR_CREATE_LOG_MAX_SIZE
                 : FS_FAT_JOURNAL_FILE_CREATE_LOG_MAX_SIZE;

      BREAK_ON_ERR(FS_FAT_JournalEnterTopLvlOp(p_fat_vol,
                                               log_size,
                                               p_err));
    }
#endif

    FS_FAT_POS_DATA_DECODE(parent_sys_pos, parent_entry_pos);
    BREAK_ON_ERR(parent_dt_first_sec_nbr = FS_FAT_EntryDirTblGet(p_fat_vol,
                                                                 &parent_entry_pos,
                                                                 p_err));

    BREAK_ON_ERR(FS_FAT_EntryCreateInternal(p_fat_vol,
                                            &de_start_pos,
                                            &de_end_pos,
                                            p_entry_name,
                                            parent_dt_first_sec_nbr,
                                            is_dir,
                                            &wr_job_handle,
                                            p_err));

#if (FS_TEST_FAT_JNL_EN == DEF_ENABLED)
    if ((FS_Test_FAT_JournalTestNumber == FSFILE_OPEN)
        || (FS_Test_FAT_JournalTestNumber == FSDIR_OPEN)) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    }
#endif

    //                                                             -------------------- CACHE SYNC --------------------
#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
    BREAK_ON_ERR(FS_CACHE_LOCK_WITH) (p_fat_vol->Vol.CacheBlkDevDataPtr->CachePtr) {
      BREAK_ON_ERR(FSCache_WrJobExec(p_fat_vol->Vol.CacheBlkDevDataPtr->CachePtr,
                                     wr_job_handle,
                                     DEF_NO,
                                     p_err));
    }
#endif

    FS_FAT_POS_DATA_ENCODE(de_start_pos, entry_sys_pos);
  } WITH_SCOPE_END

  return (entry_sys_pos);
}
#endif

/****************************************************************************************************//**
 *                                           FS_FAT_EntryAttribSet()
 *
 * @brief    Set a file or directory's attributes.
 *
 * @param    p_vol           Pointer to a core volume object.
 *
 * @param    entry_sys_pos   Encoded entry position.
 *
 * @param    attrib          Entry attributes to set (see Note #1).
 *
 * @param    p_err           Error pointer.
 *
 * @note     (1) FAT does NOT support write-only file attributes. Consequently, it is NOT possible to
 *               reset a file's read attribute.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FS_FAT_EntryAttribSet(FS_VOL     *p_vol,
                           FS_SYS_POS entry_sys_pos,
                           FS_FLAGS   attrib,
                           RTOS_ERR   *p_err)
{
  FS_FAT_VOL          *p_fat_vol;
  CPU_INT08U          *p_end_dte;
  CPU_INT08U          *p_buf;
  FS_FLAGS            de_attrib;
  FS_FAT_SEC_BYTE_POS de_start_pos;
  FS_FAT_SEC_BYTE_POS de_end_pos;
#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
  FS_CACHE_WR_JOB_HANDLE wr_job_handle = FS_CACHE_WR_JOB_HANDLE_INIT;
#endif

  WITH_SCOPE_BEGIN(p_err) {
    //                                                             See Note #1.
    RTOS_ASSERT_DBG_ERR_SET(DEF_BIT_IS_SET(attrib, FS_ENTRY_ATTRIB_RD), *p_err, RTOS_ERR_INVALID_ARG,; );

    p_fat_vol = (FS_FAT_VOL *)p_vol;

#if (FS_FAT_CFG_JOURNAL_EN == DEF_ENABLED)
    BREAK_ON_ERR(FS_FAT_JournalEnterTopLvlOp(p_fat_vol,
                                             FS_FAT_JOURNAL_ENTRY_DELETE_LOG_MAX_SIZE,
                                             p_err));
#endif

    //                                                             ----------------- PARSE CUR ENTRY ------------------
    FS_FAT_POS_DATA_DECODE(entry_sys_pos, de_start_pos);
    BREAK_ON_ERR(FS_FAT_EntryParse(p_fat_vol,
                                   &de_start_pos,
                                   &de_end_pos,
                                   DEF_NULL,
                                   DEF_NULL,
                                   DEF_NULL,
                                   0u,
                                   p_err));

#if (FS_FAT_CFG_JOURNAL_EN == DEF_ENABLED)
    BREAK_ON_ERR(FS_FAT_JournalEnterEntryUpdate(p_fat_vol,
                                                de_end_pos,
                                                de_end_pos,
                                                p_err));
#endif

    //                                                             --------------------- WR ENTRY ---------------------
    BREAK_ON_ERR(FS_VOL_CACHE_BLK_RW)(&p_fat_vol->Vol,
                                      de_end_pos.SecNbr,
                                      FS_FAT_LB_TYPE_DIRENT,
                                      FSCache_VoidWrJobHandle,
                                      &p_buf,
                                      &wr_job_handle,
                                      p_err) {
      p_end_dte = p_buf + de_end_pos.SecOffset;

      //                                                           Edit attrib.
      de_attrib = (FS_FLAGS)FS_FAT_DIRENT_ATTRIB_GET(p_end_dte);
      if (DEF_BIT_IS_SET(attrib, FS_ENTRY_ATTRIB_HIDDEN)) {
        DEF_BIT_SET(de_attrib, FS_FAT_DIRENT_ATTR_HIDDEN);
      } else {
        DEF_BIT_CLR(de_attrib, FS_FAT_DIRENT_ATTR_HIDDEN);
      }
      if (DEF_BIT_IS_SET(attrib, FS_ENTRY_ATTRIB_WR)) {
        DEF_BIT_CLR(de_attrib, FS_FAT_DIRENT_ATTR_READ_ONLY);
      } else {
        DEF_BIT_SET(de_attrib, FS_FAT_DIRENT_ATTR_READ_ONLY);
      }

      FS_FAT_DIRENT_ATTRIB_SET(p_end_dte, de_attrib);
    }

    //                                                             -------------------- CACHE SYNC --------------------
#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
    BREAK_ON_ERR(FS_CACHE_LOCK_WITH) (p_fat_vol->Vol.CacheBlkDevDataPtr->CachePtr) {
      BREAK_ON_ERR(FSCache_WrJobExec(p_fat_vol->Vol.CacheBlkDevDataPtr->CachePtr,
                                     wr_job_handle,
                                     DEF_NO,
                                     p_err));
    }
#endif

#if (FS_TEST_FAT_JNL_EN == DEF_ENABLED)
    if (FS_Test_FAT_JournalTestNumber == FSENTRY_ATTRIBSET1) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    }
#endif
  } WITH_SCOPE_END
}
#endif

/****************************************************************************************************//**
 *                                               FS_FAT_EntryDel()
 *
 * @brief    Delete a file or directory.
 *
 * @param    p_vol           Pointer to core volume object.
 *
 * @param    entry_sys_pos   Encoded entry position.
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FS_FAT_EntryDel(FS_VOL     *p_vol,
                     FS_SYS_POS entry_sys_pos,
                     RTOS_ERR   *p_err)
{
  FS_FAT_VOL             *p_fat_vol;
  FS_FAT_SEC_BYTE_POS    de_start_pos;
  FS_CACHE_WR_JOB_HANDLE entry_wr_job_handle = FS_CACHE_WR_JOB_HANDLE_INIT;

  WITH_SCOPE_BEGIN(p_err) {
    p_fat_vol = (FS_FAT_VOL *)p_vol;

#if (FS_FAT_CFG_JOURNAL_EN == DEF_ENABLED)
    BREAK_ON_ERR(FS_FAT_JournalEnterTopLvlOp(p_fat_vol,
                                             FS_FAT_JOURNAL_ENTRY_DELETE_LOG_MAX_SIZE,
                                             p_err));
#endif

    FS_FAT_POS_DATA_DECODE(entry_sys_pos, de_start_pos);

    BREAK_ON_ERR(FS_FAT_EntryDelInternal(p_fat_vol,
                                         &de_start_pos,
                                         &entry_wr_job_handle,
                                         p_err));
  } WITH_SCOPE_END
}
#endif

/****************************************************************************************************//**
 *                                           FS_FAT_EntryQuery()
 *
 * @brief    Get information about a file or directory.
 *
 * @param    p_vol           Pointer to core volume object.
 *
 * @param    entry_sys_pos   Encoded entry position.
 *
 * @param    p_entry_info    Pointer to structure that will receive the entry information.
 *
 * @param    p_name          Pointer to a buffer receiving the entry name.
 *
 * @param    p_name_len      Pointer to a variable receiving the entry name length.
 *
 * @param    name_buf_size   Size of buffer allocated for entry name.
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/
void FS_FAT_EntryQuery(FS_VOL           *p_vol,
                       FS_SYS_POS       entry_sys_pos,
                       FS_ENTRY_INFO    *p_entry_info,
                       CPU_CHAR         *p_name,
                       FS_FILE_NAME_LEN *p_name_len,
                       CPU_SIZE_T       name_buf_size,
                       RTOS_ERR         *p_err)
{
  FS_FAT_VOL          *p_fat_vol;
  FS_FAT_SEC_BYTE_POS de_start_pos;
  FS_FAT_SEC_BYTE_POS de_end_pos;
  FS_FAT_ENTRY_INFO   fat_entry_info;
  CPU_INT08U          lb_size_log2;

  WITH_SCOPE_BEGIN(p_err) {
    p_fat_vol = (FS_FAT_VOL *)p_vol;

    BREAK_ON_ERR(lb_size_log2 = FSBlkDev_LbSizeLog2Get(p_vol->BlkDevHandle, p_err));

    FS_FAT_POS_DATA_DECODE(entry_sys_pos, de_start_pos);

    if ((de_start_pos.SecNbr == 0u)                             // Handle root dir.
        && (de_start_pos.SecOffset == 0u)) {
      if (p_entry_info != DEF_NULL) {
        p_entry_info->Attrib.Wr = DEF_NO;
        p_entry_info->Attrib.Rd = DEF_NO;
        p_entry_info->Attrib.Hidden = DEF_NO;
        p_entry_info->Attrib.IsRootDir = DEF_YES;
        p_entry_info->Attrib.IsDir = DEF_YES;
        p_entry_info->DateAccess = 0u;
        p_entry_info->DateTimeCreate = 0u;
        p_entry_info->DateTimeWr = 0u;
        p_entry_info->Size = 0u;
        p_entry_info->BlkCnt = 0u;
        p_entry_info->BlkSize = FS_UTIL_PWR2(lb_size_log2);
        p_entry_info->DevId = (FS_ID)-1;
        p_entry_info->NodeId = (FS_ID)-1;
      }

      if (p_name != DEF_NULL) {
        p_name[0] = '\0';
      }

      if (p_name_len != DEF_NULL) {
        *p_name_len = 0u;
      }
    } else {                                                    // Handle regular entry.
      BREAK_ON_ERR(FS_FAT_EntryParse(p_fat_vol,
                                     &de_start_pos,
                                     &de_end_pos,
                                     &fat_entry_info,
                                     p_name,
                                     p_name_len,
                                     name_buf_size,
                                     p_err));

      if (p_entry_info != DEF_NULL) {
        p_entry_info->Attrib.IsRootDir = DEF_NO;
        p_entry_info->BlkSize = FS_UTIL_PWR2(lb_size_log2);
        p_entry_info->BlkCnt = FS_UTIL_DIV_PWR2(fat_entry_info.Size, 9u)
                               + FS_UTIL_MODULO_PWR2(fat_entry_info.Size, 9u) == 0u ? 0u : 1u;
        p_entry_info->DevId = (FS_ID)-1;
        p_entry_info->NodeId = (FS_ID)-1;

        FS_FAT_EntryInfoGet(p_entry_info, &fat_entry_info);
      }
    }
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                           FS_FAT_EntryRename()
 *
 * @brief    Rename a file or directory.
 *
 * @param    p_vol                   Pointer to core volume object.
 *
 * @param    old_entry_sys_pos       Encoded old entry's position.
 *
 * @param    new_entry_sys_pos       Encoded new entry's position.
 *
 * @param    parent_entry_sys_pos    Encoded parent directory's entry position.
 *
 * @param    p_new_name              Pointer to target entry's new name.
 *
 * @param    exist                   Indicates whether creation of new entry should be exclusive.
 *
 * @param    p_err                   Error pointer.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FS_FAT_EntryRename(FS_VOL      *p_vol,
                        FS_SYS_POS  old_entry_sys_pos,
                        FS_SYS_POS  new_entry_sys_pos,
                        FS_SYS_POS  new_entry_parent_sys_pos,
                        CPU_CHAR    *p_new_name,
                        CPU_BOOLEAN exist,
                        RTOS_ERR    *p_err)
{
  FS_FAT_VOL             *p_fat_vol;
  FS_FAT_SEC_BYTE_POS    old_de_start_pos;
  FS_FAT_SEC_BYTE_POS    old_de_end_pos;
  FS_FAT_SEC_BYTE_POS    new_de_start_pos;
  FS_FAT_SEC_BYTE_POS    new_de_end_pos;
  FS_FAT_SEC_BYTE_POS    parent_de_start_pos;
  FS_FAT_SEC_NBR         parent_dt_first_sec_nbr;
  FS_FAT_CLUS_NBR        target_entry_first_clus;
  FS_FAT_ENTRY_INFO      target_entry_info;
  FS_FAT_ENTRY_INFO      entry_info;
  FS_CACHE_WR_JOB_HANDLE entry_wr_job_handle = FS_CACHE_WR_JOB_HANDLE_INIT;

  WITH_SCOPE_BEGIN(p_err) {
    p_fat_vol = (FS_FAT_VOL *)p_vol;

#if (FS_FAT_CFG_JOURNAL_EN == DEF_ENABLED)
    BREAK_ON_ERR(FS_FAT_JournalEnterTopLvlOp(p_fat_vol,
                                             FS_FAT_JOURNAL_ENTRY_RENAME_LOG_MAX_SIZE,
                                             p_err));
#endif

    FS_FAT_POS_DATA_DECODE(old_entry_sys_pos, old_de_start_pos);
    FS_FAT_POS_DATA_DECODE(new_entry_sys_pos, new_de_start_pos);
    FS_FAT_POS_DATA_DECODE(new_entry_parent_sys_pos, parent_de_start_pos);

    //                                                             Get 1st sector of parent dir table.
    BREAK_ON_ERR(parent_dt_first_sec_nbr = FS_FAT_EntryDirTblGet(p_fat_vol,
                                                                 &parent_de_start_pos,
                                                                 p_err));

    //                                                             ------------ REM TARGET ENTRY IF NEEDED ------------
    if (exist) {
      //                                                           Get info about existing target entry.
      BREAK_ON_ERR(FS_FAT_EntryParse(p_fat_vol,
                                     &new_de_start_pos,
                                     &new_de_end_pos,
                                     &target_entry_info,
                                     DEF_NULL,
                                     DEF_NULL,
                                     0u,
                                     p_err));
      //                                                           Remove target entry from dir table.
      BREAK_ON_ERR(FS_FAT_DirEntriesErase(p_fat_vol,
                                          &new_de_start_pos,
                                          &new_de_end_pos,
                                          &entry_wr_job_handle,
                                          p_err));
    }

    //                                                             ----------------- CREATE NEW ENTRY -----------------
    //                                                             Create new entry (renamed file) in dir table.
    BREAK_ON_ERR(FS_FAT_EntryCreateInternal(p_fat_vol,
                                            &new_de_start_pos,
                                            &new_de_end_pos,
                                            p_new_name,
                                            parent_dt_first_sec_nbr,
                                            DEF_NO,
                                            &entry_wr_job_handle,
                                            p_err));

#if (FS_TEST_FAT_JNL_EN == DEF_ENABLED)
    if (FS_Test_FAT_JournalTestNumber == FSENTRY_RENAME_FILE1) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
      return;
    }
#endif

    //                                                             ------------------ REM OLD ENTRY -------------------
    //                                                             Get info about old entry (src file to rename).
    BREAK_ON_ERR(FS_FAT_EntryParse(p_fat_vol,
                                   &old_de_start_pos,
                                   &old_de_end_pos,
                                   &entry_info,
                                   DEF_NULL,
                                   DEF_NULL,
                                   0u,
                                   p_err));
    //                                                             Remove old entry from dir table.
    BREAK_ON_ERR(FS_FAT_DirEntriesErase(p_fat_vol,
                                        &old_de_start_pos,
                                        &old_de_end_pos,
                                        &entry_wr_job_handle,
                                        p_err));

#if (FS_TEST_FAT_JNL_EN == DEF_ENABLED)
    if (FS_Test_FAT_JournalTestNumber == FSENTRY_RENAME_FILE2) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
      return;
    }
#endif

    //                                                             ----------------- UPDATE NEW ENTRY -----------------
    //                                                             Update target entry attributes.
    BREAK_ON_ERR(FS_FAT_EntryUpdate(p_fat_vol,
                                    &new_de_end_pos,
                                    &entry_info,                // Attribrutes from old entry.
                                    DEF_NO,
                                    FS_FAT_DTE_FIELD_ALL,
                                    &entry_wr_job_handle,
                                    p_err));

#if (FS_TEST_FAT_JNL_EN == DEF_ENABLED)
    if (FS_Test_FAT_JournalTestNumber == FSENTRY_RENAME_FILE3) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
      return;
    }
#endif

    //                                                             --------- DEL TARGET CLUS CHAIN IF NEEDED ----------
    //                                                             Clus chain del must be last operation (See Note #?)
    if (exist) {
      target_entry_first_clus = target_entry_info.FirstClusNbr;

      if (FS_FAT_IS_VALID_CLUS(p_fat_vol, target_entry_first_clus)) {
        BREAK_ON_ERR((void)FS_FAT_ClusChainDel(p_fat_vol,
                                               target_entry_first_clus,
                                               DEF_YES,
                                               p_err));
      }
    }
  } WITH_SCOPE_END
}
#endif

/****************************************************************************************************//**
 *                                           FS_FAT_EntryTimeSet()
 *
 * @brief    Set a file or directory's date/time.
 *
 * @param    p_vol           Pointer to a core volume object.
 *
 * @param    entry_sys_pos   Encoded entry position.
 *
 * @param    p_time          Pointer to date/time.
 *
 * @param    time_type       Flag to indicate which date/time should be set:
 *                           FS_DATE_TIME_CREATE
 *                           FS_DATE_TIME_MODIFY
 *                           FS_DATE_TIME_ACCESS
 *                           FS_DATE_TIME_ALL
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FS_FAT_EntryTimeSet(FS_VOL               *p_vol,
                         FS_SYS_POS           entry_sys_pos,
                         sl_sleeptimer_date_t *p_time,
                         CPU_INT08U           time_type,
                         RTOS_ERR             *p_err)
{
  FS_FAT_VOL          *p_fat_vol;
  FS_FAT_SEC_BYTE_POS de_start_pos;
  FS_FAT_SEC_BYTE_POS de_end_pos;

  WITH_SCOPE_BEGIN(p_err) {
    p_fat_vol = (FS_FAT_VOL *)p_vol;

#if (FS_FAT_CFG_JOURNAL_EN == DEF_ENABLED)
    BREAK_ON_ERR(FS_FAT_JournalEnterTopLvlOp(p_fat_vol,
                                             FS_FAT_JOURNAL_ENTRY_DELETE_LOG_MAX_SIZE,
                                             p_err));
#endif

    FS_FAT_POS_DATA_DECODE(entry_sys_pos, de_start_pos);

    BREAK_ON_ERR(FS_FAT_EntryParse(p_fat_vol,
                                   &de_start_pos,
                                   &de_end_pos,
                                   DEF_NULL,
                                   DEF_NULL,
                                   DEF_NULL,
                                   0u,
                                   p_err));

#if (FS_FAT_CFG_JOURNAL_EN == DEF_ENABLED)
    BREAK_ON_ERR(FS_FAT_JournalEnterEntryUpdate(p_fat_vol,
                                                de_end_pos,
                                                de_end_pos,
                                                p_err));
#endif

    BREAK_ON_ERR(FS_FAT_EntryTimeWr(p_fat_vol,
                                    &de_end_pos,
                                    p_time,
                                    time_type,
                                    p_err));

#if (FS_TEST_FAT_JNL_EN == DEF_ENABLED)
    if (FS_Test_FAT_JournalTestNumber == FSENTRY_TIMESET1) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    }
#endif
  } WITH_SCOPE_END
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           FS_FAT_EntryDirTblGet()
 *
 * @brief    Get directory table associated with a given entry.
 *
 * @param    p_fat_vol       Pointer to a FAT volume object.
 *
 * @param    p_de_start_pos  Pointer to the target entry's position.
 *
 * @param    p_err           Error pointer.
 *
 * @return   Directory table's first sector.
 *******************************************************************************************************/
FS_FAT_SEC_NBR FS_FAT_EntryDirTblGet(FS_FAT_VOL          *p_fat_vol,
                                     FS_FAT_SEC_BYTE_POS *p_de_start_pos,
                                     RTOS_ERR            *p_err)
{
  FS_FAT_SEC_NBR      dt_sec_nbr;
  FS_FAT_SEC_BYTE_POS de_end_pos;
  FS_FAT_ENTRY_INFO   fat_entry_info;

  WITH_SCOPE_BEGIN(p_err) {
    dt_sec_nbr = 0u;
    if ((p_de_start_pos->SecNbr == 0u)
        && (p_de_start_pos->SecOffset == 0u)) {
      dt_sec_nbr = p_fat_vol->RootDirStart;
    } else {
      BREAK_ON_ERR(FS_FAT_EntryParse(p_fat_vol,
                                     p_de_start_pos,
                                     &de_end_pos,
                                     &fat_entry_info,
                                     DEF_NULL,
                                     DEF_NULL,
                                     0u,
                                     p_err));

      dt_sec_nbr = FS_FAT_CLUS_TO_SEC(p_fat_vol, fat_entry_info.FirstClusNbr);
    }
  } WITH_SCOPE_END

  return (dt_sec_nbr);
}

/****************************************************************************************************//**
 *                                           FS_FAT_SFN_EntryFind()
 *
 * @brief    Search directory for SFN directory entry.
 *
 * @param    p_fat_vol   Pointer to a FAT volume object.
 *
 * @param    p_name_8_3  8.3 entry's name.
 *
 * @param    p_pos       Pointer to directory position at which search should start; variable
 *                       that will receive the directory position at which the entry is located.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/

#if (FS_FAT_CFG_LFN_EN == DEF_ENABLED)
CPU_BOOLEAN FS_FAT_SFN_EntryFind(FS_FAT_VOL          *p_fat_vol,
                                 CPU_INT32U          *p_name_8_3,
                                 FS_FAT_SEC_BYTE_POS *p_pos,
                                 RTOS_ERR            *p_err)
{
  FS_FAT_ENTRY_FIND_BY_NAME_CB_DATA find_data;

  find_data.EndDirentPos = FS_FAT_VoidSecBytePos;
  find_data.StartDirentPos = FS_FAT_VoidSecBytePos;
  find_data.EntryFound = DEF_NO;
  find_data.Name83Ptr = p_name_8_3;
  find_data.HasNameMatch = DEF_NO;

  FS_FAT_DirTblBrowseAcquire(p_fat_vol,
                             p_pos,
                             FS_FAT_EntryFindBySFNCb,
                             &find_data,
                             FS_CACHE_BLK_GET_MODE_RD,
                             DEF_NULL,
                             p_err);

  return (find_data.HasNameMatch);
}
#endif

/****************************************************************************************************//**
 *                                       FS_FAT_DirTblBrowseAcquire()
 *
 * @brief    Browse a directory table, acquire a cache block and perform the given callback for each
 *           directory entry.
 *
 * @param    p_fat_vol               Pointer to a FAT volume.
 *
 * @param    p_dt_start_pos          Pointer to the start position inside the directory table.
 *
 * @param    on_dte_acquire_cb       Callback to be executed for each directory entry.
 *
 * @param    p_arg                   Callback argument.
 *
 * @param    acquire_mode            Cache block acquire mode (read, write or read-write).
 *
 * @param    p_entry_wr_job_handle   Pointer to a cache job handle.
 *
 * @param    p_err                   Error pointer.
 *******************************************************************************************************/
void FS_FAT_DirTblBrowseAcquire(FS_FAT_VOL               *p_fat_vol,
                                FS_FAT_SEC_BYTE_POS      *p_dt_start_pos,
                                FS_FAT_ON_DTE_ACQUIRE_CB on_dte_acquire_cb,
                                void                     *p_arg,
                                FS_FLAGS                 acquire_mode,
                                FS_CACHE_WR_JOB_HANDLE   *p_entry_wr_job_handle,
                                RTOS_ERR                 *p_err)
{
  FS_FAT_DTE_ACQUIRE_CB_DATA dte_acquire_data;

  dte_acquire_data.AcquireMode = acquire_mode;
  dte_acquire_data.ChkSum = 0u;
  dte_acquire_data.HasFirstLFN = DEF_NO;
  dte_acquire_data.HasLastLFN = DEF_NO;
  dte_acquire_data.DteRemCnt = 0u;
  dte_acquire_data.OnDteAcquireCb = on_dte_acquire_cb;
  dte_acquire_data.OnDteAcquireCbData = p_arg;
  dte_acquire_data.EntryCacheWrJobHandlePtr = p_entry_wr_job_handle;

  FS_FAT_DirTblBrowse(p_fat_vol,
                      p_dt_start_pos,
                      FS_FAT_DteAcquireCb,
                      (void *)&dte_acquire_data,
                      p_err);
}

/****************************************************************************************************//**
 *                                           FS_FAT_EntryInfoGet()
 *
 * @brief    Populate core entry info structure from FAT-specific entry info structure.
 *
 * @param    p_entry_info        Pointer to the destination core entry info structure.
 *
 * @param    p_fat_entry_info    Pointer to the source FAT-specific entry info structure.
 *******************************************************************************************************/
void FS_FAT_EntryInfoGet(FS_ENTRY_INFO     *p_entry_info,
                         FS_FAT_ENTRY_INFO *p_fat_entry_info)
{
  p_entry_info->Attrib.Rd = DEF_YES;
  p_entry_info->Attrib.Wr = DEF_BIT_IS_CLR(p_fat_entry_info->Attrib, FS_FAT_DIRENT_ATTR_READ_ONLY);
  p_entry_info->Attrib.IsDir = DEF_BIT_IS_SET(p_fat_entry_info->Attrib, FS_FAT_DIRENT_ATTR_DIRECTORY);
  p_entry_info->Attrib.Hidden = DEF_BIT_IS_SET(p_fat_entry_info->Attrib, FS_FAT_DIRENT_ATTR_HIDDEN);
  p_entry_info->Size = p_fat_entry_info->Size;

  FS_FAT_DateTimeParse(&p_entry_info->DateAccess,
                       p_fat_entry_info->DateAccess,
                       0u);

  FS_FAT_DateTimeParse(&p_entry_info->DateTimeWr,
                       p_fat_entry_info->DateWr,
                       p_fat_entry_info->TimeWr);

  FS_FAT_DateTimeParse(&p_entry_info->DateTimeCreate,
                       p_fat_entry_info->DateCreate,
                       p_fat_entry_info->TimeCreate);
}

/****************************************************************************************************//**
 *                                           FS_FAT_NextEntryParse()
 *
 * @brief    Parse next entry starting at given position.
 *
 * @param    p_fat_vol           Pointer to a FAT volume.
 *
 * @param    p_entry_start_pos   Pointer to a variable that contains the lookup start position and will
 *                               receive the next valid entry's start position.
 *
 * @param    p_entry_end_pos     Pointer to a variable that will receive the next valid entry's end
 *                               end position.
 *
 * @param    p_entry_info        Pointer to a FAT entry info structure (may be DEF_NULL).
 *
 * @param    p_name              Pointer to a buffer that will receive entry's name (may be DEF_NULL).
 *
 * @param    p_name_len          Pointer to a variable receiving the entry name length.
 *
 * @param    name_buf_size       Size of buffer allocated for entry name.
 *
 * @param    p_err               Error pointer.
 *******************************************************************************************************/
CPU_BOOLEAN FS_FAT_NextEntryParse(FS_FAT_VOL          *p_fat_vol,
                                  FS_FAT_SEC_BYTE_POS *p_entry_start_pos,
                                  FS_FAT_SEC_BYTE_POS *p_entry_end_pos,
                                  FS_FAT_ENTRY_INFO   *p_entry_info,
                                  CPU_CHAR            *p_name,
                                  FS_FILE_NAME_LEN    *p_name_len,
                                  CPU_SIZE_T          name_buf_size,
                                  RTOS_ERR            *p_err)
{
  FS_FAT_ENTRY_PARSE_CB_DATA parse_data;
  CPU_BOOLEAN                parsed;

  WITH_SCOPE_BEGIN(p_err) {
    //                                                             --------------------- ROOT DIR ---------------------
    parsed = DEF_NO;
    if ((p_entry_start_pos->SecNbr == 0u)
        && (p_entry_start_pos->SecOffset == 0u)) {
      if (p_entry_info != DEF_NULL) {
        p_entry_info->Attrib = FS_FAT_DIRENT_ATTR_NONE;
        p_entry_info->FirstClusNbr = 0u;
        DEF_BIT_SET(p_entry_info->Attrib, FS_FAT_DIRENT_ATTR_DIRECTORY);
      }

      p_entry_start_pos->SecNbr = 0u;
      p_entry_start_pos->SecOffset = 0u;
      p_entry_end_pos->SecNbr = 0u;
      p_entry_end_pos->SecOffset = 0u;

      parsed = DEF_YES;
    } else {                                                    // ------------------- REGULAR DIR --------------------
      parse_data.Done = DEF_NO;
      parse_data.Ovf = DEF_NO;
      parse_data.EndDirentPos = FS_FAT_VoidSecBytePos;
      parse_data.StartDirentPos = FS_FAT_VoidSecBytePos;
      parse_data.NamePtr = p_name;
      parse_data.NameBufSize = name_buf_size;
      parse_data.NameLen = 0u;
      parse_data.EntryInfoPtr = p_entry_info;

      if (p_name != DEF_NULL) {
        p_name[0] = '\0';
      }

      BREAK_ON_ERR(FS_FAT_DirTblBrowseAcquire(p_fat_vol,
                                              p_entry_start_pos,
                                              FS_FAT_EntryParseCb,
                                              &parse_data,
                                              FS_CACHE_BLK_GET_MODE_RD,
                                              DEF_NULL,
                                              p_err));

      ASSERT_BREAK(!parse_data.Ovf, RTOS_ERR_WOULD_OVF);        // Check if parsed entry name contains in provided buf.

      *p_entry_start_pos = parse_data.StartDirentPos;
      *p_entry_end_pos = parse_data.EndDirentPos;

      if (p_name_len != DEF_NULL) {
        *p_name_len = parse_data.NameLen;
      }

      parsed = parse_data.Done;
    }
  } WITH_SCOPE_END

  return (parsed);
}

/****************************************************************************************************//**
 *                                           FS_FAT_EntryUpdate()
 *
 * @brief    Update directory entry.
 *
 * @param    p_fat_vol           Pointer to a FAT volume.
 *
 * @param    p_entry_end_pos     Pointer to directory entry end position.
 *
 * @param    p_entry_info        Pointer to a FAT entry info structure.
 *
 * @param    get_date            Whether or not to get the date/time from the clock module.
 *
 * @param    fields              Fields to update. Any combination of the followings:
 *                                   - FS_FAT_DTE_FIELD_ATTR
 *                                   - FS_FAT_DTE_FIELD_SIZE
 *                                   - FS_FAT_DTE_FIELD_FIRST_CLUS
 *                                   - FS_FAT_DTE_FIELD_CREATION_TIME
 *                                   - FS_FAT_DTE_FIELD_WR_TIME
 *                                   - FS_FAT_DTE_FIELD_ACCESS_TIME
 *
 * @param    p_cur_entry_wr_job  Pointer to cache job handle.
 *
 * @param    p_err               Error pointer.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FS_FAT_EntryUpdate(FS_FAT_VOL             *p_fat_vol,
                        FS_FAT_SEC_BYTE_POS    *p_entry_end_pos,
                        FS_FAT_ENTRY_INFO      *p_entry_info,
                        CPU_BOOLEAN            get_date,
                        FS_FLAGS               fields,
                        FS_CACHE_WR_JOB_HANDLE *p_cur_entry_wr_job,
                        RTOS_ERR               *p_err)
{
  CPU_INT08U             *p_dir_entry;
  CPU_INT08U             *p_buf;
  sl_sleeptimer_date_t   stime;
  FS_FAT_DATE            date_val = 0u;
  FS_FAT_TIME            time_val = 0u;
  CPU_BOOLEAN            set_time;
  sl_status_t            status;
#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
  FS_CACHE_WR_JOB_HANDLE prev_wr_job_handle = FS_CACHE_WR_JOB_HANDLE_INIT;
#endif

#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_DISABLED)
  PP_UNUSED_PARAM(p_cur_entry_wr_job);
#endif

  WITH_SCOPE_BEGIN(p_err) {
    if (fields == FS_FAT_DTE_FIELD_NONE) {
      return;
    }

#if (FS_FAT_CFG_JOURNAL_EN == DEF_ENABLED)
    if (p_fat_vol->IsJournaled) {
      BREAK_ON_ERR(FS_FAT_JournalEnterEntryUpdate(p_fat_vol,
                                                  *p_entry_end_pos,
                                                  *p_entry_end_pos,
                                                  p_err));
#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
      prev_wr_job_handle = p_fat_vol->JournalData.JnlWrJobHandle;
#endif
    }
#else
#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
    prev_wr_job_handle = *p_cur_entry_wr_job;
#endif
#endif

    set_time = DEF_BIT_IS_SET_ANY(fields, FS_FAT_DTE_FIELD_WR_TIME
                                  | FS_FAT_DTE_FIELD_ACCESS_TIME
                                  | FS_FAT_DTE_FIELD_CREATION_TIME);
    if (set_time) {
      if (get_date == DEF_YES) {
        status = sl_sleeptimer_get_datetime(&stime);
        if (status == SL_STATUS_OK) {
          date_val = FS_FAT_DateFmt(&stime);
          time_val = FS_FAT_TimeFmt(&stime);
        } else {
          date_val = 0u;
          time_val = 0u;
        }
      } else {
        date_val = p_entry_info->DateWr;
        time_val = p_entry_info->TimeWr;
      }
    }

    BREAK_ON_ERR(FS_VOL_CACHE_BLK_RW) (&p_fat_vol->Vol,
                                       p_entry_end_pos->SecNbr,
                                       FS_FAT_LB_TYPE_DIRENT,
                                       prev_wr_job_handle,
                                       &p_buf,
                                       p_cur_entry_wr_job,
                                       p_err) {
      p_dir_entry = p_buf + p_entry_end_pos->SecOffset;

      if (DEF_BIT_IS_SET(fields, FS_FAT_DTE_FIELD_ATTR)) {
        FS_FAT_DIRENT_ATTRIB_SET(p_dir_entry, p_entry_info->Attrib);
      }
      if (DEF_BIT_IS_SET(fields, FS_FAT_DTE_FIELD_FIRST_CLUS)) {
        FS_FAT_DIRENT_FIRST_CLUS_SET(p_dir_entry, p_entry_info->FirstClusNbr);
      }
      if (DEF_BIT_IS_SET(fields, FS_FAT_DTE_FIELD_WR_TIME)) {
        FS_FAT_DIRENT_WR_TIME_DATE_SET(p_dir_entry, time_val, date_val);
      }
      if (DEF_BIT_IS_SET(fields, FS_FAT_DTE_FIELD_ACCESS_TIME)) {
        FS_FAT_DIRENT_ACCESS_DATE_SET(p_dir_entry, date_val);
      }
      if (DEF_BIT_IS_SET(fields, FS_FAT_DTE_FIELD_SIZE)) {
        FS_FAT_DIRENT_SIZE_SET(p_dir_entry, p_entry_info->Size);
      }
    }
  } WITH_SCOPE_END
}
#endif

/****************************************************************************************************//**
 *                                           FS_FAT_EntryParse()
 *
 * @brief    Convenience wrapper around FS_FAT_NextEntryParse() that returns an error if a valid entry
 *           is not located at the given start position.
 *
 * @param    p_fat_vol           Pointer to a FAT volume.
 *
 * @param    p_entry_start_pos   Pointer to directory entry's start position.
 *
 * @param    p_entry_end_pos     Pointer to a variable that will receive the entry end position.
 *
 * @param    p_entry_info        Pointer to a FAT entry info structure.
 *
 * @param    p_name              Pointer to a buffer that will receive entry's name (may be DEF_NULL).
 *
 * @param    p_name_len          Pointer to a variable receiving the entry name length.
 *
 * @param    name_buf_size       Size of buffer allocated for entry name.
 *
 * @param    p_err               Error pointer.
 *******************************************************************************************************/
void FS_FAT_EntryParse(FS_FAT_VOL          *p_fat_vol,
                       FS_FAT_SEC_BYTE_POS *p_entry_start_pos,
                       FS_FAT_SEC_BYTE_POS *p_entry_end_pos,
                       FS_FAT_ENTRY_INFO   *p_entry_info,
                       CPU_CHAR            *p_name,
                       FS_FILE_NAME_LEN    *p_name_len,
                       CPU_SIZE_T          name_buf_size,
                       RTOS_ERR            *p_err)
{
  FS_FAT_SEC_BYTE_POS tmp_pos;
  CPU_BOOLEAN         entry_parsed;

  WITH_SCOPE_BEGIN(p_err) {
    tmp_pos = *p_entry_start_pos;
    BREAK_ON_ERR(entry_parsed = FS_FAT_NextEntryParse(p_fat_vol,
                                                      p_entry_start_pos,
                                                      p_entry_end_pos,
                                                      p_entry_info,
                                                      p_name,
                                                      p_name_len,
                                                      name_buf_size,
                                                      p_err));
    ASSERT_BREAK(entry_parsed, RTOS_ERR_NOT_FOUND);
    ASSERT_BREAK(FS_FAT_SEC_BYTE_POS_ARE_EQUAL(*p_entry_start_pos, tmp_pos), RTOS_ERR_NOT_FOUND);
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                           FS_FAT_EntryFind()
 *
 * @brief    Look on disk for the entry specified by a given name, starting at a given position.
 *
 * @param    p_fat_vol           Pointer to a FAT volume object.
 *
 * @param    p_name              Pointer to the entry name to be found (see Note #1).
 *
 * @param    p_entry_info        Pointer to a FAT entry info structure (may be DEF_NULL).
 *
 * @param    p_entry_start_pos   Pointer to a variable that holds the lookup start position and will
 *                               receive the found entry start position (or the void position if no
 *                               entry is found).
 *
 * @param    p_entry_end_pos     Pointer to a variable that will receive the found entry end position.
 *
 * @param    p_err               Error pointer.
 *
 * @note     (1) The entry name may be an intermediate path component. It must contain no leading path
 *               separator and be terminated either by the null character or by a valid path separator
 *               (i.e '/' or '\').
 *******************************************************************************************************/
CPU_BOOLEAN FS_FAT_EntryFind(FS_FAT_VOL          *p_fat_vol,
                             CPU_CHAR            *p_name,
                             FS_FAT_ENTRY_INFO   *p_entry_info,
                             FS_FAT_SEC_BYTE_POS *p_entry_start_pos,
                             FS_FAT_SEC_BYTE_POS *p_entry_end_pos,
                             RTOS_ERR            *p_err)
{
  CPU_INT32U                        *p_name_8_3;
  CPU_INT32U                        name_8_3_str[3];
  FS_FLAGS                          name_chk_flags;
  FS_FAT_SEC_BYTE_POS               lookup_pos;
  FS_FAT_ENTRY_FIND_BY_NAME_CB_DATA find_data;

  WITH_SCOPE_BEGIN(p_err) {
    p_name_8_3 = DEF_NULL;                                      // Dflt val.

    if (FSCore_PathSegIsDot(p_name)) {
      p_name_8_3 = (CPU_INT32U *)&FS_FAT_NameDot[0];
    } else if (FSCore_PathSegIsDotDot(p_name)) {
      p_name_8_3 = (CPU_INT32U *)&FS_FAT_NameDotDot[0];
    } else {
      name_chk_flags = FS_FAT_SFN_NameChk(p_name);              // Chk if valid SFN.
      if (DEF_BIT_IS_CLR(name_chk_flags, FS_FAT_FN_CHK_FLAGS_NAME_IS_TOO_LONG)
          && DEF_BIT_IS_CLR(name_chk_flags, FS_FAT_FN_CHK_FLAGS_EXT_IS_TOO_LONG)
          && DEF_BIT_IS_CLR(name_chk_flags, FS_FAT_FN_CHK_FLAGS_HAS_INVALID_CHAR)) {
#if (FS_CORE_CFG_DBG_MEM_CLR_EN == DEF_ENABLED)
        Mem_Clr((void *)&name_8_3_str[0], sizeof(name_8_3_str));
#endif
        p_name_8_3 = &name_8_3_str[0];
        FS_FAT_SFN_NameCreate(p_name, &name_8_3_str[0]);        // Create 8.3 name.
      }
#if (FS_FAT_CFG_LFN_EN == DEF_DISABLED)
      else {
        BREAK_ERR_SET(RTOS_ERR_NAME_INVALID);
      }
#endif
    }

#if (FS_FAT_CFG_LFN_EN == DEF_ENABLED)
    name_chk_flags = FS_FAT_LFN_NameChk(p_name);
    ASSERT_BREAK(DEF_BIT_IS_CLR(name_chk_flags, FS_FAT_FN_CHK_FLAGS_NAME_IS_TOO_LONG), RTOS_ERR_NAME_INVALID);
    ASSERT_BREAK(DEF_BIT_IS_CLR(name_chk_flags, FS_FAT_FN_CHK_FLAGS_HAS_INVALID_CHAR), RTOS_ERR_NAME_INVALID);
#endif

    find_data.Name83Ptr = p_name_8_3;
    find_data.EntryInfoPtr = p_entry_info;
    find_data.EntryFound = DEF_NO;
    find_data.EndDirentPos = FS_FAT_VoidSecBytePos;
    find_data.StartDirentPos = FS_FAT_VoidSecBytePos;
#if (FS_FAT_CFG_LFN_EN == DEF_ENABLED)
    find_data.HasNameMatch = DEF_NO;
    find_data.NamePtr = p_name;
#endif

    lookup_pos = *p_entry_start_pos;
    BREAK_ON_ERR(FS_FAT_DirTblBrowseAcquire(p_fat_vol,
                                            &lookup_pos,
                                            FS_FAT_EntryFindByNameCb,
                                            &find_data,
                                            FS_CACHE_BLK_GET_MODE_RD,
                                            DEF_NULL,
                                            p_err));

    *p_entry_start_pos = find_data.StartDirentPos;
    *p_entry_end_pos = find_data.EndDirentPos;
  } WITH_SCOPE_END

  return (find_data.EntryFound);
}

/****************************************************************************************************//**
 *                                       FS_FAT_EntryCreateInternal()
 *
 * @brief    Entry create.
 *
 * @param    p_fat_vol           Pointer to a FAT volume.
 *
 * @param    p_entry_start_pos   Pointer to a variable that will receive the created entry start position.
 *
 * @param    p_entry_end_pos     Pointer to a variable that will receive the created entry end position.
 *
 * @param    p_fat_entry_info    Pointer to a FAT entry info structure.
 *
 * @param    p_entry_name        Entry name.
 *
 * @param    dir_tbl_first_sec   Parent entry's directory table first sector.
 *
 * @param    is_dir              Indicates whether the entry to create is a file or directory.
 *
 * @param    p_wr_job_handle     Pointer to cache job handle.
 *
 * @param    p_err               Error pointer.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FS_FAT_EntryCreateInternal(FS_FAT_VOL             *p_fat_vol,
                                FS_FAT_SEC_BYTE_POS    *p_entry_start_pos,
                                FS_FAT_SEC_BYTE_POS    *p_entry_end_pos,
                                CPU_CHAR               *p_entry_name,
                                FS_FAT_SEC_NBR         dir_tbl_first_sec,
                                CPU_BOOLEAN            is_dir,
                                FS_CACHE_WR_JOB_HANDLE *p_wr_job_handle,
                                RTOS_ERR               *p_err)
{
  CPU_INT08U           *p_dir_entry;
  CPU_INT08U           *p_buf;
  FS_FAT_CLUS_NBR      file_clus;
  FS_FAT_CLUS_NBR      dir_parent_clus;
  FS_FAT_SEC_BYTE_POS  dir_start_pos;
  FS_FAT_SEC_BYTE_POS  dir_end_pos;
  FS_FLAGS             fn_flags;
  FS_FAT_DIR_ENTRY_QTY dirent_cnt;
  CPU_INT32U           name_8_3_str[3];
  CPU_BOOLEAN          name_is_lower_case;
  CPU_BOOLEAN          ext_is_lower_case;
  CPU_INT08U           lb_size_log2;
#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
  FS_CACHE_WR_JOB_HANDLE stub_job_handle = FS_CACHE_WR_JOB_HANDLE_INIT;
#endif
#if (FS_FAT_CFG_LFN_EN == DEF_ENABLED)
  FS_FILE_NAME_LEN name_len;
  CPU_INT08U       chk_sum;
#endif

  WITH_SCOPE_BEGIN(p_err) {
    LOG_VRB(("Creating new entry in dir tbl starting at sec ", (u)dir_tbl_first_sec, "."));

    ASSERT_BREAK(!FSCore_PathSegIsDot(p_entry_name)
                 && !FSCore_PathSegIsDotDot(p_entry_name),
                 RTOS_ERR_NAME_INVALID);

    fn_flags = FS_FAT_SFN_NameChk(p_entry_name);

    if (DEF_BIT_IS_CLR(fn_flags, FS_FAT_FN_CHK_FLAGS_NAME_IS_TOO_LONG)
        && DEF_BIT_IS_CLR(fn_flags, FS_FAT_FN_CHK_FLAGS_EXT_IS_TOO_LONG)
        && DEF_BIT_IS_CLR(fn_flags, FS_FAT_FN_CHK_FLAGS_HAS_INVALID_CHAR)
        && DEF_BIT_IS_CLR(fn_flags, FS_FAT_FN_CHK_FLAGS_IS_MIXED_CASE)) {
      name_is_lower_case = DEF_BIT_IS_SET(fn_flags, FS_FAT_FN_CHK_FLAGS_NAME_IS_LOWER_CASE);
      ext_is_lower_case = DEF_BIT_IS_SET(fn_flags, FS_FAT_FN_CHK_FLAGS_EXT_IS_LOWER_CASE);
      dirent_cnt = 1u;
    } else {
#if (FS_FAT_CFG_LFN_EN == DEF_ENABLED)
      fn_flags = FS_FAT_LFN_NameChk(p_entry_name);
      name_len = FSCore_PathSegLenGet(p_entry_name);
      if (DEF_BIT_IS_CLR(fn_flags, FS_FAT_FN_CHK_FLAGS_NAME_IS_TOO_LONG)
          && DEF_BIT_IS_CLR(fn_flags, FS_FAT_FN_CHK_FLAGS_EXT_IS_TOO_LONG)
          && DEF_BIT_IS_CLR(fn_flags, FS_FAT_FN_CHK_FLAGS_HAS_INVALID_CHAR)) {
        name_is_lower_case = DEF_YES;
        ext_is_lower_case = DEF_YES;
        //                                                         LFN dir entry cnt.
        dirent_cnt = (FS_FAT_DIR_ENTRY_QTY)(((name_len - 1u) / FS_FAT_DIRENT_LFN_NBR_CHARS) + 1u);
        dirent_cnt += 1u;                                       // SFN dir entry cnt.
      } else
#endif
      {
        BREAK_ERR_SET(RTOS_ERR_NAME_INVALID);
      }
    }

    BREAK_ON_ERR(FS_FAT_DirEntriesEmptyGet(p_fat_vol,
                                           dirent_cnt,
                                           dir_tbl_first_sec,
                                           &dir_start_pos,
                                           &dir_end_pos,
                                           p_wr_job_handle,
                                           p_err));

    if (is_dir) {                                               // If dir, make 1st dir clus.
      BREAK_ON_ERR(file_clus = FS_FAT_ClusChainAlloc(p_fat_vol,
                                                     0u,
                                                     DEF_NULL,
                                                     1u,
                                                     DEF_YES,
                                                     DEF_NULL,
                                                     FS_FAT_LB_TYPE_DIRENT,
                                                     p_err));

      BREAK_ON_ERR(FS_VOL_CACHE_BLK_WR(&p_fat_vol->Vol,
                                       FS_FAT_CLUS_TO_SEC(p_fat_vol, file_clus),
                                       FS_FAT_LB_TYPE_DIRENT,
                                       *p_wr_job_handle,
                                       &p_buf,
                                       p_wr_job_handle,
                                       p_err)) {
        BREAK_ON_ERR(lb_size_log2 = FSBlkDev_LbSizeLog2Get(p_fat_vol->Vol.BlkDevHandle, p_err));
        Mem_Clr((void *)p_buf, (CPU_SIZE_T)FS_UTIL_PWR2(lb_size_log2));

        p_dir_entry = p_buf;                                    // Make 'dot' entry.
        FS_FAT_SFN_DirEntryFmt((void *)p_dir_entry,
                               (CPU_INT32U *)&FS_FAT_NameDot[0],
                               DEF_TRUE,
                               file_clus,
                               DEF_NO,
                               DEF_NO);

        p_dir_entry += FS_FAT_SIZE_DIR_ENTRY;                   // Make 'dot dot' entry.
        dir_parent_clus = (dir_tbl_first_sec == p_fat_vol->RootDirStart) ? 0u : FS_FAT_SEC_TO_CLUS(p_fat_vol, dir_tbl_first_sec);
        FS_FAT_SFN_DirEntryFmt((void *)p_dir_entry,
                               (CPU_INT32U *)&FS_FAT_NameDotDot[0],
                               DEF_TRUE,
                               dir_parent_clus,
                               DEF_NO,
                               DEF_NO);
      }
    } else {                                                    // Otherwise zero first clus.
      file_clus = 0u;
    }

#if (FS_FAT_CFG_JOURNAL_EN == DEF_ENABLED)
    BREAK_ON_ERR(FS_FAT_JournalEnterEntryCreate(p_fat_vol,
                                                &dir_start_pos,
                                                &dir_end_pos,
                                                p_err));
#endif

#if (FS_FAT_CFG_LFN_EN == DEF_ENABLED)
    if (dirent_cnt > 1u) {
      BREAK_ON_ERR(FS_FAT_LFN_SFN_Alloc(p_fat_vol,              // If SFN could NOT be formed directly ...
                                        p_entry_name,           // ... use LFN generation.
                                        &name_8_3_str[0],
                                        dir_tbl_first_sec,
                                        p_err));
    } else
#endif
    {
      FS_FAT_SFN_NameCreate(p_entry_name, &name_8_3_str[0]);
    }

#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
    //                                                             ---------- FAT, JNL, DIRENT BLK ORDERING -----------
    BREAK_ON_ERR(stub_job_handle = FSCache_WrJobAppend(p_fat_vol->Vol.CacheBlkDevDataPtr->CachePtr,
                                                       *p_wr_job_handle,
                                                       p_err));

    BREAK_ON_ERR(stub_job_handle = FSCache_WrJobJoin(p_fat_vol->Vol.CacheBlkDevDataPtr->CachePtr,
                                                     p_fat_vol->FatWrJobHandle,
                                                     stub_job_handle,
                                                     p_err));

#if (FS_FAT_CFG_JOURNAL_EN == DEF_ENABLED)
    if (p_fat_vol->IsJournaled) {
      BREAK_ON_ERR(stub_job_handle = FSCache_WrJobJoin(p_fat_vol->Vol.CacheBlkDevDataPtr->CachePtr,
                                                       p_fat_vol->JournalData.JnlWrJobHandle,
                                                       stub_job_handle,
                                                       p_err));
    }
#endif
#endif

    //                                                             ----------------- WR SFN DIR ENTRY -----------------
    BREAK_ON_ERR(FS_VOL_CACHE_BLK_RW)(&p_fat_vol->Vol,
                                      dir_end_pos.SecNbr,
                                      FS_FAT_LB_TYPE_DIRENT,
                                      stub_job_handle,
                                      &p_buf,
                                      p_wr_job_handle,
                                      p_err) {
      p_dir_entry = p_buf + dir_end_pos.SecOffset;
      FS_FAT_SFN_DirEntryFmt(p_dir_entry,
                             &name_8_3_str[0],
                             is_dir,
                             file_clus,
                             name_is_lower_case,
                             ext_is_lower_case);
    }

    //                                                             ----------------- WR LFN DIR ENTRY -----------------
#if (FS_FAT_CFG_LFN_EN == DEF_ENABLED)
    if (dirent_cnt > 1u) {
      chk_sum = FS_FAT_LFN_ChkSumCalc((CPU_INT08U *)&name_8_3_str[0]);
      BREAK_ON_ERR(FS_FAT_LFN_DirEntriesCreate(p_fat_vol,
                                               p_entry_name,
                                               chk_sum,
                                               &dir_start_pos,
                                               p_wr_job_handle,
                                               p_err));
    }
#endif

#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
    FSCache_StubWrJobRem(p_fat_vol->Vol.CacheBlkDevDataPtr->CachePtr, stub_job_handle);
#endif

    *p_entry_start_pos = dir_start_pos;
    *p_entry_end_pos = dir_end_pos;

#if (LOG_VRB_IS_EN() == DEF_ENABLED)
    if (is_dir == DEF_YES) {
      LOG_VRB(("Created directory \"", (s)p_entry_name, "\":"));
      LOG_VRB(("Dir table first clus: ", (u)file_clus));
    } else {
      LOG_VRB(("Created file \"", (s)p_entry_name, "\"."));
    }
#endif
    LOG_VRB(("Dir entry starting at sec:offset ", (u)dir_start_pos.SecNbr, ":", (u)dir_start_pos.SecOffset));
    LOG_VRB(("Dir entry ending at sec:offset ", (u)dir_end_pos.SecNbr, ":", (u)dir_end_pos.SecOffset));
  } WITH_SCOPE_END
}
#endif

/****************************************************************************************************//**
 *                                           FS_FAT_EntryDelInternal()
 *
 * @brief    Delete entry.
 *
 * @param    p_fat_vol               Pointer to a FAT volume.
 *
 * @param    p_dte_start_pos         Pointer to the directory entry start position.
 *
 * @param    p_entry_wr_job_handle   Pointer to cache job handle.
 *
 * @param    p_err                   Error pointer.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FS_FAT_EntryDelInternal(FS_FAT_VOL             *p_fat_vol,
                             FS_FAT_SEC_BYTE_POS    *p_dte_start_pos,
                             FS_CACHE_WR_JOB_HANDLE *p_entry_wr_job_handle,
                             RTOS_ERR               *p_err)
{
  FS_FAT_SEC_BYTE_POS entry_start_pos;
  FS_FAT_SEC_BYTE_POS entry_end_pos;
  FS_FAT_ENTRY_INFO   entry_info;
  FS_FAT_CLUS_NBR     first_clus_nbr;

  WITH_SCOPE_BEGIN(p_err) {
    entry_start_pos = *p_dte_start_pos;

    //                                                             ------------------ DEL DIR ENTRY -------------------
    BREAK_ON_ERR(FS_FAT_EntryParse(p_fat_vol,
                                   &entry_start_pos,
                                   &entry_end_pos,
                                   &entry_info,
                                   DEF_NULL,
                                   DEF_NULL,
                                   0u,
                                   p_err));

    BREAK_ON_ERR(FS_FAT_DirEntriesErase(p_fat_vol,
                                        &entry_start_pos,
                                        &entry_end_pos,
                                        p_entry_wr_job_handle,
                                        p_err));

#if (FS_TEST_FAT_JNL_EN == DEF_ENABLED)
    if (FS_Test_FAT_JournalTestNumber == FSENTRY_DEL_FILE1) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
      return;
    }
#endif

    //                                                             ------------------ DEL CLUS CHAIN ------------------
    first_clus_nbr = entry_info.FirstClusNbr;
    if (FS_FAT_IS_VALID_CLUS(p_fat_vol, first_clus_nbr)) {
      BREAK_ON_ERR((void)FS_FAT_ClusChainDel(p_fat_vol,
                                             first_clus_nbr,
                                             DEF_YES,
                                             p_err));

#if (FS_TEST_FAT_JNL_EN == DEF_ENABLED)
      if (FS_Test_FAT_JournalTestNumber == FSENTRY_DEL_FILE2) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
        return;
      }
#endif
    }
  } WITH_SCOPE_END
}
#endif

/****************************************************************************************************//**
 *                                           FS_FAT_DirEntriesErase()
 *
 * @brief    Delete directory entries located between the given start and end positions.
 *
 * @param    p_fat_vol               Pointer to a FAT volume.
 *
 * @param    p_dir_start_pos         Pointer to the directory table start position.
 *
 * @param    p_dir_end_pos           Pointer to the directory table end position.
 *
 * @param    p_entry_wr_job_handle   Pointer to cache job handle.
 *
 * @param    p_err                   Error pointer.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FS_FAT_DirEntriesErase(FS_FAT_VOL             *p_fat_vol,
                            FS_FAT_SEC_BYTE_POS    *p_dir_start_pos,
                            FS_FAT_SEC_BYTE_POS    *p_dir_end_pos,
                            FS_CACHE_WR_JOB_HANDLE *p_entry_wr_job_handle,
                            RTOS_ERR               *p_err)
{
  FS_FAT_DTE_ERASE_CB_DATA erase_data;

  WITH_SCOPE_BEGIN(p_err) {
#if (FS_FAT_CFG_JOURNAL_EN == DEF_ENABLED)
    BREAK_ON_ERR(FS_FAT_JournalEnterEntryUpdate(p_fat_vol,
                                                *p_dir_start_pos,
                                                *p_dir_end_pos,
                                                p_err));
#endif

    erase_data.EndDtePos = *p_dir_end_pos;
    erase_data.EntryCacheWrJobHandlePtr = p_entry_wr_job_handle;

    BREAK_ON_ERR(FS_FAT_DirTblBrowse(p_fat_vol,
                                     p_dir_start_pos,
                                     FS_FAT_DirEntriesEraseCb,
                                     (void *)&erase_data,
                                     p_err));
  } WITH_SCOPE_END
}
#endif

/****************************************************************************************************//**
 *                                           FS_FAT_DirTblBrowse()
 *
 * @brief    Browse directory table and execute the given callback for each directory entry.
 *
 * @param    p_fat_vol           Pointer to FAT volume.
 *
 * @param    p_dte_start_pos     Pointer to the start position inside the directory table.
 *
 * @param    on_dte_cb           Callback function to be called for each directory entry.
 *
 * @param    p_arg               Callback argument.
 *
 * @param    p_err               Error pointer.
 *******************************************************************************************************/
void FS_FAT_DirTblBrowse(FS_FAT_VOL          *p_fat_vol,
                         FS_FAT_SEC_BYTE_POS *p_dte_start_pos,
                         FS_FAT_ON_DTE_CB    on_dte_cb,
                         void                *p_arg,
                         RTOS_ERR            *p_err)
{
  FS_FAT_DIRENT_BROWSE_OUTCOME outcome = FS_FAT_DIRENT_BROWSE_OUTCOME_CONTINUE;
  CPU_INT08U                   lb_size_log2;

  WITH_SCOPE_BEGIN(p_err) {
    BREAK_ON_ERR(lb_size_log2 = FSBlkDev_LbSizeLog2Get(p_fat_vol->Vol.BlkDevHandle, p_err));

    BREAK_ON_ERR(while) (p_dte_start_pos->SecNbr != FS_FAT_VOID_DATA_LB_NBR) {
      //                                                           Browse dir entries within a sector.
      BREAK_ON_ERR(while) (p_dte_start_pos->SecOffset < FS_UTIL_PWR2(lb_size_log2)) {
        BREAK_ON_ERR(outcome = on_dte_cb(p_fat_vol,
                                         p_dte_start_pos,
                                         p_arg,
                                         p_err));

        if (outcome == FS_FAT_DIRENT_BROWSE_OUTCOME_STOP) {
          break;
        }
        //                                                         Go to next dir entry.
        p_dte_start_pos->SecOffset += FS_FAT_SIZE_DIR_ENTRY;
      }

      if (outcome == FS_FAT_DIRENT_BROWSE_OUTCOME_STOP) {
        break;
      }
      //                                                           Get next sector containing dir table.
      BREAK_ON_ERR(p_dte_start_pos->SecNbr = FS_FAT_SecNextGet(p_fat_vol,
                                                               p_dte_start_pos->SecNbr,
                                                               p_err));
      p_dte_start_pos->SecOffset = 0u;
    }
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                           FS_FAT_VolLabelGet()
 *
 * @brief    Get volume label.
 *
 * @param    p_vol           Pointer to volume.
 *
 * @param    p_label_buf     String buffer that will receive volume label.
 *
 * @param    buf_size        Size of string buffer (see Note #1).
 *
 * @param    p_err           Error pointer.
 *
 * @note     (1) The maximum label length is 11 characters.  If the label is shorter that this, it
 *               should be padded with space characters (ASCII code point 20h) in the directory entry;
 *               those final space characters are kept in the string returned to the application.
 *******************************************************************************************************/
void FS_FAT_VolLabelGet(FS_VOL     *p_vol,
                        CPU_CHAR   *p_label_buf,
                        CPU_SIZE_T buf_size,
                        RTOS_ERR   *p_err)
{
  FS_FAT_VOL          *p_fat_vol;
  void                *p_dir_entry;
  CPU_INT08U          *p_buf;
  FS_FAT_SEC_BYTE_POS dir_start_pos;
  FS_FAT_SEC_NBR      dir_start_sec;

  WITH_SCOPE_BEGIN(p_err) {
    p_fat_vol = (FS_FAT_VOL *)p_vol;
    dir_start_sec = p_fat_vol->RootDirStart;

    Mem_Clr(p_label_buf, buf_size);
    //                                                             -------------------- FIND LABEL --------------------
    dir_start_pos.SecNbr = dir_start_sec;
    dir_start_pos.SecOffset = 0u;
    BREAK_ON_ERR(FS_FAT_VolLabelFind(p_fat_vol,
                                     &dir_start_pos,
                                     p_err));

    if (FS_FAT_SEC_BYTE_POS_IS_VOID(dir_start_pos)) {
      return;
    }

    //                                                             ---------------- COPY LABEL TO BUF -----------------
    BREAK_ON_ERR(FS_VOL_CACHE_BLK_RD) (&p_fat_vol->Vol,
                                       dir_start_pos.SecNbr,
                                       FS_FAT_LB_TYPE_DIRENT,
                                       &p_buf,
                                       p_err) {
      p_dir_entry = (void *)(p_buf + dir_start_pos.SecOffset);
      Mem_Copy((void *)p_label_buf, p_dir_entry, DEF_MIN(buf_size, FS_FAT_MAX_VOL_LABEL_LEN));
    }

    if (buf_size > FS_FAT_MAX_VOL_LABEL_LEN) {
      p_label_buf[FS_FAT_MAX_VOL_LABEL_LEN] = '\0';
    } else {
      ASSERT_BREAK(p_label_buf[buf_size - 1] == '\0', RTOS_ERR_WOULD_OVF);
    }
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                           FS_FAT_VolLabelSet()
 *
 * @brief    Set volume label.
 *
 * @param    p_vol       Pointer to volume.
 *
 * @param    label       Volume label.
 *
 * @param    p_err       Error pointer.
 *
 * @note     (1) The maximum label length is 11 characters.  If the label is shorter than this, it is
 *               padded with space characters (ASCII code point 20h) until it is 11 characters.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FS_FAT_VolLabelSet(FS_VOL   *p_vol,
                        CPU_CHAR *label,
                        RTOS_ERR *p_err)
{
  FS_FAT_VOL             *p_fat_vol;
  CPU_INT08U             *p_dir_entry;
  CPU_INT08U             *p_buf;
  CPU_SIZE_T             label_len;
  FS_FAT_SEC_BYTE_POS    dir_start_pos;
  FS_FAT_SEC_BYTE_POS    dir_end_pos;
  FS_FAT_SEC_BYTE_POS    lookup_pos;
  FS_FILE_NAME_LEN       label_char_ix;
  CPU_BOOLEAN            is_legal;
  FS_CACHE_WR_JOB_HANDLE wr_job_handle = FS_CACHE_WR_JOB_HANDLE_INIT;

  WITH_SCOPE_BEGIN(p_err) {
    p_fat_vol = (FS_FAT_VOL *)p_vol;

    //                                                             -------------------- FIND LABEL --------------------
    dir_start_pos.SecNbr = p_fat_vol->RootDirStart;
    dir_start_pos.SecOffset = 0u;
    lookup_pos = dir_start_pos;

    label_len = Str_Len_N(label, FS_FAT_MAX_VOL_LABEL_LEN + 1u);
    ASSERT_BREAK(label_len <= FS_FAT_MAX_VOL_LABEL_LEN, RTOS_ERR_WOULD_OVF);

    BREAK_ON_ERR(FS_FAT_VolLabelFind(p_fat_vol,                 // Find label ...
                                     &lookup_pos,
                                     p_err));

    if (FS_FAT_SEC_BYTE_POS_IS_VOID(lookup_pos)) {              // If not found  find place for entry.
      lookup_pos = dir_start_pos;
      BREAK_ON_ERR(FS_FAT_DirEntriesEmptyGet(p_fat_vol,
                                             1u,
                                             p_fat_vol->RootDirStart,
                                             &lookup_pos,
                                             &dir_end_pos,      // Not used.
                                             &wr_job_handle,
                                             p_err));
    }

    //                                                             ---------------- COPY LABEL TO BUF -----------------
    BREAK_ON_ERR(FS_VOL_CACHE_BLK_RW)(&p_fat_vol->Vol,
                                      lookup_pos.SecNbr,
                                      FS_FAT_LB_TYPE_DIRENT,
                                      wr_job_handle,
                                      &p_buf,
                                      &wr_job_handle,
                                      p_err) {
      p_dir_entry = p_buf + lookup_pos.SecOffset;

      //                                                           Clear the whole dir entry.
      Mem_Clr((void *)p_dir_entry, FS_FAT_SIZE_DIR_ENTRY);

      //                                                           Clear label buf.
      for (label_char_ix = 0u;
           label_char_ix < FS_FAT_MAX_VOL_LABEL_LEN;
           label_char_ix++) {
        p_dir_entry[label_char_ix] = (CPU_CHAR)ASCII_CHAR_SPACE;
      }
      p_dir_entry[11] = (CPU_CHAR)ASCII_CHAR_NULL;              // Clear final byte of label buf.

      label_char_ix = 0u;
      BREAK_ON_ERR(while) (label[label_char_ix] != (CPU_CHAR)ASCII_CHAR_NULL) {
        is_legal = FS_FAT_IS_LEGAL_VOL_LABEL_CHAR(label[label_char_ix]);
        ASSERT_BREAK(label_char_ix <= FS_FAT_MAX_VOL_LABEL_LEN, RTOS_ERR_WOULD_OVF);
        ASSERT_BREAK(is_legal, RTOS_ERR_NAME_INVALID);
        p_dir_entry[label_char_ix] = ASCII_TO_UPPER(label[label_char_ix]);
        label_char_ix++;
      }

      p_dir_entry[FS_FAT_DIRENT_OFFSET_ATTR] = FS_FAT_DIRENT_ATTR_VOLUME_ID;
    }

#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
    BREAK_ON_ERR(FSCache_WrJobExec(p_fat_vol->Vol.CacheBlkDevDataPtr->CachePtr,
                                   wr_job_handle,
                                   DEF_NO,
                                   p_err));
#endif
  } WITH_SCOPE_END
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           FS_FAT_VolLabelFind()
 *
 * @brief    Search directory for volume label entry.
 *
 * @param    p_fat_vol       Pointer to volume.
 *
 * @param    p_entry_pos     Pointer to volume entry position.
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/
static void FS_FAT_VolLabelFind(FS_FAT_VOL          *p_fat_vol,
                                FS_FAT_SEC_BYTE_POS *p_entry_pos,
                                RTOS_ERR            *p_err)
{
  FS_FAT_SEC_BYTE_POS label_pos;

  label_pos = FS_FAT_VoidSecBytePos;

  FS_FAT_DirTblBrowseAcquire(p_fat_vol,
                             p_entry_pos,
                             FS_FAT_VolLabelFindCb,
                             (void *)&label_pos,
                             FS_CACHE_BLK_GET_MODE_RD,
                             DEF_NULL,
                             p_err);

  *p_entry_pos = label_pos;
}

/****************************************************************************************************//**
 *                                           FS_FAT_VolLabelFindCb()
 *
 * @brief    Callback called when volume label entry found in root directory.
 *
 * @param    p_dirent    Pointer to directory entry.
 *
 * @param    p_cur_pos   Pointer to current position of directory entry.
 *
 * @param    p_dte_info  Pointer to directory entry information.
 *
 * @param    p_arg       Pointer to argument passed to callback.
 *
 * @return   Browse outcome.
 *******************************************************************************************************/
static FS_FAT_DIRENT_BROWSE_OUTCOME FS_FAT_VolLabelFindCb(void                *p_dirent,
                                                          FS_FAT_SEC_BYTE_POS *p_cur_pos,
                                                          void                *p_dte_info,
                                                          void                *p_arg)
{
  FS_FAT_SEC_BYTE_POS          *p_label_pos;
  FS_FAT_DTE_TYPE              dte_type;
  FS_FAT_DIRENT_BROWSE_OUTCOME outcome;

  PP_UNUSED_PARAM(p_dirent);

  p_label_pos = (FS_FAT_SEC_BYTE_POS *)p_arg;
  dte_type = *((FS_FAT_DTE_TYPE *)p_dte_info);
  outcome = FS_FAT_DIRENT_BROWSE_OUTCOME_STOP;

  switch (dte_type) {
    case FS_FAT_DTE_TYPE_FREE:
      outcome = FS_FAT_DIRENT_BROWSE_OUTCOME_STOP;
      break;
    case FS_FAT_DTE_TYPE_ERASED:
    case FS_FAT_DTE_TYPE_LFN:
    case FS_FAT_DTE_TYPE_SFN:
    case FS_FAT_DTE_TYPE_INVALID_LFN:
      outcome = FS_FAT_DIRENT_BROWSE_OUTCOME_CONTINUE;
      break;
    case FS_FAT_DTE_TYPE_VOL_LABEL:
      *p_label_pos = *p_cur_pos;
      outcome = FS_FAT_DIRENT_BROWSE_OUTCOME_STOP;
      break;
    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, FS_FAT_DIRENT_BROWSE_OUTCOME_STOP);
  }

  return (outcome);
}

/****************************************************************************************************//**
 *                                           FS_FAT_DteAcquireCb()
 *
 * @brief    Directory entry browsing callback.
 *
 * @param    p_fat_vol   Pointer to a FAT volume.
 *
 * @param    p_dte_pos   Pointer to the current directory entry position.
 *
 * @param    p_arg       Pointer to argument passed to callback.
 *
 * @param    p_err       Error pointer.
 *
 * @return   Browse outcome.
 *
 * @note     (1) LFN entries for a file or directory are listed before the SFN (Short File Name) entry
 *               and are in reverse order. For example, the following long file name (My Long File
 *               Name.rtf) has 2 LFN entries preceding the SFN entry:
 *
 *               LFN Entry: Attribute(LFN),  available characters(Name.rtf),     sequence number(2),
 *               LFN Entry: Attribute(LFN),  available characters(My Long File), sequence number(1)
 *               SFN Entry: Attribute(File), available characters(MYLONG~1.RTF)
 *
 * @note     (2) An LFN entry has the following fields:
 *
 *               Byte Range      Description
 *               0-0             Sequence number (ORed with 0x40) and allocation status (0xE5 if unallocated)
 *               1-10            File name characters 1-5 (Unicode)
 *               11-11           File attributes (0x0f)
 *               12-12           Reserved
 *               13-13           Checksum
 *               14-25           File name characters 6-11 (Unicode)
 *               26-27           Reserved
 *               28-31           File name characters 12-13 (Unicode)
 *******************************************************************************************************/
static FS_FAT_DIRENT_BROWSE_OUTCOME FS_FAT_DteAcquireCb(FS_FAT_VOL          *p_fat_vol,
                                                        FS_FAT_SEC_BYTE_POS *p_dte_pos,
                                                        void                *p_arg,
                                                        RTOS_ERR            *p_err)
{
  FS_FAT_DTE_ACQUIRE_CB_DATA   *p_dte_acquire_data;
  CPU_INT08U                   *p_dirent_08;
  void                         *p_dte_info;
  CPU_INT08U                   *p_buf;
  CPU_INT08U                   first_entry_byte;
  CPU_INT08U                   attrib;
  CPU_BOOLEAN                  is_volume_id;
  FS_FAT_DTE_TYPE              dte_type = FS_FAT_DTE_TYPE_NONE;
  FS_FAT_SFN_DTE_INFO          sfn_dte_info;
  FS_FAT_DIRENT_BROWSE_OUTCOME outcome;
#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
  FS_CACHE_WR_JOB_HANDLE wr_job_handle = FS_CACHE_WR_JOB_HANDLE_INIT;
#endif
#if (FS_FAT_CFG_LFN_EN == DEF_ENABLED)
  FS_FAT_LFN_DTE_INFO lfn_dte_info;
  CPU_BOOLEAN         is_lfn;
  CPU_INT08U          chksum_calc;
  CPU_BOOLEAN         lfn_seq_nbr_valid;
  CPU_BOOLEAN         is_last_lfn;
  CPU_INT08U          lfn_seq_nbr;
#endif

  WITH_SCOPE_BEGIN(p_err) {
    p_dte_acquire_data = (FS_FAT_DTE_ACQUIRE_CB_DATA *)p_arg;
    outcome = FS_FAT_DIRENT_BROWSE_OUTCOME_STOP;

#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
    wr_job_handle = (p_dte_acquire_data->EntryCacheWrJobHandlePtr == DEF_NULL)
                    ? FSCache_VoidWrJobHandle : *p_dte_acquire_data->EntryCacheWrJobHandlePtr;
#endif

    BREAK_ON_ERR(FS_VOL_CACHE_BLK_ADD)(&p_fat_vol->Vol,
                                       p_dte_pos->SecNbr,
                                       FS_FAT_LB_TYPE_DIRENT,
                                       p_dte_acquire_data->AcquireMode,
                                       wr_job_handle,
                                       &p_buf,
                                       &wr_job_handle,
                                       p_err) {
      p_dirent_08 = p_buf + p_dte_pos->SecOffset;
      first_entry_byte = *p_dirent_08;

      sfn_dte_info.ChkSumMatch = DEF_YES;
      sfn_dte_info.HasLFN = DEF_NO;
      sfn_dte_info.Type = FS_FAT_DTE_TYPE_NONE;

#if (FS_FAT_CFG_LFN_EN == DEF_ENABLED)
      lfn_dte_info.IsLast = DEF_NO;
      lfn_dte_info.SeqNo = 0u;
      lfn_dte_info.Type = FS_FAT_DTE_TYPE_NONE;
#endif

      if (first_entry_byte == FS_FAT_DIRENT_NAME_FREE) {        // If dir tbl's end has been reached, stop browsing.
        dte_type = FS_FAT_DTE_TYPE_FREE;
        p_dte_info = (void *)&dte_type;
        //                                                         If dir entry free but not last, continue browsing.
      } else if (first_entry_byte == FS_FAT_DIRENT_NAME_ERASED_AND_FREE) {
        dte_type = FS_FAT_DTE_TYPE_ERASED;
        p_dte_info = (void *)&dte_type;
      } else {
        attrib = *(p_dirent_08 + FS_FAT_DIRENT_OFFSET_ATTR);
        is_volume_id = DEF_BIT_IS_SET(attrib, FS_FAT_DIRENT_ATTR_VOLUME_ID);

        //                                                         -------------------- LFN ENTRY ---------------------
#if (FS_FAT_CFG_LFN_EN == DEF_ENABLED)                          // See Note #1 & #2 for more details about LFN entry.
        is_lfn = DEF_BIT_IS_SET(attrib & FS_FAT_DIRENT_ATTR_LONG_NAME_MASK, FS_FAT_DIRENT_ATTR_LONG_NAME);
        if (is_lfn) {
          //                                                       Get and chk LFN dir entry's seq nbr range.
          lfn_seq_nbr = (first_entry_byte & FS_FAT_DIRENT_NAME_LFN_SEQ_NO_MASK);
          lfn_seq_nbr_valid = (lfn_seq_nbr > 0u) && (lfn_seq_nbr <= 20u);
          lfn_dte_info.SeqNo = lfn_seq_nbr;

          if (lfn_seq_nbr_valid) {
            is_last_lfn = DEF_BIT_IS_SET(first_entry_byte, FS_FAT_DIRENT_NAME_LFN_LAST_LONG_ENTRY);
            if (is_last_lfn) {
              p_dte_acquire_data->HasFirstLFN = DEF_NO;
              p_dte_acquire_data->HasLastLFN = DEF_YES;
              p_dte_acquire_data->DteRemCnt = lfn_seq_nbr;
              p_dte_acquire_data->ChkSum = *(p_dirent_08 + FS_FAT_LFN_OFF_CHKSUM);
              lfn_dte_info.IsLast = DEF_YES;
            } else {
              if (p_dte_acquire_data->DteRemCnt != lfn_seq_nbr) {
                LOG_DBG(("Orphaned LFN entries? starting at sec:offset ",
                         (u)p_dte_pos->SecNbr, ":", (u)p_dte_pos->SecOffset, "."));
                dte_type = FS_FAT_DTE_TYPE_INVALID_LFN;
                p_dte_info = (void *)&dte_type;
              }
            }
            //                                                     Chk if all LFN dir entries rd successfully.
            p_dte_acquire_data->DteRemCnt--;
            if (p_dte_acquire_data->DteRemCnt == 0u) {
              p_dte_acquire_data->HasFirstLFN = DEF_YES;
            }

            dte_type = FS_FAT_DTE_TYPE_LFN;
            lfn_dte_info.Type = FS_FAT_DTE_TYPE_LFN;
            p_dte_info = (void *)&lfn_dte_info;
          } else {
            LOG_DBG(("LFN entry has an invalid sequence number: ", (u)lfn_seq_nbr, "."));
            dte_type = FS_FAT_DTE_TYPE_INVALID_LFN;
            p_dte_info = (void *)&dte_type;
          }
        } else
#endif
        if (!is_volume_id) {                                    // -------------------- SFN ENTRY ---------------------
          dte_type = FS_FAT_DTE_TYPE_SFN;
          sfn_dte_info.Type = FS_FAT_DTE_TYPE_SFN;
          p_dte_info = (void *)&sfn_dte_info;

#if (FS_FAT_CFG_LFN_EN == DEF_ENABLED)
          if (p_dte_acquire_data->HasFirstLFN) {                // If SFN entry has matching LFN entries
            sfn_dte_info.HasLFN = DEF_YES;
            if (!p_dte_acquire_data->HasLastLFN) {
              LOG_DBG(("Orphaned LFN entries? starting at sec:offset ",
                       (u)p_dte_pos->SecNbr, ":", (u)p_dte_pos->SecOffset, "."));
            } else {
              chksum_calc = FS_FAT_LFN_ChkSumCalc(p_dirent_08);
              if (chksum_calc != p_dte_acquire_data->ChkSum) {
                LOG_DBG(("Checksum mismatch: ", (u)chksum_calc, " != ", (u)p_dte_acquire_data->ChkSum, "."));
                sfn_dte_info.ChkSumMatch = DEF_NO;
              }
            }
          }
#endif
        } else {                                                // -------------------- VOL LABEL ---------------------
          dte_type = FS_FAT_DTE_TYPE_VOL_LABEL;
          p_dte_info = (void *)&dte_type;
        }
      }

      outcome = p_dte_acquire_data->OnDteAcquireCb((void *)p_dirent_08,
                                                   p_dte_pos,
                                                   p_dte_info,
                                                   p_dte_acquire_data->OnDteAcquireCbData);
    }

#if (FS_FAT_CFG_LFN_EN == DEF_ENABLED)                          // If SFN or vol label, reset LFN related state.
    if (dte_type != FS_FAT_DTE_TYPE_LFN) {
      p_dte_acquire_data->DteRemCnt = 0u;
      p_dte_acquire_data->HasLastLFN = DEF_NO;
      p_dte_acquire_data->HasFirstLFN = DEF_NO;
    }
#endif
  } WITH_SCOPE_END

  return (outcome);
}

/****************************************************************************************************//**
 *                                           FS_FAT_EntryTimeWr()
 *
 * @brief    Write directory entry time.
 *
 * @param    p_fat_vol       Pointer to a FAT volume.
 *
 * @param    p_dte_end_pos   Pointer to the directory entry end position.
 *
 * @param    p_time          Pointer to a date/time structure.
 *
 * @param    time_type       Time type:
 *                           FS_DATE_TIME_CREATE
 *                           FS_DATE_TIME_WR
 *                           FS_DATE_TIME_ACCESS
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_FAT_EntryTimeWr(FS_FAT_VOL           *p_fat_vol,
                               FS_FAT_SEC_BYTE_POS  *p_dte_end_pos,
                               sl_sleeptimer_date_t *p_time,
                               CPU_INT08U           time_type,
                               RTOS_ERR             *p_err)
{
  CPU_INT08U  *p_dte;
  CPU_INT08U  *p_buf;
  FS_FAT_DATE date_val;
  FS_FAT_TIME time_val;
#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
  FS_CACHE_WR_JOB_HANDLE wr_job_handle = FS_CACHE_WR_JOB_HANDLE_INIT;
#endif

  WITH_SCOPE_BEGIN(p_err) {
    BREAK_ON_ERR(FS_VOL_CACHE_BLK_RW)(&p_fat_vol->Vol,
                                      p_dte_end_pos->SecNbr,
                                      FS_FAT_LB_TYPE_DIRENT,
                                      FSCache_VoidWrJobHandle,
                                      &p_buf,
                                      &wr_job_handle,
                                      p_err) {
      p_dte = p_buf + p_dte_end_pos->SecOffset;

      date_val = FS_FAT_DateFmt(p_time);
      time_val = FS_FAT_TimeFmt(p_time);

      switch (time_type) {
        case FS_DATE_TIME_CREATE:
          FS_FAT_DIRENT_CREATE_TIME_DATE_SET(p_dte, time_val, date_val);
          break;

        case FS_DATE_TIME_MODIFY:
          FS_FAT_DIRENT_WR_TIME_DATE_SET(p_dte, time_val, date_val);
          break;

        case FS_DATE_TIME_ACCESS:
          FS_FAT_DIRENT_ACCESS_DATE_SET(p_dte, date_val);
          break;

        case FS_DATE_TIME_ALL:
          FS_FAT_DIRENT_CREATE_TIME_DATE_SET(p_dte, time_val, date_val);
          FS_FAT_DIRENT_WR_TIME_DATE_SET(p_dte, time_val, date_val);
          FS_FAT_DIRENT_ACCESS_DATE_SET(p_dte, date_val);
          break;

        default:
          RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
      }
    }

#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
    BREAK_ON_ERR(FS_CACHE_LOCK_WITH) (p_fat_vol->Vol.CacheBlkDevDataPtr->CachePtr) {
      BREAK_ON_ERR(FSCache_WrJobExec(p_fat_vol->Vol.CacheBlkDevDataPtr->CachePtr,
                                     wr_job_handle,
                                     DEF_NO,
                                     p_err));
    }
#endif
  } WITH_SCOPE_END
}
#endif

/****************************************************************************************************//**
 *                                       FS_FAT_DotDotEntryResolve()
 *
 * @brief    Find entry that points to the same directory table as the given 'dot dot' entry does.
 *
 * @param    p_fat_vol           Pointer to a FAT volume.
 *
 * @param    p_entry_start_pos   Pointer to a variable that holds the 'dot dot' entry start position
 *                               and will receive the corresponding regular entry start position.
 *
 * @param    p_err               Error pointer.
 *******************************************************************************************************/
static void FS_FAT_DotDotEntryResolve(FS_FAT_VOL          *p_fat_vol,
                                      FS_FAT_SEC_BYTE_POS *p_entry_start_pos,
                                      RTOS_ERR            *p_err)
{
  FS_FAT_ENTRY_INFO                       fat_entry_info;
  FS_FAT_SEC_BYTE_POS                     dot_dot_entry_end_pos;
  FS_FAT_SEC_BYTE_POS                     lookup_pos;
  FS_FAT_ENTRY_FIND_BY_FIRST_CLUS_CB_DATA find_data;
  FS_FAT_CLUS_NBR                         target_first_clus;
  CPU_BOOLEAN                             entry_found;

  WITH_SCOPE_BEGIN(p_err) {
    LOG_VRB(("Resolving '..' entry."));

    if ((p_entry_start_pos->SecNbr == 0u)
        && (p_entry_start_pos->SecOffset == 0u)) {
      LOG_VRB(("Root dir is parent of itself: resolved."));
      return;
    }

    BREAK_ON_ERR(FS_FAT_EntryParse(p_fat_vol,
                                   p_entry_start_pos,
                                   &dot_dot_entry_end_pos,
                                   &fat_entry_info,
                                   DEF_NULL,
                                   DEF_NULL,
                                   0u,
                                   p_err));

    target_first_clus = fat_entry_info.FirstClusNbr;

    LOG_VRB(("'..' entry's dir tbl starts at clus ", (u)target_first_clus, "."));

    if (target_first_clus == 0u) {
      p_entry_start_pos->SecNbr = 0u;
      p_entry_start_pos->SecOffset = 0u;
      LOG_VRB(("'..' entry is root dir: resolved."));
    } else {
      lookup_pos.SecNbr = FS_FAT_CLUS_TO_SEC(p_fat_vol, target_first_clus);
      lookup_pos.SecOffset = 0u;
      BREAK_ON_ERR(entry_found = FS_FAT_EntryFind(p_fat_vol,
                                                  "..",
                                                  &fat_entry_info,
                                                  &lookup_pos,
                                                  &dot_dot_entry_end_pos,
                                                  p_err));

      ASSERT_BREAK(entry_found, RTOS_ERR_VOL_CORRUPTED);

      if (fat_entry_info.FirstClusNbr == 0u) {
        lookup_pos.SecNbr = p_fat_vol->RootDirStart;
        LOG_VRB(("'..' entry is root dir: resolved."));
      } else {
        lookup_pos.SecNbr = FS_FAT_CLUS_TO_SEC(p_fat_vol, fat_entry_info.FirstClusNbr);
      }
      lookup_pos.SecOffset = 0u;

      LOG_VRB(("Finding entry that matches '..' entry's dir tbl start clus."));

      find_data.EntryFound = DEF_NO;
      find_data.FirstClusNbr = target_first_clus;
      find_data.StartDirentPos = FS_FAT_VoidSecBytePos;

      BREAK_ON_ERR(FS_FAT_DirTblBrowseAcquire(p_fat_vol,
                                              &lookup_pos,
                                              FS_FAT_EntryFindByFirstClusCb,
                                              &find_data,
                                              FS_CACHE_BLK_GET_MODE_RD,
                                              DEF_NULL,
                                              p_err));

      ASSERT_BREAK(find_data.EntryFound, RTOS_ERR_VOL_CORRUPTED);

      LOG_VRB(("Entry found at sec ", (u)find_data.StartDirentPos.SecNbr,
               ", offset ", (u)find_data.StartDirentPos.SecOffset, "."));

      *p_entry_start_pos = find_data.StartDirentPos;
    }
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                       FS_FAT_EntryFindByNameCb()
 *
 * @brief    Find entry by name callback.
 *
 * @param    p_dte       Pointer to the current directory entry.
 *
 * @param    p_dte_pos   Pointer to the current directory entry's position.
 *
 * @param    p_dte_info  Pointer to directory entry's information.
 *
 * @param    p_arg       Pointer to argument passed to callback.
 *
 * @return   Browse outcome.
 *
 * @note     (1)  When using LFN, the first directory entry parsed always corresponds to the last entry
 *                for that file. The sequence number of the entry can be used to validate the stored file
 *                name length against the file name being searched for (i.e. If the last sequence number
 *                is 1, the filename stored in the entry has, at most, 13 characters. There is therefore
 *                no point in comparing file names if the target file name has more than 13 characters.
 *******************************************************************************************************/
static FS_FAT_DIRENT_BROWSE_OUTCOME FS_FAT_EntryFindByNameCb(void                *p_dte,
                                                             FS_FAT_SEC_BYTE_POS *p_dte_pos,
                                                             void                *p_dte_info,
                                                             void                *p_arg)
{
  FS_FAT_ENTRY_FIND_BY_NAME_CB_DATA *p_find_data;
  FS_FAT_SFN_DTE_INFO               *p_sfn_dte_info;
  CPU_INT08U                        *p_dirent_08;
  CPU_INT32U                        name_word;
  FS_FAT_DTE_TYPE                   dte_type;

  p_find_data = (FS_FAT_ENTRY_FIND_BY_NAME_CB_DATA *)p_arg;
  dte_type = *((FS_FAT_DTE_TYPE *)p_dte_info);

#if (FS_FAT_CFG_LFN_EN == DEF_ENABLED)
  if (dte_type == FS_FAT_DTE_TYPE_LFN) {                        // ----------------------- LFN ------------------------
    FS_FAT_LFN_DTE_INFO *p_lfn_dte_info;
    CPU_CHAR            *p_name_substr;
    FS_FILE_NAME_LEN    lfn_pos;

    p_lfn_dte_info = (FS_FAT_LFN_DTE_INFO *)p_dte_info;

    if (p_lfn_dte_info->IsLast) {
      //                                                          See note #1.
      lfn_pos = (p_lfn_dte_info->SeqNo * FS_FAT_DIRENT_LFN_NBR_CHARS);
      p_name_substr = FS_FAT_LFN_PathSegPosGet(p_find_data->NamePtr, lfn_pos);
      p_find_data->HasNameMatch = ((p_name_substr == DEF_NULL) || (*p_name_substr == '\0'));
      p_find_data->StartDirentPos = *p_dte_pos;                // First LFN entry contains name's last characters.
    }

    if (p_find_data->HasNameMatch) {
      lfn_pos = (p_lfn_dte_info->SeqNo - 1u) * FS_FAT_DIRENT_LFN_NBR_CHARS;
      p_name_substr = FS_FAT_LFN_PathSegPosGet(p_find_data->NamePtr, lfn_pos);
      if (p_name_substr == DEF_NULL) {
        p_find_data->HasNameMatch = DEF_NO;
      } else {
        p_find_data->HasNameMatch = FS_FAT_LFN_NameParseCmp(p_dte, p_name_substr);
      }
    }

    return (FS_FAT_DIRENT_BROWSE_OUTCOME_CONTINUE);
  }
#endif

  if (dte_type == FS_FAT_DTE_TYPE_SFN) {                        // ----------------------- SFN ------------------------
    p_sfn_dte_info = (FS_FAT_SFN_DTE_INFO *)p_dte_info;

    if (p_find_data->HasNameMatch) {
      p_find_data->EntryFound = DEF_YES;
    } else if (p_find_data->Name83Ptr != DEF_NULL) {
      p_dirent_08 = (CPU_INT08U *)p_dte;
      name_word = MEM_VAL_GET_INT32U_LITTLE((void *)(p_dirent_08 + 0u));
      if (name_word == p_find_data->Name83Ptr[0]) {
        name_word = MEM_VAL_GET_INT32U_LITTLE((void *)(p_dirent_08 + 4u));
        if (name_word == p_find_data->Name83Ptr[1]) {
          name_word = MEM_VAL_GET_INT32U_LITTLE((void *)(p_dirent_08 + 8u));
          name_word &= 0x00FFFFFFu;
          if (name_word == p_find_data->Name83Ptr[2]) {
            p_find_data->EntryFound = DEF_YES;
          }
        }
      }
    }

    if (p_find_data->EntryFound) {
      if (!p_sfn_dte_info->HasLFN) {
        p_find_data->StartDirentPos = *p_dte_pos;
      }
      p_find_data->EndDirentPos = *p_dte_pos;
      if (p_find_data->EntryInfoPtr != DEF_NULL) {
        FS_FAT_SFN_DirEntryParse((CPU_INT08U *)p_dte, p_find_data->EntryInfoPtr);
      }
      return (FS_FAT_DIRENT_BROWSE_OUTCOME_STOP);
    }

    return (FS_FAT_DIRENT_BROWSE_OUTCOME_CONTINUE);
  }
  //                                                               -------------- OTHER THAN SFN OR LFN ---------------
  return (FS_FAT_DIRENT_BROWSE_OUTCOME_CONTINUE);
}

/****************************************************************************************************//**
 *                                           FS_FAT_EntryFindBySFNCb()
 *
 * @brief    Find entry by short file name callback.
 *
 * @param    p_dte       Pointer to the current directory entry.
 *
 * @param    p_dte_pos   Pointer to the current directory entry's position.
 *
 * @param    p_dte_info  Pointer to directory entry's information.
 *
 * @param    p_arg       Pointer to argument passed to callback.
 *
 * @return   Browse outcome.
 *******************************************************************************************************/

#if (FS_FAT_CFG_LFN_EN == DEF_ENABLED)
static FS_FAT_DIRENT_BROWSE_OUTCOME FS_FAT_EntryFindBySFNCb(void                *p_dte,
                                                            FS_FAT_SEC_BYTE_POS *p_dte_pos,
                                                            void                *p_dte_info,
                                                            void                *p_arg)
{
  FS_FAT_ENTRY_FIND_BY_NAME_CB_DATA *p_find_data;
  CPU_INT08U                        *p_dirent_08;
  CPU_INT32U                        name_word;

  PP_UNUSED_PARAM(p_dte_pos);
  PP_UNUSED_PARAM(p_dte_info);

  if (*((FS_FAT_DTE_TYPE *)p_arg) != FS_FAT_DTE_TYPE_SFN) {
    return (FS_FAT_DIRENT_BROWSE_OUTCOME_CONTINUE);
  }

  p_find_data = (FS_FAT_ENTRY_FIND_BY_NAME_CB_DATA *)p_arg;

  if (p_find_data->Name83Ptr != DEF_NULL) {
    p_dirent_08 = (CPU_INT08U *)p_dte;
    name_word = MEM_VAL_GET_INT32U_LITTLE((void *)(p_dirent_08 + 0u));
    if (name_word == p_find_data->Name83Ptr[0]) {
      name_word = MEM_VAL_GET_INT32U_LITTLE((void *)(p_dirent_08 + 4u));
      if (name_word == p_find_data->Name83Ptr[1]) {
        name_word = MEM_VAL_GET_INT32U_LITTLE((void *)(p_dirent_08 + 8u));
        name_word &= 0x00FFFFFFu;
        if (name_word == p_find_data->Name83Ptr[2]) {
          p_find_data->HasNameMatch = DEF_YES;
          return (FS_FAT_DIRENT_BROWSE_OUTCOME_STOP);
        }
      }
    }
  }
  return (FS_FAT_DIRENT_BROWSE_OUTCOME_CONTINUE);
}
#endif

/****************************************************************************************************//**
 *                                           FS_FAT_EntryParseCb()
 *
 * @brief    Entry parse callback.
 *
 * @param    p_dte       Pointer to the current directory entry.
 *
 * @param    p_dte_pos   Pointer to the current directory entry's position.
 *
 * @param    p_dte_info  Pointer to directory entry's information.
 *
 * @param    p_arg       Pointer to argument passed to callback.
 *
 * @return   Browse outcome.
 *
 * @note     (1) LFN entries for a file or directory are listed before the SFN (Short File Name) entry
 *               and are in reverse order. See Note #1 in FS_FAT_DteAcquireCb() for more details.
 *
 * @note     (2) This Str_Copy() moves the file or directory name (retrieved from the various LFN
 *               entries) from the end to the beginning of the buffer. It relies on the fact that
 *               Str_Copy() can perform overlapping string copies, as long as the destination buffer
 *               is located at a lower address than the source buffer. @n
 *               @verbatim
 *               Buffer
 *               +-------------------------------------------+
 *               |            <-- <long file name characters>|   (1) Before Str_Copy()
 *               +-------------------------------------------+
 *               |<long file name characters>                |   (2) After  Str_Copy()
 *               +-------------------------------------------+
 *               @endverbatim
 *******************************************************************************************************/
static FS_FAT_DIRENT_BROWSE_OUTCOME FS_FAT_EntryParseCb(void                *p_dte,
                                                        FS_FAT_SEC_BYTE_POS *p_dte_pos,
                                                        void                *p_dte_info,
                                                        void                *p_arg)
{
  FS_FAT_ENTRY_PARSE_CB_DATA *p_parse_data;
  FS_FAT_DTE_TYPE            dte_type;

  dte_type = *((FS_FAT_DTE_TYPE *)p_dte_info);
  p_parse_data = (FS_FAT_ENTRY_PARSE_CB_DATA *)p_arg;

  //                                                               ----------------------- SFN ------------------------
  if (dte_type == FS_FAT_DTE_TYPE_SFN) {
    FS_FAT_SFN_DTE_INFO *p_sfn_dte_info;

    p_sfn_dte_info = (FS_FAT_SFN_DTE_INFO *)p_dte_info;

    if (!p_sfn_dte_info->HasLFN) {
      p_parse_data->NameLen = FS_FAT_SFN_NameParse(p_dte, p_parse_data->NamePtr);
    }

    if (p_parse_data->EntryInfoPtr != DEF_NULL) {
      FS_FAT_SFN_DirEntryParse((CPU_INT08U *)p_dte,
                               p_parse_data->EntryInfoPtr);
    }

    if (!p_sfn_dte_info->HasLFN) {
      p_parse_data->StartDirentPos = *p_dte_pos;
    }

    p_parse_data->EndDirentPos = *p_dte_pos;
    p_parse_data->Done = DEF_YES;

    return (FS_FAT_DIRENT_BROWSE_OUTCOME_STOP);
    //                                                             ----------------------- LFN ------------------------
  }

#if (FS_FAT_CFG_LFN_EN == DEF_ENABLED)
  if (dte_type == FS_FAT_DTE_TYPE_LFN) {
    FS_FAT_LFN_DTE_INFO *p_lfn_dte_info;
    FS_FILE_NAME_LEN    nbr_char_in_lfn_entry;

    p_lfn_dte_info = (FS_FAT_LFN_DTE_INFO *)p_dte_info;

    if (p_lfn_dte_info->IsLast) {                               // 1st LFN entry contains name's last char (see Note #1)
      p_parse_data->StartDirentPos = *p_dte_pos;
    }
    //                                                             Get nbr of char contained in cur LFN entry.
    nbr_char_in_lfn_entry = FS_FAT_LFN_MultiByteLenGet(p_dte);
    p_parse_data->NameLen += nbr_char_in_lfn_entry;

    if (p_parse_data->NamePtr != DEF_NULL) {                    // If app buf provided, store char from entry in it.
      if (p_lfn_dte_info->IsLast) {
        nbr_char_in_lfn_entry += 1u;                            // +1 for null-terminated char.
        p_parse_data->CurNamePos = p_parse_data->NameBufSize;   // Position at end of buf.
      }
      //                                                           Ensure enough room in buf to copy LFN char.
      if (p_parse_data->CurNamePos < nbr_char_in_lfn_entry) {
        p_parse_data->Ovf = DEF_YES;
        return (FS_FAT_DIRENT_BROWSE_OUTCOME_STOP);
      }
      //                                                           Copy char from cur LFN entry in buf.
      p_parse_data->CurNamePos -= nbr_char_in_lfn_entry;
      FS_FAT_LFN_NameParse(p_dte, p_parse_data->NamePtr + p_parse_data->CurNamePos);
      if (p_lfn_dte_info->IsLast) {                             // Add null-terminated char in buf if last LFN entry.
        p_parse_data->NamePtr[p_parse_data->CurNamePos + nbr_char_in_lfn_entry - 1u] = 0u;
      }

      if (p_lfn_dte_info->SeqNo == 1u) {                        // LFN entry containing 1st name characters.
        Str_Copy(p_parse_data->NamePtr,                         // See Note #2.
                 p_parse_data->NamePtr + p_parse_data->CurNamePos);
      }
    }
  }
#endif
  //                                                               -------------- OTHER THAN SFN OR LFN ---------------
  return (FS_FAT_DIRENT_BROWSE_OUTCOME_CONTINUE);
}

/****************************************************************************************************//**
 *                                       FS_FAT_EntryFindByFirstClusCb()
 *
 * @brief    Find entry by first cluster callback.
 *
 * @param    p_dte       Pointer to the current directory entry.
 *
 * @param    p_dte_pos   Pointer to the current directory entry's position.
 *
 * @param    p_dte_info  Pointer to directory entry's information.
 *
 * @param    p_arg       Pointer to argument passed to callback.
 *
 * @return   Browse outcome.
 *******************************************************************************************************/
static FS_FAT_DIRENT_BROWSE_OUTCOME FS_FAT_EntryFindByFirstClusCb(void                *p_dte,
                                                                  FS_FAT_SEC_BYTE_POS *p_dte_pos,
                                                                  void                *p_dte_info,
                                                                  void                *p_arg)
{
  FS_FAT_ENTRY_FIND_BY_FIRST_CLUS_CB_DATA *p_find_data;
  FS_FAT_LFN_DTE_INFO                     *p_lfn_dte_info;
  FS_FAT_SFN_DTE_INFO                     *p_sfn_dte_info;
  FS_FAT_DTE_TYPE                         dte_type;
  FS_FAT_CLUS_NBR                         first_clus_nbr;

  p_find_data = (FS_FAT_ENTRY_FIND_BY_FIRST_CLUS_CB_DATA *)p_arg;
  dte_type = *((FS_FAT_DTE_TYPE *)p_dte_info);

  switch (dte_type) {
    case FS_FAT_DTE_TYPE_ERASED:
    case FS_FAT_DTE_TYPE_FREE:
    case FS_FAT_DTE_TYPE_INVALID_LFN:
      return (FS_FAT_DIRENT_BROWSE_OUTCOME_STOP);
    case FS_FAT_DTE_TYPE_LFN:
      p_lfn_dte_info = (FS_FAT_LFN_DTE_INFO *)p_dte_info;
      if (p_lfn_dte_info->IsLast) {
        p_find_data->StartDirentPos = *p_dte_pos;
      }
    //                                                             fallthrough
    case FS_FAT_DTE_TYPE_VOL_LABEL:
      return (FS_FAT_DIRENT_BROWSE_OUTCOME_CONTINUE);
    case FS_FAT_DTE_TYPE_SFN:
      p_sfn_dte_info = (FS_FAT_SFN_DTE_INFO *)p_dte_info;
      if (!p_sfn_dte_info->HasLFN) {
        p_find_data->StartDirentPos = *p_dte_pos;
      }
      break;
    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, FS_FAT_DIRENT_BROWSE_OUTCOME_STOP);
  }

  first_clus_nbr = FS_FAT_DIRENT_FIRST_CLUS_GET(p_dte);

  if (first_clus_nbr == p_find_data->FirstClusNbr) {
    p_find_data->EntryFound = DEF_YES;
    return (FS_FAT_DIRENT_BROWSE_OUTCOME_STOP);
  }
  return (FS_FAT_DIRENT_BROWSE_OUTCOME_CONTINUE);
}

/****************************************************************************************************//**
 *                                       FS_FAT_DirEntriesEmptyFindCb()
 *
 * @brief    Free or deleted directory entries find callback.
 *
 * @param    p_dte       Pointer to the current directory entry.
 *
 * @param    p_dte_pos   Pointer to the current directory entry's position.
 *
 * @param    p_dte_info  Pointer to directory entry's information.
 *
 * @param    p_arg       Pointer to argument passed to callback.
 *
 * @return   Browse outcome.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static FS_FAT_DIRENT_BROWSE_OUTCOME FS_FAT_DirEntriesEmptyFindCb(void                *p_dte,
                                                                 FS_FAT_SEC_BYTE_POS *p_dte_pos,
                                                                 void                *p_dte_info,
                                                                 void                *p_arg)
{
  FS_FAT_DIRENT_EMPTY_GET_CB_DATA *p_dirent_empty_get_data;
  FS_FAT_DTE_TYPE                 dte_type;
  FS_FAT_DIRENT_BROWSE_OUTCOME    outcome;

  PP_UNUSED_PARAM(p_dte);

  dte_type = *((FS_FAT_DTE_TYPE *)p_dte_info);
  p_dirent_empty_get_data = (FS_FAT_DIRENT_EMPTY_GET_CB_DATA *)p_arg;
  outcome = FS_FAT_DIRENT_BROWSE_OUTCOME_STOP;

  p_dirent_empty_get_data->LastDirEntryPos = *p_dte_pos;

  switch (dte_type) {
    case FS_FAT_DTE_TYPE_LFN:
    case FS_FAT_DTE_TYPE_SFN:
    case FS_FAT_DTE_TYPE_VOL_LABEL:
      p_dirent_empty_get_data->FoundDirEntryCnt = 0u;
      outcome = FS_FAT_DIRENT_BROWSE_OUTCOME_CONTINUE;
      break;
    case FS_FAT_DTE_TYPE_FREE:
    case FS_FAT_DTE_TYPE_ERASED:
    case FS_FAT_DTE_TYPE_INVALID_LFN:
      if (p_dirent_empty_get_data->FoundDirEntryCnt == 0u) {
        p_dirent_empty_get_data->FirstDirEntryPos = *p_dte_pos;
      }
      p_dirent_empty_get_data->FoundDirEntryCnt++;
      if (p_dirent_empty_get_data->DirEntryCnt == p_dirent_empty_get_data->FoundDirEntryCnt) {
        outcome = FS_FAT_DIRENT_BROWSE_OUTCOME_STOP;
      } else {
        outcome = FS_FAT_DIRENT_BROWSE_OUTCOME_CONTINUE;
      }
      break;
    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, FS_FAT_DIRENT_BROWSE_OUTCOME_STOP);
  }

  return (outcome);
}
#endif

/****************************************************************************************************//**
 *                                       FS_FAT_DirEntriesEraseCb()
 *
 * @brief    Directory entries deletion callback.
 *
 * @param    p_fat_vol   Pointer to a FAT volume.
 *
 * @param    p_dte_pos   Pointer to the current directory entry's position.
 *
 * @param    p_arg       Pointer to argument passed to callback.
 *
 * @param    p_err       Error pointer.
 *
 * @return   Browse outcome.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static FS_FAT_DIRENT_BROWSE_OUTCOME FS_FAT_DirEntriesEraseCb(FS_FAT_VOL          *p_fat_vol,
                                                             FS_FAT_SEC_BYTE_POS *p_dte_pos,
                                                             void                *p_arg,
                                                             RTOS_ERR            *p_err)
{
  CPU_INT08U                   *p_dirent_08;
  CPU_INT08U                   *p_buf;
  FS_FAT_DTE_ERASE_CB_DATA     *p_erase_data;
  FS_FAT_DIRENT_BROWSE_OUTCOME outcome;

  WITH_SCOPE_BEGIN(p_err) {
    p_erase_data = (FS_FAT_DTE_ERASE_CB_DATA *)p_arg;
    outcome = FS_FAT_DIRENT_BROWSE_OUTCOME_STOP;

    BREAK_ON_ERR(FS_VOL_CACHE_BLK_RW) (&p_fat_vol->Vol,
                                       p_dte_pos->SecNbr,
                                       FS_FAT_LB_TYPE_DIRENT,
                                       *p_erase_data->EntryCacheWrJobHandlePtr,
                                       &p_buf,
                                       p_erase_data->EntryCacheWrJobHandlePtr,
                                       p_err) {
      p_dirent_08 = p_buf + p_dte_pos->SecOffset;

#if (FS_CORE_CFG_DBG_MEM_CLR_EN == DEF_ENABLED)
      Mem_Clr(p_dirent_08, FS_FAT_SIZE_DIR_ENTRY);
#endif
      p_dirent_08[0] = FS_FAT_DIRENT_NAME_ERASED_AND_FREE;      // Mark entry as available.

      if ((p_dte_pos->SecNbr == p_erase_data->EndDtePos.SecNbr)
          && (p_dte_pos->SecOffset == p_erase_data->EndDtePos.SecOffset)) {
        outcome = FS_FAT_DIRENT_BROWSE_OUTCOME_STOP;
      } else {
        outcome = FS_FAT_DIRENT_BROWSE_OUTCOME_CONTINUE;
      }
    }
  } WITH_SCOPE_END

  return (outcome);
}
#endif

/********************************************************************************************************
 *                                       FS_FAT_DirEntriesEmptyGet()
 *
 * Description : Find free or deleted directory entries.
 *
 * Argument(s) : p_fat_vol               Pointer to a FAT volume.
 *
 *               dte_cnt                 Number of directory entries to find.
 *
 *               start_sec_nbr           Sector to start the lookup at.
 *
 *               p_dte_start_pos         Pointer to a variable that will receive the first empty
 *                                       directory entry position.
 *
 *               p_dte_end_pos           Pointer to a variable that will receive the last empty
 *                                       directory entry position.
 *
 *               p_entry_wr_job_handle   Pointer to cache job handle.
 *
 *               p_err                   Error pointer.
 *
 * Return(s)   : none.
 *
 * Note(s)     : (1) If the number of empty directory entries found is less than required, the last
 *                   directory entry must be located at the end of a cluster. Therefore, no available
 *                   free space must be accounted for when calculating the size of the on-disk space to be
 *                   allocated.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_FAT_DirEntriesEmptyGet(FS_FAT_VOL             *p_fat_vol,
                                      FS_FAT_DIR_ENTRY_QTY   dte_cnt,
                                      FS_FAT_SEC_NBR         start_sec_nbr,
                                      FS_FAT_SEC_BYTE_POS    *p_dte_start_pos,
                                      FS_FAT_SEC_BYTE_POS    *p_dte_end_pos,
                                      FS_CACHE_WR_JOB_HANDLE *p_entry_wr_job_handle,
                                      RTOS_ERR               *p_err)
{
  FS_FAT_DIRENT_EMPTY_GET_CB_DATA cb_data;
  FS_FAT_DIR_ENTRY_QTY            new_dirent_cnt;
  FS_FAT_SEC_NBR                  sec_alloc_cnt;
  FS_FAT_CLUS_NBR                 clus_alloc_cnt;
  FS_FAT_CLUS_NBR                 cur_clus;
  FS_FAT_CLUS_NBR                 last_clus;
  FS_FAT_CLUS_NBR                 first_clus;
  CPU_SIZE_T                      alloc_size_octets;
  FS_FAT_SEC_BYTE_POS             lookup_pos;
  CPU_INT08U                      lb_size_log2;

  WITH_SCOPE_BEGIN(p_err) {
    LOG_VRB(("Getting ", (u)dte_cnt, " empty dir entries starting at sec ", (u)start_sec_nbr, "."));

    RTOS_ASSERT_CRITICAL((dte_cnt > 0u), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

    BREAK_ON_ERR(lb_size_log2 = FSBlkDev_LbSizeLog2Get(p_fat_vol->Vol.BlkDevHandle, p_err));

    cb_data.DirEntryCnt = dte_cnt;
    cb_data.FoundDirEntryCnt = 0u;
    cb_data.FirstDirEntryPos.SecNbr = 0u;
    cb_data.FirstDirEntryPos.SecOffset = 0u;
    cb_data.LastDirEntryPos.SecNbr = 0u;
    cb_data.LastDirEntryPos.SecOffset = 0u;

    lookup_pos.SecNbr = start_sec_nbr;
    lookup_pos.SecOffset = 0u;
    BREAK_ON_ERR(FS_FAT_DirTblBrowseAcquire(p_fat_vol,
                                            &lookup_pos,
                                            FS_FAT_DirEntriesEmptyFindCb,
                                            &cb_data,
                                            FS_CACHE_BLK_GET_MODE_RD,
                                            DEF_NULL,
                                            p_err));

    LOG_VRB(("Found ", (u)cb_data.FoundDirEntryCnt, " out of ", (u)dte_cnt, "."));

    if (cb_data.DirEntryCnt != cb_data.FoundDirEntryCnt) {      // See Note #1.
      RTOS_ASSERT_CRITICAL((cb_data.LastDirEntryPos.SecOffset == (FS_UTIL_PWR2(lb_size_log2) - FS_UTIL_PWR2(FS_FAT_SIZE_DIR_ENTRY_LOG2))),
                           RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
    }
    RTOS_ASSERT_CRITICAL((cb_data.LastDirEntryPos.SecNbr != 0u), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

    new_dirent_cnt = dte_cnt - cb_data.FoundDirEntryCnt;
    if (new_dirent_cnt > 0u) {
      LOG_VRB(("Must allocate space for ", (u)new_dirent_cnt, " new dir entries."));
      //                                                           No dir entries allowed beyond FAT12/16 initial root
      //                                                           dir size.
      ASSERT_BREAK(((p_fat_vol->FAT_Type != FS_FAT_TYPE_FAT12)
                    && (p_fat_vol->FAT_Type != FS_FAT_TYPE_FAT16))
                   || (start_sec_nbr != p_fat_vol->RootDirStart),
                   RTOS_ERR_DIR_FULL);
      //                                                           Find space (in octets) that needs to be alloc'd
      //                                                           (see Note #1).
      alloc_size_octets = FS_UTIL_MULT_PWR2(new_dirent_cnt, FS_FAT_SIZE_DIR_ENTRY_LOG2);

      //                                                           Find nbr of sec's to be alloc'd (see Note #1).
      sec_alloc_cnt = FS_UTIL_DIV_PWR2(alloc_size_octets, lb_size_log2);
      sec_alloc_cnt += FS_UTIL_MODULO_PWR2(alloc_size_octets, lb_size_log2) == 0u ? 0u : 1u;

      //                                                           Find nbr of clus's to be alloc'd.
      clus_alloc_cnt = FS_UTIL_DIV_PWR2(sec_alloc_cnt, p_fat_vol->SecPerClusLog2);
      clus_alloc_cnt += FS_UTIL_MODULO_PWR2(sec_alloc_cnt, p_fat_vol->SecPerClusLog2) == 0u ? 0u : 1u;

      //                                                           Find clus to start alloc from.
      cur_clus = FS_FAT_SEC_TO_CLUS(p_fat_vol, cb_data.LastDirEntryPos.SecNbr);

      BREAK_ON_ERR(first_clus = FS_FAT_ClusChainAlloc(p_fat_vol,
                                                      cur_clus,
                                                      &last_clus,
                                                      clus_alloc_cnt,
                                                      DEF_YES,
                                                      p_entry_wr_job_handle,
                                                      FS_FAT_LB_TYPE_JNL_DIRENT,
                                                      p_err));
      if (cb_data.FoundDirEntryCnt == 0u) {
        p_dte_start_pos->SecNbr = FS_FAT_CLUS_TO_SEC(p_fat_vol, first_clus);
        p_dte_start_pos->SecOffset = 0u;
      } else {
        *p_dte_start_pos = cb_data.FirstDirEntryPos;
      }
      p_dte_end_pos->SecNbr = FS_FAT_CLUS_TO_SEC(p_fat_vol, last_clus) + FS_UTIL_MODULO_PWR2(sec_alloc_cnt - 1u, p_fat_vol->SecPerClusLog2);
      p_dte_end_pos->SecOffset = FS_UTIL_MODULO_PWR2(alloc_size_octets - FS_FAT_SIZE_DIR_ENTRY, lb_size_log2);
    } else {
      *p_dte_start_pos = cb_data.FirstDirEntryPos;
      *p_dte_end_pos = cb_data.LastDirEntryPos;
    }
  } WITH_SCOPE_END
}
#endif

/****************************************************************************************************//**
 *                                           FS_FAT_SFN_NameChk()
 *
 * @brief    Check whether file name is valid SFN. Does not handle  'dot' and 'dot dot' path components
 *           which must be checked separately.
 *
 * @param    p_name  Pointer to the path component to be checked.
 *
 * @return   Flags (possibly OR'd) among the following:
 *
 *               FS_FAT_FN_CHK_FLAGS_HAS_INVALID_CHAR
 *               FS_FAT_FN_CHK_FLAGS_EXT_IS_LOWER_CASE
 *               FS_FAT_FN_CHK_FLAGS_NAME_IS_LOWER_CASE
 *               FS_FAT_FN_CHK_FLAGS_IS_MIXED_CASE
 *               FS_FAT_FN_CHK_FLAGS_NAME_IS_TOO_LONG
 *               FS_FAT_FN_CHK_FLAGS_EXT_IS_TOO_LONG
 *******************************************************************************************************/
static FS_FLAGS FS_FAT_SFN_NameChk(CPU_CHAR *p_name)
{
  FS_FILE_NAME_LEN len;
  CPU_CHAR         name_char;
  FS_FLAGS         name_chk_flags;
  CPU_BOOLEAN      has_period;
  CPU_BOOLEAN      has_upper;
  CPU_BOOLEAN      ext_lower_case;
  CPU_BOOLEAN      name_lower_case;
  CPU_BOOLEAN      lower;
  CPU_BOOLEAN      upper;

  has_period = DEF_NO;
  has_upper = DEF_NO;
  ext_lower_case = DEF_NO;
  name_lower_case = DEF_NO;
  len = 0u;
  name_chk_flags = FS_FAT_FN_CHK_FLAGS_NONE;

  //                                                               ------------------ CHK INIT CHAR -------------------
  name_char = *p_name;                                          // Rtn err if first char is '.', ' ', '\0', '\'.
  switch (name_char) {
    case ASCII_CHAR_FULL_STOP:
    case ASCII_CHAR_SPACE:
    case ASCII_CHAR_NULL:
    case FS_CHAR_PATH_SEP:
    case FS_CHAR_PATH_SEP_ALT:
      DEF_BIT_SET(name_chk_flags, FS_FAT_FN_CHK_FLAGS_HAS_INVALID_CHAR);
      return (name_chk_flags);
    default:
      break;
  }

  len += 1u;

  //                                                               ------------------- PROCESS PATH -------------------
  while ((name_char != ASCII_CHAR_NULL)
         && !FS_CHAR_IS_PATH_SEP(name_char)) {
    if (name_char == ASCII_CHAR_FULL_STOP) {
      if (has_period) {
        DEF_BIT_SET(name_chk_flags, FS_FAT_FN_CHK_FLAGS_HAS_INVALID_CHAR);
        return (name_chk_flags);
      }
      has_period = DEF_YES;
      has_upper = DEF_NO;
      len = 0u;
    } else {
      if ((has_period == DEF_NO) && (len > FS_FAT_SFN_NAME_MAX_NBR_CHAR)) {
        DEF_BIT_SET(name_chk_flags, FS_FAT_FN_CHK_FLAGS_NAME_IS_TOO_LONG);
        return (name_chk_flags);
      }
      if ((has_period == DEF_YES) && (len > FS_FAT_SFN_EXT_MAX_NBR_CHAR)) {
        DEF_BIT_SET(name_chk_flags, FS_FAT_FN_CHK_FLAGS_EXT_IS_TOO_LONG);
        return (name_chk_flags);
      }

      if (!FS_FAT_IS_LEGAL_SFN_CHAR(name_char)) {
        DEF_BIT_SET(name_chk_flags, FS_FAT_FN_CHK_FLAGS_HAS_INVALID_CHAR);
        return (name_chk_flags);
      }

      lower = ASCII_IS_LOWER(name_char);
      if (lower == DEF_YES) {
        if (has_upper == DEF_YES) {
          DEF_BIT_SET(name_chk_flags, FS_FAT_FN_CHK_FLAGS_IS_MIXED_CASE);
        } else {
          if (has_period == DEF_YES) {
            ext_lower_case = DEF_YES;
          } else {
            name_lower_case = DEF_YES;
          }
        }
      } else {
        upper = ASCII_IS_UPPER(name_char);
        if (upper == DEF_YES) {
          has_upper = DEF_YES;
          if (((has_period == DEF_YES) && (ext_lower_case == DEF_YES))
              || ((has_period == DEF_NO)  && (name_lower_case == DEF_YES))) {
            DEF_BIT_SET(name_chk_flags, FS_FAT_FN_CHK_FLAGS_IS_MIXED_CASE);
          }
        }
      }
    }
    len += 1u;
    p_name += 1u;;
    name_char = *p_name;
  }

  if (ext_lower_case) {
    DEF_BIT_SET(name_chk_flags, FS_FAT_FN_CHK_FLAGS_EXT_IS_LOWER_CASE);
  }
  if (name_lower_case) {
    DEF_BIT_SET(name_chk_flags, FS_FAT_FN_CHK_FLAGS_NAME_IS_LOWER_CASE);
  }

  return (name_chk_flags);
}

/****************************************************************************************************//**
 *                                           FS_FAT_SFN_DirEntryFmt()
 *
 * @brief    Make directory entry for SFN entry.
 *
 * @param    p_dir_entry         Pointer to directory entry.
 *
 * @param    p_name_8_3          8.3 entry's name.
 *
 * @param    is_dir              Indicates whether entry is for directory :
 *                               DEF_TRUE    Entry is for directory.
 *                               DEF_FALSE   Entry is for file.
 *
 * @param    file_first_clus     First cluster in file.
 *
 * @param    name_lower_case     Indicates whether name characters will be returned in lower case :
 *                               DEF_YES, name characters will be returned in lower case.
 *                               DEF_NO,  name characters will be returned in upper case.
 *
 * @param    ext_lower_case      Indicates whether extension characters will be returned in lower case :
 *                               DEF_YES, extension characters will be returned in lower case.
 *                               DEF_NO,  extension characters will be returned in upper case.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_FAT_SFN_DirEntryFmt(void            *p_dir_entry,
                                   CPU_INT32U      *p_name_8_3,
                                   CPU_BOOLEAN     is_dir,
                                   FS_FAT_CLUS_NBR file_first_clus,
                                   CPU_BOOLEAN     name_lower_case,
                                   CPU_BOOLEAN     ext_lower_case)
{
  CPU_INT08U           *p_dir_entry_08;
  CPU_INT16U           date_val;
  sl_sleeptimer_date_t stime;
  CPU_INT08U           ntres;
  CPU_INT16U           time_val;
  sl_status_t          status;

  status = sl_sleeptimer_get_datetime(&stime);                   // Get date/time.
  if (status == SL_STATUS_OK) {
    time_val = FS_FAT_TimeFmt(&stime);
    date_val = FS_FAT_DateFmt(&stime);
  } else {
    time_val = 0u;
    date_val = 0u;
  }

  p_dir_entry_08 = (CPU_INT08U *)p_dir_entry;

  //                                                               Octets 0-10: SFN.
  MEM_VAL_SET_INT32U_LITTLE((void *)(p_dir_entry_08 + 0u), p_name_8_3[0]);
  MEM_VAL_SET_INT32U_LITTLE((void *)(p_dir_entry_08 + 4u), p_name_8_3[1]);
  MEM_VAL_SET_INT32U_LITTLE((void *)(p_dir_entry_08 + 8u), p_name_8_3[2]);
  p_dir_entry_08 += 11u;

  //                                                               Octet  11:    Attrib.
  if (is_dir == DEF_YES) {
    *p_dir_entry_08 = FS_FAT_DIRENT_ATTR_DIRECTORY;
    p_dir_entry_08++;
  } else {
    *p_dir_entry_08 = FS_FAT_DIRENT_ATTR_NONE;
    p_dir_entry_08++;
  }

  //                                                               12:    NT reserved info.
  ntres = DEF_BIT_NONE;
  if (name_lower_case == DEF_YES) {
    ntres |= FS_FAT_DIRENT_NTRES_NAME_LOWER_CASE;
  }
  if (ext_lower_case == DEF_YES) {
    ntres |= FS_FAT_DIRENT_NTRES_EXT_LOWER_CASE;
  }
  *p_dir_entry_08 = ntres;
  p_dir_entry_08++;

  *p_dir_entry_08 = 0u;                                         // 13:    Creation time, 10ths of a sec.
  p_dir_entry_08++;
  //                                                               14-15: Creation time.
  *p_dir_entry_08 = (CPU_INT08U)(time_val        >> (DEF_INT_08_NBR_BITS * 0u)) & DEF_INT_08_MASK;
  p_dir_entry_08++;
  *p_dir_entry_08 = (CPU_INT08U)(time_val        >> (DEF_INT_08_NBR_BITS * 1u)) & DEF_INT_08_MASK;
  p_dir_entry_08++;
  //                                                               16-17: Creation date.
  *p_dir_entry_08 = (CPU_INT08U)(date_val        >> (DEF_INT_08_NBR_BITS * 0u)) & DEF_INT_08_MASK;
  p_dir_entry_08++;
  *p_dir_entry_08 = (CPU_INT08U)(date_val        >> (DEF_INT_08_NBR_BITS * 1u)) & DEF_INT_08_MASK;
  p_dir_entry_08++;
  //                                                               18-19: Last access date.
  *p_dir_entry_08 = (CPU_INT08U)(date_val        >> (DEF_INT_08_NBR_BITS * 0u)) & DEF_INT_08_MASK;
  p_dir_entry_08++;
  *p_dir_entry_08 = (CPU_INT08U)(date_val        >> (DEF_INT_08_NBR_BITS * 1u)) & DEF_INT_08_MASK;
  p_dir_entry_08++;
  //                                                               20-21: Hi word, entry's 1st clus nbr.
  *p_dir_entry_08 = (CPU_INT08U)(file_first_clus >> (DEF_INT_08_NBR_BITS * 2u)) & DEF_INT_08_MASK;
  p_dir_entry_08++;
  *p_dir_entry_08 = (CPU_INT08U)(file_first_clus >> (DEF_INT_08_NBR_BITS * 3u)) & DEF_INT_08_MASK;
  p_dir_entry_08++;
  //                                                               22-23: Last write  time.
  *p_dir_entry_08 = (CPU_INT08U)(time_val        >> (DEF_INT_08_NBR_BITS * 0u)) & DEF_INT_08_MASK;
  p_dir_entry_08++;
  *p_dir_entry_08 = (CPU_INT08U)(time_val        >> (DEF_INT_08_NBR_BITS * 1u)) & DEF_INT_08_MASK;
  p_dir_entry_08++;
  //                                                               24-25: Last wrote  date.
  *p_dir_entry_08 = (CPU_INT08U)(date_val        >> (DEF_INT_08_NBR_BITS * 0u)) & DEF_INT_08_MASK;
  p_dir_entry_08++;
  *p_dir_entry_08 = (CPU_INT08U)(date_val        >> (DEF_INT_08_NBR_BITS * 1u)) & DEF_INT_08_MASK;
  p_dir_entry_08++;
  //                                                               26-27: Lo word, entry's 1st clus nbr.
  *p_dir_entry_08 = (CPU_INT08U)(file_first_clus >> (DEF_INT_08_NBR_BITS * 0u)) & DEF_INT_08_MASK;
  p_dir_entry_08++;
  *p_dir_entry_08 = (CPU_INT08U)(file_first_clus >> (DEF_INT_08_NBR_BITS * 1u)) & DEF_INT_08_MASK;
  p_dir_entry_08++;
  *p_dir_entry_08 = 0u;                                         // 28-31: File size in bytes.
  p_dir_entry_08++;
  *p_dir_entry_08 = 0u;
  p_dir_entry_08++;
  *p_dir_entry_08 = 0u;
  p_dir_entry_08++;
  *p_dir_entry_08 = 0u;
}
#endif

/****************************************************************************************************//**
 *                                       FS_FAT_SFN_DirEntryParse()
 *
 * @brief    Parse a directory entry.
 *
 * @param    p_dir_entry         Pointer to the directory entry to be parsed.
 *
 * @param    p_fat_entry_info    Pointer to the entry info data structure to be populated.
 *******************************************************************************************************/
static void FS_FAT_SFN_DirEntryParse(CPU_INT08U        *p_dir_entry,
                                     FS_FAT_ENTRY_INFO *p_fat_entry_info)
{
  p_fat_entry_info->Attrib = FS_FAT_DIRENT_ATTRIB_GET(p_dir_entry);
  p_fat_entry_info->Size = FS_FAT_DIRENT_SIZE_GET(p_dir_entry);
  p_fat_entry_info->FirstClusNbr = FS_FAT_DIRENT_FIRST_CLUS_GET(p_dir_entry);
  p_fat_entry_info->DateAccess = FS_FAT_DIRENT_ACCESS_DATE_GET(p_dir_entry);
  p_fat_entry_info->DateCreate = FS_FAT_DIRENT_CREATE_DATE_GET(p_dir_entry);
  p_fat_entry_info->TimeCreate = FS_FAT_DIRENT_CREATE_TIME_GET(p_dir_entry);
  p_fat_entry_info->DateWr = FS_FAT_DIRENT_WR_DATE_GET(p_dir_entry);
  p_fat_entry_info->TimeWr = FS_FAT_DIRENT_WR_TIME_GET(p_dir_entry);
}

/****************************************************************************************************//**
 *                                           FS_FAT_SFN_NameCreate()
 *
 * @brief    Create 8.3 SFN from path component for comparison with data from volume.
 *
 * @param    p_name      Pointer to SFN-compatible entry name (see Note #1).
 *
 * @param    p_name_8_3  Pointer to a buffer that will receive 8.3 entry's name.
 *
 * @note     (1) The caller MUST check that 'p_name' is a valid 8.3 file name prior to calling this
 *               function.
 *
 * @note     (2) The 8.3 file name, 'p_name_8_3' is maintained as an array of three four-byte
 *               so that file name search comparison may be efficiently executed.
 *
 *               The final byte of the final name -- the byte in the 3rd word of 'p_name_8_3' at
 *               the highest memory locations -- is cleared since the 12th byte of FAT directory
 *               entries is NOT part of the file name.
 *******************************************************************************************************/
static void FS_FAT_SFN_NameCreate(CPU_CHAR   *p_name,
                                  CPU_INT32U *p_name_8_3)
{
  FS_FILE_NAME_LEN i;
  FS_FILE_NAME_LEN len;
  CPU_CHAR         name_char;
  CPU_CHAR         name_8_3_08[12];

  for (i = 0u; i < 11u; i++) {                                  // 'Clear' 8.3 file name buffer.
    name_8_3_08[i] = ASCII_CHAR_SPACE;
  }
  name_8_3_08[11] = ASCII_CHAR_NULL;                            // 'Clear' final byte of 8.3 file name (see Note #2).

  name_char = *p_name;
  len = 0u;
  i = 0u;
  while ((name_char != ASCII_CHAR_NULL)
         && (FS_CHAR_IS_PATH_SEP(name_char) == DEF_NO)) {
    if (name_char == ASCII_CHAR_FULL_STOP) {
      i = FS_FAT_SFN_NAME_MAX_NBR_CHAR;
      len = 0u;
    } else {
      name_8_3_08[i] = ASCII_TO_UPPER(name_char);
      i++;
      len++;
    }
    p_name++;
    name_char = *p_name;
  }

  p_name_8_3[0] = MEM_VAL_GET_INT32U_LITTLE((void *)&name_8_3_08[0]);
  p_name_8_3[1] = MEM_VAL_GET_INT32U_LITTLE((void *)&name_8_3_08[4]);
  p_name_8_3[2] = MEM_VAL_GET_INT32U_LITTLE((void *)&name_8_3_08[8]);
}

/****************************************************************************************************//**
 *                                           FS_FAT_SFN_NameParse()
 *
 * @brief    Read characters of SFN name in SFN entry.
 *
 * @param    p_dir_entry     Pointer to directory entry.
 *
 * @param    p_name          Pointer to buffer that will receive entry name.
 *
 * @note     (1) In an entry that contains a file name, any unused locations in the entry base name and
 *               extension fields should contain the code for a space (0x20).
 *
 * @note     (2) The dot between the entry base name and extension is not stored in the SFN entry (byte 0
 *               to 11). The dot is implicit between byte 7 (last character of base name) and 8 (first
 *               character of extension).
 *******************************************************************************************************/
static FS_FILE_NAME_LEN FS_FAT_SFN_NameParse(void     *p_dir_entry,
                                             CPU_CHAR *p_name)
{
  CPU_INT08U       *p_dir_entry_08;
  FS_FILE_NAME_LEN ix;
  FS_FILE_NAME_LEN len;
  CPU_INT08U       nt_reserved;
  CPU_BOOLEAN      name_lower_case;
  CPU_BOOLEAN      ext_lower_case;
  FS_FILE_NAME_LEN dot_len = 0;
  FS_FILE_NAME_LEN name_len;
  FS_FILE_NAME_LEN ext_len;

  p_dir_entry_08 = (CPU_INT08U *)p_dir_entry;
  //                                                               Set flags to know if base name and/or extension are
  //                                                               lower case or not.
  nt_reserved = MEM_VAL_GET_INT08U_LITTLE((void *)(p_dir_entry_08 + FS_FAT_DIRENT_OFFSET_NTRES));
  name_lower_case = DEF_BIT_IS_SET(nt_reserved, FS_FAT_DIRENT_NTRES_NAME_LOWER_CASE);
  ext_lower_case = DEF_BIT_IS_SET(nt_reserved, FS_FAT_DIRENT_NTRES_EXT_LOWER_CASE);

  //                                                               ---------------- READ SFN BASE NAME ----------------
  for (len = FS_FAT_SFN_NAME_MAX_NBR_CHAR; len > 0u; len--) {   // Compute name length (see Note #1).
    if (p_dir_entry_08[len - 1u] != (CPU_INT08U)ASCII_CHAR_SPACE) {
      break;
    }
  }
  name_len = len;

  if ((len != 0u) && (p_name != DEF_NULL)) {                    // Copy name.
    for (ix = 0u; ix < len; ix++) {
      *p_name = (name_lower_case == DEF_YES) ? ASCII_TO_LOWER((CPU_CHAR)p_dir_entry_08[ix])
                : (CPU_CHAR)p_dir_entry_08[ix];
      p_name++;
    }
  }

  //                                                               ---------------- READ SFN EXTENSION ----------------
  for (len = FS_FAT_SFN_EXT_MAX_NBR_CHAR; len > 0u; len--) {    // Compute entry extension length (see Note #1).
    if (p_dir_entry_08[len + FS_FAT_SFN_NAME_MAX_NBR_CHAR - 1u] != (CPU_INT08U)ASCII_CHAR_SPACE) {
      break;
    }
  }
  ext_len = len;

  if ((len != 0u) && (p_name != DEF_NULL)) {                    // Copy entry extension.
    *p_name = (CPU_CHAR)ASCII_CHAR_FULL_STOP;                   // Dot character between name and extension (Note #2).
    dot_len = 1;
    p_name++;

    for (ix = 0u; ix < len; ix++) {
      *p_name = (ext_lower_case == DEF_YES) ? ASCII_TO_LOWER((CPU_CHAR)p_dir_entry_08[ix + FS_FAT_SFN_NAME_MAX_NBR_CHAR])
                : (CPU_CHAR)p_dir_entry_08[ix + FS_FAT_SFN_NAME_MAX_NBR_CHAR];
      p_name++;
    }
  }

  if (p_name != DEF_NULL) {
    *p_name = (CPU_CHAR)ASCII_CHAR_NULL;                        // End with NULL character.
  }

  len = name_len + dot_len + ext_len;                           // Compute total SFN length.
  return (len);
}

#endif // FS_CORE_CFG_FAT_EN

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_FS_AVAIL
