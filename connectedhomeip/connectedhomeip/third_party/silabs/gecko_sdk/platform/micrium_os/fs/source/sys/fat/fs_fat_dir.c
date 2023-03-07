/***************************************************************************//**
 * @file
 * @brief File System - Fat Directory Operations
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

#if ((FS_CORE_CFG_FAT_EN == DEF_ENABLED) \
  && (FS_CORE_CFG_DIR_EN == DEF_ENABLED))

//                                                                 ----------------------- FAT ------------------------
#include  <fs/source/sys/fat/fs_fat_priv.h>
#include  <fs/source/sys/fat/fs_fat_xfn_priv.h>
#include  <fs/source/sys/fat/fs_fat_entry_priv.h>
#include  <fs/source/sys/fat/fs_fat_dir_priv.h>

//                                                                 ----------------------- CORE -----------------------
#include  <fs/source/shared/fs_utils_priv.h>
#include  <fs/source/core/fs_core_priv.h>
#include  <fs/source/core/fs_core_dir_priv.h>
#include  <fs/source/core/fs_core_file_priv.h>
#include  <fs/source/core/fs_core_unicode_priv.h>
#include  <fs/source/core/fs_core_vol_priv.h>
#include  <fs/source/core/fs_core_op_priv.h>
#include  <fs/source/shared/fs_obj_priv.h>
#include  <fs/source/shared/cleanup/cleanup_mgmt_priv.h>

//                                                                 ----------------------- EXT ------------------------
#include  <common/include/lib_mem.h>
#include  <common/source/logging/logging_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  RTOS_MODULE_CUR   RTOS_CFG_MODULE_FS
#define  LOG_DFLT_CH       (FS, FAT, DIR)

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static MEM_DYN_POOL FS_FAT_DirNodePool;
static MEM_DYN_POOL FS_FAT_DirDescPool;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static FS_FAT_DIRENT_BROWSE_OUTCOME FS_FAT_DirChkEmptyCb(void                *p_dirent,
                                                         FS_FAT_SEC_BYTE_POS *p_cur_pos,
                                                         void                *p_dte_info,
                                                         void                *p_arg);
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           FS_FAT_DirModuleInit()
 *
 * @brief    FAT directory module initialization.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/
void FS_FAT_DirModuleInit(RTOS_ERR *p_err)
{
  CPU_SIZE_T init_dir_desc_cnt;
  CPU_SIZE_T max_dir_desc_cnt;
  CPU_SIZE_T init_dir_node_cnt;
  CPU_SIZE_T max_dir_node_cnt;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_SET((FSCore_InitCfg.MaxFatObjCnt.DirDescCnt != 0u)
                            && (FSCore_InitCfg.MaxFatObjCnt.DirNodeCnt != 0u), *p_err, RTOS_ERR_INVALID_ARG,; );

    init_dir_desc_cnt = FSCore_InitCfg.MaxFatObjCnt.DirDescCnt == LIB_MEM_BLK_QTY_UNLIMITED ? 1u : FSCore_InitCfg.MaxFatObjCnt.DirDescCnt;
    max_dir_desc_cnt = FSCore_InitCfg.MaxFatObjCnt.DirDescCnt;

    BREAK_ON_ERR(Mem_DynPoolCreate("FS - FAT dir desc pool",
                                   &FS_FAT_DirDescPool,
                                   FSCore_InitCfg.MemSegPtr,
                                   sizeof(FS_FAT_DIR_DESC),
                                   sizeof(CPU_ALIGN),
                                   init_dir_desc_cnt,
                                   max_dir_desc_cnt,
                                   p_err));

    init_dir_node_cnt = FSCore_InitCfg.MaxFatObjCnt.DirNodeCnt == LIB_MEM_BLK_QTY_UNLIMITED ? 1u : FSCore_InitCfg.MaxFatObjCnt.DirNodeCnt;
    max_dir_node_cnt = FSCore_InitCfg.MaxFatObjCnt.DirNodeCnt;

    BREAK_ON_ERR(Mem_DynPoolCreate("FS - FAT dir node pool",
                                   &FS_FAT_DirNodePool,
                                   FSCore_InitCfg.MemSegPtr,
                                   sizeof(FS_FAT_DIR_NODE),
                                   sizeof(CPU_ALIGN),
                                   init_dir_node_cnt,
                                   max_dir_node_cnt,
                                   p_err));
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                           FS_FAT_DirNodeAlloc()
 *
 * @brief    Allocate a FAT directory node.
 *
 * @param    p_err   Error pointer.
 *
 * @return   Pointer to the allocated directory node.
 *******************************************************************************************************/
