/***************************************************************************//**
 * @file
 * @brief File System - Fat Journal Operations
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
#include  <common/include/lib_def.h>
#include  <fs_core_cfg.h>

#if ((FS_CORE_CFG_FAT_EN == DEF_ENABLED) \
  && (FS_FAT_CFG_JOURNAL_EN == DEF_ENABLED))

#include  <fs/source/sys/fat/fs_fat_journal_priv.h>
#include  <fs/include/fs_fat.h>
#include  <fs/source/sys/fat/fs_fat_priv.h>
#include  <fs/source/sys/fat/fs_fat_xfn_priv.h>
#include  <fs/source/sys/fat/fs_fat_entry_priv.h>
#include  <fs/source/sys/fat/fs_fat_fatxx_priv.h>

#include  <fs/source/core/fs_core_priv.h>
#include  <fs/source/core/fs_core_file_priv.h>
#include  <fs/source/core/fs_core_vol_priv.h>
#include  <fs/source/core/fs_core_cache_priv.h>

#include  <fs/source/shared/cleanup/cleanup_mgmt_priv.h>
#include  <common/include/lib_mem.h>
#include  <common/source/logging/logging_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH            (FS, FAT, JOURNAL)
#define  RTOS_MODULE_CUR         RTOS_CFG_MODULE_FS

#define  FS_FAT_JOURNAL_MARK_TOP_LVL_ENTER                   0xBBBBu

#define  FS_FAT_JOURNAL_MARK_ENTER                           0x6666u
#define  FS_FAT_JOURNAL_MARK_ENTER_END                       0xDDDDu

#define  FS_FAT_JOURNAL_SIG_CLUS_CHAIN_ALLOC                 0x0001u
#define  FS_FAT_JOURNAL_SIG_CLUS_CHAIN_DEL                   0x0002u
#define  FS_FAT_JOURNAL_SIG_ENTRY_CREATE                     0x0003u
#define  FS_FAT_JOURNAL_SIG_ENTRY_UPDATE                     0x0004u

#define  FS_FAT_JOURNAL_LOG_MARK_SIZE                        2u
#define  FS_FAT_JOURNAL_LOG_SIG_SIZE                         2u
#define  FS_FAT_JOURNAL_LOG_CLUS_CHAIN_ALLOC_SIZE            11u
#define  FS_FAT_JOURNAL_LOG_CLUS_CHAIN_DEL_HEADER_SIZE       13u
#define  FS_FAT_JOURNAL_LOG_ENTRY_CREATE_SIZE                22u
#define  FS_FAT_JOURNAL_LOG_ENTRY_DEL_HEADER_SIZE            20u

#define  FS_FAT_JOURNAL_MAX_DEL_MARKER_STEP_SIZE             1000u

#define  FS_FAT_JOURNAL_FILE_NAME                            "ucfs.jnl"

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef struct fs_fat_dte_update_jnl_log_wr_cb_data {
  FS_FAT_SEC_BYTE_POS EndDtePos;
  void                *BufPtr;
} FS_FAT_DTE_UPDATE_JNL_LOG_CB_DATA;

//                                                                 ---------- CLUS CHAIN ALLOC LOG STRUCTURE ----------
enum FS_FAT_JOURNAL_CLUS_CHAIN_ALLOC_LOG_STRUCTURE {
  FS_FAT_JOURNAL_CLUS_CHAIN_ALLOC_LOG_ENTER_MARK_OFFSET     = 0u,
  FS_FAT_JOURNAL_CLUS_CHAIN_ALLOC_LOG_SIG_OFFSET            = FS_FAT_JOURNAL_CLUS_CHAIN_ALLOC_LOG_ENTER_MARK_OFFSET
                                                              + FS_FAT_JOURNAL_LOG_MARK_SIZE,

  FS_FAT_JOURNAL_CLUS_CHAIN_ALLOC_LOG_START_CLUS_OFFSET     = FS_FAT_JOURNAL_CLUS_CHAIN_ALLOC_LOG_SIG_OFFSET
                                                              + FS_FAT_JOURNAL_LOG_SIG_SIZE,

  FS_FAT_JOURNAL_CLUS_CHAIN_ALLOC_LOG_IS_NEW_OFFSET         = FS_FAT_JOURNAL_CLUS_CHAIN_ALLOC_LOG_START_CLUS_OFFSET
                                                              + sizeof(FS_FAT_CLUS_NBR),

  FS_FAT_JOURNAL_CLUS_CHAIN_ALLOC_LOG_ENTER_END_MARK_OFFSET = FS_FAT_JOURNAL_CLUS_CHAIN_ALLOC_LOG_IS_NEW_OFFSET
                                                              + sizeof(CPU_BOOLEAN)
};

//                                                                 ----------- CLUS CHAIN DEL LOG STRUCTURE -----------
enum FS_FAT_JOURNAL_CLUS_CHAIN_DEL_LOG_STRUCTURE {
  FS_FAT_JOURNAL_CLUS_CHAIN_DEL_LOG_ENTER_MARK_OFFSET       = 0u,
  FS_FAT_JOURNAL_CLUS_CHAIN_DEL_LOG_SIG_OFFSET              = FS_FAT_JOURNAL_CLUS_CHAIN_DEL_LOG_ENTER_MARK_OFFSET
                                                              + FS_FAT_JOURNAL_LOG_MARK_SIZE,

  FS_FAT_JOURNAL_CLUS_CHAIN_DEL_LOG_MARKER_CNT_OFFSET       = FS_FAT_JOURNAL_CLUS_CHAIN_DEL_LOG_SIG_OFFSET
                                                              + FS_FAT_JOURNAL_LOG_SIG_SIZE,

  FS_FAT_JOURNAL_CLUS_CHAIN_DEL_LOG_START_CLUS_OFFSET       = FS_FAT_JOURNAL_CLUS_CHAIN_DEL_LOG_MARKER_CNT_OFFSET
                                                              + sizeof(CPU_INT32U),

  FS_FAT_JOURNAL_CLUS_CHAIN_DEL_LOG_DEL_FIRST_OFFSET        = FS_FAT_JOURNAL_CLUS_CHAIN_DEL_LOG_START_CLUS_OFFSET
                                                              + sizeof(FS_FAT_CLUS_NBR)
};

//                                                                 ------------ ENTRY CREATE LOG STRUCTURE ------------
enum FS_FAT_JOURNAL_ENTRY_CREATE_LOG_STRUCTURE {
  FS_FAT_JOURNAL_ENTRY_CREATE_LOG_ENTER_MARK_OFFSET         = 0u,
  FS_FAT_JOURNAL_ENTRY_CREATE_LOG_SIG_OFFSET                = FS_FAT_JOURNAL_ENTRY_CREATE_LOG_ENTER_MARK_OFFSET
                                                              + FS_FAT_JOURNAL_LOG_MARK_SIZE,

  FS_FAT_JOURNAL_ENTRY_CREATE_LOG_START_SEC_NBR_OFFSET      = FS_FAT_JOURNAL_ENTRY_CREATE_LOG_SIG_OFFSET
                                                              + FS_FAT_JOURNAL_LOG_SIG_SIZE,

  FS_FAT_JOURNAL_ENTRY_CREATE_LOG_START_SEC_POS_OFFSET      = FS_FAT_JOURNAL_ENTRY_CREATE_LOG_START_SEC_NBR_OFFSET
                                                              + sizeof(FS_FAT_SEC_NBR),

  FS_FAT_JOURNAL_ENTRY_CREATE_LOG_END_SEC_NBR_OFFSET        = FS_FAT_JOURNAL_ENTRY_CREATE_LOG_START_SEC_POS_OFFSET
                                                              + sizeof(FS_FAT_SEC_SIZE),

  FS_FAT_JOURNAL_ENTRY_CREATE_LOG_END_SEC_POS_OFFSET        = FS_FAT_JOURNAL_ENTRY_CREATE_LOG_END_SEC_NBR_OFFSET
                                                              + sizeof(FS_FAT_SEC_NBR),

  FS_FAT_JOURNAL_ENTRY_CREATE_LOG_ENTER_END_MARK_OFFSET     = FS_FAT_JOURNAL_ENTRY_CREATE_LOG_END_SEC_POS_OFFSET
                                                              + sizeof(FS_FAT_SEC_SIZE)
};

//                                                                 ------------- ENTRY DEL LOG STRUCTURE --------------
enum FS_FAT_JOURNAL_ENTRY_DEL_LOG_STRUCTURE {
  FS_FAT_JOURNAL_ENTRY_DEL_LOG_ENTER_MARK_OFFSET            = 0u,
  FS_FAT_JOURNAL_ENTRY_DEL_LOG_SIG_OFFSET                   = FS_FAT_JOURNAL_ENTRY_DEL_LOG_ENTER_MARK_OFFSET
                                                              + FS_FAT_JOURNAL_LOG_MARK_SIZE,

  FS_FAT_JOURNAL_ENTRY_DEL_LOG_START_SEC_NBR_OFFSET         = FS_FAT_JOURNAL_ENTRY_DEL_LOG_SIG_OFFSET
                                                              + FS_FAT_JOURNAL_LOG_SIG_SIZE,

  FS_FAT_JOURNAL_ENTRY_DEL_LOG_START_SEC_POS_OFFSET         = FS_FAT_JOURNAL_ENTRY_DEL_LOG_START_SEC_NBR_OFFSET
                                                              + sizeof(FS_FAT_SEC_NBR),

  FS_FAT_JOURNAL_ENTRY_DEL_LOG_END_SEC_NBR_OFFSET           = FS_FAT_JOURNAL_ENTRY_DEL_LOG_START_SEC_POS_OFFSET
                                                              + sizeof(FS_FAT_SEC_SIZE),

  FS_FAT_JOURNAL_ENTRY_DEL_LOG_END_SEC_POS_OFFSET           = FS_FAT_JOURNAL_ENTRY_DEL_LOG_END_SEC_NBR_OFFSET
                                                              + sizeof(FS_FAT_SEC_NBR),

  FS_FAT_JOURNAL_ENTRY_DEL_LOG_ENTRY_LOG_START_OFFSET       = FS_FAT_JOURNAL_ENTRY_DEL_LOG_END_SEC_POS_OFFSET
                                                              + sizeof(FS_FAT_SEC_SIZE)
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                         GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (FS_TEST_FAT_JNL_EN == DEF_ENABLED)
extern CPU_INT08U FS_Test_FAT_JournalTestNumber;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void FS_FAT_JournalFileCreate(FS_FAT_VOL *p_fat_vol,
                                     RTOS_ERR   *p_err);

static void FS_FAT_JournalFileDel(FS_FAT_VOL *p_fat_vol,
                                  RTOS_ERR   *p_err);

static void FS_FAT_JournalClr(FS_FAT_VOL       *p_fat_vol,
                              FS_FAT_FILE_SIZE start_pos,
                              FS_FAT_FILE_SIZE len,
                              RTOS_ERR         *p_err);

static void FS_FAT_JournalPeek(FS_FAT_VOL *p_fat_vol,
                               void       *p_log,
                               CPU_SIZE_T pos,
                               CPU_SIZE_T len,
                               RTOS_ERR   *p_err);

static void FS_FAT_JournalPosSet(FS_FAT_VOL *p_fat_vol,
                                 CPU_SIZE_T pos,
                                 RTOS_ERR   *p_err);

static void FS_FAT_JournalRd(FS_FAT_VOL *p_fat_vol,
                             void       *p_log,
                             CPU_SIZE_T len,
                             RTOS_ERR   *p_err);

static void FS_FAT_JournalRdWr(FS_FAT_VOL  *p_fat_vol,
                               void        *p_log,
                               CPU_SIZE_T  len,
                               CPU_BOOLEAN is_wr,
                               RTOS_ERR    *p_err);

static CPU_INT16U FS_FAT_JournalRd_INT16U(FS_FAT_VOL *p_fat_vol,
                                          RTOS_ERR   *p_err);

static CPU_INT32U FS_FAT_JournalRd_INT32U(FS_FAT_VOL *p_fat_vol,
                                          RTOS_ERR   *p_err);

static void FS_FAT_JournalWr(FS_FAT_VOL *p_fat_vol,
                             void       *p_log,
                             CPU_SIZE_T len,
                             RTOS_ERR   *p_err);

static CPU_SIZE_T FS_FAT_JournalReverseScan(FS_FAT_VOL *p_fat_vol,
                                            CPU_SIZE_T start_pos,
                                            CPU_INT08U *p_pattern,
                                            CPU_SIZE_T pattern_size,
                                            RTOS_ERR   *p_err);

static void FS_FAT_JournalReplay(FS_FAT_VOL *p_fat_vol,
                                 RTOS_ERR   *p_err);

static void FS_FAT_JournalRevertClusChainAlloc(FS_FAT_VOL *p_fat_vol,
                                               RTOS_ERR   *p_err);

static void FS_FAT_JournalReplayClusChainDel(FS_FAT_VOL *p_fat_vol,
                                             RTOS_ERR   *p_err);

static void FS_FAT_JournalRevertEntryCreate(FS_FAT_VOL *p_fat_vol,
                                            RTOS_ERR   *p_err);

static void FS_FAT_JournalRevertEntryUpdate(FS_FAT_VOL *p_fat_vol,
                                            RTOS_ERR   *p_err);

static FS_FAT_DIRENT_BROWSE_OUTCOME FS_FAT_JournalDteRestore(FS_FAT_VOL          *p_fat_vol,
                                                             FS_FAT_SEC_BYTE_POS *p_dte_pos,
                                                             void                *p_arg,
                                                             RTOS_ERR            *p_err);

static FS_FAT_DIRENT_BROWSE_OUTCOME FS_FAT_JournalDteSave(FS_FAT_VOL          *p_fat_vol,
                                                          FS_FAT_SEC_BYTE_POS *p_dte_pos,
                                                          void                *p_arg,
                                                          RTOS_ERR            *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           FS_FAT_JournalOpen()
 *
 * @brief    Replay existing journal. Create and setup journal for current use.
 *
 * @param    p_fat_vol   Pointer to volume.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
void FS_FAT_JournalOpen(FS_FAT_VOL *p_fat_vol,
                        RTOS_ERR   *p_err)
{
  FS_FAT_JOURNAL_DATA *p_jnl_data;
  FS_FAT_SEC_BYTE_POS jnl_file_start_pos;
  FS_FAT_SEC_BYTE_POS jnl_file_end_pos;
  CPU_BOOLEAN         entry_exists;
  FS_FAT_ENTRY_INFO   fat_entry_info;

  WITH_SCOPE_BEGIN(p_err) {
    p_jnl_data = &p_fat_vol->JournalData;

    //                                                             ---------------- SETUP JOURNAL DATA ----------------
    p_jnl_data->Replaying = DEF_NO;
    p_jnl_data->InitDone = DEF_NO;
    //                                                             ----------------- EXISTING JOURNAL -----------------
    //                                                             Look for jnl file under root dir.
    jnl_file_start_pos.SecNbr = p_fat_vol->RootDirStart;
    jnl_file_start_pos.SecOffset = 0u;
    BREAK_ON_ERR(entry_exists = FS_FAT_EntryFind(p_fat_vol,
                                                 FS_FAT_JOURNAL_FILE_NAME,
                                                 DEF_NULL,
                                                 &jnl_file_start_pos,
                                                 &jnl_file_end_pos,
                                                 p_err));

    if (entry_exists) {                                         // Replay existing file.
      BREAK_ON_ERR(FS_FAT_EntryParse(p_fat_vol,
                                     &jnl_file_start_pos,
                                     &jnl_file_end_pos,
                                     &fat_entry_info,
                                     DEF_NULL,
                                     DEF_NULL,
                                     0u,
                                     p_err));

      p_fat_vol->JournalData.File.CurPos = 0u;
      p_fat_vol->JournalData.File.CurSecBytePos.SecNbr = FS_FAT_CLUS_TO_SEC(p_fat_vol, fat_entry_info.FirstClusNbr);
      p_fat_vol->JournalData.File.CurSecBytePos.SecOffset = 0u;
      p_fat_vol->JournalData.File.EntryStartPos = jnl_file_start_pos;
      p_fat_vol->JournalData.File.FirstClus = fat_entry_info.FirstClusNbr;

      BREAK_ON_ERR(FS_FAT_JournalReplay(p_fat_vol, p_err));
    } else {
      //                                                           Create new jnl file.
      BREAK_ON_ERR(FS_FAT_JournalFileCreate(p_fat_vol, p_err));
    }

    //                                                             Update journal's state.
    p_jnl_data->InitDone = DEF_YES;
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                           FS_FAT_JournalClose()
 *
 * @brief    Free all references to journals.
 *
 * @param    p_fat_vol   Pointer to a volume.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
void FS_FAT_JournalClose(FS_FAT_VOL *p_fat_vol,
                         RTOS_ERR   *p_err)
{
  FS_FAT_JOURNAL_DATA *p_jnl_data;

  WITH_SCOPE_BEGIN(p_err) {
    p_jnl_data = &p_fat_vol->JournalData;
    p_jnl_data->InitDone = DEF_NO;

    BREAK_ON_ERR(FS_FAT_JournalFileDel(p_fat_vol, p_err));
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                           FS_FAT_JournalClrReset()
 *
 * @brief    Clear journal up to current position and reset current position.
 *
 * @param    p_fat_vol   Pointer to volume.
 *
 * @param    p_err       Error pointer.
 *
 * @note     (1) Journal is cleared from first to last sector. This allows the journal to be
 *               atomically invalidated since a single sector clear will remove the first enter
 *               mark. If this mark is not present, journal will not be replayed (see
 *               FS_FAT_JournalReplay() notes).
 *******************************************************************************************************/
