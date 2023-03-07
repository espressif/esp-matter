/***************************************************************************//**
 * @file
 * @brief File System - Fat File Operations
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

//                                                                 ----------------------- CORE -----------------------
#include  <fs/source/shared/fs_utils_priv.h>
#include  <fs/source/core/fs_core_cache_priv.h>
#include  <fs/source/sys/fs_sys_priv.h>

//                                                                 ----------------------- FAT ------------------------
#include  <fs/source/shared/cleanup/cleanup_mgmt_priv.h>
#include  <fs/source/sys/fat/fs_fat_priv.h>
#include  <fs/source/sys/fat/fs_fat_vol_priv.h>
#include  <fs/source/sys/fat/fs_fat_file_priv.h>
#include  <fs/source/sys/fat/fs_fat_journal_priv.h>
#include  <fs/source/sys/fat/fs_fat_fatxx_priv.h>

//                                                                 ----------------------- EXT ------------------------
#include  <cpu/include/cpu.h>
#include  <common/include/lib_mem.h>
#include  <common/source/logging/logging_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH         (FS, FAT, FILE)
#define  RTOS_MODULE_CUR      RTOS_CFG_MODULE_FS

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static MEM_DYN_POOL FS_FAT_FileNodePool;
static MEM_DYN_POOL FS_FAT_FileDescPool;

#if (FS_TEST_FAT_JNL_EN == DEF_ENABLED)
extern CPU_INT08U FS_Test_FAT_JournalTestNumber;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static CPU_BOOLEAN FS_FAT_IsFilePosCacheSync(FS_FAT_FILE_DESC *p_fat_file_desc,
                                             FS_LB_NBR        pos_octet);

static CPU_BOOLEAN FS_FAT_IsFilePosCacheValid(FS_FAT_FILE_DESC *p_fat_file_desc);

static void FS_FAT_FilePosCacheSet(FS_FAT_FILE_DESC *p_fat_file_desc,
                                   FS_LB_NBR        lb_ix,
                                   RTOS_ERR         *p_err);

static void FS_FAT_FilePosCacheSync(FS_FAT_FILE_DESC *p_fat_file_desc,
                                    FS_LB_NBR        lb_ix,
                                    RTOS_ERR         *p_err);

static FS_LB_NBR FS_FAT_FileSecGet(FS_FAT_FILE_DESC *p_fat_file_desc,
                                   FS_LB_NBR        lb_offset,
                                   RTOS_ERR         *p_err);

static CPU_SIZE_T FS_FAT_FileRdWr(FS_FAT_FILE_DESC *p_fat_file_desc,
                                  CPU_SIZE_T       pos,
                                  CPU_INT08U       *p_data_buf,
                                  CPU_SIZE_T       size,
                                  CPU_BOOLEAN      is_wr,
                                  RTOS_ERR         *p_err);

static FS_FAT_CLUS_NBR FS_FAT_FileClusCntGet(FS_FILE_SIZE file_size,
                                             CPU_INT08U   clus_size_log2_octets);

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static CPU_SIZE_T FS_FAT_FileWrInternal(FS_FAT_FILE_DESC *p_fat_file_desc,
                                        CPU_SIZE_T       pos,
                                        CPU_INT08U       *p_src,
                                        CPU_SIZE_T       wr_size,
                                        RTOS_ERR         *p_err);
#endif

/********************************************************************************************************
 ********************************************************************************************************
   GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           FS_FAT_FileNodeAlloc()
 *
 * @brief    Allocate a FAT file node.
 *
 * @param    p_err   Error pointer.
 *
 * @return   Pointer to the allocated file node.
 *******************************************************************************************************/
FS_FILE_NODE *FS_FAT_FileNodeAlloc(RTOS_ERR *p_err)
{
  FS_FAT_FILE_NODE *p_fat_file_node;

  p_fat_file_node = (FS_FAT_FILE_NODE *)Mem_DynPoolBlkGet(&FS_FAT_FileNodePool, p_err);
  Mem_Clr(p_fat_file_node, sizeof(FS_FAT_FILE_NODE));

  p_fat_file_node->EntryEndPos = FS_FAT_VoidSecBytePos;
  p_fat_file_node->FirstClusNbr = 0u;
  p_fat_file_node->ShrinkSeqNo = 0u;
#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
  p_fat_file_node->DataWrJobHandle = FSCache_VoidWrJobHandle;
  p_fat_file_node->EntryWrJobHandle = FSCache_VoidWrJobHandle;
#endif

  return (&p_fat_file_node->FileNode);
}

/****************************************************************************************************//**
 *                                           FS_FAT_FileNodeFree()
 *
 * @brief    Free a FAT file node.
 *
 * @param    p_fat_file_node     Pointer to a FAT file node.
 *******************************************************************************************************/
void FS_FAT_FileNodeFree(FS_FILE_NODE *p_file_node)
{
  RTOS_ERR err;

  Mem_DynPoolBlkFree(&FS_FAT_FileNodePool, (void *)p_file_node, &err);
  RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}

/****************************************************************************************************//**
 *                                           FS_FAT_FileDescAlloc()
 *
 * @brief    Allocate a FAT file descriptor.
 *
 * @param    p_err   Error pointer.
 *
 * @return   Pointer to the allocated file descriptor.
 *******************************************************************************************************/
FS_FILE_DESC *FS_FAT_FileDescAlloc(RTOS_ERR *p_err)
{
  FS_FAT_FILE_DESC *p_fat_file_desc;

  p_fat_file_desc = (FS_FAT_FILE_DESC *)Mem_DynPoolBlkGet(&FS_FAT_FileDescPool, p_err);
  Mem_Clr(p_fat_file_desc, sizeof(FS_FAT_FILE_DESC));

  return (&p_fat_file_desc->FileDesc);
}

/****************************************************************************************************//**
 *                                           FS_FAT_FileDescFree()
 *
 * @brief    Free a FAT file descriptor.
 *
 * @param    p_fat_file_desc     Pointer to a FAT file descriptor.
 *******************************************************************************************************/
void FS_FAT_FileDescFree(FS_FILE_DESC *p_file_desc)
{
  RTOS_ERR err;

  Mem_DynPoolBlkFree(&FS_FAT_FileDescPool, (void *)p_file_desc, &err);
  RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}

/****************************************************************************************************//**
 *                                           FS_FAT_FileDescInit()
 *
 * @brief    Initialize file descriptor.
 *
 * @param    p_file_desc     Pointer to file descriptor.
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/
void FS_FAT_FileDescInit(FS_FILE_DESC *p_file_desc,
                         RTOS_ERR     *p_err)
{
  FS_FAT_FILE_DESC *p_fat_file_desc;

  PP_UNUSED_PARAM(p_err);

  p_fat_file_desc = (FS_FAT_FILE_DESC *)p_file_desc;
  p_fat_file_desc->CurClus = 0u;
  p_fat_file_desc->CurClusIx = (FS_FAT_CLUS_NBR)-1;
  p_fat_file_desc->ShrinkSeqNo = 0u;
}

/****************************************************************************************************//**
 *                                           FS_FAT_FileNodeInit()
 *
 * @brief    Initialize a file node by retrieving file entry information.
 *
 * @param    p_file_node     Pointer to file node.
 *
 * @param    entry_pos       File entry position.
 *
 * @param    p_err           Error pointer.
 *
 *           FS_FAT_API.
 *******************************************************************************************************/
