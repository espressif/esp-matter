/***************************************************************************//**
 * @file
 * @brief File System - Fat Volume Operations
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
#include  <fs/source/core/fs_core_priv.h>
#include  <fs/source/storage/fs_blk_dev_priv.h>
#include  <fs/source/core/fs_core_partition_priv.h>
#include  <fs/source/sys/fs_sys_priv.h>
#include  <fs/source/core/fs_core_unicode_priv.h>

//                                                                 ----------------------- FAT ------------------------
#include  <fs/source/sys/fat/fs_fat_fatxx_priv.h>
#include  <fs/source/sys/fat/fs_fat_journal_priv.h>
#include  <fs/source/sys/fat/fs_fat_xfn_priv.h>
#include  <fs/source/sys/fat/fs_fat_vol_priv.h>
#include  <fs/source/sys/fat/fs_fat_dirent_priv.h>
#include  <fs/source/sys/fat/fs_fat_entry_priv.h>
#include  <fs/source/sys/fat/fs_fat_priv.h>

//                                                                 ----------------------- EXT ------------------------
#include  <cpu/include/cpu.h>
#include  <common/include/lib_mem.h>
#include  <fs/source/shared/cleanup/cleanup_mgmt_priv.h>
#include  <common/source/preprocessor/preprocessor_priv.h>
#include  <common/source/rtos/rtos_utils_priv.h>

#include  <em_core.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                (FS, FAT, VOL)
#define  RTOS_MODULE_CUR                            RTOS_CFG_MODULE_FS

/********************************************************************************************************
 *                                         FAT TYPE DEFINES
 *
 * Note(s): (1) The FAT type is determined solely by the count of clusters on the volume. A FAT12
 *              volume cannot contain more than 4084 clusters. A FAT16 volume cannot contain less than
 *              4085 clusters or more than 65,524 clusters.
 *              See section '3.5 Determination of FAT type when mounting the volume' of "Microsoft FAT
 *              Specification, August 30 2005" for more details.
 *******************************************************************************************************/

#define  FS_FAT_MAX_NBR_CLUS_FAT12                  4084u
#define  FS_FAT_MAX_NBR_CLUS_FAT16                  65524u
#define  FS_FAT_CLUS_NBR_TOLERANCE                  16u

#define  FS_FAT_MAX_SIZE_HUGE_FAT16                (32u  * 1024u * 1024u)
#define  FS_FAT_MAX_SIZE_FAT12                     (4u   * 1024u * 1024u)
#define  FS_FAT_MAX_SIZE_FAT16                     (512u * 1024u * 1024u)

/********************************************************************************************************
 *                                           FSINFO DEFINES
 *******************************************************************************************************/

//                                                                 ------------------ FSINFO OFFSETS ------------------
#define  FS_FAT_FSI_OFF_LEADSIG                            0u
#define  FS_FAT_FSI_OFF_RESERVED1                          4u
#define  FS_FAT_FSI_OFF_STRUCSIG                         484u
#define  FS_FAT_FSI_OFF_FREE_COUNT                       488u
#define  FS_FAT_FSI_OFF_NXT_FREE                         492u
#define  FS_FAT_FSI_OFF_RESERVED2                        496u
#define  FS_FAT_FSI_OFF_TRAILSIG                         508u

//                                                                 -------------------- FSINFO VAL'S ------------------
#define  FS_FAT_FSI_LEADSIG                       0x41615252u
#define  FS_FAT_FSI_STRUCSIG                      0x61417272u
#define  FS_FAT_FSI_TRAILSIG                      0xAA550000u

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef struct fs_fat_tbl_entry {
  FS_FAT_SEC_NBR VolSecCnt;
  FS_FAT_SEC_NBR ClusSize;
} FS_FAT_TBL_ENTRY;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 *
 * Note(s) : (1) These tables represent the default cluster size for FAT12, FAT16 and FAT32
 *           https://support.microsoft.com/en-us/help/140365/default-cluster-size-for-ntfs,-fat,-and-exfat
 ********************************************************************************************************
 *******************************************************************************************************/

static MEM_DYN_POOL FS_FAT_VolPool;

#if (FS_FAT_CFG_FAT12_EN == DEF_ENABLED)
static const FS_FAT_TBL_ENTRY FS_FAT_TblFAT12[] = { {        36u, 0u },
                                                    {        37u, 1u },
                                                    {      4040u, 2u },
                                                    {      8057u, 4u },
                                                    {     16040u, 8u },
                                                    {     32050u, 16u },
                                                    {     64060u, 32u },
                                                    {    128080u, 64u },
                                                    {    256120u, 0u },
                                                    {         0u, 0u } };
#endif

#if (FS_FAT_CFG_FAT16_EN == DEF_ENABLED)
static const FS_FAT_TBL_ENTRY FS_FAT_TblFAT16[] = { {      8400u, 0u },
                                                    {     32680u, 2u },
                                                    {    262000u, 4u },
                                                    {    524000u, 8u },
                                                    {   1048000u, 16u },
                                                    {   2096000u, 32u },
                                                    {   4194304u, 64u },
                                                    { 4294967295u, 0u },
                                                    {         0u, 0u } };
#endif

#if (FS_FAT_CFG_FAT32_EN == DEF_ENABLED)
static const FS_FAT_TBL_ENTRY FS_FAT_TblFAT32[] = { {     66600u, 0u },
                                                    {    532480u, 1u },
                                                    {  16777216u, 8u },
                                                    {  33554432u, 16u },
                                                    {  67108864u, 32u },
                                                    { 4294967295u, 64u },
                                                    {         0u, 0u } };
#endif

#if (FS_TEST_FAT_JNL_EN == DEF_ENABLED)
extern CPU_INT08U FS_Test_FAT_JournalTestNumber;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL MACROS
 ********************************************************************************************************
 *******************************************************************************************************/

#define  FS_FAT_TYPE_STR_GET(type)             ((type == FS_FAT_TYPE_FAT12) ? "FAT12"   \
                                                : (type == FS_FAT_TYPE_FAT16) ? "FAT16" \
                                                : (type == FS_FAT_TYPE_FAT32) ? "FAT32" : "unknown")

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void FS_FAT_VolBootSecChk(FS_FAT_VOL *p_fat_vol,
                                 CPU_INT08U *p_temp_08,
                                 RTOS_ERR   *p_err);

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static CPU_BOOLEAN FS_FAT_VolCacheFlushPredicate(FS_CACHE          *p_cache,
                                                 FS_CACHE_BLK_DESC *p_blk_desc,
                                                 void              *p_fat_vol_v);
#endif

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_LBClr(FS_BLK_DEV_HANDLE blk_dev_handle,
                     FS_LB_NBR         start,
                     FS_LB_NBR         cnt,
                     CPU_INT08U        lb_type,
                     RTOS_ERR          *p_err);

static void FS_FAT_VolBootSecMake(void           *p_temp,
                                  FS_FAT_VOL_CFG *p_sys_cfg,
                                  FS_LB_SIZE     sec_size,
                                  FS_FAT_SEC_NBR size,
                                  FS_FAT_SEC_NBR fat_size,
                                  FS_LB_NBR      partition_start);
#endif

