/***************************************************************************//**
 * @file
 * @brief File System - Fat Long File Name (Lfn) Operations
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
  && (FS_FAT_CFG_LFN_EN == DEF_ENABLED))

//                                                                 ----------------------- FAT ------------------------
#include  <fs/source/sys/fat/fs_fat_priv.h>
#include  <fs/source/sys/fat/fs_fat_journal_priv.h>
#include  <fs/source/sys/fat/fs_fat_xfn_priv.h>
#include  <fs/source/sys/fat/fs_fat_fatxx_priv.h>
#include  <fs/source/sys/fat/fs_fat_dirent_priv.h>
#include  <fs/source/sys/fat/fs_fat_entry_priv.h>
//                                                                 ----------------------- CORE -----------------------
#include  <fs/source/core/fs_core_priv.h>
#include  <fs/source/core/fs_core_unicode_priv.h>
#include  <fs/source/core/fs_core_vol_priv.h>

#include  <fs/source/shared/cleanup/cleanup_mgmt_priv.h>

//                                                                 ----------------------- EXT ------------------------
#include  <common/include/lib_mem.h>
#include  <common/include/lib_utils.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH        (FS, FAT, LFN)
#define  RTOS_MODULE_CUR     RTOS_CFG_MODULE_FS

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef struct fs_fat_lfn_name_map_elem {
  CPU_INT08U FirstCharOffset;
  CPU_INT08U CharCnt;
} FS_FAT_LFN_NAME_MAP_ELEM;

typedef CPU_INT32U FS_FAT_SFN_TAIL;

/********************************************************************************************************
 *                               DIR ENTRY BROWSE CALLBACK ARG DATA TYPES
 *******************************************************************************************************/

typedef struct fs_fat_lfn_dirent_find_max_data {
  CPU_INT08U      *Name_8_3_08;
  FS_FAT_SFN_TAIL Max;
} FS_FAT_LFN_DIRENT_FIND_MAX_DATA;

typedef struct fs_fat_lfn_dirent_create_data {
  CPU_CHAR    *NamePtr;
  CPU_INT08U  CharLeftCnt;
  CPU_INT08U  WrDirEntryCnt;
  CPU_INT08U  DirEntryCnt;
  CPU_BOOLEAN IsFinal;
  CPU_INT08U  ChkSum;
  CPU_BOOLEAN Done;
} FS_FAT_LFN_DIRENT_CREATE_DATA;

typedef struct fs_fat_lfn_dirent_get_data {
  FS_FAT_LFN_CHAR     *Name;
  FS_FAT_SEC_BYTE_POS StartPos;
  FS_FLAGS            Status;
  CPU_INT08U          LFNEntryCnt;
  CPU_INT08U          ChkSum;
} FS_FAT_LFN_DIRENT_GET_DATA;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL GLOBAL VARIABLES
 *
 * Note(s) : (1) One LFN entry contains up to 13 characters. Each character is encoded in UTF-16, that is
 *               2 bytes each. If a file name needs more than 13 characters, additional LFN entries are
 *               used.
 ********************************************************************************************************
 *******************************************************************************************************/
