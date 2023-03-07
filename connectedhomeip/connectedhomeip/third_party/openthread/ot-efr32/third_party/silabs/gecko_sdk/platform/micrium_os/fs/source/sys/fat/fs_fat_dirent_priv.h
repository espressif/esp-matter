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
 *                                                   MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  FS_FAT_DIRENT_PRIV_H_
#define  FS_FAT_DIRENT_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ----------------------- CFG ------------------------
#include  <common/include/rtos_path.h>
#include  <fs_core_cfg.h>

//                                                                 ----------------------- EXT ------------------------
#include  <cpu/include/cpu.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------------ DIR ENTRY SIZE ------------------
#define  FS_FAT_SIZE_DIR_ENTRY_LOG2              5u
#define  FS_FAT_SIZE_DIR_ENTRY                   FS_UTIL_PWR2(FS_FAT_SIZE_DIR_ENTRY_LOG2)

/********************************************************************************************************
 *                                       DIRECTORY ENTRY CONTENTS
 *******************************************************************************************************/

//                                                                 ----------- SPECIAL NAME VAL'S DECODING ------------
#define  FS_FAT_DIRENT_NAME_ERASED_AND_FREE      0xE5u
#define  FS_FAT_DIRENT_NAME_FREE                 0x00u
#define  FS_FAT_DIRENT_NAME_LFN_LAST_LONG_ENTRY  0x40u
#define  FS_FAT_DIRENT_NAME_LFN_SEQ_NO_MASK      0x3Fu

//                                                                 ----------------- ENTRY ATTRIBUTES -----------------
#define  FS_FAT_DIRENT_ATTR_NONE                 DEF_BIT_NONE
#define  FS_FAT_DIRENT_ATTR_READ_ONLY            DEF_BIT_00
#define  FS_FAT_DIRENT_ATTR_HIDDEN               DEF_BIT_01
#define  FS_FAT_DIRENT_ATTR_SYSTEM               DEF_BIT_02
#define  FS_FAT_DIRENT_ATTR_VOLUME_ID            DEF_BIT_03
#define  FS_FAT_DIRENT_ATTR_DIRECTORY            DEF_BIT_04
#define  FS_FAT_DIRENT_ATTR_ARCHIVE              DEF_BIT_05

//                                                                 ----------------- LFN FLAGS / MASK -----------------
#define  FS_FAT_DIRENT_ATTR_LONG_NAME           (FS_FAT_DIRENT_ATTR_READ_ONLY \
                                                 | FS_FAT_DIRENT_ATTR_HIDDEN  \
                                                 | FS_FAT_DIRENT_ATTR_SYSTEM  \
                                                 | FS_FAT_DIRENT_ATTR_VOLUME_ID)

#define  FS_FAT_DIRENT_ATTR_LONG_NAME_MASK      (FS_FAT_DIRENT_ATTR_READ_ONLY   \
                                                 | FS_FAT_DIRENT_ATTR_HIDDEN    \
                                                 | FS_FAT_DIRENT_ATTR_SYSTEM    \
                                                 | FS_FAT_DIRENT_ATTR_VOLUME_ID \
                                                 | FS_FAT_DIRENT_ATTR_DIRECTORY \
                                                 | FS_FAT_DIRENT_ATTR_ARCHIVE)

//                                                                 -------------------- CASE FLAGS --------------------
#define  FS_FAT_DIRENT_NTRES_NAME_LOWER_CASE     DEF_BIT_03
#define  FS_FAT_DIRENT_NTRES_EXT_LOWER_CASE      DEF_BIT_04

/********************************************************************************************************
 *                                   DIRECTORY ENTRY FIELD OFFSETS
 *******************************************************************************************************/

#define  FS_FAT_DIRENT_OFFSET_NAME                   0u
#define  FS_FAT_DIRENT_OFFSET_ATTR                  11u
#define  FS_FAT_DIRENT_OFFSET_NTRES                 12u         // Reserved for Windows NT.
#define  FS_FAT_DIRENT_OFFSET_CRTTIMETENTH          13u         // File creation time in tenths of a second.
#define  FS_FAT_DIRENT_OFFSET_CRTTIME               14u         // Creation time. Granularity is 2 seconds.
#define  FS_FAT_DIRENT_OFFSET_CRTDATE               16u         // Creation date.
#define  FS_FAT_DIRENT_OFFSET_LSTACCDATE            18u         // Last access date.
#define  FS_FAT_DIRENT_OFFSET_FSTCLUSHI             20u         // High word of 1st data cluster nbr for file/dir.
#define  FS_FAT_DIRENT_OFFSET_WRTTIME               22u         // Last modification (write) time.
#define  FS_FAT_DIRENT_OFFSET_WRTDATE               24u         // Last modification (write) date.
#define  FS_FAT_DIRENT_OFFSET_FSTCLUSLO             26u         // Low word of 1st data cluster nbr for file/dir.
#define  FS_FAT_DIRENT_OFFSET_FILESIZE              28u

/********************************************************************************************************
 *                                   DIRECTORY ENTRY FIELD ACCESSORS
 *******************************************************************************************************/