FS_DIR_NODE *FS_FAT_DirNodeAlloc(RTOS_ERR *p_err)
{
  FS_FAT_DIR_NODE *p_fat_dir_node;

  p_fat_dir_node = (FS_FAT_DIR_NODE *)Mem_DynPoolBlkGet(&FS_FAT_DirNodePool, p_err);
  Mem_Clr(p_fat_dir_node, sizeof(FS_FAT_DIR_NODE));

  p_fat_dir_node->FirstClusNbr = 0u;

  return (&p_fat_dir_node->DirNode);
}

/****************************************************************************************************//**
 *                                           FS_FAT_DirNodeFree()
 *
 * @brief    Free a FAT directory node.
 *
 * @param    p_dir_node  Pointer to the directory node to be freed.
 *******************************************************************************************************/
void FS_FAT_DirNodeFree(FS_DIR_NODE *p_dir_node)
{
  RTOS_ERR err;

  Mem_DynPoolBlkFree(&FS_FAT_DirNodePool, (void *)p_dir_node, &err);
  RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}

/****************************************************************************************************//**
 *                                           FS_FAT_DirDescAlloc()
 *
 * @brief    Allocate a FAT directory descriptor.
 *
 * @param    p_err   Error pointer.
 *
 * @return   Pointer to the allocated directory descriptor.
 *******************************************************************************************************/
FS_DIR_DESC *FS_FAT_DirDescAlloc(RTOS_ERR *p_err)
{
  FS_FAT_DIR_DESC *p_fat_dir_desc;

  p_fat_dir_desc = (FS_FAT_DIR_DESC *)Mem_DynPoolBlkGet(&FS_FAT_DirDescPool, p_err);
  Mem_Clr(p_fat_dir_desc, sizeof(FS_FAT_DIR_DESC));

  return (&p_fat_dir_desc->DirDesc);
}

/****************************************************************************************************//**
 *                                           FS_FAT_DirDescFree()
 *
 * @brief    Free a directory descriptor.
 *
 * @param    p_dir_desc  Pointer to the directory descriptor to be freed.
 *******************************************************************************************************/
void FS_FAT_DirDescFree(FS_DIR_DESC *p_dir_desc)
{
  RTOS_ERR err;

  Mem_DynPoolBlkFree(&FS_FAT_DirDescPool, (void *)p_dir_desc, &err);
  RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}

