/***************************************************************************//**
 * @file
 * @brief File System - Core Operations
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
#include  <fs/source/core/fs_core_priv.h>
#include  <fs/source/core/fs_core_working_dir_priv.h>
#include  <fs/source/core/fs_core_posix_priv.h>
#include  <fs/source/core/fs_core_file_priv.h>
#include  <fs/source/core/fs_core_dir_priv.h>
#include  <fs/source/core/fs_core_vol_priv.h>
#include  <fs/source/core/fs_core_unicode_priv.h>
#include  <fs/source/sys/fs_sys_priv.h>
#include  <fs/source/shared/cleanup/cleanup_mgmt_priv.h>

#if (FS_CORE_CFG_FAT_EN == DEF_ENABLED)
#include  <fs/source/sys/fat/fs_fat_priv.h>
#endif

#ifdef  RTOS_MODULE_COMMON_SHELL_AVAIL
#include  <fs/source/cmd/fs_shell_priv.h>
#endif

//                                                                 ----------------------- EXT ------------------------
#include  <common/include/rtos_err.h>
#include  <common/include/kal.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH       (FS, CORE)
#define  RTOS_MODULE_CUR   RTOS_CFG_MODULE_FS

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 *
 * Note(s) : (1) The double curly braces avoids a warning in GCC compiler saying that "missing braces
 *               around initializer". Single curly braces should be correct normally. But it seems that a
 *               possible GCC bug exists related to this warning :
 *               https://gcc.gnu.org/bugzilla/show_bug.cgi?id=53119
 *               This double curly braces are necessary to accommodate GCC.
 ********************************************************************************************************
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_ENABLED)
extern const FS_CORE_INIT_CFG FSCore_InitCfg;
#else
const FS_CORE_INIT_CFG FSCore_InitCfgDflt = { .MaxCoreObjCnt = { .WrkDirCnt = LIB_MEM_BLK_QTY_UNLIMITED,
                                                                 .RootDirDescCnt = LIB_MEM_BLK_QTY_UNLIMITED },

                                              .MaxFatObjCnt = { .FileDescCnt = LIB_MEM_BLK_QTY_UNLIMITED,
                                                                .FileNodeCnt = LIB_MEM_BLK_QTY_UNLIMITED,
                                                                .DirDescCnt = LIB_MEM_BLK_QTY_UNLIMITED,
                                                                .DirNodeCnt = LIB_MEM_BLK_QTY_UNLIMITED,
                                                                .VolCnt = LIB_MEM_BLK_QTY_UNLIMITED },

                                              .MemSegPtr = DEF_NULL };

FS_CORE_INIT_CFG FSCore_InitCfg = { .MaxCoreObjCnt = { .WrkDirCnt = LIB_MEM_BLK_QTY_UNLIMITED,
                                                       .RootDirDescCnt = LIB_MEM_BLK_QTY_UNLIMITED },

                                    .MaxFatObjCnt = { .FileDescCnt = LIB_MEM_BLK_QTY_UNLIMITED,
                                                      .FileNodeCnt = LIB_MEM_BLK_QTY_UNLIMITED,
                                                      .DirDescCnt = LIB_MEM_BLK_QTY_UNLIMITED,
                                                      .DirNodeCnt = LIB_MEM_BLK_QTY_UNLIMITED,
                                                      .VolCnt = LIB_MEM_BLK_QTY_UNLIMITED },

                                    .MemSegPtr = DEF_NULL };
#endif

FS_CORE_DATA FSCore_Data = { .IsInit = DEF_NO,
                             .OpLock = { { 0 } },               // See Note #1.
                             .CacheBlkDevDataListHeadPtr = DEF_NULL,
                             .CacheBlkDevDataPool = { 0 } };

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static CPU_BOOLEAN FSCore_FeatureValidate(void);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           FSCore_ConfigureMemSeg()
 *
 * @brief    Set the memory segment where file system internal data structures will be allocated.
 *
 * @param    p_seg   Pointer to a memory segment.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void FSCore_ConfigureMemSeg(MEM_SEG *p_seg)
{
  RTOS_ASSERT_DBG(!FSCore_Data.IsInit, RTOS_ERR_ALREADY_INIT,; )

  FSCore_InitCfg.MemSegPtr = p_seg;
}
#endif

/****************************************************************************************************//**
 *                                       FSCore_ConfigureMaxObjCnt()
 *
 * @brief    Set the maximum number of file system core objects.
 *
 * @param    max_cnt     Structure containing the maximum number of each core object.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void FSCore_ConfigureMaxObjCnt(FS_CORE_CFG_MAX_OBJ_CNT max_cnt)
{
  RTOS_ASSERT_DBG(!FSCore_Data.IsInit, RTOS_ERR_ALREADY_INIT,; )

  FSCore_InitCfg.MaxCoreObjCnt = max_cnt;
}
#endif

/****************************************************************************************************//**
 *                                       FSCore_ConfigureMaxFatObjCnt()
 *
 * @brief    Set the maximum number of file system FAT objects.
 *
 * @param    max_cnt     Structure containing the maximum number of each FAT object.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void FSCore_ConfigureMaxFatObjCnt(FS_CORE_CFG_MAX_FAT_OBJ_CNT max_cnt)
{
  RTOS_ASSERT_DBG(!FSCore_Data.IsInit, RTOS_ERR_ALREADY_INIT,; )

  FSCore_InitCfg.MaxFatObjCnt = max_cnt;
}
#endif

/****************************************************************************************************//**
 *                                               FSCore_Init()
 *
 * @brief    Initialize file system.
 *
 * @param    p_err   Pointer to variable that will receive the return error code(s) from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_ALREADY_INIT
 *                       - RTOS_ERR_NOT_AVAIL
 *                       - RTOS_ERR_SEG_OVF
 *******************************************************************************************************/