//                                                                 --------------------- SETTER'S ---------------------
#define  FS_FAT_DIRENT_SIZE_SET(p_dte, size) \
  MEM_VAL_SET_INT32U_LITTLE((void *)((CPU_INT08U *)(p_dte) + FS_FAT_DIRENT_OFFSET_FILESIZE), size)

#define  FS_FAT_DIRENT_CREATE_TIME_DATE_SET(p_dte, create_time, create_date)                                   \
  do { MEM_VAL_SET_INT16U_LITTLE((void *)((CPU_INT08U *)(p_dte) + FS_FAT_DIRENT_OFFSET_CRTTIME), create_time); \
       MEM_VAL_SET_INT16U_LITTLE((void *)((CPU_INT08U *)(p_dte) + FS_FAT_DIRENT_OFFSET_CRTDATE), create_date); } while (0)

#define  FS_FAT_DIRENT_WR_TIME_DATE_SET(p_dte, wr_time, wr_date)                                           \
  do { MEM_VAL_SET_INT16U_LITTLE((void *)((CPU_INT08U *)(p_dte) + FS_FAT_DIRENT_OFFSET_WRTTIME), wr_time); \
       MEM_VAL_SET_INT16U_LITTLE((void *)((CPU_INT08U *)(p_dte) + FS_FAT_DIRENT_OFFSET_WRTDATE), wr_date); } while (0)

#define  FS_FAT_DIRENT_ACCESS_DATE_SET(p_dte, access_date) \
  MEM_VAL_SET_INT16U_LITTLE((void *)((CPU_INT08U *)(p_dte) + FS_FAT_DIRENT_OFFSET_LSTACCDATE), access_date)

#define  FS_FAT_DIRENT_ATTRIB_SET(p_dte, attrib) \
  MEM_VAL_SET_INT08U_LITTLE((void *)((CPU_INT08U *)(p_dte) + FS_FAT_DIRENT_OFFSET_ATTR), attrib)

#define  FS_FAT_DIRENT_FIRST_CLUS_SET(p_dte, first_clus_nbr)                                                                               \
  do { MEM_VAL_SET_INT16U_LITTLE((void *)((CPU_INT08U *)(p_dte) + FS_FAT_DIRENT_OFFSET_FSTCLUSHI), first_clus_nbr >> DEF_INT_16_NBR_BITS); \
       MEM_VAL_SET_INT16U_LITTLE((void *)((CPU_INT08U *)(p_dte) + FS_FAT_DIRENT_OFFSET_FSTCLUSLO), first_clus_nbr &  DEF_INT_16_MASK); } while (0)

//                                                                 --------------------- GETTER'S ---------------------
#define  FS_FAT_DIRENT_SIZE_GET(p_dte) \
  MEM_VAL_GET_INT32U_LITTLE((void *)((p_dte) + FS_FAT_DIRENT_OFFSET_FILESIZE))

#define  FS_FAT_DIRENT_ATTRIB_GET(p_dte) \
  MEM_VAL_GET_INT08U_LITTLE((void *)((p_dte) + FS_FAT_DIRENT_OFFSET_ATTR))

#define  FS_FAT_DIRENT_CREATE_DATE_GET(p_dte) \
  MEM_VAL_GET_INT16U_LITTLE((void *)((p_dte) + FS_FAT_DIRENT_OFFSET_CRTDATE))

#define  FS_FAT_DIRENT_CREATE_TIME_GET(p_dte) \
  MEM_VAL_GET_INT16U_LITTLE((void *)((p_dte) + FS_FAT_DIRENT_OFFSET_CRTTIME))

#define  FS_FAT_DIRENT_WR_TIME_GET(p_dte) \
  MEM_VAL_GET_INT16U_LITTLE((void *)((p_dte) + FS_FAT_DIRENT_OFFSET_WRTTIME))

#define  FS_FAT_DIRENT_WR_DATE_GET(p_dte) \
  MEM_VAL_GET_INT16U_LITTLE((void *)((p_dte) + FS_FAT_DIRENT_OFFSET_WRTDATE))

#define  FS_FAT_DIRENT_ACCESS_DATE_GET(p_dte) \
  MEM_VAL_GET_INT16U_LITTLE((void *)((p_dte) + FS_FAT_DIRENT_OFFSET_LSTACCDATE))

#define  FS_FAT_DIRENT_FIRST_CLUS_GET(p_dte)                                                                                   \
  (((CPU_INT32U)(MEM_VAL_GET_INT16U_LITTLE((void *)((CPU_INT08U *)((void *)(p_dte)) + FS_FAT_DIRENT_OFFSET_FSTCLUSHI))) << 16) \
   + ((CPU_INT32U)(MEM_VAL_GET_INT16U_LITTLE((void *)((CPU_INT08U *)((void *)(p_dte)) + FS_FAT_DIRENT_OFFSET_FSTCLUSLO))) <<  0))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------ DIR ENTRY BROWSING OUTCOME ------------
typedef enum fs_fat_dirent_browse_outcome {
  FS_FAT_DIRENT_BROWSE_OUTCOME_CONTINUE,
  FS_FAT_DIRENT_BROWSE_OUTCOME_SKIP,
  FS_FAT_DIRENT_BROWSE_OUTCOME_STOP
} FS_FAT_DIRENT_BROWSE_OUTCOME;

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