/****************************************************************************************************//**
 *                                           FS_FAT_DirNodeInit()
 *
 * @brief    Initialize a directory node with the on-disk entry information.
 *
 * @param    p_dir_node  Pointer to the directory node.
 *
 * @param    entry_pos   Encoded entry position used to find entry location on-disk.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
void FS_FAT_DirNodeInit(FS_DIR_NODE *p_dir_node,
                        FS_SYS_POS  entry_pos,
                        RTOS_ERR    *p_err)
{
  FS_FAT_VOL          *p_fat_vol;
  FS_FAT_DIR_NODE     *p_fat_dir_node;
  FS_FAT_SEC_BYTE_POS de_start_pos;
  FS_FAT_SEC_BYTE_POS de_end_pos;
  FS_FAT_ENTRY_INFO   fat_entry_info;

  WITH_SCOPE_BEGIN(p_err) {
    p_fat_vol = (FS_FAT_VOL *)p_dir_node->VolHandle.VolPtr;

    FS_FAT_POS_DATA_DECODE(entry_pos, de_start_pos);

    BREAK_ON_ERR(FS_FAT_EntryParse(p_fat_vol,
                                   &de_start_pos,
                                   &de_end_pos,
                                   &fat_entry_info,
                                   DEF_NULL,
                                   DEF_NULL,
                                   0u,
                                   p_err));

    p_fat_dir_node = (FS_FAT_DIR_NODE *)p_dir_node;
    p_fat_dir_node->FirstClusNbr = fat_entry_info.FirstClusNbr;
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                           FS_FAT_DirDescInit()
 *
 * @brief    Initialize a directory descriptor.
 *
 * @param    p_dir_desc  Pointer to the directory descriptor.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
void FS_FAT_DirDescInit(FS_DIR_DESC *p_dir_desc,
                        RTOS_ERR    *p_err)
{
  FS_FAT_DIR_DESC *p_fat_dir_desc;

  PP_UNUSED_PARAM(p_err);

  p_fat_dir_desc = (FS_FAT_DIR_DESC *)p_dir_desc;
  p_fat_dir_desc->CurDirEntryIx = 0u;
}

/****************************************************************************************************//**
 *                                               FS_FAT_DirRd()
 *
 * @brief    Read a directory entry from a directory.
 *
 * @param    p_dir_desc      Pointer to a directory descriptor.
 *
 * @param    p_cur_pos       Current on-disk position.
 *
 * @param    p_entry_info    Pointer to an entry info structure.
 *
 * @param    p_name          Pointer to a name buffer (at least FS_CORE_CFG_MAX_FILE_NAME_LEN + 1 byte long).
 *
 * @param    name_buf_size   Size of buffer allocated for the name.
 *
 * @param    p_err           Error pointer.
 *
 * @note     (1) This condition is reached if a directory table span more than 1 sector on the disk.
 *               That is, a file and directory entry is near the end of a sector. This file or
 *               directory entry may need several entries if it is a Long File Name (LFN). Thus an
 *               LFN entry can be at the end of the sector and the following SFN entry will begin at
 *               the next sector. In that case, the condition allows to adjust the current position
 *               given the current sector to consider.
 *******************************************************************************************************/