void FSCore_Init(RTOS_ERR *p_err)
{
  CPU_SIZE_T init_blk_dev_data_cnt;
  CPU_SIZE_T max_blk_dev_data_cnt;
  CPU_SIZE_T media_cnt;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    RTOS_ASSERT_DBG_ERR_SET(!FSCore_Data.IsInit, *p_err, RTOS_ERR_ALREADY_INIT,; )

    //                                                             ------------- VALIDATE KERNEL FEATURES -------------
    RTOS_ASSERT_DBG(FSCore_FeatureValidate(), RTOS_ERR_NOT_AVAIL,; );

    //                                                             -------------- GLOBAL OP LOCK CREATE ---------------
    BREAK_ON_ERR(OpLockCreate(&FSCore_Data.OpLock, p_err));

    //                                                             ----------------- INIT FS MODULES ------------------
#if (FS_CORE_CFG_DIR_EN == DEF_ENABLED)
    BREAK_ON_ERR(FSDir_ModuleInit(p_err));
#endif
    FSVol_ModuleInit();
    BREAK_ON_ERR(FSWrkDir_Init(p_err));

#if (FS_CORE_CFG_POSIX_EN == DEF_ENABLED)
    BREAK_ON_ERR(FSPosix_Init(p_err));
#endif

#ifdef  RTOS_MODULE_COMMON_SHELL_AVAIL
    BREAK_ON_ERR(FSShell_Init(p_err));
#endif

    media_cnt = FSMedia_MaxCntGet();
    init_blk_dev_data_cnt = media_cnt == LIB_MEM_BLK_QTY_UNLIMITED ? 0u : media_cnt;
    max_blk_dev_data_cnt = media_cnt;

    BREAK_ON_ERR(Mem_DynPoolCreate("FS - Core blk dev data",
                                   &FSCore_Data.CacheBlkDevDataPool,
                                   FSCore_InitCfg.MemSegPtr,
                                   sizeof(FS_CACHE_BLK_DEV_DATA),
                                   sizeof(CPU_ALIGN),
                                   init_blk_dev_data_cnt,
                                   max_blk_dev_data_cnt,
                                   p_err));

#if (FS_CORE_CFG_FAT_EN == DEF_ENABLED)
    BREAK_ON_ERR(FS_FAT_Init(p_err));
#endif

    FSCore_Data.IsInit = DEF_YES;
  } WITH_SCOPE_END
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           FSCore_PathSegLenGet()
 *
 * @brief    Return the length (in number of characters) of a give path segment.
 *
 * @param    p_path_seg  Pointer to a path segment.
 *
 * @return   Length of the given path segment.
 *
 * @note     (1) The caller MUST ensure that the given path segment is a valid SFN/LFN path
 *               segment.
 *******************************************************************************************************/