void FS_FAT_FileNodeInit(FS_FILE_NODE *p_file_node,
                         FS_SYS_POS   entry_pos,
                         RTOS_ERR     *p_err)
{
  FS_FAT_VOL          *p_fat_vol;
  FS_FAT_FILE_NODE    *p_fat_file_node;
  FS_FAT_SEC_BYTE_POS de_start_pos;
  FS_FAT_SEC_BYTE_POS de_end_pos;
  FS_FAT_ENTRY_INFO   fat_entry_info;

  WITH_SCOPE_BEGIN(p_err) {
    p_fat_vol = (FS_FAT_VOL *)p_file_node->VolHandle.VolPtr;

    FS_FAT_POS_DATA_DECODE(entry_pos, de_start_pos);

    BREAK_ON_ERR(FS_FAT_EntryParse(p_fat_vol,
                                   &de_start_pos,
                                   &de_end_pos,
                                   &fat_entry_info,
                                   DEF_NULL,
                                   DEF_NULL,
                                   0u,
                                   p_err));

    p_fat_file_node = (FS_FAT_FILE_NODE *)p_file_node;
    p_fat_file_node->FirstClusNbr = fat_entry_info.FirstClusNbr;
    p_fat_file_node->EntryEndPos = de_end_pos;
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                               FS_FAT_FileRd()
 *
 * @brief    Read from file.
 *
 * @param    p_file_desc     Pointer to file descriptor.
 *
 * @param    pos             Offset within the file to read from.
 *
 * @param    p_dest          Pointer to destination buffer.
 *
 * @param    rd_size         Number of octets to read.
 *
 * @param    p_err           Error pointer.
 *
 * @return   Number of octets read.
 *******************************************************************************************************/
CPU_SIZE_T FS_FAT_FileRd(FS_FILE_DESC *p_file_desc,
                         CPU_SIZE_T   pos,
                         CPU_INT08U   *p_dest,
                         CPU_SIZE_T   rd_size,
                         RTOS_ERR     *p_err)
{
  FS_FAT_FILE_DESC *p_fat_file_desc;
  CPU_SIZE_T       size_rd;

  p_fat_file_desc = (FS_FAT_FILE_DESC *)p_file_desc;

  size_rd = FS_FAT_FileRdWr(p_fat_file_desc,
                            pos,
                            p_dest,
                            rd_size,
                            DEF_NO,
                            p_err);
  return (size_rd);
}

/****************************************************************************************************//**
 *                                               FS_FAT_FileWr()
 *
 * @brief    Write to file.
 *
 * @param    p_file_desc     Pointer to a file descriptor.
 *
 * @param    pos             Offset within the file to write from.
 *
 * @param    p_src           Pointer to the source buffer.
 *
 * @param    wr_size         Number of octets to write.
 *
 * @param    p_err           Error pointer.
 *
 * @return   Number of octets written.
 *
 * @note     (1) When the journal is disabled the ordering requirements for low-level FAT operations
 *               involved in file writing are those illustrated below.
 *
 *                           O (FAT)         O (entry)      O (data)
 *               .........................................................
 *                           :               :              :
 *               .........................................................
 *               Nth write     |               |              |
 *                           v               |       .------'------.
 *                       .------------.         |       |             |
 *                       | Clus alloc |         |       v             v
 *                       '------------'         |  .---------.   .---------.
 *                           |               |  | Data wr |   | Data wr |
 *                           v               |  '---------'   '---------'
 *                       .------------.         |       |             |
 *                       | Clus alloc |         |       '------.------'
 *                       '------------'         |              v
 *                           |               |            .---.
 *                           |               |            | S |
 *                           |               v            '---'
 *                           |       .--------------.       |
 *                           '------>| Entry update |<------|
 *                           |       '--------------'       |
 *                           |               |              |
 *               .........................................................
 *                           :               :              :
 *               .........................................................
 *               (N+1)th write |               |              |
 *                             v               |       .------'------.
 *                      .------------.         |       |             |
 *                      | Clus alloc |         |       v             v
 *                      '------------'         |  .---------.   .---------.
 *                             |               |  | Data wr |   | Data wr |
 *                             v               |  '---------'   '---------'
 *                      .------------.         |       |             |
 *                      | Clus alloc |         |       '------.------'
 *                      '------------'         |              v
 *                             |               |            .---.
 *                             |               |            | S |
 *                             |               v            '---'
 *                             |       .--------------.       |
 *                             '------>| Entry update |<------|
 *                             |       '--------------'       |
 *                             |               |              |
 *                 .........................................................
 *                             :               :              :
 *
 * @note     (2) When the journal is enabled the ordering requirements for low-level FAT operations
 *               involved in file writing are those illustrated below.
 *
 *                           O (FAT)         O (journal)                            O (data)
 *               .................................................................................
 *                           :               :                                      :
 *               .................................................................................
 *              Nth write    |               |                                      |
 *                           |               v                                      |
 *                           |        .------------.                                |
 *                           |--------|  Jnl log   |                         .------'------.
 *                           |        '------------'                         |             |
 *                           |               |                               |             |
 *                           v               v                               v             v
 *                   .------------.  .------------.                    .---------.   .---------.
 *                   | Clus alloc |  |  Jnl log   |----------.         | Data wr |   | Data wr |
 *                   '------------'  '------------'          |         '---------'   '---------'
 *                           |               |                |              |             |
 *                           v               |                v              '------.------'
 *                    .------------.         |        .--------------.              v
 *                    | Clus alloc |         |        | Entry update |            .---.
 *                    '------------'         |        '--------------'            | S |
 *                           |               |                                    '---'
 *                           |               |                                      |
 *               .................................................................................
 *                           :               :                                      :
 *               .................................................................................
 *              (N+1)th write|               |                                      |
 *                           |               v                                      |
 *                           |        .------------.                                |
 *                           .--------|  Jnl log   |                         .------'------.
 *                           |        '------------'                         |             |
 *                           |               |                               |             |
 *                           v               v                               v             v
 *                   .------------.  .------------.                    .---------.   .---------.
 *                   | Clus alloc |  |  Jnl log   |----------.         | Data wr |   | Data wr |
 *                   '------------'  '------------'          |         '---------'   '---------'
 *                          |                                |              |             |
 *                          |                                |              '------.------'
 *                          v                                v                     v
 *                  .------------.                  .--------------.            .---.
 *                  | Clus alloc |                  | Entry update |            | S |
 *                  '------------'                  '--------------'            '---'
 *                          |                               |                      |
 *              .................................................................................
 *                          :                               :                      :
 *              .................................................................................
 *                          |                               |                      |
 *                          v                               v                      v
 *                  .-------------------------------------------------------------------------.
 *                  |                              Journal clear                              |
 *                  '-------------------------------------------------------------------------'
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
CPU_SIZE_T FS_FAT_FileWr(FS_FILE_DESC *p_file_desc,
                         CPU_SIZE_T   pos,
                         CPU_INT08U   *p_src,
                         CPU_SIZE_T   wr_size,
                         RTOS_ERR     *p_err)
{
  FS_FAT_FILE_DESC *p_fat_file_desc;
#if (FS_FAT_CFG_JOURNAL_EN == DEF_ENABLED)
  FS_FAT_FILE_NODE *p_fat_file_node;
  FS_FAT_VOL       *p_fat_vol;
#endif
  CPU_SIZE_T size_wr;

  WITH_SCOPE_BEGIN(p_err) {
    p_fat_file_desc = (FS_FAT_FILE_DESC *)p_file_desc;
#if (FS_FAT_CFG_JOURNAL_EN == DEF_ENABLED)
    p_fat_file_node = (FS_FAT_FILE_NODE *)p_file_desc->FileNodePtr;
    p_fat_vol = (FS_FAT_VOL *)p_fat_file_node->FileNode.VolHandle.VolPtr;
    size_wr = 0u;

    //                                                             ---------------- JNL TOP LVL OP LOG ----------------
    BREAK_ON_ERR(FS_FAT_JournalEnterTopLvlOp(p_fat_vol,
                                             FS_FAT_JOURNAL_FILE_WR_LOG_MAX_SIZE,
                                             p_err));
#endif

    //                                                             --------------------- FILE WR ----------------------
    BREAK_ON_ERR(size_wr = FS_FAT_FileWrInternal(p_fat_file_desc,
                                                 pos,
                                                 p_src,
                                                 wr_size,
                                                 p_err));
  } WITH_SCOPE_END

  return (size_wr);
}
#endif

/****************************************************************************************************//**
 *                                           FS_FAT_FileTruncate()
 *
 * @brief    Truncate a file.
 *
 * @param    p_vol           Pointer to a volume.
 *
 * @param    p_file_node     Pointer an optional file node.
 *
 * @param    entry_sys_pos   Entry position.
 *
 * @param    new_size        Size of file after truncation (see Note #1).
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FS_FAT_FileTruncate(FS_VOL       *p_vol,
                         FS_FILE_NODE *p_file_node,
                         FS_SYS_POS   entry_sys_pos,
                         FS_FILE_SIZE new_size,
                         RTOS_ERR     *p_err)
{
  FS_FAT_VOL             *p_fat_vol;
  FS_FAT_FILE_NODE       *p_fat_file_node;
  FS_CACHE_WR_JOB_HANDLE *p_entry_wr_job_handle;
  CPU_INT08U             lb_size_log2;
  FS_FAT_SEC_BYTE_POS    entry_start_pos;
  FS_FAT_SEC_BYTE_POS    entry_end_pos;
  FS_FAT_ENTRY_INFO      fat_entry_info;
  FS_FILE_SIZE           file_size;
  FS_FAT_CLUS_NBR        first_clus_nbr;
  FS_FAT_CLUS_NBR        file_size_clus;
  FS_FAT_CLUS_NBR        file_size_truncated_clus;
  CPU_INT08U             clus_size_log2_octet;
  CPU_BOOLEAN            del_first;
  FS_FLAGS               update_fields;
#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
  FS_CACHE_WR_JOB_HANDLE entry_wr_job_handle = FS_CACHE_WR_JOB_HANDLE_INIT;
  FS_CACHE_WR_JOB_HANDLE stub_job_handle = FS_CACHE_WR_JOB_HANDLE_INIT;
#endif

  WITH_SCOPE_BEGIN(p_err) {
    p_fat_file_node = (FS_FAT_FILE_NODE *)p_file_node;
    p_fat_vol = (FS_FAT_VOL *)p_vol;

#if (FS_FAT_CFG_JOURNAL_EN == DEF_ENABLED)
    BREAK_ON_ERR(FS_FAT_JournalEnterTopLvlOp(p_fat_vol,
                                             FS_FAT_JOURNAL_FILE_TRUNCATE_LOG_MAX_SIZE,
                                             p_err));
#endif

    //                                                             ------------------- PARSE ENTRY --------------------
    if (p_fat_file_node == DEF_NULL) {
      FS_FAT_POS_DATA_DECODE(entry_sys_pos, entry_start_pos);
      BREAK_ON_ERR(FS_FAT_EntryParse(p_fat_vol,
                                     &entry_start_pos,
                                     &entry_end_pos,
                                     &fat_entry_info,
                                     DEF_NULL,
                                     DEF_NULL,
                                     0u,
                                     p_err));
      first_clus_nbr = fat_entry_info.FirstClusNbr;
      file_size = fat_entry_info.Size;
    } else {
      first_clus_nbr = p_fat_file_node->FirstClusNbr;
      file_size = p_fat_file_node->FileNode.Size;
      entry_end_pos = p_fat_file_node->EntryEndPos;
    }

    //                                                             ----------------- UPDATE DIR ENTRY -----------------
    //                                                             Wr new file size & first file clus to dir entry.
    update_fields = FS_FAT_DTE_FIELD_SIZE | FS_FAT_DTE_FIELD_WR_TIME;
    update_fields |= (new_size == 0u) ? FS_FAT_DTE_FIELD_FIRST_CLUS : FS_FAT_DTE_FIELD_NONE;

    fat_entry_info.FirstClusNbr = 0u;
    fat_entry_info.Size = new_size;

#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
    p_entry_wr_job_handle = (p_fat_file_node != DEF_NULL) ? &p_fat_file_node->EntryWrJobHandle
                            : &entry_wr_job_handle;
#else
    p_entry_wr_job_handle = DEF_NULL;
#endif

    BREAK_ON_ERR(FS_FAT_EntryUpdate(p_fat_vol,
                                    &entry_end_pos,
                                    &fat_entry_info,
                                    DEF_YES,
                                    update_fields,
                                    p_entry_wr_job_handle,
                                    p_err));

#if (FS_TEST_FAT_JNL_EN == DEF_ENABLED)
    if (FS_Test_FAT_JournalTestNumber == FSFILE_TRUNCATE_SHORTEN1) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
      return;
    }
#endif

    if (first_clus_nbr != 0u) {
      BREAK_ON_ERR(lb_size_log2 = FSBlkDev_LbSizeLog2Get(p_vol->BlkDevHandle, p_err));
      clus_size_log2_octet = p_fat_vol->SecPerClusLog2 + lb_size_log2;
      file_size_truncated_clus = (new_size > 0u) ? FS_UTIL_DIV_PWR2(new_size - 1u, clus_size_log2_octet) + 1u : 0u;
      file_size_clus = (file_size > 0u) ? FS_UTIL_DIV_PWR2(file_size - 1u, clus_size_log2_octet) + 1u : 0u;
      del_first = (new_size == 0u);

      if (del_first || (file_size_truncated_clus != file_size_clus)) {
        //                                                         ----------------- ORDER CACHE BLK ------------------
#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
#if (FS_FAT_CFG_JOURNAL_EN == DEF_ENABLED)
        if (p_fat_vol->IsJournaled) {
          BREAK_ON_ERR(stub_job_handle = FSCache_WrJobAppend(p_vol->CacheBlkDevDataPtr->CachePtr,
                                                             p_fat_vol->JournalData.JnlWrJobHandle,
                                                             p_err));

          BREAK_ON_ERR(stub_job_handle = FSCache_WrJobJoin(p_vol->CacheBlkDevDataPtr->CachePtr,
                                                           *p_entry_wr_job_handle,
                                                           stub_job_handle,
                                                           p_err));
        } else
#endif
        {
          BREAK_ON_ERR(stub_job_handle = FSCache_WrJobAppend(p_vol->CacheBlkDevDataPtr->CachePtr,
                                                             p_fat_vol->FatWrJobHandle,
                                                             p_err));

          BREAK_ON_ERR(stub_job_handle = FSCache_WrJobJoin(p_vol->CacheBlkDevDataPtr->CachePtr,
                                                           *p_entry_wr_job_handle,
                                                           stub_job_handle,
                                                           p_err));
          if (p_fat_file_node != DEF_NULL) {
            BREAK_ON_ERR(stub_job_handle = FSCache_WrJobJoin(p_vol->CacheBlkDevDataPtr->CachePtr,
                                                             p_fat_file_node->DataWrJobHandle,
                                                             stub_job_handle,
                                                             p_err));
          }
        }

        p_fat_vol->FatWrJobHandle = stub_job_handle;
#endif

        //                                                         ------------------ DEL CLUS CHAIN ------------------
        if (new_size != 0u) {
          FS_FAT_CLUS_NBR last_clus_tbl[3];
          FS_FAT_CLUS_NBR follow_cnt;

          BREAK_ON_ERR(follow_cnt = FS_FAT_ClusChainFollow(p_fat_vol,
                                                           first_clus_nbr,
                                                           file_size_truncated_clus - 1u,
                                                           last_clus_tbl,
                                                           p_err));
          first_clus_nbr = last_clus_tbl[0];
          ASSERT_BREAK(follow_cnt == file_size_truncated_clus - 1u, RTOS_ERR_VOL_CORRUPTED);
          ASSERT_BREAK(FS_FAT_IS_VALID_CLUS(p_fat_vol, first_clus_nbr), RTOS_ERR_VOL_CORRUPTED);
        }

        BREAK_ON_ERR((void)FS_FAT_ClusChainDel(p_fat_vol,
                                               first_clus_nbr,
                                               del_first,
                                               p_err));

#if (FS_TEST_FAT_JNL_EN == DEF_ENABLED)
        if (FS_Test_FAT_JournalTestNumber == FSFILE_TRUNCATE_SHORTEN2) {
          RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
          return;
        }
#endif

        if (p_file_node != DEF_NULL) {                          // Update file node if opened.
          if (new_size == 0u) {
            p_fat_file_node->FirstClusNbr = 0u;
          }
          p_fat_file_node->ShrinkSeqNo += 1u;
        }
      }
    }

#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
    if (!p_fat_vol->IsJournaled) {
      BREAK_ON_ERR(FS_CACHE_LOCK_WITH) (p_fat_vol->Vol.CacheBlkDevDataPtr->CachePtr) {
        BREAK_ON_ERR(FSCache_WrJobExec(p_fat_vol->Vol.CacheBlkDevDataPtr->CachePtr,
                                       p_fat_vol->FatWrJobHandle,
                                       DEF_NO,
                                       p_err));
      }
    }
#endif
  } WITH_SCOPE_END
}
#endif

/****************************************************************************************************//**
 *                                           FS_FAT_FileExtend()
 *
 * @brief    Allocate file storage without increasing file size.
 *
 * @param    p_vol       Pointer to a volume.
 *
 * @param    entry_pos   On-disk file position.
 *
 * @param    new_size    New file size.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FS_FAT_FileExtend(FS_VOL       *p_vol,
                       FS_SYS_POS   entry_pos,
                       FS_FILE_SIZE new_size,
                       RTOS_ERR     *p_err)
{
  FS_FAT_VOL             *p_fat_vol;
  FS_FAT_SEC_BYTE_POS    entry_start_pos;
  FS_FAT_SEC_BYTE_POS    entry_end_pos;
  FS_FAT_CLUS_CHAIN_ENDS clus_chain_ends;
  FS_FAT_ENTRY_INFO      fat_entry_info;
  FS_FAT_CLUS_NBR        old_clus_cnt;
  FS_FAT_CLUS_NBR        new_clus_cnt;
  FS_FAT_CLUS_NBR        delta_clus_cnt;
  CPU_INT08U             clus_size_log2;
  CPU_INT08U             lb_size_log2;
  FS_CACHE_WR_JOB_HANDLE data_wr_job_handle = FS_CACHE_WR_JOB_HANDLE_INIT;
  FS_CACHE_WR_JOB_HANDLE entry_wr_job_handle = FS_CACHE_WR_JOB_HANDLE_INIT;

  WITH_SCOPE_BEGIN(p_err) {
    p_fat_vol = (FS_FAT_VOL *)p_vol;

#if (FS_FAT_CFG_JOURNAL_EN == DEF_ENABLED)                      // ------------ JNL TOP LVL OP ENTER MARK -------------
    BREAK_ON_ERR(FS_FAT_JournalEnterTopLvlOp(p_fat_vol,
                                             FS_FAT_JOURNAL_FILE_EXTEND_LOG_MAX_SIZE,
                                             p_err));
#endif

    FS_FAT_POS_DATA_DECODE(entry_pos, entry_start_pos);

    //                                                             ---------------- EXTEND CLUS CHAIN -----------------
    BREAK_ON_ERR(FS_FAT_EntryParse(p_fat_vol,
                                   &entry_start_pos,
                                   &entry_end_pos,
                                   &fat_entry_info,
                                   DEF_NULL,
                                   DEF_NULL,
                                   0u,
                                   p_err));

    clus_chain_ends.FirstClusNbr = fat_entry_info.FirstClusNbr;

    if (fat_entry_info.FirstClusNbr != 0u) {
      FS_FAT_TYPE_API *p_fat_api = FS_FAT_TYPE_API_GET(p_fat_vol);
      FS_FAT_CLUS_NBR last_clus_tbl[3];
      BREAK_ON_ERR((void)FS_FAT_ClusChainEndFind(p_fat_vol,
                                                 fat_entry_info.FirstClusNbr,
                                                 last_clus_tbl,
                                                 p_err));
      ASSERT_BREAK(last_clus_tbl[0] >= p_fat_api->ClusEOF,
                   RTOS_ERR_VOL_CORRUPTED);
      clus_chain_ends.LastClusNbr = last_clus_tbl[1];
    } else {
      clus_chain_ends.LastClusNbr = 0u;
    }

    BREAK_ON_ERR(lb_size_log2 = FSBlkDev_LbSizeLog2Get(p_fat_vol->Vol.BlkDevHandle, p_err));
    clus_size_log2 = p_fat_vol->SecPerClusLog2 + lb_size_log2;

    old_clus_cnt = FS_FAT_FileClusCntGet(fat_entry_info.Size, clus_size_log2);
    new_clus_cnt = FS_FAT_FileClusCntGet(new_size, clus_size_log2);
    if (new_clus_cnt > old_clus_cnt) {
      delta_clus_cnt = new_clus_cnt - old_clus_cnt;
      BREAK_ON_ERR(clus_chain_ends.FirstClusNbr = FS_FAT_ClusChainAlloc(p_fat_vol,
                                                                        clus_chain_ends.LastClusNbr,
                                                                        &clus_chain_ends.LastClusNbr,
                                                                        delta_clus_cnt,
                                                                        DEF_NO,
                                                                        &data_wr_job_handle,
                                                                        FS_FAT_LB_TYPE_DATA,
                                                                        p_err));
    }

    fat_entry_info.FirstClusNbr = clus_chain_ends.FirstClusNbr;
    fat_entry_info.Size = new_size;

    BREAK_ON_ERR(FS_FAT_EntryUpdate(p_fat_vol,
                                    &entry_end_pos,
                                    &fat_entry_info,
                                    DEF_NO,
                                    FS_FAT_DTE_FIELD_FIRST_CLUS
                                    | FS_FAT_DTE_FIELD_SIZE,
                                    &entry_wr_job_handle,
                                    p_err));
  } WITH_SCOPE_END
}
#endif

/****************************************************************************************************//**
 *                                               FS_FAT_FileSync()
 *
 * @brief    Synchronize volume on which the file is.
 *
 * @param    p_file_node     Pointer to file node.
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FS_FAT_FileSync(FS_FILE_NODE *p_file_node,
                     RTOS_ERR     *p_err)
{
  FS_FAT_VolSync(p_file_node->VolHandle.VolPtr, p_err);
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           FS_FAT_FileModuleInit()
 *
 * @brief    Initialize the FAT file management module.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/
void FS_FAT_FileModuleInit(RTOS_ERR *p_err)
{
  CPU_SIZE_T init_file_desc_cnt;
  CPU_SIZE_T max_file_desc_cnt;
  CPU_SIZE_T init_file_node_cnt;
  CPU_SIZE_T max_file_node_cnt;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_SET((FSCore_InitCfg.MaxFatObjCnt.FileDescCnt != 0u)
                            && (FSCore_InitCfg.MaxFatObjCnt.FileNodeCnt != 0u), *p_err, RTOS_ERR_INVALID_ARG,; );

    init_file_desc_cnt = FSCore_InitCfg.MaxFatObjCnt.FileDescCnt == LIB_MEM_BLK_QTY_UNLIMITED ? 1u : FSCore_InitCfg.MaxFatObjCnt.FileDescCnt;
    max_file_desc_cnt = FSCore_InitCfg.MaxFatObjCnt.FileDescCnt;

    BREAK_ON_ERR(Mem_DynPoolCreate("FS - FAT file desc pool",
                                   &FS_FAT_FileDescPool,
                                   FSCore_InitCfg.MemSegPtr,
                                   sizeof(FS_FAT_FILE_DESC),
                                   sizeof(CPU_ALIGN),
                                   init_file_desc_cnt,
                                   max_file_desc_cnt,
                                   p_err));

    init_file_node_cnt = FSCore_InitCfg.MaxFatObjCnt.FileNodeCnt == LIB_MEM_BLK_QTY_UNLIMITED ? 1u : FSCore_InitCfg.MaxFatObjCnt.FileNodeCnt;
    max_file_node_cnt = FSCore_InitCfg.MaxFatObjCnt.FileNodeCnt;

    BREAK_ON_ERR(Mem_DynPoolCreate("FS - FAT file node pool",
                                   &FS_FAT_FileNodePool,
                                   FSCore_InitCfg.MemSegPtr,
                                   sizeof(FS_FAT_FILE_NODE),
                                   sizeof(CPU_ALIGN),
                                   init_file_node_cnt,
                                   max_file_node_cnt,
                                   p_err));
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                           FS_FAT_File_ClusQtyGet()
 *
 * @brief    Get the number of clusters of a given file.
 *
 * @param    file_handle     File handle.
 *
 * @param    p_err           Error pointer.
 *
 * @return   Number of clusters.
 *******************************************************************************************************/
FS_FAT_CLUS_NBR FS_FAT_File_ClusQtyGet(FS_FILE_HANDLE file_handle,
                                       RTOS_ERR       *p_err)
{
  FS_FAT_CLUS_NBR clus_cnt;
  FS_VOL_HANDLE   vol_handle;
  FS_FAT_VOL      *p_fat_vol;
  CPU_INT08U      lb_size_log2;
  CPU_INT08U      clus_size_log2;

  vol_handle = FSFile_VolGet(file_handle);
  p_fat_vol = (FS_FAT_VOL *)vol_handle.VolPtr;

  lb_size_log2 = FSBlkDev_LbSizeLog2Get(p_fat_vol->Vol.BlkDevHandle, p_err);
  APP_RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE, 0);

  clus_size_log2 = p_fat_vol->SecPerClusLog2 + lb_size_log2;

  clus_cnt = FS_FAT_FileClusCntGet(file_handle.FileDescPtr->Pos, clus_size_log2);

  return (clus_cnt);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           FS_FAT_FileWrInternal()
 *
 * @brief    Write to a file (without journal clear and volume sync handling).
 *
 * @param    p_fat_file_desc     Pointer to a FAT file descriptor.
 *
 * @param    pos                 File on-disk position.
 *
 * @param    p_src               Pointer to the user-provided source buffer.
 *
 * @param    wr_size             Number of octets to be written.
 *
 * @param    p_err               Error pointer.
 *
 * @return   Number of octets written.
 *
 * @note     (1) If the current file position is beyond the file size, unwritten region located between
 *               file end and current position must be zeroed out so that further reading from that region
 *               return zeros (and not garbage data).
 *
 * @note     (2) Region that is located both beyond the current file position and file size does not
 *               need to be zeroed out as it will be written to.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static CPU_SIZE_T FS_FAT_FileWrInternal(FS_FAT_FILE_DESC *p_fat_file_desc,
                                        CPU_SIZE_T       pos,
                                        CPU_INT08U       *p_src,
                                        CPU_SIZE_T       wr_size,
                                        RTOS_ERR         *p_err)
{
  FS_FAT_FILE_NODE       *p_fat_file_node;
  FS_FAT_VOL             *p_fat_vol;
  FS_FAT_CLUS_NBR        start_clus;
  FS_FAT_SEC_BYTE_POS    entry_end_pos;
  CPU_SIZE_T             size_wr;
  CPU_SIZE_T             fill_size;
  FS_FILE_SIZE           new_file_size;
  FS_FILE_SIZE           cur_file_size;
  FS_FAT_ENTRY_INFO      fat_entry_info;
  FS_FAT_CLUS_CHAIN_ENDS clus_chain_ends;
  FS_FAT_CLUS_NBR        new_clus_cnt;
  FS_FAT_CLUS_NBR        old_clus_cnt;
  FS_FAT_CLUS_NBR        delta_clus_cnt;
  CPU_INT08U             clus_size_log2;
  CPU_INT08U             lb_size_log2;
  FS_CACHE_WR_JOB_HANDLE stub_wr_job_handle = FS_CACHE_WR_JOB_HANDLE_INIT;

  WITH_SCOPE_BEGIN(p_err) {
    p_fat_file_node = (FS_FAT_FILE_NODE *)p_fat_file_desc->FileDesc.FileNodePtr;
    p_fat_vol = (FS_FAT_VOL *)p_fat_file_node->FileNode.VolHandle.VolPtr;

    cur_file_size = p_fat_file_node->FileNode.Size;
    size_wr = 0u;

    //                                                             ------------------- EXTEND FILE --------------------
    //                                                             Compute new file size.
    new_file_size = DEF_MAX(pos + wr_size, cur_file_size);

    if (new_file_size > cur_file_size) {
      if (pos > cur_file_size) {
        //                                                         Fill in the gap between file end and cur pos.
        //                                                         See Note #1.
        fill_size = pos - p_fat_file_node->FileNode.Size;
        BREAK_ON_ERR(FS_FAT_FileWrInternal(p_fat_file_desc,
                                           cur_file_size,
                                           DEF_NULL,
                                           fill_size,
                                           p_err));
        cur_file_size += fill_size;
      }
      //                                                           Allocate storage to be written to.
      //                                                           See Note #2.
      if (FS_FAT_IsFilePosCacheValid(p_fat_file_desc)) {
        start_clus = p_fat_file_desc->CurClus;
      } else {
        start_clus = p_fat_file_node->FirstClusNbr;
      }

      clus_chain_ends.FirstClusNbr = p_fat_file_node->FirstClusNbr;

      if (p_fat_file_node->FirstClusNbr != 0u) {
        FS_FAT_TYPE_API *p_fat_api = FS_FAT_TYPE_API_GET(p_fat_vol);
        FS_FAT_CLUS_NBR last_clus_tbl[3];
        BREAK_ON_ERR((void)FS_FAT_ClusChainEndFind(p_fat_vol,
                                                   start_clus,
                                                   last_clus_tbl,
                                                   p_err));
        ASSERT_BREAK(last_clus_tbl[0] >= p_fat_api->ClusEOF,
                     RTOS_ERR_VOL_CORRUPTED);
        clus_chain_ends.LastClusNbr = last_clus_tbl[1];
      } else {
        clus_chain_ends.LastClusNbr = 0u;
      }

      BREAK_ON_ERR(lb_size_log2 = FSBlkDev_LbSizeLog2Get(p_fat_vol->Vol.BlkDevHandle, p_err));
      clus_size_log2 = p_fat_vol->SecPerClusLog2 + lb_size_log2;

      old_clus_cnt = FS_FAT_FileClusCntGet(cur_file_size, clus_size_log2);
      new_clus_cnt = FS_FAT_FileClusCntGet(new_file_size, clus_size_log2);
      if (new_clus_cnt > old_clus_cnt) {
        delta_clus_cnt = new_clus_cnt - old_clus_cnt;
        BREAK_ON_ERR(clus_chain_ends.FirstClusNbr = FS_FAT_ClusChainAlloc(p_fat_vol,
                                                                          clus_chain_ends.LastClusNbr,
                                                                          &clus_chain_ends.LastClusNbr,
                                                                          delta_clus_cnt,
                                                                          DEF_NO,
                                                                          DEF_NULL,
                                                                          FS_FAT_LB_TYPE_DATA,
                                                                          p_err));
      }
      if (p_fat_file_node->FirstClusNbr == 0u) {
        p_fat_file_node->FirstClusNbr = clus_chain_ends.FirstClusNbr;
      }
    }

    //                                                             --------------------- WR DATA ----------------------
    BREAK_ON_ERR(size_wr = FS_FAT_FileRdWr(p_fat_file_desc,
                                           pos,
                                           p_src,
                                           wr_size,
                                           DEF_YES,
                                           p_err));

    entry_end_pos = p_fat_file_node->EntryEndPos;
    fat_entry_info.FirstClusNbr = p_fat_file_node->FirstClusNbr;
    fat_entry_info.Size = new_file_size;

    //                                                             ---------- DATA & FAT CACHE BLKS ORDERING ----------
    //                                                             See FS_FAT_FileWr() notes.
#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
#if (FS_FAT_CFG_JOURNAL_EN == DEF_ENABLED)
    if (p_fat_vol->IsJournaled) {
      stub_wr_job_handle = p_fat_file_node->EntryWrJobHandle;
    } else
#endif
    {
      FS_CACHE *p_cache = p_fat_vol->Vol.CacheBlkDevDataPtr->CachePtr;

      BREAK_ON_ERR(stub_wr_job_handle = FSCache_WrJobJoin(p_cache,
                                                          p_fat_vol->FatWrJobHandle,
                                                          p_fat_file_node->DataWrJobHandle,
                                                          p_err));

      BREAK_ON_ERR(stub_wr_job_handle = FSCache_WrJobJoin(p_cache,
                                                          p_fat_file_node->EntryWrJobHandle,
                                                          stub_wr_job_handle,
                                                          p_err));

      BREAK_ON_ERR(stub_wr_job_handle = FSCache_WrJobJoin(p_cache,
                                                          p_fat_file_node->DataWrJobHandle,
                                                          stub_wr_job_handle,
                                                          p_err));

      p_fat_file_node->EntryWrJobHandle = stub_wr_job_handle;
    }
#endif

    //                                                             ------------------- UPDATE ENTRY -------------------
    BREAK_ON_ERR(FS_FAT_EntryUpdate(p_fat_vol,
                                    &entry_end_pos,
                                    &fat_entry_info,
                                    DEF_YES,
                                    FS_FAT_DTE_FIELD_FIRST_CLUS
                                    | FS_FAT_DTE_FIELD_WR_TIME
                                    | FS_FAT_DTE_FIELD_SIZE,
                                    &stub_wr_job_handle,
                                    p_err));

#if (FS_TEST_FAT_JNL_EN == DEF_ENABLED)
    if ((FS_Test_FAT_JournalTestNumber == FSFILE_WR1)
        || (FS_Test_FAT_JournalTestNumber == FSFILE_WR2)
        || (FS_Test_FAT_JournalTestNumber == FSFILE_TRUNCATE_EXTEND)) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    }
#endif

#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
    p_fat_file_node->EntryWrJobHandle = stub_wr_job_handle;
#endif
  } WITH_SCOPE_END

  return (size_wr);
}
#endif

/****************************************************************************************************//**
 *                                           FS_FAT_FileClusCntGet()
 *
 * @brief    Return the number of cluster needed to accommodate the given number of octets.
 *
 * @param    file_size               Number of octets that must be stored.
 *
 * @param    clus_size_log2_octets   Base 2 logarithm of the cluster size (in octets).
 *
 * @return   Number of cluster.
 *******************************************************************************************************/
static FS_FAT_CLUS_NBR FS_FAT_FileClusCntGet(FS_FILE_SIZE file_size,
                                             CPU_INT08U   clus_size_log2_octets)
{
  FS_FAT_CLUS_NBR clus_cnt;

  clus_cnt = FS_UTIL_DIV_PWR2(file_size, clus_size_log2_octets);
  clus_cnt += (FS_UTIL_MODULO_PWR2(file_size, clus_size_log2_octets) == 0u) ? 0u : 1u;

  return (clus_cnt);
}

/****************************************************************************************************//**
 *                                           FS_FAT_FileLbGet()
 *
 * @brief    Get the logical block number corresponding to the given logical block offset in a file.
 *
 * @param    p_file_desc     Pointer to a file descriptor.
 *
 * @param    lb_offset       Logical block offset relative to file start.
 *
 * @param    p_err           Error pointer.
 *
 * @return   Logical block offset relative to the media start.
 *******************************************************************************************************/
static FS_LB_NBR FS_FAT_FileSecGet(FS_FAT_FILE_DESC *p_fat_file_desc,
                                   FS_LB_NBR        lb_offset,
                                   RTOS_ERR         *p_err)
{
  FS_FAT_VOL  *p_fat_vol;
  CPU_BOOLEAN pos_cache_valid;
  CPU_BOOLEAN pos_cache_match;
  FS_LB_NBR   lb_nbr = 0u;
  FS_LB_NBR   cur_clus_sec_offset;

  WITH_SCOPE_BEGIN(p_err) {
    p_fat_vol = (FS_FAT_VOL *)p_fat_file_desc->FileDesc.FileNodePtr->VolHandle.VolPtr;

    pos_cache_valid = FS_FAT_IsFilePosCacheValid(p_fat_file_desc);
    if (pos_cache_valid) {
      pos_cache_match = FS_FAT_IsFilePosCacheSync(p_fat_file_desc, lb_offset);
      if (!pos_cache_match) {
        BREAK_ON_ERR(FS_FAT_FilePosCacheSync(p_fat_file_desc,
                                             lb_offset,
                                             p_err));
      }
    } else {
      BREAK_ON_ERR(FS_FAT_FilePosCacheSet(p_fat_file_desc,
                                          lb_offset,
                                          p_err));
    }

    cur_clus_sec_offset = FS_UTIL_MODULO_PWR2(lb_offset, p_fat_vol->SecPerClusLog2);
    lb_nbr = FS_FAT_CLUS_TO_SEC(p_fat_vol, p_fat_file_desc->CurClus) + cur_clus_sec_offset;
  } WITH_SCOPE_END

  return (lb_nbr);
}

/****************************************************************************************************//**
 *                                               FS_FAT_FileRdWr()
 *
 * @brief   Read from or write to a file.
 *
 * @param   p_fat_file_desc     Pointer to a FAT file descriptor.
 *
 * @param   pos                 Position in the file to read from or write to.
 *
 * @param   p_data_buf          Pointer to the data buffer.
 *
 * @param   size                Number of octets to be read / written.
 *
 * @param   is_wr               Indicates whether to read or write.
 *
 * @param   p_err               Error pointer.
 *
 * @return  Number of read / written octets.
 *
 * @note    (1) This while loop is an essential path managing file reads and writes. The read or write is
 *              split accordingly into partial and/or full sectors accesses. The typical sector access
 *              sequences are the following:
 *
 *              (a) 1 PARTIAL SECTOR
 *
 *              (b) N FULL SECTORS
 *
 *              (c) 1 PARTIAL SECTOR
 *                  N FULL SECTORS
 *
 *              (d) N FULL SECTORS
 *                  1 PARTIAL SECTOR
 *
 *              (e) 1 PARTIAL SECTOR
 *                  N FULL SECTORS
 *                  1 PARTIAL SECTOR
 *
 *          (2) The function FS_FAT_SecNextGet() handles the read or write spanning clusters boundary.
 *              If the current sector read or written is the last sector of a cluster, it will find the
 *              next available sector contiguous or not to the sector currently evaluated .
 *******************************************************************************************************/
static CPU_SIZE_T FS_FAT_FileRdWr(FS_FAT_FILE_DESC *p_fat_file_desc,
                                  CPU_SIZE_T       pos,
                                  CPU_INT08U       *p_data_buf,
                                  CPU_SIZE_T       size,
                                  CPU_BOOLEAN      is_wr,
                                  RTOS_ERR         *p_err)
{
  FS_FAT_FILE_NODE *p_fat_file_node;
  FS_FAT_VOL       *p_fat_vol;
  CPU_INT08U       *p_buf;
  FS_FILE_SIZE     usr_buf_pos;
  FS_FILE_SIZE     cur_file_pos;
  FS_LB_SIZE       lb_byte_offset;
  FS_LB_SIZE       rem_bytes_in_lb;
  CPU_SIZE_T       rem_xfer_size;
  CPU_SIZE_T       rd_wr_size;
  FS_LB_NBR        cur_sec_nbr;
  FS_LB_NBR        next_sec_nbr;
  FS_LB_NBR        start_sec_nbr;
  FS_LB_NBR        start_sec_offset;
  FS_LB_NBR        lb_cnt;
  CPU_SIZE_T       rd_wr_octet_cnt = 0u;
  CPU_BOOLEAN      is_aligned;
  CPU_INT08U       lb_size_log2;
  CPU_SIZE_T       align;
#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
  FS_CACHE_WR_JOB_HANDLE stub_job_handle = FS_CACHE_WR_JOB_HANDLE_INIT;
  FS_CACHE_WR_JOB_HANDLE wr_job_handle = FS_CACHE_WR_JOB_HANDLE_INIT;
#endif

  if (size == 0u) {
    return (0u);
  }

  WITH_SCOPE_BEGIN(p_err) {
    p_fat_file_node = (FS_FAT_FILE_NODE *)p_fat_file_desc->FileDesc.FileNodePtr;
    p_fat_vol = (FS_FAT_VOL *)p_fat_file_node->FileNode.VolHandle.VolPtr;

    BREAK_ON_ERR(lb_size_log2 = FSBlkDev_LbSizeLog2Get(p_fat_vol->Vol.BlkDevHandle, p_err));
    BREAK_ON_ERR(align = FSBlkDev_AlignReqGet(p_fat_vol->Vol.BlkDevHandle, p_err));

    rem_xfer_size = is_wr ? size : DEF_MIN(size, p_fat_file_node->FileNode.Size - pos);
    cur_file_pos = pos;
    start_sec_offset = FS_UTIL_DIV_PWR2(pos, lb_size_log2);
    usr_buf_pos = 0u;
    rd_wr_octet_cnt = 0u;
    //                                                             Get first sector number.
    BREAK_ON_ERR(cur_sec_nbr = FS_FAT_FileSecGet(p_fat_file_desc,
                                                 start_sec_offset,
                                                 p_err));

    BREAK_ON_ERR(while) (rem_xfer_size > 0u) {                  // See Note #1 and #2.
      lb_byte_offset = FS_UTIL_MODULO_PWR2(cur_file_pos, lb_size_log2);
      rem_bytes_in_lb = FS_UTIL_PWR2(lb_size_log2) - lb_byte_offset;
      rd_wr_size = DEF_MIN(rem_xfer_size, rem_bytes_in_lb);

      //                                                           ------------------ PARTIAL SECTOR ------------------
      if (rd_wr_size < FS_UTIL_PWR2(lb_size_log2)) {            // read/write size less than sector size.
        if (is_wr) {                                            // Partial sector write.
#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
          BREAK_ON_ERR(FS_VOL_CACHE_BLK_RW)(&p_fat_vol->Vol,
                                            cur_sec_nbr,
                                            FS_FAT_LB_TYPE_DATA,
                                            p_fat_file_node->DataWrJobHandle,
                                            &p_buf,
                                            &wr_job_handle,
                                            p_err) {
            if (p_data_buf == DEF_NULL) {
              Mem_Clr((void *)(p_buf + lb_byte_offset), rd_wr_size);
            } else {
              Mem_Copy((void *)(p_buf + lb_byte_offset),
                       (void *)(p_data_buf + usr_buf_pos),
                       rd_wr_size);
            }
          }

#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
          BREAK_ON_ERR(stub_job_handle = FSCache_WrJobJoin(p_fat_vol->Vol.CacheBlkDevDataPtr->CachePtr,
                                                           wr_job_handle,
                                                           stub_job_handle,
                                                           p_err));
#endif
#else
          RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_FAIL, 0u);
#endif
        } else {                                                // Partial sector read.
          BREAK_ON_ERR(FS_VOL_CACHE_BLK_RD)(&p_fat_vol->Vol,
                                            cur_sec_nbr,
                                            FS_FAT_LB_TYPE_DATA,
                                            &p_buf,
                                            p_err) {
            Mem_Copy((void *)(p_data_buf + usr_buf_pos),
                     (void *)(p_buf + lb_byte_offset),
                     rd_wr_size);
          }
        }

        rd_wr_octet_cnt += rd_wr_size;                          // Update total size processed.
        rem_xfer_size -= rd_wr_size;                            // Update remaining transfer size.
        cur_file_pos += rd_wr_size;                             // Update file position.
        usr_buf_pos += rd_wr_size;                              // Update user buffer position.

        //                                                         Find first full sector number.
        BREAK_ON_ERR(cur_sec_nbr = FS_FAT_SecNextGet(p_fat_vol,
                                                     cur_sec_nbr,
                                                     p_err));
      }

      //                                                           ------------------- FULL SECTOR --------------------
      if (rem_xfer_size >= FS_UTIL_PWR2(lb_size_log2)) {        // read/write size greater than sector size.
        is_aligned = (((CPU_ADDR)(p_data_buf + usr_buf_pos) % align) == 0u);

        if (is_aligned && (p_data_buf != DEF_NULL)) {           // If data buf aligned according to mem alignment, ...
                                                                // ...perform direct block device accesses.
          start_sec_nbr = cur_sec_nbr;
          next_sec_nbr = cur_sec_nbr;
          rd_wr_size = 0u;
          //                                                       *INDENT-OFF*
          BREAK_ON_ERR (do ) {                                  // Find contiguous sectors within same cluster...
            cur_sec_nbr = next_sec_nbr;                         // ...or spanning clusters.
            rd_wr_size += FS_UTIL_PWR2(lb_size_log2);
            BREAK_ON_ERR(next_sec_nbr = FS_FAT_SecNextGet(p_fat_vol,
                                                          cur_sec_nbr,
                                                          p_err));
          } while (((rem_xfer_size - rd_wr_size) >= FS_UTIL_PWR2(lb_size_log2))
                   && (next_sec_nbr == cur_sec_nbr + 1u));
          //                                                       *INDENT-ON*

          //                                                       Invalidate cache blocks for each contiguous sector.
          BREAK_ON_ERR(FS_CACHE_LOCK_WITH) (p_fat_vol->Vol.CacheBlkDevDataPtr->CachePtr) {
            lb_cnt = FS_UTIL_DIV_PWR2(rd_wr_size, lb_size_log2);

            BREAK_ON_ERR(for) (cur_sec_nbr = start_sec_nbr; cur_sec_nbr < (start_sec_nbr + lb_cnt); cur_sec_nbr++) {
              BREAK_ON_ERR(FSCache_LbInvalidate(p_fat_vol->Vol.CacheBlkDevDataPtr->CachePtr,
                                                p_fat_vol->Vol.BlkDevHandle,
                                                p_fat_vol->Vol.PartitionStart + cur_sec_nbr,
                                                p_err));
            }
            cur_sec_nbr -= 1u;                                  // Get last sector nbr after cache invalidate (0-based).

            if (is_wr) {                                        // Aligned write.
#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
              BREAK_ON_ERR(FSCache_WrJobExec(p_fat_vol->Vol.CacheBlkDevDataPtr->CachePtr,
                                             wr_job_handle,
                                             DEF_NO,
                                             p_err));
#endif
              BREAK_ON_ERR(FSBlkDev_Wr(p_fat_vol->Vol.BlkDevHandle,
                                       (void *)(p_data_buf + usr_buf_pos),
                                       p_fat_vol->Vol.PartitionStart + start_sec_nbr,
                                       lb_cnt,                  // Write up to nbr of contiguous sectors.
                                       p_err));
#else
              RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_FAIL, 0u);
#endif
            } else {                                            // Aligned read.
              BREAK_ON_ERR(FSBlkDev_Rd(p_fat_vol->Vol.BlkDevHandle,
                                       (void *)(p_data_buf + usr_buf_pos),
                                       (p_fat_vol->Vol.PartitionStart + start_sec_nbr),
                                       lb_cnt,                  // Read up to nbr of contiguous sectors.
                                       p_err));
            }
          }

          rd_wr_octet_cnt += rd_wr_size;                        // Update total size processed.
          rem_xfer_size -= rd_wr_size;                          // Update remaining transfer size.
          cur_file_pos += rd_wr_size;                           // Update file position.
          usr_buf_pos += rd_wr_size;                            // Update user buffer position.

          //                                                       Get next sector in current or different cluster.
          BREAK_ON_ERR(cur_sec_nbr = FS_FAT_SecNextGet(p_fat_vol,
                                                       cur_sec_nbr,
                                                       p_err));
        } else {                                                // If data buf not aligned according to mem alignment...
                                                                // ...perform accesses through cache.
          rd_wr_size = FS_UTIL_PWR2(lb_size_log2);

          //                                                       *INDENT-OFF*
          BREAK_ON_ERR (do ) {
            if (is_wr) {                                        // Unaligned write.
              BREAK_ON_ERR(FS_VOL_CACHE_BLK_WR)(&p_fat_vol->Vol,
                                                cur_sec_nbr,
                                                FS_FAT_LB_TYPE_DATA,
                                                p_fat_file_node->DataWrJobHandle,
                                                &p_buf,
                                                &wr_job_handle,
                                                p_err) {

                if (p_data_buf == DEF_NULL) {
                  Mem_Clr((void *)p_buf, rd_wr_size);
                } else {
                  Mem_Copy((void *)p_buf,
                      (void *)(p_data_buf + usr_buf_pos),
                      rd_wr_size);
                }
              }

#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
              BREAK_ON_ERR(stub_job_handle = FSCache_WrJobJoin(p_fat_vol->Vol.CacheBlkDevDataPtr->CachePtr,
                                                               wr_job_handle,
                                                               stub_job_handle,
                                                               p_err));
#endif
            } else {                                            // Unaligned read.
              BREAK_ON_ERR(FS_VOL_CACHE_BLK_RD)(&p_fat_vol->Vol,
                                                cur_sec_nbr,
                                                FS_FAT_LB_TYPE_DATA,
                                                &p_buf,
                                                p_err) {

                Mem_Copy((void *)(p_data_buf + usr_buf_pos),
                         (void *)p_buf,
                         rd_wr_size);
              }
            }

            rd_wr_octet_cnt += rd_wr_size;                      // Update total size processed.
            rem_xfer_size -= rd_wr_size;                        // Update remaining transfer size.
            cur_file_pos += rd_wr_size;                         // Update file position.
            usr_buf_pos += rd_wr_size;                          // Update user buffer position.

                                                                // Get next sector in current or different cluster.
            BREAK_ON_ERR(cur_sec_nbr = FS_FAT_SecNextGet(p_fat_vol,
                                                         cur_sec_nbr,
                                                         p_err));
          } while (rem_xfer_size >= FS_UTIL_PWR2(lb_size_log2));
          //                                                       *INDENT-ON*
        }
      }
    }

#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
    if (is_wr) {
      p_fat_file_node->DataWrJobHandle = stub_job_handle;
    }
#endif
  } WITH_SCOPE_END

  return (rd_wr_octet_cnt);
}