CPU_BOOLEAN FS_FAT_DirRd(FS_DIR_DESC   *p_dir_desc,
                         FS_SYS_POS    *p_cur_pos,
                         FS_ENTRY_INFO *p_entry_info,
                         CPU_CHAR      *p_name,
                         CPU_SIZE_T    name_buf_size,
                         RTOS_ERR      *p_err)
{
  FS_FAT_VOL          *p_fat_vol;
  FS_FAT_DIR_DESC     *p_fat_dir_desc;
  FS_FAT_DIR_NODE     *p_fat_dir_node;
  FS_FAT_SEC_BYTE_POS cur_pos;
  FS_FAT_SEC_BYTE_POS end_pos;
  FS_FAT_ENTRY_INFO   fat_entry_info;
  CPU_BOOLEAN         entry_parsed;
  FS_FAT_CLUS_NBR     first_clus_nbr;
  CPU_INT08U          lb_size_log2;
  CPU_BOOLEAN         eof = DEF_NO;

  WITH_SCOPE_BEGIN(p_err) {
    p_fat_dir_desc = (FS_FAT_DIR_DESC *)p_dir_desc;
    p_fat_dir_node = (FS_FAT_DIR_NODE *)p_fat_dir_desc->DirDesc.DirNodePtr;
    p_fat_vol = (FS_FAT_VOL *)p_fat_dir_node->DirNode.VolHandle.VolPtr;

    BREAK_ON_ERR(lb_size_log2 = FSBlkDev_LbSizeLog2Get(p_fat_vol->Vol.BlkDevHandle, p_err));

    FS_FAT_POS_DATA_DECODE(*p_cur_pos, cur_pos);
    if (FS_FAT_SEC_BYTE_POS_IS_VOID(cur_pos)) {
      first_clus_nbr = p_fat_dir_node->FirstClusNbr;
      cur_pos.SecOffset = 0u;
      cur_pos.SecNbr = (first_clus_nbr == 0u) ? p_fat_vol->RootDirStart
                       : FS_FAT_CLUS_TO_SEC(p_fat_vol, first_clus_nbr);
    }

    //                                                             ------------ '.' AND '..' FROM VOL ROOT ------------
    if ((p_fat_dir_node->FirstClusNbr == 0u)
        && (p_fat_dir_desc->CurDirEntryIx < FS_FAT_DIR_TBL_SPECIAL_ENTRY_QTY)) {
      if (p_entry_info != DEF_NULL) {
        p_entry_info->Attrib.Wr = DEF_NO;
        p_entry_info->Attrib.Rd = DEF_NO;
        p_entry_info->Attrib.IsDir = DEF_YES;
        p_entry_info->Attrib.Hidden = DEF_YES;
        p_entry_info->Attrib.IsRootDir = DEF_YES;
        p_entry_info->BlkCnt = 0u;
        p_entry_info->BlkSize = FS_UTIL_PWR2(lb_size_log2);
        p_entry_info->DateAccess = 0u;
        p_entry_info->DateTimeCreate = 0u;
        p_entry_info->DateTimeWr = 0u;
        p_entry_info->DevId = p_fat_vol->Vol.BlkDevHandle.BlkDevId;
        p_entry_info->NodeId = p_fat_dir_node->DirNode.Id;
        p_entry_info->Size = 0u;
      }
      if (p_name != DEF_NULL) {
        Str_Copy(p_name, p_fat_dir_desc->CurDirEntryIx ? ".." : ".");
      }

      eof = DEF_NO;
    } else {
      if (cur_pos.SecNbr == FS_FAT_VOID_DATA_LB_NBR && cur_pos.SecOffset == 0) {
        eof = DEF_YES;
        break;
      }
      BREAK_ON_ERR(entry_parsed = FS_FAT_NextEntryParse(p_fat_vol,
                                                        &cur_pos,
                                                        &end_pos,
                                                        &fat_entry_info,
                                                        p_name,
                                                        DEF_NULL,
                                                        name_buf_size,
                                                        p_err));
      if (entry_parsed) {
        if (p_entry_info != DEF_NULL) {
          FS_FAT_EntryInfoGet(p_entry_info, &fat_entry_info);

          p_entry_info->Attrib.IsRootDir = DEF_NO;
          p_entry_info->BlkCnt = 0u;
          p_entry_info->BlkSize = FS_UTIL_PWR2(lb_size_log2);
          p_entry_info->DevId = p_fat_vol->Vol.BlkDevHandle.BlkDevId;
          p_entry_info->NodeId = p_fat_dir_node->DirNode.Id;
        }

        cur_pos.SecOffset = end_pos.SecOffset + FS_FAT_SIZE_DIR_ENTRY;
        if (cur_pos.SecOffset == FS_UTIL_PWR2(lb_size_log2)) {
          BREAK_ON_ERR(cur_pos.SecNbr = FS_FAT_SecNextGet(p_fat_vol,
                                                          cur_pos.SecNbr,
                                                          p_err));
          cur_pos.SecOffset = 0u;
        }

        if (cur_pos.SecNbr != 0 && cur_pos.SecNbr < end_pos.SecNbr) {   // See Note #1.
          cur_pos.SecNbr = end_pos.SecNbr;
        }

        FS_FAT_POS_DATA_ENCODE(cur_pos, *p_cur_pos);
      }

      eof = !entry_parsed;
    }

    p_fat_dir_desc->CurDirEntryIx += 1u;
  } WITH_SCOPE_END

  return (eof);
}

