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
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  FS_FAT_VOL_PRIV_H_
#define  FS_FAT_VOL_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ----------------------- CORE -----------------------
#include  <fs/source/core/fs_core_vol_priv.h>
#include  <fs/source/storage/fs_blk_dev_priv.h>
#include  <fs/source/sys/fs_sys_priv.h>
#include  <fs/source/sys/fat/fs_fat_type_priv.h>

//                                                                 ----------------------- FAT ------------------------
#include  <fs/include/fs_fat.h>

//                                                                 ----------------------- EXT ------------------------
#include  <cpu/include/cpu.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  FS_FAT_MAX_VOL_LABEL_LEN                          11u

/********************************************************************************************************
 *                                       BOOT SECTOR & BPB DEFINES
 *
 * Note(s): (1) Refer to 'Section 3: Boot Sector and BPB' of "Microsoft FAT Specification, August 30 2005"
 *              for more details about the content of the boot sector and the BIOS Parameter Block (BPB).
 *******************************************************************************************************/

#define  FS_FAT_BS_OFF_JMPBOOT                             0u
#define  FS_FAT_BS_OFF_OEMNAME                             3u
#define  FS_FAT_BPB_OFF_BYTSPERSEC                        11u
#define  FS_FAT_BPB_OFF_SECPERCLUS                        13u
#define  FS_FAT_BPB_OFF_RSVDSECCNT                        14u
#define  FS_FAT_BPB_OFF_NUMFATS                           16u
#define  FS_FAT_BPB_OFF_ROOTENTCNT                        17u
#define  FS_FAT_BPB_OFF_TOTSEC16                          19u
#define  FS_FAT_BPB_OFF_MEDIA                             21u
#define  FS_FAT_BPB_OFF_FATSZ16                           22u
#define  FS_FAT_BPB_OFF_SECPERTRK                         24u
#define  FS_FAT_BPB_OFF_NUMHEADS                          26u
#define  FS_FAT_BPB_OFF_HIDDSEC                           28u
#define  FS_FAT_BPB_OFF_TOTSEC32                          32u

#define  FS_FAT_BS_FAT1216_OFF_DRVNUM                     36u
#define  FS_FAT_BS_FAT1216_OFF_RESERVED1                  37u
#define  FS_FAT_BS_FAT1216_OFF_BOOTSIG                    38u
#define  FS_FAT_BS_FAT1216_OFF_VOLID                      39u
#define  FS_FAT_BS_FAT1216_OFF_VOLLAB                     43u
#define  FS_FAT_BS_FAT1216_OFF_FILSYSTYPE                 54u

#define  FS_FAT_BPB_FAT32_OFF_FATSZ32                     36u
#define  FS_FAT_BPB_FAT32_OFF_EXTFLAGS                    40u
#define  FS_FAT_BPB_FAT32_OFF_FSVER                       42u
#define  FS_FAT_BPB_FAT32_OFF_ROOTCLUS                    44u
#define  FS_FAT_BPB_FAT32_OFF_FSINFO                      48u
#define  FS_FAT_BPB_FAT32_OFF_BKBOOTSEC                   50u
#define  FS_FAT_BPB_FAT32_OFF_RESERVED                    52u
#define  FS_FAT_BS_FAT32_OFF_DRVNUM                       64u
#define  FS_FAT_BS_FAT32_OFF_RESERVED1                    65u
#define  FS_FAT_BS_FAT32_OFF_BOOTSIG                      66u
#define  FS_FAT_BS_FAT32_OFF_VOLID                        67u
#define  FS_FAT_BS_FAT32_OFF_VOLLAB                       71u
#define  FS_FAT_BS_FAT32_OFF_FILSYSTYPE                   82u

#define  FS_FAT_BS_FAT12_FILESYSTYPE                      "FAT12   "
#define  FS_FAT_BS_FAT16_FILESYSTYPE                      "FAT16   "
#define  FS_FAT_BS_FAT32_FILESYSTYPE                      "FAT32   "

#define  FS_FAT_BS_JMPBOOT_0                              0xEBu
#define  FS_FAT_BS_JMPBOOT_1                              0x58u
#define  FS_FAT_BS_JMPBOOT_2                              0x90u
#define  FS_FAT_BS_OEMNAME                                "MSWIN4.1"

#define  FS_FAT_BPB_MEDIA_FIXED                           0xF8u
#define  FS_FAT_BPB_MEDIA_REMOVABLE                       0xF0u

#define  FS_FAT_BS_BOOTSIG                                0x29u

#define  FS_FAT_BS_VOLLAB                                 "NO NAME    "

#define  FS_FAT_BOOT_SIG                                  0xAA55u
#define  FS_FAT_BOOT_SIG_LO                               0x55u
#define  FS_FAT_BOOT_SIG_HI                               0xAAu

#define  FS_FAT_BOOT_SIG_LO_OFF                           510u
#define  FS_FAT_BOOT_SIG_HI_OFF                           511u

/********************************************************************************************************
 *                                           DEFAULT VALUES
 *******************************************************************************************************/

#define  FS_FAT_DFLT_ROOT_CLUS_NBR                         2u
#define  FS_FAT_DFLT_FSINFO_SEC_NBR                        1u
#define  FS_FAT_DFLT_BKBOOTSEC_SEC_NBR                     6u

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                        JOURNAL DATA TYPES
 *******************************************************************************************************/