/****************************************************************************************************//**
 *                                       FS_FAT_IsFilePosCacheSync()
 *
 * @brief    Check whether or not the file position cache matches the given position.
 *
 * @param    p_fat_file_desc     Pointer to a FAT file descriptor object.
 *
 * @param    lb_offset           Logical block offset in a the given file.
 *
 * @return   Boolean indicating whether or not the file position cache is synchronized with the core
 *           current position.
 *
 * @note     (1) The caller is responsible for checking that the position cache is valid beforehand
 *               (see FS_FAT_IsFilePosCacheValid()).
 *******************************************************************************************************/
static CPU_BOOLEAN FS_FAT_IsFilePosCacheSync(FS_FAT_FILE_DESC *p_fat_file_desc,
                                             FS_LB_NBR        lb_offset)
{
  FS_FAT_FILE_NODE *p_fat_file_node;
  FS_FAT_VOL       *p_fat_vol;
  CPU_SIZE_T       low_bound;
  CPU_SIZE_T       high_bound;

  p_fat_file_node = (FS_FAT_FILE_NODE *)p_fat_file_desc->FileDesc.FileNodePtr;
  p_fat_vol = (FS_FAT_VOL *)p_fat_file_node->FileNode.VolHandle.VolPtr;

  low_bound = FS_UTIL_MULT_PWR2(p_fat_file_desc->CurClusIx, p_fat_vol->SecPerClusLog2);
  high_bound = low_bound + FS_UTIL_PWR2(p_fat_vol->SecPerClusLog2);

  return ((lb_offset >= low_bound) && (lb_offset < high_bound));
}