static void FS_FAT_GetSysCfg(FS_FAT_SEC_SIZE sec_size,
                             FS_FAT_SEC_NBR  sec_cnt,
                             FS_FAT_VOL_CFG  *p_sys_cfg,
                             RTOS_ERR        *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               FS_FAT_VolAlloc()
 *
 * @brief    Allocate a FAT volume object.
 *
 * @param    p_err   Error pointer.
 *
 * @return   Pointer to the allocated volume object.
 *******************************************************************************************************/
FS_VOL *FS_FAT_VolAlloc(RTOS_ERR *p_err)
{
  FS_VOL *p_vol;

  p_vol = (FS_VOL *)Mem_DynPoolBlkGet(&FS_FAT_VolPool, p_err);
  Mem_Clr(p_vol, sizeof(FS_VOL));

  return (p_vol);
}

/****************************************************************************************************//**
 *                                               FS_FAT_VolFree()
 *
 * @brief    Free a FAT volume object.
 *
 * @param    p_vol   Pointer to a FAT volume object.
 *******************************************************************************************************/
void FS_FAT_VolFree(FS_VOL *p_vol)
{
  RTOS_ERR err;

  RTOS_ERR_SET(err, RTOS_ERR_NONE);

  Mem_DynPoolBlkFree(&FS_FAT_VolPool,
                     (void *)p_vol,
                     &err);
  RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}

/****************************************************************************************************//**
 *                                               FS_FAT_VolOpen()
 *
 * @brief    Detect file system on volume & initialize associated structures.
 *
 * @param    p_vol   Pointer to volume.
 *
 * @param    opt     Open options.
 *                   FS_FAT_VOL_OPT_JOURNAL_DISABLED
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/
void FS_FAT_VolOpen(FS_VOL   *p_vol,
                    FS_FLAGS opt,
                    RTOS_ERR *p_err)
{
  FS_FAT_VOL *p_fat_vol;
  CPU_INT08U *p_buf;
  CPU_INT16U boot_sig;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_SET(FS_FAT_Data.IsInit, *p_err, RTOS_ERR_NOT_INIT,; );

    p_fat_vol = (FS_FAT_VOL *)p_vol;

    BREAK_ON_ERR(FS_VOL_CACHE_BLK_RD)(p_vol, 0u, FS_FAT_LB_TYPE_RSVD_AREA, &p_buf, p_err) {
      //                                                           Check signature in bytes 510:511 of sec.
      boot_sig = MEM_VAL_GET_INT16U_LITTLE((void *)(p_buf + 510u));

      ASSERT_BREAK_LOG(boot_sig == FS_FAT_BOOT_SIG, RTOS_ERR_VOL_FMT_INVALID,
                       ("Invalid boot sec sig: ", (X)boot_sig, " != ", (X)FS_FAT_BOOT_SIG));

      //                                                           --------- DETERMINE & APPLY FILE SYS PROPS ---------
      BREAK_ON_ERR(FS_FAT_VolBootSecChk(p_fat_vol,
                                        p_buf,
                                        p_err));
    }

    p_fat_vol->QueryInfoValid = DEF_NO;

#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
    p_fat_vol->FatWrJobHandle = JobSched_VoidJobHandle;
#endif

#if (FS_FAT_CFG_JOURNAL_EN == DEF_ENABLED)
#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
    p_fat_vol->JournalData.JnlWrJobHandle = FSCache_VoidWrJobHandle;
#endif
    if (DEF_BIT_IS_CLR(opt, FS_FAT_VOL_OPT_JOURNAL_DISABLED)) {
      p_fat_vol->IsJournaled = DEF_YES;
      BREAK_ON_ERR(FS_FAT_JournalOpen(p_fat_vol, p_err));
    } else {
      p_fat_vol->IsJournaled = DEF_NO;
    }
#else
    PP_UNUSED_PARAM(opt);
#endif

    //                                                             ---------------- OUTPUT TRACE INFO -----------------
#if LOG_DBG_IS_EN()
    {
      FS_FAT_SEC_SIZE lb_size;
      FS_FAT_SEC_NBR  sec_per_clus;

      BREAK_ON_ERR(lb_size = FSBlkDev_LbSizeGet(p_vol->BlkDevHandle, p_err));
      sec_per_clus = FS_UTIL_PWR2(p_fat_vol->SecPerClusLog2);

      LOG_DBG(("FS_FAT_VolOpen(): File system found: Type     : ", (s)FS_FAT_TYPE_STR_GET(p_fat_vol->FAT_Type)));
      LOG_DBG(("                                     Sec  size: ", (u)lb_size, " bytes"));
      LOG_DBG(("                                     Clus size: ", (u)sec_per_clus, " sectors"));
      LOG_DBG(("                                     # Clus   : ", (u)(p_fat_vol->ClusCnt)));
    }
#endif
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                               FS_FAT_VolQuery()
 *
 * @brief    Get info about file system on a volume.
 *
 * @param    p_vol   Pointer to FAT volume.
 *
 * @param    p_info  Pointer to structure that will receive file system information.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/
void FS_FAT_VolQuery(FS_VOL      *p_vol,
                     FS_SYS_INFO *p_info,
                     RTOS_ERR    *p_err)
{
  FS_FAT_TYPE_API *p_fat_api;
  FS_FAT_VOL      *p_fat_vol;
  FS_FAT_CLUS_NBR bad_clus_cnt;
  FS_FAT_CLUS_NBR clus;
  FS_FAT_CLUS_NBR fat_entry;
  FS_FAT_CLUS_NBR free_clus_cnt;
  FS_FAT_CLUS_NBR used_clus_cnt;

  //                                                               ---------------- ASSIGN DFLT VAL'S -----------------
  p_info->BadSecCnt = 0u;
  p_info->FreeSecCnt = 0u;
  p_info->UsedSecCnt = 0u;
  p_info->TotSecCnt = 0u;

  WITH_SCOPE_BEGIN(p_err) {
    p_fat_vol = (FS_FAT_VOL *)p_vol;
    p_fat_api = FS_FAT_TYPE_API_GET(p_fat_vol);
    //                                                             Get FAT specific info.
    p_info->SysTypeStrPtr = FS_FAT_TYPE_STR_GET(p_fat_vol->FAT_Type);
    if (p_info->DataInfoPtr != DEF_NULL) {
      FS_FAT_VOL_CFG *p_fat_cfg_info = (FS_FAT_VOL_CFG *)p_info->DataInfoPtr;
      FS_LB_SIZE     lb_size;
      FS_LB_QTY      lb_cnt;

      BREAK_ON_ERR(lb_size = FSBlkDev_LbSizeGet(p_vol->BlkDevHandle, p_err));
      BREAK_ON_ERR(lb_cnt = FSBlkDev_LbCntGet(p_vol->BlkDevHandle, p_err));

      BREAK_ON_ERR(FS_FAT_GetSysCfg(lb_size,
                                    lb_cnt,
                                    p_fat_cfg_info,
                                    p_err));
    }

    //                                                             ---------------- CHK IF INFO CACHED ----------------
    if (p_fat_vol->QueryInfoValid) {
      p_info->BadSecCnt = FS_UTIL_MULT_PWR2(p_fat_vol->QueryBadClusCnt, p_fat_vol->SecPerClusLog2);
      p_info->FreeSecCnt = FS_UTIL_MULT_PWR2(p_fat_vol->QueryFreeClusCnt, p_fat_vol->SecPerClusLog2);
      p_info->TotSecCnt = FS_UTIL_MULT_PWR2(p_fat_vol->ClusCnt, p_fat_vol->SecPerClusLog2);
      p_info->UsedSecCnt = p_info->TotSecCnt - p_info->BadSecCnt - p_info->FreeSecCnt;
    } else {
      p_fat_api = FS_FAT_TYPE_API_GET(p_fat_vol);
      //                                                           --------- CNT NBR OF BAD/FREE/USED CLUS'S ----------
      free_clus_cnt = 0u;
      used_clus_cnt = 0u;
      bad_clus_cnt = 0u;
      clus = FS_FAT_MIN_CLUS_NBR;
      BREAK_ON_ERR(while) (clus < FS_FAT_MIN_CLUS_NBR + p_fat_vol->ClusCnt) {
        BREAK_ON_ERR(fat_entry = p_fat_api->ClusValRd(p_fat_vol,
                                                      clus,
                                                      p_err));
        if (fat_entry == p_fat_api->ClusBad) {
          bad_clus_cnt++;
        } else if (fat_entry == p_fat_api->ClusFree) {
          free_clus_cnt++;
        } else {
          used_clus_cnt++;
        }
        clus++;
      }

      //                                                           ------------------ CALC SEC CNT'S ------------------
      p_fat_vol->QueryInfoValid = DEF_YES;
      p_fat_vol->QueryBadClusCnt = bad_clus_cnt;
      p_fat_vol->QueryFreeClusCnt = free_clus_cnt;

      p_info->UsedSecCnt = FS_UTIL_MULT_PWR2(used_clus_cnt, p_fat_vol->SecPerClusLog2);
      p_info->FreeSecCnt = FS_UTIL_MULT_PWR2(free_clus_cnt, p_fat_vol->SecPerClusLog2);
      p_info->BadSecCnt = FS_UTIL_MULT_PWR2(bad_clus_cnt, p_fat_vol->SecPerClusLog2);
      p_info->TotSecCnt = p_info->UsedSecCnt + p_info->FreeSecCnt + p_info->BadSecCnt;
    }
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                               FS_FAT_VolClose()
 *
 * @brief    Close a FAT volume.
 *
 * @param    p_vol   Pointer to volume.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/
void FS_FAT_VolClose(FS_VOL   *p_vol,
                     RTOS_ERR *p_err)
{
#if (FS_FAT_CFG_JOURNAL_EN == DEF_ENABLED)
  FS_FAT_VOL *p_fat_vol;
  p_fat_vol = (FS_FAT_VOL *)p_vol;
  if (p_fat_vol->IsJournaled) {
    FS_FAT_JournalClose((FS_FAT_VOL *)p_vol, p_err);            // Free journal data.
  }
#else
  PP_UNUSED_PARAM(p_vol);
  PP_UNUSED_PARAM(p_err);
#endif
}

/****************************************************************************************************//**
 *                                       FS_FAT_VolCacheFlushPredicate()
 *
 * @brief    Predicate called to determine if a cache block needs to be flushed.
 *
 * @param    p_cache         Pointer to a cache instance.
 *
 * @param    p_blk_desc      Pointer to block descriptor.
 *
 * @param    p_fat_vol_v     Pointer to a FAT volume.
 *
 * @return   DEF_YES, if the cache block needs to be flushed.
 *           DEF_NO,  otherwise
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static CPU_BOOLEAN FS_FAT_VolCacheFlushPredicate(FS_CACHE          *p_cache,
                                                 FS_CACHE_BLK_DESC *p_blk_desc,
                                                 void              *p_fat_vol_v)
{
  FS_FAT_VOL     *p_fat_vol;
  FS_FAT_SEC_NBR partition_start;
  FS_FAT_SEC_NBR partition_end;

  PP_UNUSED_PARAM(p_cache);

  p_fat_vol = (FS_FAT_VOL *)p_fat_vol_v;

  partition_start = p_fat_vol->Vol.PartitionStart;
  partition_end = partition_start + p_fat_vol->Vol.PartitionSize;

  if (FS_BLK_DEV_HANDLE_ARE_EQUAL(p_blk_desc->BlkDevHandle, p_fat_vol->Vol.BlkDevHandle)
      && (p_blk_desc->LbNbr >= partition_start)
      && (p_blk_desc->LbNbr < partition_end)) {
    return (DEF_YES);
  }

  return (DEF_NO);
}
#endif

/****************************************************************************************************//**
 *                                               FS_FAT_VolSync()
 *
 * @brief    Sync cache blocks which belong to the given volume.
 *
 * @param    p_vol   Pointer to a FAT volume.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FS_FAT_VolSync(FS_VOL   *p_vol,
                    RTOS_ERR *p_err)
{
  FS_FAT_VOL *p_fat_vol;

  WITH_SCOPE_BEGIN(p_err) {
    p_fat_vol = (FS_FAT_VOL *)p_vol;

    BREAK_ON_ERR(FSCache_Flush(p_vol->CacheBlkDevDataPtr->CachePtr,
                               FS_FAT_VolCacheFlushPredicate,
                               (void *)p_fat_vol,
                               DEF_NO,
                               p_err));

#if (FS_FAT_CFG_JOURNAL_EN == DEF_ENABLED)
    if (p_fat_vol->IsJournaled) {
      BREAK_ON_ERR(FS_FAT_JournalClrReset(p_fat_vol, p_err));

      BREAK_ON_ERR(FSCache_Flush(p_vol->CacheBlkDevDataPtr->CachePtr,
                                 FS_FAT_VolCacheFlushPredicate,
                                 (void *)p_fat_vol,
                                 DEF_NO,
                                 p_err));

#if (FS_TEST_FAT_JNL_EN == DEF_ENABLED)
      if ((FS_Test_FAT_JournalTestNumber == FSENTRY_RENAME_FILE4)
          || (FS_Test_FAT_JournalTestNumber == FSFILE_TRUNCATE_SHORTEN3)
          || (FS_Test_FAT_JournalTestNumber == FSENTRY_DEL_FILE3)
          || (FS_Test_FAT_JournalTestNumber == FSENTRY_TIMESET2)
          || (FS_Test_FAT_JournalTestNumber == FSENTRY_ATTRIBSET2)) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
      }
#endif
    }
#else
    PP_UNUSED_PARAM(p_fat_vol);
    PP_UNUSED_PARAM(p_err);
#endif
  } WITH_SCOPE_END
}
#endif

/****************************************************************************************************//**
 *                                               FS_FAT_VolFmt()
 *
 * @brief    Format a volume.
 *
 * @param    blk_dev_handle         Block device handle.
 *
 * @param    partition_nbr          Partition number.
 *
 * @param    partition_sec_start    Partition start offset in sectors from media start.
 *
 * @param    partition_sec_cnt      Partition size in number of sectors.
 *
 * @param    p_sys_cfg              Pointer to a file system driver configuration structure.
 *
 * @param    p_err                  Error pointer.
 *
 * @note     (1) The minimum number of sectors in a FAT volume is calculated by :
 *
 *                   MinSecCnt = RsvdSecCnt + RootDirSecCnt + FATSecCnt(min) * NbrFATs + DataClusCnt(min) * ClusSize_sec + (ClusSize_sec - 1);
 *
 *               where
 *
 *                   RsvdSecCnt    is the number of sectors in the reserved area.
 *                   RootDirSecCnt is the number of sectors in the root directory.
 *                   FATSecCnt     is the number of sectors in the each FAT.
 *                   NbrFATs       is the number of FATs.
 *                   DataSecCnt    is the number of sectors in the data area.
 *
 *           - (a) The number of sectors in the root directory is calculated by :
 *
 *                                               RootDirEntriesCnt * 32       RootDirEntriesCnt * 32 + SecSize
 *                       RootDirSecCnt = ceiling( ------------------------ ) = ----------------------------------
 *                                                       BytesPerSec                           SecSize
 *
 *                   where
 *
 *                       SecSize           is the size of a sector, in octets.
 *                       RootDirEntriesCnt is the number of root directory entries.
 *
 *           - (b) The number of sectors in each FAT is calculated by :
 *
 *                                                   DataClusCnt               DataClusCnt + (SecSize / BytesPerEntry) - 1
 *                       FATSecCnt = ceiling( ----------------------------- ) = ---------------------------------------------
 *                                               SecSize / BytesPerEntry                   SecSize / BytesPerEntry
 *
 *                   where
 *
 *                       SecSize       is the size of a sector, in octets.
 *                       BytesPerEntry is the number of bytes in one FAT entry.
 *                       DataClusCnt   is the number of clusters in the data area.
 *
 *           - (c) For example, a minimum-size FAT32 volume, with a default number of reserved
 *                   sectors (32) & number of FATs (2), with 512-byte sectors & 1 sector per
 *                   cluster, would need
 *
 *                                           (65535 + 1) + (512 / 4) - 1
 *                       MinSecCnt = 32 + 0 + ----------------------------- * 2 + (65535 + 1) * 1 = 66592 sectors
 *                                                       512 / 4
 *
 *                   where all multiplication & divide operations are integer divide & multiply.
 *
 *               - (1) [Ref 1] recommends that no volume be formatted with close to 4085 or 65525
 *                       clusters, the minimum cluster counts for FAT16 & FAT32 volumes, since some
 *                       FAT driver implementations do not determine properly FAT type.  In case
 *                       volumes formatted with this file system suite are used with such
 *                       non-compliant FAT driver implementations, no volume is formatted with
 *                       4069-5001 or 65509-65541 clusters.
 *
 * @note     (3) The first sector of the first cluster is always ClusSize_sec-aligned.
 *
 * @note     (4) The number of sectors in the data area of the FAT is related to the number of sectors
 *               in the file system :
 *
 *                   TotSecCnt = DataSecCnt + (FATSecCnt * NbrFATs) + RootDirSecCnt + RsvdSecCnt
 *
 *                                                               DataClusCnt
 *                           = DataSecCnt + ceiling(-----------------------------) * NbrFATs + RootDirSecCnt + RsvdSecCnt
 *                                                       SecSize / BytesPerEntry
 *
 *                                               DataSecCnt / ClusSize_sec
 *                           > DataSecCnt + (-----------------------------) * NbrFATs + RootDirSecCnt + RsvdSecCnt
 *                                               SecSize / BytesPerEntry
 *
 *               so
 *
 *                                   (TotSecCnt - RootDirSecCnt - RsvdSecCnt) * (SecSize / BytesPerEntry)
 *                   DataSecCnt < --------------------------------------------------------------------------------
 *                               (SecSize * ClusSize_sec + BytesPerEntry * NbrFATs) / (ClusSize_sec * BytesPerEntry)
 *
 *                               (TotSecCnt - RootDirSecCnt - RsvdSecCnt) * (SecSize * ClusSize_sec)
 *                               < -------------------------------------------------------------------
 *                                           SecSize * ClusSize_sec + BytesPerEntry * NbrFATs
 *
 *               Since the numerator MAY overflow a 32-bit unsigned integer, the calculation is
 *               re-ordered & rounded up to provide an upper limit:
 *
 *                                       TotSecCnt - RootDirSecCnt - RsvdSecCnt
 *                   DataSecCnt < (------------------------------------------------ + 1) * (SecSize * ClusSize_sec)
 *                                   SecSize * ClusSize_sec + BytesPerEntry * NbrFATs
 *
 * @note     (5) The number of sectors in a FAT is calculated from the data sector count, as calculated
 *               above.  Each FAT will have sufficient entries for the data area; depending on the
 *               configuration, it MAY have extra entries or sector(s).
 *
 * @note     (6) The boot sector & BPB structure is covered in [Ref 1], Pages 9-10.
 *
 *           - (a) Several fields are assigned 'default' values :
 *
 *               - (1) BS_jmpBoot is assigned {0xEB, 0x00, 0x90}.
 *               - (2) BS_OEMName is assigned "MSWIN4.1".
 *               - (3) BPB_Media  is assigned 0xF8.
 *
 *           - (b) Several fields are always assigned 'zero' values :
 *
 *               - (1) BPB_SecPerTrk.
 *               - (2) BPB_NumHeads.
 *               - (3) BPB_HiddSec.
 *
 *           - (c) (1) For FAT32, several more fields are always assigned 'zero' values :
 *
 *                   - (a) BPB_RootEntCnt.
 *                   - (b) BPB_TotSec16.
 *                   - (c) BPB_FatSz16.
 *
 *                       The remaining fields contain information describing the file system ;
 *
 *                   - (a) BPB_BytsPerSec.
 *                   - (b) BPB_SecPerClus.
 *                   - (c) BPB_RsvdSecCnt.
 *                   - (d) BPB_NumFATs (should be 2).
 *                   - (e) BPB_TotSec32.
 *
 *               - (2) For FAT12/16, several more fields are always assigned 'zero' values :
 *
 *                   - (a) BPB_TotSec32.
 *
 *                       The remaining fields contain information describing the file system :
 *
 *                   - (a) BPB_RootEntCnt (defaults to 512).
 *                   - (b) BPB_TotSec16.
 *                   - (c) BPB_FatSz16.
 *                   - (d) BPB_BytsPerSec.
 *                   - (e) BPB_SecPerClus.
 *                   - (f) BPB_RsvdSecCnt (should be 1).
 *                   - (g) BPB_NumFATs (should be 2).
 *
 * @note     (7) Default case already invalidated by prior assignment of 'sys_cfg.FAT_Type'.  However,
 *               the default case is included an extra precaution in case 'sys_cfg.FAT_Type' is
 *               incorrectly modified.
 *
 * @note     (8) Avoid 'Excessive shift value' or 'Constant expression evaluates to 0' warning.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FS_FAT_VolFmt(FS_BLK_DEV_HANDLE blk_dev_handle,
                   FS_PARTITION_NBR  partition_nbr,
                   FS_LB_NBR         partition_sec_start,
                   FS_LB_QTY         partition_sec_cnt,
                   void              *p_sys_cfg,
                   RTOS_ERR          *p_err)
{
  FS_FAT_VOL_CFG        *p_fat_sys_cfg;
  CPU_INT08U            *p_buf;
  FS_CACHE_BLK_DEV_DATA *p_cache_blk_dev_data;
  FS_FAT_VOL_CFG        sys_cfg;
  FS_FAT_CLUS_NBR       data_size_bytes;
  CPU_INT08U            fat_ix;
  FS_FAT_SEC_NBR        fat_sec_cnt;
  FS_FAT_SEC_NBR        fat_sec_cur;
  FS_FAT_SEC_NBR        fat1_sec_start;
  FS_FAT_SEC_NBR        root_dir_sec_cnt;
  FS_FAT_SEC_NBR        root_dir_sec_start;
  FS_FAT_SEC_NBR        rsvd_sec_start;
  FS_FAT_SEC_NBR        data_sec_start;
  CPU_INT32U            val;
  CPU_INT32U            clus_number = 0;
  FS_LB_SIZE            lb_size;
#if (FS_CORE_CFG_PARTITION_EN == DEF_ENABLED)
  CPU_INT08U partition_type = FS_PARTITION_TYPE_EMPTY;
#endif

#if (FS_CORE_CFG_PARTITION_EN == DEF_DISABLED)
  PP_UNUSED_PARAM(partition_nbr);
#endif

  WITH_SCOPE_BEGIN(p_err) {
    //                                                             --------------------- CHK INIT ---------------------
#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
    {
      CORE_DECLARE_IRQ_STATE;
      CORE_ENTER_ATOMIC();
      if (!FS_FAT_Data.IsInit) {
        CORE_EXIT_ATOMIC();
        RTOS_DBG_FAIL_EXEC(RTOS_ERR_NOT_INIT,; );
      }
      CORE_EXIT_ATOMIC();
    }
#endif

    p_cache_blk_dev_data = FSCache_BlkDevDataGet(blk_dev_handle);
    ASSERT_BREAK_LOG((p_cache_blk_dev_data != DEF_NULL), RTOS_ERR_NULL_PTR,
                     ("Blk dev must be assigned to a cache instance"));

    BREAK_ON_ERR(lb_size = FSBlkDev_LbSizeGet(blk_dev_handle, p_err));
    switch (lb_size) {                                          // Verify bytes per sec.
      case 512u:
      case 1024u:
      case 2048u:
      case 4096u:
        break;

      default:
        LOG_ERR(("Invalid sec size: ", (u)lb_size, "."));
        RTOS_ERR_SET(*p_err, RTOS_ERR_SIZE_INVALID);
        return;
    }

    if (p_sys_cfg == DEF_NULL) {
      BREAK_ON_ERR(FS_FAT_GetSysCfg(lb_size,                    // Get FAT configuration.
                                    partition_sec_cnt,
                                    &sys_cfg,
                                    p_err));
    } else {
      p_fat_sys_cfg = (FS_FAT_VOL_CFG *)p_sys_cfg;

      sys_cfg.FAT_Type = p_fat_sys_cfg->FAT_Type;
      sys_cfg.RootDirEntryCnt = p_fat_sys_cfg->RootDirEntryCnt;
      sys_cfg.RsvdAreaSizeInSec = p_fat_sys_cfg->RsvdAreaSizeInSec;
      sys_cfg.ClusSizeInSec = p_fat_sys_cfg->ClusSizeInSec;
      sys_cfg.NbrFATs = p_fat_sys_cfg->NbrFATs;

      ASSERT_BREAK_LOG((sys_cfg.ClusSizeInSec != 0u) && (sys_cfg.ClusSizeInSec <= 128u),
                       RTOS_ERR_VOL_FMT_INVALID,
                       ("Cfg'd cluster size invalid: ", (u)sys_cfg.ClusSizeInSec, "."));

      ASSERT_BREAK_LOG((sys_cfg.ClusSizeInSec & (sys_cfg.ClusSizeInSec - 1u)) == 0u,
                       RTOS_ERR_VOL_FMT_INVALID,
                       ("Cfg'd cluster size invalid: ", (u)sys_cfg.ClusSizeInSec, "."));

      if ((sys_cfg.NbrFATs != 1u) && (sys_cfg.NbrFATs != 2u)) {
        sys_cfg.NbrFATs = 2u;
        LOG_DBG(("Setting nbr of FAT's to 2."));
      }

      BREAK_ON_ERR(switch) (sys_cfg.FAT_Type) {
#if (FS_FAT_CFG_FAT12_EN == DEF_ENABLED)
        case FS_FAT_TYPE_FAT12:
          if (sys_cfg.RootDirEntryCnt == 0u) {
            sys_cfg.RootDirEntryCnt = DEF_MIN(512u, 1u + (partition_sec_cnt / 2u));
          }
          if (sys_cfg.RsvdAreaSizeInSec == 0u) {
            sys_cfg.RsvdAreaSizeInSec = FS_FAT_DFLT_RSVD_SEC_CNT_FAT12;
            LOG_DBG(("Setting FAT rsvd area size to default: ", (u)FS_FAT_DFLT_RSVD_SEC_CNT_FAT12, "."));
          }
          break;
#endif

#if (FS_FAT_CFG_FAT16_EN == DEF_ENABLED)
        case FS_FAT_TYPE_FAT16:
          if (sys_cfg.RootDirEntryCnt == 0u) {
            sys_cfg.RootDirEntryCnt = (CPU_INT16U)DEF_MIN(512u, 1u + (partition_sec_cnt / 2u));
          }
          if (sys_cfg.RsvdAreaSizeInSec == 0u) {
            sys_cfg.RsvdAreaSizeInSec = FS_FAT_DFLT_RSVD_SEC_CNT_FAT16;
            LOG_DBG(("Setting FAT rsvd area size to default: ", (u)FS_FAT_DFLT_RSVD_SEC_CNT_FAT16, "."));
          }
          break;
#endif

#if (FS_FAT_CFG_FAT32_EN == DEF_ENABLED)
        case FS_FAT_TYPE_FAT32:
          if (sys_cfg.RootDirEntryCnt != 0u) {
            sys_cfg.RootDirEntryCnt = 0u;
            LOG_DBG(("Setting root dir entry cnt to 0 for FAT32."));
          }
          if (sys_cfg.RsvdAreaSizeInSec == 0u) {
            sys_cfg.RsvdAreaSizeInSec = FS_FAT_DFLT_RSVD_SEC_CNT_FAT32;
            LOG_DBG(("Setting FAT rsvd area size to default: ", (u)FS_FAT_DFLT_RSVD_SEC_CNT_FAT32, "."));
          } else if (sys_cfg.RsvdAreaSizeInSec < FS_FAT_DFLT_BKBOOTSEC_SEC_NBR + 1u) {
            //                                                     Make sure there is enough room for backup boot sec.
            LOG_DBG(("Cfg'd FAT rsvd area size invalid: ", (u)sys_cfg.RsvdAreaSizeInSec, "."));
            BREAK_ERR_SET(RTOS_ERR_VOL_FMT_INVALID);
          }
          break;
#endif
        default:
          BREAK_ERR_SET(RTOS_ERR_VOL_FMT_INVALID);
      }
    }

    BREAK_ON_ERR(FSBlkDev_Trim(blk_dev_handle,                  // Release dev secs.
                               partition_sec_start,
                               partition_sec_cnt,
                               p_err));

    root_dir_sec_cnt = (((FS_FAT_SEC_NBR)sys_cfg.RootDirEntryCnt * FS_FAT_SIZE_DIR_ENTRY) + (lb_size - 1u)) / lb_size;
    ASSERT_BREAK_LOG(partition_sec_cnt > root_dir_sec_cnt + sys_cfg.RsvdAreaSizeInSec,
                     RTOS_ERR_VOL_FMT_INVALID, ("Rsvd area too large for partition size."));

    data_size_bytes = 0u;
    fat_sec_cnt = 0u;
    fat1_sec_start = 0u;
    root_dir_sec_start = 0u;
    BREAK_ON_ERR(switch) (sys_cfg.FAT_Type) {
#if (FS_FAT_CFG_FAT16_EN == DEF_ENABLED)
      case FS_FAT_TYPE_FAT16:                                   // ----------------- CALC FMT PARAM'S -----------------
                                                                // Calc data sec cnt (see Note #4).
        data_size_bytes = ((((partition_sec_cnt - root_dir_sec_cnt) - sys_cfg.RsvdAreaSizeInSec)
                            /  (((FS_FAT_SEC_NBR)sys_cfg.ClusSizeInSec * lb_size) + (FS_FAT_FAT16_ENTRY_NBR_OCTETS * (FS_FAT_SEC_NBR)sys_cfg.NbrFATs))) + 1u) * lb_size;
        //                                                         Calc fat sec cnt (see Note #5).
        fat_sec_cnt = (data_size_bytes + (lb_size / FS_FAT_FAT16_ENTRY_NBR_OCTETS) - 1u)
                      / (lb_size / FS_FAT_FAT16_ENTRY_NBR_OCTETS);

#if (FS_CORE_CFG_PARTITION_EN == DEF_ENABLED)
        //                                                         Find the FAT16 type
        partition_type = (partition_sec_cnt * lb_size) > FS_FAT_MAX_SIZE_HUGE_FAT16
                         ? FS_PARTITION_TYPE_FAT16_CHS_32MB_2GB
                         : FS_PARTITION_TYPE_FAT16_16_32MB;
#endif

        //                                                         Calc start sec nbr's.
        rsvd_sec_start = 0u;
        fat1_sec_start = rsvd_sec_start + sys_cfg.RsvdAreaSizeInSec;
        root_dir_sec_start = fat1_sec_start + (fat_sec_cnt * sys_cfg.NbrFATs);

        data_sec_start = root_dir_sec_start + root_dir_sec_cnt;
        clus_number = DEF_MIN((partition_sec_cnt - data_sec_start) / sys_cfg.ClusSizeInSec, (fat_sec_cnt * lb_size) / FS_FAT_FAT16_ENTRY_NBR_OCTETS);

        ASSERT_BREAK_LOG(data_sec_start <= partition_sec_cnt, RTOS_ERR_VOL_FMT_INVALID,
                         ("Not enough room for data area."));

        if ((clus_number <= FS_FAT_MAX_NBR_CLUS_FAT12) || (clus_number > FS_FAT_MAX_NBR_CLUS_FAT16)) {
          LOG_ERR(("FS_FAT_VolFmt(): Invalid cluster count: ", (u)clus_number, "."));
          BREAK_ERR_SET(RTOS_ERR_VOL_FMT_INVALID);
        } else if ((clus_number <= (FS_FAT_MAX_NBR_CLUS_FAT12 + FS_FAT_CLUS_NBR_TOLERANCE))
                   || (clus_number > (FS_FAT_MAX_NBR_CLUS_FAT16 - FS_FAT_CLUS_NBR_TOLERANCE))) {
          LOG_DBG(("Cluster count within unrecommended margin: ", (u)clus_number, "."));
        } else {
          ;
        }

        //                                                         --------------------- FMT DISK ---------------------
        BREAK_ON_ERR(FS_CACHE_BLK_WR)(p_cache_blk_dev_data,
                                      partition_sec_start,
                                      FS_FAT_LB_TYPE_RSVD_AREA,
                                      JobSched_VoidJobHandle,
                                      &p_buf,
                                      DEF_NULL,
                                      p_err) {
          Mem_Clr((void *)p_buf, lb_size);
          FS_FAT_VolBootSecMake(p_buf,
                                &sys_cfg,
                                lb_size,
                                partition_sec_cnt,
                                fat_sec_cnt,
                                partition_sec_start);
        }
        break;
#endif

#if (FS_FAT_CFG_FAT32_EN == DEF_ENABLED)
      case FS_FAT_TYPE_FAT32:                                   // ----------------- CALC FMT PARAM'S -----------------
                                                                // Calc data sec cnt (see Note #4).
        data_size_bytes = ((((partition_sec_cnt - root_dir_sec_cnt) - sys_cfg.RsvdAreaSizeInSec)
                            /  ((sys_cfg.ClusSizeInSec * lb_size) + (FS_FAT_FAT32_ENTRY_NBR_OCTETS * (FS_FAT_SEC_NBR)sys_cfg.NbrFATs))) + 1u) * lb_size;
        //                                                         Calc fat sec cnt (see Note #5).
        fat_sec_cnt = (data_size_bytes + (lb_size / FS_FAT_FAT32_ENTRY_NBR_OCTETS) - 1u)
                      / (lb_size / FS_FAT_FAT32_ENTRY_NBR_OCTETS);

#if (FS_CORE_CFG_PARTITION_EN == DEF_ENABLED)
        //                                                         FAT32 type
        partition_type = FS_PARTITION_TYPE_FAT32_LBA;
#endif
        //                                                         Proj'd data sec nbr.
        data_sec_start = (fat_sec_cnt * sys_cfg.NbrFATs) + root_dir_sec_cnt + sys_cfg.RsvdAreaSizeInSec;

        //                                                         Calc start sec nbr's.
        rsvd_sec_start = 0u;
        fat1_sec_start = rsvd_sec_start + sys_cfg.RsvdAreaSizeInSec;
        root_dir_sec_start = fat1_sec_start + (fat_sec_cnt * sys_cfg.NbrFATs);

        data_sec_start = root_dir_sec_start + root_dir_sec_cnt;
        clus_number = DEF_MIN((partition_sec_cnt - data_sec_start) / sys_cfg.ClusSizeInSec, (fat_sec_cnt * lb_size) / FS_FAT_FAT32_ENTRY_NBR_OCTETS);

        ASSERT_BREAK_LOG(data_sec_start <= partition_sec_cnt, RTOS_ERR_VOL_FMT_INVALID,
                         ("Not enough room for data area."));

        if (clus_number <= FS_FAT_MAX_NBR_CLUS_FAT16) {
          LOG_ERR(("FS_FAT_VolFmt(): Invalid cluster count: ", (u)clus_number, "."));
          BREAK_ERR_SET(RTOS_ERR_VOL_FMT_INVALID);
        } else if (clus_number <= (FS_FAT_MAX_NBR_CLUS_FAT16 + FS_FAT_CLUS_NBR_TOLERANCE)) {
          LOG_DBG(("Cluster count within unrecommended margin: ", (u)clus_number, "."));
        } else {
          ;
        }

        //                                                         --------------------- FMT DISK ---------------------
        BREAK_ON_ERR(FS_CACHE_BLK_WR)(p_cache_blk_dev_data,
                                      partition_sec_start,
                                      FS_FAT_LB_TYPE_RSVD_AREA,
                                      JobSched_VoidJobHandle,
                                      &p_buf,
                                      DEF_NULL,
                                      p_err) {
          Mem_Clr((void *)p_buf, lb_size);
          FS_FAT_VolBootSecMake(p_buf,
                                &sys_cfg,
                                lb_size,
                                partition_sec_cnt,
                                fat_sec_cnt,
                                partition_sec_start);
        }

        BREAK_ON_ERR(FS_CACHE_BLK_WR)(p_cache_blk_dev_data,
                                      partition_sec_start + FS_FAT_DFLT_BKBOOTSEC_SEC_NBR,
                                      FS_FAT_LB_TYPE_RSVD_AREA,
                                      JobSched_VoidJobHandle,
                                      &p_buf,
                                      DEF_NULL,
                                      p_err) {
          Mem_Clr((void *)p_buf, lb_size);
          FS_FAT_VolBootSecMake(p_buf,
                                &sys_cfg,
                                lb_size,
                                partition_sec_cnt,
                                fat_sec_cnt,
                                partition_sec_start);
        }

        break;
#endif

#if (FS_FAT_CFG_FAT12_EN == DEF_ENABLED)
      case FS_FAT_TYPE_FAT12:                                   // ----------------- CALC FMT PARAM'S -----------------
                                                                // Calc data sec cnt (see Note #4).
        data_size_bytes = ((((partition_sec_cnt - root_dir_sec_cnt) - sys_cfg.RsvdAreaSizeInSec)
                            /  (((FS_FAT_SEC_NBR)sys_cfg.ClusSizeInSec * lb_size) + ((FS_FAT_SEC_NBR)sys_cfg.NbrFATs + ((FS_FAT_SEC_NBR)sys_cfg.NbrFATs / 2u)))) + 1u) * lb_size;
        //                                                         Calc fat sec cnt (see Note #5).
        fat_sec_cnt = (data_size_bytes + (((lb_size * 2u) + 1u) / 3u) - 1u)
                      / (((lb_size * 2u) + 1u) / 3u);

#if (FS_CORE_CFG_PARTITION_EN == DEF_ENABLED)
        //                                                         FAT12 type
        partition_type = FS_PARTITION_TYPE_FAT12_CHS;
#endif

        //                                                         Calc start sec nbr's.
        rsvd_sec_start = 0u;
        fat1_sec_start = rsvd_sec_start + sys_cfg.RsvdAreaSizeInSec;
        root_dir_sec_start = fat1_sec_start + (fat_sec_cnt * sys_cfg.NbrFATs);

        data_sec_start = root_dir_sec_start + root_dir_sec_cnt;
        clus_number = DEF_MIN((partition_sec_cnt - data_sec_start) / sys_cfg.ClusSizeInSec, (fat_sec_cnt * lb_size * 2u) / 3u);

        ASSERT_BREAK_LOG(data_sec_start <= partition_sec_cnt, RTOS_ERR_VOL_FMT_INVALID,
                         ("Not enough room for data area."));

        if (clus_number > FS_FAT_MAX_NBR_CLUS_FAT12) {
          LOG_DBG(("Invalid cluster count: ", (u)clus_number));
          BREAK_ERR_SET(RTOS_ERR_VOL_FMT_INVALID);
        } else if (clus_number > (FS_FAT_MAX_NBR_CLUS_FAT12 - FS_FAT_CLUS_NBR_TOLERANCE)) {
          LOG_DBG(("Cluster number within unrecommended margin: ", (u)clus_number, ".\r\n"));
        } else {
          ;
        }

        //                                                         --------------------- FMT DISK ---------------------
        BREAK_ON_ERR(FS_CACHE_BLK_WR)(p_cache_blk_dev_data,
                                      partition_sec_start,
                                      FS_FAT_LB_TYPE_RSVD_AREA,
                                      JobSched_VoidJobHandle,
                                      &p_buf,
                                      DEF_NULL,
                                      p_err) {
          Mem_Clr((void *)p_buf, lb_size);
          FS_FAT_VolBootSecMake(p_buf,
                                &sys_cfg,
                                lb_size,
                                partition_sec_cnt,
                                fat_sec_cnt,
                                partition_sec_start);
        }

        break;
#endif

      default:
        RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
    }

#if LOG_DBG_IS_EN()
    {
      FS_FAT_SEC_SIZE sec_size;

      BREAK_ON_ERR(sec_size = FSBlkDev_LbSizeGet(blk_dev_handle, p_err));

      LOG_DBG(("FS_FAT_VolOpen(): File system found: Type     : ", (s)FS_FAT_TYPE_STR_GET(sys_cfg.FAT_Type)));
      LOG_DBG(("                                     Sec  size: ", (u)sec_size, " bytes"));
      LOG_DBG(("                                     Clus size: ", (u)sys_cfg.ClusSizeInSec, " sectors"));
      LOG_DBG(("                                     Vol  size: ", (u)partition_sec_cnt, " sectors"));
      LOG_DBG(("                                     # Clus   : ", (u)clus_number));
      LOG_DBG(("                                     # FATs   : ", (u)sys_cfg.NbrFATs));
    }
#endif

    //                                                             CLR FATs
    fat_ix = 0u;
    fat_sec_cur = fat1_sec_start;
    BREAK_ON_ERR(while) (fat_ix < sys_cfg.NbrFATs) {            // Clr each FAT.
      BREAK_ON_ERR(FS_LBClr(blk_dev_handle,
                            partition_sec_start + fat_sec_cur,
                            fat_sec_cnt,
                            FS_FAT_LB_TYPE_FAT,
                            p_err));
      fat_sec_cur += fat_sec_cnt;
      fat_ix++;
    }

    fat_ix = 0u;
    fat_sec_cur = fat1_sec_start;
    BREAK_ON_ERR(while) (fat_ix < sys_cfg.NbrFATs) {            // Wr sec to each FAT.
      BREAK_ON_ERR(FS_CACHE_BLK_WR)(p_cache_blk_dev_data,
                                    partition_sec_start + fat_sec_cur,
                                    FS_FAT_LB_TYPE_RSVD_AREA,
                                    FSCache_VoidWrJobHandle,
                                    &p_buf,
                                    DEF_NULL,
                                    p_err) {
        //                                                         ----------------- WR FIRST FAT SEC -----------------
        BREAK_ON_ERR(switch) (sys_cfg.FAT_Type) {               // Set first entries of FAT.
          case FS_FAT_TYPE_FAT12:
            val = 0x00FF8F00u | FS_FAT_BPB_MEDIA_FIXED;         // See Note #8.
            MEM_VAL_SET_INT32U_LITTLE((void *)(p_buf + 0u), val);
            break;

          case FS_FAT_TYPE_FAT16:
            val = 0xFFF8FF00u | FS_FAT_BPB_MEDIA_FIXED;         // See Note #8.
            MEM_VAL_SET_INT32U_LITTLE((void *)(p_buf + 0u), val);
            break;

          case FS_FAT_TYPE_FAT32:
            val = 0x0FFFFFF8u;                                    // See Note #8.
            MEM_VAL_SET_INT32U_LITTLE((void *)(p_buf + 0u), val);
            val = 0x0FFFFF00u | FS_FAT_BPB_MEDIA_FIXED;           // See Note #8.
            MEM_VAL_SET_INT32U_LITTLE((void *)(p_buf + 4u), val);
            val = 0x0FFFFFF8u;                                    // See Note #8.
            MEM_VAL_SET_INT32U_LITTLE((void *)(p_buf + 8u), val); // Clus 2 is root dir clus.
            break;

          default:
            RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
        }
      }
      fat_ix++;
      fat_sec_cur += fat_sec_cnt;
    }

    //                                                             ------------------ WR FSINFO SEC -------------------
    if (sys_cfg.FAT_Type == FS_FAT_TYPE_FAT32) {
      BREAK_ON_ERR(FS_CACHE_BLK_WR)(p_cache_blk_dev_data,
                                    partition_sec_start + FS_FAT_DFLT_FSINFO_SEC_NBR,
                                    FS_FAT_LB_TYPE_RSVD_AREA,
                                    JobSched_VoidJobHandle,
                                    &p_buf,
                                    DEF_NULL,
                                    p_err) {
        Mem_Clr(p_buf, lb_size);
        val = FS_FAT_FSI_LEADSIG;                               // See Note #8.
        MEM_VAL_SET_INT32U_LITTLE((void *)(p_buf + FS_FAT_FSI_OFF_LEADSIG), val);
        val = FS_FAT_FSI_STRUCSIG;                              // See Note #8.
        MEM_VAL_SET_INT32U_LITTLE((void *)(p_buf + FS_FAT_FSI_OFF_STRUCSIG), val);
        val = FS_FAT_FSI_TRAILSIG;                              // See Note #8.
        MEM_VAL_SET_INT32U_LITTLE((void *)(p_buf + FS_FAT_FSI_OFF_TRAILSIG), val);
        //                                                         Only one clus (for root dir) is alloc'd.
        MEM_VAL_SET_INT32U_LITTLE((void *)(p_buf + FS_FAT_FSI_OFF_FREE_COUNT), data_size_bytes - 1u);
        //                                                         Next free clus is clus after root dir clus.
        val = FS_FAT_DFLT_ROOT_CLUS_NBR + 1u;                   // See Note #8.
        MEM_VAL_SET_INT32U_LITTLE((void *)(p_buf + FS_FAT_FSI_OFF_NXT_FREE), val);
      }
    }
    //                                                             Update the Partition FAT type
#if (FS_CORE_CFG_PARTITION_EN == DEF_ENABLED)
    {
      RTOS_ERR err_tmp = RTOS_ERR_INIT_CODE(RTOS_ERR_NONE);
      if (partition_sec_start > 0u) {
        FSPartition_Update(blk_dev_handle,
                           partition_nbr,
                           partition_type,
                           &err_tmp);                           // Err is ignored as there might be no partition tbl.
      }
    }
#endif

    //                                                             ------------------- CLR ROOT DIR -------------------
    if (sys_cfg.FAT_Type == FS_FAT_TYPE_FAT32) {
      BREAK_ON_ERR(FS_LBClr(blk_dev_handle,                     // Clr root dir clus.
                            partition_sec_start + root_dir_sec_start,
                            sys_cfg.ClusSizeInSec,
                            FS_FAT_LB_TYPE_DIRENT,
                            p_err));
    } else {
      BREAK_ON_ERR(FS_LBClr(blk_dev_handle,                     // Clr root dir.
                            partition_sec_start + root_dir_sec_start,
                            root_dir_sec_cnt,
                            FS_FAT_LB_TYPE_DIRENT,
                            p_err));
    }

    //                                                             -------------------- CACHE SYNC --------------------
    BREAK_ON_ERR(FSCache_Sync(p_cache_blk_dev_data->CachePtr,
                              blk_dev_handle,
                              p_err));
  } WITH_SCOPE_END
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           FS_FAT_VolModuleInit()
 *
 * @brief    Initialize FAT volume module.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/
void FS_FAT_VolModuleInit(RTOS_ERR *p_err)
{
  CPU_SIZE_T init_vol_cnt;
  CPU_SIZE_T max_vol_cnt;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_SET(FSCore_InitCfg.MaxFatObjCnt.VolCnt != 0u, *p_err, RTOS_ERR_INVALID_ARG,; );

    init_vol_cnt = FSCore_InitCfg.MaxFatObjCnt.VolCnt == LIB_MEM_BLK_QTY_UNLIMITED ? 1u : FSCore_InitCfg.MaxFatObjCnt.VolCnt;
    max_vol_cnt = FSCore_InitCfg.MaxFatObjCnt.VolCnt;

    BREAK_ON_ERR(Mem_DynPoolCreate("FS - FAT vol pool",
                                   &FS_FAT_VolPool,
                                   FSCore_InitCfg.MemSegPtr,
                                   sizeof(FS_FAT_VOL),
                                   sizeof(CPU_ALIGN),
                                   init_vol_cnt,
                                   max_vol_cnt,
                                   p_err));
  } WITH_SCOPE_END
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           FS_FAT_ChkBootSec()
 *
 * Description : Check boot sector & calculate FAT parameters.
 *
 * Argument(s) : p_fat_vol     Pointer to FAT volume.
 *
 *               p_temp_08     Pointer to buffer containing boot sector.
 *
 *               p_err         Error pointer.
 *
 * Return(s)   : none.
 *
 * Note(s)     : (1) (a) The first 36 bytes of the FAT boot sector are identical for FAT12/16/32 file
 *                       systems :
 *                       @verbatim
 *                       ---------------------------------------------------------------------------------------------------------
 *                       |    JUMP INSTRUCTION TO BOOT CODE*    |             OEM NAME (in ASCII): TYPICALLY "MSWINx.y"
 *                       ---------------------------------------------------------------------------------------------------------
 *                                                               |    BYTES PER SECTOR**   | SEC/CLUS***|    RSVD SEC CNT****     |
 *                       ---------------------------------------------------------------------------------------------------------
 *                       | NBR FATs^  |    ROOT ENTRY CNT^^     | TOTAL NBR of SECTORS^^^ | MEDIA^^^^  |    FAT SIZE (in SEC)#   |
 *                       ---------------------------------------------------------------------------------------------------------
 *                       |    SECTORS PER TRACK    |       NBR of HEADS      |       NBR SECTORS before START OF PARTITION       |
 *                       ---------------------------------------------------------------------------------------------------------
 *                       |             TOTAL NBR of SECTORS^^^               |
 *                       -----------------------------------------------------
 *                       *Legal forms are
 *                           boot[0] = 0xEB, boot[1] = ????, boot[2] = 0x90
 *                           &
 *                           boot[0] = 0xE9, boot[1] = ????, boot[2] = ????.
 *
 *                       **Legal values are 512, 1024, 2048 & 4096.
 *
 *                       ***Legal values are 2, 4, 8, 16, 32, 64 & 128.  However, the number of bytes per
 *                           cluster MUST be less than or equal to 32 * 1024.
 *
 *                       ****For FAT12 & FAT16, this SHOULD be 1.  In any case, this MUST NOT be 0.
 *
 *                       ^This SHOULD be 2.
 *
 *                       ^^For FAT12/16, this field contains the number of directory entries in the root
 *                           directory, typically 512.  For FAT32, this field should be 0.
 *
 *                       ^^^Either the 16-bit count (bytes 19-20) or the 32-bit count (bytes 32-35) should
 *                           be non-zero & contain the total number of sectors in the file system.  The
 *                           other should be zero.
 *
 *                       ^^^^Media type.  Legal values are 0xF0, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE
 *                           & 0xFF.  Typically, 0xF0 should be used for fixed disks & 0xF8 should be used
 *                           for removable disks.
 *
 *                       #For FAT12/16, the number of sectors in each FAT.  For FAT32, this SHOULD be 0.
 *                       @endverbatim
 *
 *                   (b) The bytes 36-61 of the FAT boot sector for FAT12/16 file systems contain the
 *                       following :
 *                       @verbatim
 *                                                                           -----------------------------------------------------
 *                                                                           | DRIVE NBR  |    0x00    |    0x29    |
 *                       ---------------------------------------------------------------------------------------------------------
 *                           VOLUME SERIAL NBR                   |            VOLUME LABEL (in ASCII)
 *                       ---------------------------------------------------------------------------------------------------------
 *                                                                                                       |
 *                       ---------------------------------------------------------------------------------------------------------
 *                               FILE SYSTEM TYPE LABEL (in ASCII)                                    |
 *                       -------------------------------------------------------------------------------
 *                       @endverbatim
 *
 *                   (c) The bytes 36-89 of the FAT boot sector for FAT32 file systems contain the
 *                       following :
 *                       @verbatim
 *                                                                           -----------------------------------------------------
 *                                                                           |               FAT SIZE (in SECTORS)               |
 *                       ---------------------------------------------------------------------------------------------------------
 *                       |         FLAGS*          |     VERSION NUMBER**    |     CLUSTER NBR of FIRST CLUSTER OF ROOT DIR      |
 *                       ---------------------------------------------------------------------------------------------------------
 *                       |   FSINFO SECTOR NBR***  |  BACKUP BOOT RECORD**** |                       0x0000
 *                       ---------------------------------------------------------------------------------------------------------
 *                                              0x0000                                               0x0000                      |
 *                       ---------------------------------------------------------------------------------------------------------
 *                       | DRIVE NBR  |    0x00    |    0x29    |                 VOLUME SERIAL NBR                 |
 *                       ---------------------------------------------------------------------------------------------------------
 *                                                               VOLUME LABEL (in ASCII)
 *                       ---------------------------------------------------------------------------------------------------------
 *                                                 |             FILE SYSTEM TYPE LABEL (in ASCII): TYPICALLY "FAT32    "
 *                       ---------------------------------------------------------------------------------------------------------
 *                                                 |
 *                       ---------------------------
 *
 *                       *If bit 7 of this is 1, then only one FAT is active; the number of this FAT is specified in bits 0-3.
 *                        Otherwise, the FAT is mirrored at runtime into all FATs.
 *
 *                       **File systems conforming to Microsoft's FAT documentation should contain 0x0000.
 *
 *                       ***Typically 1.
 *
 *                       ****Typically 6.
 *                       @endverbatim
 *
 *                   (d) For FAT12/16/32, bytes 510 & 511 of the FAT boot sector are ALWAYS 0xAA55.
 *
 *               (2) (a) The total number of sectors in the file system MAY be smaller (perhaps
 *                       considerably smaller) than the number of sectors in the disk.  However, the
 *                       total number of sectors in the file system SHOULD NEVER be larger than the
 *                       number of sectors in the disk.
 *
 *                   (b) For FAT32, the number of sectors in the root directory is ALWAYS 0, since the
 *                       root directory lies inside the data area.
 *
 *                   (c) (1) For FAT12/16, the data area begins after the predetermined root directory
 *                           (which immediately follows the FAT sectors).
 *
 *                       (2) For FAT32,    the data area begins immediately after the FAT sectors (&
 *                           includes the root directory).
 *
 *                       (3) (a) The total number of clusters in the volume is equal to the total number
 *                               of data sectors divided by the number of clusters per sector :
 *                               @verbatim
 *                                                               Number of Data Sectors
 *                                   Number of Clusters = -------------------------------
 *                                                           Number of Clusters per Sector
 *                               @endverbatim
 *                           (b) Since clusters 0 & 1 do not exist, the highest cluster number is
 *                               'Number of Clusters' + 1.
 *
 *                   (d) (1) Four areas of a FAT12/16 file system exist :
 *                           @verbatim
 *                           (a) The reserved area.            ----------------------------------------------------------------
 *                           (b) The FAT      area.            | Rsvd |   FAT 1   |   FAT 2   | Root |         Data           |
 *                           (c) The root     directory.       | Area |   Area    |   Area    | Dir  |         Area           |
 *                           (d) The data     area.            ----------------------------------------------------------------
 *                                                                       ^           ^           ^      ^
 *                                                                       |           |           |      |
 *                                                                       |           |           |      |
 *                                                   'FS_FAT_DATA.FAT1_Start'       |           |      |
 *                                                                                   |           |      |
 *                                                               'FS_FAT_DATA.FAT2_Start'       |   'FS_FAT_DATA.DataAreaStart'
 *                                                                                               |          @ Cluster #2
 *                                                                       'FS_FAT_DATA.RootDirStart'
 *                           @endverbatim
 *
 *                       (2) Three areas of a FAT32 file system exist :
 *                           @verbatim
 *                           (a) The reserved area.            ----------------------------------------------------------------
 *                           (b) The FAT      area.            | Rsvd |   FAT 1   |   FAT 2   |            Data               |
 *                           (c) The data     area.            | Area |   Area    |   Area    |            Area               |
 *                                                               ----------------------------------------------------------------
 *                                                                       ^           ^           ^
 *                                                                       |           |           |
 *                                                                       |           |           |
 *                                                   'FS_FAT_DATA.FAT1_Start'       |        'FS_FAT_DATA.DataAreaStart'
 *                                                                                   |               @ Cluster #2
 *                                                               'FS_FAT_DATA.FAT2_Start'
 *                           @endverbatim
 *                           Unlike FAT12/16, the root directory is in cluster(s) within the data area.
 *
 *                       (3) Up to three sectors of the reserved area may be used :
 *
 *                           (1) THE BOOT SECTOR.  This sector, the sector 0 of the volume, contains
 *                               information about the format, size & layout of the file system.
 *
 *                           (2) THE BACKUP BOOT SECTOR.  This sector, typically sector 1 of the volume,
 *                               contains a backup copy of the boot sector.  The backup boot sector is
 *                               NOT used on FAT12/16 volumes.
 *
 *                           (3) THE FSINFO SECTOR.  This sector, typically sector 6 of the volume, may
 *                               be used to help the file system suite more quickly access the volume.
 *                               The FSINFO sector is NOT used on FAT12/16 volumes.
 *
 *               (3) The 'ClusSize_octet' value is stored temporarily in a 32-bit variable to protect
 *                   against 16-bit overflow.  However, according to Microsoft's FAT specification, all
 *                   legitimate values fit within the range of 16-bit unsigned integers.
 *******************************************************************************************************/
static void FS_FAT_VolBootSecChk(FS_FAT_VOL *p_fat_vol,
                                 CPU_INT08U *p_temp_08,
                                 RTOS_ERR   *p_err)
{
  CPU_INT08U      lb_size_log2;
  FS_FAT_SEC_NBR  rsvd_size;
  FS_FAT_SEC_NBR  vol_size;
  FS_LB_SIZE      sec_size;
  FS_LB_NBR       clus_size_sec;
  FS_LB_SIZE      clus_size_octet;
  FS_FAT_CLUS_NBR clus_cnt;
  FS_FAT_CLUS_NBR clus_cnt_max_fat;
  FS_FAT_SEC_NBR  fat_size;
  CPU_INT16U      root_cnt;
  CPU_INT08U      fats_cnt;
#if (FS_FAT_CFG_FAT32_EN == DEF_ENABLED)
  FS_FAT_CLUS_NBR clus_nbr = 0u;
#endif

  WITH_SCOPE_BEGIN(p_err) {
    BREAK_ON_ERR(lb_size_log2 = FSBlkDev_LbSizeLog2Get(p_fat_vol->Vol.BlkDevHandle, p_err));

    sec_size = (FS_LB_SIZE)MEM_VAL_GET_INT16U_LITTLE((void *)(p_temp_08 + FS_FAT_BPB_OFF_BYTSPERSEC));

    ASSERT_BREAK(sec_size == FS_UTIL_PWR2(lb_size_log2), RTOS_ERR_VOL_FMT_INVALID);

    clus_size_sec = (FS_FAT_SEC_NBR)MEM_VAL_GET_INT08U_LITTLE((void *)(p_temp_08 + FS_FAT_BPB_OFF_SECPERCLUS));
    BREAK_ON_ERR(switch) (clus_size_sec) {
      case 1u:
      case 2u:
      case 4u:
      case 8u:
      case 16u:
      case 32u:
      case 64u:
      case 128u:
        p_fat_vol->SecPerClusLog2 = FSUtil_Log2((CPU_INT32U)clus_size_sec);
        break;

      default:
        LOG_ERR(("Invalid secs/clus: ", (u)FS_UTIL_PWR2(p_fat_vol->SecPerClusLog2), "."));
        BREAK_ERR_SET(RTOS_ERR_VOL_FMT_INVALID);
    }

    //                                                             See Note #3.
    clus_size_octet = FS_UTIL_PWR2(lb_size_log2 + p_fat_vol->SecPerClusLog2);
    BREAK_ON_ERR(switch) (clus_size_octet) {
      case 512u:
      case 1024u:
      case 2048u:
      case 4096u:
      case 8192u:
      case 16384u:
      case 32768u:
      case 65536u:
        break;

      default:
        LOG_ERR(("Invalid bytes/clus: ", (u)clus_size_octet, "."));
        BREAK_ERR_SET(RTOS_ERR_VOL_FMT_INVALID);
    }

    fats_cnt = MEM_VAL_GET_INT08U_LITTLE((void *)(p_temp_08 + FS_FAT_BPB_OFF_NUMFATS));
    BREAK_ON_ERR(switch) (fats_cnt) {
      case 1:
      case 2:
        break;

      default:
        LOG_ERR(("Invalid nbr FATs: ", (u)fats_cnt, "."));
        BREAK_ERR_SET(RTOS_ERR_VOL_FMT_INVALID);
    }
    //                                                             Size of reserved area.
    rsvd_size = (FS_FAT_SEC_NBR)MEM_VAL_GET_INT16U_LITTLE((void *)(p_temp_08 + FS_FAT_BPB_OFF_RSVDSECCNT));
    p_fat_vol->FAT1_Start = rsvd_size;                          // Sec nbr of 1st FAT.

    //                                                             Size of each FAT.
    fat_size = (FS_FAT_SEC_NBR)MEM_VAL_GET_INT16U_LITTLE((void *)(p_temp_08 + FS_FAT_BPB_OFF_FATSZ16));
    if (fat_size == 0u) {
      fat_size = (FS_FAT_SEC_NBR)MEM_VAL_GET_INT32U_LITTLE((void *)(p_temp_08 + FS_FAT_BPB_FAT32_OFF_FATSZ32));
    }

    //                                                             Size of root dir (see Notes #2b).
    root_cnt = MEM_VAL_GET_INT16U_LITTLE((void *)(p_temp_08 + FS_FAT_BPB_OFF_ROOTENTCNT));
    p_fat_vol->RootDirSize = FS_UTIL_DIV_PWR2((((FS_FAT_SEC_NBR)root_cnt * 32u) + ((FS_FAT_SEC_NBR)sec_size - 1u)), lb_size_log2);

    //                                                             Sec nbr's of 2nd FAT, root dir, data.
    if (fats_cnt == 2u) {
      p_fat_vol->RootDirStart = rsvd_size + (fat_size * 2u);
      p_fat_vol->DataAreaStart = rsvd_size + (fat_size * 2u) + p_fat_vol->RootDirSize;
    } else {
      p_fat_vol->RootDirStart = rsvd_size + (fat_size * 1u);
      p_fat_vol->DataAreaStart = rsvd_size + (fat_size * 1u) + p_fat_vol->RootDirSize;
    }

    //                                                             Size of vol (see Notes #2a).
    vol_size = (FS_FAT_SEC_NBR)MEM_VAL_GET_INT16U_LITTLE((void *)(p_temp_08 + FS_FAT_BPB_OFF_TOTSEC16));
    if (vol_size == 0u) {
      vol_size = MEM_VAL_GET_INT32U_LITTLE((void *)(p_temp_08 + FS_FAT_BPB_OFF_TOTSEC32));
    }

    //                                                             Size of data area (see Notes #2c).
    p_fat_vol->DataAreaSize = (vol_size - (rsvd_size + ((FS_FAT_SEC_NBR)fats_cnt * fat_size))) - p_fat_vol->RootDirSize;
    p_fat_vol->NextClusNbr = 2u;                                // Next clus to alloc.
                                                                // See Notes #2c3.
    clus_cnt = (FS_FAT_CLUS_NBR)FS_UTIL_DIV_PWR2(p_fat_vol->DataAreaSize, p_fat_vol->SecPerClusLog2);

    //                                                             -------------------- CHK FAT12 ---------------------
    if (clus_cnt <= FS_FAT_MAX_NBR_CLUS_FAT12) {
#if (FS_FAT_CFG_FAT12_EN == DEF_ENABLED)
      p_fat_vol->FAT_Type = FS_FAT_TYPE_FAT12;
      clus_cnt_max_fat = ((FS_FAT_CLUS_NBR)fat_size * (FS_FAT_CLUS_NBR)FS_UTIL_PWR2(lb_size_log2) * 2u) / 3u;
      p_fat_vol->ClusCnt = DEF_MIN(clus_cnt, clus_cnt_max_fat);
#else
      LOG_ERR(("Invalid file sys: FAT12."));
      BREAK_ERR_SET(RTOS_ERR_VOL_FMT_INVALID);
      return;
#endif

      //                                                           -------------------- CHK FAT16 ---------------------
    } else if (clus_cnt <= FS_FAT_MAX_NBR_CLUS_FAT16) {
#if (FS_FAT_CFG_FAT16_EN == DEF_ENABLED)
      p_fat_vol->FAT_Type = FS_FAT_TYPE_FAT16;
      clus_cnt_max_fat = (FS_FAT_CLUS_NBR)fat_size * ((FS_FAT_CLUS_NBR)FS_UTIL_PWR2(lb_size_log2) / FS_FAT_FAT16_ENTRY_NBR_OCTETS);
      p_fat_vol->ClusCnt = DEF_MIN(clus_cnt, clus_cnt_max_fat);
#else
      LOG_ERR(("Invalid file sys: FAT16."));
      BREAK_ERR_SET(RTOS_ERR_VOL_FMT_INVALID);
#endif

      //                                                           -------------------- CHK FAT32 ---------------------
    } else {
#if (FS_FAT_CFG_FAT32_EN == DEF_ENABLED)
      p_fat_vol->FAT_Type = FS_FAT_TYPE_FAT32;
      clus_cnt_max_fat = (FS_FAT_CLUS_NBR)fat_size * ((FS_FAT_CLUS_NBR)FS_UTIL_PWR2(lb_size_log2) / FS_FAT_FAT32_ENTRY_NBR_OCTETS);
      p_fat_vol->ClusCnt = DEF_MIN(clus_cnt, clus_cnt_max_fat);

      //                                                           Get root dir clus nbr.
      clus_nbr = MEM_VAL_GET_INT32U_LITTLE((void *)(p_temp_08 + FS_FAT_BPB_FAT32_OFF_ROOTCLUS));
      if (FS_FAT_IS_VALID_CLUS(p_fat_vol, clus_nbr) == DEF_NO) {
        LOG_ERR(("Invalid FAT32 root dir clus: ", (u)clus_nbr));
        BREAK_ERR_SET(RTOS_ERR_VOL_FMT_INVALID);
      }
      p_fat_vol->RootDirStart = FS_FAT_CLUS_TO_SEC(p_fat_vol, clus_nbr);
#else
      LOG_ERR(("Invalid file sys: FAT32."));
      BREAK_ERR_SET(RTOS_ERR_VOL_FMT_INVALID);
      return;
#endif
    }

#if (FS_CORE_CFG_CTR_STAT_EN == DEF_ENABLED)
    //                                                             Keep common FAT12/16/32 BPB fields for stats.
    p_fat_vol->BootSec.BPB_BytsPerSec = sec_size;
    p_fat_vol->BootSec.BPB_SecPerClus = clus_size_sec;
    p_fat_vol->BootSec.BPB_RsvdSecCnt = rsvd_size;
    p_fat_vol->BootSec.BPB_NumFATs = fats_cnt;
    p_fat_vol->BootSec.BPB_RootEntCnt = root_cnt;
    p_fat_vol->BootSec.BPB_TotSec16 = MEM_VAL_GET_INT16U_LITTLE((void *)(p_temp_08 + FS_FAT_BPB_OFF_TOTSEC16));
    p_fat_vol->BootSec.BPB_Media = MEM_VAL_GET_INT08U_LITTLE((void *)(p_temp_08 + FS_FAT_BPB_OFF_MEDIA));
    p_fat_vol->BootSec.BPB_FATSz16 = MEM_VAL_GET_INT16U_LITTLE((void *)(p_temp_08 + FS_FAT_BPB_OFF_FATSZ16));
    p_fat_vol->BootSec.BPB_TotSec32 = MEM_VAL_GET_INT32U_LITTLE((void *)(p_temp_08 + FS_FAT_BPB_OFF_TOTSEC32));
    if (clus_cnt > FS_FAT_MAX_NBR_CLUS_FAT16) {
      p_fat_vol->BootSec.BPB_ExtFat32.BPB_FATSz32 = MEM_VAL_GET_INT32U_LITTLE((void *)(p_temp_08 + FS_FAT_BPB_FAT32_OFF_FATSZ32));;
#if (FS_FAT_CFG_FAT32_EN == DEF_ENABLED)
      p_fat_vol->BootSec.BPB_ExtFat32.BPB_RootClus = clus_nbr;
#endif
      p_fat_vol->BootSec.BPB_ExtFat32.BPB_FSInfo = MEM_VAL_GET_INT16U_LITTLE((void *)(p_temp_08 + FS_FAT_BPB_FAT32_OFF_FSINFO));
    }
#endif
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                           FS_FAT_GetSysCfg()
 *
 * @brief    Get format configuration.
 *
 * @param    sec_size    Sector size, in octets.
 *
 * @param    size        Size of device, in sectors.
 *
 * @param    p_sys_cfg   Pointer to FAT configuration that will receive configuration parameters.
 *
 * @param    p_err       Error pointer.
 *
 * @note     (1) $$$$ Verify information in FAT tables.
 *
 * @note     (2) See 'FS_FAT_VolFmt() Note #2'.
 *
 * @note     (3) A device with a large number of sectors & a large sector size may not be FAT-
 *               formattable.  In this case, an invalid bytes-per-cluster value will be computed.  If
 *               an invalid bytes-per-cluster value is computed, then an error is returned, since this
 *               may signal an error in a hardware driver.  If these are legitimate parameters,
 *               then the volume should be formatted with a reduced number of data sectors.
 *
 *           - (a) $$$$ Allow user to specify reduced number of data sectors.
 *
 * @note     (4) A valid FAT table entry should always be found; however, the sectors per cluster
 *               value is checked in case the code or structures are incorrectly modified.
 *******************************************************************************************************/
static void FS_FAT_GetSysCfg(FS_FAT_SEC_SIZE sec_size,
                             FS_FAT_SEC_NBR  sec_cnt,
                             FS_FAT_VOL_CFG  *p_sys_cfg,
                             RTOS_ERR        *p_err)
{
  const FS_FAT_TBL_ENTRY *p_tbl_entry;
  CPU_SIZE_T             clus_size_octet;
  CPU_INT64U             size_octets;
  CPU_BOOLEAN            found;

  size_octets = sec_cnt * sec_size;
  found = DEF_NO;

  //                                                               ------------------- CHK IF FAT32 -------------------
#if (FS_FAT_CFG_FAT32_EN == DEF_ENABLED)                        // This CAN be FAT32 ... get clus size.
  if (size_octets > FS_FAT_MAX_SIZE_FAT16) {
    p_tbl_entry = &FS_FAT_TblFAT32[0];
    while ((sec_cnt > p_tbl_entry->VolSecCnt) && (p_tbl_entry->VolSecCnt != 0u)) {
      p_tbl_entry++;
    }
    if (p_tbl_entry->ClusSize != 0u) {                          // Rtn err if sec per clus zero (see Note #4).
      p_sys_cfg->FAT_Type = FS_FAT_TYPE_FAT32;
      p_sys_cfg->RootDirEntryCnt = FS_FAT_DFLT_ROOT_ENT_CNT_FAT32;
      p_sys_cfg->RsvdAreaSizeInSec = FS_FAT_DFLT_RSVD_SEC_CNT_FAT32;
      p_sys_cfg->ClusSizeInSec = p_tbl_entry->ClusSize;
      p_sys_cfg->NbrFATs = 2u;
      found = DEF_YES;
    }
  }
#endif

  //                                                               ------------------- CHK IF FAT16 -------------------
#if (FS_FAT_CFG_FAT16_EN == DEF_ENABLED)                        // This CAN be FAT16 ... get clus size.
  if (!found && (size_octets > FS_FAT_MAX_SIZE_FAT12)) {
    p_tbl_entry = &FS_FAT_TblFAT16[0];
    while ((sec_cnt > p_tbl_entry->VolSecCnt) && (p_tbl_entry->VolSecCnt != 0u)) {
      p_tbl_entry++;
    }
    if (p_tbl_entry->ClusSize != 0u) {                          // Rtn err if sec per clus zero (see Note #4).
      p_sys_cfg->FAT_Type = FS_FAT_TYPE_FAT16;
      p_sys_cfg->RootDirEntryCnt = FS_FAT_DFLT_ROOT_ENT_CNT_FAT16;
      p_sys_cfg->RsvdAreaSizeInSec = FS_FAT_DFLT_RSVD_SEC_CNT_FAT16;
      p_sys_cfg->ClusSizeInSec = p_tbl_entry->ClusSize;
      p_sys_cfg->NbrFATs = 2u;
      found = DEF_YES;
    }
  }
#endif

#if (FS_FAT_CFG_FAT12_EN == DEF_ENABLED)                        // ------------------- CHK IF FAT12 -------------------
  if (!found) {
    p_tbl_entry = &FS_FAT_TblFAT12[0];
    while ((sec_cnt > p_tbl_entry->VolSecCnt) && (p_tbl_entry->VolSecCnt != 0u)) {
      p_tbl_entry++;
    }
    if (p_tbl_entry->ClusSize != 0u) {                          // Rtn err if sec per clus zero (see Note #4).
      p_sys_cfg->FAT_Type = FS_FAT_TYPE_FAT12;
      p_sys_cfg->RootDirEntryCnt = FS_FAT_DFLT_ROOT_ENT_CNT_FAT12;
      p_sys_cfg->RsvdAreaSizeInSec = FS_FAT_DFLT_RSVD_SEC_CNT_FAT12;
      p_sys_cfg->ClusSizeInSec = p_tbl_entry->ClusSize;
      p_sys_cfg->NbrFATs = 2u;
      found = DEF_YES;
    }
  }
#endif

  if (!found) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_SIZE_INVALID);
    return;
  }

  //                                                               ------------------- CHK DISK CFG -------------------
  clus_size_octet = p_sys_cfg->ClusSizeInSec * sec_size;        // Verify bytes per clus (see Note #3).
  switch (clus_size_octet) {
    case 512u:
    case 1024u:
    case 2048u:
    case 4096u:
    case 8192u:
    case 16384u:
    case 32768u:
      break;
    default:
      LOG_ERR(("Invalid bytes/clus: ", (u)p_sys_cfg->ClusSizeInSec * sec_size, "."));
      RTOS_ERR_SET(*p_err, RTOS_ERR_SIZE_INVALID);
      return;
  }

  PP_UNUSED_PARAM(size_octets);
}

/****************************************************************************************************//**
 *                                           FS_FAT_MakeBootSec()
 *
 * @brief    Make boot sector.
 *
 * @param    p_temp              Pointer to temporary buffer.
 *
 * @param    p_sys_cfg           Pointer to format configuration information.
 *
 * @param    sec_size            Sector size, in octets.
 *
 * @param    size                Size of volume in sectors.
 *
 * @param    fat_size            Size of one FAT in sectors.
 *
 * @param    partition_start     partition start: number of sectors from the MBR
 *
 * @note     (1) See 'FS_FAT_VolFmt() Note #6'.
 *
 * @note     (2) See 'FS_FAT_VolFmt() Note #7'.
 *
 * @note     (3) Avoid 'Excessive shift value' or 'Constant expression evaluates to 0' warning.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_FAT_VolBootSecMake(void           *p_temp,
                                  FS_FAT_VOL_CFG *p_sys_cfg,
                                  FS_LB_SIZE     sec_size,
                                  FS_FAT_SEC_NBR size,
                                  FS_FAT_SEC_NBR fat_size,
                                  FS_LB_NBR      partition_start)
{
  CPU_INT08U *p_temp_08;
  CPU_INT16U sec_per_trk;
  CPU_INT16U num_heads;
  CPU_INT32U nbr_32;
  CPU_INT16U nbr_16;

  p_temp_08 = (CPU_INT08U *)p_temp;

  //                                                               Boot sector & BPB structure (see Note #1).
  MEM_VAL_SET_INT08U_LITTLE((void *)&p_temp_08[FS_FAT_BS_OFF_JMPBOOT + 0u], FS_FAT_BS_JMPBOOT_0);
  MEM_VAL_SET_INT08U_LITTLE((void *)&p_temp_08[FS_FAT_BS_OFF_JMPBOOT + 1u], FS_FAT_BS_JMPBOOT_1);
  MEM_VAL_SET_INT08U_LITTLE((void *)&p_temp_08[FS_FAT_BS_OFF_JMPBOOT + 2u], FS_FAT_BS_JMPBOOT_2);

  Mem_Copy((void *)&p_temp_08[FS_FAT_BS_OFF_OEMNAME], (void *)FS_FAT_BS_OEMNAME, (CPU_SIZE_T)8);

  MEM_VAL_SET_INT16U_LITTLE((void *)&p_temp_08[FS_FAT_BPB_OFF_BYTSPERSEC], sec_size);
  MEM_VAL_SET_INT08U_LITTLE((void *)&p_temp_08[FS_FAT_BPB_OFF_SECPERCLUS], p_sys_cfg->ClusSizeInSec);
  MEM_VAL_SET_INT16U_LITTLE((void *)&p_temp_08[FS_FAT_BPB_OFF_RSVDSECCNT], p_sys_cfg->RsvdAreaSizeInSec);
  MEM_VAL_SET_INT08U_LITTLE((void *)&p_temp_08[FS_FAT_BPB_OFF_NUMFATS], p_sys_cfg->NbrFATs);

  if (p_sys_cfg->FAT_Type == FS_FAT_TYPE_FAT32) {
    MEM_VAL_SET_INT16U_LITTLE((void *)&p_temp_08[FS_FAT_BPB_OFF_ROOTENTCNT], 0);
    MEM_VAL_SET_INT16U_LITTLE((void *)&p_temp_08[FS_FAT_BPB_OFF_TOTSEC16], 0);
    MEM_VAL_SET_INT16U_LITTLE((void *)&p_temp_08[FS_FAT_BPB_OFF_FATSZ16], 0);
    MEM_VAL_SET_INT32U_LITTLE((void *)&p_temp_08[FS_FAT_BPB_OFF_TOTSEC32], size);
  } else {
    MEM_VAL_SET_INT16U_LITTLE((void *)&p_temp_08[FS_FAT_BPB_OFF_ROOTENTCNT], p_sys_cfg->RootDirEntryCnt);
    if (size <= DEF_INT_16U_MAX_VAL) {
      MEM_VAL_SET_INT16U_LITTLE((void *)&p_temp_08[FS_FAT_BPB_OFF_TOTSEC16], size);
    }
    MEM_VAL_SET_INT16U_LITTLE((void *)&p_temp_08[FS_FAT_BPB_OFF_FATSZ16], fat_size);
    if (size > DEF_INT_16U_MAX_VAL) {
      MEM_VAL_SET_INT32U_LITTLE((void *)&p_temp_08[FS_FAT_BPB_OFF_TOTSEC32], size);
    }
  }

  MEM_VAL_SET_INT08U_LITTLE((void *)&p_temp_08[FS_FAT_BPB_OFF_MEDIA], FS_FAT_BPB_MEDIA_FIXED);
  sec_per_trk = 0x3Fu;                                          // See Note #3.
  MEM_VAL_SET_INT16U_LITTLE((void *)&p_temp_08[FS_FAT_BPB_OFF_SECPERTRK], sec_per_trk);
  num_heads = 0xFFu;                                            // See Note #3.
  MEM_VAL_SET_INT16U_LITTLE((void *)&p_temp_08[FS_FAT_BPB_OFF_NUMHEADS], num_heads);
  MEM_VAL_SET_INT32U_LITTLE((void *)&p_temp_08[FS_FAT_BPB_OFF_HIDDSEC], partition_start);

  if (p_sys_cfg->FAT_Type == FS_FAT_TYPE_FAT32) {               // FAT32 struct start at offset 36 (see Note #2).
    MEM_VAL_SET_INT32U_LITTLE((void *)&p_temp_08[FS_FAT_BPB_FAT32_OFF_FATSZ32], fat_size);
    MEM_VAL_SET_INT16U_LITTLE((void *)&p_temp_08[FS_FAT_BPB_FAT32_OFF_EXTFLAGS], 0);
    MEM_VAL_SET_INT16U_LITTLE((void *)&p_temp_08[FS_FAT_BPB_FAT32_OFF_FSVER], 0);
    nbr_32 = FS_FAT_DFLT_ROOT_CLUS_NBR;                         // See Note #3.
    MEM_VAL_SET_INT32U_LITTLE((void *)&p_temp_08[FS_FAT_BPB_FAT32_OFF_ROOTCLUS], nbr_32);
    nbr_16 = FS_FAT_DFLT_FSINFO_SEC_NBR;                        // See Note #3.
    MEM_VAL_SET_INT16U_LITTLE((void *)&p_temp_08[FS_FAT_BPB_FAT32_OFF_FSINFO], nbr_16);
    nbr_16 = FS_FAT_DFLT_BKBOOTSEC_SEC_NBR;                     // See Note #3.
    MEM_VAL_SET_INT16U_LITTLE((void *)&p_temp_08[FS_FAT_BPB_FAT32_OFF_BKBOOTSEC], nbr_16);
    MEM_VAL_SET_INT08U_LITTLE((void *)&p_temp_08[FS_FAT_BS_FAT32_OFF_DRVNUM], 0);
    MEM_VAL_SET_INT08U_LITTLE((void *)&p_temp_08[FS_FAT_BS_FAT32_OFF_RESERVED1], 0);
    MEM_VAL_SET_INT08U_LITTLE((void *)&p_temp_08[FS_FAT_BS_FAT32_OFF_BOOTSIG], FS_FAT_BS_BOOTSIG);
    MEM_VAL_SET_INT32U_LITTLE((void *)&p_temp_08[FS_FAT_BS_FAT32_OFF_VOLID], 0x12345678);

    Mem_Copy((void *)&p_temp_08[FS_FAT_BS_FAT32_OFF_VOLLAB], (void *)FS_FAT_BS_VOLLAB, (CPU_SIZE_T)11);
    Mem_Copy((void *)&p_temp_08[FS_FAT_BS_FAT32_OFF_FILSYSTYPE], (void *)FS_FAT_BS_FAT32_FILESYSTYPE, (CPU_SIZE_T) 8);
  } else {                                                      // FAT12/16 struct start at offset 36 (see Note #2).
    MEM_VAL_SET_INT08U_LITTLE((void *)&p_temp_08[FS_FAT_BS_FAT1216_OFF_DRVNUM], 0);
    MEM_VAL_SET_INT08U_LITTLE((void *)&p_temp_08[FS_FAT_BS_FAT1216_OFF_RESERVED1], 0);
    MEM_VAL_SET_INT08U_LITTLE((void *)&p_temp_08[FS_FAT_BS_FAT1216_OFF_BOOTSIG], FS_FAT_BS_BOOTSIG);
    MEM_VAL_SET_INT32U_LITTLE((void *)&p_temp_08[FS_FAT_BS_FAT1216_OFF_VOLID], 0x12345678u);

    Mem_Copy((void *)&p_temp_08[FS_FAT_BS_FAT1216_OFF_VOLLAB], (void *)FS_FAT_BS_VOLLAB, (CPU_SIZE_T)11);
    if (p_sys_cfg->FAT_Type == FS_FAT_TYPE_FAT12) {
      Mem_Copy((void *)&p_temp_08[FS_FAT_BS_FAT1216_OFF_FILSYSTYPE], (void *)FS_FAT_BS_FAT12_FILESYSTYPE, (CPU_SIZE_T) 8);
    } else if (p_sys_cfg->FAT_Type == FS_FAT_TYPE_FAT16) {
      Mem_Copy((void *)&p_temp_08[FS_FAT_BS_FAT1216_OFF_FILSYSTYPE], (void *)FS_FAT_BS_FAT16_FILESYSTYPE, (CPU_SIZE_T) 8);
    } else {
      LOG_ERR(("Cfg'd fat type is invalid."));
      CPU_SW_EXCEPTION(; );
    }
  }

  //                                                               Boot sector signature.
  MEM_VAL_SET_INT08U_LITTLE((void *)&p_temp_08[FS_FAT_BOOT_SIG_LO_OFF], FS_FAT_BOOT_SIG_LO);
  MEM_VAL_SET_INT08U_LITTLE((void *)&p_temp_08[FS_FAT_BOOT_SIG_HI_OFF], FS_FAT_BOOT_SIG_HI);
}
#endif

/****************************************************************************************************//**
 *                                               FS_LBClr()
 *
 * @brief    Clear a given number of consecutive logical blocks.
 *
 * @param    blk_dev_handle  Block device handle.
 *
 * @param    start           Start logical block.
 *
 * @param    cnt             Logical block count.
 *
 * @param    lb_type         Logical block type.
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/
#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_LBClr(FS_BLK_DEV_HANDLE blk_dev_handle,
                     FS_LB_NBR         start,
                     FS_LB_NBR         cnt,
                     CPU_INT08U        lb_type,
                     RTOS_ERR          *p_err)
{
  CPU_INT08U            *p_buf;
  FS_CACHE_BLK_DEV_DATA *p_cache_blk_dev_data;
  FS_LB_NBR             cur_lb_nbr;
  FS_LB_NBR             cnt_rem;
  CPU_INT08U            lb_size_log2;

  WITH_SCOPE_BEGIN(p_err) {
    p_cache_blk_dev_data = FSCache_BlkDevDataGet(blk_dev_handle);

    BREAK_ON_ERR(lb_size_log2 = FSBlkDev_LbSizeLog2Get(blk_dev_handle, p_err));

    cnt_rem = cnt;
    cur_lb_nbr = start;
    BREAK_ON_ERR(while) (cnt_rem > 0u) {
      BREAK_ON_ERR(FS_CACHE_BLK_WR)(p_cache_blk_dev_data,
                                    cur_lb_nbr,
                                    lb_type,
                                    JobSched_VoidJobHandle,
                                    &p_buf,
                                    DEF_NULL,
                                    p_err) {
        Mem_Clr(p_buf, FS_UTIL_PWR2(lb_size_log2));
      }
      cnt_rem--;
      cur_lb_nbr++;
    }
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