FS_FILE_NAME_LEN FSCore_PathSegLenGet(CPU_CHAR *p_path_seg)
{
  CPU_CHAR         *p_cur_char;
  FS_FILE_NAME_LEN len;
#if (FS_CORE_CFG_UTF8_EN == DEF_ENABLED)
  CPU_SIZE_T char_len;
#endif

  len = 0u;
  p_cur_char = p_path_seg;
  while (*p_cur_char == ASCII_CHAR_SPACE) {
    p_cur_char++;
  }
  while (FS_CHAR_IS_PATH_SEP(*p_cur_char)) {
    p_cur_char++;
  }
  while ((*p_cur_char != ASCII_CHAR_NULL)
         && !FS_CHAR_IS_PATH_SEP(*p_cur_char)) {
#if (FS_CORE_CFG_UTF8_EN == DEF_ENABLED)
    char_len = MB_CharLen(p_cur_char, MB_MAX_LEN);
    p_cur_char += char_len;
#else
    p_cur_char++;
#endif
    len++;
  }

  return (len);
}

/****************************************************************************************************//**
 *                                           FSCore_PathSegIsDot()
 *
 * @brief    Check if the given path segment is the 'dot' path segment.
 *
 * @param    p_path_seg  Pointer to the path segment to be checked.
 *
 * @return   DEF_YES, if 'dot' path segment.
 *           DEF_NO,  if not 'dot' path segment.
 *
 * @note     (1) 'Dot' refers to the current directory location.
 *******************************************************************************************************/
CPU_BOOLEAN FSCore_PathSegIsDot(CPU_CHAR *p_path_seg)
{
  CPU_INT16S cmp;

  while (*p_path_seg == ASCII_CHAR_SPACE) {
    p_path_seg++;
  }
  while (FS_CHAR_IS_PATH_SEP(*p_path_seg)) {
    p_path_seg++;
  }
  cmp = Str_Cmp_N(p_path_seg, "./", 2u);
  cmp &= Str_Cmp_N(p_path_seg, ".", 2u);
  if (cmp == 0u) {
    return (DEF_YES);
  }
  return (DEF_NO);
}

/****************************************************************************************************//**
 *                                           FSCore_PathSegIsDotDot()
 *
 * @brief    Check if the given path segment is the 'dot dot' path segment.
 *
 * @param    p_path_seg  Pointer to the path segment to be checked.
 *
 * @return   DEF_YES, if 'dot dot' path segment.
 *           DEF_NO,  if not the 'dot dot' path segment.
 *
 * @note     (1) 'Dot dot' refers to the current parent directory location.
 *******************************************************************************************************/
CPU_BOOLEAN FSCore_PathSegIsDotDot(CPU_CHAR *p_path_seg)
{
  CPU_BOOLEAN cmp;

  while (*p_path_seg == ASCII_CHAR_SPACE) {
    p_path_seg++;
  }
  while (FS_CHAR_IS_PATH_SEP(*p_path_seg)) {
    p_path_seg++;
  }
  cmp = Str_Cmp_N(p_path_seg, "../", 3u);
  cmp &= Str_Cmp_N(p_path_seg, "..", 3u);
  if (cmp == 0u) {
    return (DEF_YES);
  }
  return (DEF_NO);
}

/****************************************************************************************************//**
 *                                       FSCore_PathLastSegGet()
 *
 * @brief    Get last path segment of a given path.
 *
 * @param    p_path     Pointer to the path to extract last segment from.
 *
 * @return   Pointer to the extracted last path segment.
 *           NULL pointer if the given path is empty.
 *
 *******************************************************************************************************/
CPU_CHAR *FSCore_PathLastSegGet(CPU_CHAR *p_path)
{
  CPU_CHAR *p_last_path_seg;

  while (*p_path == ASCII_CHAR_SPACE) {
    p_path++;
  }
  while (FS_CHAR_IS_PATH_SEP(*p_path)) {
    p_path++;
  }

  p_last_path_seg = DEF_NULL;
  while (p_path != DEF_NULL) {
    p_last_path_seg = p_path;
    p_path = FS_PathNextSegGet(p_path);
  }

  return (p_last_path_seg);
}

/****************************************************************************************************//**
 *                                           FS_PathNextSegGet()
 *
 * @brief    Get next path segment.
 *
 * @param    p_cur_path_seg  Pointer to the current path segment.
 *
 * @return   Pointer to the next path segment.
 *           NULL pointer if given current path segment is the last path segment.
 *******************************************************************************************************/