//                                                                 Map indicating char positions inside LFN dir entry...
//                                                                 ...See Note #1.
static const FS_FAT_LFN_NAME_MAP_ELEM FS_FAT_LFN_NameOffsetMap[3u] = {
  { 1u, 5u },                                                   // Offset byte 1:  5 char avail from this offset.
  { 14, 6u },                                                   // Offset byte 14: 6 char avail from this offset.
  { 28, 2u }                                                    // Offset byte 28: 2 char avail from this offset.
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static FS_FAT_DIRENT_BROWSE_OUTCOME FS_FAT_LFN_DirEntriesCreateCb(void                *p_dirent,
                                                                  FS_FAT_SEC_BYTE_POS *p_cur_pos,
                                                                  void                *p_dte_info,
                                                                  void                *p_arg);

static void FS_FAT_LFN_DirEntryFmt(void                 *p_dir_entry,
                                   CPU_CHAR             *p_name,
                                   FS_FILE_NAME_LEN     char_cnt,
                                   CPU_INT08U           chk_sum,
                                   FS_FAT_DIR_ENTRY_QTY lfn_nbr,
                                   CPU_BOOLEAN          is_final);

static FS_FILE_NAME_LEN FS_FAT_LFN_SFN_Fmt(CPU_CHAR   *p_name,
                                           CPU_INT32U *p_name_8_3,
                                           RTOS_ERR   *p_err);

static void FS_FAT_LFN_SFN_FmtTail(CPU_INT32U       p_name_8_3[],
                                   FS_FILE_NAME_LEN char_cnt,
                                   FS_FAT_SFN_TAIL  tail_nbr);

static FS_FAT_SFN_TAIL FS_FAT_LFN_SFN_DirEntryFindMax(FS_FAT_VOL     *p_fat_vol,
                                                      CPU_INT32U     name_8_3[],
                                                      FS_FAT_SEC_NBR start_sec,
                                                      RTOS_ERR       *p_err);

static FS_FAT_DIRENT_BROWSE_OUTCOME FS_FAT_LFN_SFN_DirEntryFindMaxCb(void                *p_dirent,
                                                                     FS_FAT_SEC_BYTE_POS *p_cur_pos,
                                                                     void                *p_dte_info,
                                                                     void                *p_arg);
#endif

/********************************************************************************************************
 ********************************************************************************************************
   GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       FS_FAT_LFN_DirEntryCreate()
 *
 * @brief    Create LFN directory entry.
 *
 * @param    p_fat_vol               Pointer to a FAT volume object.
 *
 * @param    p_name                  Pointer to the LFN name for the entry to be created.
 *
 * @param    chk_sum                 Checksum derived from the 8.3 entry name.
 *
 * @param    p_start_pos             Pointer to directory position at which entry should be created.
 *
 * @param    p_entry_wr_job_handle   Pointer to cache job handle.
 *
 * @param    p_err                   Error pointer.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FS_FAT_LFN_DirEntriesCreate(FS_FAT_VOL             *p_fat_vol,
                                 CPU_CHAR               *p_name,
                                 CPU_INT08U             chk_sum,
                                 FS_FAT_SEC_BYTE_POS    *p_start_pos,
                                 FS_CACHE_WR_JOB_HANDLE *p_entry_wr_job_handle,
                                 RTOS_ERR               *p_err)
{
  FS_FILE_NAME_LEN              name_len;
  FS_FAT_LFN_DIRENT_CREATE_DATA dirent_create_at_data;
  FS_FAT_SEC_BYTE_POS           cur_pos;

  WITH_SCOPE_BEGIN(p_err) {
    name_len = FSCore_PathSegLenGet(p_name);

    dirent_create_at_data.NamePtr = p_name;
    dirent_create_at_data.Done = DEF_NO;
    dirent_create_at_data.IsFinal = DEF_YES;
    dirent_create_at_data.ChkSum = chk_sum;
    dirent_create_at_data.CharLeftCnt = name_len;
    dirent_create_at_data.WrDirEntryCnt = 0u;
    dirent_create_at_data.DirEntryCnt = ((FS_FAT_DIR_ENTRY_QTY)((name_len - 1u) / FS_FAT_DIRENT_LFN_NBR_CHARS) + 1u);

    cur_pos = *p_start_pos;
    BREAK_ON_ERR(FS_FAT_DirTblBrowseAcquire(p_fat_vol,
                                            &cur_pos,
                                            FS_FAT_LFN_DirEntriesCreateCb,
                                            &dirent_create_at_data,
                                            FS_CACHE_BLK_GET_MODE_RW,
                                            p_entry_wr_job_handle,
                                            p_err));
  } WITH_SCOPE_END
}
#endif

/****************************************************************************************************//**
 *                                           FS_FAT_LFN_NameChk()
 *
 * @brief    Check whether the given file name is a valid LFN.
 *
 * @param    p_name  Pointer to the name to be checked.
 *
 * @return   One of the following flags indicating whether or not the name is a valid LFN
 *               - FS_FAT_FN_CHK_FLAGS_HAS_INVALID_CHAR
 *               - FS_FAT_FN_CHK_FLAGS_NAME_IS_TOO_LONG
 *******************************************************************************************************/
FS_FLAGS FS_FAT_LFN_NameChk(CPU_CHAR *p_name)
{
  FS_FAT_LFN_CHAR  name_char;
  FS_FILE_NAME_LEN name_len;
  FS_FLAGS         chk_flags;
  CPU_SIZE_T       char_len;

  name_len = 0u;                                                // Dflt val.
  chk_flags = FS_FAT_FN_CHK_FLAGS_NONE;

  //                                                               ------------------ CHK INIT CHAR -------------------
  switch (*p_name) {                                            // Rtn err if first char is ' ', '\0', '\'.
    case ASCII_CHAR_SPACE:
    case ASCII_CHAR_NULL:
    case FS_CHAR_PATH_SEP:
    case FS_CHAR_PATH_SEP_ALT:
      LOG_DBG(("Invalid first char."));
      DEF_BIT_SET(chk_flags, FS_FAT_FN_CHK_FLAGS_HAS_INVALID_CHAR);
      return (chk_flags);
    default:
      break;
  }

  //                                                               ---------------- CHK PATH COMPONENT ----------------
  do {
#if (FS_CORE_CFG_UTF8_EN == DEF_ENABLED)
    char_len = MB_CharToWC(&name_char, p_name, MB_MAX_LEN);
    if (char_len > MB_MAX_LEN) {
      DEF_BIT_SET(chk_flags, FS_FAT_FN_CHK_FLAGS_HAS_INVALID_CHAR);
      return (chk_flags);
    }
#else
    char_len = 1u;
    name_char = *p_name;
#endif

    if ((name_char != (FS_FAT_LFN_CHAR)ASCII_CHAR_NULL)
        && !FS_CHAR_IS_PATH_SEP(name_char)) {
      if (!FS_FAT_IS_LEGAL_LFN_CHAR(name_char)) {
        LOG_DBG(("Invalid character in file name: \'", (c)name_char, "\'."));
        DEF_BIT_SET(chk_flags, FS_FAT_FN_CHK_FLAGS_HAS_INVALID_CHAR);
        return (chk_flags);
      }

      name_len++;
      p_name += char_len;

      if (name_len > FS_FAT_MAX_FILE_NAME_LEN) {
        LOG_DBG(("File name too long.\r\n"));
        DEF_BIT_SET(chk_flags, FS_FAT_FN_CHK_FLAGS_NAME_IS_TOO_LONG);
        return (chk_flags);
      }
    }
  } while ((name_char != (FS_FAT_LFN_CHAR)ASCII_CHAR_NULL)
           && !FS_CHAR_IS_PATH_SEP(name_char));

  return (chk_flags);
}

/****************************************************************************************************//**
 *                                           FS_FAT_LFN_ChkSumCalc()
 *
 * @brief    Calculate LFN checksum.
 *
 * @param    p_name_8_3_08   Pointer to 8.3 entry name.
 *
 * @return   LFN checksum
 *
 * @note     (1) The checksum is calculated according to the algorithm given in [Ref 1], page 28.
 *******************************************************************************************************/
CPU_INT08U FS_FAT_LFN_ChkSumCalc(CPU_INT08U *p_name_8_3_08)
{
  CPU_INT08U chk_sum;
  CPU_INT16U name_8_3_len;

  chk_sum = 0u;
  name_8_3_len = 11u;

  while (name_8_3_len >= 1u) {
    chk_sum = ((DEF_BIT_IS_SET(chk_sum, DEF_BIT_00) == DEF_YES) ? DEF_BIT_07 : DEF_BIT_NONE) + (chk_sum >> 1) + *p_name_8_3_08;
    p_name_8_3_08++;
    name_8_3_len--;
  }

  return (chk_sum);
}

/****************************************************************************************************//**
 *                                           FS_FAT_LFN_NameParse()
 *
 * @brief    Read characters of LFN name in LFN entry.
 *
 * @param    p_dir_entry     Pointer to directory entry.
 *
 * @param    p_name          Pointer to a buffer that will receive entry name characters.
 *******************************************************************************************************/
void FS_FAT_LFN_NameParse(void     *p_dir_entry,
                          CPU_CHAR *p_name)
{
  CPU_INT08U *p_dir_entry_08;
  CPU_INT08U *p_cur_char_08;
  CPU_WCHAR  name_char;
  CPU_INT08U name_offset_map_ix;
  CPU_INT08U char_ix;
  CPU_INT08U first_char_offset;
  CPU_INT08U char_cnt;
  CPU_SIZE_T char_len;

  p_dir_entry_08 = (CPU_INT08U *)p_dir_entry;
  for (name_offset_map_ix = 0u; name_offset_map_ix < 3u; name_offset_map_ix++) {
    first_char_offset = FS_FAT_LFN_NameOffsetMap[name_offset_map_ix].FirstCharOffset;
    char_cnt = FS_FAT_LFN_NameOffsetMap[name_offset_map_ix].CharCnt;
    p_cur_char_08 = p_dir_entry_08 + first_char_offset;
    for (char_ix = 0u; char_ix < char_cnt; char_ix++) {
      name_char = MEM_VAL_GET_INT16U_LITTLE(p_cur_char_08);
      if (name_char == (CPU_WCHAR)ASCII_CHAR_NULL) {
        return;
      }
      char_len = WC_CharToMB(p_name, name_char);
      p_name += char_len;
      p_cur_char_08 += 2u;
    }
  }
}

/****************************************************************************************************//**
 *                                       FS_FAT_LFN_MultiByteLenGet()
 *
 * @brief    Get length of the LFN directory entry name.
 *
 * @param    p_dir_entry     Pointer to LFN entry.
 *
 * @return   length in bytes of characters contained in current LFN entry.
 *******************************************************************************************************/
FS_FILE_NAME_LEN FS_FAT_LFN_MultiByteLenGet(void *p_dir_entry)
{
  CPU_INT08U *p_dir_entry_08;
  CPU_INT08U *p_cur_char_08;
  CPU_WCHAR  name_char;
  CPU_INT08U name_offset_map_ix;
  CPU_INT08U char_ix;
  CPU_INT08U first_char_offset;
  CPU_INT08U char_cnt;
  CPU_SIZE_T name_len;
  CPU_CHAR   mb_char[4];

  name_len = 0u;
  p_dir_entry_08 = (CPU_INT08U *)p_dir_entry;
  for (name_offset_map_ix = 0u; name_offset_map_ix < 3u; name_offset_map_ix++) {
    first_char_offset = FS_FAT_LFN_NameOffsetMap[name_offset_map_ix].FirstCharOffset;
    char_cnt = FS_FAT_LFN_NameOffsetMap[name_offset_map_ix].CharCnt;
    p_cur_char_08 = p_dir_entry_08 + first_char_offset;
    for (char_ix = 0u; char_ix < char_cnt; char_ix++) {
      name_char = MEM_VAL_GET_INT16U_LITTLE(p_cur_char_08);
      if (name_char == (CPU_WCHAR)ASCII_CHAR_NULL) {
        return (name_len);
      }
      name_len += WC_CharToMB(&mb_char[0], name_char);
      p_cur_char_08 += 2u;
    }
  }
  return (name_len);
}

/****************************************************************************************************//**
 *                                           FS_FAT_LFN_NameParseCmp()
 *
 * @brief    Compare directory entry name to the reference name.
 *
 * @param    p_dir_entry     Pointer to directory entry.
 *
 * @param    p_name          Pointer to reference name.
 *
 * @return   DEF_YES, if names are identical.
 *           DEF_NO,  otherwise.
 *
 * @note     (1) Refer to Note#1b of function FS_FAT_LFN_DirEntryFmt() for a brief description of LFN
 *               entry.
 *******************************************************************************************************/
CPU_BOOLEAN FS_FAT_LFN_NameParseCmp(void     *p_dir_entry,
                                    CPU_CHAR *p_name)
{
  CPU_INT08U *p_dir_entry_08;
  CPU_INT08U *p_cur_char_08;
  CPU_WCHAR  name_char;
  CPU_INT08U name_offset_map_ix;
  CPU_WCHAR  cur_wc_char;
  CPU_INT08U char_ix;
  CPU_INT08U first_char_offset;
  CPU_INT08U char_cnt;
  CPU_SIZE_T char_len;

  p_dir_entry_08 = (CPU_INT08U *)p_dir_entry;
  //                                                               Check each chunk of consecutive characters within LFN entry.
  for (name_offset_map_ix = 0u; name_offset_map_ix < 3u; name_offset_map_ix++) {
    first_char_offset = FS_FAT_LFN_NameOffsetMap[name_offset_map_ix].FirstCharOffset;
    char_cnt = FS_FAT_LFN_NameOffsetMap[name_offset_map_ix].CharCnt;
    p_cur_char_08 = p_dir_entry_08 + first_char_offset;

    for (char_ix = 0u; char_ix < char_cnt; char_ix++) {         // Check each character in a given chunk.
      name_char = MEM_VAL_GET_INT16U_LITTLE(p_cur_char_08);
      char_len = MB_CharToWC(&cur_wc_char, p_name, MB_MAX_LEN);

      if ((cur_wc_char == (CPU_WCHAR)0) || FS_CHAR_IS_PATH_SEP(cur_wc_char)) {
        if (name_char != (CPU_WCHAR)0) {
          return (DEF_NO);
        }
        return (DEF_YES);
      }
      //                                                           Case-fold character to perform case-insensitive comparison.
      if (WC_CharToCasefold(name_char) != WC_CharToCasefold(cur_wc_char)) {
        return (DEF_NO);
      }
      p_name += char_len;
      p_cur_char_08 += 2u;                                      // Go to next UTF-16 character in LFN entry chunk.
    }
  }

  return (DEF_YES);
}

/****************************************************************************************************//**
 *                                       FS_FAT_LFN_PathSegPosGet()
 *
 * @brief    Get string between 2 paths segment separators, that is '/' or '\'.
 *
 * @param    p_str   Pointer to buffer that will receive string.
 *
 * @param    pos     Start position from which analyzing path until the next separator.
 *
 * @return   Pointer to string identified.
 *           DEF_NULL if no sting identified.
 *******************************************************************************************************/
CPU_CHAR *FS_FAT_LFN_PathSegPosGet(CPU_CHAR   *p_str,
                                   CPU_SIZE_T pos)
{
  CPU_SIZE_T char_len;
  CPU_SIZE_T cur_pos;

  cur_pos = 0u;
  while (!FS_CHAR_IS_PATH_SEP(*p_str) && (cur_pos < pos)) {
    char_len = MB_CharLen(p_str, MB_MAX_LEN);

    p_str += char_len;
    cur_pos++;
  }

  if (FS_CHAR_IS_PATH_SEP(*p_str)) {
    return (DEF_NULL);
  }
  return (p_str);
}

/****************************************************************************************************//**
 *                                       FS_FAT_LFN_StrCmpIgnoreCase_N()
 *
 * @brief    Determines if two strings are identical for up to a maximum number of characters and
 *           ignore case while comparing.
 *
 * @param    p1_str      Pointer to the first string.
 *
 * @param    p2_str      Pointer to the second string.
 *
 * @param    len_max     Maximum number of characters to compare.
 *
 * @return   0, if strings are identical.
 *           Negative value, if 'p1_str' is less than 'p2_str'.
 *           Positive value, if 'p1_str' is greater than 'p2_str'.
 *
 *           none.
 *******************************************************************************************************/
CPU_INT32S FS_FAT_LFN_StrCmpIgnoreCase_N(CPU_CHAR         *p1_str,
                                         CPU_WCHAR        *p2_str,
                                         FS_FILE_NAME_LEN len_max)
{
  CPU_WCHAR  char1;
  CPU_WCHAR  char2;
  CPU_INT32S cmp_val;
  CPU_SIZE_T cmp_len;
  CPU_SIZE_T char_len;

  cmp_len = 0u;
  char_len = MB_CharToWC(&char1,
                         p1_str,
                         MB_MAX_LEN);
  if (char_len <= MB_MAX_LEN) {
    char1 = WC_CharToCasefold(char1);
  }
  char2 = *p2_str;
  char2 = WC_CharToCasefold(char2);

  while ((char1 == char2)                                       // Cmp strs until non-matching chars ...
         && (char2 != (CPU_WCHAR)0)                             // ... or NULL chars                 ...
         && (char_len != (CPU_SIZE_T)0)                         // ... or NULL ptr(s) found
         && (char_len > (CPU_SIZE_T)0)
         && (cmp_len < (CPU_SIZE_T)len_max)) {                  // ... or max nbr chars cmp'd.
    p1_str += char_len;
    p2_str++;
    cmp_len++;

    if (FS_CHAR_IS_PATH_SEP(*p1_str)) {
      break;
    }

    char_len = MB_CharToWC(&char1,
                           p1_str,
                           MB_MAX_LEN);
    if (char_len <= MB_MAX_LEN) {
      char1 = WC_CharToCasefold(char1);
    }
    char2 = *p2_str;
    char2 = WC_CharToCasefold(char2);
  }

  if (cmp_len == len_max) {                                     // If strs identical for len nbr of chars, ...
    return ((CPU_INT32S)0);                                     // ... rtn 0.
  }

  if ((char_len == 0u)                                          // If NULL char(s) found, ...
      || (char_len > MB_MAX_LEN)) {
    if (char2 == (CPU_WCHAR)0) {
      cmp_val = 0;                                              // ... strs identical; rtn 0.
    } else {
      cmp_val = (CPU_INT32S)0 - (CPU_INT32S)char2;
    }
  } else if (char1 != char2) {                                  // If strs NOT identical, ...
    cmp_val = (CPU_INT32S)char1 - (CPU_INT32S)char2;            // ... calc & rtn char diff.
  } else {                                                      // Else ...
    cmp_val = 0;                                                // ... strs identical; rtn 0.
  }

  return (cmp_val);
}

/****************************************************************************************************//**
 *                                           FS_FAT_LFN_SFN_Alloc()
 *
 * @brief    Allocate SFN.
 *
 * @param    p_fat_vol   Pointer to FAT volume object.
 *
 * @param    p_name      Pointer to the entry's LFN name.
 *
 * @param    p_name_8_3  Pointer to a buffer that will receive the 8.3 name.
 *
 * @param    dir_sec     First sector of the directory table that will contain the entry.
 *
 * @param    p_err       Error pointer.
 *
 * @note     (1) See 'FS_FAT_SFN_Create() Note #1'.
 *
 * @note     (2) The basis SFN is formed according to the rules outlined in [Ref 1] (see
 *               'FS_FAT_LFN_SFN_Fmt() Note #2'), with the tail generation as outlined therein as well
 *               (see 'FS_FAT_LFN_SFN_FmtTail() Note #2').
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FS_FAT_LFN_SFN_Alloc(FS_FAT_VOL     *p_fat_vol,
                          CPU_CHAR       *p_name,
                          CPU_INT32U     *p_name_8_3,
                          FS_FAT_SEC_NBR dir_sec,
                          RTOS_ERR       *p_err)
{
  CPU_INT16U          char_cnt;
  FS_FAT_SEC_BYTE_POS dir_start_pos;
  FS_FAT_SFN_TAIL     tail_nbr;
  FS_FAT_SFN_TAIL     tail_max;
  CPU_BOOLEAN         entry_found;

  WITH_SCOPE_BEGIN(p_err) {
    //                                                             ------------------ FMT INIT NAME -------------------
    //                                                             Fmt SFN from LFN.
    BREAK_ON_ERR(char_cnt = FS_FAT_LFN_SFN_Fmt(p_name, p_name_8_3, p_err));

    //                                                             ------------------- FIND MAX SFN -------------------
    BREAK_ON_ERR(tail_max = FS_FAT_LFN_SFN_DirEntryFindMax(p_fat_vol,
                                                           p_name_8_3,
                                                           dir_sec,
                                                           p_err));
    if (tail_max == 0u) {
      tail_nbr = 1u;
    } else {
      if (tail_max < 999999u) {
        tail_nbr = tail_max + 1u;
      } else {                                                  // -------------- SRCH FOR FREE SFN TAIL --------------
        tail_nbr = 1u;

        BREAK_ON_ERR(while) (tail_nbr <= 999999u) {
          FS_FAT_LFN_SFN_FmtTail(p_name_8_3, char_cnt, tail_nbr);

          //                                                       Srch dir for SFN.
          dir_start_pos.SecNbr = dir_sec;
          dir_start_pos.SecOffset = 0u;
          BREAK_ON_ERR(entry_found = FS_FAT_SFN_EntryFind(p_fat_vol,
                                                          p_name_8_3,
                                                          &dir_start_pos,
                                                          p_err));
          if (!entry_found) {
            break;
          }

          tail_nbr++;                                           // Inc tail nbr.
        }

        ASSERT_BREAK(tail_nbr <= 999999u, RTOS_ERR_NO_MORE_RSRC);
      }
    }

    //                                                             ------------------ SFN TAIL FOUND ------------------
    FS_FAT_LFN_SFN_FmtTail(p_name_8_3, char_cnt, tail_nbr);     // Add tail to SFN.

    LOG_VRB(("SFN ", (s)(CPU_CHAR *)p_name_8_3, " assigned to ", (s)(CPU_CHAR *)p_name, "."));
  } WITH_SCOPE_END
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       FS_FAT_LFN_DirEntryCreateCb()
 *
 * @brief    LFN directory entry creation callback function.
 *
 * @param    p_fat_vol   Pointer to a FAT volume object.
 *
 * @param    p_cur_pos   Pointer to the current directory entry position.
 *
 * @param    p_dte_info  Pointer to the current directory entry's raw content.
 *
 * @param    p_arg       Pointer to LFN entry creation callback argument data.
 *
 * @return   Enumeration indicating whether or not the directory entry browsing should be stop.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static FS_FAT_DIRENT_BROWSE_OUTCOME FS_FAT_LFN_DirEntriesCreateCb(void                *p_dirent,
                                                                  FS_FAT_SEC_BYTE_POS *p_cur_pos,
                                                                  void                *p_dte_info,
                                                                  void                *p_arg)
{
  FS_FAT_LFN_DIRENT_CREATE_DATA *p_dirent_create_at_data;
  CPU_CHAR                      *p_dirent_name;
  CPU_INT08U                    char_cnt_wr;
  CPU_INT08U                    dirent_nbr;
  CPU_SIZE_T                    cur_name_pos;

  PP_UNUSED_PARAM(p_cur_pos);
  PP_UNUSED_PARAM(p_dte_info);

  p_dirent_create_at_data = (FS_FAT_LFN_DIRENT_CREATE_DATA *)p_arg;

  if (p_dirent_create_at_data->IsFinal == DEF_YES) {
    char_cnt_wr = p_dirent_create_at_data->CharLeftCnt % FS_FAT_DIRENT_LFN_NBR_CHARS;
    if (char_cnt_wr == 0u) {
      char_cnt_wr = FS_FAT_DIRENT_LFN_NBR_CHARS;
    }
  } else {
    char_cnt_wr = FS_FAT_DIRENT_LFN_NBR_CHARS;
  }

  cur_name_pos = p_dirent_create_at_data->CharLeftCnt - char_cnt_wr;
#if (FS_CORE_CFG_UTF8_EN == DEF_ENABLED)
  p_dirent_name = FS_FAT_LFN_PathSegPosGet(p_dirent_create_at_data->NamePtr, cur_name_pos);
#else
  p_dirent_name = p_dirent_create_at_data->NamePtr + cur_name_pos;
#endif
  dirent_nbr = p_dirent_create_at_data->DirEntryCnt - p_dirent_create_at_data->WrDirEntryCnt;
  FS_FAT_LFN_DirEntryFmt(p_dirent,
                         p_dirent_name,
                         char_cnt_wr,
                         p_dirent_create_at_data->ChkSum,
                         dirent_nbr,
                         p_dirent_create_at_data->IsFinal);

  p_dirent_create_at_data->CharLeftCnt -= char_cnt_wr;
  p_dirent_create_at_data->WrDirEntryCnt++;
  p_dirent_create_at_data->IsFinal = DEF_NO;

  if (p_dirent_create_at_data->CharLeftCnt == 0u) {
    p_dirent_create_at_data->Done = DEF_YES;
    return (FS_FAT_DIRENT_BROWSE_OUTCOME_STOP);
  }
  return (FS_FAT_DIRENT_BROWSE_OUTCOME_CONTINUE);
}
#endif

/****************************************************************************************************//**
 *                                           FS_FAT_LFN_DirEntryFmt()
 *
 * @brief    Make directory entry for LFN entry.
 *
 * @param    p_dir_entry     Pointer to directory entry raw content.
 *
 * @param    p_name          Pointer to entry's LFN name substring that will be written in
 *                           the given directory entry.
 *
 * @param    char_cnt        Number of characters to write.
 *
 * @param    chk_sum         Checksum derived form 8.3 name.
 *
 * @param    lfn_nbr         LFN sequence number.
 *
 * @param    is_final        Indicates whether LFN entry is the final LFN entry :
 *                           DEF_YES, if the LFN entry is     the final LFN entry.
 *                           DEF_NO,  if the LFN entry is NOT the final LFN entry.
 *
 * @note     (1) The LFN directory entry format is specified in [Ref. 1], Page 28 :
 *           - (a) The 0th byte contains the sequence number of the LFN entry in the LFN, starting
 *                 at 1.  The final LFN entry for the LFN (which is stored in the lowest on-disk
 *                 location) has its 6th bit set.
 *           - (b) Up to 13 Unicode characters are stored in each LFN entry.  If the final LFN entry
 *                 for the LFN has fewer than 13 characters (neglecting a NULL character), then a
 *                 NULL character is placed following the final LFN character.  Remaining character
 *                 slots, if any, are filled with 0xFFFF.
 *                 @verbatim
 *                 ---------------------------------------------------------------------------------
 *                 |    |   #1    |   #2    |   #3    |   #4    |   #5    |    |    |    |   #6    |
 *                 ---------------------------------------------------------------------------------
 *                 |   #7    |   #8    |   #9    |   #10   |   #11   |         |   #12   |   #13   |
 *                 ---------------------------------------------------------------------------------
 *                 @endverbatim
 *           - (c) The 11th byte contains the LFN entry attributes, 0x0F.
 *           - (d) The 12th, 27th & 28th bytes are NULL.
 *           - (e) The 13th byte contains the cheksum derived from the entry's 8.3 name.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_FAT_LFN_DirEntryFmt(void                 *p_dir_entry,
                                   CPU_CHAR             *p_name,
                                   FS_FILE_NAME_LEN     char_cnt,
                                   CPU_INT08U           chk_sum,
                                   FS_FAT_DIR_ENTRY_QTY lfn_nbr,
                                   CPU_BOOLEAN          is_final)
{
  CPU_INT08U       *p_dir_entry_08;
  CPU_INT08U       *p_cur_char_08;
  FS_FILE_NAME_LEN char_cnt_wr;
  CPU_WCHAR        cur_char;
  CPU_INT08U       marker;
  CPU_INT08U       name_offset_map_ix;
  CPU_INT08U       first_char_offset;
  CPU_INT08U       char_ix;
  CPU_INT08U       cur_char_cnt;
  CPU_INT08U       char_len;

  char_cnt_wr = 0u;
  p_dir_entry_08 = (CPU_INT08U *)p_dir_entry;

  //                                                               ----------------- LONG ENTRY MARKER ----------------
  marker = (is_final == DEF_YES) ? (lfn_nbr | FS_FAT_DIRENT_NAME_LFN_LAST_LONG_ENTRY)
           : (lfn_nbr);
  *p_dir_entry_08 = marker;

  //                                                               ----------------- LONG ENTRY NAME ------------------
  for (name_offset_map_ix = 0u; name_offset_map_ix < 3u; name_offset_map_ix++) {
    first_char_offset = FS_FAT_LFN_NameOffsetMap[name_offset_map_ix].FirstCharOffset;
    cur_char_cnt = FS_FAT_LFN_NameOffsetMap[name_offset_map_ix].CharCnt;
    p_cur_char_08 = p_dir_entry_08 + first_char_offset;
    for (char_ix = 0u; char_ix < cur_char_cnt; char_ix++) {
      if (char_cnt_wr > char_cnt) {
        MEM_VAL_SET_INT16U_LITTLE(p_cur_char_08, FS_FAT_LFN_CHAR_EMPTY);
      } else if (char_cnt_wr == char_cnt) {
        MEM_VAL_SET_INT16U_LITTLE(p_cur_char_08, FS_FAT_LFN_CHAR_FINAL);
      } else {
        char_len = MB_CharToWC(&cur_char, p_name, MB_MAX_LEN);
        MEM_VAL_SET_INT16U_LITTLE(p_cur_char_08, cur_char);
        p_name += char_len;
      }
      p_cur_char_08 += 2u;
      char_cnt_wr++;
    }
  }

  p_cur_char_08 = p_dir_entry_08 + FS_FAT_DIRENT_OFFSET_ATTR;   // ---------------- DIR ENTRY ATTRIB'S ----------------
  *p_cur_char_08 = FS_FAT_DIRENT_ATTR_LONG_NAME;                // Long file name.
  p_cur_char_08++;
  *p_cur_char_08 = 0x00u;                                       // NULL byte.
  p_cur_char_08++;
  *p_cur_char_08 = chk_sum;                                     // Checksum.
  p_cur_char_08++;
  //                                                               -------------------- NULL BYTES --------------------
  p_cur_char_08 = p_dir_entry_08 + FS_FAT_DIRENT_OFFSET_FSTCLUSLO;
  *p_cur_char_08 = 0x00u;
  p_cur_char_08++;
  *p_cur_char_08 = 0x00u;
  p_cur_char_08++;
}
#endif

/****************************************************************************************************//**
 *                                           FS_FAT_LFN_SFN_Fmt()
 *
 * @brief    Format SFN from LFN.
 *
 * @param    p_name      Pointer to the LFN name that the SFN will be derived from.
 *
 * @param    p_name_8_3  Pointer to the buffer that will receive the 8.3 name.
 *
 * @param    p_err       Error pointer.
 *
 * @return   The number of characters in the SFN base name.
 *
 * @note     (1) See 'FS_FAT_SFN_Create() Note #1'.
 *
 * @note     (2) The basis SFN is formed according to the rules outlined in Microsoft's
 *               'FAT: General Overview of On-Disk Format' :
 *           - (a) The file name is converted to upper case.
 *           - (b) If the character is not a valid SFN character, then it is replaced by a '_'.
 *           - (c) Leading & embedded spaces are skipped.
 *           - (d) Leading periods are skipped.  The first embedded period ends the primary portion
 *                 of the basis name.
 *                 - (1) The final period separates the base name from its extension.  Once the
 *                       final period is reached, the following characters will be parsed to form
 *                       the extension.
 *           - (e) The base name may contain as many as 8 characters.
 *           - (f) The extension may contain as many as 3 characters.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static FS_FILE_NAME_LEN FS_FAT_LFN_SFN_Fmt(CPU_CHAR   *p_name,
                                           CPU_INT32U *p_name_8_3,
                                           RTOS_ERR   *p_err)
{
  CPU_CHAR         *p_period;
  FS_FILE_NAME_LEN base_sfn_char_cnt;
  FS_FILE_NAME_LEN char_ix;
  CPU_BOOLEAN      is_legal;
  FS_FAT_LFN_CHAR  lfn_name_char;
  CPU_CHAR         name_8_3_08[12];
  CPU_BOOLEAN      processing_base_name;
  CPU_SIZE_T       char_len;

#if (FS_CORE_CFG_UTF8_EN == DEF_DISABLED)
  PP_UNUSED_PARAM(p_err);
#endif

  //                                                               ------------------- CLR SFN BUF --------------------
  for (char_ix = 0u; char_ix < 11u; char_ix++) {                // 'Clear' 8.3 file name buffer.
    name_8_3_08[char_ix] = (CPU_INT08U)ASCII_CHAR_SPACE;
  }
  name_8_3_08[11] = (CPU_INT08U)ASCII_CHAR_NULL;                // 'Clear' final byte of 8.3 file name (see Note #1).

  //                                                               Skip leading period.
  while (*p_name == ASCII_CHAR_FULL_STOP) {
    p_name++;
  }

  char_ix = 0u;
  base_sfn_char_cnt = 0u;
  processing_base_name = DEF_YES;

  while ((processing_base_name
          && (base_sfn_char_cnt < FS_FAT_SFN_NAME_MAX_NBR_CHAR))
         || (!processing_base_name
             && (base_sfn_char_cnt < FS_FAT_SFN_NAME_MAX_NBR_CHAR + FS_FAT_SFN_EXT_MAX_NBR_CHAR))) {
#if (FS_CORE_CFG_UTF8_EN == DEF_ENABLED)
    char_len = MB_CharToWC(&lfn_name_char, p_name, MB_MAX_LEN);
    if (char_len > MB_MAX_LEN) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_NAME_INVALID);
      return (0u);
    }
#else
    char_len = 1u;
    lfn_name_char = *p_name;
#endif

    if (processing_base_name
        && (lfn_name_char == ASCII_CHAR_FULL_STOP)) {
      p_period = Str_Char_Last(p_name, (CPU_CHAR)ASCII_CHAR_FULL_STOP);
      processing_base_name = DEF_NO;
      p_name = p_period + 1u;
      char_ix = FS_FAT_SFN_NAME_MAX_NBR_CHAR;
    } else {
      if (lfn_name_char == ASCII_CHAR_NULL) {                   // Exit loop if NULL char or path sep char reached.
        break;
      }

      if (lfn_name_char != ASCII_CHAR_SPACE) {                  // Skip embedded spaces (see Notes #2c).
                                                                // Chk for legal file name char.
        is_legal = FS_FAT_IS_LEGAL_SFN_CHAR(lfn_name_char);
#if (FS_CORE_CFG_UTF8_EN == DEF_ENABLED)
        if (lfn_name_char > DEF_INT_08U_MAX_VAL) {              // Chk for non-8 bit char.
          is_legal = DEF_NO;
        }
#endif
        if (is_legal == DEF_YES) {
          name_8_3_08[char_ix++] = ASCII_TO_UPPER(lfn_name_char); // See Notes #2a.
        } else {
          name_8_3_08[char_ix++] = (CPU_CHAR)ASCII_CHAR_LOW_LINE; // See Notes #2b.
        }

        if (processing_base_name) {
          base_sfn_char_cnt++;
        }
      }
      p_name += char_len;
    }
  }

  p_name_8_3[0] = MEM_VAL_GET_INT32U_LITTLE((void *)&name_8_3_08[0]);
  p_name_8_3[1] = MEM_VAL_GET_INT32U_LITTLE((void *)&name_8_3_08[4]);
  p_name_8_3[2] = MEM_VAL_GET_INT32U_LITTLE((void *)&name_8_3_08[8]);

  return (base_sfn_char_cnt);
}
#endif

/****************************************************************************************************//**
 *                                           FS_FAT_LFN_SFN_FmtTail()
 *
 * @brief    Format SFN tail.
 *
 * @param    p_name_8_3  Entry SFN.
 *
 * @param    char_cnt    Number of characters in base SFN.
 *
 * @param    tail_nbr    Number to use for tail.
 *
 * @note     (1) The numeric tail is formatted according to the numeric-tail generation algorithm
 *               in [Ref 1] :
 *           - (a) The numeric tail is a string '~n' where 'n' is a decimal number between 1 &
 *                 999999.
 *           - (b) This function MUST never be called with 'tail_nbr' > 999999; a check for this is
 *                 merely added for completeness.
 *
 * @note     (2) If the numeric tail can be appended to the base SFN WITHOUT overflowing the
 *               8-character limit, then it is appended; otherwise, the final characters of the
 *               8-character SFN will be the numeric tail (thus overwriting as few of the name
 *               characters as possible).
 *
 *           - (a) For example, if the name encoded in 'name_8_3' is 'abcd.txt' & 'tail_nbr'
 *                 is 12, then the resulting SFN will be 'abcd~12.txt'.
 *
 *           - (b) For example, if the name encoded in 'name_8_3' is 'abcd.txt' & 'tail_nbr'
 *                 is 12345, then the resulting SFN will be 'ab~12345.txt'.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_FAT_LFN_SFN_FmtTail(CPU_INT32U       *p_name_8_3,
                                   FS_FILE_NAME_LEN char_cnt,
                                   FS_FAT_SFN_TAIL  tail_nbr)
{
  CPU_INT16U       char_cnt_tail;
  FS_FILE_NAME_LEN char_ix;
  CPU_INT08U       name_8_3_08[12];
  CPU_INT08U       tail_nbr_dig;

  //                                                               --------------- CALC NBR TAIL CHAR's ---------------
  if (tail_nbr < 10u) {
    char_cnt_tail = 1u;
  } else if (tail_nbr < 100u) {
    char_cnt_tail = 2u;
  } else if (tail_nbr < 1000u) {
    char_cnt_tail = 3u;
  } else if (tail_nbr < 10000u) {
    char_cnt_tail = 4u;
  } else if (tail_nbr < 100000u) {
    char_cnt_tail = 5u;
  } else if (tail_nbr < 1000000u) {
    char_cnt_tail = 6u;
  } else {
    LOG_DBG(("Tail nbr ", (u)tail_nbr, " > 999999."));
    return;                                                     // See Notes #1b.
  }

  //                                                               ------------------- FMT TAIL STR -------------------
  //                                                               See Notes #2.
  char_ix = 0u;
  if (FS_FAT_SFN_NAME_MAX_NBR_CHAR >= char_cnt +  char_cnt_tail + 1u) {
    char_ix += char_cnt;
  } else {
    char_ix += (FS_FAT_SFN_NAME_MAX_NBR_CHAR - 1u) - char_cnt_tail;
  }

  MEM_VAL_SET_INT32U_LITTLE((void *)&name_8_3_08[0], p_name_8_3[0]);
  MEM_VAL_SET_INT32U_LITTLE((void *)&name_8_3_08[4], p_name_8_3[1]);
  MEM_VAL_SET_INT32U_LITTLE((void *)&name_8_3_08[8], p_name_8_3[2]);

  name_8_3_08[char_ix] = (CPU_INT08U)ASCII_CHAR_TILDE;
  char_ix += char_cnt_tail;

  while (char_cnt_tail > 0u) {
    tail_nbr_dig = (CPU_INT08U)(tail_nbr % 10u);
    name_8_3_08[char_ix] = tail_nbr_dig + (CPU_INT08U)ASCII_CHAR_DIGIT_ZERO;
    tail_nbr /= DEF_NBR_BASE_DEC;
    char_ix--;
    char_cnt_tail--;
  }

  p_name_8_3[0] = MEM_VAL_GET_INT32U_LITTLE((void *)&name_8_3_08[0]);
  p_name_8_3[1] = MEM_VAL_GET_INT32U_LITTLE((void *)&name_8_3_08[4]);
  p_name_8_3[2] = MEM_VAL_GET_INT32U_LITTLE((void *)&name_8_3_08[8]);
}
#endif

/****************************************************************************************************//**
 *                                       FS_FAT_LFN_SFN_DirEntryFindMax()
 *
 * @brief    Search directory for find SFN directory entries with the same stem & return max tail
 *           value.
 *
 * @param    p_fat_vol   Pointer to FAT volume object.
 *
 * @param    p_name_8_3  8.3 entry's name.
 *
 * @param    start_sec   Directory table sector at which search should start.
 *
 * @param    p_err       Error pointer.
 *
 * @return   Maximum tail value, if any is found.
 *           0,                  otherwise.
 *
 * @note     (1) The 'p_temp' pointer is 4-byte aligned since all buffers are 4-byte aligned.
 *               All pointers to directory entries are 4-byte aligned since all directory entries
 *               lie at a offset multiple of 32 (the size of a directory entry) from the beginning
 *               of a sector.
 *
 * @note     (2) The sector number gotten from the FAT should be valid. These checks are effectively
 *               redundant.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static FS_FAT_SFN_TAIL FS_FAT_LFN_SFN_DirEntryFindMax(FS_FAT_VOL     *p_fat_vol,
                                                      CPU_INT32U     name_8_3[],
                                                      FS_FAT_SEC_NBR start_sec,
                                                      RTOS_ERR       *p_err)
{
  FS_FAT_SEC_BYTE_POS             start_pos;
  FS_FAT_LFN_DIRENT_FIND_MAX_DATA dirent_find_max_data;
  CPU_INT08U                      name_8_3_08[FS_FAT_SFN_NAME_PLUS_EXT_MAX_NBR_CHAR + 1];

  MEM_VAL_SET_INT32U_LITTLE((void *)&name_8_3_08[0], name_8_3[0]);
  MEM_VAL_SET_INT32U_LITTLE((void *)&name_8_3_08[4], name_8_3[1]);
  MEM_VAL_SET_INT32U_LITTLE((void *)&name_8_3_08[8], name_8_3[2]);

  start_pos.SecNbr = start_sec;
  start_pos.SecOffset = 0u;
  dirent_find_max_data.Max = 0u;
  dirent_find_max_data.Name_8_3_08 = &name_8_3_08[0];
  FS_FAT_DirTblBrowseAcquire(p_fat_vol,
                             &start_pos,
                             FS_FAT_LFN_SFN_DirEntryFindMaxCb,
                             &dirent_find_max_data,
                             FS_CACHE_BLK_GET_MODE_RD,
                             DEF_NULL,
                             p_err);

  return (dirent_find_max_data.Max);
}
#endif

/****************************************************************************************************//**
 *                                   FS_FAT_LFN_SFN_DirEntryFindMaxCb()
 *
 * @brief    Max tail lookup callback function.
 *
 * @param    p_fat_vol   Pointer to a FAT volume object.
 *
 * @param    p_cur_pos   Pointer to the current directory entry position.
 *
 * @param    p_dte_info  Pointer to the current directory entry raw content.
 *
 * @param    p_arg       Pointer to the argument data structure.
 *
 * @return   Enumeration indication whether or not the directory entry browsing should stop.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static FS_FAT_DIRENT_BROWSE_OUTCOME FS_FAT_LFN_SFN_DirEntryFindMaxCb(void                *p_dirent,
                                                                     FS_FAT_SEC_BYTE_POS *p_cur_pos,
                                                                     void                *p_dte_info,
                                                                     void                *p_arg)
{
  CPU_INT08U                      *p_dirent_08;
  FS_FAT_LFN_DIRENT_FIND_MAX_DATA *p_dirent_find_max_data;
  CPU_INT08U                      *p_name_char;
  CPU_INT08U                      *p_name_cmp_char;
  FS_FAT_DTE_TYPE                 dte_type;
  CPU_INT08U                      cmp_len;
  FS_FAT_SFN_TAIL                 max_temp;

  PP_UNUSED_PARAM(p_cur_pos);

  p_dirent_08 = (CPU_INT08U *)p_dirent;
  p_dirent_find_max_data = (FS_FAT_LFN_DIRENT_FIND_MAX_DATA *)p_arg;

  dte_type = *((FS_FAT_DTE_TYPE *)p_dte_info);

  switch (dte_type) {
    case FS_FAT_DTE_TYPE_FREE:
      return (FS_FAT_DIRENT_BROWSE_OUTCOME_STOP);
    case FS_FAT_DTE_TYPE_ERASED:
    case FS_FAT_DTE_TYPE_INVALID_LFN:
    case FS_FAT_DTE_TYPE_LFN:
    case FS_FAT_DTE_TYPE_VOL_LABEL:
      return (FS_FAT_DIRENT_BROWSE_OUTCOME_CONTINUE);
    case FS_FAT_DTE_TYPE_SFN:
      break;
    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, FS_FAT_DIRENT_BROWSE_OUTCOME_STOP);
  }

  p_name_char = p_dirent_find_max_data->Name_8_3_08;
  p_name_cmp_char = p_dirent_08;
  cmp_len = 0u;

  while ((*p_name_cmp_char == *p_name_char)                     // Cmp first 6 chars of name & dir entry.
         && (cmp_len < FS_FAT_SFN_MAX_STEM_LEN)) {
    p_name_cmp_char++;
    p_name_char++;
    cmp_len++;
  }

  if (cmp_len > 0u) {                                           // Matching char(s) found.
    if (*p_name_cmp_char != ASCII_CHAR_TILDE) {
      return (FS_FAT_DIRENT_BROWSE_OUTCOME_CONTINUE);
    }

    p_name_cmp_char++;                                          // Move past tilde.
    max_temp = 0u;
    while (cmp_len < FS_FAT_SFN_NAME_MAX_NBR_CHAR - 1u) {
      if ((ASCII_IS_DIG(*p_name_cmp_char) == DEF_NO)            // Chk if tail char is dig.
          && (ASCII_IS_SPACE(*p_name_cmp_char) == DEF_NO)) {
        return (FS_FAT_DIRENT_BROWSE_OUTCOME_CONTINUE);         // Invalid tail found.
      }

      max_temp *= 10u;
      max_temp += (FS_FAT_SFN_TAIL)(*p_name_cmp_char - ASCII_CHAR_DIG_ZERO);
      cmp_len++;
      p_name_cmp_char++;
    }

    p_dirent_find_max_data->Max = DEF_MAX(p_dirent_find_max_data->Max, max_temp);
  }

  return (FS_FAT_DIRENT_BROWSE_OUTCOME_CONTINUE);
}
#endif

#endif // FS_CORE_CFG_FAT_EN && FS_CORE_CFG_LFN_EN

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_FS_AVAIL