/****************************************************************************************************//**
 *                                       FS_FAT_IsFilePosCacheValid()
 *
 * @brief    Check whether the file position cache is valid.
 *
 * @param    p_fat_file_desc     Pointer to a FAT file descriptor.
 *
 * @return   DEF_YES, if the cache is valid.
 *           DEF_NO,  otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN FS_FAT_IsFilePosCacheValid(FS_FAT_FILE_DESC *p_fat_file_desc)
{
  FS_FAT_FILE_NODE *p_fat_file_node;

  p_fat_file_node = (FS_FAT_FILE_NODE *)p_fat_file_desc->FileDesc.FileNodePtr;

  if ((p_fat_file_desc->CurClus == 0u)
      || (p_fat_file_desc->ShrinkSeqNo != p_fat_file_node->ShrinkSeqNo)) {
    return (DEF_NO);
  }

  return (DEF_YES);
}

/****************************************************************************************************//**
 *                                       FS_FAT_FileDescPosCacheUpdate()
 *
 * @brief    Update file cache position to reflect the given file position.
 *
 * @param    p_fat_file_desc     Pointer to a FAT file descriptor object.
 *
 * @param    lb_ix               Logical block index.
 *
 * @param    p_err               Error pointer.
 *******************************************************************************************************/
static void FS_FAT_FilePosCacheSet(FS_FAT_FILE_DESC *p_fat_file_desc,
                                   FS_LB_NBR        lb_ix,
                                   RTOS_ERR         *p_err)
{
  FS_FAT_FILE_NODE *p_fat_file_node;
  FS_FAT_VOL       *p_fat_vol;
  FS_FAT_CLUS_NBR  last_clus_tbl[3];
  FS_FAT_CLUS_NBR  clus_ix;
  FS_FAT_CLUS_NBR  follow_cnt;

  WITH_SCOPE_BEGIN(p_err) {
    p_fat_file_node = (FS_FAT_FILE_NODE *)p_fat_file_desc->FileDesc.FileNodePtr;
    p_fat_vol = (FS_FAT_VOL *)p_fat_file_node->FileNode.VolHandle.VolPtr;

    clus_ix = FS_UTIL_DIV_PWR2(lb_ix, p_fat_vol->SecPerClusLog2);

    BREAK_ON_ERR(follow_cnt = FS_FAT_ClusChainFollow(p_fat_vol,
                                                     p_fat_file_node->FirstClusNbr,
                                                     clus_ix,
                                                     last_clus_tbl,
                                                     p_err));

    ASSERT_BREAK(follow_cnt == clus_ix, RTOS_ERR_VOL_CORRUPTED);
    ASSERT_BREAK(FS_FAT_IS_VALID_CLUS(p_fat_vol, last_clus_tbl[0]), RTOS_ERR_VOL_CORRUPTED);

    p_fat_file_desc->CurClus = last_clus_tbl[0];
    p_fat_file_desc->CurClusIx = clus_ix;

    p_fat_file_desc->ShrinkSeqNo = p_fat_file_node->ShrinkSeqNo;
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                           FS_FAT_FilePosCacheSync()
 *
 * @brief    Update cache file position cache.
 *
 * @param    p_fat_file_desc     Pointer to a FAT file descriptor.
 *
 * @param    lb_ix               Logical block index relative to file start (i.e. first logical block
 *                               has index 0) to update the position cache to.
 *
 * @param    p_err               Error pointer.
 *******************************************************************************************************/
static void FS_FAT_FilePosCacheSync(FS_FAT_FILE_DESC *p_fat_file_desc,
                                    FS_LB_NBR        lb_ix,
                                    RTOS_ERR         *p_err)
{
  FS_FAT_FILE_NODE *p_fat_file_node;
  FS_FAT_VOL       *p_fat_vol;
  FS_FAT_CLUS_NBR  last_clus_tbl[3];
  FS_FAT_CLUS_NBR  clus_ix;
  FS_FAT_CLUS_NBR  clus_cnt;
  FS_FAT_CLUS_NBR  follow_cnt;
  FS_FAT_CLUS_NBR  start_clus;

  WITH_SCOPE_BEGIN(p_err) {
    p_fat_file_node = (FS_FAT_FILE_NODE *)p_fat_file_desc->FileDesc.FileNodePtr;
    p_fat_vol = (FS_FAT_VOL *)p_fat_file_node->FileNode.VolHandle.VolPtr;

    clus_ix = FS_UTIL_DIV_PWR2(lb_ix, p_fat_vol->SecPerClusLog2);

    if (clus_ix >= p_fat_file_desc->CurClusIx) {
      start_clus = p_fat_file_desc->CurClus;
      clus_cnt = clus_ix - p_fat_file_desc->CurClusIx;
    } else {
      start_clus = p_fat_file_node->FirstClusNbr;
      clus_cnt = clus_ix;
    }

    BREAK_ON_ERR(follow_cnt = FS_FAT_ClusChainFollow(p_fat_vol,
                                                     start_clus,
                                                     clus_cnt,
                                                     last_clus_tbl,
                                                     p_err));

    ASSERT_BREAK(follow_cnt == clus_cnt, RTOS_ERR_VOL_CORRUPTED);
    ASSERT_BREAK(FS_FAT_IS_VALID_CLUS(p_fat_vol, last_clus_tbl[0]), RTOS_ERR_VOL_CORRUPTED);

    p_fat_file_desc->CurClus = last_clus_tbl[0];
    p_fat_file_desc->CurClusIx = clus_ix;

    p_fat_file_desc->ShrinkSeqNo = p_fat_file_node->ShrinkSeqNo;
  } WITH_SCOPE_END
}

#endif // FS_CORE_CFG_FAT_EN

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_FS_AVAIL