void FS_FAT_JournalClrReset(FS_FAT_VOL *p_fat_vol,
                            RTOS_ERR   *p_err)
{
  FS_FAT_JOURNAL_DATA *p_jnl_data;

  WITH_SCOPE_BEGIN(p_err) {
    p_jnl_data = &p_fat_vol->JournalData;

    if (p_fat_vol->IsJournaled && p_jnl_data->File.CurPos != 0) {
      BREAK_ON_ERR(FS_FAT_JournalClr(p_fat_vol,                 // Clr journal up to current position (See Note #1).
                                     0u,
                                     p_jnl_data->File.CurPos,
                                     p_err));

      BREAK_ON_ERR(FS_FAT_JournalPosSet(p_fat_vol,              // Rst current position.
                                        0u,
                                        p_err));
    }
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                       FS_FAT_JournalEnterTopLvlOp()
 *
 * @brief    Write top level operation enter mark to journal.
 *
 * @param    p_fat_vol   Pointer to a FAT volume.
 *
 * @param    size_reqd   Required remaining space in journal.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
void FS_FAT_JournalEnterTopLvlOp(FS_FAT_VOL *p_fat_vol,
                                 CPU_SIZE_T size_reqd,
                                 RTOS_ERR   *p_err)
{
  CPU_SIZE_T rem_size;
  CPU_INT08U log_buf[FS_FAT_JOURNAL_LOG_MARK_SIZE];

  WITH_SCOPE_BEGIN(p_err) {
    if (!p_fat_vol->IsJournaled || !p_fat_vol->JournalData.InitDone) {
      return;
    }

    rem_size = FS_FAT_JOURNAL_FILE_LEN - p_fat_vol->JournalData.File.CurPos;

    if (size_reqd > rem_size) {
      BREAK_ON_ERR(FS_FAT_VolSync(&p_fat_vol->Vol, p_err));
      BREAK_ON_ERR(FS_FAT_JournalClrReset(p_fat_vol, p_err));
    }

    MEM_VAL_SET_INT16U_LITTLE(&log_buf[0], FS_FAT_JOURNAL_MARK_TOP_LVL_ENTER);

    BREAK_ON_ERR(FS_FAT_JournalWr(p_fat_vol,
                                  &log_buf[0],
                                  FS_FAT_JOURNAL_LOG_MARK_SIZE,
                                  p_err));
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                   FS_FAT_JournalEnterClusChainAlloc()
 *
 * @brief    Append a cluster chain allocate log to the journal.
 *
 * @param    p_fat_vol   Pointer to volume.
 *
 * @param    start_clus  First cluster of the chain from which allocation/extension will take place.
 *
 * @param    is_new      Indicates if the cluster chain is new (see Note #2).
 *
 * @param    p_err       Error pointer.
 *
 * @note     (1) The caller MUST check that the journal has been initialized and is not replaying
 *               before calling this function.
 *
 * @note     (2) If the cluster chain is new, then the start cluster must be deleted upon failure in
 *               order to completely unwind the cluster chain allocation. Otherwise, the start cluster
 *               must be left untouched as it was already allocated.
 *******************************************************************************************************/
void FS_FAT_JournalEnterClusChainAlloc(FS_FAT_VOL      *p_fat_vol,
                                       FS_FAT_CLUS_NBR start_clus,
                                       CPU_BOOLEAN     is_new,
                                       RTOS_ERR        *p_err)
{
  CPU_INT08U log_buf[FS_FAT_JOURNAL_LOG_CLUS_CHAIN_ALLOC_SIZE];

  WITH_SCOPE_BEGIN(p_err) {
    if (!p_fat_vol->IsJournaled || !p_fat_vol->JournalData.InitDone) {
      return;
    }

    //                                                             --------------------- FORM LOG ---------------------
    MEM_VAL_SET_INT16U_LITTLE((void *)&log_buf[FS_FAT_JOURNAL_CLUS_CHAIN_ALLOC_LOG_ENTER_MARK_OFFSET], FS_FAT_JOURNAL_MARK_ENTER);
    MEM_VAL_SET_INT16U_LITTLE((void *)&log_buf[FS_FAT_JOURNAL_CLUS_CHAIN_ALLOC_LOG_SIG_OFFSET], FS_FAT_JOURNAL_SIG_CLUS_CHAIN_ALLOC);
    MEM_VAL_SET_INT32U_LITTLE((void *)&log_buf[FS_FAT_JOURNAL_CLUS_CHAIN_ALLOC_LOG_START_CLUS_OFFSET], start_clus);
    MEM_VAL_SET_INT08U_LITTLE((void *)&log_buf[FS_FAT_JOURNAL_CLUS_CHAIN_ALLOC_LOG_IS_NEW_OFFSET], is_new);
    MEM_VAL_SET_INT16U_LITTLE((void *)&log_buf[FS_FAT_JOURNAL_CLUS_CHAIN_ALLOC_LOG_ENTER_END_MARK_OFFSET], FS_FAT_JOURNAL_MARK_ENTER_END);

    //                                                             ---------------- WR LOG TO JOURNAL -----------------
    LOG_DBG(("Wr'ing log (enter) for ", (X)FS_FAT_JOURNAL_SIG_CLUS_CHAIN_ALLOC, "."));

    BREAK_ON_ERR(FS_FAT_JournalWr(p_fat_vol,
                                  &log_buf[0],
                                  FS_FAT_JOURNAL_LOG_CLUS_CHAIN_ALLOC_SIZE,
                                  p_err));
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                       FS_FAT_JournalEnterClusChainDel()
 *
 * @brief    Append a cluster chain deletion log to the journal.
 *
 * @param    p_fat_vol   Pointer to volume.
 *
 * @param    start_clus  Cluster number to start deletion from.
 *
 * @param    nbr_clus    Number of clusters to delete.
 *
 * @param    del_first   Indicates whether or not the start cluster should be freed (see
 *                       FS_FAT_ClusChainDel()).
 *
 * @param    p_err       Error pointer.
 *
 * @note     (1) The caller MUST check that the journal has been initialized and is not replaying
 *               before calling this function.
 *******************************************************************************************************/
void FS_FAT_JournalEnterClusChainDel(FS_FAT_VOL      *p_fat_vol,
                                     FS_FAT_CLUS_NBR start_clus,
                                     FS_FAT_CLUS_NBR nbr_clus,
                                     CPU_BOOLEAN     del_first,
                                     RTOS_ERR        *p_err)
{
  FS_FAT_CLUS_NBR marker_step;
  CPU_INT32U      marker_cnt;
  CPU_INT08U      marker_size;
  FS_LB_SIZE      cur_sec_free_size;
  FS_LB_SIZE      marker_log_size;
  FS_LB_SIZE      journal_free_size;
  FS_LB_SIZE      lb_size;
  FS_FAT_CLUS_NBR cur_clus;
  FS_FAT_CLUS_NBR last_clus_tbl[3];
  FS_FAT_CLUS_NBR prev_clus;
  CPU_INT08U      log[FS_FAT_JOURNAL_LOG_CLUS_CHAIN_DEL_HEADER_SIZE];
  CPU_INT32U      marker_cnt_chk;

  WITH_SCOPE_BEGIN(p_err) {
    //                                                             ---------------- CHK JOURNAL STATE -----------------
    if (!p_fat_vol->IsJournaled || !p_fat_vol->JournalData.InitDone) {
      return;
    }

    BREAK_ON_ERR(lb_size = FSBlkDev_LbSizeGet(p_fat_vol->Vol.BlkDevHandle, p_err));

    //                                                             - COMPUTE MARKER STEP SIZE, MARKER CNT & LOG SIZE --
    //                                                             Find free space size in cur sec and journal.
    cur_sec_free_size = lb_size - p_fat_vol->JournalData.File.CurSecBytePos.SecOffset;
    journal_free_size = FS_FAT_JOURNAL_FILE_LEN - p_fat_vol->JournalData.File.CurPos;
    if (cur_sec_free_size <= (FS_FAT_JOURNAL_LOG_CLUS_CHAIN_DEL_HEADER_SIZE + FS_FAT_JOURNAL_LOG_MARK_SIZE)) {
      marker_log_size = lb_size + cur_sec_free_size
                        - (FS_FAT_JOURNAL_LOG_CLUS_CHAIN_DEL_HEADER_SIZE + FS_FAT_JOURNAL_LOG_MARK_SIZE);
    } else {
      marker_log_size = cur_sec_free_size - (FS_FAT_JOURNAL_LOG_CLUS_CHAIN_DEL_HEADER_SIZE + FS_FAT_JOURNAL_LOG_MARK_SIZE);
    }

    do {                                                        // Compute marker step size.
                                                                // No bit packing used for 12-bit clus nbr.
      marker_size = (p_fat_vol->FAT_Type == FS_FAT_TYPE_FAT32) ? 4u : 2u;
      marker_step = (((nbr_clus * marker_size) - 1u) / marker_log_size) + 1u;
      //                                                           Log size increased until marker step is appropriate.
      marker_log_size += lb_size;
    } while ((marker_step > FS_FAT_JOURNAL_MAX_DEL_MARKER_STEP_SIZE)
             && (marker_log_size <= journal_free_size - (FS_FAT_JOURNAL_LOG_CLUS_CHAIN_DEL_HEADER_SIZE
                                                         + FS_FAT_JOURNAL_LOG_MARK_SIZE)));

    if (marker_step > FS_FAT_JOURNAL_MAX_DEL_MARKER_STEP_SIZE) {
      LOG_DBG(("Unable to fit all markers in journal with step size of ", (u)FS_FAT_JOURNAL_MAX_DEL_MARKER_STEP_SIZE, "."));
      LOG_DBG(("Step size used will be ", (u)marker_step, "."));
    }

    marker_cnt = (nbr_clus - 1u) / marker_step;                 // Compute nbr of markers to be log'd.
    marker_cnt += (((nbr_clus - 1u) % marker_step) == 0u) ? 0u : 1u;

    //                                                             -------- LOG START, DEL FIRST & MARKER NBR ---------
    LOG_VRB(("Writing log (enter) for ", (X)FS_FAT_JOURNAL_SIG_CLUS_CHAIN_DEL, "."));

    MEM_VAL_SET_INT16U_LITTLE((void *)&log[FS_FAT_JOURNAL_CLUS_CHAIN_DEL_LOG_ENTER_MARK_OFFSET], FS_FAT_JOURNAL_MARK_ENTER);
    MEM_VAL_SET_INT16U_LITTLE((void *)&log[FS_FAT_JOURNAL_CLUS_CHAIN_DEL_LOG_SIG_OFFSET], FS_FAT_JOURNAL_SIG_CLUS_CHAIN_DEL);
    MEM_VAL_SET_INT32U_LITTLE((void *)&log[FS_FAT_JOURNAL_CLUS_CHAIN_DEL_LOG_MARKER_CNT_OFFSET], marker_cnt);
    MEM_VAL_SET_INT32U_LITTLE((void *)&log[FS_FAT_JOURNAL_CLUS_CHAIN_DEL_LOG_START_CLUS_OFFSET], start_clus);
    MEM_VAL_SET_INT08U_LITTLE((void *)&log[FS_FAT_JOURNAL_CLUS_CHAIN_DEL_LOG_DEL_FIRST_OFFSET], del_first);

    BREAK_ON_ERR(FS_FAT_JournalWr(p_fat_vol,
                                  &log[0],
                                  FS_FAT_JOURNAL_LOG_CLUS_CHAIN_DEL_HEADER_SIZE,
                                  p_err));

    marker_cnt_chk = 0u;
    cur_clus = start_clus;
    prev_clus = 0u;
    BREAK_ON_ERR(while) (cur_clus != prev_clus) {
      //                                                           ------------ FIND NEXT MARKER CLUS NBR -------------
      prev_clus = cur_clus;
      BREAK_ON_ERR((void)FS_FAT_ClusChainFollow(p_fat_vol,
                                                cur_clus,
                                                marker_step,
                                                last_clus_tbl,
                                                p_err));

      cur_clus = FS_FAT_IS_VALID_CLUS(p_fat_vol, last_clus_tbl[0]) ? last_clus_tbl[0]
                 : last_clus_tbl[1];
      //                                                           If cur clus has already been log'd then all markers have been log'd.
      if (cur_clus != prev_clus) {
        marker_cnt_chk++;                                       // Update log'd clus cnt.

        //                                                         -------------------- LOG MARKER --------------------
        if (p_fat_vol->FAT_Type == FS_FAT_TYPE_FAT12) {
          MEM_VAL_SET_INT16U_LITTLE((void *)&log[0], (cur_clus & 0xFFF));
          marker_size = 2u;
        } else if (p_fat_vol->FAT_Type == FS_FAT_TYPE_FAT16 ) {
          MEM_VAL_SET_INT16U_LITTLE((void *)&log[0], cur_clus);
          marker_size = 2u;
        } else {
          MEM_VAL_SET_INT32U_LITTLE((void *)&log[0], cur_clus);
          marker_size = 4u;
        }

        BREAK_ON_ERR(FS_FAT_JournalWr(p_fat_vol,
                                      &log[0],
                                      marker_size,
                                      p_err));
      }
    }

    //                                                             ------------------ CHK MARKER CNT ------------------
    if (marker_cnt_chk != marker_cnt) {
      LOG_ERR(("Marker number mismatch: ", (u)marker_cnt, " markers expected ", (u)marker_cnt_chk, " logged."));
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
    }

    //                                                             ---------------- LOG ENTER END MARK ----------------
    MEM_VAL_SET_INT16U_LITTLE((void *)&log[0], FS_FAT_JOURNAL_MARK_ENTER_END);
    BREAK_ON_ERR(FS_FAT_JournalWr(p_fat_vol,
                                  &log[0],
                                  FS_FAT_JOURNAL_LOG_MARK_SIZE,
                                  p_err));
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                       FS_FAT_JournalEnterEntryCreate()
 *
 * @brief    Append a directory entry creation log to the journal
 *
 * @param    p_fat_vol           Pointer to volume.
 *
 * @param    p_dir_start_pos     Pointer to entry start position.
 *
 * @param    p_dir_end_pos       Pointer to entry end position.
 *
 * @param    p_err               Error pointer.
 *
 * @note     (1) The caller MUST check that the journal has been initialized and is not replaying
 *               before calling this function.
 *******************************************************************************************************/
void FS_FAT_JournalEnterEntryCreate(FS_FAT_VOL          *p_fat_vol,
                                    FS_FAT_SEC_BYTE_POS *p_dir_start_pos,
                                    FS_FAT_SEC_BYTE_POS *p_dir_end_pos,
                                    RTOS_ERR            *p_err)
{
  CPU_INT08U log[FS_FAT_JOURNAL_LOG_ENTRY_CREATE_SIZE];

  WITH_SCOPE_BEGIN(p_err){
    if (!p_fat_vol->IsJournaled || !p_fat_vol->JournalData.InitDone) {
      return;
    }

    //                                                             ---------------- LOG SEC NBR & POS -----------------
    LOG_DBG(("Wr'ing log (enter) for ", (X)FS_FAT_JOURNAL_SIG_ENTRY_CREATE, "."));

    MEM_VAL_SET_INT16U_LITTLE((void *)&log[FS_FAT_JOURNAL_ENTRY_CREATE_LOG_ENTER_MARK_OFFSET], FS_FAT_JOURNAL_MARK_ENTER);
    MEM_VAL_SET_INT16U_LITTLE((void *)&log[FS_FAT_JOURNAL_ENTRY_CREATE_LOG_SIG_OFFSET], FS_FAT_JOURNAL_SIG_ENTRY_CREATE);
    MEM_VAL_SET_INT32U_LITTLE((void *)&log[FS_FAT_JOURNAL_ENTRY_CREATE_LOG_START_SEC_NBR_OFFSET], p_dir_start_pos->SecNbr);
    MEM_VAL_SET_INT32U_LITTLE((void *)&log[FS_FAT_JOURNAL_ENTRY_CREATE_LOG_START_SEC_POS_OFFSET], p_dir_start_pos->SecOffset);
    MEM_VAL_SET_INT32U_LITTLE((void *)&log[FS_FAT_JOURNAL_ENTRY_CREATE_LOG_END_SEC_NBR_OFFSET], p_dir_end_pos->SecNbr);
    MEM_VAL_SET_INT32U_LITTLE((void *)&log[FS_FAT_JOURNAL_ENTRY_CREATE_LOG_END_SEC_POS_OFFSET], p_dir_end_pos->SecOffset);
    MEM_VAL_SET_INT16U_LITTLE((void *)&log[FS_FAT_JOURNAL_ENTRY_CREATE_LOG_ENTER_END_MARK_OFFSET], FS_FAT_JOURNAL_MARK_ENTER_END);

    BREAK_ON_ERR(FS_FAT_JournalWr(p_fat_vol,
                                  &log[0],
                                  FS_FAT_JOURNAL_LOG_ENTRY_CREATE_SIZE,
                                  p_err));
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                       FS_FAT_JournalEnterEntryUpdate()
 *
 * @brief    Append a directory entry update/deletion log to the journal
 *
 * @param    p_fat_vol           Pointer to volume.
 *
 * @param    p_dir_start_pos     Pointer to entry start position.
 *
 * @param    p_dir_end_pos       Pointer to entry end position.
 *
 * @param    p_err               Error pointer.
 *
 * @note     (1) The caller MUST check that the journal has been initialized and is not replaying
 *               before calling this function.
 *******************************************************************************************************/
void FS_FAT_JournalEnterEntryUpdate(FS_FAT_VOL          *p_fat_vol,
                                    FS_FAT_SEC_BYTE_POS dir_start_pos,
                                    FS_FAT_SEC_BYTE_POS dir_end_pos,
                                    RTOS_ERR            *p_err)
{
  CPU_INT08U                        log[DEF_MAX(FS_FAT_SIZE_DIR_ENTRY, FS_FAT_JOURNAL_LOG_ENTRY_DEL_HEADER_SIZE)];
  FS_FAT_DTE_UPDATE_JNL_LOG_CB_DATA jnl_log_data;

  WITH_SCOPE_BEGIN(p_err) {
    if (!p_fat_vol->IsJournaled || !p_fat_vol->JournalData.InitDone) {
      return;
    }

    //                                                             ---------------- LOG SEC NBR & POS -----------------
    LOG_DBG(("Wr'ing log (enter) for ", (X)FS_FAT_JOURNAL_SIG_ENTRY_UPDATE, "."));

    MEM_VAL_SET_INT16U_LITTLE((void *)&log[FS_FAT_JOURNAL_ENTRY_DEL_LOG_ENTER_MARK_OFFSET], FS_FAT_JOURNAL_MARK_ENTER);
    MEM_VAL_SET_INT16U_LITTLE((void *)&log[FS_FAT_JOURNAL_ENTRY_DEL_LOG_SIG_OFFSET], FS_FAT_JOURNAL_SIG_ENTRY_UPDATE);
    MEM_VAL_SET_INT32U_LITTLE((void *)&log[FS_FAT_JOURNAL_ENTRY_DEL_LOG_START_SEC_NBR_OFFSET], dir_start_pos.SecNbr);
    MEM_VAL_SET_INT32U_LITTLE((void *)&log[FS_FAT_JOURNAL_ENTRY_DEL_LOG_START_SEC_POS_OFFSET], dir_start_pos.SecOffset);
    MEM_VAL_SET_INT32U_LITTLE((void *)&log[FS_FAT_JOURNAL_ENTRY_DEL_LOG_END_SEC_NBR_OFFSET], dir_end_pos.SecNbr);
    MEM_VAL_SET_INT32U_LITTLE((void *)&log[FS_FAT_JOURNAL_ENTRY_DEL_LOG_END_SEC_POS_OFFSET], dir_end_pos.SecOffset);

    BREAK_ON_ERR(FS_FAT_JournalWr(p_fat_vol,
                                  &log[0],
                                  FS_FAT_JOURNAL_LOG_ENTRY_DEL_HEADER_SIZE,
                                  p_err));

    //                                                             ---------- ADD ALL DIR ENTRIES TO JOURNAL ----------
    jnl_log_data.EndDtePos = dir_end_pos;
    jnl_log_data.BufPtr = &log[0];

    BREAK_ON_ERR(FS_FAT_DirTblBrowse(p_fat_vol,
                                     &dir_start_pos,
                                     FS_FAT_JournalDteSave,
                                     (void *)&jnl_log_data,
                                     p_err));

    //                                                             ---------------- LOG ENTER END MARK ----------------
    MEM_VAL_SET_INT16U_LITTLE((void *)&log[0], FS_FAT_JOURNAL_MARK_ENTER_END);

    BREAK_ON_ERR(FS_FAT_JournalWr(p_fat_vol,
                                  &log[0],
                                  FS_FAT_JOURNAL_LOG_MARK_SIZE,
                                  p_err));
  } WITH_SCOPE_END
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           FS_FAT_JournalClr()
 *
 * @brief    Clear a region of the journal file.
 *
 * @param    p_fat_vol   Pointer to a volume.
 *
 * @param    start_pos   Clear start offset within the journal file.
 *
 * @param    len         Length of the file region to be cleared.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_FAT_JournalClr(FS_FAT_VOL       *p_fat_vol,
                              FS_FAT_FILE_SIZE start_pos,
                              FS_FAT_FILE_SIZE len,
                              RTOS_ERR         *p_err)
{
  FS_FAT_JOURNAL_DATA *p_jnl_data;
  CPU_INT08U          *p_data;
  CPU_INT08U          *p_buf;
  FS_FAT_SEC_NBR      first_sec;
  FS_FAT_SEC_BYTE_POS sec_byte_start_pos;
  FS_FAT_SEC_NBR      rem_sec_cnt;
  FS_FAT_SEC_NBR      cur_sec;
  CPU_INT08U          lb_size_log2;
  FS_LB_SIZE          lb_size;

  if (len > 0) {                                                // Trivial clear requests are ignored.
    WITH_SCOPE_BEGIN(p_err) {
      p_jnl_data = &p_fat_vol->JournalData;

      BREAK_ON_ERR(lb_size_log2 = FSBlkDev_LbSizeLog2Get(p_fat_vol->Vol.BlkDevHandle, p_err));

      lb_size = FS_UTIL_PWR2(lb_size_log2);
      first_sec = FS_FAT_CLUS_TO_SEC(p_fat_vol, p_jnl_data->File.FirstClus);
      sec_byte_start_pos.SecNbr = first_sec + FS_UTIL_DIV_PWR2(start_pos, lb_size_log2);
      sec_byte_start_pos.SecOffset = FS_UTIL_MODULO_PWR2(start_pos, lb_size_log2);
      rem_sec_cnt = FS_UTIL_DIV_PWR2(len, lb_size_log2);

      //                                                             Partial sec clr.
      if (sec_byte_start_pos.SecNbr != 0u) {
        BREAK_ON_ERR(FS_VOL_CACHE_BLK_RW)(&p_fat_vol->Vol,
                                          sec_byte_start_pos.SecNbr,
                                          FS_FAT_LB_TYPE_JNL_DATA,
                                          p_fat_vol->JournalData.JnlWrJobHandle,
                                          &p_buf,
                                          &p_fat_vol->JournalData.JnlWrJobHandle,
                                          p_err) {
          p_data = p_buf + sec_byte_start_pos.SecOffset;
          Mem_Clr((void *)p_data, lb_size - sec_byte_start_pos.SecOffset);
        }
      }

      //                                                             Full sec clr.
      cur_sec = sec_byte_start_pos.SecNbr;
      BREAK_ON_ERR(while) (rem_sec_cnt > 0u) {
        BREAK_ON_ERR(FS_VOL_CACHE_BLK_WR)(&p_fat_vol->Vol,
                                          cur_sec,
                                          FS_FAT_LB_TYPE_JNL_DATA,
                                          p_fat_vol->JournalData.JnlWrJobHandle,
                                          &p_buf,
                                          &p_fat_vol->JournalData.JnlWrJobHandle,
                                          p_err) {
          Mem_Clr((void *)p_buf, lb_size);
        }

        rem_sec_cnt--;
        cur_sec++;
      }

      LOG_VRB(((u)len, " octets cleared from position ", (u)start_pos));
    } WITH_SCOPE_END
  }
}

/****************************************************************************************************//**
 *                                           FS_FAT_JournalDteLogWr()
 *
 * @brief    Directory entry journal log write callback.
 *
 * @param    p_fat_vol   Pointer to a volume.
 *
 * @param    p_dte_pos   Pointer to the current directory entry position.
 *
 * @param    p_arg       Pointer to a FS_FAT_DTE_UPDATE_JNL_LOG_CB_DATA structure.
 *
 * @param    p_err       Error pointer.
 *
 * @return   Directory table browse outcome.
 *******************************************************************************************************/
static FS_FAT_DIRENT_BROWSE_OUTCOME FS_FAT_JournalDteSave(FS_FAT_VOL          *p_fat_vol,
                                                          FS_FAT_SEC_BYTE_POS *p_dte_pos,
                                                          void                *p_arg,
                                                          RTOS_ERR            *p_err)
{
  CPU_INT08U                        *p_dte_data;
  CPU_INT08U                        *p_buf;
  FS_FAT_DTE_UPDATE_JNL_LOG_CB_DATA *p_dte_log_wr_data;

  WITH_SCOPE_BEGIN(p_err) {
    p_dte_log_wr_data = (FS_FAT_DTE_UPDATE_JNL_LOG_CB_DATA *)p_arg;

    BREAK_ON_ERR(FS_VOL_CACHE_BLK_RD(&p_fat_vol->Vol,
                                     p_dte_pos->SecNbr,
                                     FS_FAT_LB_TYPE_DIRENT,
                                     &p_buf,
                                     p_err)) {
      p_dte_data = p_buf + p_dte_pos->SecOffset;
      Mem_Copy(p_dte_log_wr_data->BufPtr, (void *)p_dte_data, FS_FAT_SIZE_DIR_ENTRY);
    }

    BREAK_ON_ERR(FS_FAT_JournalWr(p_fat_vol,
                                  p_dte_log_wr_data->BufPtr,
                                  FS_FAT_SIZE_DIR_ENTRY,
                                  p_err));
  } WITH_SCOPE_END

  if (FS_FAT_SEC_BYTE_POS_ARE_EQUAL(p_dte_log_wr_data->EndDtePos, *p_dte_pos)) {
    return (FS_FAT_DIRENT_BROWSE_OUTCOME_STOP);
  }
  return (FS_FAT_DIRENT_BROWSE_OUTCOME_CONTINUE);
}

/****************************************************************************************************//**
 *                                           FS_FAT_JournalDteLogRd()
 *
 * @brief    Directory entry journal log read callback.
 *
 * @param    p_fat_vol   Pointer to a volume.
 *
 * @param    p_dte_pos   Pointer to the current directory entry position.
 *
 * @param    p_arg       Pointer to a FS_FAT_DTE_UPDATE_JNL_LOG_CB_DATA structure.
 *
 * @param    p_err       Error pointer.
 *
 * @return   Directory table browse outcome.
 *******************************************************************************************************/
static FS_FAT_DIRENT_BROWSE_OUTCOME FS_FAT_JournalDteRestore(FS_FAT_VOL          *p_fat_vol,
                                                             FS_FAT_SEC_BYTE_POS *p_dte_pos,
                                                             void                *p_arg,
                                                             RTOS_ERR            *p_err)
{
  CPU_INT08U                        *p_dte_data;
  CPU_INT08U                        *p_buf;
  FS_FAT_DTE_UPDATE_JNL_LOG_CB_DATA *p_dte_log_rd_data;

  WITH_SCOPE_BEGIN(p_err) {
    p_dte_log_rd_data = (FS_FAT_DTE_UPDATE_JNL_LOG_CB_DATA *)p_arg;

    BREAK_ON_ERR(FS_FAT_JournalRd(p_fat_vol,
                                  p_dte_log_rd_data->BufPtr,
                                  FS_FAT_SIZE_DIR_ENTRY,
                                  p_err));

    BREAK_ON_ERR(FS_VOL_CACHE_BLK_RW) (&p_fat_vol->Vol,
                                       p_dte_pos->SecNbr,
                                       FS_FAT_LB_TYPE_DIRENT,
                                       JobSched_VoidJobHandle,
                                       &p_buf,
                                       DEF_NULL,
                                       p_err) {
      p_dte_data = p_buf + p_dte_pos->SecOffset;
      Mem_Copy((void *)p_dte_data, p_dte_log_rd_data->BufPtr, FS_FAT_SIZE_DIR_ENTRY);
    }
  } WITH_SCOPE_END

  if (FS_FAT_SEC_BYTE_POS_ARE_EQUAL(p_dte_log_rd_data->EndDtePos, *p_dte_pos)) {
    return (FS_FAT_DIRENT_BROWSE_OUTCOME_STOP);
  }
  return (FS_FAT_DIRENT_BROWSE_OUTCOME_CONTINUE);
}

/****************************************************************************************************//**
 *                                           FS_FAT_JournalFileDel()
 *
 * @brief    Delete journal file.
 *
 * @param    p_fat_vol   Pointer to a volume.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_FAT_JournalFileDel(FS_FAT_VOL *p_fat_vol,
                                  RTOS_ERR   *p_err)
{
  FS_FAT_SEC_BYTE_POS    entry_end_pos;
  FS_FAT_ENTRY_INFO      entry_info;
  FS_FAT_CLUS_NBR        first_clus_nbr;
  FS_CACHE_WR_JOB_HANDLE wr_job_handle;

  WITH_SCOPE_BEGIN(p_err) {
    BREAK_ON_ERR(FS_FAT_JournalClr(p_fat_vol,
                                   0u,
                                   FS_FAT_JOURNAL_LOG_MARK_SIZE,
                                   p_err));

#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
    BREAK_ON_ERR(p_fat_vol->FatWrJobHandle = FSCache_WrJobJoin(p_fat_vol->Vol.CacheBlkDevDataPtr->CachePtr,
                                                               p_fat_vol->FatWrJobHandle,
                                                               p_fat_vol->JournalData.JnlWrJobHandle,
                                                               p_err));
#endif

    //                                                             ------------------ DEL DIR ENTRY -------------------
    BREAK_ON_ERR(FS_FAT_EntryParse(p_fat_vol,
                                   &p_fat_vol->JournalData.File.EntryStartPos,
                                   &entry_end_pos,
                                   &entry_info,
                                   DEF_NULL,
                                   DEF_NULL,
                                   0u,
                                   p_err));

    //                                                             ------------------ DEL CLUS CHAIN ------------------
    first_clus_nbr = entry_info.FirstClusNbr;
    if (FS_FAT_IS_VALID_CLUS(p_fat_vol, first_clus_nbr)) {
      BREAK_ON_ERR((void)FS_FAT_ClusChainReverseDel(p_fat_vol,
                                                    first_clus_nbr,
                                                    DEF_YES,
                                                    p_err));
    }

#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
    wr_job_handle = p_fat_vol->FatWrJobHandle;
#else
    wr_job_handle = FS_CACHE_WR_JOB_HANDLE_INIT;
#endif

    BREAK_ON_ERR(FS_FAT_DirEntriesErase(p_fat_vol,
                                        &p_fat_vol->JournalData.File.EntryStartPos,
                                        &entry_end_pos,
                                        &wr_job_handle,
                                        p_err));

#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
    BREAK_ON_ERR(FSCache_WrJobExec(p_fat_vol->Vol.CacheBlkDevDataPtr->CachePtr,
                                   wr_job_handle,
                                   DEF_NO,
                                   p_err));
#endif
  } WITH_SCOPE_END
}

/********************************************************************************************************
 *                                       FS_FAT_JournalFileCreate()
 *
 * Description : Create journal file.
 *
 * Argument(s) : p_fat_vol          Pointer to volume.
 *
 *               p_err              Error pointer.
 * Return(s)   : none.
 *
 * Note(s)     : none.
 *******************************************************************************************************/
static void FS_FAT_JournalFileCreate(FS_FAT_VOL *p_fat_vol,
                                     RTOS_ERR   *p_err)
{
  FS_FAT_JOURNAL_DATA    *p_jnl_data;
  FS_FAT_SEC_BYTE_POS    start_dte_pos;
  FS_FAT_SEC_BYTE_POS    end_dte_pos;
  FS_FAT_ENTRY_INFO      journal_file_info;
  FS_FAT_CLUS_NBR        journal_file_len_clus;
  FS_FAT_CLUS_NBR        first_clus_nbr;
  FS_CACHE_WR_JOB_HANDLE data_wr_job_handle = FS_CACHE_WR_JOB_HANDLE_INIT;
  FS_CACHE_WR_JOB_HANDLE entry_wr_job_handle = FS_CACHE_WR_JOB_HANDLE_INIT;
  CPU_INT08U             lb_size_log2;

  WITH_SCOPE_BEGIN(p_err) {
    p_jnl_data = &p_fat_vol->JournalData;

    BREAK_ON_ERR(lb_size_log2 = FSBlkDev_LbSizeLog2Get(p_fat_vol->Vol.BlkDevHandle, p_err));

    //                                                             Create directory entry.
    BREAK_ON_ERR(FS_FAT_EntryCreateInternal(p_fat_vol,
                                            &start_dte_pos,
                                            &end_dte_pos,
                                            FS_FAT_JOURNAL_FILE_NAME,
                                            p_fat_vol->RootDirStart,
                                            DEF_NO,
                                            &entry_wr_job_handle,
                                            p_err));

    //                                                             Allocate storage.
    journal_file_len_clus = FS_UTIL_DIV_PWR2(FS_FAT_JOURNAL_FILE_LEN,
                                             p_fat_vol->SecPerClusLog2 + lb_size_log2);
    journal_file_len_clus += FS_UTIL_MODULO_PWR2(FS_FAT_JOURNAL_FILE_LEN,
                                                 p_fat_vol->SecPerClusLog2 + lb_size_log2) == 0u ? 0u : 1u;

    BREAK_ON_ERR(first_clus_nbr = FS_FAT_ClusChainAlloc(p_fat_vol,
                                                        0,
                                                        DEF_NULL,
                                                        journal_file_len_clus,
                                                        DEF_YES,
                                                        &data_wr_job_handle,
                                                        FS_FAT_LB_TYPE_JNL_DATA,
                                                        p_err));

    //                                                             Update directory entry.
    journal_file_info.Attrib = FS_FAT_DIRENT_ATTR_HIDDEN | FS_FAT_DIRENT_ATTR_READ_ONLY;
    journal_file_info.Size = FS_FAT_JOURNAL_FILE_LEN;
    journal_file_info.FirstClusNbr = first_clus_nbr;

    BREAK_ON_ERR(FS_FAT_EntryUpdate(p_fat_vol,
                                    &end_dte_pos,
                                    &journal_file_info,
                                    DEF_YES,
                                    FS_FAT_DTE_FIELD_ALL,
                                    &entry_wr_job_handle,
                                    p_err));

    //                                                             Update journal file structure.
    p_jnl_data->File.EntryStartPos = start_dte_pos;
    p_jnl_data->File.FirstClus = first_clus_nbr;
    p_jnl_data->File.CurPos = 0u;
    p_jnl_data->File.CurSecBytePos.SecNbr = FS_FAT_CLUS_TO_SEC(p_fat_vol, first_clus_nbr);
    p_jnl_data->File.CurSecBytePos.SecOffset = 0u;

#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
    //                                                             Flush data blks.
    BREAK_ON_ERR(FSCache_WrJobExec(p_fat_vol->Vol.CacheBlkDevDataPtr->CachePtr,
                                   data_wr_job_handle,
                                   DEF_NO,
                                   p_err));

    //                                                             Flush FAT blks.
    BREAK_ON_ERR(FSCache_WrJobExec(p_fat_vol->Vol.CacheBlkDevDataPtr->CachePtr,
                                   p_fat_vol->FatWrJobHandle,
                                   DEF_NO,
                                   p_err));

    //                                                             Flush directory entry blocks.
    BREAK_ON_ERR(FSCache_WrJobExec(p_fat_vol->Vol.CacheBlkDevDataPtr->CachePtr,
                                   entry_wr_job_handle,
                                   DEF_NO,
                                   p_err));
#endif
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                           FS_FAT_JournalPeek()
 *
 * @brief    'Peek' at arbitrarily located log in journal, without incrementing the file position.
 *
 * @param    p_fat_vol   Pointer to volume.
 *
 * @param    p_log       Pointer to variable that will receive read data.
 *
 * @param    pos         Position where to start reading.
 *
 * @param    len         Size of data to be read in octets.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_FAT_JournalPeek(FS_FAT_VOL *p_fat_vol,
                               void       *p_log,
                               CPU_SIZE_T pos,
                               CPU_SIZE_T len,
                               RTOS_ERR   *p_err)
{
  FS_FAT_FILE_SIZE cur_file_pos;

  WITH_SCOPE_BEGIN(p_err) {
    cur_file_pos = p_fat_vol->JournalData.File.CurPos;

    BREAK_ON_ERR(FS_FAT_JournalPosSet(p_fat_vol,
                                      pos,
                                      p_err));

    BREAK_ON_ERR(FS_FAT_JournalRd(p_fat_vol,
                                  p_log,
                                  len,
                                  p_err));

    BREAK_ON_ERR(FS_FAT_JournalPosSet(p_fat_vol,
                                      cur_file_pos,
                                      p_err));
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                           FS_FAT_JournalPosSet()
 *
 * @brief    Set journal file position, along with corresponding sector number and sector position.
 *
 * @param    p_fat_vol   Pointer to volume.
 *
 * @param    pos         Journal file position to set.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_FAT_JournalPosSet(FS_FAT_VOL *p_fat_vol,
                                 CPU_SIZE_T pos,
                                 RTOS_ERR   *p_err)
{
  FS_FAT_JOURNAL_DATA *p_jnl_data;
  FS_FAT_SEC_NBR      cur_sec;
  FS_FAT_SEC_NBR      cur_sec_cnt;
  FS_FAT_SEC_NBR      sec_cnt;
  FS_FAT_SEC_NBR      sec_ix;
  CPU_INT08U          lb_size_log2;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_CRITICAL((pos <= FS_FAT_JOURNAL_FILE_LEN), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

    //                                                             ------------------- FIND SEC NBR -------------------
    p_jnl_data = &p_fat_vol->JournalData;

    BREAK_ON_ERR(lb_size_log2 = FSBlkDev_LbSizeLog2Get(p_fat_vol->Vol.BlkDevHandle, p_err));

    sec_cnt = FS_UTIL_DIV_PWR2(pos, lb_size_log2);
    cur_sec_cnt = FS_UTIL_DIV_PWR2(p_jnl_data->File.CurPos, lb_size_log2);
    cur_sec = FS_FAT_CLUS_TO_SEC(p_fat_vol, p_jnl_data->File.FirstClus);

    if (sec_cnt != cur_sec_cnt) {                               // If new pos is not in cur sec find new sec nbr.
      BREAK_ON_ERR(for) (sec_ix = 0; sec_ix < sec_cnt; sec_ix++) {
        cur_sec = FS_FAT_SecNextGet(p_fat_vol,
                                    cur_sec,
                                    p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          return;
        }
      }
    } else {
      cur_sec = p_jnl_data->File.CurSecBytePos.SecNbr;          // ... else do not change sec nbr.
    }

    //                                                             --------------------- SET POS ----------------------
    p_jnl_data->File.CurPos = pos;
    p_jnl_data->File.CurSecBytePos.SecNbr = cur_sec;
    p_jnl_data->File.CurSecBytePos.SecOffset = (pos & (FS_UTIL_PWR2(lb_size_log2) - 1u));
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                           FS_FAT_JournalWr()
 *
 * @brief    Write log to journal.
 *
 * @param    p_fat_vol   Pointer to volume.
 *
 * @param    p_log       Pointer to log buffer.
 *
 * @param    len         Length of log, in octets.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_FAT_JournalWr(FS_FAT_VOL *p_fat_vol,
                             void       *p_log,
                             CPU_SIZE_T len,
                             RTOS_ERR   *p_err)
{
  FS_FAT_JournalRdWr(p_fat_vol, p_log, len, DEF_YES, p_err);
}

/****************************************************************************************************//**
 *                                           FS_FAT_JournalRd()
 *
 * @brief    Read log from journal & increment file position.
 *
 * @param    p_fat_vol   Pointer to volume.
 *
 * @param    p_log       Pointer to buffer that will receive log data.
 *
 * @param    len         Size of data to be read in octets.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_FAT_JournalRd(FS_FAT_VOL *p_fat_vol,
                             void       *p_log,
                             CPU_SIZE_T len,
                             RTOS_ERR   *p_err)
{
  FS_FAT_JournalRdWr(p_fat_vol, p_log, len, DEF_NO, p_err);
}
//                                                                 *INDENT-OFF*
/****************************************************************************************************//**
 *                                           FS_FAT_JournalRdWr()
 *
 * @brief    Read/write log from/to journal & increment file position.
 *
 * @param    p_fat_vol   Pointer to volume.
 *
 * @param    p_log       Pointer to buffer that will transfer log data.
 *
 * @param    len         Size of data to be transferred in octets.
 *
 * @param    is_wr       Flag indicating transfer directions:
 *                       DEF_YES     Write direction.
 *                       DEF_NO      Read direction.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_FAT_JournalRdWr(FS_FAT_VOL  *p_fat_vol,
                               void        *p_log,
                               CPU_SIZE_T  len,
                               CPU_BOOLEAN is_wr,
                               RTOS_ERR    *p_err)
{
  FS_FAT_JOURNAL_DATA *p_jnl_data;
  CPU_INT08U          *p_buf;
  FS_FAT_FILE_SIZE    file_pos_end;
  CPU_SIZE_T          cur_log_pos;
  CPU_SIZE_T          rd_wr_size;
  CPU_SIZE_T          rem_size;
  FS_FAT_SEC_BYTE_POS cur_pos;
  FS_FAT_SEC_SIZE     lb_size;
  FS_FLAGS            mode;

  WITH_SCOPE_BEGIN(p_err) {
    p_jnl_data = &p_fat_vol->JournalData;

                                                                // ----------------- CHK IF VALID POS -----------------
    file_pos_end = p_jnl_data->File.CurPos + len;
#if (FS_TEST_FAT_JNL_EN == DEF_ENABLED)
      if (FS_Test_FAT_JournalTestNumber == FSVOL_OPEN) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_ASSERT_CRITICAL_FAIL);    // Simulate journal file over FS_FAT_JOURNAL_FILE_LEN.
        return;
      }
#endif
    if (file_pos_end > FS_FAT_JOURNAL_FILE_LEN) {
      LOG_ERR(("Attempt to rd beyond journal file end."));
      RTOS_ERR_SET(*p_err, RTOS_ERR_ASSERT_CRITICAL_FAIL);
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, ;);
    }

    BREAK_ON_ERR(lb_size = FSBlkDev_LbSizeGet(p_fat_vol->Vol.BlkDevHandle, p_err));

    rem_size = len;
    cur_pos = p_jnl_data->File.CurSecBytePos;
    cur_log_pos = 0u;

    BREAK_ON_ERR (do ) {
      if (is_wr) {
        rd_wr_size = DEF_MIN((lb_size - cur_pos.SecOffset), rem_size);
        mode = (rd_wr_size != lb_size) ? FS_CACHE_BLK_GET_MODE_RW : FS_CACHE_BLK_GET_MODE_WR;
        BREAK_ON_ERR(FS_VOL_CACHE_BLK_ADD) (&p_fat_vol->Vol,
                                            cur_pos.SecNbr,
                                            FS_FAT_LB_TYPE_JNL_DATA,
                                            mode,
                                            p_fat_vol->JournalData.JnlWrJobHandle,
                                            &p_buf,
                                            &p_fat_vol->JournalData.JnlWrJobHandle,
                                            p_err) {
                                                                // Copy log into buf.
          Mem_Copy((void *)(p_buf + cur_pos.SecOffset),
                   (void *)((CPU_INT08U *)p_log + cur_log_pos),
                   rd_wr_size);
        }
      } else {
        rd_wr_size = DEF_MIN((lb_size - cur_pos.SecOffset), rem_size);
        BREAK_ON_ERR(FS_VOL_CACHE_BLK_RD) (&p_fat_vol->Vol,
                                           cur_pos.SecNbr,
                                           FS_FAT_LB_TYPE_DIRENT,
                                           &p_buf,
                                           p_err) {
                                                                // ----------------- COPY TO DEST BUF -----------------
          Mem_Copy((void *)((CPU_INT08U *)p_log + cur_log_pos),
                   (void *)(p_buf + cur_pos.SecOffset),
                   rd_wr_size);
        }
      }

                                                                // ----------- UPDATE SEC POS AND REM SIZE ------------
      cur_pos.SecOffset = (cur_pos.SecOffset + rd_wr_size) & (lb_size - 1u);
      rem_size -= rd_wr_size;
      cur_log_pos += rd_wr_size;

                                                                // ------------------- GET NEXT SEC -------------------
      if ((cur_pos.SecOffset == 0u) && (rem_size != 0u)) {
        BREAK_ON_ERR(cur_pos.SecNbr = FS_FAT_SecNextGet(p_fat_vol,
                                                        cur_pos.SecNbr,
                                                        p_err));
      }
    } while (rem_size != 0u);


                                                                // -------------------- UPDATE POS --------------------
    p_jnl_data->File.CurPos = file_pos_end;
    p_jnl_data->File.CurSecBytePos = cur_pos;
  } WITH_SCOPE_END
}
//                                                                 *INDENT-ON*
/****************************************************************************************************//**
 *                                           FS_FAT_JournalRd_INT16U()
 *
 * @brief    Read 16-bit value from journal & increment file position.
 *
 * @param    p_fat_vol   Pointer to volume.
 *
 * @param    p_err       Error pointer.
 *
 * @return   16-bit value read from journal.
 *******************************************************************************************************/
static CPU_INT16U FS_FAT_JournalRd_INT16U(FS_FAT_VOL *p_fat_vol,
                                          RTOS_ERR   *p_err)
{
  CPU_INT16U val;
  CPU_INT08U log[2u];

  FS_FAT_JournalRd(p_fat_vol, &log[0], 2u, p_err);

  val = MEM_VAL_GET_INT16U_LITTLE(&log[0]);

  return (val);
}

/****************************************************************************************************//**
 *                                           FS_FAT_JournalRd_INT32U()
 *
 * @brief    Read 32-bit value from journal & increment file position.
 *
 * @param    p_fat_vol   Pointer to volume.
 *
 * @param    p_err       Error pointer.
 *
 * @return   32-bit value read from journal.
 *******************************************************************************************************/
static CPU_INT32U FS_FAT_JournalRd_INT32U(FS_FAT_VOL *p_fat_vol,
                                          RTOS_ERR   *p_err)
{
  CPU_INT32U val;
  CPU_INT08U log[4u];

  FS_FAT_JournalRd(p_fat_vol, &log[0], 4u, p_err);

  val = MEM_VAL_GET_INT32U_LITTLE(&log[0]);

  return (val);
}

/****************************************************************************************************//**
 *                                       FS_FAT_JournalReverseScan()
 *
 * @brief    Scan journal backward, looking for given pattern.
 *
 * @param    p_fat_vol       Pointer to volume.
 *
 * @param    start_pos       Position to start scan at.
 *
 * @param    pattern         Pattern to look for.
 *
 * @param    pattern_size    Pattern size in octets.
 *
 * @param    p_err           Error pointer.
 *
 * @return   Start position of first pattern occurrence if match occurs. Zero if no match.
 *******************************************************************************************************/
static CPU_SIZE_T FS_FAT_JournalReverseScan(FS_FAT_VOL *p_fat_vol,
                                            CPU_SIZE_T start_pos,
                                            CPU_INT08U *p_pattern,
                                            CPU_SIZE_T pattern_size,
                                            RTOS_ERR   *p_err)
{
  CPU_INT08U octet_val;
  CPU_INT08U *p_pattern_pos;
  CPU_INT08U *p_pattern_start;
  CPU_INT08U *p_pattern_end;
  CPU_SIZE_T cur_pos;
  CPU_SIZE_T found_pos;

  WITH_SCOPE_BEGIN(p_err) {
    //                                                             ----------------- SET PARTTERN POS -----------------
    p_pattern_start = (CPU_INT08U *)p_pattern;
    p_pattern_end = p_pattern_start + pattern_size - 1u;
    p_pattern_pos = p_pattern_end;

    RTOS_ASSERT_CRITICAL((start_pos < FS_FAT_JOURNAL_FILE_LEN), RTOS_ERR_ASSERT_CRITICAL_FAIL, (CPU_SIZE_T)-1);

    found_pos = (CPU_SIZE_T)-1;
    cur_pos = start_pos + 1u;
    BREAK_ON_ERR(while) (cur_pos != 0u) {
      //                                                           ---------------- PEEK AT NEXT OCTET ----------------
      cur_pos--;
      BREAK_ON_ERR(FS_FAT_JournalPosSet(p_fat_vol,              // Set pos.
                                        cur_pos,
                                        p_err));

      BREAK_ON_ERR(FS_FAT_JournalRd(p_fat_vol,                  // Rd journal.
                                    &octet_val,
                                    1u,
                                    p_err));

      //                                                           ------------------- CHK IF MATCH -------------------
      if (octet_val == *p_pattern_pos) {                        // If octet match occurs ...
        if (p_pattern_pos == p_pattern_start) {                 // ... if pattern match occurs ...
          found_pos = cur_pos;
          break;
        }                                                       // ... if no pattern match occurs ...
        p_pattern_pos--;                                        // ... look for next octet.
      } else {                                                  // If no octet match occurs ...
        p_pattern_pos = p_pattern_end;                          // ... reset cur pattern pos.
      }
    }
  } WITH_SCOPE_END

  return (found_pos);
}

/****************************************************************************************************//**
 *                                           FS_FAT_JournalReplay()
 *
 * @brief    Replay journal. Revert partially completed top level FAT operations involving cluster
 *           chain allocation and/or directory entry creation/deletion. Top level FAT operations
 *           involving cluster chain deletion will be completed.
 *
 * @param    p_fat_vol   Pointer to volume.
 *
 * @param    p_err       Error pointer.
 *
 * @note     (1) The journal's on-disk layout is as follows:
 *
 *                                       .---------------------------------.
 *                                       |       TOP LVL ENTER MARK        |  (a)
 *                                       '---------------------------------'
 *                                       |       LOW LVL ENTER MARK        |  (b)
 *                                       .---------------------------------.
 *                                       |        LOW LVL OP A SIG         |  (c)
 *                                       .---------------------------------.
 *                                       |                                 |
 *                                       |        LOW LVL OP A DATA        |  (d)
 *                                       |                                 |
 *                                       '---------------------------------'
 *                                       |     LOW LVL ENTER END MARK      |  (e)
 *                                       .---------------------------------.
 *                                       |       LOW LVL ENTER MARK        |
 *                                       .---------------------------------.
 *                                       |        LOW LVL OP B SIG         |
 *                                       .---------------------------------.
 *                                       |                                 |
 *                                       |        LOW LVL OP B DATA        |
 *                                       |                                 |
 *                                       '---------------------------------'
 *                                       |     LOW LVL ENTER END MARK      |
 *                                       .---------------------------------.
 *                                       |       TOP LVL ENTER MARK        |
 *                                       .---------------------------------.
 *                                       |       LOW LVL ENTER MARK        |
 *                                       .---------------------------------.
 *                                       |        LOW LVL OP C SIG         |
 *                                       .---------------------------------.
 *                                       |                                 |
 *                                       |        LOW LVL OP C DATA        |
 *                                       |                                 |
 *                                       '---------------------------------'
 *                                       |     LOW LVL ENTER END MARK      |
 *                                       '---------------------------------'
 *
 *           - (a) A top level operation enter mark is written to the journal before each top level
 *                   FAT operation, namely:
 *
 *               - (i)   FS_FAT_FileWr()
 *                   (ii)  FS_FAT_FileTruncate()
 *                   (iii) FS_FAT_FileExtend()
 *                   (iv)  FS_FAT_EntryCreate()
 *               - (v)   FS_FAT_EntryDel()
 *
 *                   This mark allows top level operations involving cluster chain deletions to be
 *                   skipped after the cluster chain deletion has been successfully replayed.
 *
 *           - (b) A low level enter mark is written to the journal before each low level FAT
 *                   operation. This mark allows for efficient journal entry start location.
 *
 *           - (c) An low level operation signature is needed to unambiguously parse logs.
 *
 *           - (d) Data needed for low level operation reversion/completion is written to journal
 *                   before the low level operation is performed.
 *
 *           - (e) A low level operation enter end mark is written to journal after each low level
 *                   operation. This mark makes is needed to determined journal log completeness.
 *
 * @note     (2) If the first 16-bit word read from the journal is not an enter mark, the journal is
 *               clear or has been partially cleared. This indicates that the last top level FAT operation
 *               previously performed has been completed. Therefore, no operation needs to be replayed/
 *               reverted and the journal is cleared.
 *
 * @note     (3) Since reverting a cluster chain deletion basically involves logging all deleted clusters,
 *               it is practically impossible to implement without huge performance penalty.
 *
 *           - (a) Cluster chain deletion is the only low level FAT operation that is completed upon replay.
 *                   To accommodate this exception, the FAT layer is designed so that cluster chain deletion
 *                   is always the last low level operation performed inside the containing top level
 *                   operation.
 *
 *           - (b) Since the journal is replayed backward, the cluster chain deletion log is the
 *                   first log to be parsed. Once cluster chain deletion is completed, the containing
 *                   top level operation is also completed (see Note 3a) and the corresponding log
 *                   may be skipped.
 *
 * @note     (4) Journal is cleared from first to last sector. This allows the journal to be
 *               atomically invalidated since a single sector clear will remove the first enter
 *               mark. If this mark is not present, journal will not be replayed.
 *
 *******************************************************************************************************/
static void FS_FAT_JournalReplay(FS_FAT_VOL *p_fat_vol,
                                 RTOS_ERR   *p_err)
{
  FS_FAT_JOURNAL_DATA *p_jnl_data;
  CPU_INT16U          mark;
  CPU_INT16U          sig;
  CPU_INT08U          buf[4u];
  CPU_SIZE_T          journal_pos;
  CPU_BOOLEAN         jnl_bounded;

  WITH_SCOPE_BEGIN(p_err) {
    p_jnl_data = &p_fat_vol->JournalData;

    //                                                             ------------------- REPLAY START -------------------
    p_jnl_data->Replaying = DEF_YES;
    //                                                             ---------- CHK IF JOURNAL HAS BEEN CLR'D -----------
    BREAK_ON_ERR(FS_FAT_JournalPeek(p_fat_vol,                  // See Note #2.
                                    &buf[0],
                                    0u,
                                    FS_FAT_JOURNAL_LOG_MARK_SIZE,
                                    p_err));

    mark = MEM_VAL_GET_INT16U_LITTLE(&buf[0]);                  // If first enter mark is not found ...
    if (mark != FS_FAT_JOURNAL_MARK_TOP_LVL_ENTER) {            // ... then journal has been (partially) clr'd.
      jnl_bounded = false;
      goto exit;
    }

    //                                                             ----- FIND LAST COMPLETE ENTRY ENTER END MARK ------
    MEM_VAL_SET_INT16U_LITTLE(&buf[0], FS_FAT_JOURNAL_MARK_ENTER_END);
    BREAK_ON_ERR(journal_pos = FS_FAT_JournalReverseScan(p_fat_vol,
                                                         FS_FAT_JOURNAL_FILE_LEN - 1u,
                                                         &buf[0],
                                                         FS_FAT_JOURNAL_LOG_MARK_SIZE,
                                                         p_err));

    //                                                             If no enter end mark is found then journal does not contain a complete entry.
    if (journal_pos == (CPU_SIZE_T)-1) {
      jnl_bounded = false;
      goto exit;
    }

    //                                                             Boundary markers have been properly detected, proceed with replay.
    jnl_bounded = true;
    BREAK_ON_ERR(while) (journal_pos > 0u) {
      //                                                           --------------- FIND NEXT LOG ENTRY ----------------
      MEM_VAL_SET_INT16U_LITTLE(&buf[0], FS_FAT_JOURNAL_MARK_ENTER);
      BREAK_ON_ERR(journal_pos = FS_FAT_JournalReverseScan(p_fat_vol,
                                                           journal_pos,
                                                           &buf[0],
                                                           FS_FAT_JOURNAL_LOG_MARK_SIZE,
                                                           p_err));
      if (journal_pos == (CPU_SIZE_T)-1) {
        break;
      }

      //                                                           --------- SET POS AT LOG ENTRY ENTER MARK ----------
      BREAK_ON_ERR(FS_FAT_JournalPosSet(p_fat_vol,
                                        journal_pos,
                                        p_err));

      //                                                           --------------------- GET SIG ----------------------
      BREAK_ON_ERR(FS_FAT_JournalPeek(p_fat_vol,                // Enter mark presence already asserted by prev scan ...
                                      &buf[0],                  // ... so skip enter mark.
                                      p_jnl_data->File.CurPos + FS_FAT_JOURNAL_LOG_MARK_SIZE,
                                      FS_FAT_JOURNAL_LOG_SIG_SIZE,
                                      p_err));

      sig = MEM_VAL_GET_INT16U_LITTLE(&buf[0]);

      //                                                           ---------------------- REPLAY ----------------------
      BREAK_ON_ERR(switch) (sig) {
        case FS_FAT_JOURNAL_SIG_CLUS_CHAIN_ALLOC:               // Revert clus chain alloc.
          BREAK_ON_ERR(FS_FAT_JournalRevertClusChainAlloc(p_fat_vol, p_err));
          break;

        case FS_FAT_JOURNAL_SIG_CLUS_CHAIN_DEL:                 // Complete clus chain del (See Note #3a).
          BREAK_ON_ERR(FS_FAT_JournalReplayClusChainDel(p_fat_vol, p_err));

          //                                                       Jump to the prev top lvl op (See Note #3b).
          MEM_VAL_SET_INT16U_LITTLE(&buf[0], FS_FAT_JOURNAL_MARK_TOP_LVL_ENTER);
          BREAK_ON_ERR(journal_pos = FS_FAT_JournalReverseScan(p_fat_vol,
                                                               journal_pos,
                                                               &buf[0],
                                                               FS_FAT_JOURNAL_LOG_MARK_SIZE,
                                                               p_err));
          if (journal_pos == (CPU_SIZE_T)-1) {
            break;
          }

          BREAK_ON_ERR(FS_FAT_JournalPosSet(p_fat_vol,
                                            journal_pos,
                                            p_err));
          break;

        case FS_FAT_JOURNAL_SIG_ENTRY_CREATE:                   // Revert dir entry creation.
          BREAK_ON_ERR(FS_FAT_JournalRevertEntryCreate(p_fat_vol, p_err));
          break;

        case FS_FAT_JOURNAL_SIG_ENTRY_UPDATE:                   // Revert dir entry update/del.
          BREAK_ON_ERR(FS_FAT_JournalRevertEntryUpdate(p_fat_vol, p_err));
          break;

        default:
          LOG_ERR(("Unknown journal sig: ", (X)sig, "."));
          RTOS_ERR_SET(*p_err, RTOS_ERR_VOL_CORRUPTED);
          break;
      }
    }

    //                                                             ------------------- CLR JOURNAL --------------------
exit:

    BREAK_ON_ERR(FSCache_Invalidate(p_fat_vol->Vol.CacheBlkDevDataPtr->CachePtr,
                                    p_fat_vol->Vol.BlkDevHandle,
                                    p_err));

    if (jnl_bounded) {
      BREAK_ON_ERR(FS_FAT_JournalClr(p_fat_vol,                   // Clear entire journal (See Note #4).
                                     0u,
                                     FS_FAT_JOURNAL_FILE_LEN,
                                     p_err));
    }

    BREAK_ON_ERR(FS_FAT_JournalPosSet(p_fat_vol,                  // Reset position.
                                      0u,
                                      p_err));
  } WITH_SCOPE_END
  //                                                               ------------------- REPLAY DONE --------------------
  p_jnl_data->Replaying = DEF_NO;
}

/****************************************************************************************************//**
 *                                   FS_FAT_JournalRevertClusChainAlloc()
 *
 * @brief    Revert cluster chain allocation based on journal log information.
 *
 * @param    p_fat_vol   Pointer to volume.
 *
 * @param    p_err       Error pointer.
 *
 * @note     (1) Enter mark, enter end mark and signature are validated by FS_FAT_JournalReplay().
 *               This function is called only if the corresponding journal log is complete.
 *
 * @note     (2) Log arguments are strictly validated.  If any error exists, the journal replay will
 *               be aborted.
 *******************************************************************************************************/
static void FS_FAT_JournalRevertClusChainAlloc(FS_FAT_VOL *p_fat_vol,
                                               RTOS_ERR   *p_err)
{
  FS_FAT_CLUS_NBR start_clus;
  CPU_BOOLEAN     del_first;
  CPU_INT08U      log[FS_FAT_JOURNAL_LOG_CLUS_CHAIN_ALLOC_SIZE];

  WITH_SCOPE_BEGIN(p_err) {
    //                                                             ------------------ PARSE LOG ARGS ------------------
    BREAK_ON_ERR(FS_FAT_JournalRd(p_fat_vol,
                                  &log[0],
                                  FS_FAT_JOURNAL_LOG_CLUS_CHAIN_ALLOC_SIZE,
                                  p_err));

    //                                                             Ignore marks & sig (See Note #1).
    start_clus = (FS_FAT_CLUS_NBR)MEM_VAL_GET_INT32U_LITTLE((void *)&log[FS_FAT_JOURNAL_CLUS_CHAIN_ALLOC_LOG_START_CLUS_OFFSET]);
    del_first = (CPU_BOOLEAN)MEM_VAL_GET_INT08U_LITTLE((void *)&log[FS_FAT_JOURNAL_CLUS_CHAIN_ALLOC_LOG_IS_NEW_OFFSET]);

    //                                                             VALIDATE LOG ARGS (see Note #2)
    ASSERT_BREAK(FS_FAT_IS_VALID_CLUS(p_fat_vol, start_clus), RTOS_ERR_VOL_CORRUPTED);
    ASSERT_BREAK((del_first == DEF_YES) || (del_first == DEF_NO), RTOS_ERR_VOL_CORRUPTED);

    //                                                             ------------------- REVERT ALLOC -------------------
    LOG_DBG(("FS_FAT_JournalRevertClusChainAlloc(): Reverting op for ", (X)FS_FAT_JOURNAL_SIG_CLUS_CHAIN_ALLOC, "."));
    FS_FAT_ClusChainReverseDel(p_fat_vol,
                               start_clus,
                               del_first,
                               p_err);

#if 0
    //                                                             If clus chain creation was not even started ...
    //                                                             ... then clus invalid err is expected.
    if (del_first && (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_FAT_CLUS_INVALID)) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    }
#endif
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                   FS_FAT_JournalReplayClusChainDel()
 *
 * @brief    Complete cluster chain deletion based on journal log information.
 *
 * @param    p_fat_vol   Pointer to a FAT volume.
 *
 * @param    p_err       Error pointer.
 *
 * @note     (1) Enter mark, enter end mark and signature are validated by FS_FAT_JournalReplay().
 *               This function is called only if the corresponding journal log is complete.
 *
 * @note     (2) Log arguments are strictly validated.  If any error exists, the journal replay will
 *               be aborted.
 *
 * @note     (3) The start clus is handled separatly from the rest of the cluster chain for 2 different
 *               reasons, in 2 different scenarios:
 *
 *               - (a) In case a valid marker is found: if FAT12 is used, a FAT entry may span across
 *                       2 sectors. This could lead to corrupted entry if a failure occurs between the 2
 *                       sector write needed for the entry update. Although no such entry will be used when
 *                       journaling is enabled (see FS_FAT_ClusFreeFind() notes) and mounting journaled
 *                       volume under another host is strongly discouraged, the integrity of the end of
 *                       cluster mark is enforced here for extra safety.
 *
 *               - (b) In case no markers were logged: if no markers were logged, the start cluster is
 *                       the only cluster to be deleted. Since there are no markers to start first cluster
 *                       lookup from, start cluster needs to be treated separately.
 *******************************************************************************************************/
static void FS_FAT_JournalReplayClusChainDel(FS_FAT_VOL *p_fat_vol,
                                             RTOS_ERR   *p_err)
{
  FS_FAT_TYPE_API  *p_fat_api;
  CPU_BOOLEAN      del;
  CPU_BOOLEAN      del_first;
  CPU_BOOLEAN      valid_marker_found;
  FS_FAT_CLUS_NBR  first_clus;
  FS_FAT_CLUS_NBR  start_clus;
  FS_FAT_CLUS_NBR  cur_marker = 0u;
  FS_FAT_CLUS_NBR  fat_entry;
  FS_FAT_CLUS_NBR  marker_cnt;
  FS_FAT_FILE_SIZE end_mark_pos;
  CPU_INT08U       log[FS_FAT_JOURNAL_LOG_CLUS_CHAIN_DEL_HEADER_SIZE];
  CPU_INT32U       mark;

  WITH_SCOPE_BEGIN(p_err) {
    p_fat_api = FS_FAT_TYPE_API_TBL[p_fat_vol->FAT_Type];

    //                                                             ------------------ PARSE LOG ARGS ------------------
    BREAK_ON_ERR(FS_FAT_JournalRd(p_fat_vol,
                                  &log[0],
                                  FS_FAT_JOURNAL_LOG_CLUS_CHAIN_DEL_HEADER_SIZE,
                                  p_err));

    //                                                             Ignore marks & sig (see Note #1).
    marker_cnt = (FS_FAT_CLUS_NBR)MEM_VAL_GET_INT32U_LITTLE((void *)&log[FS_FAT_JOURNAL_CLUS_CHAIN_DEL_LOG_MARKER_CNT_OFFSET]);
    start_clus = (FS_FAT_CLUS_NBR)MEM_VAL_GET_INT32U_LITTLE((void *)&log[FS_FAT_JOURNAL_CLUS_CHAIN_DEL_LOG_START_CLUS_OFFSET]);
    del_first = (CPU_BOOLEAN)MEM_VAL_GET_INT08U_LITTLE((void *)&log[FS_FAT_JOURNAL_CLUS_CHAIN_DEL_LOG_DEL_FIRST_OFFSET]);

    //                                                             VALIDATE LOG ARGS (see Note #2)
    ASSERT_BREAK(marker_cnt <= p_fat_vol->ClusCnt,
                 RTOS_ERR_VOL_CORRUPTED);

    ASSERT_BREAK(FS_FAT_IS_VALID_CLUS(p_fat_vol, start_clus),
                 RTOS_ERR_VOL_CORRUPTED);

    ASSERT_BREAK((del_first == DEF_YES) || (del_first == DEF_NO),
                 RTOS_ERR_VOL_CORRUPTED);

    //                                                             ------------ COMPUTE ENTER END MARK POS ------------
    end_mark_pos = p_fat_vol->JournalData.File.CurPos + marker_cnt
                   * (p_fat_vol->FAT_Type == FS_FAT_TYPE_FAT32 ? 4u : 2u);

    //                                                             CHK ENTER END MARK LOCATION (see Note #2)
    ASSERT_BREAK(end_mark_pos <= FS_FAT_JOURNAL_FILE_LEN,       // Chk if end mark pos is valid.
                 RTOS_ERR_VOL_CORRUPTED);

    BREAK_ON_ERR(FS_FAT_JournalPeek(p_fat_vol,                  // Peek at enter end mark.
                                    &log[0],
                                    end_mark_pos,
                                    FS_FAT_JOURNAL_LOG_MARK_SIZE,
                                    p_err));

    mark = MEM_VAL_GET_INT16U_LITTLE(&log[0]);
    ASSERT_BREAK(mark == FS_FAT_JOURNAL_MARK_ENTER_END,         // If no end mark where one is expected ...
                 RTOS_ERR_VOL_CORRUPTED);                       // ... rtn err.

    //                                                             ------------- FIND FIRST VALID MARKER --------------
    valid_marker_found = DEF_NO;
    BREAK_ON_ERR(while) (!valid_marker_found
                         && (p_fat_vol->JournalData.File.CurPos < end_mark_pos)) {
      if (p_fat_vol->FAT_Type == FS_FAT_TYPE_FAT12) {
        BREAK_ON_ERR(cur_marker = FS_FAT_JournalRd_INT16U(p_fat_vol, p_err));
        cur_marker &= 0xFFFu;
      } else if (p_fat_vol->FAT_Type == FS_FAT_TYPE_FAT16 ) {
        BREAK_ON_ERR(cur_marker = FS_FAT_JournalRd_INT16U(p_fat_vol, p_err));
      } else {
        BREAK_ON_ERR(cur_marker = FS_FAT_JournalRd_INT32U(p_fat_vol, p_err));
      }

      //                                                           VALIDATE LOG ARGS (see Note #2)
      ASSERT_BREAK(FS_FAT_IS_VALID_CLUS(p_fat_vol, cur_marker),
                   RTOS_ERR_VOL_CORRUPTED);

      //                                                           ------------------- RD FAT ENTRY -------------------
      BREAK_ON_ERR(fat_entry = p_fat_api->ClusValRd(p_fat_vol,
                                                    cur_marker,
                                                    p_err));

      //                                                           Chk if valid marker found.
      if (FS_FAT_IS_VALID_CLUS(p_fat_vol, fat_entry) || (fat_entry >= p_fat_api->ClusEOF)) {
        valid_marker_found = DEF_YES;
      }
    }

    //                                                             -------------- NO VALID MARKER FOUND ---------------
    if (!valid_marker_found && (marker_cnt > 0u)) {             // If no valid marker found among log'd markers ...
      return;                                                   // ... everything has already been del'd.
    }

    //                                                             --- VALID MARKER FOUND OR NO MARKERS WERE LOG'D ----
    //                                                             ------------------ CHK START CLUS ------------------
    //                                                             See Note #3a & 3b.
    BREAK_ON_ERR(fat_entry = p_fat_api->ClusValRd(p_fat_vol,    // Rd start of chain entry.
                                                  start_clus,
                                                  p_err));

    if ((fat_entry < p_fat_api->ClusEOF) && !del_first) {       // If start clus has not been mark'd as EOC ...
                                                                // ... while it should be mark it as EOC.
      BREAK_ON_ERR(p_fat_api->ClusValWr(p_fat_vol,
                                        start_clus,
                                        p_fat_api->ClusEOF,
                                        p_err));
    }

    if ((fat_entry != p_fat_api->ClusFree) && del_first) {      // If start clus has not been mark'd as free ...
                                                                // ... while it should be, mark it as free.
      BREAK_ON_ERR(p_fat_api->ClusValWr(p_fat_vol,
                                        start_clus,
                                        p_fat_api->ClusFree,
                                        p_err));
    }

    if (marker_cnt > 0u) {                                               // If no markers were log'd ...
                                                                         // nothing to do beyond start clus handling, else ...

      //                                                           ---------- FIND BROKEN CHAIN START & DEL -----------
      BREAK_ON_ERR(first_clus = FS_FAT_ClusChainReverseFollow(p_fat_vol, // Find rem clus chain start ...
                                                              cur_marker,
                                                              start_clus,
                                                              p_err));

      //                                                           ... and del.
      del = (first_clus == start_clus) ? del_first : DEF_YES;
      BREAK_ON_ERR((void)FS_FAT_ClusChainDel(p_fat_vol,
                                             first_clus,
                                             del,
                                             p_err));

#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
      {
        FS_CACHE *p_cache_data = p_fat_vol->Vol.CacheBlkDevDataPtr->CachePtr;
        //                                                         Flush FAT blk.
        BREAK_ON_ERR(FSCache_WrJobExec(p_cache_data,
                                       p_fat_vol->FatWrJobHandle,
                                       DEF_NO,
                                       p_err));

        //                                                         Flush jnl blk.
        BREAK_ON_ERR(FSCache_WrJobExec(p_cache_data,
                                       p_fat_vol->JournalData.JnlWrJobHandle,
                                       DEF_NO,
                                       p_err));
      }
#endif
    }
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                       FS_FAT_JournalRevertEntryCreate()
 *
 * @brief    Revert directory entry creation based on journal log information.
 *
 * @param    p_fat_vol   Pointer to volume.
 *
 * @param    p_err       Error pointer.
 *
 * @note     (1) A LFN can require as many as
 *
 *                           [  FS_FAT_MAX_FILE_NAME_LEN   ]
 *                       ceil[-----------------------------] = ceil(255/13) = 20
 *                           [ FS_FAT_DIRENT_LFN_NBR_CHARS ]
 *
 *                   separate directory entries.  Each directory entry is 32-bytes, so a single file
 *                   name can occupy as many as 640 bytes of a directory & up to one full cluster
 *                   plus part of another may be consumed.  Consequently, as many as two clusters may
 *                   be allocated for the directory entry.
 *
 * @note     (2) Enter mark, enter end mark and signature are validated by FS_FAT_JournalReplay().
 *               This function is called only if the corresponding journal log is complete.
 *******************************************************************************************************/
static void FS_FAT_JournalRevertEntryCreate(FS_FAT_VOL *p_fat_vol,
                                            RTOS_ERR   *p_err)
{
  FS_FAT_SEC_BYTE_POS    dir_start_pos;
  FS_FAT_SEC_BYTE_POS    dir_end_pos;
  FS_CACHE_WR_JOB_HANDLE entry_wr_job = FS_CACHE_WR_JOB_HANDLE_INIT;
  CPU_INT08U             log[FS_FAT_JOURNAL_LOG_ENTRY_CREATE_SIZE];

  WITH_SCOPE_BEGIN(p_err) {
    //                                                             ------------------ PARSE LOG ARGS ------------------
    BREAK_ON_ERR(FS_FAT_JournalRd(p_fat_vol,
                                  &log[0],
                                  FS_FAT_JOURNAL_LOG_ENTRY_CREATE_SIZE,
                                  p_err));

    //                                                             Ignore marks & sig (see Note #2).
    dir_start_pos.SecNbr = (FS_FAT_SEC_NBR)MEM_VAL_GET_INT32U_LITTLE((void *)&log[FS_FAT_JOURNAL_ENTRY_CREATE_LOG_START_SEC_NBR_OFFSET]);
    dir_start_pos.SecOffset = (FS_FAT_SEC_SIZE)MEM_VAL_GET_INT32U_LITTLE((void *)&log[FS_FAT_JOURNAL_ENTRY_CREATE_LOG_START_SEC_POS_OFFSET]);
    dir_end_pos.SecNbr = (FS_FAT_SEC_NBR)MEM_VAL_GET_INT32U_LITTLE((void *)&log[FS_FAT_JOURNAL_ENTRY_CREATE_LOG_END_SEC_NBR_OFFSET]);
    dir_end_pos.SecOffset = (FS_FAT_SEC_SIZE)MEM_VAL_GET_INT32U_LITTLE((void *)&log[FS_FAT_JOURNAL_ENTRY_CREATE_LOG_END_SEC_POS_OFFSET]);

    //                                                             ------------------ DEL DIR ENTRY -------------------
    BREAK_ON_ERR(FS_FAT_DirEntriesErase(p_fat_vol,
                                        &dir_start_pos,
                                        &dir_end_pos,
                                        &entry_wr_job,
                                        p_err));

#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
    {
      FS_CACHE *p_cache_data = p_fat_vol->Vol.CacheBlkDevDataPtr->CachePtr;
      //                                                           Flush FAT blk.
      BREAK_ON_ERR(FSCache_WrJobExec(p_cache_data,
                                     p_fat_vol->FatWrJobHandle,
                                     DEF_NO,
                                     p_err));

      //                                                           Flush entry blk.
      BREAK_ON_ERR(FSCache_WrJobExec(p_cache_data,
                                     entry_wr_job,
                                     DEF_NO,
                                     p_err));
    }
#endif
  } WITH_SCOPE_END
}

/********************************************************************************************************
 *                                   FS_FAT_JournalRevertEntryUpdate()
 *
 * Description : Revert directory entry update/deletion based on journal log information.
 *
 * Argument(s) : p_fat_vol       Pointer to a FAT volume.
 *
 *               p_err           Error pointer.
 *
 * Return(s)   : none.
 *
 * Note(s)       (1) Enter mark, enter end mark and signature are validated by FS_FAT_JournalReplay().
 *                   This function is called only if the corresponding journal log is complete.
 *******************************************************************************************************/
static void FS_FAT_JournalRevertEntryUpdate(FS_FAT_VOL *p_fat_vol,
                                            RTOS_ERR   *p_err)
{
  FS_FAT_SEC_BYTE_POS               start_dte_pos;
  FS_FAT_SEC_BYTE_POS               end_dte_pos;
  FS_FAT_SEC_BYTE_POS               cur_dte_pos;
  CPU_INT08U                        log[DEF_MAX(FS_FAT_SIZE_DIR_ENTRY, FS_FAT_JOURNAL_LOG_ENTRY_DEL_HEADER_SIZE)];
  FS_FAT_DTE_UPDATE_JNL_LOG_CB_DATA update_jnl_log_data;

  WITH_SCOPE_BEGIN(p_err) {
    //                                                             ------------------ PARSE LOG ARGS ------------------
    BREAK_ON_ERR(FS_FAT_JournalRd(p_fat_vol,
                                  &log[0],
                                  FS_FAT_JOURNAL_LOG_ENTRY_DEL_HEADER_SIZE,
                                  p_err));

    //                                                             Ignore marks & sig (see Note #1).
    start_dte_pos.SecNbr = (FS_FAT_SEC_NBR) MEM_VAL_GET_INT32U_LITTLE((void *)&log[FS_FAT_JOURNAL_ENTRY_DEL_LOG_START_SEC_NBR_OFFSET]);
    start_dte_pos.SecOffset = (FS_FAT_SEC_SIZE)MEM_VAL_GET_INT32U_LITTLE((void *)&log[FS_FAT_JOURNAL_ENTRY_DEL_LOG_START_SEC_POS_OFFSET]);
    end_dte_pos.SecNbr = (FS_FAT_SEC_NBR) MEM_VAL_GET_INT32U_LITTLE((void *)&log[FS_FAT_JOURNAL_ENTRY_DEL_LOG_END_SEC_NBR_OFFSET]);
    end_dte_pos.SecOffset = (FS_FAT_SEC_SIZE)MEM_VAL_GET_INT32U_LITTLE((void *)&log[FS_FAT_JOURNAL_ENTRY_DEL_LOG_END_SEC_POS_OFFSET]);

    cur_dte_pos = start_dte_pos;

    update_jnl_log_data.BufPtr = &log[0];
    update_jnl_log_data.EndDtePos = end_dte_pos;

    BREAK_ON_ERR(FS_FAT_DirTblBrowse(p_fat_vol,
                                     &cur_dte_pos,
                                     FS_FAT_JournalDteRestore,
                                     &update_jnl_log_data,
                                     p_err));

#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
    {
      //                                                           Flush jnl cache blks.
      FS_CACHE *p_cache = p_fat_vol->Vol.CacheBlkDevDataPtr->CachePtr;
      BREAK_ON_ERR(FSCache_WrJobExec(p_cache,
                                     p_fat_vol->JournalData.JnlWrJobHandle,
                                     DEF_NO,
                                     p_err));
    }
#endif
  } WITH_SCOPE_END
}

#endif // FS_CORE_CFG_FAT_EN && FS_CORE_CFG_JOURNAL_EN

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_FS_AVAIL