typedef struct fs_fat_journal_file {
  FS_FAT_CLUS_NBR     FirstClus;
  FS_FAT_SEC_BYTE_POS EntryStartPos;
  FS_FAT_FILE_SIZE    CurPos;
  FS_FAT_SEC_BYTE_POS CurSecBytePos;
} FS_FAT_JOURNAL_FILE;

typedef struct fs_fat_journal_data {
  FS_FAT_JOURNAL_FILE    File;
#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
  FS_CACHE_WR_JOB_HANDLE JnlWrJobHandle;
#endif
  CPU_INT32U             InitDone : 1;
  CPU_INT32U             Replaying : 1;
} FS_FAT_JOURNAL_DATA;

/********************************************************************************************************
 *                                       BOOT SECTOR DATA TYPE
 *
 * @note    (1) The structure FS_FAT_VOL_BOOT_SEC contains information about the BIOS Parameter Block
 *              (BPB) located in the boot sector of a FAT volume. The fields are common to FAT12/16/32.
 *              See section 'Section 3: Boot Sector and BPB' of "Microsoft FAT Specification, August 30
 *              2005" for more details about BPB fields.
 *              FS_FAT_VOL_BOOT_SEC contains only a few fields of BPB, defined by Microsoft FAT
 *              specification, relevant for statistics and debugging purposes.
 *******************************************************************************************************/

#if (FS_CORE_CFG_CTR_STAT_EN == DEF_ENABLED)
typedef struct fs_fat_vol_ext_bpb_fat32 {
  CPU_INT32U BPB_FATSz32;
  CPU_INT32U BPB_RootClus;
  CPU_INT16U BPB_FSInfo;
} FS_FAT_VOL_EXT_BPB_FAT32;

typedef struct fs_fat_vol_boot_sec {
  CPU_INT16U               BPB_BytsPerSec;
  CPU_INT08U               BPB_SecPerClus;
  CPU_INT16U               BPB_RsvdSecCnt;
  CPU_INT08U               BPB_NumFATs;
  CPU_INT16U               BPB_RootEntCnt;
  CPU_INT16U               BPB_TotSec16;
  CPU_INT08U               BPB_Media;
  CPU_INT16U               BPB_FATSz16;
  CPU_INT32U               BPB_TotSec32;
  FS_FAT_VOL_EXT_BPB_FAT32 BPB_ExtFat32;
} FS_FAT_VOL_BOOT_SEC;
#endif

/********************************************************************************************************
 *                                        FAT VOLUME DATA TYPE
 *******************************************************************************************************/

typedef struct fs_fat_vol {
  FS_VOL                 Vol;
  FS_FAT_SEC_NBR         RootDirSize;
  FS_FAT_SEC_NBR         DataAreaSize;
  FS_FAT_CLUS_NBR        ClusCnt;
  FS_FAT_SEC_NBR         FAT1_Start;
  FS_FAT_SEC_NBR         RootDirStart;
  FS_FAT_SEC_NBR         DataAreaStart;
  FS_FAT_CLUS_NBR        NextClusNbr;
  CPU_INT08U             SecPerClusLog2;
  CPU_INT08U             FAT_Type;
  FS_FAT_CLUS_NBR        QueryBadClusCnt;
  FS_FAT_CLUS_NBR        QueryFreeClusCnt;
  CPU_DATA               QueryInfoValid : 1;
  CPU_DATA               IsJournaled : 1;
#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
  FS_CACHE_WR_JOB_HANDLE FatWrJobHandle;
#endif
#if (FS_CORE_CFG_CTR_STAT_EN == DEF_ENABLED)
  FS_CTR                 StatAllocClusCtr;
  FS_CTR                 StatFreeClusCtr;
  FS_FAT_VOL_BOOT_SEC    BootSec;
#endif
#if (FS_FAT_CFG_JOURNAL_EN == DEF_ENABLED)
  FS_FAT_JOURNAL_DATA    JournalData;
#endif
} FS_FAT_VOL;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void FS_FAT_VolModuleInit(RTOS_ERR *p_err);

FS_VOL *FS_FAT_VolAlloc(RTOS_ERR *p_err);

void FS_FAT_VolFree(FS_VOL *p_vol);

void FS_FAT_VolSync(FS_VOL   *p_vol,
                    RTOS_ERR *p_err);

void FS_FAT_VolOpen(FS_VOL   *p_vol,
                    FS_FLAGS open_opt,
                    RTOS_ERR *p_err);

void FS_FAT_VolClose(FS_VOL   *p_vol,
                     RTOS_ERR *p_err);

void FS_FAT_VolQuery(FS_VOL      *p_vol,
                     FS_SYS_INFO *p_info,
                     RTOS_ERR    *p_err);

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FS_FAT_VolFmt(FS_BLK_DEV_HANDLE blk_dev_handle,
                   FS_PARTITION_NBR  partition_nbr,
                   FS_LB_NBR         partition_sec_start,
                   FS_LB_QTY         partition_sec_cnt,
                   void              *p_sys_cfg,
                   RTOS_ERR          *p_err);
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