CPU_CHAR *FS_PathNextSegGet(CPU_CHAR *p_cur_path_seg)
{
  CPU_CHAR *p_cur_char;

  p_cur_char = p_cur_path_seg;
  while (*p_cur_char == ASCII_CHAR_SPACE) {
    p_cur_char++;
  }
  while (FS_CHAR_IS_PATH_SEP(*p_cur_char)) {
    p_cur_char++;
  }
  while (!FS_CHAR_IS_PATH_SEP(*p_cur_char)
         && *p_cur_char != ASCII_CHAR_NULL) {
    p_cur_char++;
  }
  while (FS_CHAR_IS_PATH_SEP(*p_cur_char)) {
    p_cur_char++;
  }

  if (*p_cur_char == ASCII_CHAR_NULL) {
    return (DEF_NULL);
  }
  return (p_cur_char);
}

/****************************************************************************************************//**
 *                                           FSCore_PathReduce()
 *
 * @brief    Skip 'dot' path segments and mutually canceling path segments (i.e. 'dot dot' and
 *           regular path segments combinations).
 *
 * @param    p_path  Pointer to the path segments to be reduced (may contain leading path separator).
 *
 * @return   Pointer to the next irreducible path segment, without leading path separator.
 *******************************************************************************************************/
CPU_CHAR *FSCore_PathReduce(CPU_CHAR *p_path)
{
  CPU_CHAR   *p_peek_seg;
  CPU_CHAR   *p_rd_seg;
  CPU_INT08U depth;

  depth = 0u;
  p_peek_seg = p_path;

  if (p_path == DEF_NULL) {
    return (DEF_NULL);
  }

  while (*p_peek_seg == ASCII_CHAR_SPACE) {                     // Remove leading spaces.
    p_peek_seg++;
  }
  while (FS_CHAR_IS_PATH_SEP(*p_peek_seg)) {                    // Remove leading path sep.
    p_peek_seg++;
  }
  if (*p_peek_seg == ASCII_CHAR_NULL) {                         // Chk if path segment is empty.
    return (DEF_NULL);
  }

  p_rd_seg = p_peek_seg;
  while (p_peek_seg != DEF_NULL) {
    if (FSCore_PathSegIsDot(p_peek_seg)) {
      p_peek_seg = FS_PathNextSegGet(p_peek_seg);
    } else if (FSCore_PathSegIsDotDot(p_peek_seg)) {
      if (depth == 0u) {
        return (p_peek_seg);
      }
      depth--;
      p_peek_seg = FS_PathNextSegGet(p_peek_seg);
      if (depth == 0u) {
        p_rd_seg = p_peek_seg;
      }
    } else {
      depth++;
      p_peek_seg = FS_PathNextSegGet(p_peek_seg);
    }
  }

  while ((p_rd_seg != DEF_NULL)
         && FSCore_PathSegIsDot(p_rd_seg)) {
    p_rd_seg = FS_PathNextSegGet(p_rd_seg);
  }

  return (p_rd_seg);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           FSCore_FeatureValidate()
 *
 * @brief    Validate some features needed by the Core layer to work.
 *
 * @return   DEF_YES, if ALL features are available.
 *           DEF_NO,  if at least one feature is missing.
 *******************************************************************************************************/
static CPU_BOOLEAN FSCore_FeatureValidate(void)
{
  CPU_BOOLEAN ok = DEF_YES;

#if (FS_CORE_CFG_THREAD_SAFETY_EN == DEF_ENABLED)
  ok = ok && KAL_FeatureQuery(KAL_FEATURE_MON, KAL_OPT_NONE);
  ok = ok && KAL_FeatureQuery(KAL_FEATURE_MON_DEL, KAL_OPT_NONE);
#endif

#if (FS_CORE_CFG_FILE_LOCK_EN == DEF_ENABLED)
  ok = ok && KAL_FeatureQuery(KAL_FEATURE_LOCK_CREATE, KAL_OPT_NONE);
  ok = ok && KAL_FeatureQuery(KAL_FEATURE_LOCK_ACQUIRE, KAL_OPT_NONE);
  ok = ok && KAL_FeatureQuery(KAL_FEATURE_LOCK_RELEASE, KAL_OPT_NONE);
  ok = ok && KAL_FeatureQuery(KAL_FEATURE_LOCK_DEL, KAL_OPT_NONE);
#endif

  return (ok);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_FS_AVAIL