/****************************************************************************************************//**
 *                                           FS_FAT_DirChkEmpty()
 *
 * @brief    Check whether directory entry is empty.
 *
 * @param    p_vol           Pointer to a core volume object.
 *
 * @param    p_entry_pos     On-disk position corresponding to the first cluster of the directory table.
 *
 * @param    p_err           Error pointer.
 *
 * @return   DEF_YES, if directory is     empty.
 *           DEF_NO,  if directory is NOT empty.
 *
 * @note     (1) The first two entries of an empty directory will be the "dot" entry & the "dot dot"
 *               entry, respectively. Consequently, a non-empty directory will have at least three
 *               entries.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
CPU_BOOLEAN FS_FAT_DirChkEmpty(FS_VOL     *p_vol,
                               FS_SYS_POS *p_entry_pos,
                               RTOS_ERR   *p_err)
{
  FS_FAT_VOL                   *p_fat_vol;
  FS_FAT_DIR_CHK_EMPTY_CB_DATA dir_chk_empty_cb_data;
  FS_FAT_SEC_BYTE_POS          dirtbl_pos;
  FS_FAT_SEC_BYTE_POS          dirent_pos;
  CPU_BOOLEAN                  is_empty;

  WITH_SCOPE_BEGIN(p_err) {
    p_fat_vol = (FS_FAT_VOL *)p_vol;
    is_empty = DEF_NO;

    FS_FAT_POS_DATA_DECODE(*p_entry_pos, dirent_pos);

    BREAK_ON_ERR(dirtbl_pos.SecNbr = FS_FAT_EntryDirTblGet(p_fat_vol,
                                                           &dirent_pos,
                                                           p_err));
    dirtbl_pos.SecOffset = 0u;
    dir_chk_empty_cb_data.FreeDirEntryCnt = 0u;

    //                                                             ---------------- CHK FOR 3 ENTRIES -----------------
    BREAK_ON_ERR(FS_FAT_DirTblBrowseAcquire(p_fat_vol,
                                            &dirtbl_pos,
                                            FS_FAT_DirChkEmptyCb,
                                            &dir_chk_empty_cb_data,
                                            FS_CACHE_BLK_GET_MODE_RD,
                                            DEF_NULL,
                                            p_err));

    is_empty = (dir_chk_empty_cb_data.FreeDirEntryCnt < 3u);
  } WITH_SCOPE_END

  return (is_empty);
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           FS_FAT_DirChkEmptyCb()
 *
 * @brief    Directory empty check callback.
 *
 * @param    p_dirent    Pointer to the current directory entry.
 *
 * @param    p_cur_pos   Current position.
 *
 * @param    p_dte_info  Pointer to a FS_FAT_DTE_TYPE structure.
 *
 * @param    p_arg       Pointer to a FS_FAT_DIR_CHK_EMPTY_CB_DATA structure.
 *
 * @return   Browse outcome.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static FS_FAT_DIRENT_BROWSE_OUTCOME FS_FAT_DirChkEmptyCb(void                *p_dirent,
                                                         FS_FAT_SEC_BYTE_POS *p_cur_pos,
                                                         void                *p_dte_info,
                                                         void                *p_arg)
{
  FS_FAT_DIR_CHK_EMPTY_CB_DATA *p_dir_chk_empty_data;
  FS_FAT_DTE_TYPE              dte_type;
  FS_FAT_DIRENT_BROWSE_OUTCOME outcome;

  PP_UNUSED_PARAM(p_dirent);
  PP_UNUSED_PARAM(p_cur_pos);

  dte_type = *((FS_FAT_DTE_TYPE *)p_dte_info);
  p_dir_chk_empty_data = (FS_FAT_DIR_CHK_EMPTY_CB_DATA *)p_arg;
  outcome = FS_FAT_DIRENT_BROWSE_OUTCOME_STOP;

  switch (dte_type) {
    case FS_FAT_DTE_TYPE_LFN:
    case FS_FAT_DTE_TYPE_SFN:
    case FS_FAT_DTE_TYPE_VOL_LABEL:
      p_dir_chk_empty_data->FreeDirEntryCnt++;
    //                                                             fallthrough
    case FS_FAT_DTE_TYPE_ERASED:
    case FS_FAT_DTE_TYPE_INVALID_LFN:
      outcome = FS_FAT_DIRENT_BROWSE_OUTCOME_CONTINUE;
      break;
    case FS_FAT_DTE_TYPE_FREE:
      outcome = FS_FAT_DIRENT_BROWSE_OUTCOME_STOP;
      break;
    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, FS_FAT_DIRENT_BROWSE_OUTCOME_STOP);
  }

  return (outcome);
}
#endif

#endif // FS_CORE_CFG_FAT_EN && FS_CORE_CFG_DIR_EN

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_FS_AVAIL
